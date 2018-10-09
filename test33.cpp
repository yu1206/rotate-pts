
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream> 
#include <io.h>
#include <Windows.h>
#include "opencv\cv.h"
#include "opencv\highgui.h"
#include <direct.h>
using namespace cv;
using namespace std;

extern int readDir(char *dirName, vector<string> &filesName);
#define M_PI 3.14159265358979323846
void coordinates33(Point2d src, float angle, Point2d & dst)
{
	dst.x = src.x*cos(angle) - src.y*sin(angle);
	dst.y = src.x*sin(angle) + src.y*cos(angle);
}
void coordinates44(Point2f src, float angle, Point2f & dst)
{
	dst.x = src.x*cos(angle) - src.y*sin(angle);
	dst.y = src.x*sin(angle) + src.y*cos(angle);

}
// 旋转中心，坐标为车牌中心，旋转区域车牌区域
void J_Rotate33(Mat src, int x1, int y1, int x2, int y2, float angle, Mat dst,
	int & l1, int & t1, int & r1, int & b1, Point & pt5, Point & pt6, Point & pt7, Point & pt8 )
{
	Point center;
	center.x = (x1 + x2) / 2; center.y = (y1 + y2) / 2;
	const double cosAngle = cos(angle);
	const double sinAngle = sin(angle);

	//原图像四个角的坐标变为以旋转中心的坐标系
	Point2d leftTop(x1 - center.x, -y1 + center.y); //(x1,y1)
	Point2d rightTop(x2 - center.x, -y1 + center.y); // (x2,y1)
	Point2d leftBottom(x1 - center.x, -y2 + center.y); //(x1,y2)
	Point2d rightBottom(x2 - center.x, -y2 + center.y); // (x2,y2)

	//以center为中心旋转后四个角的坐标
	Point2d transLeftTop, transRightTop, transLeftBottom, transRightBottom;

	coordinates33(leftTop, angle, transLeftTop);
	coordinates33(rightTop, angle, transRightTop);
	coordinates33(leftBottom, angle, transLeftBottom);
	coordinates33(rightBottom, angle, transRightBottom);

	/*coordinates44(leftTop, angle, transLeftTop);
	coordinates44(rightTop, angle, transRightTop);
	coordinates44(leftBottom, angle, transLeftBottom);
	coordinates44(rightBottom, angle, transRightBottom);
*/


	double left = min({ transLeftTop.x, transRightTop.x, transLeftBottom.x, transRightBottom.x });
	double right = max({ transLeftTop.x, transRightTop.x, transLeftBottom.x, transRightBottom.x });
	double top = min({ transLeftTop.y, transRightTop.y, transLeftBottom.y, transRightBottom.y });
	double down = max({ transLeftTop.y, transRightTop.y, transLeftBottom.y, transRightBottom.y });

	int width = static_cast<int>(abs(left - right) + 0.5);
	int height = static_cast<int>(abs(top - down) + 0.5);

	// 左上角为原点的坐标
	Point pt1, pt2;
	pt1.x = transLeftTop.x + center.x, pt1.y = -transLeftTop.y + center.y;
	pt2.x = transRightTop.x + center.x, pt2.y = -transRightTop.y + center.y;
	Point pt3, pt4;
	pt3.x = transLeftBottom.x + center.x, pt3.y = -transLeftBottom.y + center.y;
	pt4.x = transRightBottom.x + center.x, pt4.y = -transRightBottom.y + center.y;



	int left1 = min({ pt1.x, pt2.x, pt3.x, pt4.x });
	int right1 = max({ pt1.x, pt2.x, pt3.x, pt4.x });
	int top1 = min({ pt1.y, pt2.y, pt3.y, pt4.y });
	int down1 = max({ pt1.y, pt2.y, pt3.y, pt4.y });

	const double num1 = -abs(left) * cosAngle - abs(top) * sinAngle + center.x;
	const double num2 = abs(left) * sinAngle - abs(top) * cosAngle + center.y;

	Vec3b *p;
	for (int i = 0; i < height; i++)
	{
		p = dst.ptr<Vec3b>(i + top1);
		for (int j = 0; j < width; j++)
		{
			//坐标变换
			int x = static_cast<int>(j  * cosAngle + i * sinAngle + num1 + 0.5);
			int y = static_cast<int>(-j * sinAngle + i * cosAngle + num2 + 0.5);

			if (x >= 0 && y >= 0 && x < src.cols && y < src.rows)
				p[j + left1] = src.ptr<Vec3b>(y)[x];
		}
	}

	l1 = left1; t1 = top1; r1 = right1; b1 = down1;

	pt5.x = pt3.x; pt5.y = pt2.y;
	pt6.x = pt4.x; pt6.y = pt1.y;
	pt7.x = pt1.x; pt7.y = pt4.y;
	pt8.x = pt2.x; pt8.y = pt3.y;

}


// 旋转中心，坐标为车牌中心，旋转区域车牌区域
void J_Rotate44(Mat src, int x1, int y1, int x2, int y2, float angle, Mat dst,
	int & l1, int & t1, int & r1, int & b1, Point & pt5, Point & pt6, Point & pt7, Point & pt8)
{
	Point center;
	center.x = (x1 + x2) / 2+0.5; center.y = (y1 + y2) / 2+0.5;
	const double cosAngle = cos(angle);
	const double sinAngle = sin(angle);

	//原图像四个角的坐标变为以旋转中心的坐标系
	Point2d leftTop(x1 - center.x, -y1 + center.y); //(x1,y1)
	Point2d rightTop(x2 - center.x, -y1 + center.y); // (x2,y1)
	Point2d leftBottom(x1 - center.x, -y2 + center.y); //(x1,y2)
	Point2d rightBottom(x2 - center.x, -y2 + center.y); // (x2,y2)

	//以center为中心旋转后四个角的坐标
	Point2f transLeftTop, transRightTop, transLeftBottom, transRightBottom;

	

	coordinates44(leftTop, angle, transLeftTop);
	coordinates44(rightTop, angle, transRightTop);
	coordinates44(leftBottom, angle, transLeftBottom);
	coordinates44(rightBottom, angle, transRightBottom);
	


	double left = min({ transLeftTop.x, transRightTop.x, transLeftBottom.x, transRightBottom.x });
	double right = max({ transLeftTop.x, transRightTop.x, transLeftBottom.x, transRightBottom.x });
	double top = min({ transLeftTop.y, transRightTop.y, transLeftBottom.y, transRightBottom.y });
	double down = max({ transLeftTop.y, transRightTop.y, transLeftBottom.y, transRightBottom.y });

	int width = static_cast<int>(abs(left - right) + 0.5);
	int height = static_cast<int>(abs(top - down) + 0.5);

	// 左上角为原点的坐标
	Point2f pt1, pt2;
	pt1.x = transLeftTop.x + center.x, pt1.y = -transLeftTop.y + center.y;
	pt2.x = transRightTop.x + center.x, pt2.y = -transRightTop.y + center.y;
	Point2f pt3, pt4;
	pt3.x = transLeftBottom.x + center.x, pt3.y = -transLeftBottom.y + center.y;
	pt4.x = transRightBottom.x + center.x, pt4.y = -transRightBottom.y + center.y;



	int left1 = min({ pt1.x, pt2.x, pt3.x, pt4.x })+0.5;
	int right1 = max({ pt1.x, pt2.x, pt3.x, pt4.x }) + 0.5;
	int top1 = min({ pt1.y, pt2.y, pt3.y, pt4.y }) + 0.5;
	int down1 = max({ pt1.y, pt2.y, pt3.y, pt4.y }) + 0.5;

	int flag = 0;
	if (left1<0 || left1>src.cols || top1<0 || top1>=src.rows)
	{
		flag = 1;
		return;
	}
	if (right1<0 || right1>src.cols || down1<0 || down1>=src.rows)
	{
		flag = 2;
		return;
	}

	if (left1 + width >= src.cols)
	{
		flag = 3;
		return;
	}
	if (top1 + height >= src.rows)
	{
		flag = 4;
		return;
	}

	const double num1 = -abs(left) * cosAngle - abs(top) * sinAngle + center.x;
	const double num2 = abs(left) * sinAngle - abs(top) * cosAngle + center.y;

	Vec3b *p;
	for (int i = 0; i < height; i++)
	{
		p = dst.ptr<Vec3b>(i + top1);
		for (int j = 0; j < width; j++)
		{
			//坐标变换
			int x = static_cast<int>(j  * cosAngle + i * sinAngle + num1 + 0.5);
			int y = static_cast<int>(-j * sinAngle + i * cosAngle + num2 + 0.5);

			if (x >= 0 && y >= 0 && x < src.cols && y < src.rows)
				p[j + left1] = src.ptr<Vec3b>(y)[x];
		}
	}

	l1 = left1; t1 = top1; r1 = right1; b1 = down1;

	pt5.x = pt3.x + 0.5; pt5.y = pt2.y + 0.5;
	pt6.x = pt4.x + 0.5; pt6.y = pt1.y + 0.5;
	pt7.x = pt1.x + 0.5; pt7.y = pt4.y + 0.5;
	pt8.x = pt2.x + 0.5; pt8.y = pt3.y + 0.5;

}
void DrawLine(cv::Mat img, Point pt1, Point pt2, Point pt3, Point pt4)
{
	int thick = 1;
	
	CvScalar green = CV_RGB(0, 255, 0);
	cv::line(img, pt1, pt2,green, thick);
	cv::line(img, pt2, pt4, green, thick);
	cv::line(img, pt4, pt3, green, thick);
	cv::line(img, pt3, pt1, green, thick);
}

void PutText(cv::Mat img, Point pt1, Point pt2, Point pt3, Point pt4)
{
	CvScalar green = CV_RGB(0, 255, 0);

	char file[16]; Point pt;
	sprintf(file, "1");
	pt.x = pt1.x; pt.y = pt1.y;
	putText(img, file, pt, 1, 1, green);

	sprintf(file, "2");
	pt.x = pt2.x; pt.y = pt2.y;
	putText(img, file, pt, 1, 1, green);

	sprintf(file, "3");
	pt.x = pt3.x; pt.y = pt3.y;
	putText(img, file, pt, 1, 1, green);

	sprintf(file, "4");
	pt.x = pt4.x; pt.y = pt4.y;
	putText(img, file, pt, 1, 1, green);

}
void PutCircle(cv::Mat img, Point pt1, Point pt2, Point pt3, Point pt4)
{


	CvScalar green = CV_RGB(0, 255, 0);

	char file[16]; Point pt;
	sprintf(file, "1");
	pt.x = pt1.x-2; pt.y = pt1.y-2;
	putText(img, file, pt, 1, 1, green);
	circle(img, pt1, 1, green);
	sprintf(file, "2");
	pt.x = pt2.x-2; pt.y = pt2.y-2;
	putText(img, file, pt, 1, 1, green);
	circle(img, pt2, 1, green);
	sprintf(file, "3");
	pt.x = pt3.x-2; pt.y = pt3.y-2;
	putText(img, file, pt, 1, 1, green);
	circle(img, pt3, 1, green);
	sprintf(file, "4");
	pt.x = pt4.x-2; pt.y = pt4.y-2;
	putText(img, file, pt, 1, 1, green);
	circle(img, pt4, 1, green);

}
// 只旋转车牌区域，以车牌中心为旋转中心，坐标原点
int test33(int argc, char *argv[])
{
	float f1 = sin(30);
	float f2 = sin(45);
	float f3 = sin(90);
	float f4 = sin(180);
	double d1 = sin(M_PI / 6);
	double d2 = sin(M_PI / 4);
	double d3 = sin(M_PI / 3);
	double d4 = sin(M_PI / 2);
	double d5 = sin(M_PI);

	string inputPath = "I:/mtcnn-train/rotate";
	string outputPath = "I:/mtcnn-train/rotateResult";
	mkdir(outputPath.c_str());
	string outputsrc = "I:/mtcnn-train/rotateResult/src";
	mkdir(outputsrc.c_str());
	string outputtxt = "I:/mtcnn-train/rotateResult/src/000_one.txt";
	fstream finWrite;
	finWrite.open(outputtxt, ios::out);
	if (!finWrite.is_open())
	{
		cout << "finRead 文件数据打开错误！" << endl;
		system("pause");
		return false;
	}

	string outputdraw = "I:/mtcnn-train/rotateResult/draw";
	mkdir(outputdraw.c_str());
	vector<string> v_img_;
	readDir((char*)inputPath.c_str(), v_img_);
	srand((unsigned)time(NULL));
	for (int i = 0; i < v_img_.size(); i++)
	{
		cout << v_img_[i] << endl;
		int npos = v_img_[i].find_last_of('/');
		int npos2 = v_img_[i].find_last_of('.');
		string name1 = v_img_[i].substr(npos + 1, npos2 - npos - 1);
		Mat img = imread(v_img_[i].c_str());
		if (img.data == NULL)
		{

			printf("图像为空!\n");
			cout << v_img_[i].c_str() << endl;
			system("pause");
		}
		string nametxt = v_img_[i];
		nametxt.replace(nametxt.find_last_of("."), 4, ".txt");

		fstream fin1;
		fin1.open(nametxt);
		if (!fin1.is_open())
		{
			cout << "fin1 文件数据打开错误！" << endl;
			system("pause");
			continue;
		}

		int label; int x1, y1, x2, y2;

		fin1 >> label >> x1 >> y1 >> x2 >> y2;

		fin1.close();

		//int  degarr[] = { -20, -15, -10, -5, 0, 5, 10, 15, 20 };
		//for (int j = 0; j < 9; j++)
		//{
		//	int degree = degarr[j];
		//	double radian = M_PI*degree*1.0 / 180;

		//	Mat dst = img.clone();
		//	Point pt1, pt2, pt3, pt4;
		//	Point pt5, pt6, pt7, pt8;
		//	pt1.x = x1; pt1.y = y1; pt2.x = x2; pt2.y = y2;
		//	int left = 0; int top = 0; int right = 0; int bottom = 0;
		//	J_Rotate44(img, x1, y1, x2, y2, radian, dst, left, top, right, bottom, pt5, pt6, pt7, pt8);
		//	pt3.x = left; pt3.y = top; pt4.x = right; pt4.y = bottom;

		//	char intstr[128];
		//	sprintf(intstr, "-%da%d", j, degree);

		//	string str1 = outputsrc + "/" + name1 + intstr + ".jpg";
		//	imwrite(str1.c_str(), dst);

		//	finWrite << str1 << " " << label << " " << left << " " << top << " " << right << " " << bottom << " " <<
		//		pt5.x << " " << pt5.y << " " << pt6.x << " " << pt6.y << " " << pt7.x << " " << pt7.y << " " <<
		//		pt8.x << " " << pt8.y << " " << degree << endl;

		//	Mat drawimg = dst.clone();
		//	rectangle(drawimg, pt3, pt4, Scalar(0, 0, 255));
		//	//DrawLine(drawimg, pt5, pt6, pt7, pt8);
		//	//PutText(drawimg, pt5, pt6, pt7, pt8);


		//	string str2 = outputdraw + "/" + name1 + intstr + ".jpg";
		//	imwrite(str2.c_str(), drawimg);

		//	int jjjjjj = 90;
		//}

		
		//int  degarr1[] = { -10,-9, -8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10 };
		int  degarr1[61] = { 0};
		for (int j = 0; j < 41; j++)
		{
			degarr1[j] = j - 20;
		}
		for (int j = 0; j < 41; j++)
		{

			int degree = degarr1[j];
			double radian = M_PI*degree*1.0 / 180;

			Mat dst = img.clone();
			Point pt1, pt2, pt3, pt4;
			Point pt5, pt6, pt7, pt8;
			pt1.x = x1; pt1.y = y1; pt2.x = x2; pt2.y = y2;
			int left = 0; int top = 0; int right = 0; int bottom = 0;
			J_Rotate44(img, x1, y1, x2, y2, radian, dst, left, top, right, bottom, pt5, pt6, pt7, pt8);
			pt3.x = left; pt3.y = top; pt4.x = right; pt4.y = bottom;

			char intstr[128];
			sprintf(intstr, "-%da%d", j, degree);

			string str1 = outputsrc + "/" + name1 + intstr + ".jpg";
			imwrite(str1.c_str(), dst);

			finWrite << str1 << " " << label << " " << left << " " << top << " " << right << " " << bottom << " " <<
				pt5.x << " " << pt5.y << " " << pt6.x << " " << pt6.y << " " << pt7.x << " " << pt7.y << " " <<
				pt8.x << " " << pt8.y << " " << degree << endl;
			Mat drawimg = dst.clone();
			rectangle(drawimg, pt3, pt4, Scalar(0, 0, 255));
			PutCircle(drawimg, pt5, pt6, pt7, pt8);

			string str2 = outputdraw + "/" + name1 + intstr + ".jpg";
			imwrite(str2.c_str(), drawimg);
		}


		int jjjjjj = 90;

	}

	finWrite.close();
	return 0;
}







