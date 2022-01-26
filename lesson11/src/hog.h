#pragma once

#include <opencv2/highgui.hpp>

#include <vector>

// Здесь мы будем хранить параметры нашего алгоритма:
#define NBINS 16 // число корзин (т.е. количество разных направлений угла которые мы рассматриваем)


typedef std::vector<float> HoG;


HoG buildHoG(cv::Mat grad_x, cv::Mat grad_y); // принимает на вход градиенты по обеим осям

HoG buildHoG(const cv::Mat& img); // принимает на вход оригинальную картинку и вызывает функцию buildHoG объявленную выше


std::ostream& operator << (std::ostream& os, const HoG& hog);


double distance(HoG a, HoG b);
