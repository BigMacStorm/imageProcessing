#include <iostream>
#include <cstdlib>
#include "image.h"
#include "image.cpp"
#include <cstdlib>
#include <cstring>
#include <vector>
#include <fstream>

using namespace std;

void modify(ImageType& image, int colorNumber){
	int rows, cols, levels, temp;

	image.getImageInfo(rows, cols, levels);

	int divisor = levels/colorNumber;
	cout << divisor << endl;

	for(int i=0; i<rows; i++) {
		for(int j=0; j<cols; j++) {
			image.getPixelVal(i, j, temp);
			image.setPixelVal(i, j, (temp/divisor)*divisor);
		}
	}
}

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


int main(){

	//Lenna
	string fileName = "lenna.pgm";
	int N, M, Q;
	bool imageFormat;

	readImageHeader(fileName, N, M, Q, imageFormat);
	ImageType myImageLenna(N, M, Q);

	//128 colors
	readImage(fileName, myImageLenna);
	modify(myImageLenna, 128);
	writeImage("128ColorsLenna.pgm", myImageLenna);

	//32 colors
	readImage(fileName, myImageLenna);
	modify(myImageLenna, 32);
	writeImage("32ColorsLenna.pgm", myImageLenna);

	//8 colors
	readImage(fileName, myImageLenna);
	modify(myImageLenna, 8);
	writeImage("8ColorsLenna.pgm", myImageLenna);

	//2 colors
	readImage(fileName, myImageLenna);
	modify(myImageLenna, 2);
	writeImage("2ColorsLenna.pgm", myImageLenna);


	//Lenna
	fileName = "peppers.pgm";
	readImageHeader(fileName, N, M, Q, imageFormat);
	ImageType myImagePeppers(N, M, Q);

	//128 colors
	readImage(fileName, myImagePeppers);
	modify(myImagePeppers, 128);
	writeImage("128ColorsPeppers.pgm", myImagePeppers);

	//32 colors
	readImage(fileName, myImagePeppers);
	modify(myImagePeppers, 32);
	writeImage("32ColorsPeppers.pgm", myImagePeppers);

	//8 colors
	readImage(fileName, myImagePeppers);
	modify(myImagePeppers, 8);
	writeImage("8ColorsPeppers.pgm", myImagePeppers);

	//2 colors
	readImage(fileName, myImagePeppers);
	modify(myImagePeppers, 2);
	writeImage("2ColorsPeppers.pgm", myImagePeppers);

	return 0;
}