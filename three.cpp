#include <iostream>
#include <cstdlib>
#include "image.h"
#include "image.cpp"
#include <cstdlib>
#include <cstring>
#include <vector>
#include <fstream>

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
Histogram Equalization function
Written by: Jeremiah Berns
Dependincies:image.h, image.cpp
Discription:  This function will perform the histogram equalization algorithem to the oldImage
             and will output the newImage with the given newImageName.  
*/
void histogramEq(ImageType oldImage, ImageType& newImage, string newImageName)
{
  int rows, cols, Q, pixelValue, pixelCount;
  oldImage.getImageInfo(rows,cols,Q);
  pixelCount = rows*cols;
  int adjustedHistogram[Q];
  double histogramArray[Q], equalizedHistogram[Q];
  double probabilityArray[Q], cumulativeProbability[Q], probTotal=0;
 

  for (int i = 0; i<Q;i++)
    {
    histogramArray[i] = 0;
    equalizedHistogram[i] = 0;

    }

  for(int i=0; i<rows;i++)
    {
      for(int j=0; j<cols;j++)
        {
	  oldImage.getPixelVal(i,j,pixelValue);
  	  histogramArray[pixelValue]+=1;
	}
    }

  for(int i=0;i<Q;i++)
    {
     probTotal+= histogramArray[i]/pixelCount;
    
     cumulativeProbability[i] = probTotal;
     cumulativeProbability[i] = cumulativeProbability[i]*255;
     adjustedHistogram[i] = cumulativeProbability[i];
     cout<<adjustedHistogram[i]<<endl;
    }

  for(int i=0; i<rows;i++)
    {
      for(int j=0; j<cols;j++)
        {
	  oldImage.getPixelVal(i,j,pixelValue);
  	  newImage.setPixelVal(i,j,adjustedHistogram[pixelValue-1]);
	}
    }

  writeImage(newImageName, newImage);
}

int main(){

	//f16
	string fileName = "f_16.pgm";
	int N, M, Q;
	bool imageFormat;

	//Before you declare your Image Type Object, you have to run readImageHeader to get the info of the image
	//N,M,Q, and imageFormat are all passed by refrence.
	readImageHeader(fileName, N,M,Q,imageFormat);

	//Only after the readImageHeader is run, can you create your imageType Object in this format.
	ImageType myImageF16(N,M,Q);

	//Will read the image in and store the pixel value in myImage.pixelValue
	readImage(fileName,myImageF16);

	//Equlize an image
	ImageType newImageF16(256,256,255);
	histogramEq(myImageF16, newImageF16, "EQF16.pgm");

	//Boat
	fileName = "boat.pgm";
	readImageHeader(fileName, N,M,Q,imageFormat);
	ImageType myImageBoat(N,M,Q);

	readImage(fileName,myImageBoat);

	ImageType newImageBoat(256,256,255);
	histogramEq(myImageBoat, newImageBoat, "EQBoat.pgm");

	return 0;
}