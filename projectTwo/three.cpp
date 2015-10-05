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

void medianFilter(ImageType padded, ImageType& medianImage, int padVal){
	//varibale decleration
	int tempMatrix[3][3], flatMatrix[9];
	int N,M,Q,padN,padM, tempPixel;
	padded.getImageInfo(padN,padM,Q);

	//main loop to go through the function
	for(int i=padVal; i<padN-padVal;i++)
	{
		for(int j=padVal;j<padM-padVal;j++)
		{
			
			for (int k=0;k<3;k++)
			{
				//builds the 3x3 matrix 
				for(int l=0;l<3;l++)
				{
					padded.getPixelVal(i-padVal+k,j-padVal+l,tempPixel);
					tempMatrix[k][l] = tempPixel;
					
				}
				//converts the 3x3 matrix to a flat matix
				for(int m=0;m<3;m++)
				{
					for(int n=0;n<3;n++)
					{
						flatMatrix[m*3+n] = tempMatrix[n][m];
					}

				}
				
			}
			//sort the matrix, and place the median value in the new image
			sort(flatMatrix,flatMatrix+9);
			medianImage.setPixelVal(i-padVal,j-padVal, flatMatrix[4]);
			
		}
	}
	//write the image
	writeImage("MedianFilter.pgm",medianImage);
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
	
	for(int i=0;i<numOfDistortions;i++)
	{
		oldImage.setPixelVal(rand()%256, rand()%256, 255);
	}

	writeImage("saltAndPeppa.pgm", oldImage);	
}

int main(){

	string fileName = "lenna.pgm";
	int N, M, Q;
	bool imageFormat;

	//Before you declare your Image Type Object, you have to run readImageHeader to get the info of the image
	//N,M,Q, and imageFormat are all passed by refrence.
	readImageHeader(fileName, N,M,Q,imageFormat);

	//Only after the readImageHeader is run, can you create your imageType Object in this format.
	ImageType myImageLenna(N,M,Q);

	//Will read the image in and store the pixel value in myImage.pixelValue
	readImage(fileName,myImageLenna);


	//create the pad value, value will add that man zeros from the edge of the image, IE value of 1 will be 1 set of zeros around the edge
	int padValue = 1;

	//create the padded image with the extra length needed to support the padding
	ImageType paddedImage(N+(padValue*2),M+(padValue*2),Q);
	ImageType sharpImage(N,M,Q);

	

	
	
	//place the old image inside the padded image
	padImage( paddedImage,myImage, padValue);


	/* uncomment for salt and peper and median filter
	saltAndPeppaNoise(myImage,10);
	medianFilter(paddedImage, sharpImage, padValue);
	*/
	
	//performs the specified shappen function, either, "Prewitt", "Sobel", or "Laplacian" is case sensitive.
	//also requiers the padValue for calculations

	/* uncomment and change sharpening type for shaprening filter
	sharpenImage(paddedImage, sharpImage, "Laplacian", padValue);
	*/
	
return (0);
}