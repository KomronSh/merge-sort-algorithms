#pragma once
#include <fstream>
#include <string>
#include <chrono>
#include "SortStats.h"
using namespace std;

template <typename T>
class TwoPhaseSimpleMergeSort
{
private:
    const string kTemp1 = "temp1.txt";
    const string kTemp2 = "temp2.txt";

public:
    TwoPhaseSimpleMergeSort(const string& filename, SortStats* stats) {
        SimpleMergeSort(filename, stats);
    }
private:
    void SimpleMergeSort(const string& kMainFile, SortStats* stats) {
        long long cmp_cnt = 0;
        long long move_cnt = 0;
        auto start = chrono::steady_clock::now();
        int n{ 0 }, temp;
        {
            std::ifstream f_count(kMainFile);
            while (f_count >> temp) n++;
        }

        for (int size{ 1 }; size < n; size *= 2) {
            // Distribution Phase
            std::ifstream src(kMainFile);
            std::ofstream f1(kTemp1, std::ios::trunc), f2(kTemp2, std::ios::trunc);
            int val, count = 0;
            while (src >> val) {
                if ((count / size) % 2 == 0)
                    f1 << val << " ";
                else
                    f2 << val << " ";
                count++;
                move_cnt++;
            }
            src.close();
            f1.close();
            f2.close();

            // Merge Phase
            std::ifstream in1(kTemp1), in2(kTemp2);
            std::ofstream out(kMainFile, std::ios::trunc);
            int v1, v2;
            bool h1{ static_cast<bool>(in1 >> v1) };
            bool h2{ static_cast<bool>(in2 >> v2) };

            while (h1 || h2) {
                int i{ 0 }, j{ 0 };
                while (i < size && h1 && j < size && h2) {
                    cmp_cnt++;
                    if (v1 <= v2) {
                        out << v1 << " ";
                        h1 = static_cast<bool>(in1 >> v1);
                        i++;
                    }
                    else {
                        out << v2 << " ";
                        h2 = static_cast<bool>(in2 >> v2);
                        j++;
                    }
                    move_cnt++;
                }
                while (i < size && h1) {
                    out << v1 << " ";
                    h1 = static_cast<bool>(in1 >> v1);
                    i++;
                    move_cnt++;
                }
                while (j < size && h2) {
                    out << v2 << " ";
                    h2 = static_cast<bool>(in2 >> v2);
                    j++;
                    move_cnt++;
                }
            }
            out.close();
        }
        auto end = chrono::steady_clock::now();
        if (stats) {
            stats->cmp_cnt = cmp_cnt;
            stats->move_cnt = move_cnt;
            stats->duration_ns = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
        }
    }
};