#define _USE_MATH_DEFINES

#include "blur.h"


#include <libutils/rasserts.h>
#include <math.h>


const int size = 11;

cv::Mat blur(cv::Mat img, double sigma) {


    double weights[size][size];
    double div = 1 / (2 * sigma * sigma * M_PI);
    double div1 = 1 / (2 * sigma * sigma);
    int c = size / 2;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            weights[j][i] = div * exp(((double) (c - i) * (c - i) + (double) (c - j) * (c - j)) * div1 * (-1));
        }
    }

    int width = img.cols;
    int height = img.rows;
    cv::Mat res(height, width, CV_8UC3);
    auto delta = size / 2;
    for (int j = delta; j < height - delta; ++j) {
        for (int i = delta; i < width - delta; ++i) {

            cv::Vec3f val(0, 0, 0);

            float k = 0;

            for (int dj = -delta; dj <= delta; ++dj) {
                for (int di = -delta; di <= delta; ++di) {
                    cv::Vec3b color = img.at<cv::Vec3b>(j + dj, i + di);
                    cv::Vec3f color1 = cv::Vec3f((float) color[0], (float) color[1], (float) color[2]);
                    val += color1 * weights[delta + dj][delta + di];
                    k += weights[delta + dj][delta + di];
                }
            }
            val /= k;
            res.at<cv::Vec3b>(j, i) = cv::Vec3b((uchar) val[0], (uchar) val[1], (uchar) val[2]);
        }
    }
    return res;
}
