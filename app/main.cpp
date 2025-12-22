#include "genetic_algorithm.h"
#include <iostream>
#include <iomanip> // для std::setprecision
#include <filesystem>

void printSchedule(const Schedule& s, int index) {
    auto fitness = s.calculateFitness();

    // --- Взвешенная сумма для справки ---
    double weighted_sum =
        Config::weights.hard_conflict     * fitness[0] +
        Config::weights.soft_gap          * fitness[1] +
        Config::weights.soft_balance      * fitness[2] +
        Config::weights.capacity_conflict * fitness[3] +
        Config::weights.type_conflict     * fitness[4] +
        Config::weights.teacher_load      * fitness[5];

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "\n=== Расписание " << index + 1
              << " ===\n"
              << "Взвешенная оценка: " << weighted_sum << "\n"
              << "├─ Конфликты (жесткие): " << fitness[0]
              << " × вес " << Config::weights.hard_conflict << "\n"
              << "├─ Окна в расписании: " << fitness[1]
              << " × вес " << Config::weights.soft_gap << "\n"
              << "├─ Баланс нагрузки: " << fitness[2]
              << " × вес " << Config::weights.soft_balance << "\n"
              << "├─ Вместимость аудиторий: " << fitness[3]
              << " × вес " << Config::weights.capacity_conflict << "\n"
              << "├─ Тип аудитории: " << fitness[4]
              << " × вес " << Config::weights.type_conflict << "\n"
              << "├─ Нагрузка преподавателей: " << fitness[5]
              << " × вес " << Config::weights.teacher_load << "\n";

    // --- Далее вывод самого расписания ---
    for (int g = 0; g < Config::NUM_GROUPS; ++g) {
        std::cout << "\nГруппа: " << Config::groups[g] << "\n";
        for (int d = 0; d < Config::NUM_DAYS; ++d) {
            std::cout << "  " << Config::days[d] << ":\n";
            std::vector<Lesson> day_lessons;

            for (auto& l : s.lessons)
                if (l.group == g && l.day == d)
                    day_lessons.push_back(l);

            std::sort(day_lessons.begin(), day_lessons.end(),
                      [](const Lesson& a, const Lesson& b) { return a.slot < b.slot; });

            for (auto& l : day_lessons) {
                std::cout << "    Пара " << l.slot + 1 << ": "
                          << Config::subjects[l.subject] << " ведет "
                          << Config::teachers[l.teacher]
                          << " в аудитории " << Config::rooms[l.room]
                          << " (" << l.type << ")\n";
            }
        }
    }
}


int main() {
    GeneticAlgorithm ga;
    std::vector<Schedule> pareto_front = ga.run();

//    std::cout << "\n=== Найдено " << pareto_front.size()
//              << " Парето-оптимальных расписаний ===\n";
//
//    for (size_t i = 0; i < pareto_front.size(); ++i) {
//        std::string filename = "/Users/gyuk/Star/Учёба/СФУ/ККП/Расписания/schedule_" + std::to_string(i+1) + ".csv";
//        pareto_front[i].exportToCSV(filename);
//    }

    std::filesystem::create_directory("output");

    for (size_t i = 0; i < pareto_front.size(); ++i) {
        printSchedule(pareto_front[i], i);
        pareto_front[i].exportToCSV(
            "/Users/gyuk/Star/Учёба/СФУ/ККП/Расписания/расписание_" + std::to_string(i+1) + ".csv"
        );
    }

    return 0;
}
