#pragma once

#include <opencv2/opencv.hpp>

cv::Mat makeAllBlackPixelsBlue(cv::Mat image);

cv::Mat invertImageColors(cv::Mat image);

cv::Mat addBackgroundInsteadOfBlackPixels(cv::Mat object, cv::Mat background);

cv::Mat addBackgroundInsteadOfBlackPixelsLargeBackground(cv::Mat object, cv::Mat largeBackground);

cv::Mat drawImgOn(cv::Mat object, cv::Mat bg, int x, int y);

cv::Mat drawNTimes(int n, cv::Mat object, cv::Mat bg);

cv::Mat setRandColor(cv::Mat img);

std::vector<cv::Vec3b> getColors(cv::Mat object, std::vector<std::pair<int, int>> &vec);

double distance(cv::Vec3b vec1, cv::Vec3b vec2);

cv::Mat setBg(cv::Mat object, cv::Mat bg, std::vector<cv::Vec3b> vec);

cv::Mat setRedColor(cv::Mat object, std::vector<std::pair<int, int>> &vec);

cv::Mat dilate(cv::Mat object, int r);

cv::Mat erode(cv::Mat object, int r);
