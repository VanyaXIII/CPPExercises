#include "morphology.h"
#include "disjoint_set.h"



cv::Mat checkBg(cv::Mat object, cv::Mat bg) {
    int x = object.cols, y = object.rows;
    cv::Mat mat1(y, x, CV_8UC3, cv::Scalar(255, 255, 255));
    for (int i = 0; i < y; ++i) {
        for (int j = 0; j < x; ++j) {
            cv::Vec3b color = object.at<cv::Vec3b>(i, j);
            if (distance(object.at<cv::Vec3b>(i, j), bg.at<cv::Vec3b>(i, j)) <= 40.0)
                mat1.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
        }
    }

    return mat1;
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

cv::Mat translate(cv::Mat mask, cv::Mat bg, cv::Mat object) {
    int x = object.cols, y = object.rows;
    for (int i = 0; i < y; ++i) {
        for (int j = 0; j < x; ++j) {
            cv::Vec3b color = mask.at<cv::Vec3b>(i, j);
            if (color[0] == 0 && color[1] == 0 && color[2] == 0)
                object.at<cv::Vec3b>(i, j) = bg.at<cv::Vec3b>(i, j);
        }
    }
    return object;
}

cv::Mat processWithDS(cv::Mat mask, long size) {
    int x = mask.cols, y = mask.rows;
    DisjointSet set(x * y);
    long p = 0;
    for (int i = 0; i < y; ++i) {
        for (int j = 0; j < x; ++j) {
            cv::Vec3b color = mask.at<cv::Vec3b>(i, j);
            if (i > 0)
            {
                cv::Vec3b color1 = mask.at<cv::Vec3b>(i-1, j);
                if (distance(color, color1) < 10) {
                    set.union_sets(p, p - x);
                }
            }
            if (j > 0)
            {
                cv::Vec3b color1 = mask.at<cv::Vec3b>(i, j-1);
                if (distance(color, color1) < 10)
                    set.union_sets(p,p - 1);
            }
            ++p;
        }
    }
    p = 0;
    for (int i = 0; i < y; ++i) {
        for (int j = 0; j < x; ++j) {
            cv::Vec3b color = mask.at<cv::Vec3b>(i, j);
            if (color[0] + color[1] + color[2] > 0)
                if (set.get_set_size(p) < size)
                    mask.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
            ++p;
        }
    }
    return mask;
}

