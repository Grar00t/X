# syntax=docker/dockerfile:1
FROM node:20-alpine
WORKDIR /usr/src/app
ENV NODE_ENV=production
RUN addgroup -S gra && adduser -S gra -G gra
COPY package.json package-lock.json* ./
RUN npm ci --only=production || npm install --production
COPY server.js ./
COPY openapi.json ./openapi.json
USER gra
EXPOSE 7070
HEALTHCHECK --interval=30s --timeout=3s --start-period=10s --retries=3   CMD node -e "fetch('http://127.0.0.1:7070/health').then(r=>process.exit(r.ok?0:1)).catch(()=>process.exit(1))"
CMD ["node","server.js"]
