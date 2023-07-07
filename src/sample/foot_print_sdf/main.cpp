#include <opencv2/opencv.hpp>

cv::Mat GenerateSdf(const cv::Mat& img);

int main(int argc, char** argv)
{
	if (argc <= 2) return -1;

	cv::Mat img = cv::imread(argv[1], -1);
    cv::Mat edgeImg = cv::imread(argv[2], -1);
    if (img.empty() || edgeImg.empty()) return  -1;


    std::vector<cv::Mat> channels;
    cv::split(img, channels);
    // 使用 Alpha 通道作为灰度图像的灰度值
    const cv::Mat& grayImage = channels[3];

    cv::Mat depthImg(grayImage.size(),CV_16UC1);

    auto sdf = GenerateSdf(edgeImg);

    for (int i = 0; i < depthImg.rows; i++)
    {
        for (int j = 0; j < depthImg.cols; j++)
        {
            depthImg.at<uint16_t>(i,j) =
                    static_cast<uint16_t>(static_cast<double>(std::numeric_limits<uint16_t>::max()) * (static_cast<double>(grayImage.at<uchar>(i,j)) / 255.0));
        }
    }

    cv::imwrite("depth.png",depthImg);
    cv::imwrite("sdf.png",sdf);

    return 0;

//    cv::Mat distShow;
//    distShow = cv::Mat::zeros(edgeGrayImage.size(), CV_8UC1);
//    for (int i = 0; i < imageThin.rows; i++)
//    {
//        for (int j = 0; j < imageThin.cols; j++)
//        {
//            distShow.at<uchar>(i, j) = imageThin.at<float>(i, j);
//            printf("%f ",imageThin.at<float>(i, j));
//        }
//    }
//    cv::normalize(distShow, distShow, 0, 255, CV_MINMAX); //为了显示清晰，做了0~255归一化
    cv::Mat mixArray[2] = { depthImg,sdf };
    cv::Mat output(depthImg.size(),CV_16UC2);
    int formTo[4] = {0,0,0,1};
    cv::mixChannels(mixArray, 2,&output,1,formTo,2);
	cv::namedWindow("example1", cv::WINDOW_AUTOSIZE);
	cv::imshow("example1", sdf);
	cv::waitKey(0);
	double ret = -1.0;
	if ((ret = cv::getWindowProperty("example1", 0)) != -1.0)
	{
		printf("%lf\n", ret);
		cv::destroyWindow("example1");
	}
	return 0;
}

cv::Mat GenerateSdf(const cv::Mat& img)
{
    std::vector<cv::Mat> channels;
    cv::split(img, channels);
    // 使用 Alpha 通道作为灰度图像的灰度值
    cv::Mat edgeGrayImage = channels[3];
    cv::Mat reverseImg(edgeGrayImage.size(),CV_8UC1);

    cv::threshold(edgeGrayImage, edgeGrayImage, 128, 255, cv::THRESH_BINARY);

    for (int i = 0; i < edgeGrayImage.rows; i++)
        for (int j = 0; j < edgeGrayImage.cols; j++)
            reverseImg.at<uchar>(i,j) = edgeGrayImage.at<uchar>(i,j) == 255 ? 0 : 255;

    cv::Mat imageThin(reverseImg.size(), CV_32FC1);

    cv::distanceTransform(reverseImg, imageThin, cv::DIST_L2, cv::DIST_MASK_PRECISE);

    cv::normalize(imageThin, imageThin, 1, 0, CV_MINMAX);

    cv::Mat res1(reverseImg.size(),CV_16UC1);

    for (int i = 0; i < res1.rows; i++)
    {
        for (int j = 0; j < res1.cols; j++)
        {
            res1.at<uint16_t>(i,j) = (uint16_t)(imageThin.at<float>(i,j) <= 0 ? 0.0f : (0.0f - imageThin.at<float>(i,j)) * (float)std::numeric_limits<uint16_t>::max());
        }
    }

    return res1;
}
