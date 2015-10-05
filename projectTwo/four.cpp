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

/*
SharpenImage:
Writen by: Jeremiah Berns
dependencies: image.h, image.cpp, math.h
Discription:	This funtion will take in a padded imageType opbject, the new image to store the sharp image,
		the value of the padding applied to the padded image, and the name of the sharpening to be 
		done.  It will perform the desired sharpening, if the selected sharpening type generates both
		a X, and Y gradient, then those images will be printed as well. 

*/

void sharpenImage(ImageType padded, ImageType& sharpImage, string maskType,int padVal){

	//Variable decliration.
	int N,M,Q,padN,padM, tempValX=0, tempValY=0, tempPixel,tempPixelTwo;
	int prewittX[3][3],prewittY[3][3], sobelX[3][3],sobelY[3][3], laplacian[3][3];
	sharpImage.getImageInfo(N,M,Q);
	padded.getImageInfo(padN,padM,Q);

	//load masks

	//Prewitt
	prewittX[0][0] = prewittX[1][0] =prewittX[2][0] = -1;
	prewittX[0][1] = prewittX[1][1] =prewittX[2][1] = 0;
	prewittX[0][2] = prewittX[1][2] =prewittX[2][2] = 1;

	prewittY[0][0] = prewittY[0][1] =prewittY[0][2] = -1;
	prewittY[1][0] = prewittY[1][1] =prewittY[1][2] = 0;
	prewittY[2][0] = prewittY[2][1] =prewittY[2][2] = 1;
	
	//Sobel
	sobelX[0][0] = sobelX[2][0] = -1;
	sobelX[1][0] = -2;
	sobelX[0][1] = sobelX[1][1] = sobelX[2][1] = 0;
	sobelX[0][2] = sobelX[2][2] = 1;
	sobelX[1][2] = 2;
	
	sobelY[0][0] = sobelY[0][2] = -1;
	sobelY[0][1] = -2;
	sobelY[1][0] = sobelY[1][1] = sobelY[1][2] = 0;
	sobelY[2][0] = sobelY[2][2] = 1;
	sobelY[2][1] = 2;


	//laplacian

	laplacian[0][0]=laplacian[0][2]=laplacian[2][0]=laplacian[2][2] = 0;
	laplacian[1][0]=laplacian[0][1]=laplacian[2][1]=laplacian[1][2] = 1;
	laplacian[1][1] = -4;

	//If prewitt sharpening
	if (maskType == "Prewitt")
	{
		//gradient mask of both the x and y direction
		ImageType gradX(N,M,Q);
		ImageType gradY(N,M,Q);
		

		//main loop to go through the function
		for(int i=padVal; i<padN-padVal;i++)
		{
			for(int j=padVal;j<padM-padVal;j++)
			{
				//inner loop that interates over a 3x3 area, applies the mask, then sets the calculated convolution value to the gradX and GradY masks
				for (int k=0;k<3;k++)
				{
					for(int l=0;l<3;l++)
					{
						padded.getPixelVal(i-padVal+k,j-padVal+l,tempPixel);
						tempValY += tempPixel* prewittY[k][l];
						tempValX+= tempPixel* prewittX[k][l];
					}
				}
				if(tempValX > 255)
				{
					tempValX = 255;
				}

				else if(tempValX < 0)
				{
					tempValX = 0;
				}

				if(tempValY > 255)
				{
					tempValY = 255;
				}

				else if(tempValY < 0)
				{
					tempValY = 0;
				}

				gradY.setPixelVal(i-padVal,j-padVal,tempValY);
				gradX.setPixelVal(i-padVal,j-padVal,tempValX);
				tempValX=0;
				tempValY=0;
			}
		}

	//output of the gradX and grad Y images
	writeImage("prewittGradX.pgm", gradX);
	writeImage("prewittGradY.pgm", gradY);

	tempValX = tempValY = 0;

	//calculation of the combined grad x and y images
	for(int i=0;i<N;i++)
	{
		for(int j=0;j<M;j++)
		{

			gradX.getPixelVal(i,j,tempPixel);
			gradY.getPixelVal(i,j,tempPixelTwo);
			tempPixel = tempPixel*tempPixel;
			tempPixelTwo = tempPixelTwo*tempPixelTwo;
			
			tempPixel = tempPixel+tempPixelTwo;
			tempPixel = sqrt(tempPixel);
			if(tempPixel > 255)
			{
				tempPixel = 255;
			}
			
			sharpImage.setPixelVal(i,j,tempPixel);
			tempPixel = tempPixelTwo = 0;
			
		}
	
	}

	//the final prewitt sharp image
	writeImage("PrewittShapImage.pgm",sharpImage);

		
	}
	
	//if sobel
	else if (maskType == "Sobel")
	{
		//decleration of the grad x and grad y images
		ImageType gradX(N,M,Q);
		ImageType gradY(N,M,Q);
		
		//main loop to calculate the gradx and grady images
		for(int i=padVal; i<padN-padVal;i++)
		{
			for(int j=padVal;j<padM-padVal;j++)
			{
				//inner loop dividing the image into 3x3 areas to apply the convolution of the mask
				for (int k=0;k<3;k++)
				{
					for(int l=0;l<3;l++)
					{
						padded.getPixelVal(i-padVal+k,j-padVal+l,tempPixel);
						tempValY += tempPixel* sobelY[k][l];
						tempValX+= tempPixel* sobelX[k][l];
					}
				}
				if(tempValX > 255)
				{
					tempValX = 255;
				}

				else if(tempValX < 0)
				{
					tempValX = 0;
				}

				if(tempValY > 255)
				{
					tempValY = 255;
				}

				else if(tempValY < 0)
				{
					tempValY = 0;
				}

				gradY.setPixelVal(i-padVal,j-padVal,tempValY);
				gradX.setPixelVal(i-padVal,j-padVal,tempValX);
				tempValX=0;
				tempValY=0;
			}
		}

	//outputs the gradX and gradY images
	writeImage("SobelGradX.pgm", gradX);
	writeImage("SobelGradY.pgm", gradY);

	tempValX = tempValY = 0;

	//calculates the final sharpened image
	for(int i=0;i<N;i++)
	{
		for(int j=0;j<M;j++)
		{

			gradX.getPixelVal(i,j,tempPixel);
			gradY.getPixelVal(i,j,tempPixelTwo);
			tempPixel = tempPixel*tempPixel;
			tempPixelTwo = tempPixelTwo*tempPixelTwo;
			
			tempPixel = tempPixel+tempPixelTwo;
			tempPixel = sqrt(tempPixel);
			if(tempPixel > 255)
			{
				tempPixel = 255;
			}
			
			sharpImage.setPixelVal(i,j,tempPixel);
			tempPixel = tempPixelTwo = 0;
			
		}
	
	}

	//writes the sobel sharppened image
	writeImage("SobelSharpImage.pgm",sharpImage);

	}

	//if lapacian
	else if (maskType == "Laplacian")
	{
		
		
		//main loop to calculate the laplacian sharppened mask
		for(int i=padVal; i<padN-padVal;i++)
		{
			for(int j=padVal;j<padM-padVal;j++)
			{
				for (int k=0;k<3;k++)
				{
					for(int l=0;l<3;l++)
					{
						padded.getPixelVal(i-padVal+k,j-padVal+l,tempPixel);
						
						tempValX+= tempPixel* laplacian[k][l];
					}
				}
				if(tempValX > 255)
				{
					tempValX = 255;
				}

				else if(tempValX < 0)
				{
					tempValX = 0;
				}

				

				
				sharpImage.setPixelVal(i-padVal,j-padVal,tempValX);
				tempValX=0;
				
			}
		}


	//outputs the laplacian sharp image
	writeImage("lapacianSharp.pgm", sharpImage);
	

	tempValX = tempValY = 0;
	
	}

	//if you spelled the type of image wrong....dont do that
	else
	{
	cout<<"No valid mask type"<<endl;
	}
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

	return 0;
}