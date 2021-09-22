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

    cv::Mat blueUnicorn = makeAllBlackPixelsBlue(imgUnicorn.clone());
    std::string filename = resultsDir + "01_blue_unicorn.jpg";
    cv::imwrite(filename, blueUnicorn);

    cv::Mat invertedUnicorn = invertImageColors(imgUnicorn.clone());
    filename = resultsDir + "02_inverted_unicorn.jpg";
    cv::imwrite(filename, invertedUnicorn);

    cv::Mat castle = cv::imread("lesson03/data/castle.png");
    cv::Mat unicornInCastle = addBackgroundInsteadOfBlackPixels(imgUnicorn.clone(), castle.clone());
    filename = resultsDir + "03_unicorn_castle.png";
    cv::imwrite(filename, unicornInCastle);

    cv::Mat largeCastle = cv::imread("lesson03/data/castle_large.jpg");
    cv::Mat unicornInLargeCastle = addBackgroundInsteadOfBlackPixelsLargeBackground(imgUnicorn.clone(), largeCastle.clone());
    filename = resultsDir + "04_large_castle.jpg";
    cv::imwrite(filename, unicornInLargeCastle);


    cv::Mat nTimesImg = drawNTimes(100, imgUnicorn.clone(), largeCastle.clone());
    filename = resultsDir + "05_random.jpg";
    cv::imwrite(filename, nTimesImg);

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
    int lastClickX;
    int lastClickY;
};

void onMouseClick(int event, int x, int y, int flags, void *pointerToMyVideoContent) {
    MyVideoContent &content = *((MyVideoContent*) pointerToMyVideoContent);
    if (event == cv::EVENT_LBUTTONDOWN) {
        std::cout << "Left click at x=" << x << ", y=" << y << std::endl;
    }
}

void task3() {
    cv::VideoCapture video(0);

    rassert(video.isOpened(), 3423948392481);

    MyVideoContent content;

    while (video.isOpened()) {
        bool isSuccess = video.read(content.frame);
        rassert(isSuccess, 348792347819);
        rassert(!content.frame.empty(), 3452314124643);

        cv::imshow("video", content.frame);
        cv::setMouseCallback("video", onMouseClick, &content);

        int key = cv::waitKey(10);
        // TODO добавьте завершение программы в случае если нажат пробел
        // TODO добавьте завершение программы в случае если нажат Escape (придумайте как нагуглить)

        // TODO сохраняйте в вектор (std::vector<int>) координаты всех кликов мышки
        // TODO и перед отрисовкой очередного кадра - заполняйте все уже прокликанные пиксели красным цветом
    }
}

void task4() {
    // TODO на базе кода из task3 (скопируйте просто его сюда) сделайте следующее:
    // при клике мышки - определяется цвет пикселя в который пользователь кликнул, теперь этот цвет считается прозрачным (как было с черным цветом у единорога)
    // и теперь перед отрисовкой очередного кадра надо подложить вместо прозрачных пикселей - пиксель из отмасштабированной картинки замка (castle_large.jpg)

    // TODO попробуйте сделать так чтобы цвет не обязательно совпадал абсолютно для прозрачности (чтобы все пиксели похожие на тот что был кликнут - стали прозрачными, а не только идеально совпадающие)
}

int main() {
    try {
//        task1();
        task2();
//        task3();
//        task4();
        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
