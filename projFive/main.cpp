#include <math.h>
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <complex>
#include <iostream>
#include <cstdlib>
#include "image.h"
#include <fstream>
#include "image.cpp"
#include <cstdlib>
#include <cstring>
#include <math.h>
#include <ctime>
#include <algorithm>
#include <vector>

using namespace std;

#include <math.h>


double ranf(double m = 1.0) {
    return (m*rand())/(double)RAND_MAX;
}


float box_muller(float m, float s)	/* normal random variate generator */
{				        /* mean m, standard deviation s */
	float x1, x2, w, y1;
	static float y2;
	static int use_last = 0;

	if (use_last)		        /* use value from previous call */
	{
		y1 = y2;
		use_last = 0;
	}
	else
	{
		do {
			x1 = 2.0 * ranf() - 1.0;
			x2 = 2.0 * ranf() - 1.0;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0 );

		w = sqrt( (-2.0 * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}

	return( m + y1 * s );
}


//readimage Function
void readImage(string fname, ImageType& image){
	int i, j;
	int N, M, Q;
	unsigned char *charImage;
	char header [100], *ptr;
	ifstream ifp;
	ifp.open(fname.c_str(), ios::in | ios::binary);

	if (!ifp) {
		cout << "Can't read image: " << fname << endl;
		exit(1);
	}

	// read header

	ifp.getline(header,100,'\n');
	if((header[0]!=80) || (header[1]!=53) ) {
    	cout << "Image " << fname << " is not PGM" << endl;
    	exit(1);
	}

	ifp.getline(header,100,'\n');

	while(header[0]=='#')
	ifp.getline(header,100,'\n');


	M=strtol(header,&ptr,0);
	N=atoi(ptr);

	ifp.getline(header,100,'\n');

	Q=strtol(header,&ptr,0);

	cout<<Q<<endl;

	charImage = (unsigned char *) new unsigned char [M*N];

	ifp.read( reinterpret_cast<char *>(charImage), (M*N)*sizeof(unsigned char));

	if (ifp.fail()) {
		exit(1);
	}

	ifp.close();

	int val;

	for(i=0; i<N; i++) {
		for(j=0; j<M; j++) {
			val = (int)charImage[i*M+j];
			image.setPixelVal(i, j, val);     
   		}
   	}
}

void writeImage(string fname, ImageType& image){
	int i, j;
	int N, M, Q;
	unsigned char *charImage;
	ofstream ofp;

	image.getImageInfo(N, M, Q);

	charImage = (unsigned char *) new unsigned char [M*N];

	// convert the integer values to unsigned char

	int val;

	for(i=0; i<N; i++){
		for(j=0; j<M; j++){
			image.getPixelVal(i, j, val);
			charImage[i*M+j]=(unsigned char)val;
		}
	}

	ofp.open(fname.c_str(), ios::out | ios::binary);

	if (!ofp) {
		cout << "Can't open file: " << fname << endl;
		exit(1);
	}

	ofp << "P5" << endl;
	ofp << M << " " << N << endl;
	ofp << Q << endl;

	ofp.write( reinterpret_cast<char *>(charImage), (M*N)*sizeof(unsigned char));

	if (ofp.fail()) {
		cout << "Can't write image " << fname << endl;
		exit(0);
	}

	ofp.close();
}

//Read image header funtion
void readImageHeader(string fname, int& N, int& M, int& Q, bool& type){
	int i, j;
	unsigned char *charImage;
	char header [100], *ptr;
	ifstream ifp;

	ifp.open(fname.c_str(), ios::in | ios::binary);

	if (!ifp){
		cout << "Can't read image: " << fname << endl;
		exit(1);
	}

	type = false; // PGM

	ifp.getline(header,100,'\n');

	if ( (header[0] == 80) &&  /* 'P' */
	  (header[1]== 53) ) {  /* '5' */
	  type = false;
	}

	else if ( (header[0] == 80) &&  /* 'P' */
		(header[1] == 54) ) {        /* '6' */
		type = true;
	} 

	else {
		cout << "Image " << fname << " is not PGM or PPM" << endl;
		exit(1);
	}

	ifp.getline(header,100,'\n');
	while(header[0]=='#')
	ifp.getline(header,100,'\n');

	M=strtol(header,&ptr,0);
	N=atoi(ptr);

	ifp.getline(header,100,'\n');

	Q=strtol(header,&ptr,0);

	ifp.close();
}


void haarTransform(ImageType inputImage, vector<vector<float> > &haarImage, int N, int M, int isign);

void haarRow(vector<float> &row, int N, int isign);

void haarCol(vector<vector <float > > &haarImage, int N,int  isign);


int main(){
	string fileName = "lenna.pgm";
	int N, M, Q;
	int tempIntOrig,tempIntRest;
	double MSE;
	bool imageFormat;
	vector<vector<float> > haarImage;
	//Creating of the image to be worked with
	readImageHeader(fileName, N,M,Q,imageFormat);
	ImageType myImage(N,M,Q);
	readImage(fileName, myImage);
	//copy of the original image
	readImageHeader(fileName, N,M,Q,imageFormat);
	ImageType originalImage(N,M,Q);
	readImage(fileName, originalImage);

	//calls the Haar Foward transformation
	haarTransform(myImage, haarImage, N, M, 1);

	//outputs the transformed image to HaarOutput.pgm
	for(int i=0;i<N;i++)
		{
			for(int j=0;j<M;j++)
			{

			if(haarImage[i][j] >= 0)
			{
			myImage.setPixelVal(i,j,haarImage[i][j]);
			}

			else
			{myImage.setPixelVal(i,j,0);}

			}		
		}

	writeImage("HaarOutput.pgm", myImage);


	//Calls in Haar Inverse transform
	haarTransform(myImage, haarImage, N, M, -1);

	//outputs the restored images
	for(int i=0;i<N;i++)
		{
			for(int j=0;j<M;j++)
			{
			myImage.setPixelVal(i,j,haarImage[i][j]);
			}		
		}

	writeImage("RestoredLenna.pgm", myImage);


	//computes the Mean Squared Error
	MSE =0;
	for(int i=0;i<N;i++)
	{
	for(int j=0;j<M;j++)
		{
		myImage.getPixelVal(i,j,tempIntRest);
		originalImage.getPixelVal(i,j,tempIntOrig);
		MSE += pow((tempIntOrig-tempIntRest),2);
		}
	}

	MSE = MSE/(M*N);	

	cout<<"Mean Squared Error: "<<MSE<<endl;
	
	return 0;
}

//Computes the haar transformation on a single column on the left most side of the image
//This funtion takes in the haar image, a 2d vector array, of size N by N
//isign determins the forward or inverse, 1 is forward, -1 is inverse. 

void haarCol(vector<vector <float > > &haarImage, int N,int isign)
{

//variable decleration
float cValue =1/sqrt(2);
vector<float> rowH;
vector<float> rowL;
rowH.resize(N/2,0);
rowL.resize(N/2,0);


	//forward
	if(isign ==1)
	{
	float lowPass, highPass;
	
	
	//this snipit of code computes the value useing the high and low pass,
	//then stores it in a sorted order back into the column. 
	for(int i=0;i<N;i+=2)
		{
		
		rowL[i/2] = cValue*(haarImage[i][0]*1+haarImage[i+1][0]*1);
		rowH[i/2] = cValue*(haarImage[i][0]*1+haarImage[i+1][0]*-1);
		
		
		}

		for(int i = 0;i<rowL.size();i++)
		{
		haarImage[i][0] = rowL[i];
		haarImage[i+rowL.size()][0] = rowH[i];
		}
		
		if(N>2)
		{
		haarCol(haarImage,N/2,1);
		}

	}

	
	//inverse
	if(isign == -1)
	{

	//This part will swap the numbers to get them in the correct format to do the inverse 
	//haar transformation
	//swap
	float tempOne, tempTwo;

		for(int i=0;i<N/2;i++)
		{
			rowL[i] = haarImage[i][0];
			rowH[i] = haarImage[i+N/2][0];
		}

		for(int i=0;i<N;i+=2)
		{
		haarImage[i][0] = rowL[i/2];
		haarImage[i+1][0] = rowH[i/2];
		
		}

	//computes the haar transformation
	//reverse calculation

		for(int i=0;i<N;i+=2)
		{
		tempOne = cValue*(haarImage[i][0] *1+haarImage[i+1][0] *1);
		tempTwo = cValue*(haarImage[i][0] *1+haarImage[i+1][0] *-1);
		haarImage[i][0]  = tempOne;
		haarImage[i+1][0]  = tempTwo;

		//rounds very low numbers to 0 on the inverse.
		if(haarImage[i][0]  <.00000001)
		{
			haarImage[i][0]=0;
		}

		if(haarImage[i+1][0]  <.00000001)
		{
			haarImage[i][0] =0;
		}

		}

		//recurseive call
		if(N<haarImage[0].size())
		{
		haarCol(haarImage,N*2,-1);
		}




	}

}
//Computes the haar transformation on a single row passed via a vector
//This funtion takes in a row, of a 1d vector array, of size N
//isign determins the forward or inverse, 1 is forward, -1 is inverse. 
void haarRow(vector<float> &row, int N, int isign)
{

	//variables
	float lowPass, highPass;
	float cValue =1/sqrt(2);
	vector <float> rowH;
	vector<float> rowL;
	
	rowH.resize(N/2,0);
	rowL.resize(N/2,0);

	
	//forward
	if(isign == 1)
	{
	
		//this snipit of code computes the value useing the high and low pass,
		//then stores it in a sorted order back into the row. 
		for(int i=0;i<N;i+=2)
		{
		lowPass = cValue*(row[i]*1+row[i+1]*1);
		highPass = cValue*(row[i]*1+row[i+1]*-1);
		rowL[i/2]=lowPass;
		rowH[i/2]=highPass;
		
		}

		for(int i = 0;i<rowL.size();i++)
		{
		row[i] = rowL[i];
		row[i+rowL.size()] = rowH[i];
		}
		
		if(N>2)
		{
		haarRow(row,N/2,1);
		}


	}
	//inverse
	else if(isign == -1)
	{

	//swaps the numbers into the proper format to compute the haar.
	//swap
	float tempOne, tempTwo;

		for(int i=0;i<N/2;i++)
		{
		rowL[i] = row[i];
		rowH[i]=row[i+N/2];	
		}
		for(int i=0;i<N;i+=2)
		{
		row[i]= rowL[i/2];
		row[i+1] = rowH[i/2];
		}


	//reverse calculation
	//performs the haar transfformation and rounds off the numbers
		for(int i=0;i<N;i+=2)
		{
		tempOne = cValue*(row[i]*1+row[i+1]*1);
		tempTwo = cValue*(row[i]*1+row[i+1]*-1);
		row[i] = tempOne;
		row[i+1] = tempTwo;
		if(row[i] <.00001)
		{
			row[i]=0;
		}

		if(row[i+1] <.00001)
		{
			row[i]=0;
		}

		}

		//recursive funtion call
		if(N<row.size())
		{
		haarRow(row,N*2,-1);
		}

	}
	else
	{
	cout<<"incorrect sign, haar transform aborted."<<endl;
	}

}


void haarTransform(ImageType inputImage, vector<vector<float> > &haarImage, int N, int M, int isign)
{
	
	//variables
	vector<float> temp;
	vector<float> tempTest;
	int tempPixel;
	
	
	temp.resize(N,0);
	tempTest.resize(M,0);

	//forward
	if(isign == 1)
	{
		for(int i=0;i<N;i++)
		{
			for(int j=0;j<M;j++)
			{
			inputImage.getPixelVal(i,j,tempPixel);
			temp[j]=(float) tempPixel;
			
			}
			haarImage.push_back(temp);		
		}


		for(int k = 0;k<N;k++)
		{
			haarRow(haarImage[k],N,1);
		}
		haarCol(haarImage,N,1);
	
		

	}


	//inverse
	else if(isign == -1)
	{
	
		haarCol(haarImage,2,-1);

		for(int i=0;i<N;i++)
		{
			haarRow(haarImage[i],2,-1);
		}	

	}
	else
	{
	cout<<"incorrect sign, haar transform aborted."<<endl;
	}


}
