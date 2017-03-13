//#include "stdafx.h"
#include <iostream>
#include <fstream>
#include<conio.h>
#include <opencv\cv.h>
#include <opencv\cxcore.h>
#include <opencv\highgui.h>
#include <opencv2\opencv.hpp>
#include<vector>

using namespace std;
using namespace cv;
int main()
{
	IplImage *img = cvLoadImage("C:\\TableRecognition\\Data\\unlv-table-png\\1060_146.png");
	CvSize imgSize = cvGetSize(img);

	//Gray scale
	IplImage *gray = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	cvCvtColor(img, gray, CV_RGB2GRAY);

	//binary
	IplImage *binary = cvCreateImage(cvSize(img->width, img->height), 8, 1);
	cvThreshold(gray, binary, 5, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

	double pixel;
	int count = 0;
	int height = binary->height;
	int *linecount = new int[height];
	int width = binary->width;
	int *wordcount = new int[width];
	
	// Array to store the black pixel of horizontal Histogram
	// Dynamically Allocating memory 
	int *HorizontalHistogram = new int[height];
	
	// initializing the array with 0 
	for (int i = 0; i <height; i++)
	{
		HorizontalHistogram[i] = 0;
	}
	
	// Assigning the values for black pixels and printing 
	//printf("\t\tHorizontal Bin Values \n");
	for (int j = 0; j<(binary->height); j++) {
		count = 0;
		for (int i = 0; i<(binary->width); i++) {
			pixel = cvGetReal2D(binary, j, i);
			if (pixel == 0) {
				HorizontalHistogram[j]++;
				count++;
			}
		}
		//cout <<"Total Black Pixels at x="<<j<<" are : "<<count<<endl;
	}
	
	

	// Array to store the black pixel of horizontal Histogram
	// Dynamically Allocating memory 

	int *VerticalHistogram = new int[width];
	
	// initializing the array with 0 

	for (int i = 0; i < width; i++)
	{
		VerticalHistogram[i] = 0;
	}
	
	// Assigning the values for black pixels and printing 
	//printf("\t\tVertical Bin Values \n");
	for (int j = 0; j<(binary->width); j++) {
		count = 0;
		for (int i = 0; i<(binary->height); i++) {
			pixel = cvGetReal2D(binary, i, j);
			if (pixel == 0) {
				VerticalHistogram[j]++;
				count++;
			}
		}
		//cout << "Total Black Pixels at y=" << j << " are : " << count << endl;
	}

	int Hhist_w = height; int Hhist_h = 300;
	int Vhist_w = height; int Vhist_h = 300;
	float range[] = { 0,255 };
	float *ranges[] = { range };
	int Hhist_size = { binary->height };
	int Vhist_size = { binary->width };
	float min_value, max_value = 0;
	IplImage *histImage1 = cvCreateImage(cvSize(height, 300), 8, 1);
	IplImage *histImage2 = cvCreateImage(cvSize(width, 300), 8, 1);
	cvSet(histImage1, cvScalarAll(255), 0);
	cvSet(histImage2, cvScalarAll(255), 0);
	CvHistogram *hist = cvCreateHist(1, &Hhist_size, CV_HIST_ARRAY, ranges, 1);
	int bin_w1 = cvRound((double)histImage1->width / Hhist_size);
	int bin_w2 = cvRound((double)histImage2->width / Vhist_size);

	// drawing Horizontal Histogram of the Image

	for (int i = 0; i <height; i++)
	{
		cvLine(histImage1, cvPoint(bin_w1*(i), Hhist_h),
			cvPoint(bin_w1*(i), Hhist_h - HorizontalHistogram[i]),
			cvScalar(0, 0, 0), 1, 8, 0);
	}

	// drawing Vertical Histogram of the Image 

	for (int i = 0; i < width; i++)
	{
		cvLine(histImage2, cvPoint(bin_w2*(i), Vhist_h),
			cvPoint(bin_w2*(i), Vhist_h - VerticalHistogram[i]),
			cvScalar(0, 0, 0), 1, 8, 0);
	}
	

	// displaying the results
	cvNamedWindow("Binary:", CV_WINDOW_NORMAL);
	cvShowImage("Binary:", binary);
	cvNamedWindow("HorizontalHistogram:", CV_WINDOW_NORMAL);
	cvShowImage("HorizontalHistogram:", histImage1);
	cvNamedWindow("Vertical_Histogram", CV_WINDOW_NORMAL);
	cvShowImage("Vertical_Histogram", histImage2);
	
	//displaying both histograms in the same window
	Mat img1 = histImage1;
	Mat img2 = histImage2;
	cvtColor(img1, img1, CV_GRAY2BGR);
	cvtColor(img2, img2, CV_GRAY2BGR);
	Size sz1 = img1.size();
	Size sz2 = img2.size();
	Mat im3(sz1.height, sz1.width + sz2.width+10, CV_8UC3);
	Mat left(im3, Rect(0, 0, sz1.width, sz1.height));
	img1.copyTo(left);
	Mat right(im3, Rect(sz1.width+10, 0, sz2.width, sz2.height));
	img2.copyTo(right);
	namedWindow("Horizontal_Vertical", CV_WINDOW_NORMAL);
	resizeWindow("Horizontal_Vertical", 800, 500);
	imshow("Horizontal_Vertical", im3);
	
	cvWaitKey(0);
	
	//writing the result to csv file
	ofstream csvFile("H_V_Hist.csv", ios::app);
	csvFile << "Horizontal,vertical\n";
	for (int i = 0; i < width; i++)
	{
		csvFile << HorizontalHistogram[i] << ","<<endl;
	}
	csvFile.close();
	
	// releasing the memory and destroying preview windows
	// after a key is pressed
	delete[] HorizontalHistogram;
	delete[] VerticalHistogram;

	cvDestroyWindow("Binary:");
	cvReleaseImage(&binary);
	cvDestroyWindow("HorizontalHistogram:");
	cvReleaseImage(&histImage1);
	cvDestroyWindow("Vertical_Histogram");
	cvReleaseImage(&histImage2);
	_getch();
	return 0;
}