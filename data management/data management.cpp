#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <string>
#include <iostream>

using namespace std;
using namespace cv;

static void drawQRCodeContour(Mat &color_image, vector<Point> transform);
static void drawFPS(Mat &color_image, double fps);
static int  liveQRCodeDetect(const string& out_file);


int main(int argc, char *argv[])
{
	int return_code = 0;
	string out_file_name;
	for (;;) {
		return_code = liveQRCodeDetect(out_file_name);

	}

	return return_code;

}

void drawQRCodeContour(Mat &color_image, vector<Point> transform)//画二维码轮廓
{
	if (!transform.empty())
	{
		double show_radius = (color_image.rows > color_image.cols)
			? (2.813 * color_image.rows) / color_image.cols
			: (2.813 * color_image.cols) / color_image.rows;
		double contour_radius = show_radius * 0.4;

		vector< vector<Point> > contours;
		contours.push_back(transform);
		drawContours(color_image, contours, 0, Scalar(211, 0, 148), cvRound(contour_radius));

		RNG rng(1000);
		for (size_t i = 0; i < 4; i++)
		{
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			circle(color_image, transform[i], cvRound(show_radius), color, -1);
		}
	}
}

void drawFPS(Mat &color_image, double fps)  //显示FPS
{
	ostringstream convert;
	convert << cvRound(fps) << " FPS (QR detection)";
	putText(color_image, convert.str(), Point(25, 25), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 255), 2);
}

int liveQRCodeDetect(const string& out_file)//**检测程序**//
{
	VideoCapture cap(0);//开摄像头
	if (!cap.isOpened())
	{
		cout << "Cannot open a camera" << endl;
		return -4;
	}

	QRCodeDetector qrcode;
	TickMeter total;
	for (;;)
	{
		Mat frame, src, straight_barcode;
		string decode_info;
		vector<Point> transform;
		cap >> frame;
		if (frame.empty())
		{
			cout << "End of video stream" << endl;
			break;
		}
		cvtColor(frame, src, COLOR_BGR2GRAY);   //把frame 转为灰度图src

		total.start();
		bool result_detection = qrcode.detect(src, transform);//检测器
		if (result_detection)
		{
			decode_info = qrcode.decode(src, transform, straight_barcode);//解码器
			if (!decode_info.empty()) {
				cout << decode_info << endl;     //解码后输出一次返回
				waitKey(2000);
				return  0;
			}

		}
		total.stop();
		double fps = 1 / total.getTimeSec();
		total.reset();

		if (result_detection) { drawQRCodeContour(frame, transform); }
		drawFPS(frame, fps);

		imshow("Live QR code detector", frame);//显示摄像头
		char c = (char)waitKey(30);
		if (c == 27)
			break;
		if (c == ' ' && !out_file.empty())
			imwrite(out_file, frame); // TODO write original frame too
			
	}
	return 0;
}
