#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <filesystem>
#include <iomanip>

#include "SortStats.h"
#include "FileGenerator.h"
#include "TwoPhaseSimpleMergeSort.h"
#include "TwoPhaseNaturalMergeSort.h"
#include "OnePhaseSimpleMergeSort.h"
#include "OnePhaseNaturalMergeSort.h"
#include "MultipathMerger.h"
using namespace std;

void print_menu() {

    cout << "МЕНЮ\n";
    cout << "1. Создать новый входной файл\n";
    cout << "2. Выполнить двухфазную сортировку\n";
    cout << "3. Выполнить однофазную сортировку\n";
    cout << "4. Выполнить многопутевое слияние\n";
    cout << "5. Выход\n";
    cout << "Выберите действие: ";
}

void print_two_phase_menu() {
    cout << "\n Двухфазная сортировка \n";
    cout << "1. Простое слияние\n";
    cout << "2. Естественное слияние\n";
    cout << "3. Вернуться в главное меню\n";
    cout << "Выберите метод: ";
}

void print_one_phase_menu() {
    cout << "\n Однофазная сортировка \n";
    cout << "1. Простое слияние\n";
    cout << "2. Естественное слияние\n";
    cout << "3. Вернуться в главное меню\n";
    cout << "Выберите метод: ";
}

void print_multipath_menu() {
    cout << "\n Многопутевое слияние \n";
    cout << "1. Выполнить многопутевое слияние\n";
    cout << "2. Вернуться в главное меню\n";
    cout << "Выберите действие: ";
}

void print_result(const string& sort_name, const SortStats& stats) {
    cout << "\n=== " << sort_name << " ===\n";
    cout << "Сравнений: " << stats.cmp_cnt << "\n";
    cout << "Перемещений: " << stats.move_cnt << "\n";
    cout << "Время: " << fixed << setprecision(2) << (stats.duration_ns / 1000000.0) << " мс"
         << " (" << static_cast<long long>(stats.duration_ns) << " нс)\n";
}

bool file_exists(const string& filename) {
    try {
        return filesystem::exists(filename);
    } catch (const exception&) {
        return false;
    }
}

int get_valid_choice(int min_choice, int max_choice) {
    int choice;
    while (!(cin >> choice) || choice < min_choice || choice > max_choice) {
        if (cin.eof()) {
            cout << "\nВвод закончился. Выход из программы.\n";
            exit(0);
        }
        cout << "Ошибка ввода! Введите число от " << min_choice << " до " << max_choice << ": ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return choice;
}

int main() {
    string filename = "input.txt";
    int choice;
    
    while (true) {
        print_menu();
        choice = get_valid_choice(1, 5);
        
        if (choice == 1) {
            try {
                create_input_file(filename);
            } catch (const exception& e) {
                cerr << "Ошибка при создании файла: " << e.what() << "\n";
            }
        } 
        else if (choice == 2) {
            if (!file_exists(filename)) {
                cout << "Файл не найден! Сначала создайте входной файл!\n";
                continue;
            }
            print_two_phase_menu();
            int method = get_valid_choice(1, 3);
            
            if (method == 1) {
                try {
                    SortStats stats;
                    TwoPhaseSimpleMergeSort<int> tpsms(filename, &stats);
                    print_result("Двухфазная сортировка (простое слияние)", stats);
                } catch (const exception& e) {
                    cerr << "Ошибка при выполнении сортировки: " << e.what() << "\n";
                }
            }
            else if (method == 2) {
                try {
                    SortStats stats;
                    TwoPhaseNaturalMergeSort<int> tphnms(filename, &stats);
                    print_result("Двухфазная сортировка (естественное слияние)", stats);
                } catch (const exception& e) {
                    cerr << "Ошибка при выполнении сортировки: " << e.what() << "\n";
                }
            }
        } 
        else if (choice == 3) {
            if (!file_exists(filename)) {
                cout << "Файл не найден! Сначала создайте входной файл!\n";
                continue;
            }
            print_one_phase_menu();
            int method = get_valid_choice(1, 3);
            
            if (method == 1) {
                try {
                    SortStats stats;
                    OnePhaseSimpleMergeSort<int> opsms(filename, &stats);
                    print_result("Однофазная сортировка (простое слияние)", stats);
                } catch (const exception& e) {
                    cerr << "Ошибка при выполнении сортировки: " << e.what() << "\n";
                }
            }
            else if (method == 2) {
                try {
                    SortStats stats;
                    OnePhaseNaturalMergeSort<int> ophnms(filename, &stats);
                    print_result("Однофазная сортировка (естественное слияние)", stats);
                } catch (const exception& e) {
                    cerr << "Ошибка при выполнении сортировки: " << e.what() << "\n";
                }
            }
        } 
        else if (choice == 4) {
            if (!file_exists(filename)) {
                cout << "Файл не найден! Сначала создайте входной файл!\n";
                continue;
            }
            print_multipath_menu();
            int method = get_valid_choice(1, 2);
            if (method == 1) {
                try {
                    int T;
                    cout << "Введите количество путей (T): ";
                    cin >> T;
                    SortStats stats;
                    MultipathMerger<int> merger(filename, T, &stats);
                    print_result("Многопутевое слияние", stats);
                } catch (const exception& e) {
                    cerr << "Ошибка при выполнении многопутевого слияния: " << e.what() << "\n";
                }
            }
        }
        else if (choice == 5) {
            cout << "Выход из программы.\n";
            break;
        }
    }
    return 0;
}