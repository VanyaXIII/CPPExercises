#include <filesystem>
#include <iostream>
#include <libutils/rasserts.h>

#include "parseSymbols.h"

#include <opencv2/imgproc.hpp>


cv::Scalar randColor() {
    return cv::Scalar(128 + rand() % 128, 128 + rand() % 128, 128 + rand() % 128); // можно было бы брать по модулю 255, но так цвета будут светлее и контрастнее
}


cv::Mat drawContours(int rows, int cols, const std::vector<std::vector<cv::Point>>& contoursPoints) {



    // создаем пустую черную картинку
    cv::Mat blackImage(rows, cols, CV_8UC3, cv::Scalar(0, 0, 0));
    // теперь мы на ней хотим нарисовать контуры
    cv::Mat imageWithContoursPoints = blackImage.clone();
    for (const auto& points : contoursPoints) {
//        // сейчас мы смотрим на контур номер contourI
//
        cv::Scalar contourColor = randColor(); // выберем для него случайный цвет

        for (auto &point : points) {
            imageWithContoursPoints.at<cv::Vec3b>(point.y, point.x) = cv::Vec3b(contourColor[0], contourColor[1], contourColor[2]);
        }

    }

    return imageWithContoursPoints;
}


void test(std::string name, std::string k) {
    std::cout << "Processing " << name << "/" << k << "..." << std::endl;

    std::string full_path = "lesson11/data/" + name + "/" + k + ".png";

    // создаем папочки в которые будем сохранять картинки с промежуточными результатами
    std::filesystem::create_directory("lesson11/resultsData/" + name);
    std::string out_path = "lesson11/resultsData/" + name + "/" + k;
    std::filesystem::create_directory(out_path);

    // считываем оригинальную исходную картинку
    cv::Mat original = cv::imread(full_path) ;
    rassert(!original.empty(), 238982391080010);
    rassert(original.type() == CV_8UC3, 23823947238900020);

    // сохраняем ее сразу для удобства
    cv::imwrite(out_path + "/00_original.jpg", original);

    // преобразуем в черно-белый цвет и тоже сразу сохраняем
    cv::Mat img;
    cv::cvtColor(original, img, cv::COLOR_BGR2GRAY);
    cv::imwrite(out_path + "/01_grey.jpg", img);


    cv::Mat binary;
    cv::threshold(img, binary, 127, 255, cv::THRESH_BINARY_INV);
    cv::imwrite(out_path + "/02_binary_thresholding.jpg", binary);

    cv::adaptiveThreshold(img, binary, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 5, 13);
    cv::imwrite(out_path + "/03_adaptive_thresholding.jpg", binary);

    cv::Mat binary_eroded;
    cv::erode(binary, binary_eroded, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)));
    cv::imwrite(out_path + "/04_erode.jpg", binary_eroded);

    cv::Mat binary_dilated;
    cv::dilate(binary, binary_dilated, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4)));
    cv::imwrite(out_path + "/05_dilate.jpg", binary_dilated);

    binary = binary_dilated;
    std::vector<std::vector<cv::Point>> contoursPoints; // по сути это вектор, где каждый элемент - это одна связная компонента-контур,
//                                                        // а что такое компонента-контур? это вектор из точек (из пикселей)
    cv::findContours(binary, contoursPoints, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    std::cout << "Contours: " << contoursPoints.size() << std::endl;
    cv::Mat imageWithContoursPoints = drawContours(img.rows, img.cols, contoursPoints);
    cv::imwrite(out_path + "/06_contours_points.jpg", imageWithContoursPoints);

    std::vector<std::vector<cv::Point>> contoursPoints2;
    cv::findContours(binary, contoursPoints2, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
//    // Обратите внимание на кромку картинки - она всё победила, т.к. черное - это ноль - пустота, а белое - это 255 - сам объект интереса
//    // как перевернуть ситуацию чтобы периметр не был засчитан как контур?
//    // когда подумаете - замрите! и прежде чем кодить:
//    // Посмотрите в документации у функций cv::threshold и cv::adaptiveThreshold
//    // про некоего cv::THRESH_BINARY_INV, чем он отличается от cv::THRESH_BINARY?
//    // Посмотрите как изменились все картинки.
    std::cout << "Contours2: " << contoursPoints2.size() << std::endl;
    cv::Mat imageWithContoursPoints2 = drawContours(img.rows, img.cols, contoursPoints2);
    cv::imwrite(out_path + "/07_contours_points2.jpg", imageWithContoursPoints2);

    cv::Mat imgWithBoxes = original.clone();
    for (const auto& points : contoursPoints) {
        // перем очередной контур
        cv::Rect box = cv::boundingRect(points); // строим прямоугольник по всем пикселям контура (bounding box = бокс ограничивающий объект)
        cv::Scalar blackColor(0, 0, 0);
        cv::rectangle(imgWithBoxes, box, blackColor, 2);
    }
    cv::imwrite(out_path + "/08_boxes.jpg", imgWithBoxes);
//                                                           // например если нет прямоугольников - посмотрите в верхний левый пиксель - белый ли он?
//                                                           // если не белый, то что это значит? почему так? сколько в целом нашлось связных компонент?
}

void finalExperiment(const std::string& name, const std::string& k) {
    // TODO 100:
    // 1) вытащите результат которым вы довольны в функцию splitSymbols в parseSymbols.h/parseSymbols.cpp
    //    эта функция должна находить контуры букв и извлекать кусочки картинок в вектор
    // 2) классифицируйте каждую из вытащенных букв (результатом из прошлого задания) и выведите полученный текст в консоль

    std::string full_path = "lesson11/data/" + name + "/" + k + ".png";
    cv::Mat mat = cv::imread(full_path);
    splitSymbols(mat);
}


int main() {
    try {
//        test("alphabet", "3_gradient");

        std::vector<std::string> names;
//        names.emplace_back("alphabet");
//        names.emplace_back("line");
//        names.emplace_back("text");
        for (auto & name : names) {
            for (int j = 1; j <= 5; ++j) {
                test(name, std::to_string(j));
            }
        }

//        test("alphabet", "3_gradient");
        finalExperiment("alphabet", "1");
        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}

