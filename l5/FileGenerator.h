#pragma once
#include <fstream>
#include <iostream>
using namespace std;

void create_input_file(const string& filename) {
    ofstream f(filename);
    if (!f.is_open()) {
        cerr << "Error creating file" << "\n";
        return;
    }

    int mode;
    cout << "Способ заполнения:\n1 - случайно\n2 - с клавиатуры\n";
    while (!(cin >> mode) || (mode != 1 && mode != 2)) {
        cout << "Ошибка ввода! Введите 1 или 2: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    int size;
    cout << "Введите количество элементов: ";
    while (!(cin >> size) || size <= 0) {
        cout << "Ошибка ввода! Введите положительное число: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    if (mode == 1) {
        srand(time(nullptr));
        cout << "Сгенерированные числа: ";
        for (int i = 0; i < size; i++) {
            int num = rand() % 3200;
            f << num << " ";
            cout << num << " ";
        }
        cout << "\n";
    }
    else if (mode == 2) {
        cout << "Введите " << size << " чисел:\n";
        for (int i = 0; i < size; i++) {
            int num;
            while (!(cin >> num)) {
                cout << "Ошибка ввода! Введите целое число: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            f << num << " ";
        }
    }

    f.close();
    cout << "Файл " << filename << " создан.\n" << "\n";
}
