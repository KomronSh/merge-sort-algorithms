#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

struct SortingStats {
  long long comparisons{0};
  long long writes{0};
  double duration{0};
};

const std::string kMainFile{"data.txt"};
const std::string kBackupFile{"backup.txt"};
const std::string kTemp1{"f1.txt"};
const std::string kTemp2{"f2.txt"};
const std::string kTemp3{"f3.txt"};
const std::string kTemp4{"f4.txt"};

// Helper to clear all temp files before starting
void ClearTempFiles() {
  std::ofstream(kTemp1, std::ios::trunc);
  std::ofstream(kTemp2, std::ios::trunc);
  std::ofstream(kTemp3, std::ios::trunc);
  std::ofstream(kTemp4, std::ios::trunc);
}

void PrintFile(const std::string& label, const std::string& filename) {
  std::ifstream file(filename);
  int val;
  std::cout << std::left << std::setw(15) << label << ": ";
  bool empty{true};
  while (file >> val) {
    std::cout << val << " ";
    empty = false;
  }
  if (empty) std::cout << "[empty]";
  std::cout << std::endl;
}

void CopyFile(const std::string& src_name, const std::string& dest_name) {
  std::ifstream src(src_name);
  std::ofstream dest(dest_name, std::ios::trunc);
  int val;
  while (src >> val) dest << val << " ";
}

void FillRandom(int n) {
  std::ofstream file(kMainFile, std::ios::trunc);
  for (int i{0}; i < n; ++i) file << rand() % 100 << " ";
  file.close();
  CopyFile(kMainFile, kBackupFile);
}

void FillManual(int n) {
  std::ofstream file(kMainFile, std::ios::trunc);
  std::cout << "Enter " << n << " integers: ";
  for (int i = 0; i < n; ++i) {
    int val;
    std::cin >> val;
    file << val << " ";
  }
  file.close();
  CopyFile(kMainFile, kBackupFile);
}
void SimpleMergeSort(SortingStats& st) {
  int n{0}, temp;
  {
    std::ifstream f_count(kMainFile);
    while (f_count >> temp) n++;
  }

  clock_t start = clock();
  for (int size{1}; size < n; size *= 2) {
    if (n <= 20)
      std::cout << "\n--- Simple Merge Pass (Series Size: " << size
                << ") ---\n";

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
      st.writes++;
    }
    src.close();
    f1.close();
    f2.close();

    if (n <= 20) {
      PrintFile("Distribute F1", kTemp1);
      PrintFile("Distribute F2", kTemp2);
    }

    // Merge Phase
    std::ifstream in1(kTemp1), in2(kTemp2);
    std::ofstream out(kMainFile, std::ios::trunc);
    int v1, v2;
    bool h1{static_cast<bool>(in1 >> v1)};
    bool h2{static_cast<bool>(in2 >> v2)};

    while (h1 || h2) {
      int i{0}, j{0};
      while (i < size && h1 && j < size && h2) {
        st.comparisons++;
        if (v1 <= v2) {
          out << v1 << " ";
          h1 = static_cast<bool>(in1 >> v1);
          i++;
        } else {
          out << v2 << " ";
          h2 = static_cast<bool>(in2 >> v2);
          j++;
        }
        st.writes++;
      }
      while (i < size && h1) {
        out << v1 << " ";
        h1 = static_cast<bool>(in1 >> v1);
        i++;
        st.writes++;
      }
      while (j < size && h2) {
        out << v2 << " ";
        h2 = static_cast<bool>(in2 >> v2);
        j++;
        st.writes++;
      }
    }
    out.close();
    if (n <= 20) PrintFile("Merged Result", kMainFile);
  }
  st.duration = static_cast<double>(clock() - start) / CLOCKS_PER_SEC;
}

void NaturalMergeSort(SortingStats& st) {
  int n{0}, temp;
  {
    std::ifstream f_count(kMainFile);
    while (f_count >> temp) n++;
  }

  clock_t start = clock();
  ClearTempFiles();

  // --- Phase 0: Initial Distribution (Source -> f1, f2) ---
  {
    std::ifstream src(kMainFile);
    std::ofstream f1(kTemp1), f2(kTemp2);
    int curr, prev, file_num = 1;
    if (!(src >> curr)) return;

    f1 << curr << " ";
    st.writes++;
    prev = curr;
    while (src >> curr) {
      st.comparisons++;
      if (curr < prev) file_num = (file_num == 1) ? 2 : 1;
      (file_num == 1 ? f1 : f2) << curr << " ";
      st.writes++;
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
          st.comparisons++;
          choose_v1 = (v1 <= v2);
        } else {
          // If only one run is active, we just "drain" it
          choose_v1 = run1_active;
        }

        if (choose_v1) {
          dest << v1 << " ";
          st.writes++;
          int old = v1;
          h1 = static_cast<bool>(in1 >> v1);
          // End run1 if file ends or we hit a step-down
          if (!h1 || v1 < old) run1_active = false;
        } else {
          dest << v2 << " ";
          st.writes++;
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
  st.duration = static_cast<double>(clock() - start) / CLOCKS_PER_SEC;
}

int main() {
  srand(static_cast<unsigned>(time(nullptr)));
  int choice;
  while (true) {
    std::cout << "EXTERNAL MERGE SORT MENU\n";
    std::cout << "1. Generate/Enter Data\n";
    std::cout << "2. Simple Merge Sort (2-Phase)\n";
    std::cout << "3. Natural Merge Sort (1-Phase / 4-Files)\n";
    std::cout << "0. Exit\n";
    std::cout << ">> ";

    if (!(std::cin >> choice)) break;
    if (choice == 0) break;

    if (choice == 1) {
      int n;
      std::cout << "Number of elements: ";
      std::cin >> n;
      std::cout << "1. Randomly Generate\n2. Manually Enter\n>> ";
      int m;
      std::cin >> m;
      if (m == 1)
        FillRandom(n);
      else
        FillManual(n);
      PrintFile("Initial Data", kMainFile);
    } else if (choice == 2 || choice == 3) {
      CopyFile(kBackupFile, kMainFile);
      SortingStats st;

      if (choice == 2) {
        SimpleMergeSort(st);
      } else {
        NaturalMergeSort(st);
      }

      std::cout << "\nFinal Sorted Result:\n";
      PrintFile("Main File", kMainFile);
      std::cout << "\n--- Statistics ---\n";
      std::cout << "Comparisons: " << st.comparisons << "\n";
      std::cout << "Writes:      " << st.writes << "\n";
      std::cout << "Time:        " << std::fixed << std::setprecision(6)
                << st.duration << "s\n";
    }
  }
  return 0;
}