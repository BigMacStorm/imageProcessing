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
shrink Image funtion.
Writen By Jeremiah Berns.
Dependincies: image.h, image.cpp
Discription: Will take in the old image, and the new image, and the pixel value
	    based apon the shrink value passed to it.  It will place that value
	    from the old image into the new image, then save the new image with
   	    the passed in file name. 
*/
void shrinkImage(ImageType oldImage, ImageType& newImage, int shrinkVal, string newImageFname)
{
	//Variable decliration
	int rows, col, Q, tempValue;
	

	//Variable setting
	oldImage.getImageInfo(rows, col, Q);

	for(int i=0; i<rows;i++)
	  {
	    for(int j=0;j<col;j++)
	      {
		if(i%shrinkVal == 0 && j%shrinkVal ==0)
		  {
		    oldImage.getPixelVal(i,j, tempValue);
		    newImage.setPixelVal(i/shrinkVal,j/shrinkVal,tempValue);
		  }
	      }
	    
	  }

	writeImage(newImageFname, newImage);
}

/*
Expand image function
Writen by: Jeremiah Berns
Dependincies, image.cpp, image.h
Discription: Will accept the shrunken image, the grow size of the image, and then
	     expand the image back to 256x256
*/
void expandImage(ImageType oldImage, ImageType& newImage, int growVal, string newImageName)
{
  //Variable decliration
    int rows, cols, Q, tempValue;
	

  //Variable setting
    oldImage.getImageInfo(rows, cols, Q);

    for(int i=0;i<rows;i++)
      {
	for(int j=0;j<cols;j++)
	  {
	  oldImage.getPixelVal(i,j, tempValue);
	  for(int k=0;k<growVal;k++)
	    {
	      for(int l=0;l<growVal;l++)
	        {
		newImage.setPixelVal(i*growVal+k,j*growVal+l,tempValue);
		}
	    }
	  }
      }

  writeImage(newImageName, newImage);
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

	//shrinking images
	ImageType newImageLenna(128,128,255);
	shrinkImage(myImageLenna, newImageLenna, 2, "SampleBy2Lenna.pgm");
	ImageType newImage2Lenna(64,64,255);
	shrinkImage(myImageLenna, newImage2Lenna, 4, "SampleBy4Lenna.pgm");
	ImageType newImage3Lenna(32,32,255);
	shrinkImage(myImageLenna, newImage3Lenna, 8, "SampleBy8Lenna.pgm");

	//expanding images back out	
	ImageType newImage4Lenna(256,256,255);
	expandImage(newImageLenna, newImage4Lenna, 2, "GrowBy2Lenna.pgm");
	expandImage(newImage2Lenna, newImage4Lenna, 4, "GrowBy4Lenna.pgm");
	expandImage(newImage3Lenna, newImage4Lenna, 8, "GrowBy8Lenna.pgm");


	fileName = "peppers.pgm";
	readImageHeader(fileName, N,M,Q,imageFormat);
	ImageType myImagePeppers(N,M,Q);
	readImage(fileName,myImagePeppers);

	writeImage("pepperstest.pgm", myImagePeppers);

	//shrinking images
	ImageType newImagePeppers(128,128,255);
	shrinkImage(myImagePeppers, newImagePeppers, 2, "SampleBy2Peppers.pgm");
	ImageType newImage2Peppers(64,64,255);
	shrinkImage(myImagePeppers, newImage2Peppers, 4, "SampleBy4Peppers.pgm");
	ImageType newImage3Peppers(32,32,255);
	shrinkImage(myImagePeppers, newImage3Peppers, 8, "SampleBy8Peppers.pgm");

	//expanding images back out	
	ImageType newImage4Peppers(256,256,255);
	expandImage(newImagePeppers, newImage4Peppers, 2, "GrowBy2Peppers.pgm");
	expandImage(newImage2Peppers, newImage4Peppers, 4, "GrowBy4Peppers.pgm");
	expandImage(newImage3Peppers, newImage4Peppers, 8, "GrowBy8Peppers.pgm");

	return 0;
}