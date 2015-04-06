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
	cv::Mat src;
	cv::Mat dst;
	cv::Mat paintable;
	int end=false;

	// read the img
	src = imread("C:/Users/giki/Pictures/quadri_dataset/quadro_tre.jpg", 1);
	if (src.empty())
		return -1;

	cv::Size s = src.size();
	src.copyTo(paintable);

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
	vector<vector<Point>> contours_poly( contours.size() );
	vector<Rect> bound_rect( contours.size() );
	vector<vector<Rect>> good_rect;
	int good_rect_index=0;
	for (int i = 0; i < contours.size(); i++)
	{
		// poligon approximation
		approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
		// extraciont only poligon=rect
		bound_rect[i] = boundingRect( Mat(contours_poly[i]) );	
		//-------------
		if (bound_rect[i].area() > (s.area()*0.30) )
		{
			vector<Rect> vector;
			vector.push_back(bound_rect[i]);
			good_rect.push_back(vector); // estrapolo tutti i rettangoli buoni
			good_rect_index++;
			Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
			rectangle( paintable, bound_rect[i].tl(), bound_rect[i].br(), color, 2, 8, 0 );
		}
	} 

	if (good_rect.size()==0)
		// non si sono trovati rettangoli compatibili, forse 
		// l'immagine è una parte interna di quadro
	{
		dst=src;
		end=true;
	}

	//cv::imshow("src", paintable);
	//waitKey();

	if (end==false)
	{
		// adesso di questi rettangoli buoni devo prendere quelli che effettivamente costituiscono il mio quadro
		for (int j = 0; j < good_rect_index; j++)
		{
			Rect confront = good_rect[j][0];
			int x_center = confront.x+confront.width/2;
			int y_center = confront.y+confront.height/2;

			for (int i = 0; i < good_rect_index; i++)
			{
				if (j==i)
					continue;
				else if (x_center > good_rect[i][0].x + good_rect[i][0].width/2 - good_rect[i][0].width*0.05)
						if (x_center < good_rect[i][0].x + good_rect[i][0].width/2+good_rect[i][0].width*0.05)
							if (y_center > good_rect[i][0].y+good_rect[i][0].height/2-good_rect[i][0].height*0.05)
								if (y_center < good_rect[i][0].y+good_rect[i][0].height/2+good_rect[i][0].height*0.05)
									good_rect[i].push_back(confront);
			}// sto codicione che andrà semplificato praticamente controlla il centro dei rettangoli e definisce
			 // due rettangoli come buoni se hanno lo stesso centro (con un minimo di errore accettabile)
			 // infatti ho deciso che per me un quadro è una successione di rettangoli con lo stesso centro
			 // (cornice interna + cornice esterna + inizio tela = 3 rettangoli con lo stesso centro),
			 // più o meno funziona sempre, a meno di cornici strane.

		}
		// Ho un vettore di vettori contenente tutti gli accoppiamenti, prendo l'accoppiamento che 
		// contiene i rettangoli più grandi
		vector<Rect> quadro;
		for( int i = 0; i< good_rect_index; i++ ) // iterate through each contour.
		{	
			if (i==0)
				quadro=good_rect[i];
			else if (quadro.size() < good_rect[i].size())
				{
					quadro=good_rect[i];
				}
		}

		int size = quadro.size();

		for (int i=0; i < size; i++)
		{	
			Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
			rectangle( paintable, quadro[i].tl(), quadro[i].br(), color, 2, 8, 0 );
		}

		// Max rect extraction from the quadro
		int largest_contour_index=0;
		cv::Rect big_rect; // The variable to store max rect

		for( int i = 0; i < quadro.size(); i++ ) // iterate through each contour.
		{	
			if (i==0)
				big_rect=quadro[i];
			else if (big_rect.area() < quadro[i].area())
				{
					big_rect=quadro[i];
					largest_contour_index=i;
				}
		}

		// cut the image to have only the quadro
		src(big_rect).copyTo(dst); 

	}
	cv::imshow("src", src);
	cv::imshow("src_with rect", paintable);
	cv::imshow("dst", dst);
    waitKey();
	return 0;
}			