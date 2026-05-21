# Plan: «Добавить бота» через UI клиента

> Дата: 2026-05-20  
> Статус: план, реализация не начата

---

## Цель

Добавить в меню `Правка` пункт **«Добавить бота»**.  
При нажатии:
1. В мире перед игроком появляется бот (стандартный аватар).
2. Открывается диалог настроек бота.

---

## Архитектура: как работают боты сейчас

Бот — это **Avatar** с флагом `Avatar::CHATBOT_FLAG`, а не WorldObject.

```
Сервер:
  ChatBot (id, pos, heading, avatar_settings, prompt, animations, llm_thread)
    └── Avatar (uid, pos, rotation, flags=CHATBOT_FLAG, name)

Клиент:
  Avatar с CHATBOT_FLAG отображается как аватар в мире
  Настройка — только через веб-форму /edit_chatbot
```

**Серверные файлы:**
- `server/ChatBot.h` / `ChatBot.cpp` — структура ChatBot
- `server/ServerWorldState.h` — `createAndInsertAvatarForChatBot()`
- `webserver/ChatBotHandlers.cpp` — `handleNewChatBotPost()`, `handleEditChatBotPost()`

**Клиентские файлы (нужно добавить):**
- `gui_client/MainWindow.cpp` — новый пункт меню + triggered()
- `gui_client/BotSettingsDialog.h/cpp` — новый диалог настроек бота

---

## Нужные изменения

### 1. Новый сетевой протокол: CreateBot / UpdateBot

Сейчас клиент создаёт бота только через HTTP веб-форму.  
Нужно добавить бинарные сообщения:

**`shared/Protocol.h`** — добавить новые типы сообщений:
```cpp
CreateChatBotMessage  = ???  // клиент → сервер
UpdateChatBotMessage  = ???  // клиент → сервер  
ChatBotCreatedMessage = ???  // сервер → клиент (подтверждение + id)
```

**`server/WorkerThread.cpp`** — обработать новые сообщения:
```cpp
case Protocol::CreateChatBotMessage: {
    // проверить права (owner/admin)
    // создать ChatBot + Avatar
    // сохранить в БД
    // broadcast ChatBotCreatedMessage
}
```

**`gui_client/ClientThread.cpp`** — отправить сообщение и принять ответ.

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

### 3. Диалог настроек бота — `BotSettingsDialog`

**Новые файлы:** `gui_client/BotSettingsDialog.h`, `gui_client/BotSettingsDialog.cpp`, `gui_client/BotSettingsDialog.ui`

#### Вкладки диалога:

**Вкладка «Аватар»**
- Поле: URL модели (как в AddObjectDialog)
- Кнопка: «Копировать внешность моего аватара»
- Превью аватара (AvatarPreviewGLUIWidget)

**Вкладка «ИИ / Личность»**
- Выбор провайдера: Groq / OpenAI / Локальный LLM
- Поля API: endpoint, API key (хранить в настройках, не в БД)
- Имя бота
- Системный промпт (текстовое поле, многострочное)
- Кнопка «Тест» — отправить тестовое сообщение

**Вкладка «Анимации»**
- Приветствие: URL анимации, имя, cooldown (сек)
- Простой: URL анимации, имя, интервал (сек)
- Реактивный: URL анимации, имя, cooldown (сек)
- Кнопка «Тест анимации»

**Вкладка «Реакции»**
- Реакция на приближение:
  - Дистанция срабатывания (м)
  - Действие: начать разговор / приветственная анимация / оба
- Реакция на жесты:
  - Список жестов → ответное действие бота
- Реакция на удаление:
  - Дистанция прощания (м)
  - Прощальная анимация

**Вкладка «Инструменты (Tools)»**
- Список info_tool_functions (как на веб-форме)
- Кнопки: добавить / удалить / редактировать

**Кнопки диалога:** «Сохранить», «Отмена», «Удалить бота»

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

std::map<std::string, Reference<ChatBotToolFunction>> info_tool_functions;

// Runtime (только сервер)
Reference<LLMThread> llm_thread;
UID avatar_uid;
Reference<Avatar> avatar;
std::map<Reference<Avatar>, OtherAvatarInfo> other_avatar_info;
```

---

## Порядок реализации

1. **Протокол** — `shared/Protocol.h`: добавить `CreateChatBotMessage`, `UpdateChatBotMessage`, `ChatBotCreatedMessage`
2. **Сервер** — `server/WorkerThread.cpp`: обработать CreateChatBot
3. **Клиент** — `gui_client/GUIClient.cpp`: `createBot()`, получение подтверждения
4. **UI меню** — `gui_client/MainWindow.ui` + `MainWindow.cpp`
5. **Диалог** — `BotSettingsDialog.ui/h/cpp`
6. **Сохранение настроек** — UpdateChatBot протокол
7. **Тест** — добавить бота, изменить имя, анимации

---

## Права доступа

Создание бота — только с правом `ObjectCreationPermission` на участке  
(аналогично CreateObject для других объектов).  
Редактирование — только owner или admin.

---

## Связанные файлы

| Файл | Роль |
|------|------|
| `server/ChatBot.h` | Структура бота |
| `server/ChatBot.cpp` | Поведение, реакции |
| `server/LLMThread.h` | LLM коммуникация |
| `server/ServerWorldState.h` | `createAndInsertAvatarForChatBot()` |
| `webserver/ChatBotHandlers.cpp` | Текущая веб-реализация (эталон) |
| `shared/Protocol.h` | Нужно добавить новые типы сообщений |
| `gui_client/MainWindow.cpp` | Нужно добавить меню + обработчик |
| `gui_client/BotSettingsDialog.*` | Нужно создать |
