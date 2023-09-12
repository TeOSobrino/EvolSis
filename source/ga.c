#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#include "ga.h"

/**
 @brief evaluates the population fitness, saving the best solution
 * and increasing the mutation rate if the stall conditions are met

 * @param eval_fnt objective function
 * @param pop population
 */
void fitness_fnt(eval_ptr eval_fnt, individual pop[POP_SIZE])
{

        char best_changed = 0;
        gene_t best_fit = eval_fnt(best_sol);
        for (int i = 0; i < POP_SIZE; i++) {
                gene_t fit = eval_fnt(pop[i]);
                fitness_score[i] = fit;
                if (fit > best_fit) {
                        best_changed = 1;
                        individual_cp(best_sol, pop[i]);
                        best_fit = fit;
                }
        }

        if (best_changed == 0) {
                if (++stall_num >= STALL_MAX)
                        mut_rate *= 2;
                if (mut_rate > 1.0)
                        mut_rate = 1.0;
                //printf("mut_r = %.3f\n", mut_rate);
                //population_print(pop);
        } else {
                mut_rate = BASE_MUT_RATE;
                stall_num = 0;
        }
}

/**
 * @brief crossover between two individuals
 *
 * @param parent_1
 * @param parent_2
 * @param new_ind offspring produced
 */
void crossover(individual parent_1, individual parent_2, individual new_ind)
{

        int mutated_allele = rand() % GENE_NUM;

        for (int i = 0; i < GENE_NUM; i++) {
                new_ind[i] = (parent_1[i] + parent_2[i]) / 2.00;
        }

        new_ind[mutated_allele] += ((gene_t)(rand() % MAXX) - (MAXX / 2)) *
                                   (gene_t)(mut_rate); // mutação
}

/**
 * @brief mate all individuals with tournament in one side and the best in other
 * (not really a good idea, converges too fast, shouldn't be used)
 */
void elitist_crossover(individual pop[POP_SIZE])
{

        individual new_pop[POP_SIZE];
        individual_cp(new_pop[0], best_sol);

        for (int i = 1; i < POP_SIZE; i++) {
                int candidate_1 = rand() % (POP_SIZE);
                int candidate_2 = rand() % (POP_SIZE);
                int mate;

                if (fitness_score[candidate_1] > fitness_score[candidate_2])
                        mate = candidate_1;
                else
                        mate = candidate_2;

                crossover(best_sol, pop[mate], new_pop[i]);
        }

        for (int i = 0; i < POP_SIZE; i++) {
                pop[i][0] = new_pop[i][0];
        }
}

void strictly_elitist_crossover(individual pop[POP_SIZE])
{

        individual new_pop[POP_SIZE];
        individual_cp(new_pop[0], best_sol);

        individual new_ind;

        for (int i = 0; i < POP_SIZE; i++) {
                crossover(best_sol, pop[i], new_pop[i]);
        }

        for (int i = 0; i < POP_SIZE; i++) {
                individual_cp(pop[i], new_pop[i]);
        }
}

void tournament_crossover(individual pop[POP_SIZE])
{

        individual new_pop[POP_SIZE];

        for (int i = 0; i < POP_SIZE; i++) {
                int candidate_11 = rand() % (POP_SIZE);
                int candidate_21 = rand() % (POP_SIZE);
                int mate_1;

                if (fitness_score[candidate_11] > fitness_score[candidate_21])
                        mate_1 = candidate_11;
                else
                        mate_1 = candidate_21;

                int candidate_12 = rand() % (POP_SIZE);
                int candidate_22 = rand() % (POP_SIZE);
                int mate_2;

                if (fitness_score[candidate_12] > fitness_score[candidate_22])
                        mate_2 = candidate_12;
                else
                        mate_2 = candidate_22;

                crossover(pop[mate_1], pop[mate_2], new_pop[i]);
        }

        for (int i = 0; i < POP_SIZE; i++) {
                individual_cp(pop[i], new_pop[i]);
        }

        individual_cp(pop[0], best_sol);
}

void wheel_crossover(individual pop[POP_SIZE]) {}

void genocide(individual pop[POP_SIZE])
{
        for(int i = 0; i < POP_SIZE; i++){
                individual_generate(pop[i]);
        }
}

gene_t *interface(char selection_type[1], eval_ptr obj_fnt)
{

        fitness_ptr eval_fnt = &fitness_fnt;
        crossover_ptr crossover_type;

        switch (selection_type[0]) {
        case 't':
                crossover_type = &tournament_crossover;
                break;

        case 'e':
                crossover_type = &strictly_elitist_crossover;
                break;
        default:
                printf("error: %c is not a valid crossover type\n", selection_type[0]);
                break;
        }

        srand(time(0));

        for (int i = 0; i < POP_SIZE; i++) {
                individual_generate(pop[i]);
        }

        for (int i = 0; i < GENE_NUM; i++) {        
                best_sol[i] = INT32_MIN;
        }

        int t = 0;
        while (t < GENERATION_NUM) {
                eval_fnt(obj_fnt, pop);
                crossover_type(pop);
                t++;
                gen_log_print(t, best_sol, obj_fnt);

                //GENOCIDE CONDITION!
                if(t%150 == 0)
                        genocide(pop);
        }

        gene_t *solve = malloc(sizeof(gene_t) * GENE_NUM);
        individual_cp(solve, best_sol);
        return solve;
}