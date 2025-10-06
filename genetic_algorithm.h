#pragma once
#include "schedule.h"
#include <random>
#include <iostream>

class GeneticAlgorithm {
private:
    std::vector<Schedule> population;
    std::mt19937 gen;

public:
    GeneticAlgorithm() {
        std::random_device rd;
        gen.seed(rd());
        for (int i = 0; i < Config::POPULATION_SIZE; ++i) {
            Schedule s;
            s.initialize(gen);
            population.push_back(s);
        }
    }

    Schedule select() {
        std::uniform_int_distribution<> dist(0, Config::POPULATION_SIZE - 1);
        Schedule &a = population[dist(gen)];
        Schedule &b = population[dist(gen)];
        return (a.calculateFitness() < b.calculateFitness()) ? a : b;
    }

    Schedule crossover(const Schedule &p1, const Schedule &p2) {
        Schedule child;
        if (p1.lessons.empty() || p2.lessons.empty()) return child;

        std::uniform_int_distribution<> dist(0, (int)p1.lessons.size() - 1);
        int point = dist(gen);
        child.lessons.reserve(p1.lessons.size());
        for (size_t i = 0; i < p1.lessons.size(); ++i)
            child.lessons.push_back(i < point ? p1.lessons[i] : p2.lessons[i]);
        return child;
    }

    Schedule run() {
        for (int gen_num = 0; gen_num < Config::MAX_GENERATIONS; ++gen_num) {
            std::vector<Schedule> new_pop;

            auto best_it = std::min_element(population.begin(), population.end(),
                [&](const Schedule &a, const Schedule &b) {
                    return a.calculateFitness(gen_num, Config::MAX_GENERATIONS) <
                           b.calculateFitness(gen_num, Config::MAX_GENERATIONS);
                });

            int best_fit = best_it->calculateFitness(gen_num, Config::MAX_GENERATIONS);
            std::cout << "Gen " << gen_num << ": best fitness = " << best_fit << std::endl;

            new_pop.push_back(*best_it); // элитизм

            while (new_pop.size() < Config::POPULATION_SIZE) {
                Schedule parent1 = select();
                Schedule parent2 = select();
                Schedule child = crossover(parent1, parent2);
                if (child.lessons.empty()) child.initialize(gen);
                child.mutate(gen);
                new_pop.push_back(child);
            }

            population = std::move(new_pop);
            if (best_fit == 0) break;
        }

        return *std::min_element(population.begin(), population.end(),
            [](const Schedule &a, const Schedule &b) {
                return a.calculateFitness() < b.calculateFitness();
            });
    }
};
