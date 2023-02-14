#include<iostream>
#include<fstream>
#include<time.h>
#include<iomanip>
#include<windows.h>
#include<string.h>
#include<sstream>
#include"boncuk.h"
#include<stdio.h>
#include<cmath>
#include"sqlite3.h"
#include<thread>
#include<vector>
//using namespace std;


const int maxmarketcap=4;
const int horizon=24;  
const int regsupcap=25;  
const float lambda=2;
const int acqr=100;int acqs=60;
const float holdingr=0.1*acqr;
const float holdings=0.06*acqr;
const float backlog=2*acqr;
const float subst=1*acqr;
const float discountrate=0.95;
const float epsilon=0.0001;
const int thresholdmatsize= 1073741824;


const int LTreg=3;
const int LTmark=0;
const int statelimittime=2*LTreg;



void setKprimemat(double *pointKprime[],int dim);
void calculatebounds(int *pbounds[], int horizon, int Dmax, int regsupcap, int maxmarketcap,int LTreg);
void calculaterowsizes(int *prowsizes[],int *pbounds[]);
std::string generatestatecode(int row, int time, int *pbounds[],int *prows[],int &highqinv,int &lowqinv,int &qrtdeliv,int &K,std::string &conststcd,int outstandord[]);  
void generateactspacebounds(int *pactbounds[],int *pactboundsL[],int *pbounds[], int Dmax,int regscap,int maxmarketcap);
static int callback(void *NotUsed, int argc, char **argv, char **azColName);  //SQLITE FUNCTION FOR ERRORS
void matrixcheckint (int *pointvect[], int dim1, int dim2);
void matrixcheckfloat (float *pointvect[], int dim1, int dim2, int vstart=0,int istart=0);
float matrixmin (float *pontmat[], int& optv, int& optw,int limrw,int limvect);
void matrixoutputfloat(float *pointvect[], int dim1, int dim2, std::ofstream&fnm);
void write2database(float *pmatrix[],int rowdim,int coldim,int time,bool flg,bool databaseflg);


int main()
{

cout<<"Starting parameters: horizon="<<horizon<<" market capacity="<<maxmarketcap<<" supplier capacity="<<regsupcap<<" supplier lead time="<<LTreg<<endl;
cout<<"backlog="<<backlog<<" substitution="<<subst<<" acqusition costs=("<<acqr<<","<<acqs<<"), holding costs=("<<holdingr<<","<<holdings<<")"<<endl;


	double Kprimemat[maxmarketcap+1][maxmarketcap+1];
	double *pKprimemat[maxmarketcap+1];
	int bounds[4][horizon];
	int rowsizes[LTreg-1+3][horizon];
	int *prowsizes[LTreg-1+3];
	int *pbounds[4];
	int STrowsize,STcolumnsize,STcolumnsizeAct;
	int Dmax,j,k,l,ind,K;
	unsigned int reqmatsize,matsize,rownum;
	int thresholdtime=0;
	int totstatesize;
	char c;
	int t,repeat;
	float statevect[LTreg+3];
	float minval,temp;
	int optv,optw;
	int v,w,x,state,z;
	int ystrt,qrtdeliv;
	bool flg=0;
	int m,i,n;
	ofstream prevmat,prevmatpos,file1,logfile;
	
	 char header[200];
	char filename[200];	
	char oldname[]="testLT.db";	


n=sprintf(header,"Starting parameters:horizon=%d, supplierLT=%d, market capacity=%d, supplier capacity=%d, backlog=%.1f, substitution=%.1f, acqusition costs=(%d,%d), holding costs=(%.1f,%.1f),lambda=%f",horizon,LTreg,maxmarketcap,regsupcap,backlog,subst,acqr,acqs,holdingr,holdings,lambda);

	int outstandord[LTreg-2];
	
	time_t timer;
	struct tm * timeinfo;
	  
	time(&timer);
	timeinfo=localtime(&timer);
		
	strftime(filename,200,"outputfile_LT_DB_Optimal_%d%m%Y_%H%M%S.db",timeinfo);

	logfile.open("Logfile.txt",ios::out|ios::app);
	 logfile<<"\n\n===================================================================================<BR>\n";
	 logfile<<"Algorithm OPTIMAL_LT_DB is started. Time: "<<asctime(timeinfo)<<"<BR>\n";
	 logfile<<header<<"<BR>"<<endl;
	 logfile<<"backlog="<<backlog<<" substitution="<<subst<<" acqusition costs=("<<acqr<<","<<acqs<<"), holding costs=("<<holdingr<<","<<holdings<<")<BR>"<<endl;
	 logfile.close();


	sqlite3 *db;	
	//sqlite3_stmt *res3;
	
	const char *errMSG;
	const char *tail;
	char *zErrMsg = 0;


	std::vector<std::thread>threads(horizon);


	
	for(i=0;i<LTreg-1+3;i++)
		prowsizes[i]=&rowsizes[i][0];
	
	for(i=0;i<maxmarketcap+1;i++)
		pKprimemat[i]=&Kprimemat[i][0];
    	
    
	setKprimemat(pKprimemat,maxmarketcap+1);
	Dmax=maxdemand(epsilon,lambda);
	cout<<"Dmax :"<<Dmax<<endl;
	pbounds[0]=&bounds[0][0];
	pbounds[1]=&bounds[1][0];
	pbounds[2]=&bounds[2][0];
	pbounds[3]=&bounds[3][0];
	calculatebounds(pbounds,horizon,Dmax,regsupcap,maxmarketcap,LTreg);
	matrixcheckint(pbounds,4,horizon);//cin>>c;
	calculaterowsizes(prowsizes,pbounds);
	matrixcheckint(prowsizes,LTreg-1+3,horizon);
	
	int sizeofactHbound=(int)rowsizes[LTreg+1][horizon-1];
	int sizeofactLbound=(int)bounds[LTreg-1][horizon-1]+1;
	
	float test2;
	
	
	int actboundsH[sizeofactHbound][3];
	int *pactboundsH[sizeofactHbound];
	
	int acboundsL[sizeofactLbound][3];
	int *pactboundsL[sizeofactLbound];
	                             
	for(i=0;i<sizeofactLbound;i++)
		pactboundsL[i]=&acboundsL[i][0];
	
	for(i=0;i<sizeofactHbound;i++)
		pactboundsH[i]=&actboundsH[i][0];
	
	generateactspacebounds(pactboundsH,pactboundsL,pbounds,Dmax,regsupcap,maxmarketcap);
	//matrixcheckint(pactboundsH,sizeofactHbound,3);
	//matrixcheckint(pactboundsL,sizeofactLbound,3);cin>>c;
	
	//int error=sqlite3_open("testLT.db",&db);  

	//DATABASE PARAMETERS.
	//	sqlite3_exec(db, "PRAGMA cache_size = 20000", NULL, NULL, &zErrMsg);  //NUMBER OF PAGES IN THE MEMERY
	//	sqlite3_exec(db, "PRAGMA synchronous = OFF", NULL, NULL, &zErrMsg);  
	//	sqlite3_exec(db, "PRAGMA journal_mode = MEMORY", NULL, NULL, &zErrMsg);

	//IS DATABASE OPENED?
	//	if(error)
	//	{
	//	std::cout<<"Could not open DB"<<std::endl;
	//		sqlite3_close(db);
	//	system("pause");
	//		return 0;
	//	}

       

	
		
	//REMOVE
	STrowsize=rowsizes[0][horizon-1];

	float **ALLSTATES=new float*[STrowsize];
	float **pAllstates=new float*[STrowsize];
	
	float **ALLSTATESprev=new float*[STrowsize];
	float **pAllstatesprev=new float*[STrowsize];
       
	//	float **dumbp=new float*[STrowsize];
	//float **pdumbp=new float*[STrowsize];

	for(i=0;i<STrowsize;i++)
	{
		ALLSTATES[i]= new float[LTreg+5];  //LTreg-1 for outstanding and 3 for other states, 1 for cost
	      ALLSTATESprev[i]=new float[LTreg+5];
	      //	dumbp[i]=new float[LTreg+5];

	    pAllstates[i]=&ALLSTATES[i][0];
	    pAllstatesprev[i]=&ALLSTATESprev[i][0];
	    //	    pdumbp[i]=&dumbp[i][0];
	}
	
	double	testd;	
	int  singlerows=(bounds[1][horizon-1]-bounds[0][horizon-1]) + regsupcap;
	int  singlecolumns=bounds[2][horizon-1]+maxmarketcap;
	float SINGLEPERMAT[singlerows+1][singlecolumns+1];
	float *psinglepermat[singlerows+1];
	int test,dim1,dim2,ymin;	
	float ACTIONSPACE[regsupcap+1][maxmarketcap+1]; 
	float *pACTIONSPACE[regsupcap+1];
	for(i=0;i<regsupcap+1;i++)
		pACTIONSPACE[i]=&ACTIONSPACE[i][0];
	
	std::cout<<"Starting calculation of single period costs..."<<endl;
	int y=bounds[0][horizon-1];
	ystrt=y;
	for(i=0;i<=singlerows;i++)   //singlerows
		{
	 	for(j=0;j<=singlecolumns;j++)   //singlecolumns
	 		{
	 		 SINGLEPERMAT[i][j]=singlepersubstitute(y,j,lambda,holdingr,holdings,subst,backlog,acqr,acqs);
	 		}
	    psinglepermat[i]=&SINGLEPERMAT[i][0];
	    y=y+1;
	    }
	std::cout<<"Single Period Costs are calculated!"<<endl;
  logfile.open("Logfile.txt",ios::out|ios::app);
  logfile<<"Single Period Costs are calculated! Time: "<<asctime(timeinfo)<<"<BR>"<<endl;
  logfile.close();

	 
	 //
	// file1.open("singlerpermat.txt");
	//	matrixcheckfloat (psinglepermat, singlerows, singlecolumns);
	//	cin>>c;
	//matrixoutputfloat(psinglepermat, singlerows, singlecolumns, file1);
	// file1.close();
	 
	
	int maxy=bounds[2][horizon-1]+bounds[1][horizon-1]+ regsupcap+maxmarketcap;
	int ind1,ind2,upH,lowH,upL,lowL;int f,g,s,u;
	int stsize1,stsize2,stsize3,stsize4,stsize5;
	int index1_1,index1_2,index1_3,block,indexs;
	float costprevper,costcontr,contr;
	float approxconst;
	int ylowerbound,lowermarkcap,lowerstatebound;


	int dene,rownumprev;
	bool breakingflg,threadflg=0;
	
	float poisvect[maxy];
	for(i=0;i<maxy;i++)
		poisvect[i]=(float)poispdf(i,lambda)*discountrate;
	std::cout<<"Probability matrix is built. Vector Size="<<maxy<<endl;

	std::ostringstream tablecode,statecode,statecode2,sqlcode,sqlcoderead,indexcode;
	std::string query,queryread,stcd,stcd2,conststcd;
	


		
	//	//PREPARE SQLCODE. LATER WE BIND THEM WITH PARAMETERS TO BE REPLACED QUESTION MARKS
	//	sqlcode<<"INSERT INTO periods (rownum,time,stateid,cost,optL,optH) VALUES(?,?,?,?,?,?);";
	//	query=sqlcode.str();
	//	error=sqlite3_prepare_v2(db,query.c_str(),query.length(),&res2,&tail);

	
		
	//START THE LOOP	
	//t=horizon-1;
	t=horizon;
		
do
{
	stsize1=rowsizes[LTreg][t];  //THESE COEFFICIENTS ARE USED TO SPOT STATE POSITIONS FOR T+1
	stsize2=rowsizes[LTreg-1][t];
	stsize3=rowsizes[LTreg-2][t];
	stsize4=bounds[1][t]+1;
	stsize5=bounds[1][t]-bounds[0][t]+1;
	lowerstatebound=bounds[0][t];
	lowermarkcap=bounds[3][t];
       


	rownum=rowsizes[0][t-1];



	for(i=0;i<rownum;i++)  //27
	{
		//GENERATE STATECODE FROM THE ROW NUMBER OF A STATE
		stcd=generatestatecode(i,t,pbounds,prowsizes,y,x,qrtdeliv,K,conststcd,outstandord);

		//if((t==horizon-1)&&(i>=5300000))
		//	cout<<"Row: "<<i<<" "<<stcd.c_str()<<endl;
		

		ind=y-ystrt;
		index1_1=y-bounds[0][t];
		index1_2=y;
		index1_3=0-bounds[0][t];
		block=outstandord[0]*stsize2;
		
		
		
		if(t==horizon)
		{
			for(j=0;j<=regsupcap;j++)//ORDER QUANTITY TO REGULAR SUPPLIER
			{
				for(k=0;k<=K;k++)  //ORDER QUANTITY TO MARKET
				{
					w=x+k;
					v=y+j;
					test=SINGLEPERMAT[ind][w] + j*acqr + k*acqs;
					ACTIONSPACE[j][k]=test;
				}
			}
			dim1=regsupcap+1;
			dim2=K+1;
		}
		else
		{
			
				ylowerbound=bounds[0][t];
				ind1=y-ystrt;
				
				if(t<horizon-LTreg)
					upH=actboundsH[ind1][1];
				else
					upH=0;
					
				
				lowH=actboundsH[ind1][2];	
				dim1=lowH-upH+1;
				ind2=x;
				upL=acboundsL[ind2][1];
				lowL=min(acboundsL[ind2][2],K);
				dim2=lowL-upL+1;			
			
			for(l=max(K-1,0);l<=min(maxmarketcap,K+1);l++)
			{
				f=0;
				for(j=upH;j<=lowH;j++)//ORDER QUANTITY TO REGULAR SUPPLIER
				{
					g=0;
					for(k=upL;k<=lowL;k++)  //ORDER QUANTITY TO MARKET  //min(lowL,K)
					{
						w=x+k;
						if(l==max(K-1,0))   // DO THIS ONLY ONCE!
						{
							costprevper=SINGLEPERMAT[ind1][w] + j*acqr + k*acqs;
//							if((t==horizon-1)&&(i==0)&&(j==upH)&&(k==upL))
//								cout<<costprevper<<endl;
						
							ACTIONSPACE[f][g]=costprevper;
						}
						
						costcontr=0;
						if(y<0)
						{
							if(qrtdeliv+y+w<=0)
							{
								indexs=(l-lowermarkcap)*stsize3+block+j*stsize1 + index1_1+w+qrtdeliv;
								for(s=0;s<=Dmax;s++)
								{
								  u=indexs-s;
					//				test2=ALLSTATESprev[u][LTreg+2];
					//				test2=poisvect[s];
					//				test2=Kprimemat[K][l];
									if(y+w+qrtdeliv-s<ylowerbound)
									{
										temp=0;
										if(t<horizon-1)
											approxconst=(1-(float)pow((double)discountrate,(double)horizon-t+1))/(1-discountrate)-1-discountrate;
										else
										{
											approxconst=(1-(float)pow((double)discountrate,(double)horizon-t+1))/(1-discountrate)-1;
											temp=singlepersubstitute(y+(horizon-t+1)*w+qrtdeliv-s,0,lambda,holdingr,holdings,subst,backlog,acqr,acqs)*approxconst;
										}
										
										for(m=0;((m<LTreg-1)&&(t<horizon-1));m++)
										{
											if(m>=1)
												temp+=singlepersubstitute(y+(horizon-t+1+m)*w+outstandord[m-1]+qrtdeliv-s,0,lambda,holdingr,holdings,subst,backlog,acqr,acqs)*approxconst;
											else
												temp+=singlepersubstitute(y+(horizon-t+1)*w+qrtdeliv-s,0,lambda,holdingr,holdings,subst,backlog,acqr,acqs)*discountrate;
										}
										temp*=poispdf(s,lambda)*Kprimemat[K][l];
										costcontr+=temp;
									}									
									else
										costcontr+=ALLSTATESprev[u][LTreg+2]*poisvect[s]*Kprimemat[K][l];
								}
				
							}
							else
							{
								if((y+qrtdeliv)<=0)
								{
									indexs=(l-lowermarkcap)*stsize3+block+j*stsize1+(y+qrtdeliv+w-1)*stsize4 + stsize5;
								//	cout<<indexs<<endl;
									for(s=0;s<=y+qrtdeliv+w;s++)
									{
										//indexs=l*stsize3+block+j*stsize1+(y+qrtdeliv+w-1)*stsize4 + stsize5 ;
										indexs=(l-lowermarkcap)*stsize3+block+j*stsize1+(y+qrtdeliv+w-1-s)*stsize4 + stsize5;
										costcontr+=ALLSTATESprev[indexs][LTreg+2]*poisvect[s]*Kprimemat[K][l];
									}
									for(s=y+qrtdeliv+w+1;s<=Dmax+y+qrtdeliv+w;s++)
									{
										indexs--;
										costcontr+=ALLSTATESprev[indexs][LTreg+2]*poisvect[s]*Kprimemat[K][l];
			//							cout<<"for2 optH="<<j<<" optL="<<k<<" s="<<s<<" index="<<indexs<<endl;
									}
								}
								else
								{
									//indexs=l*stsize3+block+j*stsize1 + index1_3+qrtdeliv+y + ((y+qrtdeliv)+w-1)*stsize4;
									indexs=(l-lowermarkcap)*stsize3+block+j*stsize1 + index1_3+qrtdeliv+y + w*stsize4;
			//						cout<<indexs<<endl;;
									for(s=0;s<=qrtdeliv+y;s++)
									{
										u=indexs-s;
										costcontr+=ALLSTATESprev[u][LTreg+2]*poisvect[s]*Kprimemat[K][l];
				//						cout<<"for3 optH="<<j<<" optL="<<k<<" s="<<s<<" index="<<u<<endl;										
									}
									for(s=qrtdeliv+y+1;s<=(y+qrtdeliv)+w;s++)
									{
										u=(l-lowermarkcap)*stsize3+block+j*stsize1 + index1_3 + ((y+qrtdeliv)+w-s)*stsize4;
										//indexs=l*stsize3+block+j*stsize1 + index1_3 + ((y+qrtdeliv)+w-s)*stsize4;
										costcontr+=ALLSTATESprev[u][LTreg+2]*poisvect[s]*Kprimemat[K][l];
				//						cout<<"for4 optH="<<j<<" optL="<<k<<" s="<<s<<" index="<<u<<endl;
									}
									
									for(s=(y+qrtdeliv)+w+1;s<=(y+qrtdeliv)+w+Dmax;s++)
									{
										u--;
										//u=s;//(y+qrtdeliv)+w-1+s;
										contr=ALLSTATESprev[u][LTreg+2]*poisvect[s]*Kprimemat[K][l];
										costcontr+=contr;
				//						cout<<"for5 optH="<<j<<" optL="<<k<<" s="<<s<<" index="<<u<<endl;
										if(contr<epsilon*0.01)
											break;
									}
									
									//if((l==1)||(l==0))  //||(l==0)
				//					cin>>c;

								}
							}
						}
						else
						{
							breakingflg=0;
							indexs=(l-lowermarkcap)*stsize3+block+j*stsize1 + index1_2 + (w-1)*stsize4 +stsize5+ qrtdeliv;						
							
							for(s=0;(s<qrtdeliv+y);s++)
							{
								u=indexs-s;
		//						test2=ALLSTATESprev[u][LTreg+2];
		//						test2=poisvect[s];
		//						test2=Kprimemat[K][l];
								contr=ALLSTATESprev[u][LTreg+2]*poisvect[s]*Kprimemat[K][l];
								costcontr+=contr;
								if((s>=3*lambda)&&(contr<=epsilon*0.1))
								{
									breakingflg=1;break;
								}
				//				cout<<"for6 optH="<<j<<" optL="<<k<<" s="<<s<<" index="<<u<<endl;
							}
							for(s=0;((s<=w)&&(breakingflg!=1));s++)
							{
								indexs=(l-lowermarkcap)*stsize3+block+j*stsize1 + (w-1-s)*stsize4 +stsize5;
								u=(y+qrtdeliv)+s;
			//					test2=ALLSTATESprev[indexs][LTreg+2];
			//					test2=poisvect[u];
			//					test2=Kprimemat[K][l];
								costcontr+=ALLSTATESprev[indexs][LTreg+2]*poisvect[u]*Kprimemat[K][l];
				//				cout<<"for7 optH="<<j<<" optL="<<k<<" s="<<u<<" index="<<indexs<<endl;
							}
							for(s=1;((s<=Dmax)&&(breakingflg!=1));s++)
							{
								indexs--;
								u=(y+qrtdeliv)+w+s;
								test2=ALLSTATESprev[indexs][LTreg+2];
								test2=poisvect[u];
								test2=Kprimemat[K][l];

								contr=ALLSTATESprev[indexs][LTreg+2]*poisvect[u]*Kprimemat[K][l];
								costcontr+=contr;
					//			cout<<"for8 optH="<<j<<" optL="<<k<<" s="<<u<<" index="<<indexs<<endl;
								if(contr<epsilon*0.01)
									break;
							}
							//if(l==1)
					//			cin>>c;							
						}
						
						
//						test2=ACTIONSPACE[f][g];
						ACTIONSPACE[f][g]+=costcontr;
						g++;
					}
					f++;
				}
			}		
		}
	

//		if((t==horizon-1)&&(i==395))
//		{
//			cout<<"Row: "<<i<<" "<<stcd.c_str()<<endl;
//			matrixcheckfloat (pACTIONSPACE, dim1, dim2);
//			cin>>c;
//		}
		minval= matrixmin (pACTIONSPACE,optv,optw,dim1,dim2);
		
		ALLSTATES[i][0]=K;
		ALLSTATES[i][1]=qrtdeliv;
		for(j=1;j<=LTreg-2;j++)
			ALLSTATES[i][j+1]=outstandord[0];  //THIS WILL BREAK DOWN FOR LT<3
		ALLSTATES[i][LTreg]=x;
		ALLSTATES[i][LTreg+1]=y;
		ALLSTATES[i][LTreg+2]=minval;
		ALLSTATES[i][LTreg+3]=(float)optw;
		ALLSTATES[i][LTreg+4]=(float)optv;

		
		//sqlite3_bind_int(res2,1,i);
		//sqlite3_bind_int(res2,2,t);		//BIND CURRENT MARKET CAPACITY TO WRITE QUERY (res2)
		//sqlite3_bind_text(res2,3,stcd.c_str(),-1,NULL);  //BIND CURRENT STATE INFOR TO WRITE QUERY (res2)
		//sqlite3_bind_double(res2,4,(double)minval);		//BIND MINMUM COST OF STATE TO QRITE QUERY (res2)
		//sqlite3_bind_int(res2,5,optw);
		//sqlite3_bind_int(res2,6,optv);		
		//sqlite3_step(res2);								//EXECURE WRITING (res2)

		if((i%200000)==0)
		{
		  time(&timer);
		  timeinfo=localtime(&timer);
		  std::cout<<"  State# "<<setw(6)<<i<<" complete!. Total State# "<<rownum<<" Time: "<<asctime(timeinfo);
		}

		//    	logfile.open("Logfile.txt",ios::out|ios::app);
		//	if((logfile.is_open())&&((i%400000)==0))
		//{
    		//	logfile<<"\tState# "<<setw(6)<<i<<" complete!. Tot.State# "<<rownum<<" Time "<<asctime(timeinfo);
    		//	logfile.close();
    		//}



			
		//sqlite3_clear_bindings(res2);					//CLEAR BINDINGS OF WRITE QUERY
		//sqlite3_reset(res2);							//RESER BINDINGS OF WRITE QUER		
	}
	time(&timer);
	timeinfo=localtime(&timer);
	std::cout<<"Time "<<t<<" is complete. Time: "<<asctime(timeinfo);
	// 	end=clock();
 	logfile.open("Logfile.txt",ios::out|ios::app);
	logfile<<"Time: "<<t<<" is completed! Total State:"<<rownum<<" Time: "<<asctime(timeinfo)<<"<BR>"<<endl;
	logfile.close();



	if(t<horizon)
	  {
	    std::cout<<"Waiting for previous period's database thread"<<std::endl;
	    threads.at(t).join();
	  }

	std::cout<<"Transferring matrices..."<<endl; 
	for(i=0;i<rownum;i++)
	{
	  for(j=0;j<LTreg+5;j++)
	    {
       		ALLSTATESprev[i][j]=ALLSTATES[i][j];
		//		dumbp[i][j]=ALLSTATES[i][j];
	    }
      }
	
	rownumprev=rownum;	
	
	threads.at(t-1)=std::thread(write2database,pAllstatesprev,rownumprev,LTreg+5,t,0,1);

	//threads.push_back(std::thread(write2database,pdumbp,rownumprev,LTreg+5,t,0));

	//std::thread mm(write2database,pdumbp,rownumprev,LTreg+5,t,0);
	//mm.detach();

	//	threadflg=1;	
	//
	//	mm.join();

	//	for(i=0;i<rownum;i++)       
	//	  {
	//	    delete [] dumbp[i];
	//	    dumbp[i]=0;
	//	  }
	//	 delete [] dumbp;
	//	 delete [] pdumbp;

		
	
	time(&timer);
	timeinfo=localtime(&timer);
	cout<<"Matrix is transferred. Time: "<<asctime(timeinfo)<<endl;
	
	t--;
}
while(t>=1);

 threads.at(0).join();

//CALCULATE FOR t=0
t=0;
K=maxmarketcap;x=0;y=0;
ind=0;
index1_1=y-bounds[0][1];
index1_2=y;
index1_3=0-bounds[0][1];
block=0;qrtdeliv=0;

stsize1=rowsizes[LTreg][0];  //THESE COEFFICIENTS ARE USED TO SPOT STATE POSITIONS FOR T+1
stsize2=rowsizes[LTreg-1][0];
stsize3=rowsizes[LTreg-2][0];
stsize4=bounds[1][0]+1;
stsize5=bounds[1][0]-bounds[0][t]+1;
lowermarkcap=bounds[3][0];

f=0;
for(j=0;j<=regsupcap;j++)//ORDER QUANTITY TO REGULAR SUPPLIER
{
	g=0;
	for(k=0;k<=K;k++)  //ORDER QUANTITY TO MARKET  //min(lowL,K)
	{
		w=x+k;
		costprevper=SINGLEPERMAT[ind1][w] + j*acqr + k*acqs;
		ACTIONSPACE[f][g]=costprevper;
		costcontr=0;

		for(l=max(K-1,0);l<=min(maxmarketcap,K+1);l++)
		{
			indexs=(l-lowermarkcap)*stsize3+block+j*stsize1 + index1_2 + (w-1)*stsize4 +stsize5+ qrtdeliv;			
			for(s=0;(s<qrtdeliv+y);s++)
			{
				u=indexs-s;
				costcontr+=ALLSTATES[u][LTreg+2]*poisvect[s]*Kprimemat[K][l];
//				cout<<"for6 optH="<<j<<" optL="<<k<<" s="<<s<<" index="<<u<<endl;
			}
			for(s=0;s<=w;s++)
			{
				indexs=(l-lowermarkcap)*stsize3+block+j*stsize1 + (w-1-s)*stsize4 +stsize5;
				u=(y+qrtdeliv)+s;
				costcontr+=ALLSTATES[indexs][LTreg+2]*poisvect[u]*Kprimemat[K][l];
//				cout<<"for7 optH="<<j<<" optL="<<k<<" s="<<u<<" index="<<indexs<<endl;
			}
			for(s=1;s<=Dmax;s++)
			{
				indexs--;
				u=(y+qrtdeliv)+w+s;
				costcontr+=ALLSTATES[indexs][LTreg+2]*poisvect[u]*Kprimemat[K][l];
//			cout<<"for8 optH="<<j<<" optL="<<k<<" s="<<u<<" index="<<indexs<<endl;
				if(costcontr<epsilon*0.01)
					break;
			}
		}
		ACTIONSPACE[f][g]+=costcontr;
		g++;
	}
	f++;
}
minval= matrixmin (pACTIONSPACE,optv,optw,regsupcap+1,K+1);



int error=sqlite3_open("testLT.db",&db);


tablecode.str("");
tablecode<<"CREATE TABLE period0 (rownum INT,time INT, stateid CHAR(20) PRIMARY KEY, cost DECIMAL(5,4),optL INT,optH INT);";
query=tablecode.str();
sqlite3_exec(db,query.c_str(),callback,0,&zErrMsg);



sqlcode.str("");
sqlcode<<"INSERT INTO period0 (rownum,time,stateid,cost,optL,optH) VALUES(0,"<<0<<","<<"'0_4_0_0_0_0',"<<minval<<","<<optw<<","<<optv<<");";
query=sqlcode.str();
 
sqlite3_exec(db,query.c_str(),callback,0,&zErrMsg);
sqlite3_exec(db, "PRAGMA query_only = TRUE", NULL, NULL, &zErrMsg);


//for(i=1;i<horizon;i++)
//{
//   sqlcode.str("");
//   sqlcode<<"CREATE INDEX ind"<<i<<" ON periods (time="<<i<<",rownum);";
//   query=sqlcode.str();
//   sqlite3_exec(db,query.c_str(),callback,0,&zErrMsg);
//}



sqlite3_close(db);

cout<<"Time 0 is complete. Time: "<<asctime(timeinfo);
 	logfile.open("Logfile.txt",ios::out|ios::app);
	logfile<<"Time: 0 is completed! Time: "<<asctime(timeinfo)<<"<BR>"<<endl;
	logfile<<"Algorithm is complete. Results are written to"<<filename<<"<BR>"<<endl;
	logfile.close();


	//matrixcheckfloat(pAllstates,600,LTreg+3); 
	sqlcode.str("");					//CLEAR SQL CODE FOR WRITING 
	tablecode.str("");					//CLEAR SQL CODE FOR CREATING TABLE

	n=rename(oldname,filename);

	cout<<"quiting...\a"<<endl;
	//	cin>>c;	

			//	
		
		//
		//	

	
	
	for(i=0;i<STrowsize;++i)
	{
	      delete [] ALLSTATES[i];
	      delete [] ALLSTATESprev[i];
	      //	      delete [] dumbp[i];
	      ALLSTATES[i]=0;
	      ALLSTATESprev[i]=0;
	      //		dumbp[i]=0;
	}

	 delete [] ALLSTATES;
	 delete [] ALLSTATESprev;
	 delete [] pAllstates;
	 delete [] pAllstatesprev;
	 //	 delete [] dumbp;
	 //	 delete [] pdumbp;



	
return 0;

}

void calculatebounds(int *pbounds[], int horizon, int Dmax, int regsupcap, int maxmarketcap,int LTreg)
{
  int i,j;
  int up=0;int low=0;int lowmark=0;int markcap=0;
  int *pchng;

  pchng=pbounds[0];
  *pchng=up-Dmax;
  pchng=pbounds[1];
  *pchng=low+regsupcap*0;  // NO DELIVERY WILL HAPPEN IN PERIOD 1
  pchng=pbounds[2]; 
  *pchng=lowmark+maxmarketcap;
  pchng=pbounds[3]; 
  *pchng=max(maxmarketcap-1,0);

  for(i=1;i<horizon;i++)
    {
	  if(i<statelimittime)
	  {
    	  pchng=pbounds[0]+(i-1);
	      up=*pchng;
	      pchng=pchng+1;
	      *pchng=up-Dmax;

	      pchng=pbounds[1]+(i-1);
	      low=*pchng;
	      pchng=pchng+1;
	      j=min((i+1)/LTreg,1);  //THIS FORMULATION LOOKS COMPLICATED. KEEP IN MOND THAT WE KEEP ADDING REGSUPCAP TO LOW IN EACH ITERATION 
	      *pchng=low+regsupcap*j;
	  }
      else
      {
    	  pchng=pbounds[0]+(i-1);
    	  up=*pchng;
    	  pchng=pchng+1;
    	  *pchng=up;

    	  pchng=pbounds[1]+(i-1);
    	  low=*pchng;
    	  pchng=pchng+1;
    	  *pchng=low;
    	  
       }

      if(i<statelimittime)
      {
    	  pchng=pbounds[2]+(i-1);
          lowmark=*pchng;
          pchng=pchng+1;
          *pchng=lowmark+maxmarketcap;
      }
      else
      {
    	  pchng=pbounds[2]+(i-1);
    	  lowmark=*pchng;
    	  pchng=pchng+1;
    	  *pchng=lowmark;
      }
      
      pchng=pbounds[3]+(i-1);
      markcap=*pchng;
      pchng=pchng+1;
      *pchng=max(markcap-1,0);
    }
}

void setKprimemat(double *pointKprime[],int dim)
{
  int i,j;
  double *pointchng;

  for(i=0;i<dim;i++)
    {
      for(j=0;j<dim;j++)
	{
	  pointchng=pointKprime[i]+j;
	  if(i==0)
	    {
	      if((j==i)||(j==i+1))
		*pointchng=0.5;                     //Kprimemat[i][j]=0.5;
	      else
		*pointchng=0;                         //Kprimemat[i][j]=0;
	    }
	  else if(i==dim-1)
	    {
	      if((j==i)||(j==dim-2))
		*pointchng=0.5;                     //		  Kprimemat[i][j]=0.5;
	      else
		*pointchng=0;                         //		Kprimemat[i][j]=0;
	    }
	  else
	    {
	      if((j!=i-1)&&(j!=i)&&(j!=i+1))
		*pointchng=0;                          //		Kprimemat[i][j]=0;
	      else
		{
		  pointchng=pointKprime[i]+i-1;
		  *pointchng=0.25;
		  pointchng=pointKprime[i]+i;
		  *pointchng=0.5;
		  pointchng=pointKprime[i]+i+1;
		  *pointchng=0.25;
		};
	    };
	}
    }
}

void calculaterowsizes(int *prowsizes[],int *pbounds[])
{
	int i=0;
	int up,down,maxmark,markcap;
	int *pchange;
	int temp,temp2;
	double poweredtemp;
	int t=0;
	
	for(t=0;t<horizon;t++)
	{
		pchange=pbounds[0] + t;
		up=*pchange;
		pchange=pbounds[1] + t;
		down=*pchange;

		temp=down-up+1;
		pchange=prowsizes[LTreg+2-1]+ t;
		*pchange=down-up+1;
		pchange=pbounds[2] + t;
		maxmark=*pchange;
		pchange=prowsizes[LTreg+2-2] + t;
	
		temp=(down-up+1)*(maxmark+1);
		*pchange=((down-up+1)+(down+1)*maxmark);
			
		for(i=0;i<LTreg-1;i++)
		{
			pchange=prowsizes[LTreg+2-3-i]+t;
			poweredtemp=pow((double)regsupcap+1,i+1);

			temp=((down-up+1)+(down+1)*maxmark)*(int)poweredtemp;
			*pchange=((down-up+1)+(down+1)*maxmark)*(int)poweredtemp;		
		}
		pchange=pbounds[3] + t;
		markcap=*pchange;
		markcap=(maxmarketcap-markcap+1);
	
		pchange=prowsizes[0]+t;
		poweredtemp=pow((double)regsupcap+1,LTreg-1);
		temp=(down-up+1)*(maxmark+1)*(int)poweredtemp*markcap;
		temp2=((down-up+1)+(down+1)*maxmark)*markcap*(int)poweredtemp;
		*pchange=temp2;
		//cout<<"Old States: "<<temp<<"New States: "<<temp2<<endl;
	}
	

}



void matrixcheckint (int *pointvect[], int dim1, int dim2)
{
  int i,j;
  int *pointchng;
  cout<<setiosflags(ios::fixed | ios::showpoint);


  for(i=0;i<dim1;i++)
    {
      cout<<pointvect[i]<<"  ";
      for(j=0;j<dim2;j++)
	{
	  pointchng=pointvect[i]+j;
	  if(j==dim2-1)
	    cout<<setprecision(0)<<setw(4)<<*pointchng<<endl;   //	    cout<<Kprimemat[i][j]<<endl;
	  else
	    cout<<setprecision(0)<<setw(4)<<*pointchng<<" ";   //	            cout<<Kprimemat[i][j]<<" ";
	}
    }
}

void matrixcheckfloat (float *pointvect[], int dim1, int dim2, int vstart, int istart)
{
  int i,j,y;
  float *pointchng;
  cout<<setiosflags(ios::fixed | ios::showpoint);

  y=vstart;
  for(i=istart;i<dim1;i++)
    {
      cout<<setw(6)<<y<<": ";
      for(j=0;j<dim2;j++)
	{
	  pointchng=pointvect[i]+j;
	  if(j==dim2-1)
	    cout<<setprecision(1)<<setw(7)<<*pointchng<<endl;   //	    cout<<Kprimemat[i][j]<<endl;
	  else
	    cout<<setprecision(1)<<setw(7)<<*pointchng<<" ";   //	            cout<<Kprimemat[i][j]<<" ";
	}
      y++;
    }

}




float matrixmin (float *pontmat[], int& optv, int& optw,int limrw,int limvect)
{
  float minval=100000000;float val;
  float *pchng;
  int i,j;
  
  for(i=0;i<limrw;i++)
  {
      for(j=0;j<limvect;j++)
      {
    	  pchng=pontmat[i]+j;
    	  val=*pchng;
    	  if(val<minval)
    	  {
    		  minval=val;
    		  optw=j;
    		  optv=i;
    	  }
      }
  }

  return minval;
}

std::string generatestatecode(int row, int time,int *pbounds[],int *prows[],int &highqinv,int &lowqinv,int &qrtdeliv,int &K,std::string &conststcd,int outstandord[])
{
	int i=row,z=0,val1,val2,val3,val4,j,test,l,m=0;
	std::ostringstream statecode,constcd;
	int *pchange,*pchange2,*pchange3,*pchange4;

	statecode<<time<<"_";

		for(l=0;l<LTreg-1+3;l++)
		{
			if(l==0)  // FIRST CODE IS MARKET CAPACITY
			{
				pchange=prows[l+1];
				pchange+=time-1;
				val1=*pchange;
				pchange2=pbounds[3];   //be careful with no parameterization for LT
				pchange2+= time-1;
				val2=*pchange2;
				j=i/val1+val2;
				K=j;
			}
			else if (l==LTreg+1)  // HIGH QUALITY INVENTORY
			{
				pchange=prows[l];
				pchange+=time-1;
				pchange2=pbounds[0];
				pchange2+=time-1;
				pchange3=pbounds[1];
				pchange3+=time-1;

				pchange4=prows[l-1];
				pchange4+=time-1;

				val1=*pchange;
				val2=*pchange2;
				val3=*pchange3;
				val4=*pchange4;

				if((i%val4)<val1)
					j=(i%val4)%val1+val2;
				else
					j=((i%val4)-val1)%(val3+1);

				//j=i%rowsizes[l][horizon-1]+bounds[0][t-1];
				highqinv=j;

			}
			else
			{
				if(l==LTreg)  //LOW QUALITY INVENTORY
				{
					pchange=pbounds[2];
					pchange+=time-1;
					val1=*pchange+1;

					pchange2=prows[l+1];
					pchange2+=time-1;
					val2=*pchange2;

					pchange3=pbounds[1];
					pchange3+=time-1;
					val3=*pchange3;
					
					pchange4=prows[l];
					pchange4+=time-1;
					val4=*pchange4;
					//					test=(bounds[2][horizon-1]+1);
					if((i%val4)<val2)
						j=0;
					else
						j=(((i%val4)-val2)/(val3+1)+1)%val1;
//					j=((i/rowsizes[l+1][horizon-1])%test);
					lowqinv=j;
				}
				else if (l==1)
				{
					pchange=prows[l+1];
					pchange+=time-1;
					pchange2=prows[LTreg];
					pchange2+=time-1;


					val1=*pchange;
					val2=*pchange2;
					j=(i/val1)%(regsupcap+1);

					//j=((i/rowsizes[l+1][horizon-1])%regsupcap);
					qrtdeliv=j;
				}
				else
				{
					pchange=prows[l+1];
					pchange+=time-1;
					val1=*pchange;

					j=(i/val1)%(regsupcap+1);
					constcd<<j<<"_";
					outstandord[m++]=j;
					//j=((i/rowsizes[l+1][horizon-1])%regsupcap);
				}
			}
				
			if(l==LTreg-2+3)
				{statecode<<j;}
			else
				{statecode<<j<<"_";}
		}
	
		std::string outp=statecode.str();
		conststcd=constcd.str();

	return outp;
}

void generateactspacebounds(int *pactbounds[],int *pactboundsL[],int *pbounds[], int Dmax,int regscap,int maxmarketcap)
{
	int *pchange;int *pchange2;
	int state,up,low,lowmark,i,k,z,v;

	pchange=pbounds[0]+horizon-1;
	up=*pchange;
	pchange=pbounds[1]+horizon-1;
	low=*pchange;
	
	pchange=pbounds[2]+horizon-1;
	lowmark=*pchange;
	
	k=0;z=1;v=1;
	for(i=0;i<low-up+1;i++)
	{
		state=i+up;
		pchange2=pactbounds[i];
		*pchange2=state;
	
		pchange2=pactbounds[i]+1;  // LOWER BOUNDS FOR STATE SPACE
		if(state-Dmax<up)
			{*pchange2=Dmax-k;k++;}
		else
			*pchange2=0;
	
		pchange2=pactbounds[i]+2;
		if(state+regscap>low)
			{*pchange2=regscap-z;z++;}
		else
			*pchange2=regscap;
	}


	for(i=0;i<=lowmark;i++)
	{
		state=i;
		pchange2=pactboundsL[i];
		*pchange2=i;

		pchange2=pactboundsL[i]+1;
		*pchange2=0;

		pchange2=pactboundsL[i]+2;

		if(state+maxmarketcap>lowmark)
		{*pchange2=maxmarketcap-v;v++;}
		else
		*pchange2=maxmarketcap;
	}

}

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}


void matrixoutputfloat(float *pointvect[], int row, int col, std::ofstream&fnm)
{
  int i,j;
  float k;
  float *pchange;

  //fnm<<header<<"\n"<<endl;
  
  for(i=0;i<=col;i++)
  {
	  if(i==0)
		  {fnm<<"Orders\t";continue;}
	  if(i==col)     
		  fnm<<i<<"\n\n";
	  else
	  	  fnm<<i<<"\t";
  }
  
  for(i=0;i<row;i++)
    {
      for(j=0;j<col;j++)
      {
    	  if(j==0)
    	  {
    		  fnm<<i<<"\t";continue;
    	  }
    	  
    	  pchange=pointvect[i]+j-1;
    	  k=*pchange;
	  
    	  if(j==col)     
    		  fnm<<k<<"\n";
       	  else
    		  fnm<<k<<"\t";
      }
    }
}


void write2database(float *pmatrix[],int rowdim,int coldim,int time,bool flg1,bool databaseflg)
{
	int i,j;
	float stvar;
	float *pchange;
	float cost;
	
	sqlite3 *db2;
	//	sqlite3_mutex_enter(sqlite3_db_mutex(db));

	sqlite3_stmt *res2;
	ofstream outputfl;
	
	const char *errMSG;
	const char *tail;
	char *zErrMsg = 0;

	float optv,optw;int error;

	std::ostringstream tablecode,statecode,statecode2,sqlcode,sqlcoderead,indexcode,filename;
	std::string query,queryread,stcd,stcd2,conststcd,fil;


	if(databaseflg)
	  {
	    error=sqlite3_open("testLT.db",&db2);  

	    //IS DATABASE OPENED?
	    if(error)
	      {
		std::cout<<"Could not open DB"<<std::endl;
		sqlite3_close(db2);
		system("pause");
		return;
	      }

	//DATABASE PARAMETERS.
	sqlite3_exec(db2,"BEGIN TRANSACTION", NULL, NULL, &zErrMsg);
	sqlite3_exec(db2, "PRAGMA cache_size = 2000", NULL, NULL, &zErrMsg);  //NUMBER OF PAGES IN THE MEMERY
	sqlite3_exec(db2, "PRAGMA synchronous = OFF", NULL, NULL, &zErrMsg);  
	sqlite3_exec(db2, "PRAGMA journal_mode = MEMORY", NULL, NULL, &zErrMsg);	
	sqlite3_exec(db2, "PRAGMA count_changes=OFF", NULL, NULL, &zErrMsg);
	sqlite3_exec(db2, "PRAGMA temp_store=MEMORY", NULL, NULL, &zErrMsg);
	
	
	//CREATE TABLE: W CREATE A DIFFERENT TABLE FOR EACH PERIOD'S  STATES AND COSTS.
	tablecode.str("");
	tablecode<<"CREATE TABLE period"<<time<<" (rownum INT,time INT, stateid CHAR(20) PRIMARY KEY, cost DECIMAL(5,4),optL INT,optH INT);";
	query=tablecode.str();
	sqlite3_exec(db2,query.c_str(),callback,0,&zErrMsg);
	       
	
	//PREPARE SQLCODE. LATER WE BIND THEM WITH PARAMETERS TO BE REPLACED QUESTION MARKS
	sqlcode.str("");
	sqlcode<<"INSERT INTO period"<<time<<" (rownum,time,stateid,cost,optL,optH) VALUES(?,?,?,?,?,?);";
	query=sqlcode.str();
	error=sqlite3_prepare_v2(db2,query.c_str(),query.length(),&res2,&tail);
	  }
	else
	  {
	    
	    filename<<"output_per"<<time<<".txt";
	    fil=filename.str();
	    outputfl.open(fil.c_str());filename.str("");
	    outputfl<<rowdim<<endl;
	  }

	
	for(i=0;i<rowdim;i++)
	
		statecode2<<time<<"_";
		for(j=0;j<coldim;j++)
		{
			pchange=pmatrix[i]+j;			
			stvar=*pchange;
			
			if(j==coldim-4)
				statecode2<<(int)stvar;
			else if(j<coldim-4)
				statecode2<<(int)stvar<<"_";
			
			if(j==coldim-2)
				optw=(int)stvar;
			else if(j==coldim-1)
				optv=(int)stvar;	
		}
		

		stcd=statecode2.str();
		pchange=pmatrix[i]+coldim-3;
		cost=*pchange;
		statecode2.str("");
		

		if(databaseflg)
		  {
		      sqlite3_bind_int(res2,1,i);
		      sqlite3_bind_int(res2,2,time);		//BIND CURRENT MARKET CAPACITY TO WRITE QUERY (res2)
		      sqlite3_bind_text(res2,3,stcd.c_str(),-1,NULL);  //BIND CURRENT STATE INFOR TO WRITE QUERY (res2)
		      sqlite3_bind_double(res2,4,(double)cost);		//BIND MINMUM COST OF STATE TO QRITE QUERY (res2)
		      sqlite3_bind_int(res2,5,(int)optw);
		      sqlite3_bind_int(res2,6,(int)optv);		
		      sqlite3_step(res2);								//EXECURE WRITING (res2)

		      sqlite3_clear_bindings(res2);					//CLEAR BINDINGS OF WRITE QUERY
		      sqlite3_reset(res2);							//RESER BINDINGS OF WRITE QUER		
		
		      if(((i%50000)==0)&&(flg1))
			cout<<"Row# "<<i<<" is written to database."<<endl; 
		     
		    }
		  else
		    outputfl<<i<<"\t"<<time<<"\t"<<stcd<<"\t"<<cost<<"\t"<<optw<<"\t"<<optv<<"\n";
		  

	}
	
	if(databaseflg)
	    {
		sqlite3_exec(db2, "COMMIT TRANSACTION", NULL, NULL, &zErrMsg);
		sqlite3_finalize(res2);
		sqlcode.str("");
		sqlcode<<"CREATE INDEX ind"<<time<<" ON period"<<time<<" (rownum,stateid);";
   		query=sqlcode.str();
   		sqlite3_exec(db2,query.c_str(),callback,0,&zErrMsg);
		sqlite3_close(db2);
	    }
	  else
	    outputfl.close();

}


