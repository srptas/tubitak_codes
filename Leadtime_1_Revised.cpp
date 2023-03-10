﻿#include <math.h>
#include <conio.h>
#include <fstream>
#include <algorithm>
#include <functional>
#include <queue>
#include <vector>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <random>
#include <string>
#include <sstream>
#include <ctime>
#include <ios>
#include <cmath>
#include <ctime>
//#include "\Users\Mustafa Hekimoglu\source\Mylib\sqlite3.h"
#include<Windows.h>
#include <thread>
#include<process.h>

#include"C:\Users\khas\source\repos\boncuk.h"


#include <sqlite3.h>


//FOR CPU AND WALL CLOCK
#include <iomanip>
#include <chrono>
#include <ctime>
#include <thread>


// V=1, Revised Zülal İşler - 12.06.2021
// V=2, Revised Melisa Tugcu - 08.12.2022
// Revised Serap Tas - 10.03.2023

using namespace std;

int calculatestatespace(int i, int timeindex, int maxDmax, int lt, int v, int N, int yvect[], int mvect[]);
int calculateinitialstate(int horizon, int maxDmax, int lt, int N, int initialstateindex[]);

int calculateindex(int timeindex, int yvect[], int mvect[], int lt, int v, int N, int maxDmax);
double calculateprobvect(int v, int mvect[], double pvect[], double probvect[], int distsupport);

double functionL(int y, int totqp, int mvect[], double probvect[], double holdrate, double backlograte);
double singlepercost3Dprint(int v, int y, int mvect[], int qr, int qpvect[], double cr, double cpvect[], double hold, double backlog, int d);
void convolution2araydist(double dist1[], double dist2[], double targetdist[], int maxsupport);
void calculatemvectdouble(int y, int mvect[], int mvectdoublebar[]);
long double binompdf2(int k, int n, double p);
long double binomcdf(int k, int n, double p);
int invbinomcdf(float theta, float n, float p);


const int v = 1;  //3
const int LT = 1;
int N = 10;
int distruption_type;
const int Nn = 100;
const int M = 10000000; //very large number;

double holdrate = 0.25;
double backlograte = 1.416666667; //24.75;
double substitutionrate = 0;
double gamma = 0.5;

long double qrcost;
long double qpcost;
long double hc;
long double bc;
long double pr;

const double discountfactor = 0.995;
int setup = 1;
double phi = 0.1; //0.999; //0.929; // 0.65; //
double eta = 0.9; //0.01; //0.032; //0.9; //
double ptilP_;
double cpcr_;
char dbname[30];
char filename1[30];
char filename2[30];

const char* errMSG;
const char* tail;
char* zErrMsg = 0;

bool testflg = FALSE;
const int horizon = 10;
int px;
long double gammafunccost;

struct states {
	int st;
	int per;
	long double optcost;
	int optqr;

	int optqpvect[v];
	int yvect[LT + 1];
	int mvect[v + 1];


	long double gammacost;

	float sales;
	float lostsales_exp;
	float carryc_exp;
	float eip_exp;
	float wos_exp;
	float salvagecost_exp;
};


int letter2ind(string letters);
void readparams(int ind, int& param1, int& param2, double& param3, double& param4, double& param5);





//SQLITE
// For sqlite bind
std::ostringstream tablecode, sqlcode;
std::string query;
using namespace std;


// Create a callback function
int callback(void* NotUsed, int argc, char** argv, char** azColName) {
	// Return successful
	return 0;
}
//


//CPU TIME CALC. FUNCTION
void f();

//int main()
int main(int argc, char* argv[])
{


	//TIME IN SECONDS
	clock_t tStart = clock(); //for the elapsed time


	//CPU TIME AND WALL CLOCK CALCULATION START
	std::clock_t c_start = std::clock();
	auto t_start = std::chrono::high_resolution_clock::now();
	thread t1(f);
	thread t2(f); // f() is called on two threads
	t1.join();
	t2.join();
	//


	// SQLITE INITIALIZE START
	// Pointer to SQLite connection
	sqlite3* db = nullptr;


	//DATABASE PARAMETERS FOR FASTER PROCESS.
	sqlite3_exec(db, "PRAGMA cache_size = 20000", NULL, NULL, &zErrMsg);  //NUMBER OF PAGES IN THE MEMORY
	sqlite3_exec(db, "PRAGMA synchronous = OFF", NULL, NULL, &zErrMsg);
	sqlite3_exec(db, "PRAGMA journal_mode = MEMORY", NULL, NULL, &zErrMsg);

	// For binding
	sqlite3_stmt* res;
	const char* tail;

	// Save any error messages
	char* zErrMsg = 0;

	// Save the result of opening the file
	int rc;

	// Save any SQL
	string sql;

	// Save the result of opening the file
	rc = sqlite3_open("database.db", &db);

	if (rc) {
		// Show an error message
		cout << "DB Error: " << sqlite3_errmsg(db) << endl;
		// Close the connection
		sqlite3_close(db);
		// Return an error
		return(1);
	}
	//SQLITE INITIALIZE END




	cout << "=======================================NEW RUN====================================================================\n\n" << endl;
	//!!!!!!!!!!! //int indexx = letter2ind(argv[1]);
	int indexx = letter2ind(argv[1]);
	//int indexx = 2;
	//!!!!!!!!!!! //readparams(indexx, horizon, backlograte, substitutionrate, holdrate, gamma);
	readparams(indexx, N, distruption_type, cpcr_, holdrate, ptilP_);
	//int initialhorizon = horizon;
	cout << indexx << " " << N << " " << distruption_type << " " << cpcr_ << " " << holdrate << " " << ptilP_ << endl;
	//!!!!!!!!!!! //cout << indexx << " " << horizon << " " << backlograte << " " << substitutionrate << " " << holdrate << " " << gamma << endl;
	//holdingrate, ptil/p, cr/cp,distruption type,N


	// CREATE TABLE ACCORDING TO INDEXX
	tablecode.str("");
	tablecode << "CREATE TABLE Leadtime1_" << indexx << " (ID INTEGER PRIMARY KEY AUTOINCREMENT, timeindex INT, yvect0 INT, yvectLT INT, mvect0 INT, mvect1 INT, optqr INT, optqvect0 INT, optcost REAL, gammacost REAL);";
	query = tablecode.str();
	sqlite3_exec(db, query.c_str(), callback, 0, &zErrMsg);


	// Run the SQL (convert the string to a C-String with c_str() )
	rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);


	// Check if the table is created
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else {
		fprintf(stdout, "Table created successfully!\n");
	}






	int i = 0, j, jj;
	char c;
	int d;


	int d1;
	long double prob0, prob1, probmult;

	float mu = 0.8, sigma = 1, epsilon = 0.001;
	int timeindex = horizon; //DEĞİŞİM 18.11
	int initialstateindex[horizon + 1]; //DEĞİŞİM 18.11

	int yvect[LT + 1];
	int yvectnew[LT + 1];
	int mvect[v + 1];
	int mvectdoublebar[v + 1];
	int mvectnew[v + 1];
	double ptildevect[v + 1];
	double xivect[v] = { 100 }; //{ 100,200,300 }; //{ 50, 250, 450 };   //100;  //{ 100,200,300,400,500 };
	double cpvect[v];
	double cr = 100;
	//double cp0 = 40;
	// double alpha= 0.426, r= 0.15, s= -0.00001254;
	//ptilde vect hesaplanırken kullanılıyor
	double alpha = 3, r = 0.01, s = -0.00001;
	ptildevect[0] = 0.1;
	ptildevect[1] = ptildevect[0] * ptilP_;
	cpvect[0] = cr * cpcr_;





	long double probvect[Nn + 1];
	long double probvect2[Nn + 1];
	long double targetprobvect[Nn + 1];
	long double costper, costpergamma;


	long double gammacost;
	long double opttotcost, totcost;
	long double totcostgamma, optcostgamma;
	long double nextpercost;
	long double nextpergammacost;

	long double hold = cr * holdrate;
	long double backlog = backlograte * cr;
	long double substitutioncost = substitutionrate * cr;
	int maxDmax = invbinomcdf(0.99999, N * (LT + 1), ptildevect[1]);//eklendi 06.01.2023
	//int inverse= invbinomcdf(0.99999,10, 0.1);
	int maxyupperbound = (N + 1) * (LT + 1); //(N + 1) * (v + 1);

	calculateinitialstate(maxDmax, LT, N, horizon, initialstateindex);

	int statesize = initialstateindex[timeindex - 1] - initialstateindex[timeindex - 2];  //pow((double)N + 1, v)*pow(N + 1, (LT + 1));
	int statesize2 = initialstateindex[timeindex - 1] - initialstateindex[timeindex - 2];


	cout << "Total State Size Per Period is: " << statesize << endl;

	int qr;
	int qpvect[v];

	double optcost;
	int optqr;
	int optqvect[v];
	int optqvectgamma[v];

	int searchspace = pow((double)N + 1, v);
	//int orderuptoqr;
	long double ps;

	int totprintedparts;
	int totprintedpartsafterchange;
	int totorgpartsafterchange;
	int totprintedpartsnew;
	int totprint;
	int totaldemand;
	int y_inv_oh;


	time_t now = time(NULL);
	char dt[26];


	states* statevect = new states[statesize];
	states* statevectnextper = new states[statesize];

	if (distruption_type == 0) {
		phi = 0.1;
		eta = 0.1;
		//long and infrequent
	}
	if (distruption_type == 1) {
		phi = 0.5;
		eta = 0.5;
		//short and frequent
	}
	/*if (setup == 1)
	{
		ptildevect[0] = 0.5;

		for (i = 1; i <= v; i++)
		{
			ptildevect[i] = ptildevect[0] + 1 / (3 + 0.01 * xivect[i - 1] - 0.000001 * pow(xivect[i - 1], 2));
			cpvect[i - 1] = 10 * pow(xivect[i - 1], gamma);  //0.65
		}
	}
	*/
	/*if (setup == 2)
	{
		ptildevect[0] = 0.3;

		for (i = 1; i <= v; i++)
		{
			ptildevect[i] = ptildevect[0] + 1 / (alpha + r * xivect[i - 1] + s * pow(xivect[i - 1], 2)); //ptildevect[0] + 1 / (3 + 0.01*xivect[i - 1] + 0.000001* pow(xivect[i - 1], 2));
			cpvect[i - 1] = cp0 + 10 * pow(xivect[i - 1], gamma);  //0.65
		}

	}*/

	//ptildevect[0] = 0.01;
	//ptildevect[1] = 0.99;



	std::ostringstream paramheader;
	paramheader << "Parameters: Horizon: " << horizon << " HoldingRate:" << holdrate << " BacklogRate:" << backlograte << " SubstitutionRate: " << substitutionrate << " xivect=(" << xivect[0] << ")";
	paramheader << " alpha=" << alpha << ", r=" << r << ", s=" << s << ", ptilde.vect = (" << ptildevect[0] << "," << ptildevect[1] << ")";
	paramheader << " gamma= " << gamma << " cpr.vect = (" << cr << "," << cpvect[0] << ")";
	std::string parameterheader = paramheader.str();
	cout << parameterheader;

	cout << "\n\nLaser Energy Density: \t xi.1=" << xivect[0] << endl;
	cout << "Failure Rates: \t\t p.org=" << ptildevect[0] << " p.1=" << ptildevect[1] << endl;
	cout << "Acquisition Costs: \t cr=" << cr << " cp0=" << cpvect[0] << endl;

	/*if (v == 1)
	{
		mvect[1] = 1;
		mvect[0] = N;
		for (i = 0; i < v; i++)
		{
			mvect[0] -= mvect[i + 1];
			// cout << mvect[0] << "    " << mvect[1] << endl;
		}
	}*/

	//!!!!!!!!!
	/* time_t timer;
	struct tm* timeinfo;
	time(&timer);
	// timeinfo = localtime(&timer);
	*/


	//int y = 1;


	ofstream outputfile, outputsub, resultfile, logfile, resultfile0, errorfile, timefile;
	// logfile.open("Logfile.txt", std::ofstream::out | std::ofstream::app);
	// logfile << "Program Starts! Parameter Index: " << indexx << " TimeStamp: " << asctime(timeinfo);
	// outputfile.open(filename1);

	errorfile.open("errorfile.txt", std::ofstream::out | std::ofstream::app);
	errorfile << "timeindex" << "\t" << "yvect[0]" << "\t" << "yvect[LT]" << "\t" << "mvect[0]" << "\t" << "mvect[1]" << "\t" << "mvect[2]" << "\t" << "mvect[3]" << "\t" << "optcost" << "\t" << "optcostgamma" << endl;

	resultfile0.open("resultfile0.txt", std::ofstream::out | std::ofstream::app);
	resultfile0 << "timeindex" << "\t" << "yvect[0]" << "\t" << "yvect[LT]" << "\t" << "mvect[0]" << "\t" << "mvect[1]" << "\t" << "mvect[2]" << "\t" << "mvect[3]" << "\t" << "d" << "\t" << "d1" << "\t" << "d2" << "\t" << "d3" << "\t" << "totaldemand" << "\t" << "qr" << "\t" << "qpvect[0]" << "\t" << "qpvect[1]" << "\t" << "qpvect[2]" << "\t" << "hc" << "\t" << "bc" << "\t" << "qpcost" << "\t" << "qrcost" << "\t" << "prob.Dr" << "\t" << "prob.Dp1" << "\t" << "prob.Dp2" << "\t" << "prob.Dp3" << "\t" << "costper" << "\t" << "gammacost" << "\t" << "nextpercost" << "\t" << "nextpergammacost" << "\t" << "index" << "\t" << "optcost" << "\t" << "optcostgamma" << endl;

	ofstream testfile;
	testflg = true;
	if (testflg)
	{
		testfile.open("filetest.txt", std::ofstream::out | std::ofstream::app);
		testfile << "i" << "\t" << "timeindex" << "\t" << "yvect[0]" << "\t" << "yvect[LT]" << "\t" << "mvect[0]" << "\t" << "mvect[1]" << "\t" << "optqr" << "\t" << "optcost" << "\t" << "gammacost" << endl;
		// cout << statesize << endl;
		for (i = 0; i < initialstateindex[timeindex - 1] - initialstateindex[timeindex - 2]; i++)
		{
			px = i;

			calculatestatespace(px, timeindex, maxDmax, LT, v, N, yvect, mvect);

			mvect[0] = N; totprintedparts = 0;
			for (j = 1; j <= v; j++)
				totprintedparts += mvect[j];

			// if ((totprintedparts > 0) & (yvect[0] > 0))
			// continue;

			mvect[0] -= totprintedparts;
			if (totprintedparts > N)
				continue;
			jj = calculateindex(timeindex, yvect, mvect, LT, v, N, maxDmax);

			// cout << i << " : " <<jj<<" "<< yvect[0] << " " << yvect[LT] << " == " << mvect[0] << " " << mvect[1] << " " << mvect[2] << " " << mvect[3] << endl;
			testfile << i << " \t : \t" << jj << "\t" << yvect[0] << "\t" << yvect[LT] << " \t == \t" << mvect[0] << "\t" << mvect[1] << endl;
			// std::cout<< i << " \t : \t " << jj << "\t" << yvect[0] << "\t" << yvect[LT] << " \t == \t" << mvect[0] << "\t" << mvect[1] <<  endl;

		}
	}
	//BURADA STATESPACE I DOĞRU HESAPLADIĞINI TEST ETTİM ÇALIŞIYOR!!!!!!!!!!

	/* yvect[0] = 1; yvect[1] = 2;
	mvect[0] = 1; mvect[1] = 2;
	int aaa= calculateindex(yvect, mvect, LT, v, N + 1, maxyupperbound);
	cout << yvect[0]<<"   "<< yvect[1] << "   " << "index number    "<<aaa << endl;
	/* BURADA INDEX I DOĞRU HESAPLADIĞINI TEST ETTİM ÇALIŞIYOR!!!!!! */


	//outputsub.open("outputsubfile.txt");
	outputsub.open(filename2);
	outputsub << parameterheader << endl;


	//LAST PERIOD
	std::cout << "\n\n" << "Period " << timeindex << " starts!" << endl;
	for (i = 0; i < initialstateindex[timeindex - 1] - initialstateindex[timeindex - 2]; i++)
	{
		px = i;
		calculatestatespace(px, timeindex, maxDmax, LT, v, N, yvect, mvect);

		mvect[0] = N; //incele
		totprintedparts = 0;
		totprintedpartsafterchange = 0;

		//		if (yvect[0] + yvect[LT] > maxyupperbound)//comment out
		//			continue;

		for (j = 1; j <= v; j++)
			totprintedparts += mvect[j];

		if (totprintedparts > N)
			cin >> c;

		//continue;

		mvect[0] -= totprintedparts;


		optqr = N;  // burası 10 du N yaptık
		optqvect[0] = 0;
		opttotcost = M;


		for (qr = 0; qr <= maxyupperbound; qr++)  // qr search başlangıcı
		{
			gammacost = 0;
			//		qr = maxoftwo(orderuptoqr - yvect[0] - yvect[LT], 0);  //değiştirildi

			totcost = qr * cr; //+ minoftwo(y, totprintedparts) * substitutioncost;
			qrcost = totcost;
			hc = 0;
			bc = 0;
			totcostgamma = 0;


			for (d = 0; d <= mvect[0]; d++)
			{
				prob0 = binompdf2(d, mvect[0], ptildevect[0]);
				// cout << setprecision(30)<<prob0 << endl;
				// cin >> c;
				for (d1 = 0; d1 <= mvect[1]; d1++)
				{
					prob1 = binompdf2(d1, mvect[1], ptildevect[1]);

					probmult = prob0 * prob1;

					totaldemand = d + d1;
					optcost = M;
					optcostgamma = M;

					y_inv_oh = yvect[0] + yvect[1];
					qpvect[0] = maxoftwo(0, totaldemand - y_inv_oh);

					qpcost = qpvect[0] * cpvect[0]; //long double qpcost = optcost;   /REMOVE

					costper = qpvect[0] * cpvect[0];
					costper += hold * maxoftwo(0, y_inv_oh - totaldemand);
					costper += backlog * maxoftwo(0, totaldemand - y_inv_oh - qpvect[0]);


					costpergamma = qpvect[0] * cpvect[0];
					costpergamma += hold * maxoftwo(0, y_inv_oh - totaldemand);
					costpergamma += backlog * maxoftwo(0, totaldemand - y_inv_oh - qpvect[0]);





					totcost += costper * probmult;
					totcostgamma += costpergamma * probmult;;




				} //CONDITIONAL PROBABILITY
			} //CONDITIONAL PROBABILITY

			if (qr == 0)
			{
				statevect[i].gammacost = totcostgamma;
			}

			if (opttotcost > totcost)
			{
				optqr = qr;
				opttotcost = totcost;
			}


		}  //END OF qr SEARCH

		if (totcostgamma < opttotcost)
		{
			cout << "ERROR GAMMA COST IS SMALLER THAN OPTIMUM COST" << endl;
			errorfile << timeindex << "\t" << yvect[0] << "\t" << yvect[LT] << "\t" << mvect[0] << "\t" << mvect[1] << "\t" << totcost << "\t" << totcostgamma << endl;
			cin >> c;
		}





		statevect[i].optqr = optqr;
		statevect[i].optcost = opttotcost;
		statevect[i].per = timeindex;
		statevect[i].yvect[0] = yvect[0];
		statevect[i].yvect[1] = yvect[1];
		statevect[i].mvect[0] = mvect[0];
		statevect[i].mvect[1] = mvect[1];
		statevect[i].optqpvect[0] = optqvect[0];

		/*	if ((i % 500) == 0)
			{
				std::cout << "Period " << timeindex << " State Index: " << i << ": State= (" << yvect[0] << " " << yvect[LT] << " || " << mvect[0] << " " << mvect[1] << ") " << " qr=" << optqr << " Cost=" << opttotcost << endl;
			}*/

	} //END OF WHILE LOOP

	/*for (i = 0; i < statesize; i++)
	{
		testfile << setprecision(30) << i << ":\t" << statevect[i].per << "\t" << statevect[i].yvect[0] << "\t" << statevect[i].yvect[1] << "\t" << statevect[i].mvect[0] << "\t" << statevect[i].mvect[1] << "\t" << statevect[i].optqr << "\t" << statevect[i].optcost << "\t" << statevect[i].gammacost << endl;
	}*/

	//std::cout << "Period " << timeindex << " is complete! Writing Starts!" << endl;
	//int n;
	//n = sprintf_s(filename1, "outputfile_Leadtime1_%d.txt", indexx);

	//outputfile.open(filename1, std::ofstream::out | std::ofstream::app);
	//outputfile << "timeindex" << "\t" << "yvect[0]" << "\t" << "yvect[LT]" << "\t" << "mvect[0]" << "\t" << "mvect[1]" << "\t" << "optqr" << "\t" << "optqvect[0]" << "\t" << "optcost" << "\t" << "gammacost" << endl;

	//outputfile << parameterheader << endl;



	/*for (i = 0; i < initialstateindex[timeindex - 1] - initialstateindex[timeindex - 2]; i++)
	{
		px = i;
		calculatestatespace(px, timeindex, maxDmax, LT, v, N, yvect, mvect);

		mvect[0] = N; totprintedparts = 0;
		//if (yvect[0] + yvect[LT] > maxyupperbound)
			//	continue;

		for (j = 1; j <= v; j++)
			totprintedparts += mvect[j];

		if (totprintedparts > N)
			continue;

		mvect[0] -= totprintedparts;

		//outputfile << setprecision(30) << statevect[i].per << "\t" << statevect[i].yvect[0] << "\t" << statevect[i].yvect[1] << "\t" << statevect[i].mvect[0] << "\t" << statevect[i].mvect[1] << "\t" << statevect[i].optqr << "\t" << statevect[i].optqpvect[0] << "\t" << statevect[i].optcost << "\t" << statevect[i].gammacost << endl;
	}*/
	//cin >> c;
	//outputfile.close();



	//SQLITE BIND START

	if (sqlite3_open("database.db", &db) == SQLITE_OK)
	{


		//PREPARE SQLCODE. LATER WE BIND THEM WITH PARAMETERS TO BE REPLACED QUESTION MARKS
		sqlcode.str("");
		sqlcode << "INSERT INTO Leadtime1_" << indexx << " (timeindex, yvect0, yvectLT, mvect0, mvect1, optqr, optqvect0, optcost, gammacost) VALUES(?,?,?,?,?,?,?,?,?);";
		query = sqlcode.str();
		rc = sqlite3_prepare_v2(db, query.c_str(), query.length(), &res, &tail);


		for (i = 0; i < initialstateindex[timeindex - 1] - initialstateindex[timeindex - 2]; i++) {


			if (rc == SQLITE_OK)
			{
				//sqlite3_bind_int(res, 1, NULL);
				sqlite3_bind_int(res, 1, statevect[i].per);
				sqlite3_bind_int(res, 2, statevect[i].yvect[0]);
				sqlite3_bind_int(res, 3, statevect[i].yvect[1]);
				sqlite3_bind_int(res, 4, statevect[i].mvect[0]);
				sqlite3_bind_int(res, 5, statevect[i].mvect[1]);
				sqlite3_bind_int(res, 6, statevect[i].optqr);
				sqlite3_bind_int(res, 7, statevect[i].optqpvect[0]);
				sqlite3_bind_double(res, 8, statevect[i].optcost);
				sqlite3_bind_double(res, 9, statevect[i].gammacost);


				sqlite3_step(res);
				sqlite3_clear_bindings(res);					//CLEAR BINDINGS OF WRITE QUERY
				sqlite3_reset(res);
				//cout << "id: " << id;

			}
		}

		std::cout << "Period " << timeindex << " is complete! Inserting to the Database starts!" << endl;

	}

	//SQLITE BIND END




	timeindex--;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//NEXT PERIOD
	int endindex;
	while (timeindex >= 1)
	{
		cout << "Period " << timeindex << " starts!" << endl;
		if (timeindex == 1)
			endindex = 0;
		if (timeindex > 1)
			endindex = initialstateindex[timeindex - 2];
		for (i = 0; i < initialstateindex[timeindex - 1] - endindex; i++)
		{
			px = i;
			calculatestatespace(px, timeindex, maxDmax, LT, v, N, yvect, mvect);
			y_inv_oh = yvect[0] + yvect[1];

			mvect[0] = N;
			totprintedparts = 0;

			//		if (yvect[0] + yvect[LT] > maxyupperbound)
			//			continue;

			for (j = 1; j <= v; j++)
				totprintedparts += mvect[j];

			if (totprintedparts > N)
				cin >> c;
			//continue;

			mvect[0] -= totprintedparts;
			optqr = N;
			optqvect[0] = 0;
			opttotcost = M;

			for (qr = 0; qr < maxDmax; qr++)  //N * (LT + 1)
			{
				//qr = maxoftwo(orderuptoqr - yvect[0] - yvect[LT], 0); //değiştirildi
				totcost = qr * cr;// +minoftwo(y, totprintedparts) * substitutioncost;
				qrcost = totcost;
				double demand = 0;
				hc = 0; bc = 0;
				totcostgamma = 0;

				for (d = 0; d <= mvect[0]; d++) //totorgpartsafterchange
				{
					prob0 = binompdf2(d, mvect[0], ptildevect[0]);

					for (d1 = 0; d1 <= mvect[1]; d1++) //mvectdoublebar
					{
						prob1 = binompdf2(d1, mvect[1], ptildevect[1]);
						probmult = prob0 * prob1;

						totaldemand = d + d1;
						optcost = M;
						optcostgamma = M;
						qpvect[0] = maxoftwo(0, totaldemand - y_inv_oh);

						costper = qpvect[0] * cpvect[0];
						costper += hold * maxoftwo(0, y_inv_oh - totaldemand) + backlog * maxoftwo(0, totaldemand - y_inv_oh - qpvect[0]);
						costpergamma = qpvect[0] * cpvect[0];
						costpergamma += hold * maxoftwo(0, y_inv_oh - totaldemand) + backlog * maxoftwo(0, totaldemand - y_inv_oh - qpvect[0]);

						yvectnew[LT] = qr;
						yvectnew[0] = maxoftwo(0, y_inv_oh - totaldemand);

						mvectnew[0] = mvect[0] - d + minoftwo((y_inv_oh), totaldemand);
						mvectnew[1] = mvect[1] - d1 + qpvect[0];


						jj = calculateindex(timeindex + 1, yvectnew, mvectnew, LT, v, N, maxDmax);
						nextpercost = statevect[jj].optcost;
						nextpergammacost = statevect[jj].gammacost;
						costper += nextpercost * discountfactor * phi + nextpergammacost * discountfactor * (1 - phi);
						costpergamma += nextpercost * discountfactor * eta + nextpergammacost * discountfactor * (1 - eta);


						if (qr == 0)
						{
							if (costpergamma < optcostgamma)
							{
								optqvectgamma[0] = qpvect[0];
								optcostgamma = costpergamma;
							}
						}

						pr = prob0; ps = prob1;
						qpcost = qpvect[0] * cpvect[0];


						totcost += costper * probmult;
						totcostgamma += costpergamma * probmult;

					} //CONDITIONAL PROBABILITY
				} //CONDITIONAL PROBABILITY

//				if ((horizon == 1) & (yvect[0] == 3) & (yvect[1] == 0) & (mvect[0] == 10))
//					cout<<qr<<" "<<totcost<<endl;

				if (qr == 0)
				{
					statevectnextper[i].gammacost = totcostgamma;
				}

				if (opttotcost > totcost)
				{
					optqr = qr;
					opttotcost = totcost;
				}

			}//END OF QR SEARCH

			statevectnextper[i].optqr = optqr;
			statevectnextper[i].optcost = opttotcost;
			statevectnextper[i].per = timeindex;
			statevectnextper[i].yvect[0] = yvect[0];
			statevectnextper[i].yvect[1] = yvect[1];
			statevectnextper[i].mvect[0] = mvect[0];
			statevectnextper[i].mvect[1] = mvect[1];
			statevectnextper[i].optqpvect[0] = optqvect[0];

			/*if ((i % 500) == 0)
			{
				std::cout << "Period " << timeindex << " State Index: " << i << ": State= (" << yvect[0] << " " << yvect[LT] << " || " << mvect[0] << " " << mvect[1] << ") " << " qr=" << optqr << " Cost=" << opttotcost << endl;
			}*/
		}

		//std::cout << "Period " << timeindex << " is complete! Writing Starts!" << endl;
		// outputfile.open("output.txt", std::ofstream::out | std::ofstream::app);
		// outputfile.open(filename1, std::ofstream::out | std::ofstream::app);


		if (timeindex == 1)
			endindex = 0;
		if (timeindex > 1)
			endindex = initialstateindex[timeindex - 2];
		for (i = 0; i < initialstateindex[timeindex - 1] - endindex; i++)
		{
			px = i;

			statevect[i].optqr = statevectnextper[i].optqr;
			statevect[i].optcost = statevectnextper[i].optcost;
			statevect[i].per = statevectnextper[i].per;
			statevect[i].gammacost = statevectnextper[i].gammacost;
			statevect[i].optqpvect[0] = statevectnextper[i].optqpvect[0];
			statevect[i].yvect[0] = statevectnextper[i].yvect[0];
			statevect[i].yvect[1] = statevectnextper[i].yvect[1];
			statevect[i].mvect[0] = statevectnextper[i].mvect[0];
			statevect[i].mvect[1] = statevectnextper[i].mvect[1];


			calculatestatespace(px, timeindex, maxDmax, LT, v, N, yvect, mvect);


			mvect[0] = N; totprintedparts = 0;
			//			if (yvect[0] + yvect[LT] > maxyupperbound)
			//				continue;
			for (j = 1; j <= v; j++)
				totprintedparts += mvect[j];

			//			if (totprintedparts > N)
			//				continue;

			mvect[0] -= totprintedparts;

			//outputfile << setprecision(30) << statevect[i].per << "\t" << statevect[i].yvect[0] << "\t" << statevect[i].yvect[1] << "\t" << statevect[i].mvect[0] << "\t" << statevect[i].mvect[1] << "\t" << statevect[i].optqr << "\t" << statevect[i].optqpvect[0] << "\t" << statevect[i].optcost << "\t" << statevect[i].gammacost << endl;

		}
		// outputfile.close();


		if (sqlite3_open("database.db", &db) == SQLITE_OK)
		{

			sqlcode.str("");
			sqlcode << "INSERT INTO Leadtime1_" << indexx << " (timeindex, yvect0, yvectLT, mvect0, mvect1, optqr, optqvect0, optcost, gammacost) VALUES(?,?,?,?,?,?,?,?,?);";
			query = sqlcode.str();
			rc = sqlite3_prepare_v2(db, query.c_str(), query.length(), &res, &tail);



			for (i = 0; i < initialstateindex[timeindex - 1] - endindex; i++) {

				if (rc == SQLITE_OK)
				{
					//sqlite3_bind_int(res, 1, NULL);
					sqlite3_bind_int(res, 1, statevectnextper[i].per);
					sqlite3_bind_int(res, 2, statevectnextper[i].yvect[0]);
					sqlite3_bind_int(res, 3, statevectnextper[i].yvect[1]);
					sqlite3_bind_int(res, 4, statevectnextper[i].mvect[0]);
					sqlite3_bind_int(res, 5, statevectnextper[i].mvect[1]);
					sqlite3_bind_int(res, 6, statevectnextper[i].optqr);
					sqlite3_bind_int(res, 7, statevectnextper[i].optqpvect[0]);
					sqlite3_bind_double(res, 8, statevectnextper[i].optcost);
					sqlite3_bind_double(res, 9, statevectnextper[i].gammacost);


					sqlite3_step(res);
					sqlite3_clear_bindings(res);					//CLEAR BINDINGS OF WRITE QUERY
					sqlite3_reset(res);


				}


			}
			std::cout << "Period " << timeindex << " is complete! Inserting to the Database starts!" << endl;
		}


		timeindex--;
	}
	// Close SQLITE connection
	sqlite3_close(db);

	//outputfile.close();
	testfile.close();
	resultfile0.close(); outputsub.close();
	resultfile.open("resultfile.txt", std::ofstream::out | std::ofstream::app);
	resultfile << indexx << "\t" << parameterheader << "\t" << statevect[0].per << "\t" << statevect[0].optcost << "\t" << statevect[0].optqr << endl;
	resultfile.close();




	delete[] statevect;
	delete[] statevectnextper;
	//return 0;

	// CPU TIME AND WALL CLOCK CALCULATION END
	clock_t c_end = clock();

	printf("Time taken: %.2fs\n", (double)(clock() - tStart) / CLOCKS_PER_SEC); //elapsed time



	auto t_end = chrono::high_resolution_clock::now();

	cout << fixed << setprecision(2) << "CPU time used: "
		<< 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC << " ms\n"
		<< "Wall clock time passed: "
		<< chrono::duration<double, std::milli>(t_end - t_start).count()
		<< " ms\n";

	double cpu_time = 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC;
	double elapsed_time = chrono::duration<double, std::milli>(t_end - t_start).count();

	// WRITE TIME INFO TO THE TIMEFILE RESPECT TO PARAMS
	timefile.open("timefile.txt", std::ofstream::out | std::ofstream::app);
	timefile << "Parameters: Indexx: " << indexx << " Horizon: " << horizon << " HoldingRate : " << holdrate << " BacklogRate : " << backlograte << " SubstitutionRate : " << substitutionrate << " xivect = (" << xivect[0] << ")" << " Times : CPU Time : " << cpu_time << "ms" << " Elapsed Time : " << elapsed_time << "ms\n";
	timefile.close();

	return 0;

}






double calculateprobvect(int v, int mvect[], double pvect[], double probvect[], int distsupport)
{
	int i, j;

	double dist1[Nn + 1];
	double dist2[Nn + 1];
	double targetdist[Nn + 1];

	for (j = 0; j <= N; j++)
	{
		dist1[j] = binompdf(j, mvect[0], pvect[0]);
		dist2[j] = binompdf(j, mvect[1], pvect[1]);
	}

	convolution2araydist(dist1, dist2, targetdist, N);
	for (i = 3; i <= v; i++)
	{
		for (j = 0; j <= N; j++)
		{
			dist1[j] = binompdf(j, mvect[i], pvect[i]);
			dist2[j] = targetdist[j];
		}

		convolution2araydist(dist1, dist2, targetdist, N);

	}

	for (j = 0; j <= N; j++)
		probvect[j] = targetdist[j];


	return 0;

}

void calculatemvectdouble(int y, int mvect[], int mvectdoublebar[])
{
	int i, j;
	int mtemp;
	mtemp = mvect[1];
	mvectdoublebar[1] = (int)maxoftwo(mtemp - y, 0);
	/* mtemp = mvect[2];
	mvectdoublebar[2] = (int)maxoftwo((int)mvect[2] - (int)maxoftwo(y - (int)mvect[1], 0), 0);

	mtemp = mvect[1] + mvect[2];
	*/
	/*
	for (i = 3; i <= v; i++)
	{
	mvectdoublebar[i] = maxoftwo(mvect[i] - maxoftwo(y - mtemp, 0), 0);
	mtemp += mvect[i];
	}
	*/
	mvectdoublebar[0] = mvect[0];
}

double functionL(int y, int totqp, int mvect[], double probvect[], double holdrate, double backlograte)
{
	int s = 0;
	double expectedcost = 0;
	for (s = 0; s <= N; s++)
	{
		expectedcost += holdrate * maxoftwo(y - s, 0) * probvect[s] + maxoftwo(s - y - totqp, 0) * probvect[s] * backlograte;
	}

	return expectedcost;

}

double singlepercost3Dprint(int v, int y, int mvect[], int qr, int qpvect[], double cr, double cpvect[], double hold, double backlog, int d)
{
	double cost = 0;
	// cost += (double)qr*cr;

	int totm = 0;
	int totqp = 0;

	for (int i = 0; i < v; i++)
	{
		totm += mvect[i];
		cost += (double)qpvect[i] * cpvect[i];
		totqp += qpvect[i];
	}


	cost += hold * maxoftwo(y - d, 0) + maxoftwo(d - y - totqp, 0) * backlog;


	// functionL(y, totqp, mvect, probvect, hold, backlog);

	return cost;
}

int calculatestatespace(int i, int timeindex, int maxDmax, int lt, int v, int N, int yvect[], int mvect[])
{
	int lengthd = maxDmax + 1;  //+1
	int lengthv = N + 1;
	int l;
	int num = lengthd, denom = 1;
	int num2 = pow(lengthd, timeindex - 1) * lengthv, denom2 = pow(lengthd, timeindex - 1);
	if (timeindex < lt + 1) {
		for (l = 0; l <= lt; l++) {
			yvect[lt - l] = (i % num) / denom;
			denom *= lengthd;
			num = pow(lengthd, timeindex - 1);
		}
		for (l = 0; l < v; l++)
		{
			mvect[l + 1] = (i % num2) / denom2;
			denom2 = num2;
			num2 *= lengthv;
		}
		mvect[0] = N - mvect[1];
	}
	if (timeindex == lt + 1) {
		for (l = 0; l < lt; l++) {
			yvect[lt - l] = (i % num) / denom;
			denom = num;
			num *= lengthd;
		}
		yvect[lt - lt] = (i % lengthd) / denom;

		for (l = 0; l < v; l++)
		{
			mvect[l + 1] = (i % num2) / denom2;
			denom2 = num2;
			num2 *= lengthv;
		}
		mvect[0] = N - mvect[1];
	}
	if (timeindex > lt + 1) {
		for (l = 0; l < lt; l++) {
			yvect[lt - l] = (i % num) / denom;
			denom = num;
			num *= lengthd;
		}
		yvect[lt - lt] = (i % ((num / lengthd) * ((timeindex - lt - 1) * maxDmax + 1))) / denom;

		for (l = 0; l < v; l++)
		{
			num2 = pow(lengthd, lt) * ((timeindex - lt - 1) * maxDmax + 1) * (N + 1);//lt yeni eklendii!!!
			denom2 = pow(lengthd, lt) * ((timeindex - lt - 1) * maxDmax + 1);
			mvect[l + 1] = (i % num2) / denom2;

		}
		mvect[0] = N - mvect[1];


	}


	return 0;
}


int calculateinitialstate(int maxDmax, int lt, int N, int horizon, int initialstateindex[])
{
	int timeindex = 1;
	initialstateindex[timeindex - 1] = 1;


	for (timeindex = 2; timeindex <= 2 * lt + 1; timeindex++)
	{
		initialstateindex[timeindex - 1] = initialstateindex[timeindex - 2] + pow(maxDmax + 1, timeindex - 1) * (N + 1);
	}

	for (timeindex = 2 * lt + 2; timeindex <= horizon; timeindex++)
	{
		initialstateindex[timeindex - 1] = initialstateindex[timeindex - 2] + pow(maxDmax + 1, lt) * ((timeindex - lt - 1) * maxDmax + 1) * (N + 1);
	}



	return 0;
}



int calculateindex(int timeindex, int yvect[], int mvect[], int lt, int v, int N, int maxDmax)
{
	int multp1 = 0, multp2 = 0;
	int lengthd = maxDmax + 1;
	int lengthv = N + 1;

	int l, res = 0;

	for (l = 0; l <= lt; l++)
	{
		multp1 += yvect[lt - l] * (int)pow(lengthd, l);
	}

	if (timeindex > lt + 2) {

		multp2 = (int)pow((double)lengthd, lt) * ((timeindex - lt - 1) * (maxDmax)+1);
	}
	else {
		multp2 = (int)pow((double)lengthd, timeindex - 1);
	}

	for (l = 0; l < v; l++)
	{
		multp1 += multp2 * mvect[l + 1];
		multp2 *= lengthv;
	}



	res = multp1;


	return res;
}


long double binompdf2(int k, int n, double p)
{
	long double res;
	if (k <= n)
		res = pow(p, (long double)k) * pow(1 - p, (long double)(n - k)) * comb(n, k);
	if (k > n)
		res = 0;
	return res;
}

long double binomcdf(int k, int n, double p)
{
	long  double prb = 0;
	int i;

	for (i = 0; i <= k; i++)
	{
		prb = prb + binompdf(i, n, p);
	}

	return prb;
}

int invbinomcdf(float theta, float n, float p)
{
	//theta: the probility for inverse cdf function.
	int x = 0;
	long double prob;
	do
	{
		prob = binomcdf(x++, n, p);

	} while (prob <= theta);

	return x - 1;
}

void convolution2araydist(double dist1[], double dist2[], double targetdist[], int maxsupport)
{

	int i = 1, k, j;
	float temp[bignum];
	// float tempscalar;


	for (k = 0; k <= maxsupport; k++)
	{
		temp[k] = 0;
		for (j = 0; j <= k; j++)
			temp[k] += dist1[j] * dist2[k - j];
	}

	for (k = 0; k <= maxsupport; k++)
		targetdist[k] = temp[k];

}

int letter2ind(string letters)
{
	int a1 = letters[0];
	int a2 = letters[1];
	int a3 = letters[2];
	int a4 = letters[3];
	int a5 = letters[4];

	int num = (a1 - 97) * 10000 + (a2 - 97) * 1000 + (a3 - 97) * 100 + (a4 - 97) * 10 + (a5 - 97);

	cout << "Run index is: " << num << endl;

	return num;
}

//readparams(indexx, horizon, backlograte, substitutionrate, holdrate, gamma);

void readparams(int ind, int& param1, int& param2, double& param3, double& param4, double& param5)
{
	int trial, i, j; char c;
	double param[6];


	double temp;


	ifstream input;

	input.open("params.txt");


	if (input.is_open()) //ADDED. LOD INDEX
		cout << "Parameter file is opened successfully." << endl;
	else
	{
		cout << "Unable to open the parameter file" << endl;
		cout << "quitting...";
		cout << '\a';
		cin >> c;
		exit(0);
	}



	for (i = 1; i <= ind; i++)
	{
		if (i != ind)
		{
			for (j = 0; j < 5; j++)
				input >> temp;
		}
		else
		{
			for (j = 0; j < 5; j++)
			{
				input >> param[j];
			}
		}
	}


	input.close();

	for (j = 0; j < 5; j++)
	{
		switch (j)
		{
		case 0:
		{
			param1 = (int)param[j]; break;
		}
		case 1:
		{
			param2 = (int)param[j]; break;
		}
		case 2:
		{
			param3 = param[j]; break;
		}
		case 3:
		{
			param4 = param[j]; break;
		}
		case 4:
		{
			param5 = param[j];
		}
		}
	}

}

//CPU TIME CALC. FUNCTION
void f()
{
	volatile double d = 0;
	for (int n = 0; n < 10000; ++n)
		for (int m = 0; m < 10000; ++m) {
			double diff = d * n * m;
			d = diff + d;
		}
}

// Programı çalıştır: Ctrl + F5 veya Hata Ayıkla > Hata Ayıklamadan Başlat menüsü
// Programda hata ayıkla: F5 veya Hata Ayıkla > Hata Ayıklamayı Başlat menüsü

// Kullanmaya Başlama İpuçları:
//   1. Dosyaları eklemek/yönetmek için Çözüm Gezgini penceresini kullanın
//   2. Kaynak denetimine bağlanmak için Takım Gezgini penceresini kullanın
//   3. Derleme çıktısını ve diğer iletileri görmek için Çıktı penceresini kullanın
//   4. Hataları görüntülemek için Hata Listesi penceresini kullanın
//   5. Yeni kod dosyaları oluşturmak için Projeye Git > Yeni Öğe ekle veya varolan kod dosyalarını projeye eklemek için Proje > Var Olan Öğeyi Ekle adımlarını izleyin
//   6. Bu projeyi daha sonra yeniden açmak için Dosya > Aç > Proje'ye gidip .sln uzantılı dosyayı seçin