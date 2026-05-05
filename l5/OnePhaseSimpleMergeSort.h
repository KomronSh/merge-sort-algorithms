#pragma once
#include <fstream>
#include <string>
#include <chrono>
#include "SortStats.h"
using namespace std;

template <typename T>
class OnePhaseSimpleMergeSort
{
public:
    OnePhaseSimpleMergeSort(const string& filename, SortStats* stats) {
        SimpleMergeSort(filename, stats);
    }
private:
    const string kTemp1 = "f1.txt", kTemp2 = "f2.txt", kTemp3 = "f3.txt", kTemp4 = "f4.txt";

    void ClearTempFiles() {
        std::ofstream(kTemp1, std::ios::trunc);
        std::ofstream(kTemp2, std::ios::trunc);
        std::ofstream(kTemp3, std::ios::trunc);
        std::ofstream(kTemp4, std::ios::trunc);
    }
    
    void PrintFile(const string& label, const string& fname) {
        ifstream f(fname);
        cout << label << ": ";
        int x;
        while (f >> x) cout << x << " ";
        cout << "\n";
    }

    void CopyFile(const string& src, const string& dest) {
        std::ifstream in(src);
        std::ofstream out(dest, std::ios::trunc);
        std::string line;
        while (std::getline(in, line)) {
            out << line << "\n";
        }
    }

    void SimpleMergeSort(const string& kMainFile, SortStats* stats) {
        long long cmp_cnt = 0;
        long long move_cnt = 0;
        auto start = chrono::steady_clock::now();
        ClearTempFiles();

        // Count elements in input file
        int n = 0;
        {
            std::ifstream count_file(kMainFile);
            int dummy;
            while (count_file >> dummy) n++;
        }

        // --- Phase 0: Initial Distribution (Source -> f1, f2) ---
        {
            std::ifstream src(kMainFile);
            std::ofstream f1(kTemp1), f2(kTemp2);
            int curr, prev, file_num = 1;
            if (!(src >> curr)) return;

            f1 << curr << " ";
            move_cnt++;
            prev = curr;
            while (src >> curr) {
                cmp_cnt++;
                if (curr < prev) file_num = (file_num == 1) ? 2 : 1;
                (file_num == 1 ? f1 : f2) << curr << " ";
                move_cnt++;
                prev = curr;
            }
        }

        bool sideA = true;  // true: f1,f2 -> f3,f4 | false: f3,f4 -> f1,f2
        int pass = 1;

        while (true) {
            std::string in_name1 = sideA ? kTemp1 : kTemp3;
            std::string in_name2 = sideA ? kTemp2 : kTemp4;
            std::string out_name1 = sideA ? kTemp3 : kTemp1;
            std::string out_name2 = sideA ? kTemp4 : kTemp2;

            std::ifstream in1(in_name1), in2(in_name2);
            std::ofstream out1(out_name1, std::ios::trunc),
                out2(out_name2, std::ios::trunc);

            if (n <= 20) {
                std::cout << "\n--- One-Phase Pass " << ++pass << " ---\n";
                PrintFile("Source 1", in_name1);
                PrintFile("Source 2", in_name2);
            }

            int v1, v2, out_file = 1, total_runs = 0;
            bool h1 = static_cast<bool>(in1 >> v1);
            bool h2 = static_cast<bool>(in2 >> v2);

            while (h1 || h2) {
                std::ofstream& dest = (out_file == 1) ? out1 : out2;
                total_runs++;

                // Track if the current run for each file is still valid
                bool run1_active = h1;
                bool run2_active = h2;

                while (run1_active || run2_active) {
                    bool choose_v1;

                    if (run1_active && run2_active) {
                        cmp_cnt++;
                        choose_v1 = (v1 <= v2);
                    }
                    else {
                        // If only one run is active, we just "drain" it
                        choose_v1 = run1_active;
                    }

                    if (choose_v1) {
                        dest << v1 << " ";
                        move_cnt++;
                        int old = v1;
                        h1 = static_cast<bool>(in1 >> v1);
                        // End run1 if file ends or we hit a step-down
                        if (!h1 || v1 < old) run1_active = false;
                    }
                    else {
                        dest << v2 << " ";
                        move_cnt++;
                        int old = v2;
                        h2 = static_cast<bool>(in2 >> v2);
                        // End run2 if file ends or we hit a step-down
                        if (!h2 || v2 < old) run2_active = false;
                    }
                }

                // Toggle output file for the next merged run
                out_file = (out_file == 1) ? 2 : 1;
            }
            in1.close();
            in2.close();
            out1.close();
            out2.close();
            if (n <= 20) {
                PrintFile("Result 1", out_name1);
                PrintFile("Result 2", out_name2);
            }

            if (total_runs <= 1) {
                CopyFile(out_name1, kMainFile);
                break;
            }
            sideA = !sideA;
        }
        auto end = chrono::steady_clock::now();
        if (stats) {
            stats->cmp_cnt = cmp_cnt;
            stats->move_cnt = move_cnt;
            stats->duration_ns = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
        }
    }
};