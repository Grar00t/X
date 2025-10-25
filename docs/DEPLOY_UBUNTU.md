# Deploy actions-api + Nginx (Ubuntu)

```bash
# Node runtime
sudo apt-get update && sudo apt-get install -y nodejs npm nginx
# app files
sudo mkdir -p /opt/gratech/actions-api
sudo rsync -a ops/actions-api/ /opt/gratech/actions-api/
cd /opt/gratech/actions-api && sudo npm install --omit=dev

# systemd
sudo install -m 0644 ops/systemd/actions-api.service /etc/systemd/system/actions-api.service
sudo systemctl daemon-reload
sudo systemctl enable --now actions-api.service

# nginx
sudo install -m 0644 ops/nginx/actions-api.conf /etc/nginx/sites-available/actions-api.conf
sudo ln -sf /etc/nginx/sites-available/actions-api.conf /etc/nginx/sites-enabled/actions-api.conf
# disable other conflicting sites if needed:
# for f in /etc/nginx/sites-enabled/*; do [[ $(basename "$f") != actions-api.conf ]] && sudo mv "$f" "$f.disabled"; done
sudo nginx -t && sudo systemctl reload nginx

# verify
curl -sS -I https://api.gratech.sa/api/health | head -n 1
curl -sS -I https://api.gratech.sa/openapi.json | grep -i content-type
curl -sS https://api.gratech.sa/openapi.json | jq '.openapi, .servers[0].url'
```
