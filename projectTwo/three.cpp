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

void padImage( ImageType& paddedImage, ImageType oldImage, int padValueX, int padValueY){
	//variable declaration
	int N,M,Q,tempValue;
	
	//get paddImage info
	paddedImage.getImageInfo(N,M,Q);

	//zero out padded image
	for(int i=0; i<N;i++)
	{
		for(int j=0; j<M;j++)
			{
				paddedImage.setPixelVal(i,j,0);
			}
	}

	//loop through the padded image, and place the old image with in it. 
	for(int i=padValueX;i<N-padValueX;i++)
	{
		for(int j = padValueY;j<M-padValueY;j++)
		{
			oldImage.getPixelVal(i-padValueX,j-padValueY,tempValue);
			paddedImage.setPixelVal(i,j,tempValue);
		}
		
	}
}

/*
SaltAndPeppaNoise:
Written by: Jeremiah Berns
Dependincies: ctime, Image.h, Image.cpp
Description:	Will take the passed image, and the degrigation percentage, then apply salt and pepper noise to the image
		based on how much of the image should be distroted. 
*/

void saltAndPeppaNoise(ImageType& oldImage, int distortionPercent){
	int N,M,Q, totalPix, numOfDistortions;
	oldImage.getImageInfo(N,M,Q);
	totalPix = N*M;
	numOfDistortions = totalPix/(100/distortionPercent);
	cout<<numOfDistortions<<endl;
	
	srand(time(0));
	int color = 255;
	
	for(int i=0;i<numOfDistortions;i++)
	{
		color = 255;
		if(rand()%2==0) color = 0;
		oldImage.setPixelVal(rand()%N, rand()%M, color);		
	}	
}

void average(ImageType image, int size, string printName){
	cout << "start" << endl;
	int N, M, Q, temp, temp2, count;
	bool imageFormat;
	int max=0;

	double temp3;

	ImageType newImage(image);
	image.getImageInfo(N, M, Q);

	int padValueX = (size-1)/2;
	int padValueY = (size-1)/2;
	cout << padValueX << " " << padValueX << endl;
	ImageType paddedImage(N+padValueX*2, M+padValueY*2, Q);
	padImage(paddedImage, image, padValueX, padValueY);

	//iteration over image with mask now.
	//first two loops handle iteration across image
	//second two for loops handle the math
	for(int x=padValueX; x<N+padValueX; x++){
		for(int y=padValueY; y<M+padValueY; y++){
			temp = 0;
			for(int a=x-padValueX; a<x+padValueX; a++){
				for(int b=y-padValueY; b<y+padValueY; b++){
					paddedImage.getPixelVal(a, b, temp2);
					temp += temp2;
				}
			}
			if((temp/(size*size))+1 > max) max = (temp/(size*size))+1;
			newImage.setPixelVal(x-padValueX, y-padValueY, (temp/(size*size))+1);			
		}
	}

	//normalize colors.
	newImage.getImageInfo(N,M,Q);
	for(int x=0; x<N; x++){
		for(int y=0; y<M; y++){
			newImage.getPixelVal(x, y, temp);
			temp3 = temp;
			temp3 *= 212;
			temp3 /= max;
			if(temp3 < 0) cout << temp3 << endl;
			temp = temp3;
			newImage.setPixelVal(x, y, temp);
		}
	}

	writeImage(printName, newImage);
}

void medianFilter(ImageType image, int size, string printName1){	
	int N,M,Q,temp, temp2;
	int count;
	long long int temp3;
	cout << size << endl;
	vector<int> medArray(size*size);

	//First thing is to make a copy of the image before padding anything.
	ImageType newImage(image);

	//now, pad the image depending on size of mask.
	image.getImageInfo(N, M, Q);
	int padValueX = (size-1)/2;
	int padValueY = (size-1)/2;
	ImageType paddedImage(N+padValueX*2, M+padValueY*2, Q);
	padImage(paddedImage, image, padValueX, padValueY);

	//iteration over image with mask now.
	//first two loops handle iteration across image
	//second two for loops handle the math
	for(int x=padValueX; x<N+padValueX; x++){
		for(int y=padValueY; y<M+padValueY; y++){
			temp = 0;
			count = 0;
			for(int a=x-padValueX; a<x+padValueX; a++){
				for(int b=y-padValueY; b<y+padValueY; b++){
					paddedImage.getPixelVal(a, b, temp2);
					medArray[count] = temp2;
					count++;
				}
			}
			sort(medArray.begin(), medArray.end());			
			newImage.setPixelVal(x-padValueX, y-padValueY, medArray[(size*size)/2]);			
		}
	}

	//write the image
	writeImage(printName1, newImage);
}

int main(){

	string fileName = "./images/boat.pgm";
	int N, M, Q;
	bool imageFormat;

	readImageHeader(fileName, N,M,Q,imageFormat);
	ImageType image(N,M,Q);
	readImage(fileName,image);

	//salt and pepper the image
	saltAndPeppaNoise(image, 50);

	writeImage("Salt50boat.pgm", image);

	medianFilter(image, 7, "boat-50-7-Median.pgm");
	average(image, 7, "boat-50-7-Average.pgm");
	medianFilter(image, 15, "boat-50-15-Median.pgm");
	average(image, 15, "boat-50-15-Average.pgm");
	
return (0);
}