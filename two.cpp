#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

//function to modify the image
//paramater decides how many equal "Buckets" there are for the grayscale values.
void modify(vector<int> sent, int colorNumber){
	//so there are 256 colors currently in the picture, and we need to reduce it to
	//colorNumber amount of colors. So we can simply create a piewheel type system, where
	//there will be colorNumber number of equal slices on the wheel.


	//Then we simply do a lookup for each cell, if it exists in each section of the piechart,
	//replace with which cell it exists in.
}

//function to load the image
void load(vector<int> sent){
	//blah
}

//function to save the image
void save(vector<int> sent){
	//blah
}


int main(){

	vector<int> imageData;
	imageData.resize(65536);

	load(imageData);
	modify(imageData, 256);
	save(imageData);

	return 0;
}


