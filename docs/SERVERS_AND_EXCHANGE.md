# Серверы Metasiberia и обмен между ними

Документ описывает текущую инфраструктуру после переезда на новый сервер: домены, Caddy reverse proxy/TLS, Metasiberia C++ backend, TheRift Hyperfy, отправку писем и минимальные улучшения аутентификации.
Секреты (пароли/токены/ключи) здесь не храним: см. `C:\programming\AGENTS_SECRETS.local.md`.

## 1) Узлы и роли

## 1.0 Публичные данные доступа (без секретов)
Секреты (пароли/токены) см. `C:\programming\AGENTS_SECRETS.local.md`.

Новый основной сервер:
- SSH login: `denshipilov`
- SSH alias (локально): `metasiberia-server` (в `C:\Users\densh\.ssh\config`)
- LAN IP: `192.168.0.30`
- Public IP: `87.103.196.229`

Metasiberia v2 (старый fallback/архив):
- SSH login: `root`
- SSH alias (локально): `metasiberia-v2` (в `C:\Users\densh\.ssh\config`)
- IP: `185.182.110.184`

TheRift (старый источник/архив):
- SSH login: `root`
- IP: `130.49.151.103`

DNS-панель:
- URL: `https://dnsadmin.hosting.reg.ru/manager/ispmgr`
- Логин: `ce105715135`

REG.RU hosting metasiberia.com (ISPmanager):
- URL: `https://server263.hosting.reg.ru:1500/`
- Логин: `u2978374`

### 1.1 Новый основной Metasiberia server
- LAN IP: `192.168.0.30`
- Public IP: `87.103.196.229`
- Роль: основной Substrata game server + Caddy public TLS/reverse proxy + встроенный C++ webserver backend (сайт/админка/регистрация) + TheRift Hyperfy.
- Публичные production-домены:
  - `https://vr.metasiberia.com/` -> Caddy -> Metasiberia C++ webserver
  - `https://rift.metasiberia.com/` -> Caddy -> TheRift Hyperfy
- Текущие URL по IP:
  - `http://87.103.196.229/` -> 301 на `https://vr.metasiberia.com/`
  - `http://87.103.196.229:3002/` закрыт UFW; backend Hyperfy проверять с сервера через `http://127.0.0.1:3002/`
- Слушаемые порты (факт):
  - `caddy`: `80/tcp`, `443/tcp`
  - `metasiberia-server.service`: `8080/tcp`, `8443/tcp`, `7600/tcp`, `7601/udp`
  - `therift-hyperfy.service`: `3002/tcp` только локально/за UFW
- Вспомогательные сервисы:
  - `metasiberia-bot.service`: `/srv/metasiberia/data/services/metasiberia-bot`, symlink `/opt/metasiberia-bot`, подключается к `127.0.0.1:7600`
  - `metasiberia-map-progress.timer`: каждые 15 минут запускает `/usr/local/bin/metasiberia_map_maintenance.py sample`
  - `metasiberia-map-refresh.timer`: ежедневный low-cost refresh через `/usr/local/bin/metasiberia_map_maintenance.py regen`
  - map maintenance использует `METASIBERIA_BASE_URL=https://127.0.0.1:8443`, так как с самого сервера публичный `https://vr.metasiberia.com` может не открываться через router hairpin/NAT loopback
- Backup:
  - `metasiberia-backup.timer`: ежедневно запускает `/srv/metasiberia/bin/backup_server_state.sh`
  - source: `/home/denshipilov/cyberspace_server_state` -> `/srv/metasiberia/data/state/cyberspace_server_state.candidate-20260621`
  - destination: `/srv/metasiberia/data/backups`
  - retention: 7 дней
  - первый полный архив активного state создан 2026-06-21: `metasiberia-server_cyberspace_server_state_20260621_152746.tar.gz` (~20 GiB)
- Service backup / health:
  - `metasiberia-services-backup.timer`: отдельный ежедневный backup TheRift Hyperfy + Sniper в `/srv/metasiberia/data/backups/services`, retention 14 дней
  - `metasiberia-healthcheck.timer`: каждые 5 минут проверяет systemd-сервисы, локальные HTTP endpoints, backup age, map JSON, UFW, disk usage и SMART; статус: `/srv/metasiberia/data/health/health.json`
  - `metasiberia-restore-check.timer`: еженедельный smoke-check восстановления backup; быстрый service-restore check 2026-06-22 прошёл успешно
  - Windows external pull-backup: `C:\programming\substrata\scripts\metasiberia_backup_pull.ps1`, destination `E:\MetasiberiaBackups`, scheduled task `MetasiberiaBackupPull` в 12:30 локального времени
- Laptop/server settings:
  - `enp2s0f0` статически настроен через netplan на `192.168.0.30/24`, gateway `192.168.0.1`
  - lid close игнорируется; `sleep.target`, `suspend.target`, `hibernate.target`, `hybrid-sleep.target` замаскированы
- Router/NAT: `192.168.0.1`, правило `SubstrataServer` -> `192.168.0.30` для `80/tcp`, `443/tcp`, `7600/tcp`, `7601/udp`. Если `3002/tcp` ещё есть в NAT роутера, серверный UFW его блокирует.
- Caddy config: `/etc/caddy/Caddyfile`
- Server state dir: `/home/denshipilov/cyberspace_server_state` -> `/srv/metasiberia/data/state/cyberspace_server_state.candidate-20260621`
  - Конфиг: `/home/denshipilov/cyberspace_server_state/substrata_server_config.xml`
  - Credentials: `/home/denshipilov/cyberspace_server_state/substrata_server_credentials.txt`
  - В production-конфиге web ports: `web_http_port=8080`, `web_https_port=8443`
  - Публичный TLS выпускает и обновляет Caddy; backend TLS Metasiberia остаётся внутренним за Caddy.

### 1.2 TheRift / Hyperfy / Sniper
- Старый IP TheRift: `130.49.151.103`.
- Hyperfy перенесен на новый основной сервер:
  - путь: `/srv/metasiberia/data/services/therift/hyperfy`
  - symlink: `/var/www/hyperfy`
  - сервис: `therift-hyperfy.service`
  - production URL: `https://rift.metasiberia.com/`
  - backend/diagnostic URL: `http://127.0.0.1:3002/` с самого сервера; внешний `http://87.103.196.229:3002/` закрыт UFW
  - public env: `PUBLIC_WS_URL=wss://rift.metasiberia.com/ws`, `PUBLIC_API_URL=https://rift.metasiberia.com/api`, `PUBLIC_ASSETS_URL=https://rift.metasiberia.com/assets`
- Sniper перенесен на новый основной сервер:
  - путь: `/srv/metasiberia/data/services/therift/sniper-bot`
  - symlink: `/opt/sniper-bot`
  - сервисы: `sniper-bot.service`, `sniper-dryrun.service`
- `metasiberia-walk-bot` и `openclaw` не нужны, не переносить и не запускать.
- Старый TheRift `130.49.151.103` после переноса погашен как runtime: Hyperfy удален из PM2, `pm2-root.service` выключен, `caddy.service` выключен. Старый сервер оставлен только как SSH-архив.
- Снимок старой Hyperfy DB сохранен на новом сервере как архив миграции: `/srv/metasiberia/data/migration-archive/therift-old-20260621/db.sqlite`. Рабочую новую DB им не перетирать без отдельного решения.
- `riftworld.duckdns.org` устарел и не используется; попытка обновления старым найденным token дала `KO`, нужен актуальный DuckDNS token только если решим возвращать DuckDNS.
- Доступ: SSH (пароль в `C:\programming\AGENTS_SECRETS.local.md`).
- Примечание: если SSH ругается на host key mismatch, сначала проверить отпечаток ключа и обновить `known_hosts` осознанно.

### 1.3 Hosting/DNS metasiberia.com (REG.RU)
- Управление DNS/хостингом ведется через REG.RU (см. URL/логины выше).
- Важно: `metasiberia.com` сейчас может быть занят внешним лендингом/прокси; нельзя “просто” перевести A-record корня на `87.103.196.229` без риска сломать лендинг.

## 2) Доменная схема

Требования после cutover:
- Основные пользовательские адреса должны быть доменными.
- TLS на публичных доменах обслуживает Caddy.
- IP-адреса допускаются только для диагностики/legacy.
- Письма (reset password и др.) должны содержать доменную ссылку, а не IP.

### 2.1 Реализованная стратегия (без ломания корня домена)
1. Корень `metasiberia.com` и `www.metasiberia.com` оставлены на REG.RU shared hosting.
2. `vr.metasiberia.com` указывает на новый основной сервер и обслуживает Metasiberia web/admin/game.
3. `rift.metasiberia.com` указывает на новый основной сервер и обслуживает TheRift Hyperfy.
4. Caddy занимает публичные `80/443`, выпускает Let's Encrypt сертификаты и проксирует backend-сервисы.
5. Metasiberia C++ webserver слушает внутренние `8080/8443`, чтобы не конфликтовать с Caddy.

Если корневой домен `metasiberia.com` нужно использовать именно как основной webserver Substrata, это отдельное решение (и, вероятно, потребует переноса/изменения текущего лендинга).

### 2.2 Факт по DNS (обновлено 2026-06-21)
- `metasiberia.com` -> `37.140.192.242` (REG.RU хостинг, лендинг)
- `www.metasiberia.com` -> `37.140.192.242` (REG.RU хостинг)
- `vr.metasiberia.com` -> `87.103.196.229` (новый основной игровой сервер)
- `www.vr.metasiberia.com` -> `87.103.196.229`
- `rift.metasiberia.com` -> `87.103.196.229` (TheRift Hyperfy на новом сервере)
- `riftworld.duckdns.org` -> `95.163.227.206` (устаревшая запись, не использовать как текущую)

### 2.3 Текущее состояние (2026-06-21)
- `https://vr.metasiberia.com/` — основной публичный адрес сервера/сайта.
- `https://rift.metasiberia.com/` — основной публичный адрес TheRift Hyperfy.
- `http://87.103.196.229/` редиректит на `https://vr.metasiberia.com/`.
- Старый Metasiberia v2 `185.182.110.184` остановлен как production (`substrata.service inactive`) и оставлен как fallback/архив.
- TheRift Hyperfy снаружи доступен только через `https://rift.metasiberia.com/`; прямой `http://87.103.196.229:3002/` закрыт UFW.

## 3) TLS (Let's Encrypt) и Caddy

Текущий production TLS обслуживает Caddy. Сертификаты на `vr.metasiberia.com`, `www.vr.metasiberia.com` и `rift.metasiberia.com` автоматически выпущены через Let's Encrypt и хранятся в storage Caddy (`/var/lib/caddy/...`).

В коде Metasiberia также есть поддержка http-01 challenge (файлы вида `/.well-known/acme-challenge/<token>`), но после перехода на Caddy это fallback/legacy-возможность, а не основной production-путь.

### 3.1 Настройка webroot для ACME
В `/root/cyberspace_server_state/substrata_server_config.xml` нужно задать:
- `letsencrypt_webroot_dir` (директория webroot)

Ожидаемый путь файла challenge:
- `<letsencrypt_webroot_dir>/.well-known/acme-challenge/<token>`

Пример (концептуально):
```xml
<config>
  <letsencrypt_webroot_dir>/root/cyberspace_server_state/letsencrypt_webroot</letsencrypt_webroot_dir>
</config>
```

После этого можно использовать `certbot` в режиме webroot (конкретные команды зависят от окружения на сервере).

## 4) Канонический доменный адрес (редирект с IP на домен)

В конфиг добавлен параметр:
- `canonical_web_hostname` (например `vr.metasiberia.com`)

Поведение:
- Если `canonical_web_hostname` задан, сервер будет делать 301 редирект на канонический хост, сохраняя `path` и query-параметры.
- Если параметр пустой, поведение не меняется.

Пример (концептуально):
```xml
<config>
  <canonical_web_hostname>vr.metasiberia.com</canonical_web_hostname>
</config>
```

### 4.1 Факт по текущему TLS на Metasiberia v2 (снимок на 2026-02-15)
В state dir обычно лежат `MyCertificate.crt` и `MyKey.key`. После перехода на Caddy публичный сертификат обслуживает Caddy, а TLS Metasiberia на `8443` считается внутренним backend TLS за reverse proxy.

## 5) Почта: отправка писем и “почтовая аутентификация” домена

### 5.1 Отправка писем из сервера (уже поддержано)
Сервер читает SMTP-настройки из credentials-файла:
- `email_sending_smtp_servername`
- `email_sending_smtp_username`
- `email_sending_smtp_password`
- `email_sending_from_name`
- `email_sending_from_email_addr`
- `email_sending_reset_webserver_hostname` (хост, который вставляется в ссылку reset password)

Важно:
- Значение `email_sending_reset_webserver_hostname` должно соответствовать выбранному доменному адресу (поддомену), иначе ссылки в письмах будут вести на старый хост/IP.
- Секреты (username/password) остаются только в credentials и/или `AGENTS_SECRETS.local.md`, не в репозитории.

### 5.2 DNS-аутентификация почты (SPF/DKIM/DMARC)
Чтобы письма не попадали в спам, для домена нужно настроить:
- SPF (TXT)
- DKIM (обычно CNAME/TXT, зависит от SMTP-провайдера)
- DMARC (TXT)

Конкретные значения берутся у SMTP-провайдера (например Mailgun/SendGrid/другой).  
Этот шаг делается в DNS-панели домена и не требует изменений в коде.

## 6) Аутентификация web: минимальные улучшения безопасности (без ломания)

Серверные сессии используют cookie `site-b`. В коде добавлено:
- `SameSite=Lax`
- `Secure` только при TLS-соединении
- `HttpOnly` сохранено

Это снижает риск CSRF и утечек cookie, при этом не ломает dev HTTP-сценарии.

## 7) Обмен между серверами (текущее состояние и намерение)

Текущее (факт):
- Основная логика мира/аккаунтов/веба живет на новом основном сервере `metasiberia-server` (`192.168.0.30` LAN / `87.103.196.229` public).
- TheRift Hyperfy и Sniper физически перенесены на тот же новый сервер, но остаются отдельными сервисами и каталогами данных.
- Схема обмена/репликации данных между Substrata и Hyperfy/Sniper не зафиксирована в коде и должна быть явно описана перед внедрением (чтобы не сломать совместимость и не получить рассинхрон).

Рекомендуемое правило:
- Пока нет формализованного протокола/репликации, считать сервера независимыми.
- Если понадобится обмен (например, общие аккаунты/SSO/общий каталог worlds), нужно проектировать как отдельный слой с явными контрактами и документацией (и с учетом `SERVER_PROTOCOL.md`).

## 8) Что уже сделано и что держать в голове
1. DNS для `vr.metasiberia.com`, `www.vr.metasiberia.com` и `rift.metasiberia.com` указывает на `87.103.196.229`.
2. Caddy запущен и включен в автозагрузку; `metasiberia-server.service` больше не занимает публичные `80/443`.
3. В production-конфиг Metasiberia добавлены `web_http_port=8080` и `web_https_port=8443`.
4. TheRift Hyperfy `.env` переведен на `https://rift.metasiberia.com/` и `wss://rift.metasiberia.com/ws`.
5. Прямой внешний доступ `:3002` закрыт через UFW; TheRift public flow должен идти через Caddy/443.
6. `webserver_fragments` восстановлены с Metasiberia v2 на новом сервере.
7. `metasiberia-bot.service` перенесен на новый сервер и подключается локально к `127.0.0.1:7600`.
8. `metasiberia-map-progress.timer` и `metasiberia-map-refresh.timer` перенесены на новый сервер и используют локальный backend `https://127.0.0.1:8443`; публичные JSON доступны через встроенный webserver как `/files/map_progress.json` и `/files/map_refresh_status.json`.
9. Старые runtime-сервисы Metasiberia v2 и TheRift выключены; старые серверы оставлены как архивы/источники.

## 9) Данные сайта, пользователи и “база”

### 9.0 Что такое webserver_fragments
`webserver_fragments` — это набор HTML-фрагментов (`*.htmlfrag`), которые встроенный C++ webserver подгружает с диска для некоторых страниц (about/docs/help и т.п.).  
Это не “отдельный сайт”, а часть сервера: страницы собираются из C++ обработчиков + этих шаблонов/фрагментов.

### 9.1 Где лежат данные сайта на основном сервере
Факт по новому серверу `metasiberia-server`:
- Public files (CSS/JS/PNG) используются из: `/home/denshipilov/cyberspace_server_state/webserver_public_files`
- Webclient (wasm/html) используется из: `/home/denshipilov/cyberspace_server_state/webclient`
- HTML fragments восстановлены из старого Metasiberia v2 и лежат в `/var/www/cyberspace/webserver_fragments` (10 файлов). Сейчас `webserver_fragments_dir` в конфиге может оставаться закомментированным, если backend использует дефолтный путь; если нужно удобно редактировать фрагменты рядом со state dir, задавать путь явно в `/home/denshipilov/cyberspace_server_state/substrata_server_config.xml`.

### 9.2 Быстрое редактирование сайта (рекомендуемый workflow)
Рекомендация: исходники web-части держим в git (в этом репозитории), а на сервер выкатываем синхронизацией.
Добавлен скрипт деплоя статики/фрагментов на основной сервер:
- `scripts/deploy_web_to_metasiberia_v2.ps1`

Перед первой выкладкой (чтобы ничего не потерять), можно снять snapshot текущих серверных директорий:
- `scripts/snapshot_web_from_metasiberia_v2.ps1`

Важно:
- На сервере сейчас `webserver_public_files_dir` и `webclient_dir` уже переопределены в `/root/cyberspace_server_state/...` (через `substrata_server_config.xml`).
- `webserver_fragments_dir` пока НЕ переопределен, значит на Linux по умолчанию используется `/var/www/cyberspace/webserver_fragments`.
- На Linux сервер использует inotify-watcher. Поэтому выкладка должна обновлять содержимое директорий *in-place* (без `mv`/rename самой директории), иначе watcher “теряет” обновления. Скрипт деплоя учитывает это (rsync).

### 9.3 Пользователи и “БД” (как сейчас устроено)
Сервер хранит состояние (включая пользователей, парсели, сессии и т.п.) в файле базы:
- `/root/cyberspace_server_state/server_state.bin`

Суперадмин (god user):
- `UserID == 0` (`shared/UserID.h`).
  - Практически: самый первый зарегистрированный пользователь на новом пустом сервере получает id `0` и становится суперадмином.

Web-админка (основные страницы):
- `/admin` (главная)
- `/admin_users` (список пользователей)
- `/admin_user/<id>` (карточка пользователя)

## 10) Figma MCP (для быстрой работы с дизайном)
Figma MCP (Talk To Figma MCP) — это локальный dev-инструмент. Он не “часть прод-сайта”, а связка:
- локальный сокет-сервер на твоем ПК (`ws://localhost:3055`);
- плагин в Figma;
- инструменты в IDE (Cursor/Codex), которые могут читать контекст дизайна.

Скрипт запуска локального сокет-сервера:
- `scripts/start_figma_mcp_socket.ps1`

Данные подключения (file key / channel / порт) см. `C:\programming\AGENTS.md`.

Полезные операции (на сервере):
- Сделать быстрый backup базы перед выкатыванием изменений:
  - `sudo cp -a /home/denshipilov/cyberspace_server_state/server_state.bin /home/denshipilov/cyberspace_server_state/server_state.bin.bak_$(date +%Y%m%d_%H%M%S)`
- Проверить, что сервер жив и слушает порты:
  - `sudo ss -lntup | egrep ':(80|443|7600|8080|8443|3002)\\s'`
  - `sudo ss -lnup | egrep ':7601\\s'`
