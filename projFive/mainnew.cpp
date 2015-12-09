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
#include <queue>

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

void fft(float data[], unsigned long nn, int isign){
	unsigned long n,mmax,m,j,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta;
	float tempr,tempi;
	
	n=nn << 1;
	
	j=1;

	for (i=1;i<n;i+=2) {
		if (j > i) {
			SWAP(data[j],data[i]);
			SWAP(data[j+1],data[i+1]);
		}
		m=n >> 1;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}	

	mmax=2;
	while (n > mmax) {
		istep=mmax << 1;
		theta=isign*(6.28318530717959/mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=n;i+=istep) {
				j=i+mmax;
				tempr=wr*data[j]-wi*data[j+1];
				tempi=wr*data[j+1]+wi*data[j];
				data[j]=data[i]-tempr;
				data[j+1]=data[i+1]-tempi;
				data[i] += tempr;
				data[i+1] += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}

	if (isign == 1)
	{
	for( int i=1; i<n+1;i++)
		{
		data[i] = data[i]/nn;
		}
	}	
}
#undef SWAP
/* (C) Copr. 1986-92 Numerical Recipes Software 0#Y". */

void fft2D(int N, int M, vector<vector<float> >& real_Fuv, vector<vector<float> >& imag_Fuv, int isign){

	vector<float> tempV;
	vector<vector<float> > combo;
	tempV.resize(M*2, 0);

	int count = 0;
	for(int x=0; x<N; x++){
		for(int y=0; y<M*2; y+=2){
			tempV[y] = real_Fuv[x][count];
			tempV[y+1] = imag_Fuv[x][count];
			count++;
		}
		count = 0;
		combo.push_back(tempV);
	}

	//do all of the rows first time
	//need to store into data array, run function, and then put back into vector
	float data[M*2];
	for(int x=0; x<N; x++){
		for(int y=0; y<M*2; y++){
			data[y] = combo[x][y];
		}
		fft(data-1, M, isign);	
		for(int y=0; y<M*2; y++){
			combo[x][y] = data[y];
		}
	}

	int counter2 = 0;
	//now we operate on each col
	for(int y=0; y<M*2; y+=2){
		for(int x=0; x<N; x++){
			data[counter2] = combo[x][y];
			counter2++;
			data[counter2] = combo[x][y+1];
			counter2++;
		}
		fft(data-1, M, isign);	
		counter2 = 0;
		for(int x=0; x<N; x++){
			combo[x][y] = data[counter2];
			counter2++;
			combo[x][y+1] = data[counter2];
			counter2++;
		}
		counter2 = 0;
	}

	//temp = combo;
	//Finally, we need to split the data up into real and imaginary vectors
	count = 0;
	for(int x=0; x<N; x++){
		for(int y=0; y<M*2; y+=2){
			real_Fuv[x][count] = combo[x][y];
			imag_Fuv[x][count] = combo[x][y+1];
			count++;
		}
		count = 0;
	}
}

void haarTransform(ImageType inputImage, vector<vector<float> > &haarImage, int N, int M, int isign);

void haarRow(vector<float> &row, int N, int isign);

void haarCol(vector<vector <float > > &haarImage, int N,int  isign, int col);

bool compare ( pair < int, pair<int,int> > x, pair < int, pair<int,int> > y) {
  return ( abs(x.first) > abs(y.first));
}

bool compare2 ( pair < pair<float, float>, pair<int,int> > x, pair < pair<float, float>, pair<int,int> > y) {
  return ( sqrt(pow(x.first.first, 2)+pow(x.first.second, 2) ) > 
  		   sqrt(pow(y.first.first, 2)+pow(y.first.second, 2) ));
}

void topXHaar(vector<vector <float > > &haarImage, int N, int M, int top){
	cout << endl << haarImage[0][0] << endl;
	vector<pair<int, pair<int,int> > > holder;
	pair<int, int> tempLoc;
	for(int x=0; x<N; x++){
		for(int y=0; y<M; y++){
			if(x==0 && y==0) y++;
			tempLoc.first = x;
			tempLoc.second = y;
			holder.push_back(make_pair(haarImage[x][y], tempLoc));
		}
	}

	sort(holder.begin(), holder.end(), compare);

	for(int x=0; x<N; x++){
		for(int y=0; y<M; y++){
			if(x==0 && y==0) y++;
			haarImage[x][y] = 0;
		}
	}
	
	for(int x=0; x<top; x++){
		haarImage[holder[x].second.first][holder[x].second.second] = holder[x].first;
	}
	cout << endl << haarImage[0][0] << endl;
}

void topXDFT(vector<vector<float> >& real, vector<vector<float> >& imag, int N, int M, int top){
	vector<pair<pair<float, float>, pair<int, int> > > holder;
	pair<float, float> data;
	pair<int, int> coord;
	for(int x=0; x<N; x++){
		for(int y=0; y<M; y++){
			data.first = real[x][y];
			data.second = imag[x][y];
			coord.first = x;
			coord.second = y;
			holder.push_back(make_pair(data, coord));
		}
	}
	sort(holder.begin(), holder.end(), compare2);

	//0 out the data
	for(int x=0; x<N; x++){
		for(int y=0; y<M; y++){
			real[x][y] = 0;
			imag[x][y] = 0;
		}
	}

	for(int x=0; x<top; x++){
		real[holder[x].second.first][holder[x].second.second] = holder[x].first.first;
		imag[holder[x].second.first][holder[x].second.second] = holder[x].first.second;
	}

}


int main(){
	string fileName = "boat.pgm";
	int N, M, Q;
	int tempIntOrig,tempIntRest;
	double MSE;
	bool imageFormat;
	vector<vector<float> > haarImage;
	//Creating of the image to be worked with
	readImageHeader(fileName, N,M,Q,imageFormat);
	ImageType myImage(N,M,Q);
	ImageType secondImage(N,M,Q);
	readImage(fileName, myImage);
	readImage(fileName, secondImage);
	//copy of the original image
	readImageHeader(fileName, N,M,Q,imageFormat);
	ImageType originalImage(N,M,Q);
	readImage(fileName, originalImage);

	vector<vector<float> > real;
	vector<vector<float> > image;
	vector<vector<float> > magnitude;

	vector<float> temp;
	vector<float> zeros;
	temp.resize(M, 0);
	zeros.resize(M, 0);

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

	//tops
	//comment this out to not remove stuff
	topXHaar(haarImage, N, M, 80);


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

	int tempInt;
	for(int x=0; x<N; x++){
		for(int y=0; y<M; y++){
			secondImage.getPixelVal(x, y, tempInt);
			//UNCOMMENT THIS NEXT LINE TO DO MAGNITUDE SHIFTING
			////////////////////////////////////////////////////
			//tempInt = tempInt*pow(-1, (x+y));
			////////////////////////////////////////////////////
			temp[y] = tempInt;
		}
		real.push_back(temp);
		image.push_back(zeros);
	}
	//DFT stuff	can be all commented out if you want.
	fft2D(N, M, real, image, 1);

	topXDFT(real, image, N, M, 80);	

	fft2D(N, M, real, image, -1);

	for(int x=0; x<N; x++){
		for(int y=0; y<M; y++){
			secondImage.setPixelVal(x, y, real[x][y]);
		}
	}

	writeImage("outputDFT.pgm", secondImage);

	
	return 0;
}

//Computes the haar transformation on a single column on the left most side of the image
//This funtion takes in the haar image, a 2d vector array, of size N by N
//isign determins the forward or inverse, 1 is forward, -1 is inverse. 

void haarCol(vector<vector <float > > &haarImage, int N,int isign, int col)
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
		
		rowL[i/2] = cValue*(haarImage[i][col]*1+haarImage[i+1][col]*1);
		rowH[i/2] = cValue*(haarImage[i][col]*1+haarImage[i+1][col]*-1);
		
		
		}

		for(int i = 0;i<rowL.size();i++)
		{
		haarImage[i][col] = rowL[i];
		haarImage[i+rowL.size()][col] = rowH[i];
		}
		
		if(N>2)
		{
		//haarCol(haarImage,N/2,1);
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
			rowL[i] = haarImage[i][col];
			rowH[i] = haarImage[i+N/2][col];
		}

		for(int i=0;i<N;i+=2)
		{
		haarImage[i][col] = rowL[i/2];
		haarImage[i+1][col] = rowH[i/2];
		
		}

	//computes the haar transformation
	//reverse calculation

		for(int i=0;i<N;i+=2)
		{
		tempOne = cValue*(haarImage[i][col] *1+haarImage[i+1][col] *1);
		tempTwo = cValue*(haarImage[i][col] *1+haarImage[i+1][col] *-1);
		haarImage[i][col]  = tempOne;
		haarImage[i+1][col]  = tempTwo;

		//rounds very low numbers to 0 on the inverse.
		

		}

		//recurseive call
		if(N<haarImage[0].size())
		{
		//haarCol(haarImage,N*2,-1);
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
		//haarRow(row,N/2,1);
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
		

		}

		//recursive funtion call
		if(N<row.size())
		{
		//haarRow(row,N*2,-1);
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

		
		for(N;N>1; N=N/2){
		for(int k = 0;k<N;k++)
		{
			haarRow(haarImage[k],N,1);
		}

	
		for(int l=0;l<N/2;l++){
		haarCol(haarImage,N,1, l);}
	}
		
	
		

	}


	//inverse
	else if(isign == -1)
	{
	
			haarCol(haarImage,2,-1, 0);
			haarRow(haarImage[0],2,-1);
			haarRow(haarImage[1],2,-1);			
			

			for(int i=0;i<2;i++)
			{
			haarCol(haarImage,4,-1, i);	
			}
			for(int j=0;j<4;j++)
			{
			haarRow(haarImage[j],4,-1);	
			}

			for(int i=0;i<4;i++)
			{
			haarCol(haarImage,8,-1, i);	
			}
			for(int j=0;j<8;j++)
			{
			haarRow(haarImage[j],8,-1);	
			}

			for(int i=0;i<8;i++)
			{
			haarCol(haarImage,16,-1, i);	
			}
			for(int j=0;j<16;j++)
			{
			haarRow(haarImage[j],16,-1);	
			}

			for(int i=0;i<16;i++)
			{
			haarCol(haarImage,32,-1, i);	
			}
			for(int j=0;j<32;j++)
			{
			haarRow(haarImage[j],32,-1);	
			}

			for(int i=0;i<32;i++)
			{
			haarCol(haarImage,64,-1, i);	
			}
			for(int j=0;j<64;j++)
			{
			haarRow(haarImage[j],64,-1);	
			}

			for(int i=0;i<64;i++)
			{
			haarCol(haarImage,128,-1, i);	
			}
			for(int j=0;j<128;j++)
			{
			haarRow(haarImage[j],128,-1);	
			}

			for(int i=0;i<128;i++)
			{
			haarCol(haarImage,256,-1, i);	
			}
			for(int j=0;j<256;j++)
			{
			haarRow(haarImage[j],256,-1);	
			}
			
			

	}
	else
	{
	cout<<"incorrect sign, haar transform aborted."<<endl;
	}


}
