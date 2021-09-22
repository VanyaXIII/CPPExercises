#include "helper_functions.h"

#include <libutils/rasserts.h>


cv::Mat makeAllBlackPixelsBlue(cv::Mat image) {
    int c = image.cols, r = image.rows;
    for (int i = 0; i < r; ++i)
    {
        for (int j = 0; j < c; ++j)
        {
            cv::Vec3b color = image.at<cv::Vec3b>(i, j);
            if (color[1] + color[0] + color[2] == 0)
            {
                image.at<cv::Vec3b>(i, j) = cv::Vec3b(255, 0, 0);
            }
        }
    }

    return image;
}

cv::Mat invertImageColors(cv::Mat image) {
    int c = image.cols, r = image.rows;
    for (int i = 0; i < r; ++i)
    {
        for (int j = 0; j < c; ++j)
        {
            cv::Vec3b color = image.at<cv::Vec3b>(i, j);
            image.at<cv::Vec3b>(i, j) = cv::Vec3b(255 - color[0], 255 - color[1], 255 - color[2]);
        }
    }
    return image;
}

cv::Mat addBackgroundInsteadOfBlackPixels(cv::Mat object, cv::Mat background) {

    int c = object.cols, r = object.rows;
    for (int i = 0; i < r; ++i)
    {
        for (int j = 0; j < c; ++j)
        {
            cv::Vec3b color = object.at<cv::Vec3b>(i, j);
            if (color[0] + color[1] + color[2] == 0)
                object.at<cv::Vec3b>(i, j) = background.at<cv::Vec3b>(i, j);
        }
    }
    rassert(object.cols == background.cols, 341241251251351);
    return object;
}


cv::Mat addBackgroundInsteadOfBlackPixelsLargeBackground(cv::Mat object, cv::Mat largeBackground)
{
    return drawImgOn(object, largeBackground, largeBackground.cols / 2 - object.cols / 2, largeBackground.rows / 2 - object.rows / 2);
}

cv::Mat drawImgOn(cv::Mat object, cv::Mat bg, int x, int y)
{
    int c = object.cols, r = object.rows;
    for (int i = 0; i < r; ++i)
    {
        for (int j = 0; j < c; ++j)
        {
            cv::Vec3b color = object.at<cv::Vec3b>(i, j);
            if (color[0] + color[1] + color[2] != 0)
                bg.at<cv::Vec3b>(i + y, j + x) = object.at<cv::Vec3b>(i, j);
        }
    }
    return bg;
}

cv::Mat drawNTimes(int n, cv::Mat object, cv::Mat bg) {
    for (int i = 0; i < n; ++i)
    {
        int x = rand() % (bg.cols - object.cols), y = rand() % (bg.rows - object.rows);
        bg = drawImgOn(object, bg, x, y);
    }
    return bg;
}

cv::Mat setRandColor(cv::Mat img)
{
    int c = img.cols, r = img.rows;
    for (int i = 0; i < r; ++i)
    {
        for (int j = 0; j < c; ++j)
        {
            cv::Vec3b color = img.at<cv::Vec3b>(i, j);
            if (color[0] + color[1] + color[2] == 0) {
                int a = rand() % 255;
                int g = rand() % 255;
                int b = rand() % 255;
                img.at<cv::Vec3b>(i, j) = cv::Vec3b(b, g, a);
            }
        }
    }
    return img;
}
