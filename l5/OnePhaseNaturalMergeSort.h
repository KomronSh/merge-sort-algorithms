#pragma once
#include <fstream>
#include <string>
#include <chrono>
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
    void phase_sort(const string& fin1_name, const string& fin2_name,
                    const string& fout1_name, const string& fout2_name, int p,
                    int& cmp_cnt, int& move_cnt) {
        ifstream fin1(fin1_name);
        ifstream fin2(fin2_name);
        ofstream fout1(fout1_name);
        ofstream fout2(fout2_name);
        if (!fin1.is_open() || !fin2.is_open() || !fout1.is_open() || !fout2.is_open()) {
            cerr << "Error opening file in phase_sort" << "\n";
            return;
        }
        int m1 = 0, m2 = 0;
        T z1, z2;
        bool h1 = static_cast<bool>(fin1 >> z1);
        bool h2 = static_cast<bool>(fin2 >> z2);
        while (h1 || h2) {
            int i = 0, j = 0;
            while (i < p && h1 && j < p && h2) {
                cmp_cnt++;
                if (z1 <= z2) {
                    fout1 << z1 << " ";
                    move_cnt++;
                    h1 = static_cast<bool>(fin1 >> z1);
                    i++;
                } else {
                    fout1 << z2 << " ";
                    move_cnt++;
                    h2 = static_cast<bool>(fin2 >> z2);
                    j++;
                }
            }
            while (i < p && h1) {
                fout1 << z1 << " ";
                move_cnt++;
                h1 = static_cast<bool>(fin1 >> z1);
                i++;
            }
            while (j < p && h2) {
                fout1 << z2 << " ";
                move_cnt++;
                h2 = static_cast<bool>(fin2 >> z2);
                j++;
            }
        }
        fin1.close();
        fin2.close();
        fout1.close();
        fout2.close();
    }

    void one_phase_sort(const string& filename, SortStats* stats) {
        int cmp_cnt = 0;
        int move_cnt = 0;
        auto start = chrono::steady_clock::now();

        ifstream f(filename);
        ofstream f1("f1.txt");
        ofstream f2("f2.txt");
        ofstream cl("console_log.txt");

        if (!f.is_open() || !f1.is_open() || !f2.is_open() || !cl.is_open()) {
            cerr << "Error opening file" << "\n";
            return;
        }

        T z;
        bool to_f1 = true;
        int N = 0;

        while (f >> z) {
            if (to_f1) {
                f1 << z << " ";
            }
            else {
                f2 << z << " ";
            }
            move_cnt++;
            to_f1 = !to_f1;
            N++;
        }

        f1.close();
        f2.close();
        f.close();

        int p = 1;
        bool up = true;

        while (p < N) {
            if (up) {
                phase_sort("f1.txt", "f2.txt", "f3.txt", "f4.txt", p, cmp_cnt, move_cnt);
            }
            else {
                phase_sort("f3.txt", "f4.txt", "f1.txt", "f2.txt", p, cmp_cnt, move_cnt);
            }
            p *= 2;
            up = !up;
        }

        string fin1_name, fin2_name;
        if (!up) {
            fin1_name = "f3.txt";
            fin2_name = "f4.txt";
        }
        else {
            fin1_name = "f1.txt";
            fin2_name = "f2.txt";
        }

        ifstream fin1(fin1_name);
        ifstream fin2(fin2_name);
        ofstream fout(filename);

        if (!fin1.is_open() || !fin2.is_open() || !fout.is_open()) {
            cerr << "Error opening files for merge" << "\n";
            return;
        }

        T z1, z2;
        bool has_z1 = static_cast<bool>(fin1 >> z1);
        bool has_z2 = static_cast<bool>(fin2 >> z2);
        while (has_z1 && has_z2) {
            cmp_cnt++;
            if (z1 <= z2) {
                fout << z1 << " ";
                move_cnt++;
                has_z1 = static_cast<bool>(fin1 >> z1);
            } else {
                fout << z2 << " ";
                move_cnt++;
                has_z2 = static_cast<bool>(fin2 >> z2);
            }
        }
        while (has_z1) {
            fout << z1 << " ";
            move_cnt++;
            has_z1 = static_cast<bool>(fin1 >> z1);
        }
        while (has_z2) {
            fout << z2 << " ";
            move_cnt++;
            has_z2 = static_cast<bool>(fin2 >> z2);
        }

        fin1.close();
        fin2.close();
        fout.close();
        cl.close();

        auto end = chrono::steady_clock::now();
        if (stats) {
            stats->cmp_cnt = cmp_cnt;
            stats->move_cnt = move_cnt;
            stats->duration_ns = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
        }
    }
};

