#include<iostream>
#include<opencv2/opencv.hpp>
#include<vector>

using namespace std;
using namespace cv;

#define PI 3.1415926

/// 图像旋转
int main() {
	Mat img = imread("D:/_jpg_all/Test/3.jpg");
	// 导入图像
	Mat img1;
	resize(img, img1, Size(500, 500));
	// 缩小图像 便于计算
	float row = img1.rows;
	float col = img1.cols;
	float r = img1.channels();
	Mat gray;
	if (r > 1) {
		cvtColor(img1, gray, COLOR_BGR2GRAY);
	}
	gray.convertTo(gray, CV_32FC1);
	//如果没有这一步，gray为8位整型
	// 
	float theta = 60;

	// 旋转矩阵
	Mat rot = (Mat_<float>(2, 2) << cos(theta * PI / 180), sin(theta * PI / 180), -sin(theta * PI / 180), cos(theta * PI / 180));
	// 左乘 实现计算 旋转后顶点坐标
	Mat a = rot * (Mat_<float>(2, 1) << 0, 0);
	Mat b = rot * (Mat_<float>(2, 1) << 0, col - 1);
	Mat c = rot * (Mat_<float>(2, 1) << row - 1, 0);
	Mat d = rot * (Mat_<float>(2, 1) << row - 1, col - 1);
	//cout << a << endl<<b<<endl<<c<<endl<<d;

	// 两种旋转情况的宽度 weigth
	float w1 = abs(b.at<float>(1, 0) - c.at<float>(1, 0));
	float w2 = abs(d.at<float>(1, 0) - a.at<float>(1, 0));
	//判断最长的宽度
	float w;
	if (w1 > w2) {
		w = round(w1);
	}
	else {
		w = round(w2);
	}

	// 两种旋转情况的高度 hight
	float h1 = abs(b.at<float>(0, 0) - c.at<float>(0, 0));
	float h2 = abs(d.at<float>(0, 0) - a.at<float>(0, 0));
	//判断最长的宽度
	float h;
	if (h1 > h2) {
		h = round(h1);
	}
	else {
		h = round(h2);
	}

	//创建新矩阵 用于放置旋转后图像
	Mat new_img = Mat::zeros(h, w, CV_32FC1);
	// 计算偏移
	Mat delta_h = (Mat_<float>(4, 1) << a.at<float>(0, 0), b.at<float>(0, 0), c.at<float>(0, 0), d.at<float>(0, 0));
	Mat delta_w = (Mat_<float>(4, 1) << a.at<float>(1, 0), b.at<float>(1, 0), c.at<float>(1, 0), d.at<float>(1, 0));
	//寻找最小
	float min_h = abs(*(min_element(delta_h.begin<float>(), delta_h.end<float>())));
	float min_w = abs(*(min_element(delta_w.begin<float>(), delta_w.end<float>())));

	for (int i = 0 - min_h; i < h - min_h; i++) {
		for (int j = 0 - min_w; j < w - min_w; j++) {
			// 得到了旋转后坐标，在原图对应的坐标
			Mat pix = rot.inv() * (Mat_<float>(2, 1) << i, j);
			// 坐标位小数
			float pix1[2] = { pix.at<float>(0,0),pix.at<float>(1,0) };
			// 双线性插值寻找小数的插值 得到该点坐标值
			// 计算该插值
			if (pix1[0] >= 0 && pix1[0] <= row - 1 && pix1[1] >= 0 && pix1[1] <= col - 1) {
				float left_up[2] = { floor(pix1[0]),floor(pix1[1]) };
				float right_up[2] = { floor(pix1[0]),ceil(pix1[1]) };
				float left_dowm[2] = { ceil(pix1[0]),floor(pix1[1]) };
				float right_dowm[2] = { ceil(pix1[0]),ceil(pix1[1]) };

				float w1 = pix1[0] - floor(pix1[0]);// (x2+x)/(x2-x1)  x2-x1=1；
				float w2 = pix1[1] - floor(pix1[1]);//(y2+y)/(y2-y1)  y2-y1=1；

				///*round 四舍五入 考虑到不为整数的情况*/
				new_img.at<float>(i+min_h, j+min_w) = round(gray.at<float>(left_up[0], left_up[1]) * (1 - w1) * (1 - w2)\
					+ gray.at<float>(right_up[0], right_up[1]) * w1 * (1 - w2)\
					+ gray.at<float>(left_dowm[0], left_dowm[1]) * (1 - w1) * w2\
					+ gray.at<float>(right_dowm[0], right_dowm[1]) * w1 * w2);
			}
		}
	}
	new_img.convertTo(new_img, CV_8UC1);
	imshow("img", new_img);
	waitKey(0);

	return 0;
}
