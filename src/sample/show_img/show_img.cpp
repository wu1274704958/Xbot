#include <opencv2/opencv.hpp>

int main(int argc, char** argv)
{
	if (argc <= 1) return -1;
	cv::Mat img = cv::imread(argv[1], -1);
	if (img.empty()) return  -1;
	cv::namedWindow("example1", cv::WINDOW_AUTOSIZE);
	cv::imshow("example1", img);
	cv::waitKey(0);
	double ret = -1.0;
	if ((ret = cv::getWindowProperty("example1", 0)) != -1.0)
	{
		printf("%lf\n", ret);
		cv::destroyWindow("example1");
	}
	return 0;
}
