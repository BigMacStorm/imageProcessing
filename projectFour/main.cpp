#include <math.h>
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <complex>
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

#include <math.h>


double ranf(double m = 1.0) {
    return (m*rand())/(double)RAND_MAX;
}


float box_muller(float m, float s)	/* normal random variate generator */
{				        /* mean m, standard deviation s */
	float x1, x2, w, y1;
	static float y2;
	static int use_last = 0;

	if (use_last)		        /* use value from previous call */
	{
		y1 = y2;
		use_last = 0;
	}
	else
	{
		do {
			x1 = 2.0 * ranf() - 1.0;
			x2 = 2.0 * ranf() - 1.0;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0 );

		w = sqrt( (-2.0 * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}

	return( m + y1 * s );
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

void fft(float data[], unsigned long nn, int isign){
	unsigned long n,mmax,m,j,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta;
	float tempr,tempi;
	
	n=nn << 1;
	
	j=1;

	for (i=1;i<n;i+=2) {
		if (j > i) {
			SWAP(data[j],data[i]);
			SWAP(data[j+1],data[i+1]);
		}
		m=n >> 1;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}	

	mmax=2;
	while (n > mmax) {
		istep=mmax << 1;
		theta=isign*(6.28318530717959/mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=n;i+=istep) {
				j=i+mmax;
				tempr=wr*data[j]-wi*data[j+1];
				tempi=wr*data[j+1]+wi*data[j];
				data[j]=data[i]-tempr;
				data[j+1]=data[i+1]-tempi;
				data[i] += tempr;
				data[i+1] += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}

	if (isign == 1)
	{
	for( int i=1; i<n+1;i++)
		{
		data[i] = data[i]/nn;
		}
	}	
}
#undef SWAP
/* (C) Copr. 1986-92 Numerical Recipes Software 0#Y". */

void fft2D(int N, int M, vector<vector<float> >& real_Fuv, vector<vector<float> >& imag_Fuv, int isign){

	vector<float> tempV;
	vector<vector<float> > combo;
	tempV.resize(M*2, 0);

	int count = 0;
	for(int x=0; x<N; x++){
		for(int y=0; y<M*2; y+=2){
			tempV[y] = real_Fuv[x][count];
			tempV[y+1] = imag_Fuv[x][count];
			count++;
		}
		count = 0;
		combo.push_back(tempV);
	}

	//do all of the rows first time
	//need to store into data array, run function, and then put back into vector
	float data[M*2];
	for(int x=0; x<N; x++){
		for(int y=0; y<M*2; y++){
			data[y] = combo[x][y];
		}
		fft(data-1, M, isign);	
		for(int y=0; y<M*2; y++){
			combo[x][y] = data[y];
		}
	}

	int counter2 = 0;
	//now we operate on each col
	for(int y=0; y<M*2; y+=2){
		for(int x=0; x<N; x++){
			data[counter2] = combo[x][y];
			counter2++;
			data[counter2] = combo[x][y+1];
			counter2++;
		}
		fft(data-1, M, isign);	
		counter2 = 0;
		for(int x=0; x<N; x++){
			combo[x][y] = data[counter2];
			counter2++;
			combo[x][y+1] = data[counter2];
			counter2++;
		}
		counter2 = 0;
	}

	//temp = combo;
	//Finally, we need to split the data up into real and imaginary vectors
	count = 0;
	for(int x=0; x<N; x++){
		for(int y=0; y<M*2; y+=2){
			real_Fuv[x][count] = combo[x][y];
			imag_Fuv[x][count] = combo[x][y+1];
			count++;
		}
		count = 0;
	}
}

int main(){
	bool homo = false;
	bool band = false;
	bool blur = false;
	bool inverse = false;
	bool wein = true;
	string fileName;
	if(homo) fileName = "girl.pgm"; 
	if(band) fileName = "boy_noisy.pgm"; 
	if(blur) fileName = "lenna.pgm";
	if(inverse || wein) fileName = "output4.pgm";
	int N, M, Q;
	int tempInt;
	bool imageFormat;
	
	readImageHeader(fileName, N,M,Q,imageFormat);
	ImageType myImage(N,M,Q);
	readImage(fileName,myImage);

	vector<vector<float> > real;
	vector<vector<float> > image;
	vector<vector<float> > magnitude;
	vector<vector<float> > huv;

	vector<float> temp;
	vector<float> zeros;
	temp.resize(M, 0);
	zeros.resize(M, 0);

	int count = 0;

	//create empty imaginary and real from the image.
	for(int x=0; x<N; x++){
		for(int y=0; y<M; y++){
			myImage.getPixelVal(x, y, tempInt);
			if(band || blur || inverse || wein) tempInt = tempInt*pow(-1, (x+y));
			if(homo) tempInt = log(tempInt);
			temp[y] = tempInt;
		}
		real.push_back(temp);
		image.push_back(zeros);
	}


	//call the function
	fft2D(N, M, real, image, 1);
	
	if(homo){
		//centers the real and imaginary frequencies
		for(int x=0; x<N; x++){
			for(int y=0; y<M; y++){
				real[x][y] = real[x][y]*pow(-1,(x+y));
				image[x][y] = image[x][y]*pow(-1,(x+y));
			}
		}

		//applies the homomorphic filter
		double yh,yl,doh;

		yh = 1.9;
		yl = 0.9;
		doh = 1.8;
		
		for(int x=0; x<N; x++){
			for(int y=0; y<M; y++){
				real[x][y] = real[x][y]*  ((yh-yl) *  (  1-exp  (-1*( (pow(x,2)+pow(y,2)) / pow(doh,2) ) ) )+yl);
				image[x][y] = image[x][y]*  ((yh-yl) *  (  1-exp  (-1*( (pow(x,2)+pow(y,2)) / pow(doh,2) ) ) )+yl);
			}
		}

		//uncenters the frequencies
		for(int x=0; x<N; x++){
			for(int y=0; y<M; y++){
				real[x][y] = real[x][y]*pow(-1,(x+y));
				image[x][y] = image[x][y]*pow(-1,(x+y));
			}
		}
	}
	

	if(band){
		//band reject code
		magnitude = real;
		int bandRange;
		for(int i=0;i<N;i++)
		{
			for(int j=0;j<M;j++)
			{
			bandRange = sqrt(pow((i-255),2)+pow((j-255),2));
			if(bandRange< 38 && bandRange >33)
				{
				magnitude[i][j] =0;
				real[i][j] = 0;
				}
			}

		}	
	}
	cout << "asdf" << endl;

	if(blur || inverse || wein){
		complex<float> fake = sqrt(complex<float>(-1,0));
		//need to generate H(u,v)
		complex <float> answer;
		float T, a, b, group, k;
		T = 1;
		a = 0.1;
		b = 0.1;
		k = .001;
		huv = real;
		for(int x=0; x<N; x++){
			for(int y=0; y<M; y++){
				group = (M_PI*((x*a)+(y*b)));
				answer = (T/group);
				answer = answer * sin(group) * exp(- fake*group);
				huv[x][y] = answer.real() * pow(-1,(x+y));
				if(x == 0 || y == 0)
					huv[x][y] = 1.0;
				if(blur) real[N-x-1][M-y-1] *= huv[x][y];
				if(inverse) real[N-x-1][M-y-1] /= huv[x][y];
				if(wein) real[N-x-1][M-y-1] = ((huv[x][y]*huv[x][y])/(huv[x][y]*huv[x][y]+k))*(real[N-x-1][M-y-1]/huv[x][y]);
			} 
		}		
	}
	
	//call again with inverse now
	fft2D(N, M, real, image, -1);

	for(int x=0; x<N; x++){
		for(int y=0; y<M; y++){
			if (band || blur || wein || inverse) real[x][y] = real[x][y]*pow(-1,(x+y));  
			if (homo) real[x][y] = exp(real[x][y]);  
			if (blur) real[x][y] = real[x][y] + box_muller(0,10);
		}
	}
	//calc the normal
	double max = 0;
	double min = 0;
	for(int x=0; x<N; x++){
		for(int y=0; y<M; y++){
			if(real[x][y] < min) min = real[x][y];
			if(real[x][y] > max) max = real[x][y];
		}
	}

	double temp3;	
	cout << "min: " << min << endl;
	cout << "max: " << max << endl;

	for(int x=0; x<N; x++){
		for(int y=0; y<M; y++){
			temp3 = (real[x][y]+(min*-1))/double (max + (min*-1));
			temp3 *= 255.0;
			myImage.setPixelVal(x,y,(int)temp3);
			if(temp3 < 0 || temp3 > 255) cout << temp3 << endl;
		}
	}	

	writeImage("output4.pgm", myImage);
	
	
	return 0;
}
