#ifndef BONCUK_H_
#define BONCUK_H_

#include<iostream>
#include<cmath>
#include<time.h>
#include<stdio.h>
#include<iomanip>
using namespace std;

const int bignum = 10000;
# define M_PI           3.14159265358979323846



#include <stdio.h>  /* defines FILENAME_MAX */
#define WINDOWS  /* uncomment this line to use it for windows.*/ 
#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif
#include<iostream>

std::string GetCurrentWorkingDir(void) {
	char buff[FILENAME_MAX];
	GetCurrentDir(buff, FILENAME_MAX);
	std::string current_working_dir(buff);
	return current_working_dir;
}

//SIMPLE FUNCTIONS

template <class T> const T& maxoftwo(const T& a, const T& b)
{
	//MIN OF TWO VARIABLES
	return (a < b) ? b : a;     // or: return comp(a,b)?b:a; for version (2)
}

template <class T> const T& minoftwo(const T& a, const T& b)
{
	//MAX OF TWO VARIABLES
	return (a > b) ? b : a;     // or: return comp(a,b)?b:a; for version (2)
}



int sumoftwo(int x, int y)
{
	int k = x + y;
	return k;
}

int vectsum(int vect[], int vectsize)
{
	int i, sum = 0;

	for (i = 0; i < vectsize; i++)
	{
		sum = sum + vect[i];
	}

	return sum;
}

long double fact(int x)
{
	long  double prd = 1;

	if (x == 0)
	{
		prd = 1;
	}
	else
	{
		do
		{
			prd = prd * x;
			x--;
		} while (x > 1);
	}
	return prd;
}

long double comb(int k, int r)
{
	return fact(k) / (fact(r)*fact(k - r));
}


double binompdf(int k, int n, double p)
{
	double res;
	if(k<= n)
		res= pow(p, (double)k) * pow(1 - p, (double)(n - k))*comb(n, k);
	if (k > n)
		res = 0;
	return res;
}

void convolutionofaraydist(float dist[], float targetdist[], int maxsupport, int per)
{

	int i = 1, k, j;
	float temp[bignum];
//	float tempscalar;


	for (k = 0; k < maxsupport; k++)
		targetdist[k] = dist[k];

	for (i = 2; i <= per; i++)
	{
		for (k = 0; k < maxsupport; k++)
		{
			temp[k] = 0;
			for (j = 0; j <= k; j++)
				temp[k] += dist[j] * targetdist[k - j];
		}

		for (k = 0; k < maxsupport; k++)
			targetdist[k] = temp[k];
	}

}



//PROBABILITY DISTRIBUTIONS

long double poispdf(int y,float lambda)
{
  long double prb= (long double) exp(-1*lambda)*pow(lambda,y);
  //  double factor=fact<double>(y);
  //  prb=prb/factor;  
  prb=prb/fact(y);
  
  return prb;
}

long double poiscdf(int y, float lambda)
{
long  double prb=0;
  int i;

  for(i=0;i<=y;i++)
    {
      prb=prb+poispdf(i,lambda);
    }

  return prb;
}

int invpoiscdf(float theta,float lambda)
{
//theta: the probility for inverse cdf function.
	int x=0;
	long double prob;
	do
	{
		prob=poiscdf(x++,lambda);
		
	}
	while(prob<=theta);
	
	return x-1;
}

long double logdistpdf(int y, float p)
{
//p: distribution parameter 0<p<1

	long double prb=-1/log(1-p)*pow(p,y)/y;
	return prb;
}

long double negativebinompdf(int y, float p,int r)
{
	//p: success probability of a Bernouilli trial
	return comb(y+ r -1,y)*pow(p,y)*pow(1-p,r);
}

long double negativebinomcdf(int y, float p,int r)
{
	  long double prb=0;int i;
	  for(i=0;i<=y;i++)
	      prb+=negativebinompdf(i,p,r);  

	  return prb;
}

long double geompdf(int y, float p)
{
  //p: probability of scucess for a Bernuilli trial (Wikipedia)
	
	long double prb=0;
  double q=1-(double)p;
  prb=(long double) pow(q,y)*p;
  return prb;

}

long double geomcdf(int y, float p)
{
	  //p: probability of scucess for a Bernuilli trial (Wikipedia)
	long double prb=0;int i;
  for(i=0;i<=y;i++)
      prb+=geompdf(i,p);  

  return prb;
}

int invgeomcdf(float theta,float param)
{
//param is succeess probabiltiy in teh Bernouilli trial for geometric distribution
	
	//theta: the probility for inverse cdf function.
	int x=0;
	long double prob;
	do
	{
		prob=geomcdf(x++,param);
		
	}
	while(prob<=theta);
	
	return x-1;
}

double normalCDF(double x, double mu, double sigma) // Phi(-∞, x) aka N(x)
{
	//NORMAL CDF
	//THIS VERSION OF THE FUNCTION IS USED IN THE MULTICLASSDP PROJECT
	double z = (x - mu) / sigma;
	return std::erfc(-z / std::sqrt(2)) / 2;
}

double normalPDF(double x, double mu, double sigma)
{
	//NORMAL PDF
	//THIS VERSION OF THE FUNCTION IS USED IN THE MULTICLASSDP PROJECT
	double z = (x - mu) / sigma;
	return 1 / (sqrt(2 * M_PI)*sigma) * exp(-0.5 * z*z);
}

void normalPDFdic(double probvect[],  double mu, double sigma, int maxd)
{
	//DISCRETISE NORMAL PROBABILITY
	//THIS FUNCTION IS MODIFIED FOR USAGE IN SingStoreLostSales
	int d;
//		char c;


		for (d = 0; d < maxd; d++)
		{
			if (d == 0)
				probvect[d] = normalCDF((float)d + 0.5, mu, sigma);
			if (d > 0)
				probvect[d] = normalCDF((float)d + 0.5, mu, sigma) - normalCDF((float)(d - 1) + 0.5, mu, sigma);
		}

		probvect[maxd] = 1 - normalCDF((float)maxd - 0.5, mu, sigma);

}

int invnormaldisc(float theta, double mu, double sigma)
{
	int x=-1;
	double prob;
	do
	{
		prob = normalCDF(x++, mu, sigma);
	} while (prob <= theta);
		return x - 1;
}

int invnegativebinom(float theta, float param, int r)
{
	//param: success probability in Bernouilli trials
	int x = 0;
	long double prob;
	do
	{
		prob = negativebinomcdf(x++, param, r);

	} while (prob <= theta);

	return x - 1;

}

void compoundpoisson(float dist[], int maxsupport, float lambda, int distflg, double param1, double param2 = 0)
{

	if (distflg <= 3)
		cout << "Poisson Arrivals: (" << lambda << ")" << endl;
	else
		cout << "Bernouilli Arrivals: (" << lambda << ")" << endl;
	if ((distflg == 1) || (distflg == 4))
		cout << "Geometic Demand Size: (" << param1 << ")" << endl;
	if ((distflg == 2) || (distflg == 5))
		cout << "Logarithmic Demand Size: (" << param1 << ")" << endl;
	if ((distflg == 3) || (distflg == 6))
		cout << "NBinom Demand Size: (" << param1 << "," << param2 << ")" << endl;



	int i, k;
	float z;
	char c;

	if (distflg == 1)
		dist[0] = exp(-1 * lambda*(1 - geompdf(0, (float)param1)));
	else if (distflg == 2)
		dist[0] = exp(-1 * lambda);
	else if (distflg == 3)
		dist[0] = exp(-1 * lambda*(1 - negativebinompdf(0, (float)param1, (float)param2)));
	else if (distflg >= 4)
		dist[0] = 1 - lambda;

	//cout<<dist[0]<<endl;
	//cout<<geompdf(0,param1);
	// cin>>c;


	for (i = 1; i < maxsupport; i++)
	{
		z = 0;
		for (k = 0; k <= (i - 1); k++)
		{
			if (distflg == 1)
				z += (i - k)*geompdf(i - k, (float)param1)*dist[k];
			else if (distflg == 2)
				z += (i - k)*logdistpdf(i - k, (float)param1)*dist[k];
			else if (distflg == 3)
				z += (i - k)*negativebinompdf(i - k, (float)param1, (float)param2)*dist[k];
		}
		if (distflg <= 3)
			dist[i] = lambda / i * z;
		else if (distflg == 4)
			dist[i] = (lambda)*geompdf(i - 1, (float)param1);
		else if (distflg == 5)
			dist[i] = (lambda)*logdistpdf(i, (float)param1);
		else if (distflg == 6)
			dist[i] = (lambda)*negativebinompdf(i - 1, (float)param1, (float)param2);
	}
	if (dist[maxsupport] < 1)
	{
		dist[maxsupport] = 1;
		cout << "missing digit!";
	}
}


//INVENTORY DOMAIN FUNCTIONS
double lozznorm(double x)
{
	//LOSS FUNCTION OF THE NORMAL DISTRIBUTION
	//THIS VERSION OF THE FUNCTION IS USED IN THE MULTICLASSDP PROJECT
	return normalPDF(x, 0, 1) - x * (1 - normalCDF(x, 0, 1));
}

double calculatenewsboy(int y, float holdrate, float lostsalerate, double mu, double sigma)
{
	// CALCULATE SINGLE PERIOD CPST FUNCTION  - NEWSBOY
	//THIS VERSION OF THE FUNCTION IS USED IN THE MULTICLASSDP PROJECT
	double costofy = holdrate * (y - mu) + (holdrate + lostsalerate)*sigma * lozznorm((y - mu) / sigma);
	return costofy;
}

double condiotionalsingleper(int y,int d1, float holdrate, float lostsalerate)
{
	return maxoftwo(y-d1, 0)*holdrate - minoftwo(y-d1, 0)*lostsalerate;
}

int maxdemand(float precision,float lambda) //GIVES MAX AMOUNT OF DEMAND FOR GIVEN PRECISION LEVEL
{
  double epsilon;
  int i=0;
do
  {
    epsilon=1-poiscdf(i,lambda);
    i++;
  }
 while(epsilon>precision);

 return i;
}


int maxdemandv2(float precision,float param,int distflg) //GIVES MAX AMOUNT OF DEMAND FOR GIVEN PRECISION LEVEL
{
  double epsilon;
  int i=0;
  //param stands for success probabiltiy when we use geometric distribution.
  //FLAG FOR DISTRIBUTION 1:POISSON, 2:GEOMETRIC DIST. 
  //THIS IS VALID ONLY FOR SINGLE PARAMETER DISTRIBUTIONS
do
  {
    if(distflg==1)
    	epsilon=1-poiscdf(i,param);
    else if(distflg==2)
    	epsilon=1-geomcdf(i, param);
    
    i++;
  }
 while(epsilon>precision);

 return i;
}

int maxdemandv3(float precision, float distarray[])
{
	double epsilon;
	int i=0;
	float p=0;

	do
	{
		p+=distarray[i];
		epsilon=1-p;
		i++;
	}
	while(epsilon>precision);
	
	return i;
}


int maxdemandnormal(float mu, float sigma, float precision)
{
	double epsilon;
	int i = 0;
	do
	{
		epsilon = 1 - normalCDF(i, mu, sigma);
		i++;
	} while (epsilon > precision);

	return i;
}

int inverseofarraydist(float dist[], int maxsupport, float x)
{
	int i = 0;
	float prob = 0;


	do
	{
		prob += dist[i++];
	} while (prob <= x);

	return i - 1;

}




float singlepercost (int y, float holding, float backlog, float lambda, float precision)
{
  int i=0,inv,mdemand;
  float singlepercost=0;
  mdemand=maxdemand(precision,lambda);
  for(i=0;i<=mdemand;i++)
    {
      inv=(y-i);

      if(inv>=0)
      {
	  singlepercost= maxoftwo(inv,0)*holding*poispdf(i,lambda) + singlepercost;
      }
      else
      {
	  singlepercost= maxoftwo(-1*inv,0)*backlog*poispdf(i,lambda) + singlepercost;
      }
    }
  
  return singlepercost;
}


double diffclock(clock_t clock1, clock_t clock2)
{
  double diffticks=clock1-clock2;
  double diffms=(diffticks*1000)/CLOCKS_PER_SEC;
  return diffms;
}

double singlepersubstitute(int y, int x, float lambda, float hR,float hS, float substmultp, float backlogmultp, int acqr, int acqs)
{
  float hr=hR;
  float hs=hS;
  float backlog=backlogmultp;
  float subst=substmultp;
  double cost1=0;double cost2=0;double cost3=0;
  double contrb;
  
  int s;

  //  cout<<"what is wrong?";


  if(y>0)
    {
      //  cout<<"check0";
      for(s=0;s<y;s++)
	  {
		contrb=(hr*(y-s) + hs*x)*poispdf(s,lambda);
		cost1+=contrb;
	  }
      for(s=y;s<y+x;s++)
	  {
		  contrb=(hs*(y+x-s) + subst*(s-y))*poispdf(s,lambda);
		  cost2+=contrb;
	  }
      for(s=x+y;s<x+y+1000;s++)
	  {
		  contrb=(backlog*(s-y-x) + x*subst)*poispdf(s,lambda);
		  cost3+= contrb;
		  if((contrb<0.00001)&&(s>lambda*3))
			  break;
	  }
    };

  if((y<=0)&&(y+x>0))
    {
      for(s=0;s<x+y;s++)
	  {
		  contrb=(hs*(x+y-s) + subst*(s-y))*poispdf(s,lambda);
		  cost2+=contrb;
	  }
      for(s=x+y;s<x+y+1000;s++)
	  {
		  contrb=(backlog*(s-y-x) + x*subst)*poispdf(s,lambda);
		  cost3+=contrb;
		  if((contrb<0.00001)&&(s>lambda*3))
			  break;
	  }
    };
  
  if((y<=0)&&(x+y<=0))
    {
      for(s=0;s<100;s++)
	  {
		contrb=(backlog*(s-y-x) + x*subst)*poispdf(s,lambda);
		cost3+=contrb;
		if((contrb<0.00001)&&(s>lambda*3))
			break;
	  }
	}


  double costs=cost1+cost2+cost3;

  return costs;

}




double singlepersubstitutev4(int y, int x, float param, float hR,float hS, float substmultp, float backlogmultp, int acqr, int acqs, int distflg,float discountrate,bool endofhor=false)
{
  
	//param: success probability for geometric distribution
	float hr=hR;
  float hs=hS;
  float backlog=backlogmultp;
  float subst=substmultp;
  double cost1=0;double cost2=0;double cost3=0;
  double contrb, probscalar[bignum];
  float expectation;
  //float discountrate
  int s;

  for(s=0;s<=x+y+1000;s++)
  {
	  switch(distflg)
		{
		case 1:
			probscalar[s]=poispdf(s,param);expectation=param;break;
		case 2:
			probscalar[s]= geompdf(s, param);expectation=(1-param)/param;
		}
  }
	  

  if(y>0)
    {
      //  cout<<"check0";
      for(s=0;s<y;s++)
	  {
    	  contrb=(hr*(y-s) + hs*x)*probscalar[s];
		if(endofhor)
			contrb=(hr*(y-s) + hs*x)*probscalar[s] -((y-s)*acqr + x*acqs)*probscalar[s]*discountrate;
		cost1+=contrb;
	  }
      for(s=y;s<y+x;s++)
	  {
     	  contrb=(hs*(y+x-s) + subst*(s-y))*probscalar[s];
		  	if(endofhor)
				contrb=(hs*(y+x-s) + subst*(s-y))*probscalar[s]-acqs*(y+x-s)*probscalar[s]*discountrate;
		  cost2+=contrb;
	  }
      for(s=x+y;s<x+y+1000;s++)
	  {
      	  contrb=(backlog*(s-y-x) + x*subst)*probscalar[s];
		  if(endofhor)
			   contrb=(backlog*(s-y-x) + x*subst)*probscalar[s]-acqr*(y+x-s)*probscalar[s]*discountrate; 
		  cost3+= contrb;
		  if((contrb<0.00001)&&(s>expectation*3))
			  break;
	  }
    };


  if((y<=0)&&(y+x>0))
    {
      for(s=0;s<x+y;s++)
	  {
    	  contrb=(hs*(x+y-s) + subst*(s-y))*probscalar[s];
		  if(endofhor)
			contrb=(hs*(x+y-s) + subst*(s-y))*probscalar[s]-acqs*(y+x-s)*probscalar[s]*discountrate;
		  cost2+=contrb;
	  }
      for(s=x+y;s<x+y+1000;s++)
	  {
    	  contrb=(backlog*(s-y-x) + x*subst)*probscalar[s];
		  if(endofhor)
			contrb=(backlog*(s-y-x) + x*subst)*probscalar[s]-acqr*(y+x-s)*probscalar[s]*discountrate;
		  cost3+=contrb;
		  if((contrb<0.00001)&&(s>expectation*3))
			  break;
	  }
    };

  if((y<=0)&&(x+y<=0))
    {
      for(s=0;s<100;s++)
	  {
		contrb=(backlog*(s-y-x) + x*subst)*probscalar[s];
		if(endofhor)
			contrb=(backlog*(s-y-x) + x*subst)*probscalar[s]-acqr*(y+x-s)*probscalar[s]*discountrate;
		cost3+=contrb;
		if((contrb<0.00001)&&(s>expectation*3))
			break;
	  }

	}

  double costs=cost1+cost2+cost3;

  return costs;
}


double singlepersubstitutev5(float distarray[], int maxsupp ,int y, int x,  float hR,float hS, float substmultp, float backlogmultp, int acqr, int acqs, float discountrate,bool endofhor=false)
{
  
	//param: success probability for geometric distribution
	float hr=hR;
  float hs=hS;
  float backlog=backlogmultp;
  float subst=substmultp;
  double cost1=0;double cost2=0;double cost3=0;
  double contrb;
  float expectation=0;
  //float discountrate
  int s;
  char c;  
  for(s=0;s<maxsupp;s++)
	  expectation+=s*distarray[s];

  int boundfors= minoftwo(maxsupp,x+y+1000);
//  cout<<"bountfors="<<boundfors<<endl;  

  if(y>0)
    {
      //  cout<<"check0";
      for(s=0;s<y;s++)
	  {
    	  contrb=(hr*(y-s) + hs*x)*distarray[s];
		cost1+=contrb;
//		if(y==21)
//			cout<<"y: "<<y<<" x: "<<x<<" s: "<<s<<" prob: "<<distarray[s]<<" contr: "<<contrb<<" cost1: "<<cost1<<endl;
	  }
//      if(y==21)
//      cout<<"check1";
      for(s=y;s<y+x;s++)
	  {
     	  contrb=(hs*(y+x-s) + subst*(s-y))*distarray[s];
		  cost2+=contrb;
//		  if(y>=20)
//			  cout<<"y: "<<y<<" x: "<<x<<" s: "<<s<<" prob: "<<distarray[s]<<" contr: "<<contrb<<" cost2: "<<cost2<<endl;
	  }
//      if(y==21)
//      cout<<"check2";
      

      for(s=x+y;s<boundfors;s++)
	  {
//    	  if(y==21){
//    	  cout<<"check2."<<s;
//    	  cin>>c;}
    	  contrb=(backlog*(s-y-x) + x*subst)*distarray[s];
//      	  if(endofhor)
//			   contrb=(backlog*(s-y-x) + x*subst)*distarray[s]-acqr*(y+x-s)*distarray[s]*discountrate; 
		  cost3+= contrb;

	  }
//      if(y==21)
//      cout<<"check3";
    };


  if((y<=0)&&(y+x>0))
    {
      for(s=0;s<x+y;s++)
	  {
    	  contrb=(hs*(x+y-s) + subst*(s-y))*distarray[s];
		  if(endofhor)
			contrb=(hs*(x+y-s) + subst*(s-y))*distarray[s]-acqs*(y+x-s)*distarray[s]*discountrate;
		  cost2+=contrb;
//		  cout<<"y: "<<y<<" x: "<<x<<" s: "<<s<<" prob: "<<distarray[s]<<" contr: "<<contrb<<" cost2: "<<cost2<<endl;
	  }
      for(s=x+y;s<boundfors;s++)
	  {
    	  contrb=(backlog*(s-y-x) + x*subst)*distarray[s];
		  if(endofhor)
			contrb=(backlog*(s-y-x) + x*subst)*distarray[s]-acqr*(y+x-s)*distarray[s]*discountrate;
		  cost3+=contrb;
//		  cout<<"y: "<<y<<" x: "<<x<<" s: "<<s<<" prob: "<<distarray[s]<<" contr: "<<contrb<<" cost3: "<<cost3<<endl;
//		  if((contrb<0.00001)&&(s>expectation*3))
//			  break;
	  }
    };

  
  if((y<=0)&&(x+y<=0))
    {
      for(s=0;s<boundfors;s++)
	  {
		contrb=(backlog*(s-y-x) + x*subst)*distarray[s];
		if(endofhor)
			contrb=(backlog*(s-y-x) + x*subst)*distarray[s]-acqr*(y+x-s)*distarray[s]*discountrate;
		cost3+=contrb;
//		if((contrb<0.00001)&&(s>expectation*3))
//			break;
	  }

	}

  double costs=cost1+cost2+cost3;

  return costs;
}



void calculatebounds(int *pbounds[], int horizon, int Dmax, int regsupcap, int maxmarketcap)
{
//THIS VERSION OF THE FUNCTION IS USED IN THE SUBSTITUTION PROJECT

	int i;
	int up = 0; int low = 0; int lowmark = 0; int markcap = 0;
	int *pchng;

	pchng = pbounds[0];
	*pchng = up - Dmax;
	pchng = pbounds[1];
	*pchng = low + regsupcap;
	pchng = pbounds[2];
	*pchng = lowmark + maxmarketcap;
	pchng = pbounds[3];
	*pchng = maxoftwo(2 - 1, 0);  //maxmarketcap

	for (i = 1; i < horizon; i++)
	{
		pchng = pbounds[0] + (i - 1);
		up = *pchng;
		pchng = pchng + 1;
		*pchng = up - Dmax;

		pchng = pbounds[1] + (i - 1);
		low = *pchng;
		pchng = pchng + 1;
		*pchng = low + regsupcap;

		pchng = pbounds[2] + (i - 1);
		lowmark = *pchng;
		pchng = pchng + 1;
		*pchng = lowmark + maxmarketcap;

		pchng = pbounds[3] + (i - 1);
		markcap = *pchng;
		pchng = pchng + 1;
		*pchng = maxoftwo(markcap - 1, 0);
	}
}


void matrixcheckint(int *pointvect[], int dim1, int dim2)
{
//THIS VERSION OF THE FUCNTION IS USED IN THE SUBSTITUTION PROJECT
	int i, j;
	int *pointchng;
//	cout << setiosflags(ios::fixed | ios::showpoint);


	for (i = 0; i < dim1; i++)
	{
		cout << pointvect[i] << "  ";
		for (j = 0; j < dim2; j++)
		{
			pointchng = pointvect[i] + j;
			if (j == dim2 - 1)
				cout << setprecision(0) << setw(4) << *pointchng << endl;   //	    cout<<Kprimemat[i][j]<<endl;
			else
				cout << setprecision(0) << setw(4) << *pointchng << " ";   //	            cout<<Kprimemat[i][j]<<" ";
		}
	}

}

void matrixcheckdouble(double *pointvect[], int dim1, int dim2, int vstart, int istart)
{
	int i, j, y;
	double *pointchng;
	cout << setiosflags(ios::fixed | ios::showpoint);

	y = vstart;
	for (i = istart; i < dim1; i++)
	{
		cout << setw(6) << y << ": ";
		for (j = 0; j < dim2; j++)
		{
			pointchng = pointvect[i] + j;
			if (j == dim2 - 1)
				cout << setprecision(2) << setw(7) << *pointchng << endl;   //	    cout<<Kprimemat[i][j]<<endl;
			else
				cout << setprecision(2) << setw(7) << *pointchng << " ";   //	            cout<<Kprimemat[i][j]<<" ";
		}
		y++;
	}

}


void matrixoutputint(int *pointvect[], int row, int col, std::ofstream& fnm, string header)
{
	int i, j, k;
	int *pchange;

//	fnm << header << "\n" << endl;
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
		{
			pchange = pointvect[i] + j;
			k = *pchange;

			if (j == col - 1)
				fnm << k << "\n";
			else
				fnm << k << "\t";
		}
	}
}

void matrixoutputdouble(double *pointvect[], int row, int col, std::ofstream&fnm, string header)
{
	int i, j;
	double k;
	double *pchange;

//	fnm << header << "\n" << endl;
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
		{
			pchange = pointvect[i] + j;
			k = *pchange;

			if (j == col - 1)
				fnm << k << "\n";
			else
				fnm << k << "\t";
		}
	}
}


void matrixoutputfloat(float *pointvect[], int row, int col, std::ofstream&fnm, string header)
{
	int i, j;
	float k;
	float *pchange;

//	fnm << header << "\n" << endl;
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
		{
			pchange = pointvect[i] + j;
			k = *pchange;

			if (j == col - 1)
				fnm << k << "\n";
			else
				fnm << k << "\t";
		}
	}
}

//OTHER FUNCTIONS

int marketcost(int mrktamnt, int maxcap, int acqcost, int mincost)
{
	//THIS VERSION OF THE FUNCTION IS USED IN THE SUBSTITUTION PROJECT
	int  x = acqcost + (mincost - acqcost)*mrktamnt / maxcap;
	return x;
}

float survivalprob(int tau, int psi, int flagg)
{
	//THIS VERSION OF THE FUNCTION IS USED IN THE SUBSTITUTION PROJECT
	float alpha, beta, gamma, x, x1;

	switch (flagg)
	{
	case 1:
	{
		alpha = -0.155389491;
		beta = 0.037982626;
		gamma = -0.717293861;
		x = exp(alpha + tau * beta + psi * gamma);
		break;
	}

	case 2:
	{
		// VALUES FOR Tfail=20
		alpha = -1.2352;
		beta = 0.2303;
		gamma = -0.2201;

		x1 = alpha + beta * tau + gamma * psi;
		x = 1 / (1 + exp(-x1));
		break;
	}
	case 3:
	{
		//y=[1,tau,1];
		alpha = -1.63304;
		beta = 0.13582;
		gamma = -0.51723;
		//	coef={-1.63304, 0.13582, -0.51723};
		//      x1=coef[0] + coef[1]*tau + coef[2];

		x1 = alpha + beta * tau + gamma;
		x = 1 / (1 + exp(-x1));
		break;
	}
	case 4:
	{
		x = 1;
		break;
	}
	case 5:
	{
		//y=[1,tau,psi];
		alpha = -1.63304;
		beta = 0.13582;
		gamma = -0.51723;
		//	coef={-1.63304, 0.13582, -0.51723}; //  VALUES FOR Tfail=40
		//	x1= coef[0] + coef[1]*tau + coef[2]*psi;

		x1 = alpha + beta * tau + gamma * psi;
		x = 1 / (1 + exp(-x1));
	}
	}
	return x;
}


#endif /*BONCUK_H_*/
