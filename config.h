#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace Config {
    // Основные параметры
    const int NUM_GROUPS = 3;
    const int NUM_TEACHERS = 5;
    const int NUM_ROOMS = 6;
    const int NUM_SUBJECTS = 5;
    const int SLOTS_PER_DAY = 6;
    const int NUM_DAYS = 6; // включая субботу
    const int POPULATION_SIZE = 10;
    const int MAX_GENERATIONS = 500;
    const double MUTATION_RATE = 0.1;
    const double HOURS_PER_CLASS = 1.5;  // 1 пара = 1.5 часа

    // Весовые коэффициенты для фитнеса
    struct FitnessWeights {
        double hard_conflict = 1;         // За каждый конфликт преподавателя / комнаты / группы
        double soft_gap = 1;               // За окна в расписании
        double soft_balance = 1;           // За неравномерное распределение
        double capacity_conflict = 1;     // За нехватку мест
        double type_conflict = 1;         // За неподходящий класс аудитории
        double teacher_load = 1;           // За отклонение от желаемой нагрузки
    };

    inline FitnessWeights weights;

    // Тестовые данные
    inline std::vector<std::string> days = { "Понедельник", "Вторник", "Среда", "Четверг", "Пятница", "Суббота" };
    inline std::vector<std::string> groups = { "КИ22-03Б", "КИ21-03Б", "КИ22-04Б" };
    inline std::vector<std::string> teachers = { "Иванов", "Смирнов", "Петров", "Соболев", "Едреев" };
    inline std::vector<std::string> subjects = { "Математика", "Физика", "Химия", "Биология", "История" };
    inline std::vector<std::string> rooms = { "101", "102", "103", "104", "105", "106" };

    // === Размеры групп ===
    inline std::vector<int> group_sizes = { 25, 30, 20 };

    // === Параметры аудиторий ===
    inline std::vector<int> room_capacities = { 30, 40, 20, 25, 50, 15 };
    inline std::vector<std::string> room_types = {
        "Лекционная", "Компьютерная", "Лабораторная",
        "Лекционная", "Компьютерная", "Лабораторная"
    };

    // Допустимые связи (преподаватель–предмет)
    inline std::vector<std::pair<int, int>> teacher_subject_pairs = {
        {0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2}, {2, 3},
        {3, 3}, {3, 4}, {4, 0}, {4, 4}
    };

    inline std::unordered_map<int, double> teacher_desired_hours = {
        {0, 180.0},  // Иванов
        {1, 240.0},  // Смирнов
        {2, 300.0},  // Петров
        {3, 180.0},  // Соболев
        {4, 120.0}   // Едреев
    };
}
