#include "aux.h"

#ifndef GA_H_
#define GA_H_

void crossover(individual parent_1, individual parent_2, individual new_ind);
void fitness_fnt(eval_ptr eval_fnt, individual pop[POP_SIZE]);
void elitist_crossover(individual pop[POP_SIZE]);

/**
 * @brief mate all (other) individuals with the best individual
 */
void strictly_elitist_crossover(individual pop[POP_SIZE]);

/**
 * @brief mates two individuals, choose each parent based on it's fitness after
 * randomly selecting them from the population
 */

void tournament_crossover(individual pop[POP_SIZE]);

// TODO
void wheel_crossover(individual pop[POP_SIZE]);

//TODO
void genocide(individual pop[POP_SIZE]);

/**
 * @brief basic interface as an example solver
 * 
 * @param selection_type crossover type
 * @param obj_fnt objective function (function being evaluated)
 * @return gene_t* the best individual
 */
gene_t* interface(char selection_type[1], eval_ptr obj_fnt);

#endif //!GA_H_