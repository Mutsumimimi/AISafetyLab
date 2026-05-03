#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <numeric>
#include <cmath>
#include <iomanip>

// 数据结构：每个 label 汇总的值
struct LabelStats {
    std::vector<double> clean_accs;
    std::vector<double> adv_accs;
    std::vector<double> attack_rates;
};

// 从 CSV 文件读取数据
// 期望列: model, attack, label, clean_accuracy, adv_accuracy, attack_success_rate
std::map<int, LabelStats> readCSV(const std::string& filepath) {
    std::map<int, LabelStats> data;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file: " << filepath << std::endl;
        return data;
    }

    std::string line;
    // 跳过表头
    std::getline(file, line);

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> row;

        while (std::getline(ss, cell, ',')) {
            // 去除首尾空白
            size_t start = cell.find_first_not_of(" \t");
            size_t end = cell.find_last_not_of(" \t");
            if (start == std::string::npos) {
                row.push_back("");
            } else {
                row.push_back(cell.substr(start, end - start + 1));
            }
        }

        if (row.size() < 6) continue;  // 需要至少6列

        int label = std::stoi(row[2]);
        double clean_acc = std::stod(row[3]);
        double adv_acc = std::stod(row[4]);
        double attack_rate = std::stod(row[5]);

        data[label].clean_accs.push_back(clean_acc);
        data[label].adv_accs.push_back(adv_acc);
        data[label].attack_rates.push_back(attack_rate);
    }

    return data;
}

double average(const std::vector<double>& v) {
    if (v.empty()) return 0.0;
    return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
}

double stdev(const std::vector<double>& v, double mean) {
    if (v.size() <= 1) return 0.0;
    double sum = 0.0;
    for (double x : v) {
        sum += (x - mean) * (x - mean);
    }
    return std::sqrt(sum / (v.size() - 1));
}

int main(int argc, char* argv[]) {
    std::string filepath = "test/per_class_results.csv.save";
    if (argc > 1) {
        filepath = argv[1];
    }

    std::cout << "Reading: " << filepath << std::endl;
    auto data = readCSV(filepath);

    if (data.empty()) {
        std::cerr << "No data found. Expected CSV columns:\n";
        std::cerr << "  model,attack,label,clean_accuracy,adv_accuracy,attack_success_rate\n";
        std::cerr << "With label being an integer 0-9.\n";
        return 1;
    }

    // 输出结果表
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "\n| Label | Avg Clean Acc | Avg Adv Acc | Avg Attack Rate | Std Attack |\n";
    std::cout << "|-------|---------------|------------|-----------------|------------|\n";

    for (int label = 0; label <= 9; ++label) {
        if (data.find(label) == data.end()) {
            std::cout << "| " << std::setw(5) << label << " | (no data) |\n";
            continue;
        }

        auto& stats = data[label];
        double avg_clean = average(stats.clean_accs);
        double std_clean = stdev(stats.clean_accs, avg_clean);
        double avg_adv = average(stats.adv_accs);
        double std_adv = stdev(stats.adv_accs, avg_adv);
        double avg_rate = average(stats.attack_rates);
        double std_rate = stdev(stats.attack_rates, avg_rate);
        int n = (int)stats.clean_accs.size();

        std::cout << "| " << std::setw(5) << label
                  << " | " << std::setw(13) << avg_clean
                //   << " | " << std::setw(9) << std_clean
                  << " | " << std::setw(10) << avg_adv
                //   << " | " << std::setw(7) << std_adv
                  << " | " << std::setw(15) << avg_rate
                  << " | " << std::setw(10) << std_rate
                //   << " | " << std::setw(6) << n
                  << " |\n";
    }

    return 0;
}