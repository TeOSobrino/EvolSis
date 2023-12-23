#include "ga.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <iostream>

gene_t hill_fnt(individual& ind)
{
    gene_t val = ind.genes[0];
    return (val > 10 ? 20 - val : val);
}

// obs: para usar a parabóla verifique se MAX^2 <= RAND_MAX -> (MAX << 2^16)
gene_t parabola(individual& ind)
{
    gene_t val = ind.genes[0];
    gene_t y = -val * val + 12 * val + 8;
    ind.fitness = y;

    return y;
}

gene_t senoid(individual& ind)
{

    gene_t x = ind.genes[0];
    gene_t y = (2 * cos(0.039 * x) + 5 * sin(0.05 * x) + 0.5 * cos(0.01 * x) +
                10 * sin(0.07 * x) + 5 * sin(0.1 * x) + 5 * sin(0.035 * x)) *
                   10 +
               500;
    ind.fitness = y;

    return y;
}

gene_t multi_dim(individual& ind)
{
    gene_t x = ind.genes[0];
    gene_t y = ind.genes[1];

    gene_t z = 5 - 2 * (x * x + 5 * y * y);

    ind.fitness = z;
    return z;
}

gene_t wtf(individual& ind)
{
    gene_t x = ind.genes[0];
    gene_t y = ind.genes[1];
    gene_t z = ind.genes[2];

    gene_t k = -5 * x * x - 10 * y * y - 5 * z * z + 10 + 4 * x * y -
               5 * x * z + 10 * x + 4 * z - 5 * x * x * x * x -
               3 * x * x * x * x * z * z * z * z;
    ind.fitness = k;
    
    return k;
}

int main(int argc, char **argv)
{

    eval_ptr evalued_fnt = &wtf;

    individual* best_s = interface("b", "c", evalued_fnt);

    printf("best fit: %.3f, best_sol = ", evalued_fnt(*best_s));
    individual_print(*best_s);
    printf("\n");

    free(best_s);

    std::string s = "python3 ./source/pyplot.py ";
    s += std::to_string(ISLAND_NUM);
    if(system(s.c_str()))
        std::cout << "error printing" << "\n";

    return 0;
}