
#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <cmath>
#include <string>
#include <fstream>

using namespace cv;
using namespace std;
int cCount = 0;
int snd = 0;
vector<Point> Pvec;
vector<Mat> Mvec;
float histo[33] = { 0, };
float histo2[33] = { 0, };
int Pcount = 0;

float intensity(int r, int g, int b) {
	return  0.2125 * r + 0.7152 * g + 0.0722 * b;
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	Mat im = *(Mat*)(userdata);

	if (cCount < 4) {
		if (event == EVENT_LBUTTONDOWN)
		{
			cCount++;
			cout << "왼쪽 마우스 버튼 클릭.. 좌표 = (" << x << ", " << y << ")" << endl;
			if (x < 8)
				x = 8;
			if (y < 8)
				y = 8;
			if (x > 591)
				x = 591;
			if (y > 791)
				y = 791;
			rectangle(im, Rect(Point(x - 7, y - 7), Point(x + 7, y + 7)), Scalar(0, 254, 0), 2, 8, 0);
			
			//Mat gx, gy;
			//Sobel(im(Rect(Point(x - 3, y - 3), Point(x + 3, y + 3))), gx, CV_32F, 1, 0, 1);
			//Sobel(im(Rect(Point(x - 3, y - 3), Point(x + 3, y + 3))), gy, CV_32F, 0, 1, 1);
			//Mat mag, angle;
			//cartToPolar(gx, gy, mag, angle, 1);
			//imshow("new1", mag);
			//imshow("new2", angle);
			if (snd == 0) {
				for (int i = 0; i < 15; i++) {
					for (int j = 0; j < 15; j++) {
						//dx
						float dx;
						
						uchar b = im.at<Vec3b>(y - 7 + i + 1, x - 7 + j)[0];
						uchar g = im.at<Vec3b>(y - 7 + i + 1, x - 7 + j)[1];
						uchar r = im.at<Vec3b>(y - 7 + i + 1, x - 7 + j)[2];
						dx = intensity(r, g, b);

						b = im.at<Vec3b>(y - 7 + i - 1, x - 7 + j)[0];
						g = im.at<Vec3b>(y - 7 + i - 1, x - 7 + j)[1];
						r = im.at<Vec3b>(y - 7 + i - 1, x - 7 + j)[2];
						dx = dx - intensity(r, g, b);
						//dy
						float dy;
						b = im.at<Vec3b>(y - 7 + i, x - 7 + j - 1)[0];
						g = im.at<Vec3b>(y - 4 + i, x - 7 + j - 1)[1];
						r = im.at<Vec3b>(y - 7 + i, x - 7 + j - 1)[2];
						dy = intensity(r, g, b);

						b = im.at<Vec3b>(y - 7 + i, x - 7 + j + 1)[0];
						g = im.at<Vec3b>(y - 7 + i, x - 7 + j + 1)[1];
						r = im.at<Vec3b>(y - 7 + i, x - 7 + j + 1)[2];
						dy = dy - intensity(r, g, b);
						//cout << dx << ", " << dy<<"\n";
						float gori = atan2(dy, dx) * 57.2957951 + 180;
						float gmag = sqrt(dx * dx + dy * dy);
						int indx = (gori / 45);
						
						float binrate = (gori-indx*45)/45;
						float binrate2 = 1 - binrate;

						float bin = gmag*binrate2;

						indx = indx + (8 * Pcount);
						int indx2 = (indx + 1) % 8 + (Pcount * 8);
						//cout << gori << ", " << gmag << "\n";

	
						histo[indx] = histo[indx] + bin;
						bin = gmag * binrate;
						histo[indx2] = histo[indx2] + bin;
							
					

					}
				}
				Pcount++;
			
			}
			else if (snd == 1) {
				for (int i = 0; i < 15; i++) {
					for (int j = 0; j < 15; j++) {
						//dx
						float dx;

						uchar b = im.at<Vec3b>(y - 7 + i + 1, x - 7 + j)[0];
						uchar g = im.at<Vec3b>(y - 7 + i + 1, x - 7 + j)[1];
						uchar r = im.at<Vec3b>(y - 7 + i + 1, x - 7 + j)[2];
						dx = intensity(r, g, b);

						b = im.at<Vec3b>(y - 7 + i - 1, x - 7 + j)[0];
						g = im.at<Vec3b>(y - 7 + i - 1, x - 7 + j)[1];
						r = im.at<Vec3b>(y - 7 + i - 1, x - 7 + j)[2];
						dx = dx - intensity(r, g, b);
						//dy
						float dy;
						b = im.at<Vec3b>(y - 7 + i, x - 7 + j - 1)[0];
						g = im.at<Vec3b>(y - 4 + i, x - 7 + j - 1)[1];
						r = im.at<Vec3b>(y - 7 + i, x - 7 + j - 1)[2];
						dy = intensity(r, g, b);

						b = im.at<Vec3b>(y - 7 + i, x - 7 + j + 1)[0];
						g = im.at<Vec3b>(y - 7 + i, x - 7 + j + 1)[1];
						r = im.at<Vec3b>(y - 7 + i, x - 7 + j + 1)[2];
						dy = dy - intensity(r, g, b);
						//cout << dx << ", " << dy<<"\n";
						float gori = atan2(dy, dx) * 57.2957951 + 180;
						float gmag = sqrt(dx * dx + dy * dy);
						int indx = (gori / 45);

						float binrate = (gori - indx * 45) / 45;
						float binrate2 = 1 - binrate;

						float bin = gmag * binrate2;

						indx = indx + (8 * Pcount);
						int indx2 = (indx + 1) % 8 + (Pcount * 8);
						//cout << gori << ", " << gmag << "\n";


						histo2[indx] = histo2[indx] + bin;
						bin = gmag * binrate;
						histo2[indx2] = histo2[indx2] + bin;


					}
				}
				Pcount++;
			}

			Pvec.push_back(Point(x, y));

			//Mvec.push_back(mag);
			//Mvec.push_back(angle);
			//rectangle(im, Rect(400, 400, 100, 100), Scalar(0, 0, 255), 1, 8, 0);
			cv::imshow("image", im);

		}
	}
	
}

void displayType(cv::Mat& img, const std::string type, std::vector<cv::Point>& contour)
{
	cv::Rect rect = cv::boundingRect(contour);
	cv::Point top_left(rect.x, rect.y);
	cv::putText(img, type, top_left, 0, 0.5, CV_RGB(0, 0, 0));
}

double computeCosine(cv::Point pointA, cv::Point pointB, cv::Point anglePoint)
{
	std::pair<double, double> v1(pointA.x - anglePoint.x, pointA.y - anglePoint.y);
	std::pair<double, double> v2(pointB.x - anglePoint.x, pointB.y - anglePoint.y);
	return (v1.first * v2.first + v1.second * v2.second) /
		sqrt((pow(v1.first, 2) + pow(v1.second, 2)) * (pow(v2.first, 2) + pow(v2.second, 2)));
}

int main()
{
	string f_name;
	cout << "enter file name.. (1st.jpg)" << "\n";

	getline(cin, f_name, '\n');

	//cin << f_name;
	cv::Mat imgori = cv::imread(f_name);
	Mat img;
	GaussianBlur(imgori, imgori, Size(5, 5), 0.0);
	resize(imgori, img, Size(600, 800));

	if (img.empty())
		return EXIT_FAILURE;
	


	cv::imshow("image", img);
	setMouseCallback("image", CallBackFunc,&img);
	while (cCount != 4) {
		cv::waitKey(30);
	}
	cv::waitKey(500);
	destroyWindow("image");
	
	cout << "enter 2nd file name.. (2nd.jpg)" << "\n";
	cCount = 0;
	snd = 1;
	Pcount = 0;

	getline(cin, f_name, '\n');
	cv::Mat imgori2 = cv::imread(f_name);
	Mat img2;
	GaussianBlur(imgori2, imgori2, Size(5, 5), 0.0);
	resize(imgori2, img2, Size(600, 800));
	if (img2.empty())
		return EXIT_FAILURE;
	cv::imshow("image", img2);
	setMouseCallback("image", CallBackFunc, &img2);
	while (cCount != 4) {
		cv::waitKey(30);
	}
	destroyWindow("image");
	cv::waitKey(500);
	//snd = 2;
	cout << "=========================\n";
	
	for (int i = 0; i < 4; i++) {
		cout << "1st.jpg => Point_" << i + 1 << " Histogram\n";
		cout << "[0~1/4/*pi]: " << histo[i * 8] << "  [1/4*pi~1/2*pi]: " << histo[i * 8 + 1];
		cout << "  [1/2*pi~3/4*pi]: " << histo[i * 8 + 2] << "  [3/4*pi~1*pi]: " << histo[i * 8 + 3] << "\n";
		cout << "[1*pi~5/4*pi]: " << histo[i * 8 + 4] << "  [5/4*pi~3/2*pi]: " << histo[i * 8 + 5];
		cout << "  [3/2*pi~7/4*pi]: " << histo[i * 8 + 6] << "  [7/4*pi~2*pi]: " << histo[i * 8 + 7] << "\n";
		cout << "--------------------------\n";
	}
	cout << "=========================\n";
	for (int i = 0; i < 4; i++) {
		cout << "2nd.jpg => Point_" << i + 1 << " Histogram\n";
		cout << "[0~1/4/*pi]: " << histo2[i * 8] << "  [1/4*pi~1/2*pi]: " << histo2[i * 8 + 1];
		cout << "  [1/2*pi~3/4*pi]: " << histo2[i * 8 + 2] << "  [3/4*pi~1*pi]: " << histo2[i * 8 + 3] << "\n";
		cout << "[1*pi~5/4*pi]: " << histo2[i * 8 + 4] << "  [5/4*pi~3/2*pi]: " << histo2[i * 8 + 5];
		cout << "  [3/2*pi~7/4*pi]: " << histo2[i * 8 + 6] << "  [7/4*pi~2*pi]: " << histo2[i * 8 + 7] << "\n";
		cout << "--------------------------\n";
	}

	cout << "\n======거리계산======\n";
	float smallsum = 65535;
	int rot = 0;
	int whtP = 0;
	vector<int> matched;
	for (int u = 0; u < 4; u++) {//점 4개 비교
		
		for (int i = 0; i < 4; i++) {//2nd점 4개 비교

			for (int j = 0; j < 8; j++) {//각도
				float distsum = 0;
				for (int k = 0; k < 8; k++) {
					int histo2idx = (k + j) % 8 + (i * 8);
					int Point_1st = k + u * 8;
					distsum = distsum + abs(histo[Point_1st] - histo2[histo2idx]);
				}
				if (smallsum > distsum) {
					smallsum = distsum;
					rot = j;
					whtP = i;
				}
			}
		}
		matched.push_back(whtP + 1);
		smallsum = 65535;
		rot = 0;
		whtP = 0;
	}
	for (int i = 0; i < 4; i++) {
		cout << "1st.jpg의 "<<i+1<<"번째 점은\n";
		cout << " 2st.jpg의 " << matched[i] << "번째 점과 같음\n";
	}
	Mat concatimg;
	hconcat(img, img2, concatimg);
	for (int i = 0; i < 4; i++) {
		Pvec[4 + i].x += 599;
	}
	for (int i = 0; i < 4; i++) {
		line(concatimg, Pvec[i], Pvec[3 + matched[i]], Scalar(0, 254, 0), 5);
	}
	//line(concatimg, Pvec[0], Pvec[3 + matched[0]], Scalar(0, 254, 0),5);


	cv::imshow("image", concatimg);

	cv::waitKey(0);


	//cv::imshow("image", img);
	//cv::waitKey(0);
	return EXIT_SUCCESS;
}