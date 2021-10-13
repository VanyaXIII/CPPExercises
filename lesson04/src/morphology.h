#pragma once

#include <opencv2/opencv.hpp>

cv::Mat checkBg(cv::Mat object, cv::Mat bg);

double distance(cv::Vec3b vec1, cv::Vec3b vec2);

cv::Mat dilate(cv::Mat object, int r);

cv::Mat erode(cv::Mat object, int r);

cv::Mat translate(cv::Mat mask, cv::Mat bg, cv::Mat object);

cv::Mat processWithDS(cv::Mat mask, long size);
