#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <chrono>
#include <iostream>
#include <filesystem>
#include <memory>

#include <libutils/rasserts.h>


void drawText(cv::Mat img, const std::string &text, double fontScale, int &yOffset) {
    cv::Scalar white(255, 255, 255);
    // рассчитываем высоту текста в пикселях:
    float textHeight = cv::getTextSize(text, cv::FONT_HERSHEY_DUPLEX, fontScale, 1, nullptr).height;
    yOffset += textHeight; // увеличиваем сдвиг на высоту текста в пикселях
    cv::putText(img, text, cv::Point(0, yOffset), cv::FONT_HERSHEY_DUPLEX, fontScale, white);
}


void run() {
    const bool useWebcam = true;

    bool drawOver = true; // рисовать ли поверх наложенную картинку (можно включить-включить чтобы мигнуть картинкой и проверить качество выравнивания)
    bool drawDebug = true; // рисовать ли поверх отладочную информацию (например красный кант вокруг нарисованной поверх картинки)
    bool useSIFTDescriptor = false; // SIFT работает довольно медленно, попробуйте использовать ORB + не забудьте что тогда вам нужен другой DescriptorMatcher

    cv::Mat imgFrame, imgForDetection, imgToDraw;
    // если у вас не работает через веб. камеру - будут использоваться заготовленные картинки
    imgFrame = cv::imread(
            "lesson14/data/1_box2/box0.png"); // пример кадра с вебкамеры, на нем мы хотим найти объект и вместо него нарисовать другую картинку
    imgForDetection = cv::imread(
            "lesson14/data/1_box2/box1.png"); // пример картинки которую мы хотим найти на видеокадре
    imgToDraw = cv::imread(
            "lesson14/data/1_box2/box1_nesquik.png"); // пример картинки которую мы хотим нарисовать заместо искомой
    double c = 1.0 * imgForDetection.cols / imgForDetection.rows;
    cv::resize(imgForDetection.clone(), imgForDetection, cv::Size((int) (c * 300), 300), cv::INTER_LINEAR);
    cv::resize(imgToDraw.clone(), imgToDraw, cv::Size(imgForDetection.cols, imgForDetection.rows), cv::INTER_LINEAR);
    rassert(!imgFrame.empty(), 324789374290023);
    rassert(!imgForDetection.empty(), 3789572984290019);
    rassert(!imgToDraw.empty(), 3789572984290021);

    std::shared_ptr<cv::VideoCapture> video;
    if (useWebcam) {
        std::cout << "Trying to open web camera..." << std::endl;
        video = std::make_shared<cv::VideoCapture>(0);
        rassert(video->isOpened(), 3482789346289027);
        std::cout << "Web camera video stream opened." << std::endl;
    }

    while (true) {
        cv::Mat currentFrame; // текущий кадр с веб. камеры
        if (useWebcam) {
            bool isSuccess = video->read(currentFrame);
            rassert(isSuccess, 347283947290039);
            rassert(!currentFrame.empty(), 347283947290040);
        } else {
            currentFrame = imgFrame; // или если у вас не работает OpenCV с веб. камерой - то пусть хотя бы картинка-пример используется
        }

        auto frameProcessingStartTime = std::chrono::steady_clock::now(); // замеряем сколько сейчас времени чтобы оценить FPS

        cv::Mat mainWindowImage = currentFrame.clone(); // делаем копию чтобы на ней рисовать любую отладочную информацию не портя оригинальную картинку

        {
            // TODO сопоставьте две картинки: currentFrame и imgForDetection, затем нарисуйте imgToDraw в соответствии с матрицей Гомографии
            cv::Ptr<cv::FeatureDetector> detector;
            cv::Ptr<cv::DescriptorMatcher> matcher;
            if (useSIFTDescriptor) {
                detector = cv::SIFT::create();
                matcher = cv::FlannBasedMatcher::create();
            } else {
                detector = cv::ORB::create();
                matcher = cv::BFMatcher::create();
            }

            // TODO детектируйте и постройте дескрипторы у ключевых точек
            std::vector<cv::KeyPoint> keypoints0, keypoints1; // здесь будет храниться список ключевых точек
            cv::Mat descriptors0, descriptors1; // здесь будут зраниться дескрипторы этих ключевых точек
//            std::cout << "Detecting SIFT keypoints and describing them (computing their descriptors)..." << std::endl;
            detector->detectAndCompute(mainWindowImage, cv::noArray(), keypoints0, descriptors0);
            detector->detectAndCompute(imgForDetection, cv::noArray(), keypoints1, descriptors1);
//             std::cout << "Keypoints initially: " << keypoints0.size() << ", " << keypoints1.size() << "..." << std::endl;

            // TODO сопоставьте ключевые точки

            std::vector<std::vector<cv::DMatch>> matches01;
//            std::cout << "Matching " << keypoints0.size() << " points with " << keypoints1.size() << "..." << std::endl;
            matcher->knnMatch(descriptors0, descriptors1, matches01, 2);
//            std::cout << "matching done" << std::endl;

            // TODO пофильтруйте сопоставления, как минимум через K-ratio test, но лучше на ваш выбор
            std::vector<cv::Point2f> points0;
            std::vector<cv::Point2f> points1;
            int goodMatches = 0;
            for (int i = 0; i < keypoints0.size(); ++i) {
                int fromKeyPoint0 = i;
                int toKeyPoint1Best = matches01[i][0].trainIdx;
                float distanceBest = matches01[i][0].distance;
                rassert(fromKeyPoint0 == i, 348723974920074);
                rassert(toKeyPoint1Best < keypoints1.size(), 347832974820076);

                int toKeyPoint1SecondBest = matches01[i][1].trainIdx;
                float distanceSecondBest = matches01[i][1].distance;
                rassert(distanceBest <= distanceSecondBest, 34782374920082);

                if (distanceBest < 0.85 * distanceSecondBest) {
                    points0.push_back(keypoints0[i].pt);
                    points1.push_back(keypoints1[toKeyPoint1Best].pt);
                    ++goodMatches;
                }
            }
            rassert(points0.size() == points1.size(), 234723947289089);
//            std::cout << goodMatches << "/" << keypoints0.size() << " good matches left" << std::endl;


            // TODO findHomography(...) + рисование поверх:
            const double ransacReprojThreshold = 3.0;
            std::vector<unsigned char> inliersMask; // в этот вектор RANSAC запишет флажки - какие сопоставления он посчитал корректными (inliers)
            try {
                cv::Mat H = cv::findHomography(points1, points0, cv::RANSAC, ransacReprojThreshold, inliersMask);
                if (H.empty()) {
                    // см. документацию https://docs.opencv.org/4.5.1/d9/d0c/group__calib3d.html#ga4abc2ece9fab9398f2e560d53c8c9780
                    // "Note that whenever an H matrix cannot be estimated, an empty one will be returned."
                    std::cout << "FAIL 24123422!" << std::endl;
                } else {
                    cv::Mat overlapImg = imgToDraw.clone();
                    if (drawDebug) {
                        // рисуем красный край у накладываемой картинки
                        cv::Scalar red(0, 0, 255);
                        cv::rectangle(overlapImg, cv::Point(0, 0), cv::Point(overlapImg.cols - 1, overlapImg.rows - 1),
                                      red, 5);
                    }
                    if (drawOver) {
                        cv::warpPerspective(overlapImg, mainWindowImage, H, mainWindowImage.size(), cv::INTER_LINEAR,
                                            cv::BORDER_TRANSPARENT);
                    }
                }
            } catch (const std::exception &e) {}
        }

        if (drawDebug) {
            int textYOffset = 0;

            auto frameProcessingEndTime = std::chrono::steady_clock::now();
            int timeForFrame = std::chrono::duration_cast<std::chrono::milliseconds>(
                    frameProcessingEndTime - frameProcessingStartTime).count();
            int fps;
            if (timeForFrame == 0) {
                fps = 99999;
            } else {
                fps = (int) std::round(1000.0 / timeForFrame);
            }
            drawText(mainWindowImage, std::to_string(fps) + " FPS", 0.5, textYOffset);

            // TODO добавьте короткую справку про кнопки управления
            drawText(mainWindowImage, "Controls: ", 0.5, textYOffset);
            drawText(mainWindowImage, "1 - replace the search image with the current frame", 0.5, textYOffset);
            drawText(mainWindowImage, "2 - replace the rendering image with the current frame", 0.5, textYOffset);
            drawText(mainWindowImage, "3 - rendering debugging information", 0.5, textYOffset);
            drawText(mainWindowImage, "H - hide the image drawn on top", 0.5, textYOffset);
            drawText(mainWindowImage, "S - change SIFT/ORB", 0.5, textYOffset);

            // TODO добавьте разбивку сколько времени занимает детектирование, сколько матчинг, сколько фильтрация (по аналогии с тем как выше замерялось время на обработку для рассчета FPS):
//            drawText(mainWindowImage, "Timings: " + std::to_string(timeForFrame) + " ms = "
//                    + std::to_string(detect_ms) + " ms detect + " + TODO,
//                    0.5, textYOffset);
        }

        // Рисуем все три окошка:
        cv::imshow("Frame", mainWindowImage);
        cv::imshow("Image for detection", imgForDetection);
        cv::imshow("Image to draw", imgToDraw);

        // Смотрим нажал ли пользователь какую-нибудь кнопку
        int key = cv::waitKey(
                10); // число в скобочках - максимальное число миллисекунд которые мы ждем кнопки от пользователя, а иначе - считаем что ничего не нажато
        if (key == -1) {
            // прошло 5 миллисекунд но ничего не было нажато - значит идем обрабатывать следующий кадр с веб. камеры
        } else if (key == 27) { // Esc - выключаем программу
            break;
        } else if (useWebcam && key == 49) {
            imgForDetection = currentFrame.clone();
            double c = 1.0 * imgForDetection.cols / imgForDetection.rows;
            cv::resize(imgForDetection.clone(), imgForDetection, cv::Size((int) (c * 300), 300), cv::INTER_LINEAR);
            cv::resize(imgToDraw.clone(), imgToDraw, cv::Size(imgForDetection.cols, imgForDetection.rows), cv::INTER_LINEAR);
        } else if (useWebcam && key == 50) {
            imgToDraw = currentFrame.clone();
        } else if (key == 104) {
            drawOver = !drawOver;
        } else if (key == 51) {
            drawDebug = !drawDebug;
        } else if (key == 115) {
            useSIFTDescriptor = !useSIFTDescriptor;
        }
    }
    }


    int main() {
        try {
            std::cin.tie(nullptr);
            std::ios::sync_with_stdio(false);
            run();

            return 0;
        } catch (const std::exception &e) {
            std::cout << "Exception! " << e.what() << std::endl;
            return 1;
        }
    }
