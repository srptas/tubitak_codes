// sqlite_tutorial.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

/*

This project is for basic SQLITE examples. 

You can find answers for the following questions:

1- How to compile SQLITE
2- How to create table
3- How to insert data for different data types
4- How to insert data with BIND function
5- How to insert data with BIND function in a for loop

@author Serap Tas

*/


/*
COMPILING SQLITE FOR WINDOWS

1- Project -> Properties 
2- C/C++ -> General -> Additional Include Directories: "path of the sqlite document" 
3- C/C++ -> Code Generation -> Runtime Library : Multi-threaded Debug (/MTd)
4- Linker -> General -> Additional Library Directories: "path of the sqlite3.lib file"
5- Linker -> Input -> Additional Dependencies: "path of the sqlite3.lib file"
6- Linker -> Input -> Ignore Specific Default Libraries: sqlite3;%(IgnoreSpecificDefaultLibraries)

*/

#include <iostream>
#include <sqlite3.h>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

// These are for the sqlite bind SECOND EXAMPLE: 
std::ostringstream tablecode, sqlcode;
std::string query;


// Create a callback function: Callback function provides to execute sql statement for multiple times
// callback function should be used after each sql statement with rc variable otherwise sql statement can not be executed!!
int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    // Return successful
    return 0;
}


int main()
{
    // Pointer to SQLite connection
    sqlite3* db;

    // These are gonna be the parameters of the prepare function to binding
    sqlite3_stmt* res; // res is the prepared statement object
    const char* tail;

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
    
   
    // Save SQL statement to create a table. This sql variable can be used for other sql statements.
    // 
    // DATA TYPES IN SQLITE
    // INT PRIMARY KEY: primary key 
    // TEXT: string
    // INT: int
    // REAL: float
    // 
    sql = "CREATE TABLE IF NOT EXISTS COMPANY ("  \
        "ID INT PRIMARY KEY    NOT NULL," \
        "NAME          TEXT     NOT NULL," \
        "AGE           INT    NOT NULL,"\
        "ADDRESS       TEXT    NOT NULL,"\
        "SALARY        REAL    NOT NULL);";

    // other sql statement
    //sql = "SELECT * from COMPANY";

    // Run the SQL (convert the string to a C-String with c_str() )
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);

    // Check if the table is created
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else {
        fprintf(stdout, "Table created successfully\n");
    }


    // Other sql statement to insert data
    sql = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
        "VALUES (1, 'Paul', 32, 'California', 20000.00 ); " \
        "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  \
        "VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "     \
        "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
        "VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );" \
        "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" \
        "VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );";

    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);



    // INSERT DATA with BIND
    // Bind function ensures to insert variables to the table. It binds the variable with the cell in the Database and 
    // useful when you want to insert variables to the table in a for loop.
    // 
    // 
    // THIS IS THE EXAMPLE FOR BINDING DIFFERENT DATA TYPES
    if (sqlite3_open("example.db", &db) == SQLITE_OK)
    {
        sql = "INSERT INTO COMPANY (ID, NAME, AGE, ADDRESS, SALARY) VALUES (?,?,?,?,?);"; // SQL statement to bind data


        // sqlite3_prepare function is necessary for binding
        
        // PARAMETERS IN PREPARE FUNCTION
        // db: Database handle
        // sql.c_str(): sql statement
        // sql.length(): length of sql
        // &res: statement handle
        // &tail: pointer to unused portion
        rc = sqlite3_prepare(db, sql.c_str(), sql.length(), &res, &tail);

        // Declaring the variables/data
        int id = 5;
        string name = "Serap";
        int age = 21;
        string address = "Istanbul";
        float salary = 2180.00; 

        // BINDING START
        if (rc == SQLITE_OK)
        {
            sqlite3_bind_int(res, 1, id); // second parameter declares the column number
            sqlite3_bind_text(res, 2, name.c_str(), -1, NULL); // the fourth parameter is the byte length. If you pass a negative value to fourth parameter, byte length will be automatically calculated
            sqlite3_bind_int(res, 3, age);
            sqlite3_bind_text(res, 4, address.c_str(), -1, NULL);
            sqlite3_bind_double(res, 5, salary);

           
            //Other steps for the prepared statement object: res

            sqlite3_step(res); // Run the SQL one or more times
            sqlite3_clear_bindings(res); // CLEAR BINDINGS OF WRITE QUERY
            sqlite3_reset(res); // Reset the prepared statement
            sqlite3_finalize(res); // Destroy the object

        }

      
    }



    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    

    //SECOND EXAMPLE FOR BINDING
    int indexx = 1;

    std::vector<double> numbers1;
    std::vector<double> numbers2;

    numbers1.push_back(3);
    numbers1.push_back(102);
    numbers1.push_back(133);
   

    numbers2.push_back(3.123);
    numbers2.push_back(102.487);
    numbers2.push_back(133.879);
    

    tablecode.str("");
    //AUTOINCREMENT ensures the auto increment of primary key 
    tablecode << "CREATE TABLE Leadtime1_" << indexx << " (ID INTEGER PRIMARY KEY AUTOINCREMENT, timeindex INT, gammacost REAL);";
    query = tablecode.str(); // change to string
    rc = sqlite3_exec(db, query.c_str(), callback, 0, &zErrMsg);


    sqlcode.str("");
    sqlcode << "INSERT INTO Leadtime1_" << indexx << " (timeindex, gammacost) VALUES(?,?);";
    query = sqlcode.str();  // change to string
    rc = sqlite3_prepare_v2(db, query.c_str(), query.length(), &res, &tail);

    for (int i = 0; i < numbers1.size(); i++) {

        if (rc == SQLITE_OK) {
            sqlite3_bind_int(res, 1, numbers1[i]);
            sqlite3_bind_double(res, 2, numbers2[i]);


            sqlite3_step(res); // Run the SQL one or more times
            sqlite3_clear_bindings(res); // CLEAR BINDINGS OF WRITE QUERY
            sqlite3_reset(res); // Reset the prepared statement
            //sqlite3_finalize(res); // Don't Destroy the object in a for loop otherwise for loop can not execute following steps in the loop
        }
        
    }

    sqlite3_finalize(res); // Destroy prepared statement object

    // Close the SQL connection
    sqlite3_close(db);

    
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
