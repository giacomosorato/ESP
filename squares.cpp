#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>
#include <iostream>

using namespace cv;
RNG rng(12345);


int main()
{
	cv::Mat dst;
	cv::Mat src;

	// read the img
	src = imread("../quadri_dataset/quadro_donzelli.jpg", 1);
	if (src.empty())
		return -1;

	// Convert to grayscale
	cv::Mat gray;
	cv::cvtColor(src, gray, CV_BGR2GRAY);

	// Convert to binary image using Canny
	cv::Mat bw;
	cv::Canny(gray, bw, 0, 50, 5);
	
	// Find contours
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	vector<Vec4i> hierarchy;

	// The array for storing the approximation curve
	std::vector<cv::Point> approx;

	// Rects extraction
	vector<vector<Point> > contours_poly( contours.size() );
	vector<Rect> bound_rect( contours.size() );

	for (int i = 0; i < contours.size(); i++)
	{
		// poligon approximation
		approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
		// extraciont only poligon=rect
		bound_rect[i] = boundingRect( Mat(contours_poly[i]) );	
	} 

	// Max rect extraction from all the rects

	int largest_area=0;				//selection by max area
	int largest_contour_index=0;
	cv::Rect big_rect; // The variable to store max rect

	for( int i = 0; i< contours.size(); i++ ) // iterate through each contour.
	{	
		if (i==0)
			big_rect=bound_rect[i];
		else if (big_rect.area() < bound_rect[i].area())
			big_rect=bound_rect[i];
	}

	// cut the image to have only the quadro
	src(big_rect).copyTo(dst);

	cv::imshow("src", src);
	cv::imshow("dst", dst);
    waitKey();
	return 0;
}