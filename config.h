#pragma once
#include <string>
#include <vector>

namespace Config {
    // Основные параметры
    const int NUM_GROUPS = 3;
    const int NUM_TEACHERS = 5;
    const int NUM_ROOMS = 6;
    const int NUM_SUBJECTS = 5;
    const int SLOTS_PER_DAY = 6;
    const int NUM_DAYS = 5;
    const int POPULATION_SIZE = 100;
    const int MAX_GENERATIONS = 500;
    const double MUTATION_RATE = 0.1;

    // Весовые коэффициенты для фитнеса
    struct FitnessWeights {
        double hard_conflict = 10.0;   // За каждый конфликт преподавателя / комнаты / группы
        double soft_gap = 5.0;         // За окна в расписании
        double soft_balance = 5.0;     // За неравномерное распределение
        double capacity_conflict = 10.0; // За нехватку мест
        double type_conflict = 10.0;    // За неподходящий класс аудитории
    };

    inline FitnessWeights weights;

    // Тестовые данные 
    inline std::vector<std::string> days = { "Понедельник", "Вторник", "Среда", "Четверг", "Пятница", "Суббота" };
    inline std::vector<std::string> groups = { "КИ22-03Б", "КИ21-03Б", "КИ22-04Б" };
    inline std::vector<std::string> teachers = { "Иванов", "Смирнов", "Петров", "Соболев", "Едреев" };
    inline std::vector<std::string> subjects = { "Математика", "Физика", "Химия", "Биология", "История" };
    inline std::vector<std::string> rooms = { "101", "102", "103", "104", "105", "106" };

    // Количество студентов в каждой группе
    inline std::vector<int> group_sizes = { 25, 30, 20 }; // КИ22-03Б: 25, КИ21-03Б: 30, КИ22-04Б: 20

    // Количество мест и класс аудитории
    inline std::vector<int> room_capacities = { 30, 40, 20, 25, 50, 15 }; // Аудитории 101–106
    inline std::vector<std::string> room_types = {
        "Лекционная",    // 101
        "Компьютерная",  // 102
        "Лабораторная",  // 103
        "Лекционная",    // 104
        "Компьютерная",  // 105
        "Лабораторная"   // 106
    };

    // Допустимые связи: какой преподаватель может вести какой предмет
    inline std::vector<std::pair<int, int>> teacher_subject_pairs = {
        {0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2}, {2, 3}, {3, 3}, {3, 4}, {4, 0}, {4, 4}
    };
}
