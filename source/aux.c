#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "aux.h"

void individual_generate(individual ind)
{
        for (int i = 0; i < GENE_NUM; i++) {
                ind[i] =
                    (((gene_t)rand() / (gene_t)(INT32_MAX)) * (gene_t)MAXX) -
                    ((gene_t)(MAXX) / 2.0f);
        }
}

void individual_cp(individual target, individual source)
{

        for (int i = 0; i < GENE_NUM; i++) {
                target[i] = source[i];
        }
}

void population_cp(individual target_pop[POP_SIZE],
                   individual source_pop[POP_SIZE])
{

        for (int i = 0; i < POP_SIZE; i++) {
                individual_cp(target_pop[i], source_pop[i]);
        }
}


void individual_print(individual ind)
{
        printf("(");
        for (int i = 0; i < GENE_NUM-1; i++) {
                printf("%f, ", ind[i]);
        }
        printf("%f)", ind[GENE_NUM-1]);
}

#if PRINT

void population_print(individual ind[POP_SIZE])
{
        for (int i = 0; i < POP_SIZE; i++) {
                individual_print(ind[i]);
        }
        printf("\n");
}

void gen_log_print(int time, individual ind, eval_ptr obj_fnt)
{

        printf("t = %d, best fit = %.2f, best sol = ", time, obj_fnt(ind));
        individual_print(ind);
        printf("\n");
}

#else

void population_print(individual ind[POP_SIZE]) {}

void gen_log_print(int time, individual ind, eval_ptr obj_fnt) {}

#endif