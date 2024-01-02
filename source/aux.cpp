#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <random>
#include <utility>

#include "aux.h"

void individual_generate(individual &ind)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<> d(-MAX, MAX);

    for (int i = 0; i < GENE_NUM; i++) {
        ind.genes[i] = (((gene_t)d(gen)));
    }
}

void individual_cp(individual &target, individual &source)
{

    for (int i = 0; i < GENE_NUM; i++) {
        target.genes[i] = source.genes[i];
    }
    target.fitness = source.fitness;
}

void population_cp(individual *target_pop, std::shared_ptr<individual[]> source_pop)
{

    for (int i = 0; i < POP_SIZE; i++) {
        individual_cp(target_pop[i], source_pop[i]);
    }
}

void sort_by_fitness(individual *pop, int size)
{
    std::sort(pop, pop + size, std::greater<individual>());
}

void file_print_individual(individual ind, int time, std::ofstream file){

    file << time << "," << ind.fitness << "\n";
}

#ifdef PRINT

void individual_print(individual ind)
{
    std::cout << "(";
    for (int i = 0; i < GENE_NUM - 1; i++) {
        std::cout << ind.genes[i] << ", ";
    }
    std::cout << ind.genes[GENE_NUM-1] << ")";
}



void population_print(individual ind[POP_SIZE])
{
    for (int i = 0; i < POP_SIZE; i++) {
        individual_print(ind[i]);
    }
    std::cout << "\n";
}

void gen_log_print(int time, individual ind, eval_ptr obj_fnt, float mut_rate)
{

    std::cout << std::setprecision(4) << "t = " << time << ", best fit = " << obj_fnt(ind) <<", best sol = ";
    individual_print(ind);
    std::cout << std::setprecision(4) << " mut = " << mut_rate << "\n";
}

#else

void population_print(individual ind[POP_SIZE]) {}

void gen_log_print(int time, individual ind, eval_ptr obj_fnt, float mut_rate)
{
}
void individual_print(individual ind) {}
#endif
