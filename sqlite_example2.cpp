// sqlite_example2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//1_outputfile_Leadtime3.txt


//THIS PROJECT IS FOR GETTING DATA FROM TXT FILE TO DB WITH SQLITE

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <cstring>
#include<sqlite3.h>


// tokenize function: to use delimiter for find the number of columns
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

// some declaretion for sqlite
ostringstream tablecode, sqlcode;
string query;

// call back funtion for sqlite
int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    // Return successful
    return 0;
}

int main() {

    char c; // to stop the code

    ifstream file;
    file.open("test.txt");

    // START CALCULATION OF NUMBER OF ROWS AND COLUMNS
    string line = "";
    vector<string> array_str;

    int no_of_rows = 0;     // Number of rows
    int no_of_columns = 0; // Number of columns

    

    // Open the file and get lines to array_str named vector
    if (file.is_open()) {
        while (getline(file, line)) {
            array_str.push_back(line);
            
            
        }

        string row_single_string = array_str[1]; // Get any line from array_str vector to calcuate  # of rows and columns
        const char* delim = "\t"; // delimeter to calculate column number

        vector<std::string> column_array;
        vector<std::string> title_array;
        tokenize(row_single_string, delim, column_array);
        tokenize(array_str[0], delim, title_array);

        no_of_rows = array_str.size(); // lines number = row number
        no_of_columns = column_array.size(); // column number 
    }

    for (int i = 0; i < title_array.size(); i++) {

        
    }
    // print the results 
    cout << "lines: " << no_of_rows << endl;
    cout << "columns: " << no_of_columns << endl;

    // END CALCULATION NUMBER OF ROWS AND COLUMNS
    cout << "**" << endl;



    // START FOR MATRIX
    string **array_2d = new string* [no_of_rows];


    file.clear();
    file.seekg(0, ios::beg);
    for ( int i = 0; i < no_of_rows; ++i) {
        array_2d[i] = new string[no_of_columns];

        for ( int j = 0; j < no_of_columns; ++j) {
            file >> array_2d[i][j];
            cout << array_2d[i][j] << " ";
        }
        std::cout << std::endl;
    }
    
    // END FOR MATRIX
    cin >> c; // ask user a chracter for stop

    return 0;
}







