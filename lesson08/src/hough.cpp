#define _USE_MATH_DEFINES

#include "hough.h"
#include "math.h"

#include <libutils/rasserts.h>

double toRadians(double degrees)
{
    const double PI = 3.14159265358979323846264338327950288;
    return degrees * PI / 180.0;
}

double estimateR(double x0, double y0, double theta0radians)
{
    double r0 = x0 * cos(theta0radians) + y0 * sin(theta0radians);
    return r0;
}

cv::Mat buildHough(cv::Mat sobel) {// единственный аргумент - это результат свертки Собелем изначальной картинки
    // проверяем что входная картинка - одноканальная и вещественная:
    rassert(sobel.type() == CV_32FC1, 237128273918006);

    rassert(sobel.type() == CV_32FC1, 237128273918006);

    int width = sobel.cols;
    int height = sobel.rows;

    // решаем какое максимальное значение у параметра theta в нашем пространстве прямых
    int max_theta = 360;

    // решаем какое максимальное значение у параметра r в нашем пространстве прямых:
    int max_r = (int) ceil(sqrt(width * width + height * height)); // TODO замените это число так как вам кажется правильным (отталкиваясь от разрешения картинки - ее ширины и высоты)

    // создаем картинку-аккумулятор, в которой мы будем накапливать суммарные голоса за прямые
    // так же известна как пространство Хафа
    cv::Mat accumulator(max_r, max_theta, CV_32FC1, cv::Scalar(0)); // TODO подумайте какого разрешения она должна быть и поправьте ее размер

    // TODO не забудьте заполнить эту матрицу-картинку-аккумулятор нулями (очистить)

    // проходим по всем пикселям нашей картинки (уже свернутой оператором Собеля)
    for (int y0 = 0; y0 < height; ++y0) {
        for (int x0 = 0; x0 < width; ++x0) {
            // смотрим на пиксель с координатами (x0, y0)
            float strength = sobel.at<float>(y0, x0);// TODO считайте его "силу градиента" из картинки sobel

            // теперь для текущего пикселя надо найти все возможные прямые которые через него проходят
            // переберем параметр theta по всему возможному диапазону (в градусах):
            for (int theta0 = 0; theta0 + 1 < max_theta; ++theta0) {
                // TODO рассчитайте на базе информации о том какие координаты у пикселя - (x0, y0) и какой параметр theta0 мы сейчас рассматриваем
                // TODO обратите внимание что функции sin/cos принимают углы в радианах, поэтому сначала нужно пересчитать theta0 в радианы (воспользуйтесь константой PI)
                double theta0_rad = theta0 * M_PI / 180.0;
                double theta1_rad = (theta0 + 1) * M_PI / 180.0;
                double r0 = x0 * cos(theta0_rad) + y0 * sin(theta0_rad);
                double r1 = x0 * cos(theta1_rad) + y0 * sin(theta1_rad);
                int r11 = int(r1);
                int r00 = int(r0);

                for (int j = std::min(r11, r00); j < std::max(r11, r00); ++j)
                {
                    if (j >= 0) {
                        accumulator.at<float>(j, theta0) += strength * .5f;
                        accumulator.at<float>(j, theta0 + 1) += strength * .5f;
                    }
                }
                // TODO теперь рассчитайте координаты пикслея в пространстве Хафа (в картинке-аккумуляторе) соответсвующего параметрам theta0, r0
            }
        }
    }
    return accumulator;
}

std::vector<PolarLineExtremum> findLocalExtremums(cv::Mat houghSpace)
{
    rassert(houghSpace.type() == CV_32FC1, 234827498237080);

    const int max_theta = 360;
    rassert(houghSpace.cols == max_theta, 233892742893082);
    const int max_r = houghSpace.rows;

    std::vector<PolarLineExtremum> winners;

    for (int theta = 1; theta + 1 < max_theta; ++theta) {
        for (int r = 1; r + 1 < max_r; ++r) {
            float votes = houghSpace.at<float>(r, theta);
             bool ok = true;
             for (int j = r - 1; j <= r + 1 && ok; ++j)
             {
                 for (int i = theta - 1; i <= theta + 1; ++i)
                 {
                     if (votes < houghSpace.at<float>(j, i)) {
                         ok = false;
                         break;
                     }
                 }
             }
             if (ok) {
                 PolarLineExtremum line(theta, r, votes);
                 winners.push_back(line);
             }
        }
    }

    return winners;
}

std::vector<PolarLineExtremum> filterStrongLines(std::vector<PolarLineExtremum>& allLines, double thresholdFromWinner)
{
    double max = .0;
    for (auto &l : allLines)
    {
        max = std::max(max, l.votes);
    }

    std::vector<PolarLineExtremum> res;
    for (auto &l : allLines)
    {
        if (l.votes / max >= thresholdFromWinner)
            res.push_back(l);
    }
    return res;
}
