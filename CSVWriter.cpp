#include "CSVWriter.h"

#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>

namespace {
    void parse_str(const std::string& str, int& n1, float& n2, int& n3) {
        std::stringstream token_stream(str);
        std::string s;

        std::getline(token_stream, s, ',');
        n1 = std::stoi(s);

        std::getline(token_stream, s, ',');
        n2 = std::stof(s);

        std::getline(token_stream, s, ',');
        n3 = std::stoi(s);
    }

    std::vector<int> create_vec_indices(size_t N) {
        std::vector<int> v(N);
        std::iota(std::begin(v), std::end(v), 0);

        return v;
    }

    template <typename T>
    bool sort_indices(const std::vector<T>& data, std::vector<int>& o_indices, bool less = true) {
        if (data.size() != o_indices.size()) return false;

        std::function<bool(const int, const int)> f1 = [&data](const int i1, const int i2) { return data[i1] < data[i2]; };
        std::function<bool(const int, const int)> f2 = [&data](const int i1, const int i2) { return data[i1] > data[i2]; };

        auto compare = less ? f1 : f2;

        std::sort(std::begin(o_indices), std::end(o_indices), compare);
        return true;
    }
}

CSVWriter::CSVWriter(int i_id) 
    : id(i_id)
    , file_name("client_" + std::to_string(i_id)) 
{}

CSVWriter::~CSVWriter() {
    if (p_file) {
        fclose(p_file);
    }

    SaveSorted();
}

void CSVWriter::WriteLine(std::string&& line) {
    if (!p_file) {
        if (!(p_file = fopen(("./csv/" + file_name + ".csv").c_str(), "a"))) {
            std::cout << "ERROR: file " << file_name << " was not opened\n";
            return;
        }
    }
    fprintf(p_file, "%s\n", line.c_str());

    parse_str(line, n1, n2, n3);
    v_n1.push_back(n1);
    v_n2.push_back(n2);
    v_n3.push_back(n3);
}

void CSVWriter::SaveToFile(const std::vector<int>& indices, const std::string& i_file_name) const {
    FILE* p_f = fopen(i_file_name.c_str(), "w");
    if (!p_f) {
        std::cout << "ERROR: file " << i_file_name << " was not opened\n";
    }

    char buf[128] = { 0 };
    for (auto i : indices) {
        sprintf(buf, "%d,%f,%d", v_n1[i], v_n2[i], v_n3[i]);
        fprintf(p_f, "%s\n", buf);
    }

    fclose(p_f);
}

void CSVWriter::SaveSorted() const {
    auto N = v_n1.size();

    // 1. operate with v_n1
    auto indices1 = create_vec_indices(N);
    sort_indices(v_n1, indices1);
    SaveToFile(indices1, "./csv/" + file_name + "_n1.csv");

    //2. operate with v_n2
    auto indices2 = create_vec_indices(N);
    sort_indices(v_n2, indices2, false);
    SaveToFile(indices2, "./csv/" + file_name + "_n2.csv");

    // 3. operate with v_n3
    auto indices3 = create_vec_indices(N);
    sort_indices(v_n3, indices3);
    SaveToFile(indices3, "./csv/" + file_name + "_n3.csv");
}