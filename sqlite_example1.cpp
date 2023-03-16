#include <iostream>
#include <string>
#include <sqlite3.h>
#include <sstream>
#include <stdio.h>

std::ostringstream sqlcode;
std::string query;
using namespace std;

// Create a callback function
int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    // Return successful
    return 0;
}

int main() {

    // Pointer to SQLite connection
    sqlite3* db;

    // For binding
    sqlite3_stmt* res;
    const char* tail;

    // Save any error messages
    char* zErrMsg = 0;

    // Save the result of opening the file
    int rc;

    // Save any SQL
    string sql;

    // Save the result of opening the file. DB file has opened.
    rc = sqlite3_open("example.db", &db);


    // Check the Sqlite connection. This, if statement is for unsuccesful connection
    if (rc) {
        // Show an error message
        cout << "DB Error: " << sqlite3_errmsg(db) << endl;
        // Close the connection
        sqlite3_close(db);
        // Return an error
        return(1);
    }

    // Save SQL to create a table. This sql variable can be used for other sql statement.
    sql = "CREATE TABLE IF NOT EXISTS period ("  \
        "ID INT PRIMARY KEY      NOT NULL," \
        "TIME           INT     NOT NULL," \
        "COST           INT    NOT NULL);";

    //  sql = "SELECT * from period";
   

    // Run the SQL (convert the string to a C-String with c_str() 
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);


    // Check if the table is created
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else {
        fprintf(stdout, "Table created successfully\n");
    }



    //INSERT DATA with BIND
    if (sqlite3_open("example.db", &db) == SQLITE_OK)
    {
        sql = "INSERT INTO period (id, time, cost) VALUES (?, ?, ?);";
        rc = sqlite3_prepare(db, sql.c_str(), sql.length(), &res, &tail);

        int id = 2;
        int time = 5;
        int cost = 25;
        
        int i = 0;

        for (id = 0; id <= 100; id++) {
            if (rc == SQLITE_OK)
            {
                sqlite3_bind_int(res, 1, id);
                sqlite3_bind_int(res, 2, time);
                sqlite3_bind_int(res, 3, cost);

                sqlite3_step(res);
                sqlite3_clear_bindings(res);					//CLEAR BINDINGS OF WRITE QUERY
                sqlite3_reset(res);
                //cout << "id: " << id;
         
            }


        }
        

    }

    // Close the SQL connection
    sqlite3_close(db);

    return 0;
}



statevect[i].optqr = optqr;
statevect[i].optcost = opttotcost;
statevect[i].per = timeindex;
statevect[i].yvect[0] = yvect[0];
statevect[i].yvect[1] = yvect[1];
statevect[i].mvect[0] = mvect[0];
statevect[i].mvect[1] = mvect[1];
statevect[i].optqpvect[0] = optqvect[0];

outputfile << "timeindex" << "\t" << "yvect[0]" << "\t" << "yvect[LT]" << "\t" << "mvect[0]" << "\t" << "mvect[1]" << "\t" << "optqr" << "\t" << "optqvect[0]" << "\t" << "optcost" << "\t" << "gammacost" << endl;
