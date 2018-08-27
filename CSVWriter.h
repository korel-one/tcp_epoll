#pragma once

#include <string>
#include <vector>

class CSVWriter {
public:
    explicit CSVWriter(int i_id);
    ~CSVWriter();

    void WriteLine(std::string&& line);
    const int GetId() const { return id; }

private:
    void SaveToFile(const std::vector<int>& indices, const std::string& i_file_name) const;
    void SaveSorted() const;

private:
    const int id;
    const std::string file_name;
    FILE* p_file = nullptr;

    //aux members
    std::vector<int> v_n1, v_n3;
    std::vector<float> v_n2;

    int n1, n3;
    float n2;
};
