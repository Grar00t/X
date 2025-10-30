import express from 'express';
import cors from 'cors';
import helmet from 'helmet';
import rateLimit from 'express-rate-limit';
import jwt from 'jsonwebtoken';
import dotenv from 'dotenv';
import fs from 'fs';
import path from 'path';
import multer from 'multer';
dotenv.config();

const app = express();
app.use(express.json({ limit: '1mb' }));
app.use(helmet());

// CORS allowlist
const allowlist = (process.env.CORS_ORIGINS || '').split(',').map(s => s.trim()).filter(Boolean);
const corsOptions = {
  origin: function (origin, callback) {
    if (!origin || allowlist.length === 0 || allowlist.includes(origin)) return callback(null, true);
    return callback(new Error('Not allowed by CORS'));
  },
  credentials: false,
};
app.use(cors(corsOptions));

// Rate limiting
const limiter = rateLimit({
  windowMs: parseInt(process.env.RATE_LIMIT_WINDOW_MS || '60000', 10),
  max: parseInt(process.env.RATE_LIMIT_MAX || '300', 10),
  standardHeaders: true,
  legacyHeaders: false,
});
app.use(limiter);

// Configure upload directory
const uploadDir = process.env.UPLOAD_DIR || './uploads';
if (!fs.existsSync(uploadDir)) {
  fs.mkdirSync(uploadDir, { recursive: true });
}

// Configure multer for file uploads
const storage = multer.diskStorage({
  destination: function (req, file, cb) {
    cb(null, uploadDir);
  },
  filename: function (req, file, cb) {
    const uniqueSuffix = Date.now() + '-' + Math.round(Math.random() * 1E9);
    cb(null, file.fieldname + '-' + uniqueSuffix + path.extname(file.originalname));
  }
});

const upload = multer({
  storage: storage,
  limits: {
    fileSize: parseInt(process.env.MAX_FILE_SIZE || '524288000', 10) // Default 500MB
  },
  fileFilter: function (req, file, cb) {
    // Block potentially dangerous file types
    const dangerousExtensions = ['.exe', '.bat', '.cmd', '.sh', '.php', '.asp', '.aspx', '.jsp'];
    const ext = path.extname(file.originalname).toLowerCase();
    
    if (dangerousExtensions.includes(ext)) {
      return cb(new Error('File type not allowed'), false);
    }
    cb(null, true);
  }
});

// Liveness
app.get('/health', (_, res) => res.json({ ok: true }));

// Metrics (Prometheus)
let promClient;
if ((process.env.ENABLE_METRICS || 'true') === 'true') {
  const pc = await import('prom-client');
  promClient = pc.default || pc;
  promClient.collectDefaultMetrics();
  app.get('/metrics', async (_, res) => {
    res.set('Content-Type', promClient.register.contentType);
    res.end(await promClient.register.metrics());
  });
}

// JWT protect
function requireJWT(req, res, next) {
  const auth = req.headers.authorization || '';
  const token = auth.startsWith('Bearer ') ? auth.slice(7) : null;
  if (!token) return res.status(401).json({ error: 'missing_token' });
  try {
    const payload = jwt.verify(token, process.env.JWT_SECRET, {
      issuer: process.env.JWT_ISSUER,
      audience: process.env.JWT_AUDIENCE,
    });
    req.user = payload;
    next();
  } catch (e) {
    return res.status(401).json({ error: 'invalid_token' });
  }
}

// Protected sample
app.get('/v1/secure/ping', requireJWT, (req, res) => {
  res.json({ pong: true, sub: req.user.sub });
});

// File upload endpoint
app.post('/v1/upload', requireJWT, upload.single('file'), (req, res) => {
  if (!req.file) {
    return res.status(400).json({ error: 'no_file_uploaded' });
  }
  
  res.json({
    success: true,
    file: {
      originalName: req.file.originalname,
      filename: req.file.filename,
      size: req.file.size,
      mimetype: req.file.mimetype
    }
  });
});

// Multiple files upload endpoint
app.post('/v1/upload/multiple', requireJWT, upload.array('files', 10), (req, res) => {
  if (!req.files || req.files.length === 0) {
    return res.status(400).json({ error: 'no_files_uploaded' });
  }
  
  const files = req.files.map(file => ({
    originalName: file.originalname,
    filename: file.filename,
    size: file.size,
    mimetype: file.mimetype
  }));
  
  res.json({
    success: true,
    count: files.length,
    files: files
  });
});

// Error handling middleware for multer errors
app.use((err, req, res, next) => {
  if (err instanceof multer.MulterError) {
    if (err.code === 'LIMIT_FILE_SIZE') {
      return res.status(413).json({ error: 'file_too_large', maxSize: process.env.MAX_FILE_SIZE || '524288000' });
    }
    if (err.code === 'LIMIT_FILE_COUNT') {
      return res.status(400).json({ error: 'too_many_files', maxFiles: 10 });
    }
    return res.status(400).json({ error: 'upload_error', message: err.message });
  }
  
  if (err.message === 'File type not allowed') {
    return res.status(400).json({ error: 'invalid_file_type', message: 'File type not allowed' });
  }
  
  next(err);
});

// Serve OpenAPI if present
if (fs.existsSync('./openapi.json')) {
  app.get('/openapi.json', (_, res) => res.sendFile(process.cwd() + '/openapi.json'));
}

const HOST = process.env.HOST || '127.0.0.1';
const PORT = parseInt(process.env.PORT || '7070', 10);
app.listen(PORT, HOST, () => {
  console.log(`✅ API running on http://${HOST}:${PORT}`);
});
