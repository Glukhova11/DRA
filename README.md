# DRA
Diameter Routing Agent

A Diameter Routing Agent (DRA) is an application with all functionalities of Diameter Relay Agents along with some additional functionalities such as load balancing, 
routing diameter traffic based on any field, routing Diameter messages to a specific node, divide traffic with various policies. It is a functional element in a 3G or 4G 
(such as LTE) network that provides real-time routing capabilities to ensure that messages are routed among the correct elements in a network.

Project description:
/Source - top-level folder

/Source/ServiceLogic/SLM  - Service Logic Mahager - Менеджер логик. Создает и запускает логики-обработчики примитивов, кол-во логик задается в конфигурационном файле. 
В него попадают приходящие Diameter примитивы(пакеты). Распределяет примитивы между логиками. 
/Source/ServiceLogic/SL - Service Logic - Основной рабочий элемент. Обрабатывает и маршрутизирует в соответствии с правилами поступающие Diameter-примитивы. 
После успешной пересылки запроса, логика меняет свое состояние, уходит в состояние ожидания ответа, далее пересылает ответ клиенту. Логики выполняются параллельно.
/Source/ServiceLogic/SL/CDR_Writer - Запись журнала транзакций.
/Source/ServiceLogic/SessionCounter - Счетчик входящих initial-запросов. При достижении лимита входящих запросов остальные отбивает. 
/Source/ServiceLogic/AlarmMonitor - Модуль инфомирования о происходящих авариях в системе, о загруженности всех запущенных логик. Пишет журнал аварий. 
/Source/ServiceLogic/TrafficManager - Сбор и запись статистики по занятым ресурсам.

/Source/SystemManager - Запуск всех модулей системы и отдельных потоков обработки примитивов при старте.

/Source/Config/DRA_Config - Загрузка общего конфигурационного файла проекта
/Source/Config/Routing - Загрузка конфигурации правил маршрутизации в формате json.

/Source/DataModel - Хранилище информации по обрабатываемым сессиям.

/Source/Primitives - Внутрение примитивы проекта.

/Source/Common/MemoryTimer - Сбор и запись статистики об внутренних очередях примитивов в разных модулях системы.
/Source/Common/DRA_Trace - Модуль записи лог файла проекта.



