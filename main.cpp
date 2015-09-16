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

int main()
{

/*
The following is the order of steps needed to read and write an image.
This code opens the lenna.pgm image, and then saves it again
*/


//Decliration of variables, latter the file name will be passed as an argument
string fileName = "lenna.pgm";

//Int values needed for image construction, N and M are the dimenstions of the array

//Q is a letter that im sure surves some purpose that im just not sure of right now. same for imageFormat.

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
writeImage("newImage.pgm", myImage);


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
cout <<N<<endl<<M<<endl;


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
