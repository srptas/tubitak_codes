# sqlite_from_txt

/*
int main() {

    sqlite3* db;

    sqlite3_stmt* res;
    const char* tail;

    // Save any error messages
    char* zErrMsg = 0;

    // Save the result of opening the file
    int rc;


    // Save the result of opening the file. DB file has opened.
    rc = sqlite3_open("fromtxt.db", &db);


    // Check the Sqlite connection. This, if statement is for unsuccesful connection
    if (rc) {
        // Show an error message
        cout << "DB Error: " << sqlite3_errmsg(db) << endl;
        // Close the connection
        sqlite3_close(db);
        // Return an error
        return(1);
    }


    tablecode.str("");
    tablecode << "CREATE TABLE IF NOT EXISTS period0 (ID INTEGER PRIMARY KEY AUTOINCREMENT, timeindex INT, yvect0 INT, yvect1 INT);";
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

    vector<double> vecX, vecY, vecZ;
    double x, y, z;

    ifstream inputFile("test1.txt");

    char c;
    while (inputFile >> x >> y >> z)
    {
        vecX.push_back(x);
        vecY.push_back(y);
        vecZ.push_back(z);
    }

    for (int i(0); i < vecX.size(); i++) {
        cout << vecX[i] << endl;
        cin >> c;
    }

    sqlcode.str("");
    sqlcode << "INSERT INTO period0 (timeindex, yvect0, yvec1) VALUES(?,?,?);";
    query = sqlcode.str();
    rc = sqlite3_prepare_v2(db, query.c_str(), query.length(), &res, &tail);

    for (int i = 0; i < vecX.size(); i++) {

        if (rc == SQLITE_OK) {
            sqlite3_bind_int(res, 1, vecX[i]);
            sqlite3_bind_int(res, 2, vecY[i]);
            sqlite3_bind_int(res, 3, vecZ[i]);
            


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
*/




/*
int main() {

    char ch;
   
    vector<double> col0, col1, col2, col3, col4, col5, col6, col7, col8, col9, col10 ;
    double a, b, c, d, e, f, g, h, i, j, k;

    ifstream inputFile("1_outputfile_Leadtime3.txt");

    while (inputFile >> a >> b >> c >> d >> e >> f >> g >> h >> i >> j >> k)
    {
        col0.push_back(a);
        col1.push_back(b);
        col2.push_back(c);
        col3.push_back(d);
        col4.push_back(e);
        col5.push_back(f);
        col6.push_back(g);
        col7.push_back(h);
        col8.push_back(i);
        col9.push_back(j);
        col10.push_back(k);
         
        cout << "Inserting from txt file to vector..." << endl;  
    }
    cin >> ch;

    


    sqlite3* db;

    sqlite3_stmt* res;
    const char* tail;

    // Save any error messages
    char* zErrMsg = 0;

    // Save the result of opening the file
    int rc;


    // Save the result of opening the file. DB file has opened.
    rc = sqlite3_open("fromtxt.db", &db);


    // Check the Sqlite connection. This, if statement is for unsuccesful connection
    if (rc) {
        // Show an error message
        cout << "DB Error: " << sqlite3_errmsg(db) << endl;
        // Close the connection
        sqlite3_close(db);
        // Return an error
        return(1);
    }


    tablecode.str("");
    tablecode << "CREATE TABLE IF NOT EXISTS 1_outputfile_Leadtime3 (ID INTEGER PRIMARY KEY AUTOINCREMENT, timeindex INT, yvect0 INT, yvect1 INT, yvect2 INT, yvect3 INT, mvect0 INT, mvect1 INT, optqvect1 INT, optqr INT, optcost REAL, gammacost REAL);";
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

    sqlcode.str("");
    sqlcode << "INSERT INTO 1_outputfile_Leadtime3 (timeindex, yvect0, yvec1, yvec2, yvec3, mvect0, mvec1, optqvect1, optqr, optcost, gammacost) VALUES(?,?,?,?,?,?,?,?,?,?,?);";
    query = sqlcode.str();
    rc = sqlite3_prepare_v2(db, query.c_str(), query.length(), &res, &tail);

    for (int i = 0; col0.size(); i++) {

        if (rc == SQLITE_OK) {
            sqlite3_bind_int(res, 1, col0[i]);
            sqlite3_bind_int(res, 2, col1[i]);
            sqlite3_bind_int(res, 3, col2[i]);
            sqlite3_bind_int(res, 4, col3[i]);
            sqlite3_bind_int(res, 5, col4[i]);
            sqlite3_bind_int(res, 6, col5[i]);
            sqlite3_bind_int(res, 7, col6[i]);
            sqlite3_bind_int(res, 8, col7[i]);
            sqlite3_bind_int(res, 9, col8[i]);
            sqlite3_bind_double(res, 10, col9[i]);
            sqlite3_bind_double(res, 11, col10[i]);


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
*/




/*
int main() {

    std::vector<std::vector<double> > table;
    std::fstream ifs;

    ifs.open("test.txt");

    while (true) {
        std::string line;
        std::getline(ifs, line);

        if (!ifs)
            break;

        if (line[0] == 'y')
            continue;

        std::vector<double> row;
        std::copy(std::istream_iterator<double>(ifs),
            std::istream_iterator<double>(),
            std::back_inserter(row));

        table.push_back(row);
    }

    
    char c;
    for (auto row : table) {
        for (auto el : row) {
            std::cout << el << ' ';
        }
        std::cout << "\n";
        cin >> c;

    }
}
*/


/*
int main() {
    
    std::vector<std::vector<int>> v;
    std::ifstream ifs("test.txt");
    std::string tempstr;
    int tempint;
    char delimiter, c;
    
   
    while (std::getline(ifs, tempstr)) {
        std::istringstream iss(tempstr);
        std::vector<int> tempv;
        while (iss >> tempint) {
            tempv.push_back(tempint);
            iss >> delimiter;
        }
        v.push_back(tempv);
    }

    for (auto row : v) {
        for (auto el : row) {
            std::cout << el << ' ';
        }
        std::cout << "\n";
        cin >> c;
        
    }
}
*/


/*
#include <iostream>
#include <sqlite3.h>
#include <sstream>
#include <string>
#include <fstream>
using namespace std;

ostringstream tablecode, sqlcode;
string query;

FILE *file;


int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    // Return successful
    return 0;
}

int main()
{
    sqlite3* db;

    // Save any error messages
    char* zErrMsg = 0;

    // Save the result of opening the file
    int rc;

    
    // Save the result of opening the file. DB file has opened.
    rc = sqlite3_open("fromtxt.db", &db);


    // Check the Sqlite connection. This, if statement is for unsuccesful connection
    if (rc) {
        // Show an error message
        cout << "DB Error: " << sqlite3_errmsg(db) << endl;
        // Close the connection
        sqlite3_close(db);
        // Return an error
        return(1);
    }

   
    tablecode.str("");
   
    tablecode << "CREATE TABLE IF NOT EXISTS 1_outputfile_Leadtime3 (ID INTEGER PRIMARY KEY AUTOINCREMENT, timeindex INT, yvect0 INT, yvect1 INT, yvect2 INT, yvect3 INT, mvect0 INT, mvect1 INT, optqvect1 INT, optqr INT, optcost REAL, gammacost REAL);";
    query = tablecode.str();
    rc = sqlite3_exec(db, query.c_str(), callback, 0, &zErrMsg);


    //file = fopen("1_outputfile_Leadtime3.txt", "r");


    ifstream input;
    char c;
    input.open("1_outputfile_Leadtime3.txt");


    if (input.is_open()) {//ADDED. LOD INDEX
        cout << "Parameter file is opened successfully." << endl;
        cin >> c;
    }
    else
    {
        cout << "Unable to open the parameter file" << endl;
        cout << "quitting...";
        cout << '\a';
        cin >> c;
        exit(0);
    }
   

    
    string line;
    ifstream res("1_outputfile_Leadtime3.txt");
    while (getline(res, line)) {
        string sql = "INSERT into 1_outputfile_Leadtime3 (timeindex,yvect0,yvect1,yvect2,yvect3,mvect0,mvect1,optqvect1,optqr,optcost,gammacost) VALUES (" + line + ");";
       
        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            cout << "SQL error:" << zErrMsg;
            sqlite3_free(zErrMsg);
        }
    }

    sqlite3_close(db);
    


}
*/
