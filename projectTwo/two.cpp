#include <fstream>
#include "image.cpp"
#include <cstdlib>
#include <cstring>
#include <math.h>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <vector>
#include <iostream>

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

void average(string fileName, int size, string printName){
	int N, M, Q, temp, temp2, count;
	bool imageFormat;
	int max=0;

	double temp3;

	readImageHeader(fileName, N,M,Q, imageFormat);
	ImageType image(N,M,Q);
	readImage(fileName, image);

	ImageType newImage(image);

	int padValueX = (size-1)/2;
	int padValueY = (size-1)/2;
	ImageType paddedImage(N+padValueX*2, M+padValueY*2, Q);
	padImage(paddedImage, image, padValueX, padValueY);

	vector<int> maskArray(size*size, 1);

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
					temp += temp2;
					count++;
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

void gauss(string fileName, int size, string printName){
	int N, M, Q, temp, temp2, count;
	bool imageFormat;

	double temp3;
	double r, s;
	if(size == 7) s = 2.0 * 1.4 * 1.4;
	else s = 2.0 * 3.0 * 3.0;

	readImageHeader(fileName, N,M,Q, imageFormat);
	ImageType image(N,M,Q);
	readImage(fileName, image);

	ImageType newImage(image);

	int padValueX = (size-1)/2;
	int padValueY = (size-1)/2;
	ImageType paddedImage(N+padValueX*2, M+padValueY*2, Q);
	padImage(paddedImage, image, padValueX, padValueY);

	vector<double> maskArray(size*size);

	//build Guassian surface, this code is not mine, was copied from internet.
	double sum = 0.0;
	for (int x = -1*((size-1)/2); x <= ((size-1)/2); x++){
        for(int y = -1*((size-1)/2); y <= ((size-1)/2); y++){
            r = sqrt(x*x + y*y);
            maskArray[((x+((size-1)/2))*size)+y+((size-1)/2)] = (exp(-(r*r)/s))/(M_PI * s);
            sum += maskArray[((x+((size-1)/2))*size)+y+((size-1)/2)];
        }
    }
    // normalize the Kernel
    for(int i = 0; i < 5; ++i)
        for(int j = 0; j < 5; ++j)
            maskArray[(i*size)+j] /= sum;

    //end internet code

    int max = 0;
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
					temp += temp2*maskArray[count];
					count++;
				}
			}
			if (temp>max) max=temp;
			newImage.setPixelVal(x-padValueX, y-padValueY, temp+1);			
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

int main(){

	string fileName = "./images/lenna.pgm";
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


	gauss("./images/lenna.pgm", 7, "lennaGauss7.pgm");
	gauss("./images/lenna.pgm", 15, "lennaGauss15.pgm");
	average("./images/lenna.pgm", 7, "lennaaverage7.pgm");
	average("./images/lenna.pgm", 15, "lennaaverage15.pgm");
	gauss("./images/sf.pgm", 7, "sfGauss7.pgm");
	gauss("./images/sf.pgm", 15, "sfGauss15.pgm");
	average("./images/sf.pgm", 7, "sfaverage7.pgm");
	average("./images/sf.pgm", 15, "sfaverage15.pgm");
	
	return (0);
}