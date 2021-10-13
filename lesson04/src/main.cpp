#include <iostream>
#include <filesystem>
#include <libutils/rasserts.h>

#include "disjoint_set.h"
#include "morphology.h"

#include <opencv2/highgui.hpp>
#include <opencv2/world.hpp>


void testingMyDisjointSets() {
    DisjointSet set(5);
    rassert(set.count_differents() == 5, 2378923791);
    for (int element = 0; element < 5; ++element) {
        rassert(set.get_set(element) == element, 23892803643);
        rassert(set.get_set_size(element) == 1, 238928031);
    }

    // TODO 100 по мере реализации DisjointSet - добавьте здесь каких-то вызовов операции "объединение двух множеств", сразу после этого проверяя через rassert что после этого результат такой как вы ожидаете
    // TODO 100 затем попробуйте создать СНМ размера 10.000.000 - и пообъединяйте какие-нибудь элементы (в цикле), быстро ли работает? а если при подвешивании одного корня множества к другому мы не будем учитывать ранк (высоту дерева) - как быстро будет работать?
    // TODO 100 попробуйте скомпилировать программу с оптимизациями и посмотреть ускорится ли программа - File->Settings->CMake->Плюсик над Debug->и переключите его в RelWithDebInfo (чтобы были хоть какие-то отладочные символы)
}

// TODO 200 перенесите сюда основную часть кода из прошлого задания про вычитание фона по первому кадру, но:
// 1) добавьте сохранение на диск визуализации картинок:
// 1.1) картинка эталонного фона
// 1.2) картинка текущего кадра
// 1.3) картинка визуализирующая маску "похож ли пиксель текущего кадра на эталонный фон"
// 1.4) картинка визуализирующая эту маску после применения волшебства морфологии
// 1.5) картинка визуализирующая эту маску после применения волшебства СНМ (системы непересекающихся множеств)
// 2) сохраняйте эти картинки визуализации только для тех кадров, когда пользователем был нажат пробел (код 32)


cv::Mat bg;
cv::Mat largeCastle = cv::imread("lesson03/data/castle_large.jpg");
cv::Mat resizedCastle;

int i = 0;
cv::Mat bg1;
cv::Mat morph;
cv::Mat set;

bool fBg = false;
bool t = false;
bool m = false;
bool d = false;

struct MyVideoContent {
    cv::Mat frame;
};

void onMouseClick(int event, int x, int y, int flags, void *frame) {
    MyVideoContent &content = *((MyVideoContent*) frame);
    if (event == cv::EVENT_LBUTTONDOWN) {
        bg = content.frame.clone();
    }
}

void backgroundMagickStreaming() {
    cv::VideoCapture video(0);

    rassert(video.isOpened(), 3423948392481);

    MyVideoContent content;

    std::string resultsDir = "lesson04/resultsData/";
    if (!std::filesystem::exists(resultsDir)) {
        std::filesystem::create_directory(resultsDir);
    }


    while (video.isOpened()) {
        bool isSuccess = video.read(content.frame);
        rassert(isSuccess, 348792347819);
        rassert(!content.frame.empty(), 3452314124643);

        if (i == 0)
        {
            cv::resize(largeCastle, resizedCastle, cv::Size(content.frame.cols, content.frame.rows));
            bg = resizedCastle;
            bg1 = resizedCastle;
            morph = resizedCastle;
            set = resizedCastle;
            ++i;
        }

        cv::blur(content.frame, content.frame, cv::Size(2, 2));
        cv::setMouseCallback("video", onMouseClick, &content);
        if (fBg) {
            cv::Mat res;
            bg1 = checkBg(content.frame, bg);
            res = bg1.clone();
            if (m) {
                morph = erode(dilate(bg1, 1), 1);
                res = morph.clone();
            }
            if (d)
            {
                set = processWithDS(res.clone(), 200);
                res = set.clone();
            }
            if (t)
            {
                res = translate(res.clone(), resizedCastle, content.frame.clone());
            }
            cv::imshow("video", res);
        }
        else
        {
            cv::imshow("video", content.frame);
        }

        int key = cv::waitKey(10);
        if (key == 32)
        {
            cv::imwrite(resultsDir + "bg.png", bg);
            cv::imwrite(resultsDir + "bg1.png", bg1);
            cv::imwrite(resultsDir + "morph.png", morph);
            cv::imwrite(resultsDir + "set.png", set);
            cv::imwrite(resultsDir + "frame.png", content.frame);
            return;
        }

        if (key == 83)
        {
            fBg = !fBg;
        }
        if (key == 84)
        {
            t = !t;
        }
        if (key == 77)
        {
            m = !m;
        }
        if (key == 68)
        {
            d = !d;
        }

    }
}


int main() {
    try {
//        testingMyDisjointSets();
        backgroundMagickStreaming();
        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
