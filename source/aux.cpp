#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <algorithm>
#include <functional>
#include <random>
#include <utility>

#include "aux.h"

void individual_generate(individual &ind)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<> d(-MAX, MAX);

    for (int i = 0; i < GENE_NUM; i++) {
        ind.genes[i] = (((gene_t)d(gen) / (gene_t)(INT32_MAX)) * (gene_t)MAX) -
                       ((gene_t)(MAX) / 2.0f);
    }
}

void individual_cp(individual &target, individual &source)
{

    for (int i = 0; i < GENE_NUM; i++) {
        target.genes[i] = source.genes[i];
    }
    target.fitness = source.fitness;
}

void population_cp(individual *target_pop, individual *source_pop)
{

    for (int i = 0; i < POP_SIZE; i++) {
        individual_cp(target_pop[i], source_pop[i]);
    }
}

void individual_print(individual ind)
{
    printf("(");
    for (int i = 0; i < GENE_NUM - 1; i++) {
        printf("%f, ", ind.genes[i]);
    }
    printf("%f)", ind.genes[GENE_NUM-1]);
}

void sort_by_fitness(individual *pop, int size)
{
    std::sort(pop, pop + size, std::greater<individual>());
}

#ifdef PRINT

void population_print(individual ind[POP_SIZE])
{
    for (int i = 0; i < POP_SIZE; i++) {
        individual_print(ind[i]);
    }
    printf("\n");
}

void gen_log_print(int time, individual ind, eval_ptr obj_fnt, float mut_rate)
{

    printf("t = %d, best fit = %.2f, best sol = ", time, obj_fnt(ind));
    individual_print(ind);
    printf(" mut = %.4f\n", mut_rate);
}

#else

void population_print(individual ind[POP_SIZE]) {}

void gen_log_print(int time, individual ind, eval_ptr obj_fnt, float mut_rate)
{
}

#endif