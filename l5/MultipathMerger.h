#pragma once
#include <fstream>
#include <string>
#include <chrono>
#include <iostream>
#include "SortStats.h"

using namespace std;
template <typename Data>
class MultipathMerger
{
public:
	MultipathMerger(const string& filename, int T, SortStats* stats) {
        auto start = chrono::high_resolution_clock::now();
        multipath_distributoin(filename, T, stats);
        auto end = chrono::high_resolution_clock::now();
        if (stats) {
            stats->duration_ns = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
        }
	}

	~MultipathMerger() = default;
	
private:
    long long cmp_cnt = 0;
    long long move_cnt = 0;
    int T = 0;
    // Вспомогательные структуры для многопутевого слияния
    vector<vector<Data>> f; // данные по путям
    vector<int> MT, MTA, Dlfile, dl, zt, zp, pos;

    void multipath_distributoin(const string& filename, int T_, SortStats* stats) {
        T = T_;
        ifstream fa(filename);
        if (!fa.is_open()) {
            cerr << "Error opening input file\n";
            return;
        }
        f.assign(2 * T + 2, vector<Data>()); // 1-based индексация, запас для 2T путей
        Data z1, z2;
        int i = 1;
        int L = 0;
        if (fa >> z1) {
            f[i].push_back(z1);
            move_cnt++;
        }
        while (fa >> z2) {
            cmp_cnt++;
            // продолжаем серию
            while (!fa.eof() && z1 <= z2) {
                f[i].push_back(z2);
                move_cnt++;
                z1 = z2;
                fa >> z2;
                cmp_cnt++;
            }
            // серия закончилась
            L++;
            i++;
            int ii = (i > T ? 1 : i);
            if (!fa.eof()) {
                f[ii].push_back(z2);
                move_cnt++;
                z1 = z2;
            }
            if (i > T) i = 1;
        }
        multipath_merge(T, stats, L);
    }

    void multipath_merge(int t, SortStats* stats, int n) {
        T = t;
        int p = 2 * T;
        // Инициализация вспомогательных структур
        MT.assign(p + 2, 0);
        MTA.assign(T + 2, 0);
        Dlfile.assign(2 * T + 2, 0);
        dl.assign(2 * T + 2, 0);
        zt.assign(2 * T + 2, 0);
        zp.assign(2 * T + 2, 0);
        pos.assign(2 * T + 2, 0);
        for (int i = 1; i <= p; i++)
            MT[i] = i;
        for (int i = 1; i <= T; i++) {
            Dlfile[i] = (int)f[i].size();
            Dlfile[i + T] = 0;
        }
        int L = min(n, T);
        bool Up = true;
        while (true) {
            for (int i = 1; i <= T; i++) {
                pos[MT[i]] = 0;
                dl[MT[i]] = 0;
                f[MT[i + T]].clear();
                Dlfile[MT[i + T]] = 0;
            }
            int K1 = min(L, T);
            for (int i = 1; i <= K1; i++) {
                MTA[i] = MT[i];
                zt[MTA[i]] = f[MTA[i]][0];
                pos[MTA[i]] = 1;
                dl[MTA[i]] = 1;
            }
            L = 0;
            int j = T + 1;
            while (true) {
                L++;
                int K2 = K1;
                while (true) {
                    int min_val, mx;
                    find_min(K2, min_val, mx);
                    cmp_cnt += K2;
                    f[MT[j]].push_back(min_val);
                    move_cnt++;
                    Dlfile[MT[j]]++;
                    dl[MTA[mx]]++;
                    if (dl[MTA[mx]] > Dlfile[MTA[mx]]) {
                        MTA[mx] = MTA[K2];
                        MTA[K2] = MTA[K1];
                        K1--;
                        K2--;
                    } else {
                        zp[MTA[mx]] = zt[MTA[mx]];
                        if (pos[MTA[mx]] < (int)f[MTA[mx]].size())
                            zt[MTA[mx]] = f[MTA[mx]][pos[MTA[mx]]];
                        pos[MTA[mx]]++;
                        cmp_cnt++;
                        if (zp[MTA[mx]] > zt[MTA[mx]]) {
                            int tx = MTA[mx];
                            MTA[mx] = MTA[K2];
                            MTA[K2] = tx;
                            K2--;
                        }
                    }
                    if (K2 == 0) break;
                }
                if (j < 2 * T)
                    j++;
                else
                    j = T + 1;
                if (K1 == 0) break;
            }
            for (int i = 1; i <= T; i++) {
                int tx = MT[i];
                MT[i] = MT[i + T];
                MT[i + T] = tx;
            }
            for (int i = 1; i <= T; i++) {
                Dlfile[MT[i]] = (int)f[MT[i]].size();
                dl[MT[i]] = 0;
                pos[MT[i]] = 0;
            }
            Up = !Up;
            if (L == 1) break;
        }
        if (stats) {
            stats->cmp_cnt = cmp_cnt;
            stats->move_cnt = move_cnt;
        }
    }

    void find_min(int K2, int& min, int& mx)
    {
        int k = 1;
        mx = 1;
        min = zt[MTA[1]];

        while (k < K2)
        {
            k = k + 1;
            int x = zt[MTA[k]];
            // сравнение происходит здесь, но учтём их в вызывающей функции
            if (x < min)
            {
                min = x;
                mx = k;
            }
        }
    }
};