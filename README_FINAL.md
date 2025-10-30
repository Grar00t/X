# GrA — Final Integrated Bundle

هذا الإصدار يضم:
- خادم actions-api آمن (CORS/JWT/Rate-Limit/Helmet/Metrics)
- **دعم رفع الملفات الكبيرة (حتى 500 ميجابايت)**
- إعدادات .env مثال
- Nginx + systemd مصلّبين
- Dockerfile و docker-compose
- OpenAPI مصغّر
- CI بسيط
- تحسينات CMake (تحذيرات وSanitizers)

## تشغيل سريع
```bash
# داخل المجلد
npm --prefix ops/actions-api ci --omit=dev || (cd ops/actions-api && npm install --production)
node ops/actions-api/server.cjs &
# افحص الصحة
curl -s http://127.0.0.1:7070/health
```

## رفع الملفات الكبيرة
للحصول على تعليمات مفصلة حول كيفية رفع الملفات الكبيرة، راجع:
[دليل رفع الملفات الكبيرة](docs/LARGE_FILE_UPLOAD.md)

## Docker Compose
```bash
docker compose up --build
```

## نشر يدوي
- انسخ systemd/Nginx كما هو وفعّل الخدمة.
- عدّل قيم .env، وفعّل TLS.
