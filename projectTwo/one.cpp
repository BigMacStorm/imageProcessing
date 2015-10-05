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

void correlation(ImageType image, int sizeOfMaskX, int sizeOfMaskY, vector<int> maskArray, string printName){

	int N,M,Q,temp, temp2;
	int count;
	long long int temp3;

	//First thing is to make a copy of the image before padding anything.
	ImageType newImage(image);

	//now, pad the image depending on size of mask.
	image.getImageInfo(N, M, Q);
	int padValueX = (sizeOfMaskX-1)/2;
	int padValueY = (sizeOfMaskY-1)/2;
	ImageType paddedImage(N+padValueX*2, M+padValueY*2, Q);
	padImage(paddedImage, image, padValueX, padValueY);

	//iteration over image with mask now.
	//first two loops handle iteration across image
	//second two for loops handle the math
	int max = 0;
	for(int x=padValueX; x<N+padValueX; x++){
		for(int y=padValueY; y<M+padValueY; y++){
			temp = 0;
			count = 0;
			for(int a=x-padValueX; a<x+padValueX; a++){
				for(int b=y-padValueY; b<y+padValueY; b++){
					paddedImage.getPixelVal(a, b, temp2);
					temp += temp2*maskArray[count];
					count++;
				}
			}
			if(temp>max)max=temp;
			newImage.setPixelVal(x-padValueX, y-padValueY, temp);			
		}
	}
	newImage.getImageInfo(N,M,Q);
	for(int x=0; x<N; x++){
		for(int y=0; y<M; y++){
			newImage.getPixelVal(x, y, temp);
			temp3 = temp;
			temp3 *= 255;
			temp3 /= max;
			if(temp3 < 0) cout << temp3 << endl;
			temp = temp3;
			newImage.setPixelVal(x, y, temp);
		}
	}


	writeImage(printName, newImage);
}

void makeMask(string fileName, int& xSize, int& ySize, vector<int>& maskArray){
	int N, M, Q, temp;
	bool imageFormat;

	readImageHeader(fileName, N,M,Q, imageFormat);
	ImageType image(N,M,Q);
	readImage(fileName, image);

	xSize = N;
	ySize = M;

	maskArray.resize(N*M);

	for(int x=0; x<N; x++){
		for(int y=0; y<M; y++){
			image.getPixelVal(x, y, temp);
			maskArray[x*N+y] = temp;
			cout << temp << " ";
		}
		cout << endl;
	}

	cout << xSize << " " << ySize << endl;
}

int main(){

	string fileName = "./images/Image.pgm";
	int N, M, Q;
	bool imageFormat;

	int xSize, ySize;

	//Before you declare your Image Type Object, you have to run readImageHeader to get the info of the image
	//N,M,Q, and imageFormat are all passed by refrence.
	readImageHeader(fileName, N,M,Q,imageFormat);

	//Only after the readImageHeader is run, can you create your imageType Object in this format.
	ImageType myImage(N,M,Q);

	//Will read the image in and store the pixel value in myImage.pixelValue
	readImage(fileName,myImage);

	vector<int> padArray;
	makeMask("./images/Pattern.pgm", xSize, ySize, padArray);
	correlation(myImage, xSize, ySize, padArray, "output.pgm");

return (0);
}