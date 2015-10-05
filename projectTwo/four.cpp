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

void padImage( ImageType& paddedImage, ImageType oldImage, int padValue);

void sharpenImage(ImageType oldImage, ImageType& sharpImage, string maskType,int padVal);

void saltAndPeppaNoise(ImageType& oldImage, int distortionPercent);

void medianFilter(ImageType oldImage, ImageType& medianImage, int padVal);

int main()
{

	/*
	The following is the order of steps needed to read and write an image.
	This code opens the lenna.pgm image, and then saves it again
	*/
	//Decliration of variables, latter the file name will be passed as an argument
	string fileName = "./images/sf.pgm";
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

	
	sharpenImage(paddedImage, sharpImage, "Laplacian", padValue);
	sharpenImage(paddedImage, sharpImage, "Sobel", padValue);
	sharpenImage(paddedImage, sharpImage, "Prewitt", padValue);
	
	

	

	
	


return (0);
}

/*
medianFilter:
Writen By: Jeremiah Berns
Description:	Will take an image, and apply the median filtering to it.  It will then output the image to a file.
*/


void medianFilter(ImageType padded, ImageType& medianImage, int padVal)
{
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

void saltAndPeppaNoise(ImageType& oldImage, int distortionPercent)
{
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



/*
SharpenImage:
Writen by: Jeremiah Berns
dependencies: image.h, image.cpp, math.h
Discription:	This funtion will take in a padded imageType opbject, the new image to store the sharp image,
		the value of the padding applied to the padded image, and the name of the sharpening to be 
		done.  It will perform the desired sharpening, if the selected sharpening type generates both
		a X, and Y gradient, then those images will be printed as well. 

*/

void sharpenImage(ImageType padded, ImageType& sharpImage, string maskType,int padVal)
{

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


/*
PadImage: 
Writen by: Jeremiah Berns
dependencies: Image.h, image.cpp
Discription: PadImage will take in 2 image objects, the old image, and the new image, which is the size of the old image,
		plus 2x the pad value.  This function will then place the image in the center of the padded image and return
		it.
*/

void padImage( ImageType& paddedImage, ImageType oldImage, int padValue)
{

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
	for(int i=padValue;i<N-padValue;i++)
	{
		for(int j = padValue;j<M-padValue;j++)
		{
			oldImage.getPixelVal(i-padValue,j-padValue,tempValue);
			paddedImage.setPixelVal(i,j,tempValue);
			
		}
		
	}


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



