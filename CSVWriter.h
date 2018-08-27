#pragma once

#include <string>
#include <vector>


/*-------------------------------------------------------------------------------
class CSVWriter saves received data from the client into './csv/client_{$id}.csv.
Also it performs saving three more csv files, preliminarily sorting the data by
first, second, third column of original csv file.

Members:
	  - id: unique file id
	  - file_name: all the files are in format 'client_{id}'
	  - p_file: file pointer
	  - aux members: are used in order to perform sorting
-------------------------------------------------------------------------------*/
class CSVWriter {
public:
    explicit CSVWriter(int i_id);
    ~CSVWriter();

    // Writes received line to file, caches values to aux containers
    void WriteLine(std::string&& line);

    // Returns file id
    const int GetId() const { return id; }

private:
    /*
       Writes received data w.r.t. specified order
       Args: indices - provide an order to write the data
             i_file_name - write data to this file name
    */
    void SaveToFile(const std::vector<int>& indices, const std::string& i_file_name) const;

    /*
       Generates and sorts the indices that correspond to ascending/descending order
       of all received values from appropriate client. Then it saves the data to the
       supplementary csv files with names ./csv/client_{id}_n{1/2/3}.csv .
    */
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
