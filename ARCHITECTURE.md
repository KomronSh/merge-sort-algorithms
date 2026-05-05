# 🏗️ Архитектура проекта

## Общая структура

```
merge-sort-algorithms/
│
├── 📄 README.md                    # Основное описание проекта
├── 📄 GITHUB_UPLOAD_GUIDE.md       # Инструкция по загрузке на GitHub
├── 📄 ARCHITECTURE.md              # Этот файл
├── 📄 .gitignore                   # Файлы, исключаемые из версионирования
│
├── 📂 l5/                          # 🎯 Основной проект (Курсовая работа)
│   ├── 📄 main.cpp                 # Точка входа программы
│   ├── 📄 SortStats.h              # Структура для сбора статистики
│   ├── 📄 FileGenerator.h           # Генератор входных данных
│   ├── 📄 TwoPhaseSimpleMergeSort.h       # Двухфазная сортировка (простое слияние)
│   ├── 📄 TwoPhaseNaturalMergeSort.h      # Двухфазная сортировка (естественное слияние)
│   ├── 📄 OnePhaseSimpleMergeSort.h       # Однофазная сортировка (простое слияние)
│   ├── 📄 OnePhaseNaturalMergeSort.h      # Однофазная сортировка (естественное слияние)
│   ├── 📄 l5.vcxproj               # Visual Studio проект
│   └── 📄 input.txt                # Входной файл (автоматически генерируется)
│
├── 📂 Kirill/                      # 📚 Архивные материалы
│   ├── 📄 Ass5/
│   │   ├── Ass1.cpp
│   │   └── Ass1.vcxproj
│   ├── 📄 SortSimpleTwoPhase.drawio      # Диаграмма двухфазной сортировки
│   ├── 📄 SortNaturalOnePhase.drawio     # Диаграмма однофазной сортировки
│   └── 📄 SortTime.xlsx            # Таблица измерений производительности
│
└── 📄 External sorting.slnx        # Visual Studio Solution
```

## 📦 Компоненты и их взаимодействие

```
┌─────────────────────────────────────────────────────────────┐
│                       main.cpp                              │
│                  (Интерактивное меню)                       │
└────┬────────────────────────────┬────────────────────┬──────┘
     │                            │                    │
     │ вызов                       │ вызов              │ вызов
     ▼                            ▼                    ▼
┌──────────────────┐   ┌──────────────────┐   ┌──────────────────┐
│ FileGenerator    │   │ TwoPhaseSort     │   │ OnePhaseSort     │
│   (создает       │   │  (двухфазная)    │   │  (однофазная)    │
│  случайные       │   │                  │   │                  │
│  входные         │   │ ┌──────────────┐ │   │ ┌──────────────┐ │
│  данные)         │   │ │Simple        │ │   │ │Simple        │ │
│                  │   │ │MergeSort     │ │   │ │MergeSort     │ │
│                  │   │ └──────────────┘ │   │ └──────────────┘ │
│                  │   │ ┌──────────────┐ │   │ ┌──────────────┐ │
│                  │   │ │Natural       │ │   │ │Natural       │ │
│                  │   │ │MergeSort     │ │   │ │MergeSort     │ │
│                  │   │ └──────────────┘ │   │ └──────────────┘ │
└──────────────────┘   └────┬─────────────┘   └────┬─────────────┘
                            │                      │
                            │ собирает             │ собирает
                            ▼                      ▼
                    ┌──────────────────────────────────┐
                    │     SortStats                    │
                    │  (метрики производительности)   │
                    │  - cmp_cnt (сравнения)           │
                    │  - move_cnt (перемещения)        │
                    │  - duration_ns (время в нс)      │
                    └──────────────────────────────────┘
```

## 🔄 Взаимодействие между компонентами

### 1. **main.cpp** - Управление программой
- Точка входа приложения
- Интерактивное меню с 4 основными опциями
- Управление потоком выполнения
- Вывод результатов пользователю

```cpp
while (true) {
    print_menu();
    // 1. Создать файл → FileGenerator::create_input_file()
    // 2. Двухфазная сортировка → TwoPhaseSimpleMergeSort или TwoPhaseNaturalMergeSort
    // 3. Однофазная сортировка → OnePhaseSimpleMergeSort или OnePhaseNaturalMergeSort
    // 4. Выход
}
```

### 2. **FileGenerator.h** - Создание входных данных
```cpp
void create_input_file(const string& filename)
- Запрашивает количество элементов
- Генерирует случайные числа
- Сохраняет в файл в формате "число число число ..."
- Пример: "342 156 789 123 456"
```

### 3. **SortStats.h** - Структура метрик
```cpp
struct SortStats {
    long long cmp_cnt;      // Счетчик сравнений
    long long move_cnt;     // Счетчик перемещений
    double duration_ns;     // Время в наносекундах
};
```

### 4. **Алгоритмы сортировки** - Обработка данных

Каждый алгоритм:
- Принимает имя файла и указатель на SortStats
- Читает данные из файла
- Выполняет сортировку, подсчитывая операции
- Измеряет время выполнения
- Сохраняет отсортированные данные

## 📊 Поток данных

```
USER
  │
  ├─→ [1] Создать файл
  │   └─→ FileGenerator::create_input_file()
  │       └─→ input.txt: "342 156 789 ..."
  │
  ├─→ [2-3] Выбрать алгоритм сортировки
  │   └─→ TwoPhaseSimpleMergeSort / OnePhaseNaturalMergeSort / ...
  │       ├─→ Читает: input.txt
  │       ├─→ Создает: temp1.txt, temp2.txt (для двухфазной)
  │       ├─→ Подсчитывает: cmp_cnt, move_cnt
  │       ├─→ Измеряет: duration_ns
  │       └─→ Выводит: SortStats
  │
  └─→ [4] Выход
```

## 🎯 Ключевые классы и их методы

### TwoPhaseSimpleMergeSort\<T>
```cpp
public:
    TwoPhaseSimpleMergeSort(const string& filename, SortStats* stats)

private:
    void SimpleMergeSort(const string& filename, SortStats* stats)
    // 1. Распределение: input.txt → temp1.txt + temp2.txt
    // 2. Слияние: temp1.txt + temp2.txt → input.txt
    // 3. Повторение с удвоением размера серии
```

### TwoPhaseNaturalMergeSort\<T>
```cpp
public:
    TwoPhaseNaturalMergeSort(const string& filename, SortStats* stats)

private:
    void NaturalMergeSort(const string& filename, SortStats* stats)
    // 1. Определение естественных серий (уже отсортированные последовательности)
    // 2. Распределение серий в две группы
    // 3. Слияние серий с их расширением
```

### OnePhaseSimpleMergeSort\<T>
```cpp
public:
    OnePhaseSimpleMergeSort(const string& filename, SortStats* stats)

private:
    void SimpleMergeSort(const string& filename, SortStats* stats)
    // 1. Чтение серий из файла
    // 2. Слияние прямо в выходной файл
    // 3. Повторение с удвоением размера
```

### OnePhaseNaturalMergeSort\<T>
```cpp
public:
    OnePhaseNaturalMergeSort(const string& filename, SortStats* stats)

private:
    void NaturalMergeSort(const string& filename, SortStats* stats)
    // 1. Обнаружение естественных серий в файле
    // 2. Слияние найденных серий
    // 3. Повторение до полной сортировки
```

## 🔌 Паттерны проектирования

### 1. **Template Pattern** (Шаблонный метод)
Все алгоритмы сортировки используют шаблоны (templates):
```cpp
template <typename T>
class TwoPhaseSimpleMergeSort { ... };
```
- Позволяет использовать алгоритм с разными типами данных
- Текущая реализация: `int`
- Легко расширяется на `long`, `double`, и т.д.

### 2. **Observer Pattern** (Наблюдатель)
Сбор статистики во время сортировки:
```cpp
SortStats* stats;  // Указатель на объект для сбора метрик
stats->cmp_cnt++;  // Увеличение счетчика сравнений
```

### 3. **File I/O Abstraction** (Абстракция для работы с файлами)
- Работа с файлами инкапсулирована в методах
- Легко заменить на работу с памятью или БД

## 🔒 Безопасность и обработка ошибок

```cpp
// Проверка существования файла
if (!file_exists(filename)) {
    cout << "Файл не найден!\n";
    continue;
}

// Обработка исключений при открытии файла
try {
    TwoPhaseSimpleMergeSort<int> sort(filename, &stats);
    print_result("...", stats);
} catch (const exception& e) {
    cerr << "Ошибка: " << e.what() << "\n";
}

// Валидация пользовательского ввода
int choice = get_valid_choice(1, 4);
// Автоматически повторяет запрос при неправильном вводе
```

## 📈 Сложность алгоритмов

| Алгоритм | Временная сложность | Пространственная сложность |
|----------|-------------------|--------------------------|
| TwoPhaseSimple | O(n log n) | O(n) |
| TwoPhaseNatural | O(n log n) | O(n) |
| OnePhaseSimple | O(n log n) | O(1) |
| OnePhaseNatural | O(n log n)* | O(1) |

*Зависит от исходной отсортированности данных

## 🔧 Технические требования

- **C++ Standard**: C++17 (использует `<filesystem>`)
- **Компилятор**: MSVC, GCC, Clang
- **Операционная система**: Windows, Linux, macOS
- **Память**: Минимальные требования (зависит от размера входного файла)
- **Дисковое пространство**: Для временных файлов (особенно для двухфазной сортировки)

## 🚀 Расширение проекта

### Возможные улучшения:
1. ✅ Параллельная сортировка (OpenMP, std::thread)
2. ✅ Сортировка в памяти без файловой системы
3. ✅ Поддержка других типов данных (double, string)
4. ✅ Визуализация процесса сортировки
5. ✅ GUI вместо консоли
6. ✅ Запись результатов в CSV для анализа

## 📚 Используемые библиотеки C++

- `<iostream>` - Ввод/вывод в консоль
- `<fstream>` - Работа с файлами
- `<string>` - Работа со строками
- `<chrono>` - Измерение времени
- `<filesystem>` - Работа с файловой системой (C++17)
- `<iomanip>` - Форматирование вывода
- `<limits>` - Лимиты типов данных
- `<cstdlib>` - Общие функции C
- `<ctime>` - Работа с временем

---

**Диаграммы архитектуры находятся в папке `Kirill/`:**
- `SortSimpleTwoPhase.drawio` - Визуальное представление двухфазной сортировки
- `SortNaturalOnePhase.drawio` - Визуальное представление однофазной сортировки

