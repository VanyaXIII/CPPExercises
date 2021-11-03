#include <filesystem>
#include <iostream>
#include <libutils/rasserts.h>

#include "blur.h" // TODO реализуйте функцию блюра с произвольной силой размытия в файле blur.cpp

void testSomeBlur() {
    // TODO выберите любую картинку и любую силу сглаживания - проверьте что результат - чуть размытая картинка
    // Входные картинки для тестирования возьмите из предыдущего урока (т.е. по пути lesson05/data/*).
    // Результирующие картинки сохарняйте в эту папку (т.е. по пути lesson06/resultsData/*).

    std::string name = "test";
    cv::Mat img = cv::imread("lesson06/data/" + name + ".jpg");

    cv::imwrite("lesson06/resultsData/" + name + "_res.jpg", blur(img.clone(), 30));

    name = "sergey";
    img = cv::imread("lesson06/data/" + name + ".jpg");

    cv::imwrite("lesson06/resultsData/" + name + "_res.jpg", blur(img.clone(), 100));

    name = "color";
    img = cv::imread("lesson06/data/" + name + ".jpg");

    cv::imwrite("lesson06/resultsData/" + name + "_res.jpg", blur(img.clone(), 50));
}

void testManySigmas() {
    // TODO возьмите ту же самую картинку но теперь в цикле проведите сглаживание для нескольких разных сигм
    // при этом результирующую картинку сохраняйте с указанием какая сигма использовалась:
    // для того чтобы в название файла добавить значение этой переменной -
    // воспользуйтесь функцией преобразующей числа в строчки - std::to_string(sigma)
}

int main() {
    try {
        testSomeBlur();
        testManySigmas();

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}

