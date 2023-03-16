#define _CRT_SECURE_NO_WARNINGS


#include <time.h>
#define LEN 256

#include<iostream>
#include<windows.h>
#include<fstream>


#include<direct.h> // for getting the directory

using namespace std;

int main()
{
	char d;  char c;

	char trial[] = "substitution.exe aaaa ";

	//   char trial2[]="simulHeurist.exe aaaa ";
	char trial2[] = "simulLTnoOPT.exe aaaaa";

	char trial3[] = "MarkocModulated.exe aaaa ";

	char trial4[] = "simulLTnoOPTintermittent.exe aaaaa";

	char trial5[] = "regulardualindex.exe aaaaa";

	char trial6[] = "ZulalCode.exe aaaaa";


	int i, j, x, y, z, v;
	int maxindex, minindex;

	cout << "ENTER YOUR STARTING INDEX: ";
	std::cin >> minindex;
	cout << "ENTER YOUR ENDING INDEX: ";
	std::cin >> maxindex;

	//maxindex=144;

	char cur_time[128];

	time_t      t;
	struct tm* ptm;

	t = time(NULL);
	ptm = localtime(&t);
	strftime(cur_time, 128, "%d-%m-%Y %H:%M:%S", ptm);

	ofstream logfile;
	logfile.open("Logfile.txt", ios::out | ios::app);
	logfile << "\n\n**************************************************************************\n";
	logfile << cur_time << "\n";
	// logfile<<"Sheopuri's Modified-newruns Index: "<<minindex<<"-"<<maxindex<<endl;
	// cout<<"Sheopuri's Modified-newruns Index: "<<minindex<<"-"<<maxindex<<endl;
	logfile << "Intermittent Demand Index: " << minindex << "-" << maxindex << endl;
	cout << "Intermittent Demand Index: " << minindex << "-" << maxindex << endl;
	
	char cCurrentPath[MAX_PATH];
	logfile << "Path of the file: " << _getcwd(cCurrentPath, sizeof(cCurrentPath)) << endl;

	logfile.close();
	

	/*
	FILE* fp;

	//open the file for writing
	fp = fopen("Logfile.txt", "w");

	//write time as text into the file stream
	char cur_time[128];

	time_t      t;
	struct tm* ptm;

	t = time(NULL);
	ptm = localtime(&t);

	strftime(cur_time, 128, "%Y-%m-%d %H:%M:%S", ptm);

	fprintf(fp, "%s\n", cur_time);

	//close the file
	fclose(fp);
	*/

	//	trial4[30] = (char)z;
	for (i = minindex; i <= maxindex; i++)
	{
		v = (i / 10000) % 10;
		z = (i / 1000) % 10;
		y = (i / 100) % 10;
		x = (i / 10) % 10;
		j = i % 10;

		//	 cout<<v<<z<<y<<x<<j<<endl;
		//	 cin>>c;
		v += 97;
		j += 97;
		y += 97;
		x += 97;
		z += 97;

		trial[17] = (char)z; trial[18] = (char)y; trial[19] = (char)x; trial[20] = (char)j;
		trial2[17] = (char)v; trial2[18] = (char)z; trial2[19] = (char)y; trial2[20] = (char)x; trial2[21] = (char)j;
		trial3[20] = (char)z; trial3[21] = (char)y; trial3[22] = (char)x; trial3[23] = (char)j;
		trial4[29] = (char)v; trial4[30] = (char)z; trial4[31] = (char)y; trial4[32] = (char)x; trial4[33] = (char)j;
		trial5[21] = (char)v; trial5[22] = (char)z; trial5[23] = (char)y; trial5[24] = (char)x; trial5[25] = (char)j;
		trial6[14] = (char)v; trial6[15] = (char)z; trial6[16] = (char)y; trial6[17] = (char)x; trial6[18] = (char)j;

		/*	 cout<<i<<" "<<trial<<endl;
			 logfile.open("Logfile.txt",ios::out|ios::app);
			 logfile<<"\n===================================================================================\n";
			 logfile<<endl<<trial<<endl;
			 logfile.close();
		//     system(trial);
		*/

		cout << i << " " << trial6 << endl;
		logfile.open("Logfile.txt", ios::out | ios::app);
		logfile << trial6 << endl;
		logfile.close();
		//    system(trial);
		system(trial6);
	}



	std::cin >> c;

	return 0;
}
