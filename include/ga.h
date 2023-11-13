#include "aux.h"

#ifndef GA_H_
#define GA_H_


void avg_crossover(eval_ptr eval_fnt, individual& parent_1, individual& parent_2, individual& new_ind,
    float mut_rate);


void fitness_fnt(eval_ptr eval_fnt, individual* pop,
    individual& best_sol, int& stall_num, float& mut_rate);

void elitist_selection(crossover_ptr crossover_type, eval_ptr eval_fnt, individual* pop, 
    individual& best_sol, float mut_rate);

/**
 * @brief mate all (other) individuals with the best individual
 */
void strictly_elitist_selection(crossover_ptr crossover_type, eval_ptr eval_fnt, individual* pop, 
    individual& best_sol, float mut_rate);

/**
 * @brief mates two individuals, choose each parent based on it's fitness after
 * randomly selecting them from the population
 */
void tournament_selection(crossover_ptr crossover_type, eval_ptr eval_fnt, individual* pop, 
    individual& best_sol, float mut_rate);

// TODO
void wheel_selection(individual* pop);

// TODO
void genocide(individual* pop);

/**
 * @brief basic interface as an example solver
 *
 * @param selection_type avg_crossover type
 * @param obj_fnt objective function (function being evaluated)
 * @return gene_t* the best individual
 */
individual *interface(const char* selection, const char* crossover, eval_ptr obj_fnt);

#endif //! GA_H_