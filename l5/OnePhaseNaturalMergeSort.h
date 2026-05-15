#pragma once
#include <fstream>
#include <string>
#include <chrono>
#include <iostream>
#include "SortStats.h"
using namespace std;

template <typename T>
class OnePhaseNaturalMergeSort
{
public:
    OnePhaseNaturalMergeSort(const string& filename, SortStats* stats) {
        one_phase_sort(filename, stats);
    }

private:
    struct RunInput {
        ifstream file;
        bool has_value = false;
        T value{};

        explicit RunInput(const string& filename) : file(filename) {}

        bool read_next() {
            has_value = static_cast<bool>(file >> value);
            return has_value;
        }
    };

    int distribute_initial_runs(const string& filename, const string& fout1_name,
                                const string& fout2_name, long long& cmp_cnt,
                                long long& move_cnt) {
        ifstream fin(filename);
        ofstream fout1(fout1_name, ios::trunc);
        ofstream fout2(fout2_name, ios::trunc);

        if (!fin.is_open() || !fout1.is_open() || !fout2.is_open()) {
            cerr << "Error opening file in distribute_initial_runs" << "\n";
            return 0;
        }

        T prev{}, current{};
        if (!(fin >> current)) {
            return 0;
        }

        int runs_count = 1;
        bool to_first = true;
        fout1 << current << " ";
        move_cnt++;
        prev = current;

        while (fin >> current) {
            cmp_cnt++;
            if (current < prev) {
                runs_count++;
                to_first = !to_first;
            }

            (to_first ? fout1 : fout2) << current << " ";
            move_cnt++;
            prev = current;
        }

        return runs_count;
    }

    int merge_natural_runs(const string& fin1_name, const string& fin2_name,
                           const string& fout1_name, const string& fout2_name,
                           long long& cmp_cnt, long long& move_cnt) {
        RunInput fin1(fin1_name);
        RunInput fin2(fin2_name);
        ofstream fout1(fout1_name, ios::trunc);
        ofstream fout2(fout2_name, ios::trunc);

        if (!fin1.file.is_open() || !fin2.file.is_open() || !fout1.is_open() || !fout2.is_open()) {
            cerr << "Error opening file in merge_natural_runs" << "\n";
            return 0;
        }

        fin1.read_next();
        fin2.read_next();

        int runs_count = 0;
        bool to_first = true;

        while (fin1.has_value || fin2.has_value) {
            ofstream& fout = to_first ? fout1 : fout2;
            bool run1_active = fin1.has_value;
            bool run2_active = fin2.has_value;
            runs_count++;

            while (run1_active || run2_active) {
                bool take_first;

                if (run1_active && run2_active) {
                    cmp_cnt++;
                    take_first = fin1.value <= fin2.value;
                }
                else {
                    take_first = run1_active;
                }

                if (take_first) {
                    T old_value = fin1.value;
                    fout << fin1.value << " ";
                    move_cnt++;
                    if (!fin1.read_next()) {
                        run1_active = false;
                    }
                    else {
                        cmp_cnt++;
                        if (fin1.value < old_value) {
                            run1_active = false;
                        }
                    }
                }
                else {
                    T old_value = fin2.value;
                    fout << fin2.value << " ";
                    move_cnt++;
                    if (!fin2.read_next()) {
                        run2_active = false;
                    }
                    else {
                        cmp_cnt++;
                        if (fin2.value < old_value) {
                            run2_active = false;
                        }
                    }
                }
            }

            to_first = !to_first;
        }

        return runs_count;
    }

    void copy_file(const string& src_name, const string& dest_name) {
        ifstream src(src_name);
        ofstream dest(dest_name, ios::trunc);
        if (!src.is_open() || !dest.is_open()) {
            cerr << "Error opening file in copy_file" << "\n";
            return;
        }

        T value;
        while (src >> value) {
            dest << value << " ";
        }
    }

    void one_phase_sort(const string& filename, SortStats* stats) {
        long long cmp_cnt = 0;
        long long move_cnt = 0;
        auto start = chrono::steady_clock::now();

        int runs_count = distribute_initial_runs(filename, "f1.txt", "f2.txt", cmp_cnt, move_cnt);
        bool from_first_pair = true;

        while (runs_count > 1) {
            if (from_first_pair) {
                runs_count = merge_natural_runs("f1.txt", "f2.txt", "f3.txt", "f4.txt", cmp_cnt, move_cnt);
            }
            else {
                runs_count = merge_natural_runs("f3.txt", "f4.txt", "f1.txt", "f2.txt", cmp_cnt, move_cnt);
            }
            from_first_pair = !from_first_pair;
        }

        if (runs_count == 1) {
            copy_file(from_first_pair ? "f1.txt" : "f3.txt", filename);
        }

        auto end = chrono::steady_clock::now();
        if (stats) {
            stats->cmp_cnt = cmp_cnt;
            stats->move_cnt = move_cnt;
            stats->duration_ns = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
        }
    }
};

