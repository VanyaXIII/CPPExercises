#include <iostream>
#include <filesystem> // это нам понадобится чтобы создать папку для результатов
#include <libutils/rasserts.h>

#include "helper_functions.h"

#include <opencv2/highgui.hpp> // подключили часть библиотеки OpenCV, теперь мы можем читать и сохранять картинки

void task1() {
    cv::Mat imgUnicorn = cv::imread("lesson03/data/unicorn.png");
    rassert(!imgUnicorn.empty(), 3428374817241);

    int width = imgUnicorn.cols;
    int height = imgUnicorn.rows;
    std::cout << "Unicorn image loaded: " << width << "x" << height << std::endl;

    std::string resultsDir = "lesson03/resultsData/";
    if (!std::filesystem::exists(resultsDir)) {
        std::filesystem::create_directory(resultsDir);
    }

//    cv::Mat blueUnicorn = makeAllBlackPixelsBlue(imgUnicorn.clone());
//    std::string filename = resultsDir + "01_blue_unicorn.jpg";
//    cv::imwrite(filename, blueUnicorn);
//
//    cv::Mat invertedUnicorn = invertImageColors(imgUnicorn.clone());
//    filename = resultsDir + "02_inverted_unicorn.jpg";
//    cv::imwrite(filename, invertedUnicorn);
//
//    cv::Mat castle = cv::imread("lesson03/data/castle.png");
//    cv::Mat unicornInCastle = addBackgroundInsteadOfBlackPixels(imgUnicorn.clone(), castle.clone());
//    filename = resultsDir + "03_unicorn_castle.png";
//    cv::imwrite(filename, unicornInCastle);
//
//    cv::Mat largeCastle = cv::imread("lesson03/data/castle_large.jpg");
//    cv::Mat unicornInLargeCastle = addBackgroundInsteadOfBlackPixelsLargeBackground(imgUnicorn.clone(), largeCastle.clone());
//    filename = resultsDir + "04_large_castle.jpg";
//    cv::imwrite(filename, unicornInLargeCastle);
//
//
//    cv::Mat nTimesImg = drawNTimes(100, imgUnicorn.clone(), largeCastle.clone());
//    filename = resultsDir + "05_random.jpg";
//    cv::imwrite(filename, nTimesImg);
//
//    cv::Mat resized;
//    cv::resize(largeCastle, resized, cv::Size(100, 100));
//    filename = resultsDir + "06_resized.jpg";
//    cv::imwrite(filename, resized);

    cv::Mat d = cv::imread("lesson03/data/dilate.png");
    cv::Mat dd = dilate(d.clone(), 1);
    std::string filename = resultsDir + "07_dilate.png";
    cv::imwrite(filename, dd);

    cv::Mat r = cv::imread("lesson03/data/erode.png");
    cv::Mat rr = erode(r.clone(), 1);
    filename = resultsDir + "08_erode.png";
    cv::imwrite(filename, rr);


}

void task2() {
    cv::Mat imgUnicorn = cv::imread("lesson03/data/unicorn.png");
    rassert(!imgUnicorn.empty(), 3428374817241);
    int updateDelay = 10;
    while (cv::waitKey(updateDelay) != 32) {

        cv::imshow("lesson03 window", setRandColor(imgUnicorn.clone()));
    }
}

struct MyVideoContent {
    cv::Mat frame;
};

void onMouseClick(int event, int x, int y, int flags, void *vec) {
    std::vector<std::pair<int, int>> &ar = *((std::vector<std::pair<int, int>>*) vec);
    if (event == cv::EVENT_LBUTTONDOWN) {
        ar.emplace_back(x, y);
    }
}

void task3() {
    cv::VideoCapture video(0);

    rassert(video.isOpened(), 3423948392481);

    MyVideoContent content;

    std::vector<std::pair<int, int>> coords;

    while (video.isOpened()) {
        bool isSuccess = video.read(content.frame);
        rassert(isSuccess, 348792347819);
        rassert(!content.frame.empty(), 3452314124643);

        cv::imshow("video", setRedColor(content.frame.clone(), coords));
        cv::setMouseCallback("video", onMouseClick, &coords);

        int key = cv::waitKey(10);
        if (key == 32 || key == 27)
            return;

    }
}

void task4() {
    cv::VideoCapture video(0);

    rassert(video.isOpened(), 3423948392481);

    MyVideoContent content;

    std::vector<std::pair<int, int>> coords;

    cv::Mat largeCastle = cv::imread("lesson03/data/castle_large.jpg");
    cv::Mat resizedCastle;

    int i = 0;

    while (video.isOpened()) {
        bool isSuccess = video.read(content.frame);
        rassert(isSuccess, 348792347819);
        rassert(!content.frame.empty(), 3452314124643);
        ++i;
        if (i == 1)
        {
            cv::resize(largeCastle, resizedCastle, cv::Size(content.frame.cols, content.frame.rows));
        }

        cv::imshow("video", setBg(content.frame, resizedCastle, getColors(content.frame, coords)));
        cv::setMouseCallback("video", onMouseClick, &coords);

        int key = cv::waitKey(10);
        if (key == 32 || key == 27)
            return;

    }
}

int main() {
    try {
//        task1();
//        task2();
//        task3();
        task4();
        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
