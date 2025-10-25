# رفع الملفات الكبيرة - Large File Upload Guide

## العربية

### نظرة عامة
تدعم واجهة GrA Actions API رفع الملفات الكبيرة حتى حجم 500 ميجابايت (قابل للتكوين). يتم استخدام بروتوكول `multipart/form-data` لرفع الملفات بشكل آمن وفعال.

### المتطلبات
- مصادقة JWT صالحة
- حجم الملف لا يتجاوز الحد المسموح (افتراضياً: 500 ميجابايت)

### إعدادات التكوين
يمكن تعديل الإعدادات في ملف `.env`:

```bash
# مسار حفظ الملفات المرفوعة
UPLOAD_DIR=./uploads

# الحد الأقصى لحجم الملف بالبايت (500 ميجابايت)
MAX_FILE_SIZE=524288000
```

### نقاط النهاية (Endpoints)

#### 1. رفع ملف واحد
**الطريقة:** `POST /v1/upload`

**المثال باستخدام cURL:**
```bash
curl -X POST http://127.0.0.1:7070/v1/upload \
  -H "Authorization: Bearer YOUR_JWT_TOKEN" \
  -F "file=@/path/to/your/large-file.zip"
```

**الاستجابة:**
```json
{
  "success": true,
  "file": {
    "originalName": "large-file.zip",
    "filename": "file-1729836000000-123456789.zip",
    "size": 104857600,
    "mimetype": "application/zip",
    "path": "./uploads/file-1729836000000-123456789.zip"
  }
}
```

#### 2. رفع ملفات متعددة
**الطريقة:** `POST /v1/upload/multiple`

**المثال باستخدام cURL:**
```bash
curl -X POST http://127.0.0.1:7070/v1/upload/multiple \
  -H "Authorization: Bearer YOUR_JWT_TOKEN" \
  -F "files=@/path/to/file1.pdf" \
  -F "files=@/path/to/file2.pdf" \
  -F "files=@/path/to/file3.pdf"
```

**الاستجابة:**
```json
{
  "success": true,
  "count": 3,
  "files": [
    {
      "originalName": "file1.pdf",
      "filename": "files-1729836000000-123456789.pdf",
      "size": 2048576,
      "mimetype": "application/pdf"
    }
  ]
}
```

### أمثلة برمجية

#### JavaScript (Node.js)
```javascript
const FormData = require('form-data');
const fs = require('fs');
const fetch = require('node-fetch');

async function uploadLargeFile(filePath, jwtToken) {
  const form = new FormData();
  form.append('file', fs.createReadStream(filePath));
  
  const response = await fetch('http://127.0.0.1:7070/v1/upload', {
    method: 'POST',
    headers: {
      'Authorization': `Bearer ${jwtToken}`
    },
    body: form
  });
  
  return await response.json();
}
```

#### Python
```python
import requests

def upload_large_file(file_path, jwt_token):
    with open(file_path, 'rb') as f:
        files = {'file': f}
        headers = {'Authorization': f'Bearer {jwt_token}'}
        response = requests.post(
            'http://127.0.0.1:7070/v1/upload',
            files=files,
            headers=headers
        )
        return response.json()
```

### الأخطاء الشائعة

| رمز الخطأ | الوصف | الحل |
|----------|-------|------|
| `no_file_uploaded` | لم يتم رفع أي ملف | تأكد من إرسال الملف في حقل `file` |
| `missing_token` | رمز JWT مفقود | أضف رأس المصادقة `Authorization: Bearer TOKEN` |
| `invalid_token` | رمز JWT غير صالح | تأكد من صحة الرمز وأنه لم ينتهي |
| `file_too_large` | الملف كبير جداً | قلّل حجم الملف أو زد `MAX_FILE_SIZE` |
| `invalid_file_type` | نوع الملف غير مسموح | تجنب الملفات القابلة للتنفيذ (.exe, .sh, .bat, إلخ) |
| `too_many_files` | عدد كبير من الملفات | لا يمكن رفع أكثر من 10 ملفات دفعة واحدة |

### الأمان
- جميع نقاط رفع الملفات محمية بمصادقة JWT
- يتم حفظ الملفات في مجلد آمن محدد في `UPLOAD_DIR`
- أسماء الملفات فريدة لتجنب التضارب
- يتم حظر أنواع الملفات الخطرة (.exe, .bat, .sh, .php, إلخ)
- لا يتم إرجاع مسارات الملفات الكاملة في الاستجابة لحماية بنية الخادم

---

## English

### Overview
The GrA Actions API supports uploading large files up to 500MB (configurable). It uses the `multipart/form-data` protocol for secure and efficient file uploads.

### Requirements
- Valid JWT authentication
- File size within the allowed limit (default: 500MB)

### Configuration Settings
Settings can be modified in the `.env` file:

```bash
# Upload directory path
UPLOAD_DIR=./uploads

# Maximum file size in bytes (500MB)
MAX_FILE_SIZE=524288000
```

### Endpoints

#### 1. Upload Single File
**Method:** `POST /v1/upload`

**Example using cURL:**
```bash
curl -X POST http://127.0.0.1:7070/v1/upload \
  -H "Authorization: Bearer YOUR_JWT_TOKEN" \
  -F "file=@/path/to/your/large-file.zip"
```

**Response:**
```json
{
  "success": true,
  "file": {
    "originalName": "large-file.zip",
    "filename": "file-1729836000000-123456789.zip",
    "size": 104857600,
    "mimetype": "application/zip"
  }
}
```

#### 2. Upload Multiple Files
**Method:** `POST /v1/upload/multiple`

**Example using cURL:**
```bash
curl -X POST http://127.0.0.1:7070/v1/upload/multiple \
  -H "Authorization: Bearer YOUR_JWT_TOKEN" \
  -F "files=@/path/to/file1.pdf" \
  -F "files=@/path/to/file2.pdf" \
  -F "files=@/path/to/file3.pdf"
```

### Common Errors

| Error Code | Description | Solution |
|-----------|-------------|----------|
| `no_file_uploaded` | No file was uploaded | Ensure the file is sent in the `file` field |
| `missing_token` | JWT token is missing | Add the `Authorization: Bearer TOKEN` header |
| `invalid_token` | JWT token is invalid | Verify the token is correct and not expired |
| `file_too_large` | File exceeds size limit | Reduce file size or increase `MAX_FILE_SIZE` |
| `invalid_file_type` | File type not allowed | Avoid executable files (.exe, .sh, .bat, etc.) |
| `too_many_files` | Too many files | Cannot upload more than 10 files at once |

### Security
- All upload endpoints are protected with JWT authentication
- Files are saved in a secure directory specified by `UPLOAD_DIR`
- Unique filenames prevent conflicts
- Dangerous file types are blocked (.exe, .bat, .sh, .php, etc.)
- Full file paths are not returned in responses to protect server structure
