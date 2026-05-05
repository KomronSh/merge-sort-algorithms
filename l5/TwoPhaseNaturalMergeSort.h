#pragma once
#include <fstream>
#include <string>
#include <chrono>
#include "SortStats.h"
using namespace std;

template <typename T>
class TwoPhaseNaturalMergeSort
{
private:
    long long cmp_cnt = 0;
    long long move_cnt = 0;

public:
    TwoPhaseNaturalMergeSort(const string& filename, SortStats* stats) {
        two_phase_sort(filename, stats);
    }

private:
    void two_phase_sort(const string& filename, SortStats* stats) {
        auto start = chrono::steady_clock::now();
        int L = 0;

        do {
            L = phase_distribution(filename);
            fusion_phase(filename);
        } while (L != 1);

        auto end = chrono::steady_clock::now();
        if (stats) {
            stats->cmp_cnt = cmp_cnt;
            stats->move_cnt = move_cnt;
            stats->duration_ns = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
        }
    }

    int phase_distribution(const string& filename) {
        ifstream f(filename);
        ofstream f1("f1.txt");
        ofstream f2("f2.txt");
        ofstream cl("console_log.txt");
        if (!f.is_open() || !f1.is_open() || !f2.is_open() || !cl.is_open()) {
            cerr << "Error opening file" << "\n";
            return -1;
        }
        int L = 0;
        cout << "Phase distribution started." << "\n";
        T z1{}, z2{};
        if (!(f >> z1)) {
            cerr << "Input file is empty or invalid" << "\n";
            return -1;
        }
        f1 << z1 << " ";
        cout << "f1<< " << z1 << "\n";
        move_cnt++; // Перемещение z1 в f1

        while (true) {
            if (!(f >> z2)) break;
            while (z1 <= z2) {
                cmp_cnt++; // Сравнение z1 <= z2
                z1 = z2;
                f1 << z2 << " ";
                cout << "f1<< " << z2 << "\n";
                move_cnt++; // Перемещение z2 в f1
                if (!(f >> z2)) break;
            }
            L++;
            cout << "Seria: " << L << "\n";
            if (f.eof()) break;
            z1 = z2;
            f2 << z1 << " ";
            cout << "f2<< " << z1 << "\n";
            move_cnt++; // Перемещение z1 в f2
            if (!(f >> z2)) break;
            while (z1 <= z2) {
                cmp_cnt++; // Сравнение z1 <= z2
                z1 = z2;
                f2 << z2 << " ";
                cout << "f2<< " << z2 << "\n";
                move_cnt++; // Перемещение z2 в f2
                if (!(f >> z2)) break;
            }
            L++;
            cout << "Seria: " << L << "\n";
            if (f.eof()) break;
        }
        cout << "Series count L = " << L << "\n";
        cout << "Phase distribution completed.\n" << "\n";

        f.close();
        f1.close();
        f2.close();
        cl.close();

        return L;
    }

    void fusion_phase(const string filename) {
        ifstream f1("f1.txt");
        ifstream f2("f2.txt");
        ofstream f(filename);
        ofstream cl("console_log.txt", ios::app);
        if (!f.is_open() || !f1.is_open() || !f2.is_open() || !cl.is_open()) {
            cerr << "Error opening file" << "\n";
            return;
        }
        cout << "Fusion phase started." << "\n";
        T z1, z2;
        bool has_z1 = static_cast<bool>(f1 >> z1);
        bool has_z2 = static_cast<bool>(f2 >> z2);
        while (has_z1 && has_z2) {
            cmp_cnt++;
            if (z1 <= z2) {
                f << z1 << " ";
                cout << "f<< " << z1 << "\n";
                move_cnt++;
                has_z1 = static_cast<bool>(f1 >> z1);
            } else {
                f << z2 << " ";
                cout << "f<< " << z2 << "\n";
                move_cnt++;
                has_z2 = static_cast<bool>(f2 >> z2);
            }
        }
        while (has_z1) {
            f << z1 << " ";
            cout << "f<< " << z1 << "\n";
            move_cnt++;
            has_z1 = static_cast<bool>(f1 >> z1);
        }
        while (has_z2) {
            f << z2 << " ";
            cout << "f<< " << z2 << "\n";
            move_cnt++;
            has_z2 = static_cast<bool>(f2 >> z2);
        }
        cout << "Fusion phase completed.\n" << "\n";
        f.close();
        f1.close();
        f2.close();
        cl.close();
    }
};

