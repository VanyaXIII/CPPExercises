#include "parseSymbols.h"
#include <filesystem>
#include <iostream>
#include <opencv2/imgproc.hpp>
#include "hog.h"

#define NSAMPLES_PER_LETTER 5
#define LETTER_DIR_PATH std::string("lesson10/generatedData/letters")

cv::Scalar rnd() {
    return cv::Scalar(128 + rand() % 128, 128 + rand() % 128, 128 + rand() % 128); // можно было бы брать по модулю 255, но так цвета будут светлее и контрастнее
}

cv::Mat draw(int rows, int cols, const std::vector<std::vector<cv::Point>>& contoursPoints) {



    // создаем пустую черную картинку
    cv::Mat blackImage(rows, cols, CV_8UC3, cv::Scalar(0, 0, 0));
    // теперь мы на ней хотим нарисовать контуры
    cv::Mat imageWithContoursPoints = blackImage.clone();
    for (const auto& points : contoursPoints) {
//        // сейчас мы смотрим на контур номер contourI
//
        cv::Scalar contourColor = rnd(); // выберем для него случайный цвет

        for (auto &point : points) {
            imageWithContoursPoints.at<cv::Vec3b>(point.y, point.x) = cv::Vec3b(contourColor[0], contourColor[1], contourColor[2]);
        }

    }

    return imageWithContoursPoints;
}

char findLetter(const cv::Mat& img)
{
    HoG a = buildHoG(img);
    double distMin = DBL_MAX; char res = 'a';
    for (char letter = 'a'; letter <= 'z'; ++letter)
    {
        for (int i = 1; i <= NSAMPLES_PER_LETTER; ++i)
        {
            cv::Mat lt = cv::imread(LETTER_DIR_PATH + "/" + letter + "/" + std::to_string(2) + ".png");
            HoG b = buildHoG(lt);
            auto d = distance(a, b);
            if (d < distMin)
            {
                res = letter;
                distMin = d;
            }
        }
    }
    return res;
}

std::vector<cv::Mat> splitSymbols(const cv::Mat& original)
{

    cv::Mat img;
    cv::cvtColor(original, img, cv::COLOR_BGR2GRAY);

    cv::Mat binary;
    cv::threshold(img, binary, 127, 255, cv::THRESH_BINARY_INV);

    cv::adaptiveThreshold(img, binary, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 5, 13);

    cv::Mat binary_eroded;
    cv::erode(binary, binary_eroded, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)));

    cv::Mat binary_dilated;
    cv::dilate(binary, binary_dilated, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)));

    binary = binary_dilated;
    std::vector<std::vector<cv::Point>> contoursPoints;
    cv::findContours(binary, contoursPoints, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    cv::Mat imageWithContoursPoints = draw(img.rows, img.cols, contoursPoints);

    std::vector<std::vector<cv::Point>> contoursPoints2;
    cv::findContours(binary, contoursPoints2, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    cv::Mat imageWithContoursPoints2 = draw(img.rows, img.cols, contoursPoints2);

    std::vector<cv::Rect> rects;
    cv::Mat imgWithBoxes = original.clone();
    for (const auto& points : contoursPoints) {
        cv::Rect box = cv::boundingRect(points);
        rects.emplace_back(box.x - 1, box.y - 1, box.width + 1, box.height + 1);
    }

    auto cmpX = [&](const cv::Rect& r1, const cv::Rect& r2)
    {
        return r1.x < r2.x;
    };
    auto cmpY = [&](const cv::Rect& r1, const cv::Rect& r2)
    {
        return r1.y < r2.y;
    };

    std::sort(rects.begin(), rects.end(), cmpY);
    std::vector<std::vector<cv::Rect>> rows;
    rows.emplace_back();
    rows[0].push_back(rects[0]);
    int d = 0;
    for (auto &r : rects)
    {
        d = d < r.height ? r.height : d;
    }
    for (int i = 1; i < rects.size(); ++i) {
        if (rects[i].y - rects[i-1].y > d)
        {
            rows.emplace_back();
        }
        rows[rows.size() - 1].push_back(rects[i]);
    }
    for (auto &e : rows)
    {
        std::sort(e.begin(), e.end(), cmpX);
    }
    std::vector<cv::Mat> symbols;
    for (auto &r : rows)
    {
        for (auto &e : r)
        {
            symbols.emplace_back(original, e);
        }
    }
    std::string out_path = "lesson11/resultsData/";
    for (int i = 0; i < symbols.size(); ++i)
    {
        cv::imwrite(out_path + "test" + std::to_string(i) + ".jpg", symbols[i]);
//        findLetter(symbols[i]);
        std::cout << std::to_string(i) << ": " << findLetter(symbols[i]) << "\n";
    }
    // TODO 101: чтобы извлечь кусок картинки (для каждого прямоугольника cv::Rect вокруг символа) - загуглите "opencv how to extract subimage"
    return symbols;

}

