
#include <opencv2/opencv.hpp>

int main(int argc, char** argv)
{
	if (argc <= 1) return -1;
	cv::namedWindow("example2", cv::WINDOW_AUTOSIZE);
	cv::Mat img;
	cv::VideoCapture cap;
	cap.open(argv[1]);
	if (!cap.isOpened())return -1;
	for (;;)
	{
		cap >> img;
		if (img.empty())break;
		cv::imshow("example2", img);
		if (cv::waitKey(33) > 0) break;
	}
	cv::destroyWindow("example2");
	return 0;
}