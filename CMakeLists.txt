cmake_minimum_required(VERSION 3.1)

add_subdirectory(libs)

project(CPPExercises)

set(CMAKE_CXX_STANDARD 17)

# распознавание цифр/букв: построение HoG (Histogram Of Gradients) для цифр/букв
add_subdirectory(lesson11) # распознавание цифр/букв: учимся разбивать картинку на цифры/буквы
add_subdirectory(lesson12) # поиск прямой на базе точек
add_subdirectory(lesson13) # сопоставление двух картинок для склейки "панорамы" (SIFT + K-ratio test + Left-Right check + RANSAC)
add_subdirectory(lesson14) # накладывание картинки на другую в видеопотоке с веб. камеры (простой вариант AR)
add_subdirectory(lesson15) # накладывание картинки на другую в видеопотоке с веб. камеры (простой вариант AR)
add_subdirectory(lesson16) # накладывание картинки на другую в видеопотоке с веб. камеры (простой вариант AR)
