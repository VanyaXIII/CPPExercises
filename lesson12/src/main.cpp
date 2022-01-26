#include "line.h"

#include <iostream>
#include <filesystem>


void test1() {
    cv::Scalar blue(255, 0, 0);
    cv::Scalar green(0, 255, 0);
    cv::Scalar red(0, 0, 255);
    cv::Scalar blueRed(255, 0, 255);
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string results = "lesson12/resultsData/";
    if (!std::filesystem::exists(results)) { // если папка еще не создана
        std::filesystem::create_directory(results); // то создаем ее
    }

    // нарисуйте на листочке эту прямую, т.е. множество точек соответствующих уравнению 0.5*x-y+5=0
    Line line(1, -1, 5);
    std::cout << "Line: " << line << std::endl;

    // здесь создается несколько случайных точек на прямой
    int n = 100;
    double fromX = 0.0;
    double toX = 20.0;
    double plotScale = 50.0;
    double sigma = 0.2;
    std::vector<cv::Point2f> pointsNearLine = line.generatePoints(n, fromX, toX, sigma);
    std::cout << pointsNearLine.size() << " points: ";
    for (auto & i : pointsNearLine) {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    cv::Mat plot;
    plotPoints(plot, pointsNearLine, plotScale, blue);

    line.plot(plot, plotScale, green);

    cv::imwrite(results + "test1_1points_and_line.png", plot);

    for (int i = 0; i < pointsNearLine.size() - 1; ++i) {
            cv::Point2f a = pointsNearLine[i];
            cv::Point2f b = pointsNearLine[i + 1];
            Line lineI = fitLineFromTwoPoints(a, b);
            std::cout << "Line #" << (i + 1) << ": " << lineI << "\n";
//            lineI.plot(plot, plotScale, red);
    }

    cv::imwrite(results + "test1_2all_lines.png", plot);

    Line oneLineToRuleThemAll = fitLineFromNPoints(pointsNearLine);
    std::cout << "One line to rule them all: " << oneLineToRuleThemAll << std::endl;
    oneLineToRuleThemAll.plot(plot, plotScale, blue);
    cv::imwrite(results + "test1_3final_line.png", plot);

    // Дальше идет бонусное задание

    int randomPointsN = 2*n;
    double fromY = 0.0;
    double toY = plot.rows / plotScale;
    std::vector<cv::Point2f> randomPoints = generateRandomPoints(randomPointsN, fromX, toX, fromY, toY);

    std::cout << randomPoints.size() << " random points: ";
    for (auto & randomPoint : randomPoints) {
        std::cout << randomPoint << ", ";
    }
    std::cout << std::endl;

    plotPoints(plot, randomPoints, plotScale, red);
    cv::imwrite(results + "test1_4random_points.png", plot);

    std::vector<cv::Point2f> allPoints;
    allPoints.insert(allPoints.end(), randomPoints.begin(), randomPoints.end());
    allPoints.insert(allPoints.end(), pointsNearLine.begin(), pointsNearLine.end());

    Line oneLineToRuleThemAll2 = fitLineFromNNoisyPoints(allPoints); // TODO 06 БОНУС - реализуйте построение прямой по многим точкам включающим нерелевантные (такое чтобы прямая как можно лучше учитывала НАИБОЛЬШЕЕ число точек)
    std::cout << "One line to rule them all (from noisy points): " << oneLineToRuleThemAll2 << std::endl;
    oneLineToRuleThemAll2.plot(plot, plotScale, blueRed);
    cv::imwrite(results + "test1_5final_line_after_noise.png", plot);
}


int main() {
    try {
        test1();

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
