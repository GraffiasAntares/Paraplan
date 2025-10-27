#include "genetic_algorithm.h"
#include <iostream>
#define NOMINMAX
#include <windows.h> 
#include <algorithm>

void printSchedule(const Schedule& s, int index) {
    auto fitness = s.calculateFitness();
    std::cout << "\n=== Расписание " << index + 1 << " (Конфликты = " << fitness[0]
        << ", Окна = " << fitness[1] << ", Баланс = " << fitness[2]
        << ", Нехватка мест = " << fitness[3] << ", Неподходящий тип = " << fitness[4] << ") ===\n";
    for (int g = 0; g < Config::NUM_GROUPS; ++g) {
        std::cout << "\nГруппа: " << Config::groups[g] << " (" << Config::group_sizes[g] << " студентов)\n";
        for (int d = 0; d < Config::NUM_DAYS; ++d) {
            std::cout << "  " << Config::days[d] << ":\n";
            std::vector<Lesson> day_lessons;
            for (auto& l : s.lessons)
                if (l.group == g && l.day == d)
                    day_lessons.push_back(l);
            std::sort(day_lessons.begin(), day_lessons.end(),
                [](const Lesson& a, const Lesson& b) { return a.slot < b.slot; });

            for (auto& l : day_lessons) {
                std::cout << "    Лента " << l.slot + 1 << ": "
                    << Config::subjects[l.subject] << " ведет "
                    << Config::teachers[l.teacher] << " в аудитории "
                    << Config::rooms[l.room] << " (" << l.room_type << ", " << l.room_capacity << " мест, " << l.type << ")\n";
            }
        }
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "ru_RU.UTF-8");
    GeneticAlgorithm ga;
    std::vector<Schedule> pareto_front = ga.run();

    // Сортировка Парето-фронта по критериям (приоритет: конфликты, окна, баланс, вместимость, тип)
    std::sort(pareto_front.begin(), pareto_front.end(), [](const Schedule& a, const Schedule& b) {
        auto fa = a.calculateFitness();
        auto fb = b.calculateFitness();
        if (fa[0] != fb[0]) return fa[0] < fb[0]; // Меньше конфликтов
        if (fa[1] != fb[1]) return fa[1] < fb[1]; // Меньше окон
        if (fa[2] != fb[2]) return fa[2] < fb[2]; // Лучше баланс
        if (fa[3] != fb[3]) return fa[3] < fb[3]; // Меньше конфликтов по вместимости
        return fa[4] < fb[4]; // Меньше конфликтов по типу
        });

    // Ограничение вывода до 10 лучших расписаний
    size_t output_limit = std::min(pareto_front.size(), size_t(10));
    std::cout << "\n=== Найдено " << pareto_front.size() << " Парето-оптимальных расписаний, выводятся 10 лучших ===\n";
    for (size_t i = 0; i < output_limit; ++i) {
        printSchedule(pareto_front[i], i);
    }
    return 0;
}