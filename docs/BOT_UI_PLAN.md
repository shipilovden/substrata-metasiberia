# Plan: «Добавить бота» через UI клиента

> Дата: 2026-05-20  
> Статус: базовая UI-реализация выполнена, требуется полевое тестирование в мире

## Обновление на 2026-05-24: полноценный dock-редактор

Рабочий редактор ботов расширен до игровых настроек, которые проходят полный путь `BotEditorWidget -> GUIClient -> Protocol -> WorkerThread -> ChatBot -> UserBotList`.

Добавлено и сохраняется:
- имя, avatar URL и масштаб модели;
- per-bot AI model id, personality preset, private knowledge, temperature и max tokens;
- поведение: disabled, stationary, always face nearest user;
- триггеры: proximity, nearby chat, keyword filter, gesture trigger, Use/E trigger, trigger cooldown;
- анимации/жесты: greeting, idle, reactive, URL `.subanim`, cooldown/interval и кнопки тестового проигрывания через `TestChatBotGesture`;
- звук: audio URL, volume, hearing radius, activation radius, restart cooldown, autoplay, loop, spatial flags.

Серверная логика уже использует:
- `TRIGGER_PROXIMITY_FLAG` для входа/выхода игрока в радиус;
- `TRIGGER_CHAT_FLAG` и `TRIGGER_KEYWORDS_FLAG` для реакции на чат;
- `ALWAYS_FACE_NEAREST_USER_FLAG` для поворота бота к ближайшему пользователю;
- per-bot AI model/temperature/max tokens при создании `LLMThread`.

Ограничение текущего шага: аудио-настройки уже сохраняются и передаются, но для слышимости всем игрокам нужен отдельный runtime-слой трансляции bot-audio к клиентам, потому что бот является `Avatar`, а не `WorldObject`, и существующий аудио-плеер привязан к `WorldObject::audio_source_url`.

---

## Цель

Добавить в меню `Правка` пункт **«Добавить бота»**.  
При нажатии:
1. В мире перед игроком появляется бот (стандартный аватар).
2. Открывается dock-редактор настроек бота.
3. Бот выбирается как avatar-entity, перемещается через gizmo и удаляется без задержки из UI.

---

## Фактический статус на 2026-05-23

Реализовано:
- пункт меню `Правка -> Добавить бота`;
- сетевой протокол `CreateChatBot`, `ChatBotCreated`, `UpdateChatBot`, `DeleteChatBot`, `QueryUserBots`, `UserBotList`, `MoveChatBot`;
- серверная обработка создания, обновления, удаления, перемещения и запроса списка доступных пользователю ботов;
- dock-редактор `BotEditorWidget` со списком ботов, настройками имени, prompt, avatar URL, позиции, анимаций, дистанций реакций и флагом отключения;
- мгновенный локальный feedback при перемещении бота через gizmo, с финальной отправкой `MoveChatBot` на сервер;
- мгновенное удаление выбранного бота по клавише Delete и кнопке в редакторе;
- применение имени и `AvatarSettings` к avatar бота через `other_dirty`, чтобы изменение доходило до клиентов;
- сохранение расширенных дистанций реакций: greeting, farewell и chat radius.

Ограничения текущего этапа:
- провайдер LLM, модель, API endpoint/key и temperature/max tokens пока не вынесены в клиентский бинарный контракт;
- tools/function-calling редактируются серверной/web-частью, но не через новый dock-редактор;
- gesture trigger rules и реакция на конкретные жесты пока не имеют отдельной UI-модели;
- тест анимаций/LLM из редактора пока не реализован как отдельная команда;
- нужна ручная проверка в живом мире: создание, перетаскивание, avatar URL, сохранение после reconnect, удаление и список существующих ботов.

---

## Архитектура: как работают боты сейчас

Бот — это **Avatar** с флагом `Avatar::CHATBOT_FLAG`, а не WorldObject.

```
Сервер:
  ChatBot (id, pos, heading, avatar_settings, prompt, animations, llm_thread)
    └── Avatar (uid, pos, rotation, flags=CHATBOT_FLAG, name)

Клиент:
  Avatar с CHATBOT_FLAG отображается как аватар в мире
  Настройка — через dock-редактор BotEditorWidget и частично через веб-форму /edit_chatbot
```

**Серверные файлы:**
- `server/ChatBot.h` / `ChatBot.cpp` — структура ChatBot
- `server/ServerWorldState.h` — `createAndInsertAvatarForChatBot()`
- `webserver/ChatBotHandlers.cpp` — `handleNewChatBotPost()`, `handleEditChatBotPost()`

**Клиентские файлы:**
- `gui_client/MainWindow.cpp` / `MainWindow.ui` — пункт меню, dock-редактор, callback выбора бота
- `gui_client/GUIClient.cpp` / `GUIClient.h` — клиентский state ботов, выбор avatar, gizmo, create/update/move/delete/query
- `gui_client/BotEditorWidget.h` / `BotEditorWidget.cpp` — основной dock-редактор настроек бота
- `gui_client/BotSettingsDialog.h` / `BotSettingsDialog.cpp` — старый dialog-слой совместимости, не основной UI

---

## Нужные изменения

### 1. Сетевой протокол: Create / Update / Move / Delete / List

Клиент больше не зависит от HTTP веб-формы для базовой работы с ботами.
Используются бинарные сообщения:

**`shared/Protocol.h`** — добавить новые типы сообщений:
```cpp
CreateChatBot  = 1500  // клиент -> сервер
ChatBotCreated = 1501  // сервер -> клиент
UpdateChatBot  = 1502  // клиент -> сервер
DeleteChatBot  = 1503  // клиент -> сервер
QueryUserBots  = 1504  // клиент -> сервер
UserBotList    = 1505  // сервер -> клиент
MoveChatBot    = 1506  // клиент -> сервер
```

**`server/WorkerThread.cpp`** — обработка:
```cpp
case Protocol::CreateChatBot:  // создание ChatBot + Avatar, проверка placement permissions
case Protocol::UpdateChatBot:  // имя, prompt, avatar, анимации, флаги, дистанции
case Protocol::MoveChatBot:    // позиция + heading, проверка placement permissions
case Protocol::DeleteChatBot:  // State_Dead avatar + удаление DB record
case Protocol::QueryUserBots:  // список ботов, доступных для редактирования
```

**`gui_client/ClientThread.cpp`** — читает `ChatBotCreated` и `UserBotList`, включая расширенные поля.

### 2. Меню «Правка» → «Добавить бота»

**`gui_client/MainWindow.ui`** — добавить action в меню Edit/Правка:
```xml
<action name="actionAddBot">
    <property name="text"><string>Добавить бота</string></property>
</action>
```

**`gui_client/MainWindow.cpp`** — подключить и реализовать:
```cpp
void MainWindow::on_actionAddBot_triggered()
{
    // 1. Вычислить позицию перед камерой (как для других объектов)
    const Vec3d cam_pos = gui_client.cam_controller.getFirstPersonPosition();
    const Vec3d cam_forward = gui_client.cam_controller.getForwardsVec();
    const Vec3d bot_pos = cam_pos + cam_forward * 3.0;

    // 2. Создать ChatBot на сервере
    gui_client.createBot(bot_pos, /*heading=*/0.f);

    // 3. Открыть диалог настроек (после получения ChatBotCreated от сервера)
}
```

### 3. Редактор настроек бота — `BotEditorWidget`

**Основные файлы:** `gui_client/BotEditorWidget.h`, `gui_client/BotEditorWidget.cpp`

#### Текущий состав редактора:

**Список ботов**
- список доступных пользователю ботов;
- кнопка обновления списка;
- выбор строки выбирает avatar бота в мире и открывает его настройки.

**Основное**
- имя бота;
- системный prompt;
- флаг `Disabled`;
- позиция и heading.

**Аватар**
- URL модели avatar;
- изменение отправляется через `UpdateChatBot` и применяется к `AvatarSettings` бота.

**Анимации**
- Приветствие: URL анимации, имя, cooldown (сек)
- Простой: URL анимации, имя, интервал (сек)
- Реактивный: URL анимации, имя, cooldown (сек)

**Реакции**
- greeting distance;
- farewell distance;
- chat radius.

**Кнопки редактора**
- сохранение отправляет `UpdateChatBot`;
- удаление сразу вызывает `DeleteChatBot`.

Запланированные расширения редактора:
- provider/model/endpoint/key для LLM;
- temperature, max tokens, system/personality presets;
- memory policy и knowledge base;
- rules для gesture triggers;
- редактор tools/function-calling;
- кнопки теста LLM и теста анимаций.

---

## Структура ChatBot — все поля (из `server/ChatBot.h`)

```cpp
uint64 id;
UserID owner_id;
std::string name;                     // Имя бота (отображается над аватаром)
AvatarSettings avatar_settings;       // Модель + материалы
uint32 flags;                         // DISABLED_FLAG

Vec3d pos;                            // Позиция в мире
float heading;                        // Направление (радианы)
std::string world;                    // Имя мира

std::string custom_prompt_part;       // Системный промпт (характер, инструкции)

// Анимации
std::string greeting_gesture_name;    // Приветствие: имя анимации
URLString   greeting_gesture_URL;
uint32      greeting_gesture_flags;
float       greeting_gesture_cooldown_s;

std::string idle_gesture_name;        // Простой: имя анимации
URLString   idle_gesture_URL;
uint32      idle_gesture_flags;
float       idle_gesture_interval_s;

std::string reactive_gesture_name;    // Реактивный: имя анимации
URLString   reactive_gesture_URL;
uint32      reactive_gesture_flags;
float       reactive_gesture_cooldown_s;

float greeting_distance;              // Дистанция входа в proximity, м
float farewell_distance;              // Дистанция выхода из proximity, м
float chat_radius;                    // Радиус fallback-реакции на чат, м

std::map<std::string, Reference<ChatBotToolFunction>> info_tool_functions;

// Runtime (только сервер)
Reference<LLMThread> llm_thread;
UID avatar_uid;
Reference<Avatar> avatar;
std::map<Reference<Avatar>, OtherAvatarInfo> other_avatar_info;
```

---

## Порядок реализации

1. **Протокол** — `shared/Protocol.h`: выполнено.
2. **Сервер** — `server/WorkerThread.cpp`: выполнено для create/update/move/delete/list.
3. **Клиент** — `gui_client/GUIClient.cpp`: выполнено для create/select/update/move/delete/query.
4. **UI меню** — `gui_client/MainWindow.ui` + `MainWindow.cpp`: выполнено.
5. **Редактор** — `BotEditorWidget.h/cpp`: выполнено для базовых и animation/reaction настроек.
6. **Сохранение настроек** — `UpdateChatBot`: выполнено.
7. **Тест** — требуется ручной сценарий в мире и серверная сборка.

---

## Права доступа

Создание и перемещение бота требуют placement permissions на AABB бота
(аналогично CreateObject для других объектов).
Редактирование и удаление доступны owner, god user или владельцу текущего world.

---

## Связанные файлы

| Файл | Роль |
|------|------|
| `server/ChatBot.h` | Структура бота |
| `server/ChatBot.cpp` | Поведение, реакции |
| `server/LLMThread.h` | LLM коммуникация |
| `server/ServerWorldState.h` | `createAndInsertAvatarForChatBot()` |
| `webserver/ChatBotHandlers.cpp` | Текущая веб-реализация (эталон) |
| `shared/Protocol.h` | Сообщения create/update/move/delete/list для ботов |
| `gui_client/MainWindow.cpp` | Меню, dock, выбор бота |
| `gui_client/GUIClient.cpp` | Клиентская логика ботов, gizmo, update/move/delete/list |
| `gui_client/BotEditorWidget.*` | Основной редактор ботов |
| `gui_client/BotSettingsDialog.*` | Совместимость со старым dialog API |
