#include "genetic_algorithm.h"
#include <iostream>

void printSchedule(const Schedule &s) {
    std::cout << "\n=== Лучшее расписание (фитнес = " << s.calculateFitness() << ") ===\n";
    for (int g = 0; g < Config::NUM_GROUPS; ++g) {
        std::cout << "\nГруппа: " << Config::groups[g] << "\n";
        for (int d = 0; d < Config::NUM_DAYS; ++d) {
            std::cout << "  " << Config::days[d] << ":\n";
            std::vector<Lesson> day_lessons;
            for (auto &l : s.lessons)
                if (l.group == g && l.day == d)
                    day_lessons.push_back(l);
            std::sort(day_lessons.begin(), day_lessons.end(),
                      [](const Lesson &a, const Lesson &b) { return a.slot < b.slot; });

            for (auto &l : day_lessons) {
                std::cout << "    Лента " << l.slot + 1 << ": "
                          << Config::subjects[l.subject] << " ведет "
                          << Config::teachers[l.teacher] << " в аудитории "
                          << Config::rooms[l.room] << " (" << l.type << ")\n";
            }
        }
    }
}

int main() {
    GeneticAlgorithm ga;
    Schedule best = ga.run();
    printSchedule(best);
    return 0;
}
