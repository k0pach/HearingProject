# Документация к проекту «ИИ-компаньон на StateTree»

## 1. Общее описание

Проект реализует систему ИИ-компаньона с возможностью отдачи приказов в реальном времени в Unreal Engine 5.4.  
Основная технология: **StateTree** + **Enhanced Input** + **AI Perception** + **multicast delegates C++**.

## 2. Руководство пользователя

### Горячие клавиши (по умолчанию)

- **Key 1** — «Ждать на месте» (Wait)
- **Key 2** — «Следовать за мной» (Follow)
- **Key 3** — «Идти в точку» (MoveToPoint) — навести камеру и нажать
- **Key 4** — «Атаковать цель» (Attack) — навести камеру на врага и нажать

### Как играть
1. Запустите уровень.
2. Компаньон автоматически начинает следовать за игроком.
3. Используйте указанные клавиши для отдачи команд.
4. При обнаружении врага компаньон может автоматически перейти в боевой режим (если не получил другую команду).

## 3. Руководство разработчика

### Структура проекта

- `AHearingProjectCharacter` — класс игрока (делегаты команд)
- `ANPC` — физический класс компаньона (Blueprint BP_NPC)
- `ANPC_AIController` — контроллер ИИ + AI Perception
- `USTE_Perception` — основной Evaluator StateTree
- `USTT_GetNPCLocation`, `USTT_DefenseSearchEnemy`, `USTT_AttackSearchEnemy` — кастомные задачи
- `ST_NPC` — дерево состояний StateTree

### Добавление новой команды

1. Создайте новый Input Action в Enhanced Input.
2. Добавьте новый делегат в `AHearingProjectCharacter.h`.
3. Добавьте обработчик в `.cpp` и выполните `Broadcast`.
4. Добавьте обработчик в `USTE_Perception`.
5. Добавьте новый Gameplay Tag.
6. Добавьте новое состояние/переход в StateTree.

### Gameplay Tags (используемые)

- StateTree.Follow
- StateTree.Wait
- StateTree.Combat.Attack
- StateTree.MoveToPointCommand.MoveToPoint
- StateTree.Idle

## 4. Инструкция по настройке игрового уровня

1. Убедитесь, что на уровне есть **NavMesh Bounds Volume**.
2. Сгенерируйте NavMesh (Build → Build Navigation).
3. Разместите врагов класса `AEnemy`.
4. Разместите компаньона класса `BP_NPC`.
5. Установите в `BP_NPC` Anim Class = `ABP_Manny_Combat`.

## 5. Архитектура системы

- Команды от игрока → multicast delegates → USTE_Perception → GameplayTag → StateTree.
- AI Perception → события → USTE_Perception → GameplayTag → StateTree.
- StateTree управляет всеми переходами и задачами.

## 6. Требования

- Unreal Engine 5.4+
- C++ проект
- NavMesh настроена
