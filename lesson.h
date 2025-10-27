#pragma once
#include "config.h"
#include <random>

struct Lesson {
    int group;      // Номер группы
    int teacher;    // Номер преподавателя
    int subject;    // Номер предмета
    int room;       // Номер аудитории
    int day;        // День недели
    int slot;       // Номер слота в дне
    std::string type; // Тип занятия (Лекция / Практика / Лабораторная)
    int room_capacity; // Количество мест в аудитории
    std::string room_type; // Класс аудитории (Компьютерная / Лекционная / Лабораторная)

    Lesson() = default;
};