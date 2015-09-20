#include <iostream>
#include <cstdlib>
#include "image.h"
#include <fstream>
#include "image.cpp"
#include <cstdlib>
#include <cstring>



using namespace std;

//Reads the image into a 2d array 
void readImage(string fname, ImageType& image);

//writes the image from the image class to a file specified by fname
void writeImage(string fname, ImageType& image);

//reads meta data of the image.
void readImageHeader(string fname, int& N, int& M, int& Q, bool& type);

//image shrink function
void shrinkImage(ImageType oldImage, ImageType& newImage, int shrinkVal, string newImageFname);

//expand image function
void expandImage(ImageType oldImage, ImageType& newImage, int growVal, string newImageFName);

void histogramEq(ImageType oldImage, ImageType& newImage, string newImageName);

int main()
{

	/*
	The following is the order of steps needed to read and write an image.
	This code opens the lenna.pgm image, and then saves it again
	*/
	//Decliration of variables, latter the file name will be passed as an argument
	string fileName = "lenna.pgm";
	int N, M, Q;
	bool imageFormat;
	//Before you declare your Image Type Object, you have to run readImageHeader to get the info of the image
	//N,M,Q, and imageFormat are all passed by refrence.
	readImageHeader(fileName, N,M,Q,imageFormat);
	//Only after the readImageHeader is run, can you create your imageType Object in this format.
	ImageType myImage(N,M,Q);
	//Will read the image in and store the pixel value in myImage.pixelValue
	readImage(fileName,myImage);
	//will Write the image in myImage into the given new file name.
	
	//For testing we will shrink to a 128x128 image


	//input the new size of the image
	ImageType newImage(256,256,255);

	histogramEq(myImage, newImage, "EQLenna.pgm");

	//for the shrink value aka that 8 right there, do the original image size divided by the new image size
	//shrinkImage( myImage, newImage, 8, "lenna32x32.pgm");
	


return (0);
}



//readimage Function
void readImage(string fname, ImageType& image)
{
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
 if ( (header[0]!=80) ||    /* 'P' */
      (header[1]!=53) ) {   /* '5' */
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



 //
 // Convert the unsigned characters to integers
 //

 int val;



 for(i=0; i<N; i++)
   for(j=0; j<M; j++) {

     val = (int)charImage[i*M+j];

     image.setPixelVal(i, j, val);     
   }



}




//Write image function
void writeImage(string fname, ImageType& image)
{
 int i, j;
 int N, M, Q;
 unsigned char *charImage;
 ofstream ofp;

 image.getImageInfo(N, M, Q);

 charImage = (unsigned char *) new unsigned char [M*N];

 // convert the integer values to unsigned char

 int val;

 for(i=0; i<N; i++)
   for(j=0; j<M; j++) {
     image.getPixelVal(i, j, val);
     charImage[i*M+j]=(unsigned char)val;
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
void readImageHeader(string fname, int& N, int& M, int& Q, bool& type)
{
 int i, j;
 unsigned char *charImage;
 char header [100], *ptr;
 ifstream ifp;

 ifp.open(fname.c_str(), ios::in | ios::binary);

 if (!ifp) {
   cout << "Can't read image: " << fname << endl;
   exit(1);
 }

 // read header

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



