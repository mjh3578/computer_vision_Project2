
#include <stdio.h>
#include<utility>
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
int maxX = -1;
int minX = 65535;
int maxY = -1;
int minY = 65535;

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
				Pvec.push_back(Point(x, y));
				Pcount++;
			
			}
			else if (snd == 1) {
				Pvec.push_back(Point(x, y));
				if (x > maxX)
					maxX = x;
				if (x < minX)
					minX = x;
				if (y > maxY)
					maxY = y;
				if (y < minY)
					minY = y;
				Pcount++;
			}


			//Mvec.push_back(mag);
			//Mvec.push_back(angle);
			//rectangle(im, Rect(400, 400, 100, 100), Scalar(0, 0, 255), 1, 8, 0);
			cv::imshow("image", im);

		}
	}
	
}
float BilinearInterpolation(float q11, float q12, float q21, float q22, float x1, float x2, float y1, float y2, float x, float y)
{
	float x2x1, y2y1, x2x, y2y, yy1, xx1;
	x2x1 = x2 - x1;
	y2y1 = y2 - y1;
	x2x = x2 - x;
	y2y = y2 - y;
	yy1 = y - y1;
	xx1 = x - x1;
	return 1.0 / (x2x1 * y2y1) * (
		q11 * x2x * y2y +
		q21 * xx1 * y2y +
		q12 * x2x * yy1 +
		q22 * xx1 * yy1
		);
}
void displayType(cv::Mat& img, const std::string type, std::vector<cv::Point>& contour)
{
	cv::Rect rect = cv::boundingRect(contour);
	cv::Point top_left(rect.x, rect.y);
	cv::putText(img, type, top_left, 0, 0.5, CV_RGB(0, 0, 0));
}

int ccw(pair<int, int> a, pair<int, int> b, pair<int, int> c) {
	int op = a.first * b.second + b.first * c.second + c.first * a.second;
	op -= (a.second * b.first + b.second * c.first + c.second * a.first);
	if (op > 0)return 1;
	else if (op == 0)return 0;
	else return -1;
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
	

	

	//////////////////////////////
	string f_name;
	cout << "enter file name.. (1st.jpg)" << "\n";

	getline(cin, f_name, '\n');

	//cin << f_name;
	cv::Mat imgori = cv::imread(f_name);
	Mat img;
	
	GaussianBlur(imgori, imgori, Size(5, 5), 0.0);
	resize(imgori, img, Size(600, 800));
	Mat rimg;
	img.copyTo(rimg);

	if (img.empty())
		return EXIT_FAILURE;
	


	cv::imshow("image", img);
	setMouseCallback("image", CallBackFunc,&img);
	while (cCount != 4) {
		cv::waitKey(30);
	}
	cv::waitKey(500);
	destroyWindow("image");
	


	cout << "enter 2nd file name.. (target.jpg)" << "\n";
	cCount = 0;
	snd = 1;
	Pcount = 0;

	getline(cin, f_name, '\n');
	cv::Mat imgori2 = cv::imread(f_name);
	Mat img2;
	
	GaussianBlur(imgori2, imgori2, Size(5, 5), 0.0);
	resize(imgori2, img2, Size(600, 800));
	Mat rimg2;
	img2.copyTo(rimg2);

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
	vector<Point2f> corners(4);
	corners[0] = Pvec[0];
	corners[1] = Pvec[1];
	corners[2] = Pvec[2];
	corners[3] = Pvec[3];

	Size warpSize(600, 800);

	Mat warpImg(warpSize, rimg.type());



	vector<Point2f> warpCorners(4);
	warpCorners[0] = Pvec[4];
	warpCorners[1] = Pvec[5];
	warpCorners[2] = Pvec[6];
	warpCorners[3] = Pvec[7];

	Mat trans = getPerspectiveTransform(corners, warpCorners);
	warpPerspective(rimg, warpImg, trans, warpSize);
	imshow("save",warpImg);
	//cout << maxX << ", " << minX << ", " << maxY << ", " << minY;

	//vector<Point> line1 = linePoints(Pvec[4].x, Pvec[4].y, Pvec[5].x, Pvec[5].y);
	//vector<Point> line2 = linePoints(Pvec[5].x, Pvec[5].y, Pvec[6].x, Pvec[6].y);
	//vector<Point> line3 = linePoints(Pvec[6].x, Pvec[6].y, Pvec[7].x, Pvec[7].y);
	//vector<Point> line4 = linePoints(Pvec[7].x, Pvec[7].y, Pvec[4].x, Pvec[4].y);
	cout << "-=-=-=-=-\n";
	
	//img_result2.at<uchar>(y, x) = p;
	//pair<int, int> p1 (Pvec[4].x,Pvec[4].y);
	//pair<int, int> p2(Pvec[5].x, Pvec[5].y);
	//pair<int, int> p3(Pvec[6].x, Pvec[6].y);
	//pair<int, int> p4(Pvec[7].x, Pvec[7].y);


	for (int i = minX; i <= maxX; i++) {
		for (int j = minY; j <= maxY; j++) {
			int cc = 0;
			pair<int, int> a;
			a = make_pair(i, j);
			pair<int, int> b = make_pair(maxX, j);
			for (int p = 0; p < 3; p++) {

				pair<int, int> c = make_pair(Pvec[4 + p].x, Pvec[4 + p].y);
				pair<int, int> d = make_pair(Pvec[5 + p % 3].x, Pvec[5 + p % 3].y);
				int ab = ccw(a, b, c) * ccw(a, b, d);
				int cd = ccw(c, d, a) * ccw(c, d, b);
				if (ab == 0 && cd == 0) {
					if (a > b)swap(a, b);
					if (c > d)swap(c, d);
					if (c <= b && a <= d) {
						rimg2.at<Vec3b>(j, i)[0] = warpImg.at<Vec3b>(j, i)[0];
						rimg2.at<Vec3b>(j, i)[1] = warpImg.at<Vec3b>(j, i)[1];
						rimg2.at<Vec3b>(j, i)[2] = warpImg.at<Vec3b>(j, i)[2];
					}
						
				}
				if (ab <= 0 && cd <= 0) {
					rimg2.at<Vec3b>(j, i)[0] = warpImg.at<Vec3b>(j, i)[0];
					rimg2.at<Vec3b>(j, i)[1] = warpImg.at<Vec3b>(j, i)[1];
					rimg2.at<Vec3b>(j, i)[2] = warpImg.at<Vec3b>(j, i)[2];
				}
			}
			
		}
	}
	imshow("resut", rimg2);
	cout << "end\n";

	waitKey(0);
	
	return EXIT_SUCCESS;
}