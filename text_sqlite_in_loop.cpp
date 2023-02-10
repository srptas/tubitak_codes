
// text_sqlite.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <cstring>
#include<sqlite3.h>

#include <numeric>


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

// These are for the sqlite bind SECOND EXAMPLE: 
std::ostringstream tablecode, sqlcode;
std::string query;



// call back funtion for sqlite
int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    // Return successful
    return 0;
}

int main() {

    // CREATING THE DB FILE
    // 
    // Pointer to SQLite connection
    sqlite3* db;


    // Save any error messages
    char* zErrMsg = 0;

    // Save the result 
    int rc;

    // Save any SQL
    string sql;


    // Save the result of opening the file. DB file has opened.
    rc = sqlite3_open("example.db", &db);


    // Check the Sqlite connection. This if statement is for unsuccesful connection.
    if (rc) {
        // Show an error message
        cout << "DB Error: " << sqlite3_errmsg(db) << endl;
        // Close the connection
        sqlite3_close(db);
        // Return an error
        return(1);
    }
    // CREATING THE DB FILE END

    //DATABASE PARAMETERS FOR FASTER PROCESS.
    sqlite3_exec(db, "PRAGMA cache_size = 20000", NULL, NULL, &zErrMsg);  //NUMBER OF PAGES IN THE MEMORY
    sqlite3_exec(db, "PRAGMA synchronous = OFF", NULL, NULL, &zErrMsg);
    sqlite3_exec(db, "PRAGMA journal_mode = MEMORY", NULL, NULL, &zErrMsg);


    char c;

    ifstream file_names;
    file_names.open("filenames.txt");

    string name = "";

    vector<string> file_names_array;
    // Open the file and get lines to array_str named vector
    if (file_names.is_open()) {
        while (getline(file_names, name)) {
            file_names_array.push_back(name);
        }  
    }
   
    for (int i = 0; i < file_names_array.size(); i++) {
        cout << "******** \n" << endl;
        cout << file_names_array[i] << endl;
        cout << "********" << endl;

        ifstream file;
        file.open(file_names_array[i] + ".txt");

        // START CALCULATION OF NUMBER OF ROWS AND COLUMNS
        string line = "";
        vector<string> array_str;

        int no_of_rows = 0;     // Number of rows
        int no_of_columns = 0; // Number of columns


        vector<std::string> column_array;
        vector<std::string> title_array;
        // Open the file and get lines to array_str named vector
        if (file.is_open()) {
            while (getline(file, line)) {
                array_str.push_back(line);
            }


            string row_single_string = array_str[1]; // Get any line from array_str vector to calcuate  # of rows and columns

            const char* delim = "\t"; // delimeter to calculate column number

            tokenize(row_single_string, delim, column_array);
            tokenize(array_str[0], delim, title_array);

            no_of_rows = array_str.size(); // lines number = row number
            no_of_columns = column_array.size(); // column number
        }



        // print the results 
        cout << "lines: " << no_of_rows << endl;
        cout << "columns: " << no_of_columns << endl;

        // END CALCULATION NUMBER OF ROWS AND COLUMNS
        cout << "**" << endl;


        // START FOR MATRIX
        string** array_2d = new string * [no_of_rows];


        file.clear();
        file.seekg(0, ios::beg); // to prevent nonsense numbers

        for (int i = 0; i < no_of_rows; i++) {
            array_2d[i] = new string[no_of_columns];

            for (int j = 0; j < no_of_columns; j++) {
                file >> array_2d[i][j];
                cout << array_2d[i][j] << " ";
            }
            std::cout << std::endl;
        }
        // END FOR MATRIX


        for (auto& str : title_array) {
            str.erase(std::remove(str.begin(), str.end(), '['), str.end());
        }
        for (auto& str : title_array) {
            str.erase(std::remove(str.begin(), str.end(), ']'), str.end());
        }

        string s1;
        string s2;
        string question_marks;
        vector<std::string> column_names;
        vector<std::string> column_names_bind;
        vector<std::string> question_marks_array;
        for (int i = 0; i < title_array.size() - 1; i++) {
            s1 = title_array[i] + " TEXT,";
            column_names.push_back(s1);

            s2 = title_array[i] + ",";
            column_names_bind.push_back(s2);

            question_marks = "?,";
            question_marks_array.push_back(question_marks);

        }
        int no_of_titles = title_array.size();
        column_names.push_back(title_array[no_of_titles - 1] + " TEXT"); // add last element without comma
        column_names_bind.push_back(title_array[no_of_titles - 1]); // add last element without comma
        question_marks_array.push_back("?"); // add last element without comma



        cout << "******" << endl;
        std::string column_names_in_string = std::accumulate(column_names.begin(), column_names.end(), std::string(""));
        cout << column_names_in_string << endl;

        cout << "******" << endl;
        std::string column_names_bind_in_string = std::accumulate(column_names_bind.begin(), column_names_bind.end(), std::string(""));
        cout << column_names_bind_in_string << endl;
        cout << "******" << endl;

        cout << "******" << endl;
        std::string question_marks_in_string = std::accumulate(question_marks_array.begin(), question_marks_array.end(), std::string(""));
        cout << question_marks_in_string << endl;
        cout << "******" << endl;



        tablecode.str("");
        //AUTOINCREMENT ensures the auto increment of primary key 
        tablecode << "CREATE TABLE IF NOT EXISTS " << file_names_array[i] << " (ID INTEGER PRIMARY KEY AUTOINCREMENT," << column_names_in_string << "); ";
        query = tablecode.str();
        rc = sqlite3_exec(db, query.c_str(), callback, 0, &zErrMsg);

        // Check if the table is created
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        else {
            fprintf(stdout, "Table created successfully\n");
        }



        // These are gonna be the parameters of the prepare function to binding
        sqlite3_stmt* res; // res is the prepared statement object
        const char* tail;


        sqlcode.str("");
        sqlcode << "INSERT INTO " << file_names_array[i] <<" (" << column_names_bind_in_string << ") VALUES(" << question_marks_in_string << "); ";
        query = sqlcode.str();
        rc = sqlite3_prepare_v2(db, query.c_str(), query.length(), &res, &tail);



        for (int i = 1; i < no_of_rows; i++) {

            for (int j = 0; j < no_of_columns; j++) {

                sqlite3_bind_text(res, j + 1, array_2d[i][j].c_str(), -1, NULL);
            }

            sqlite3_step(res); // Run the SQL one or more times
            sqlite3_clear_bindings(res); // CLEAR BINDINGS OF WRITE QUERY
            sqlite3_reset(res); // Reset the prepared statement
            
        }

        sqlite3_finalize(res); // Destroy the object
        cout << file_names_array[i] + " ended." << endl;
    } // END OF FOR LOOP

   

    sqlite3_close(db);
    cin >> c;
    return 0;










 
}