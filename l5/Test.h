#pragma once
#include <random>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "SortStats.h"
#include "FileGenerator.h"
#include "TwoPhaseSimpleMergeSort.h"
#include "TwoPhaseNaturalMergeSort.h"
#include "OnePhaseSimpleMergeSort.h"
#include "OnePhaseNaturalMergeSort.h"
#include "MultipathMerger.h"
using namespace std;

bool file_exists(const string& filename) {
    try {
        return filesystem::exists(filename);
    }
    catch (const exception&) {
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

void write_values_to_file(const string& filename, const vector<int>& values) {
    ofstream file(filename, ios::trunc);
    if (!file.is_open()) {
        throw runtime_error("Не удалось открыть файл для записи");
    }

    for (int value : values) {
        file << value << " ";
    }
}

vector<int> generate_test_values(int size) {
    vector<int> values;
    values.reserve(size);

    static mt19937 generator(static_cast<unsigned int>(time(nullptr)));
    uniform_int_distribution<int> distribution(100, 10000);

    for (int i = 0; i < size; i++) {
        values.push_back(distribution(generator));
    }

    return values;
}

void write_benchmark_row(ostream& table, int size, const string& algorithm_name, const SortStats& stats) {
    table << size << "\t"
        << algorithm_name << "\t"
        << stats.cmp_cnt << "\t"
        << stats.move_cnt << "\t"
        << fixed << setprecision(3) << (stats.duration_ns / 1000000.0) << "\t"
        << static_cast<long long>(stats.duration_ns) << "\n";
}

void write_benchmark_header(ostream& table) {
    table << "Размер\tАлгоритм\tСравнения\tПеремещения\tВремя, мс\tВремя, нс\n";
}

void run_benchmark_tests() {
    const string test_filename = "benchmark_input.txt";
    const string table_filename = "benchmark_results.txt";
    const vector<int> sizes = { 100, 500, 1000, 2000, 5000, 10000 };
    const int multipath_t = 4;

    ofstream table(table_filename, ios::trunc);
    if (!table.is_open()) {
        cerr << "Не удалось создать файл таблицы " << table_filename << "\n";
        return;
    }

    write_benchmark_header(cout);
    write_benchmark_header(table);

    streambuf* old_cout = cout.rdbuf();
    ostringstream quiet_output;

    try {
        for (int size : sizes) {
            vector<int> values = generate_test_values(size);
            cout << "Тестирование размера " << size << "...\n";

            write_values_to_file(test_filename, values);
            SortStats two_phase_simple_stats;
            TwoPhaseSimpleMergeSort<int> two_phase_simple(test_filename, &two_phase_simple_stats);
            write_benchmark_row(cout, size, "Двухфазная сортировка простым слиянием", two_phase_simple_stats);
            write_benchmark_row(table, size, "Двухфазная сортировка простым слиянием", two_phase_simple_stats);

            write_values_to_file(test_filename, values);
            SortStats two_phase_natural_stats;
            cout.rdbuf(quiet_output.rdbuf());
            TwoPhaseNaturalMergeSort<int> two_phase_natural(test_filename, &two_phase_natural_stats);
            cout.rdbuf(old_cout);
            quiet_output.str("");
            quiet_output.clear();
            write_benchmark_row(cout, size, "Двухфазная сортировка естественным слиянием", two_phase_natural_stats);
            write_benchmark_row(table, size, "Двухфазная сортировка естественным слиянием", two_phase_natural_stats);

            write_values_to_file(test_filename, values);
            SortStats one_phase_simple_stats;
            OnePhaseSimpleMergeSort<int> one_phase_simple(test_filename, &one_phase_simple_stats);
            write_benchmark_row(cout, size, "Однофазная сортировка простым слиянием", one_phase_simple_stats);
            write_benchmark_row(table, size, "Однофазная сортировка простым слиянием", one_phase_simple_stats);

            write_values_to_file(test_filename, values);
            SortStats one_phase_natural_stats;
            OnePhaseNaturalMergeSort<int> one_phase_natural(test_filename, &one_phase_natural_stats);
            write_benchmark_row(cout, size, "Однофазная сортировка естественным слиянием", one_phase_natural_stats);
            write_benchmark_row(table, size, "Однофазная сортировка естественным слиянием", one_phase_natural_stats);

            write_values_to_file(test_filename, values);
            SortStats multipath_stats;
            MultipathMerger<int> multipath(test_filename, multipath_t, &multipath_stats);
            write_benchmark_row(cout, size, "Многопутевое слияние", multipath_stats);
            write_benchmark_row(table, size, "Многопутевое слияние", multipath_stats);
        }
    }
    catch (const exception& e) {
        cout.rdbuf(old_cout);
        filesystem::remove(test_filename);
        cerr << "Ошибка при тестировании: " << e.what() << "\n";
        return;
    }

    cout.rdbuf(old_cout);
    table.close();
    filesystem::remove(test_filename);
    cout << "Тестирование завершено. Таблица также сохранена в файл " << table_filename << "\n";
}
