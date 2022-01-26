#define _USE_MATH_DEFINES

#include "hough.h"


#include <libutils/rasserts.h>
#include <opencv2/imgproc.hpp>
#include <math.h>

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

cv::Mat drawCirclesOnExtremumsInHoughSpace(cv::Mat houghSpace, const std::vector<PolarLineExtremum>& lines, int radius)
{
    // TODO Доделайте эту функцию - пусть она скопирует картинку с пространством Хафа и отметит на ней красным кружком указанного радиуса (radius) места где были обнаружены экстремумы (на базе списка прямых)

    // делаем копию картинки с пространством Хафа (чтобы не портить само пространство Хафа)
    cv::Mat houghSpaceWithCrosses = houghSpace.clone();

    // проверяем что пространство состоит из 32-битных вещественных чисел (т.е. картинка одноканальная)
    rassert(houghSpaceWithCrosses.type() == CV_32FC1, 347823472890137);

    // но мы хотим рисовать КРАСНЫЙ кружочек вокруг найденных экстремумов, а значит нам не подходит черно-белая картинка
    // поэтому ее надо преобразовать в обычную цветную BGR картинку
    cv::cvtColor(houghSpaceWithCrosses, houghSpaceWithCrosses, cv::COLOR_GRAY2BGR);
    // проверяем что теперь все хорошо и картинка трехканальная (но при этом каждый цвет - 32-битное вещественное число)
    rassert(houghSpaceWithCrosses.type() == CV_32FC3, 347823472890148);

    for (auto &line : lines) {

        // Пример как рисовать кружок в какой-то точке (закомментируйте его):
        cv::Point point(line.theta, line.r);
        cv::Scalar color(0, 0, 255); // BGR, т.е. красный цвет
        cv::circle(houghSpaceWithCrosses, point, radius, color);

        // TODO отметьте в пространстве Хафа красным кружком радиуса radius экстремум соответствующий прямой line
    }

    return houghSpaceWithCrosses;
}

cv::Point PolarLineExtremum::intersect(PolarLineExtremum that)
{
    // Одна прямая - наш текущий объект (this) у которого был вызван этот метод, у этой прямой такие параметры:
    double theta1 = this->theta;
    double r1 = this->r;

    // Другая прямая - другой объект (that) который был передан в этот метод как аргумент, у этой прямой такие параметры:
    double theta2 = that.theta;
    double r2 = that.r;

    // TODO реализуйте поиск пересечения этих двух прямых, напоминаю что формула прямой описана тут - https://www.polarnick.com/blogs/239/2021/school239_11_2021_2022/2021/11/02/lesson8-hough-transform.html
    // после этого загуглите как искать пересечение двух прямых, пример запроса: "intersect two 2d lines"
    // и не забудьте что cos/sin принимают радианы (используйте toRadians)

    double cos1 = cos(toRadians(theta1)); double sin1 = sin(toRadians(theta1));
    double cos2 = cos(toRadians(theta2)); double sin2 = sin(toRadians(theta2));

    double x = (r2 * sin1 - r1 * sin2) / (cos2 * sin1 - cos1 * sin2);
    double y = -cos1 / sin1 * x + r1/sin1;

    return {(int) round(x), (int)round(y)};
}

// TODO Реализуйте эту функцию - пусть она скопирует картинку и отметит на ней прямые в соответствии со списком прямых
cv::Mat drawLinesOnImage(const cv::Mat& img, std::vector<PolarLineExtremum> lines)
{
    // делаем копию картинки (чтобы при рисовании не менять саму оригинальную картинку)
    cv::Mat imgWithLines = img.clone();

    // проверяем что картинка черно-белая (мы ведь ее такой сделали ради оператора Собеля) и 8-битная
    rassert(imgWithLines.type() == CV_8UC1, 45728934700167);

    // но мы хотим рисовать КРАСНЫЕ прямые, а значит нам не подходит черно-белая картинка
    // поэтому ее надо преобразовать в обычную цветную BGR картинку с 8 битами в каждом пикселе
    cv::cvtColor(imgWithLines, imgWithLines, cv::COLOR_GRAY2BGR);
    rassert(imgWithLines.type() == CV_8UC3, 45728934700172);

    // выпишем размер картинки
    int width = imgWithLines.cols;
    int height = imgWithLines.rows;

    PolarLineExtremum leftImageBorder(0, 0, 0);
    PolarLineExtremum bottomImageBorder(90, 0, 0);
    PolarLineExtremum rightImageBorder(0, width, 0);
    PolarLineExtremum topImageBorder(90, height, 0);

    for (int i = 0; i < lines.size(); ++i) {
        PolarLineExtremum line = lines[i];

        // нам надо найти точки на краях картинки
        cv::Point pointA;
        cv::Point pointB;

        // TODO создайте четыре прямых соответствующих краям картинки (на бумажке нарисуйте картинку и подумайте какие theta/r должны быть у прямых?):
        // напоминаю - чтобы посмотреть какие аргументы требует функция (или в данном случае конструктор объекта) - нужно:
        // 1) раскомментировать эти четыре строки ниже
        // 2) поставить каретку (указатель где вы вводите новые символы) внутри скобок функции (или конструктора, т.е. там где были три вопроса: ???)
        // 3) нажать Ctrl+P чтобы показать список параметров (P=Parameters)

        // TODO воспользуйтесь недавно созданной функций поиска пересечения прямых чтобы найти точки пересечения краев картинки:
        pointA = line.intersect(leftImageBorder);
        pointB = line.intersect(rightImageBorder);

        // TODO а в каких случаях нужно использовать пересечение с верхним и нижним краем картинки?
        if (pointA.x < 0 || pointA.y < 0 || pointA.x > width || pointA.y > height
        || pointB.x < 0 || pointB.y < 0 || pointB.x > width || pointB.y > height)
        {
            pointA = line.intersect(bottomImageBorder);
            pointB = line.intersect(topImageBorder);
        }

        if (pointA.x < 0 || pointA.y < 0 || pointA.x > width || pointA.y > height
            || pointB.x < 0 || pointB.y < 0 || pointB.x > width || pointB.y > height)
        {
            pointA = line.intersect(leftImageBorder);
            pointB = line.intersect(bottomImageBorder);
        }

        if (pointA.x < 0 || pointA.y < 0 || pointA.x > width || pointA.y > height
            || pointB.x < 0 || pointB.y < 0 || pointB.x > width || pointB.y > height)
        {
            pointA = line.intersect(leftImageBorder);
            pointB = line.intersect(topImageBorder);
        }

        if (pointA.x < 0 || pointA.y < 0 || pointA.x > width || pointA.y > height
            || pointB.x < 0 || pointB.y < 0 || pointB.x > width || pointB.y > height)
        {
            pointA = line.intersect(rightImageBorder);
            pointB = line.intersect(topImageBorder);
        }

        if (pointA.x < 0 || pointA.y < 0 || pointA.x > width || pointA.y > height
            || pointB.x < 0 || pointB.y < 0 || pointB.x > width || pointB.y > height)
        {
            pointA = line.intersect(bottomImageBorder);
            pointB = line.intersect(rightImageBorder);
        }

        cv::Scalar color((uchar) rand() % 255, (uchar) rand() % 255, (uchar) rand() % 255);
        cv::line(imgWithLines, pointA, pointB, color);
    }

    return imgWithLines;
}
