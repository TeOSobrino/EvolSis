#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "ga.h"

/**
 @brief evaluates the population fitness, saving the best solution
 * and increasing the mutation rate if the stall conditions are met

 * @param eval_fnt objective function
 * @param pop population
 */
void fitness_fnt(eval_ptr eval_fnt, individual* pop,
    individual& best_sol, int& stall_num, float& mut_rate)
{

    char best_changed = 0;
    gene_t best_fit = eval_fnt(best_sol);
    individual synthetic;
    
    for (int i = 0; i < POP_SIZE; i++) {
        pop[i].fitness = eval_fnt(pop[i]);
        for(int j = 0; j < GENE_NUM; j++){
            if(i >= POP_SIZE - POP_SIZE/(POP_SIZE+SYNTH_FACTOR))
                synthetic.genes[j] += (pop[i].genes[j]/POP_SIZE);
        }
    }

    sort_by_fitness(pop, POP_SIZE);

    if(pop[0].fitness > best_sol.fitness){
        individual_cp(best_sol, pop[0]);
        best_changed = 1;
    }

    //substitui os piores indivíduos pela média
    for(int i = 0; i < SYNTH_FACTOR; i++){
        individual_cp(pop[POP_SIZE-(1+i)], synthetic);
    }

    // metodo simoes da mutacao variavel -> primeiro busca o máximo no local
    // depois altera a taxa de mutacao dobrando
    if (!best_changed) {

        if (++stall_num == STALL_MAX) {
            mut_rate /= 2;
        } else if (stall_num == 2 * STALL_MAX) {
            mut_rate /= 2;
        } else if (mut_rate >= MAX_MUT_RATE) {
            mut_rate = BASE_MUT_RATE;
            stall_num = 0;
        } else if (stall_num == 3 * STALL_MAX) {
            mut_rate *= 8;
        } else if(stall_num > 3*STALL_MAX){
            mut_rate *= 2;
        }
    } else {
        mut_rate = BASE_MUT_RATE;
        stall_num = 0;
    }
}

/**
 * @brief avg_crossover between two individuals
 *
 * @param parent_1
 * @param parent_2
 * @param new_ind offspring produced
 */
void avg_crossover(eval_ptr eval_fnt, individual& parent_1, individual& parent_2, individual& new_ind,
    float mut_rate)
{

    int mutated_allele = rand() % GENE_NUM;

    for (int i = 0; i < GENE_NUM; i++) {
        new_ind.genes[i] = (parent_1.genes[i] + parent_2.genes[i]) / 2.00;
    }

    new_ind.genes[mutated_allele] +=
        ((gene_t)(rand() % MAXX) - (MAXX / 2)) * (gene_t)(mut_rate); // mutação
}

void central_pt_crossover(eval_ptr eval_fnt, individual& parent_1, individual& parent_2, individual& new_ind,
    float mut_rate)
{

    int mutated_allele = rand() % GENE_NUM;
    individual cand_1, cand_2;

    int mid = (GENE_NUM-1)/2;

    for (int i = 0; i <= mid; i++) {
        cand_1.genes[i] = parent_2.genes[i];
        cand_2.genes[i] = parent_1.genes[i];

        cand_1.genes[mid+i] = parent_1.genes[mid+i];
        cand_2.genes[mid+i] = parent_2.genes[mid+i];
    }

    if(eval_fnt(cand_1) > eval_fnt(cand_2)){
        individual_cp(new_ind, cand_1);
    } else {
        individual_cp(new_ind, cand_2);
    }

    new_ind.genes[mutated_allele] +=
        ((gene_t)(rand() % MAXX) - (MAXX / 2)) * (gene_t)(mut_rate); // mutação
}


/**
 * @brief mate all individuals with tournament in one side and the best in other
 * (not really a good idea, converges too fast, shouldn't be used)
 */
void elitist_selection(crossover_ptr crossover_type, eval_ptr eval_fnt, individual* pop, 
    individual& best_sol, float mut_rate)
{

    individual new_pop[POP_SIZE];
    individual_cp(new_pop[0], best_sol);

    for (int i = 1; i < POP_SIZE; i++) {
        int candidate_1 = rand() % (POP_SIZE);
        int candidate_2 = rand() % (POP_SIZE);
        int mate;

        if (pop[candidate_1].fitness > pop[candidate_2].fitness)
            mate = candidate_1;
        else
            mate = candidate_2;

        crossover_type(eval_fnt, best_sol, pop[mate], new_pop[i], mut_rate);
    }

    for (int i = 0; i < POP_SIZE; i++) {
        individual_cp(pop[i], new_pop[i]);
    }

    individual_cp(pop[0], best_sol);
}

void strictly_elitist_selection(crossover_ptr crossover_type, eval_ptr eval_fnt, individual* pop, 
     individual& best_sol, float mut_rate)
{

    individual new_pop[POP_SIZE];
    individual_cp(new_pop[0], best_sol);

    individual new_ind;

    for (int i = 0; i < POP_SIZE; i++) {
        crossover_type(eval_fnt, best_sol, pop[i], new_pop[i], mut_rate);
    }

    for (int i = 1; i < POP_SIZE; i++) {
        if (eval_fnt(new_pop[i]) > eval_fnt(pop[i]))
            individual_cp(pop[i], new_pop[i]);
    }
    individual_cp(pop[0], best_sol);
}

void tournament_selection(crossover_ptr crossover_type, eval_ptr eval_fnt, individual* pop, 
     individual& best_sol, float mut_rate)
{

    individual new_pop[POP_SIZE];

    for (int i = 0; i < POP_SIZE; i++) {
        int candidate_11 = rand() % (POP_SIZE);
        int candidate_21 = rand() % (POP_SIZE);
        int mate_1;

        if (pop[candidate_11].fitness > pop[candidate_21].fitness)
            mate_1 = candidate_11;
        else
            mate_1 = candidate_21;

        int candidate_12 = rand() % (POP_SIZE);
        int candidate_22 = rand() % (POP_SIZE);
        int mate_2;

        if (pop[candidate_12].fitness > pop[candidate_22].fitness)
            mate_2 = candidate_12;
        else
            mate_2 = candidate_22;

        crossover_type(eval_fnt, pop[mate_1], pop[mate_2], new_pop[i], mut_rate);
    }

    for (int i = 1; i < POP_SIZE; i++) {
        if (eval_fnt(new_pop[i]) > eval_fnt(pop[i]))
            individual_cp(pop[i], new_pop[i]);
    }
    individual_cp(pop[0], best_sol);
}

void wheel_selection(individual* pop) {}

void genocide(individual* pop)
{
    for (int i = 0; i < POP_SIZE; i++) {
        individual_generate(pop[i]);
    }
}

individual *interface(const char* selection, const char* crossover, eval_ptr obj_fnt)
{

    individual best_sol;            // best solution
    individual best_last_sol;       // best solution of last generation
    float mut_rate = BASE_MUT_RATE; // starting mutation rate
    int stall_num = 0; // number of generations since best solution changed

    individual* pop = (individual*) malloc(sizeof(individual) * POP_SIZE);       // population
    
    fitness_ptr eval_fnt = &fitness_fnt;
    selection_ptr selection_type;

    switch (selection[0]) {
    case 't':
        selection_type = &tournament_selection;
        break;

    case 'e':
        selection_type = &strictly_elitist_selection;
        break;
    case 's':
        selection_type = &elitist_selection;
        break;
    default:
        printf("error: %c is not a valid selection type\n", selection[0]);
        exit(1);
    }

    crossover_ptr crossover_type;
    switch (crossover[0]) {
    case 'a':
        crossover_type = &avg_crossover;
        break;
    case 'c':
        crossover_type = &central_pt_crossover;
        break;
    default:
        printf("error: %c is not a valid crossover type\n", selection[0]);
        exit(1);
    }
    srand(time(0));

    for (int i = 0; i < POP_SIZE; i++) {
        individual_generate(pop[i]);
    }

    for (int i = 0; i < GENE_NUM; i++) {
        best_sol.genes[i] = INT32_MIN;
    }

    int t = 0;
    while (t < GENERATION_NUM) {
        eval_fnt(obj_fnt, pop, best_sol, stall_num, mut_rate);
        selection_type(crossover_type, obj_fnt, pop, best_sol, mut_rate);
        t++;
        gen_log_print(t, best_sol, obj_fnt, mut_rate);

        // GENOCIDE CONDITION!
        if (t % GENOCIDE_TIME == 0)
            genocide(pop);
    }

    individual *solve = (individual *)malloc(sizeof(individual) * GENE_NUM);
    individual_cp(*solve, best_sol);
    return solve;
}