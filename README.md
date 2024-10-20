# TCPClientServer

Проект TCPClientServer представляет собой клиент-серверное приложение, использующее сокеты TCP для передачи данных. Архитектура приложения включает в себя два основных компонента: сервер и клиент, каждый из которых реализует свою собственную логику работы.

## Общая архитектура

1. **Сервер**:
   - Сервер запускается и слушает определенный порт (например, 8888) для входящих подключений от клиентов.
   - Когда клиент подключается, сервер создает новый поток для обработки этого подключения, позволяя обслуживать нескольких клиентов одновременно.
   - Сервер ожидает входящие сообщения от клиента, обрабатывает их и отправляет соответствующий ответ.

2. **Клиент**:
   - Клиент инициирует соединение с сервером, указывая адрес и порт.
   - После установления соединения клиент отправляет сообщение серверу.
   - Клиент ожидает ответ от сервера и выводит его на консоль.

## Логика работы

### Сервер

1. **Инициализация**:
   - Сервер инициализирует библиотеку Winsock.
   - Создает сокет и связывает его с указанным портом.
   - Начинает слушать входящие подключения.

2. **Обработка клиентов**:
   - При получении запроса на соединение сервер принимает его и создает новый поток для обслуживания этого клиента.
   - Поток клиента обрабатывает обмен сообщениями с клиентом, ожидая входящие данные и отправляя ответы.

3. **Обработка сообщений**:
   - Сервер читает данные из сокета, обрабатывает их (например, выводит в консоль) и отправляет ответ обратно клиенту.
   - После завершения обмена сообщениями сервер закрывает соединение.

### Клиент

1. **Инициализация**:
   - Клиент также инициализирует библиотеку Winsock.
   - Создает сокет для подключения к серверу.

2. **Подключение к серверу**:
   - Клиент использует функцию `connect` для установки соединения с сервером, используя указанные адрес и порт.

3. **Отправка и получение сообщений**:
   - После успешного подключения клиент отправляет сообщение на сервер через функцию `send`.
   - Затем клиент ожидает ответ от сервера через функцию `recv` и выводит его на консоль.

4. **Завершение работы**:
   - После завершения обмена сообщениями клиент закрывает сокет и завершает работу.

## Многопоточность

- Для обеспечения одновременной работы с несколькими клиентами сервер использует многопоточность. Каждый новый клиент обрабатывается в отдельном потоке, что позволяет серверу оставаться отзывчивым и обрабатывать несколько запросов одновременно.
- Клиенты также могут быть запущены в отдельных потоках, что позволяет отправлять сообщения одновременно от разных клиентов.

## Пример работы

1. Сервер запущен и слушает на порту 8888.
2. Клиенты подключаются к серверу, отправляя сообщения, например, "Hello Server from client 1".
3. Сервер принимает сообщение, обрабатывает его и отвечает "Message received".
4. Каждый клиент получает ответ и выводит его на экран.
5. После завершения работы клиенты закрывают соединение.

## Заключение

Проект TCPClientServer демонстрирует основные принципы работы с сетевыми приложениями на C++ с использованием сокетов и многопоточности. Он может служить основой для более сложных приложений и расширяться новыми функциями, такими как аутентификация, шифрование данных и т.д.
