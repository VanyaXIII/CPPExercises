#include "helper_functions.h"
#include <cmath>
#include <algorithm>

#include <libutils/rasserts.h>

#include <utility>


cv::Mat makeAllBlackPixelsBlue(cv::Mat image) {
    int c = image.cols, r = image.rows;
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            cv::Vec3b color = image.at<cv::Vec3b>(i, j);
            if (color[1] + color[0] + color[2] == 0) {
                image.at<cv::Vec3b>(i, j) = cv::Vec3b(255, 0, 0);
            }
        }
    }

    return image;
}

cv::Mat invertImageColors(cv::Mat image) {
    int c = image.cols, r = image.rows;
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            cv::Vec3b color = image.at<cv::Vec3b>(i, j);
            image.at<cv::Vec3b>(i, j) = cv::Vec3b(255 - color[0], 255 - color[1], 255 - color[2]);
        }
    }
    return image;
}

cv::Mat addBackgroundInsteadOfBlackPixels(cv::Mat object, cv::Mat background) {

    int c = object.cols, r = object.rows;
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            cv::Vec3b color = object.at<cv::Vec3b>(i, j);
            if (color[0] + color[1] + color[2] == 0)
                object.at<cv::Vec3b>(i, j) = background.at<cv::Vec3b>(i, j);
        }
    }
    rassert(object.cols == background.cols, 341241251251351);
    return object;
}


cv::Mat addBackgroundInsteadOfBlackPixelsLargeBackground(cv::Mat object, cv::Mat largeBackground) {
    return drawImgOn(object, largeBackground, largeBackground.cols / 2 - object.cols / 2,
                     largeBackground.rows / 2 - object.rows / 2);
}

cv::Mat drawImgOn(cv::Mat object, cv::Mat bg, int x, int y) {
    int c = object.cols, r = object.rows;
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            cv::Vec3b color = object.at<cv::Vec3b>(i, j);
            if (color[0] + color[1] + color[2] != 0)
                bg.at<cv::Vec3b>(i + y, j + x) = object.at<cv::Vec3b>(i, j);
        }
    }
    return bg;
}

cv::Mat drawNTimes(int n, cv::Mat object, cv::Mat bg) {
    for (int i = 0; i < n; ++i) {
        int x = rand() % (bg.cols - object.cols), y = rand() % (bg.rows - object.rows);
        bg = drawImgOn(object, bg, x, y);
    }
    return bg;
}

cv::Mat setRandColor(cv::Mat img) {
    int c = img.cols, r = img.rows;
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
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

cv::Mat setRedColor(cv::Mat object, std::vector<std::pair<int, int>> &vec) {
    for (auto &e: vec) {
        object.at<cv::Vec3b>(e.second, e.first) = cv::Vec3b(0, 0, 255);
    }
    return object;
}

cv::Mat setBg(cv::Mat object, cv::Mat bg, std::vector<cv::Vec3b> vec) {
    int x = object.cols, y = object.rows;
    cv::Mat mat1(y, x, CV_8UC3, cv::Scalar(255, 255, 255));
    for (int i = 0; i < y; ++i) {
        for (int j = 0; j < x; ++j) {
            cv::Vec3b color = object.at<cv::Vec3b>(i, j);
            for (auto &elem: vec) {
                if (distance(object.at<cv::Vec3b>(i, j), elem) <= 10.0)
                    mat1.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
            }
        }
    }
    auto mat2 = dilate(mat1, 2);
    for (int i = 0; i < y; ++i) {
        for (int j = 0; j < x; ++j) {
            cv::Vec3b color = mat2.at<cv::Vec3b>(i, j);
            for (auto &elem: vec) {
                if (color[0] == 0 && color[1] == 0 && color[2] == 0)
                    object.at<cv::Vec3b>(i, j) = bg.at<cv::Vec3b>(i, j);
            }
        }
    }
    return object;
}

std::vector<cv::Vec3b> getColors(cv::Mat object, std::vector<std::pair<int, int>> &vec) {
    std::vector<cv::Vec3b> res;
    for (auto &e: vec)
        res.push_back(object.at<cv::Vec3b>(e.second, e.first));
    return res;

}

double distance(cv::Vec3b vec1, cv::Vec3b vec2) {
    return sqrt((vec2[0] - vec1[0]) * (vec2[0] - vec1[0]) + (vec2[1] - vec1[1]) * (vec2[1] - vec1[1]) +
                (vec2[2] - vec1[2]) * (vec2[2] - vec1[2]));
}

cv::Mat dilate(cv::Mat object, int r) {
    cv::Mat res = object.clone();
    int x = object.cols, y = object.rows;
    for (int i = 0; i < y; ++i) {
        for (int j = 0; j < x; ++j) {
            cv::Vec3b color = object.at<cv::Vec3b>(i, j);
            if (color[0] == 0 && color[1] == 0 && color[2] == 0)
                res.at<cv::Vec3b>(i, j) = object.at<cv::Vec3b>(i, j);
            else {
                bool f = false;
                for (int ii = i - r; ii <= i + r && !f; ++ii) {
                    for (int jj = j - r; jj <= j + r && !f; ++jj) {
                        if (ii < 0 || jj < 0 || ii >= y || jj >= x)
                            continue;
                        color = object.at<cv::Vec3b>(ii, jj);
                        if (color[0] == 0 && color[1] == 0 && color[2] == 0)
                            f = true;
                    }
                }
                if (f)
                    res.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
            }
        }
    }
    return res;
}

cv::Mat erode(cv::Mat object, int r) {
    int x = object.cols, y = object.rows;
    cv::Mat res(y, x, CV_8UC3, cv::Scalar(255, 255, 255));
    for (int i = 0; i < y; ++i) {
        for (int j = 0; j < x; ++j) {
            bool f = true;
            for (int ii = i - r; ii <= i + r && f; ++ii) {
                for (int jj = j - r; jj <= j + r && f; ++jj) {
                    if (ii < 0 || jj < 0 || ii >= y || jj >= x)
                        continue;
                    cv::Vec3b color = object.at<cv::Vec3b>(ii, jj);
                    if (color[0] == 255 && color[1] == 255 && color[2] == 255)
                        f = false;
                }
            }
            if (f)
                res.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
        }
    }
    return res;
}


