#pragma once
#include "lesson.h"
#include "config.h"
#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <iomanip>


struct FitnessReport {
    double hard_conflicts;
    double soft_gaps;
    double soft_balance;
    double capacity_conflicts;
    double type_conflicts;

    double weighted_sum; // если используешь веса
};


class Schedule {
public:
    std::vector<Lesson> lessons;

    void initialize(std::mt19937& gen) {
        lessons.clear();
        std::uniform_int_distribution<> room_dist(0, Config::NUM_ROOMS - 1);
        std::uniform_int_distribution<> slot_dist(0, Config::SLOTS_PER_DAY - 1);
        std::uniform_int_distribution<> day_dist(0, Config::NUM_DAYS - 1);
        std::uniform_int_distribution<> type_dist(0, 2);

        for (int g = 0; g < Config::NUM_GROUPS; ++g) {
            for (int d = 0; d < Config::NUM_DAYS; ++d) {
                for (int i = 0; i < 2; ++i) {
                    Lesson l;
                    l.group = g;
                    l.day = d;
                    l.slot = slot_dist(gen);
                    int room = room_dist(gen);
                    l.room = room;
                    l.room_capacity = Config::room_capacities[room];
                    l.room_type = Config::room_types[room];
                    l.type = (type_dist(gen) == 0 ? "Лекция" : (type_dist(gen) == 1 ? "Практика" : "Лабораторная"));

                    // Подбираем корректную пару преподаватель-предмет
                    auto& pairs = Config::teacher_subject_pairs;
                    std::uniform_int_distribution<> pair_dist(0, (int)pairs.size() - 1);
                    auto p = pairs[pair_dist(gen)];
                    l.teacher = p.first;
                    l.subject = p.second;

                    lessons.push_back(l);
                }
            }
        }
    }


    FitnessReport calculateFitnessReport() const {
        double hard_conflicts = 0.0;
        double soft_gaps = 0.0;
        double soft_balance = 0.0;
        double capacity_conflicts = 0.0;
        double type_conflicts = 0.0;

        // --- Жёсткие конфликты ---
        for (size_t i = 0; i < lessons.size(); ++i) {
            for (size_t j = i + 1; j < lessons.size(); ++j) {
                const Lesson& a = lessons[i];
                const Lesson& b = lessons[j];
                if (a.day == b.day && a.slot == b.slot) {
                    if (a.group == b.group) hard_conflicts++;
                    if (a.teacher == b.teacher) hard_conflicts++;
                    if (a.room == b.room) hard_conflicts++;
                }
            }
        }

        // --- Вместимость и тип аудитории ---
        for (const auto& l : lessons) {
            if (Config::group_sizes[l.group] > l.room_capacity)
                capacity_conflicts++;

            if ((l.type == "Лекция" && l.room_type != "Лекционная") ||
                (l.type == "Практика" && l.room_type != "Компьютерная") ||
                (l.type == "Лабораторная" && l.room_type != "Лабораторная"))
                type_conflicts++;
        }

        // --- Окна ---
        for (int g = 0; g < Config::NUM_GROUPS; ++g) {
            for (int d = 0; d < Config::NUM_DAYS; ++d) {
                std::vector<int> slots;
                for (const auto& l : lessons)
                    if (l.group == g && l.day == d)
                        slots.push_back(l.slot);

                std::sort(slots.begin(), slots.end());
                for (size_t i = 1; i < slots.size(); ++i)
                    soft_gaps += (slots[i] - slots[i - 1] - 1);
            }
        }

        // --- Баланс ---
        for (int g = 0; g < Config::NUM_GROUPS; ++g) {
            std::vector<int> per_day(Config::NUM_DAYS, 0);
            for (const auto& l : lessons)
                if (l.group == g)
                    per_day[l.day]++;

            double mean = 0.0;
            for (int v : per_day) mean += v;
            mean /= Config::NUM_DAYS;

            for (int v : per_day)
                soft_balance += std::pow(v - mean, 2);
        }

        // --- Нормировка ---
        double n = lessons.size();
        hard_conflicts /= (n * (n - 1) / 2.0);
        soft_gaps /= (Config::NUM_GROUPS * Config::NUM_DAYS * (Config::SLOTS_PER_DAY - 1));
        soft_balance /= (Config::NUM_GROUPS * std::pow(Config::NUM_DAYS, 2));
        capacity_conflicts /= n;
        type_conflicts /= n;

        // --- Взвешенная сумма ---
        double weighted =
            Config::weights.hard_conflict * hard_conflicts +
            Config::weights.soft_gap * soft_gaps +
            Config::weights.soft_balance * soft_balance +
            Config::weights.capacity_conflict * capacity_conflicts +
            Config::weights.type_conflict * type_conflicts;

        return {
            hard_conflicts,
            soft_gaps,
            soft_balance,
            capacity_conflicts,
            type_conflicts,
            weighted
        };
    }

    std::vector<double> calculateFitness() const {
        auto r = calculateFitnessReport();
        return {
            r.hard_conflicts,
            r.soft_gaps,
            r.soft_balance,
            r.capacity_conflicts,
            r.type_conflicts,
            r.weighted_sum
        };
    }
    
    void mutate(std::mt19937& gen, double mutation_rate = Config::MUTATION_RATE) {
        std::uniform_real_distribution<> prob(0.0, 1.0);
        std::uniform_int_distribution<> room_dist(0, Config::NUM_ROOMS - 1);
        std::uniform_int_distribution<> slot_dist(0, Config::SLOTS_PER_DAY - 1);
        std::uniform_int_distribution<> day_dist(0, Config::NUM_DAYS - 1);

        // Построим карту: subject -> возможные преподаватели
        std::vector<std::vector<int>> teachers_for_subject(Config::NUM_SUBJECTS);
        for (const auto& pair : Config::teacher_subject_pairs) {
            teachers_for_subject[pair.second].push_back(pair.first);
        }

        // Подсчёт фактической нагрузки преподавателей (в часах)
        std::unordered_map<int, double> current_load;
        for (const auto& l : lessons)
            current_load[l.teacher] += Config::HOURS_PER_CLASS;

        auto is_overloaded = [&](int t) {
            double max_hours = Config::teacher_desired_hours.at(t);
            return current_load[t] > max_hours;
        };
        auto is_underloaded = [&](int t) {
            double max_hours = Config::teacher_desired_hours.at(t);
            return current_load[t] < max_hours * 0.9; // допустим, если меньше 90%
        };

        for (auto& l : lessons) {
            if (prob(gen) < mutation_rate) {
                // --- 1. Случайное изменение дня и пары ---
                l.day = day_dist(gen);
                l.slot = slot_dist(gen);

                // --- 2. Случайная смена аудитории ---
                int new_room = room_dist(gen);
                l.room = new_room;
                l.room_capacity = Config::room_capacities[new_room];
                l.room_type = Config::room_types[new_room];

                // --- 3. Контроль трудовой нагрузки ---
                int subj = l.subject;
                int t_old = l.teacher;

                // если преподаватель перегружен — попробуем заменить
                if (is_overloaded(t_old) && !teachers_for_subject[subj].empty()) {
                    // выбираем другого преподавателя, который может вести этот предмет и не перегружен
                    std::vector<int> candidates;
                    for (int cand : teachers_for_subject[subj])
                        if (is_underloaded(cand))
                            candidates.push_back(cand);

                    if (!candidates.empty()) {
                        std::uniform_int_distribution<> pick(0, (int)candidates.size() - 1);
                        int new_teacher = candidates[pick(gen)];

                        // обновляем нагрузку
                        current_load[t_old] -= Config::HOURS_PER_CLASS;
                        current_load[new_teacher] += Config::HOURS_PER_CLASS;
                        l.teacher = new_teacher;
                    }
                }
            }
        }
    }

    void exportToCSV(const std::string& filename) const {
        std::ofstream file(filename);
        auto r = calculateFitnessReport();

        file << "# === ОЦЕНКА РАСПИСАНИЯ ===\n";
        file << "Жёсткие конфликты," << r.hard_conflicts << "\n";
        file << "Окна," << r.soft_gaps << "\n";
        file << "Баланс," << r.soft_balance << "\n";
        file << "Вместимость," << r.capacity_conflicts << "\n";
        file << "Тип аудитории," << r.type_conflicts << "\n";
        file << "Взвешенная оценка," << r.weighted_sum << "\n\n";

        file << "# === РАСПИСАНИЕ ===\n";
        file << "Группа,День,Слот,Предмет,Преподаватель,Аудитория,Тип\n";

        for (const auto& l : lessons) {
            file << Config::groups[l.group] << ","
                 << Config::days[l.day] << ","
                 << (l.slot + 1) << ","
                 << Config::subjects[l.subject] << ","
                 << Config::teachers[l.teacher] << ","
                 << Config::rooms[l.room] << ","
                 << l.type << "\n";
        }
    }

};


