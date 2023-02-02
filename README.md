# sqlite_from_txt


#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <cstring>

void tokenize(std::string const& str, const char* delim,
    std::vector<std::string>& out)
{
    char* token = strtok(const_cast<char*>(str.c_str()), delim);
    while (token != nullptr)
    {
        out.push_back(std::string(token));
        token = strtok(nullptr, delim);
    }
}


using namespace std;

ostringstream tablecode, sqlcode;
string query;

int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    // Return successful
    return 0;
}

int main() {

    char chr;

    ifstream file;
    file.open("test1.txt");

    //ifstream file("test1.txt");

    //// count the newlines with an algorithm specialized for counting:
    //unsigned line_count = std::count(
    //    std::istream_iterator<char>(file),
    //    std::istream_iterator<char>(),
    //    '\n');

    //std::cout << "Lines: " << line_count << "\n";

    string row1 = "";
    vector<string> array_str;

    int no_of_rows = 0;
    int no_of_columns = 0;


    if (file.is_open()) {
        while (getline(file, row1)) {
            array_str.push_back(row1);
        }


        string row_single_string = array_str[1];
        const char* delim = " ";

        vector<std::string> column_array;
        tokenize(row_single_string, delim, column_array);

        no_of_rows = array_str.size();
        no_of_columns = column_array.size();
    }

    cout << "lines: " << no_of_rows << endl;
    cout << "columns: " << no_of_columns << endl;

    

    cout << "**" << endl;



    int **array_2d = new int* [no_of_rows];

    file.clear();
    file.seekg(0, ios::beg);
    for ( int i = 0; i < no_of_rows; ++i) {
        array_2d[i] = new int[no_of_columns];

        for ( int j = 0; j < no_of_columns; ++j) {
            file >> array_2d[i][j];
            cout << array_2d[i][j] << " ";
        }
        std::cout << std::endl;
    }
    
    cin >> chr;

    return 0;
}
