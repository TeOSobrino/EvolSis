// this program aims to find global maxima, if minima, change function signal
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <algorithm>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

#include "ga.h"

typedef struct{
    int first;
    int second;

}m_int_pair;

void fitness_normalize(individual *pop)
{
    sort_by_fitness(pop, POP_SIZE);
    double l = pop[0].fitness;
    double r = pop[POP_SIZE - 1].fitness;
    double g = l - r;

    double acc = 0.0;
    for (int i = 0; i < POP_SIZE; i++) {
        pop[i].fitness = (pop[i].fitness - r) / g;
        acc += pop[i].fitness;
    }
}

void fitness_fnt(eval_ptr eval_fnt, individual *pop, individual &best_sol,
                 int &stall_num, float &mut_rate)
{

    char best_changed = 0;
    gene_t best_fit = eval_fnt(best_sol);
    individual synthetic;

    for (int i = 0; i < POP_SIZE; i++) {
        pop[i].fitness = eval_fnt(pop[i]);
        if (i <= POP_SIZE - SYNTH_FACTOR) {
            for (int j = 0; j < GENE_NUM; j++) {
                synthetic.genes[j] += (pop[i].genes[j] / POP_SIZE);
            }
        }
    }

    sort_by_fitness(pop, POP_SIZE);

    if (pop[0].fitness > best_sol.fitness) {
        individual_cp(best_sol, pop[0]);
        best_changed = 1;
    }

    synthetic.fitness = eval_fnt(synthetic);
    // replace the worst individuals for the average
    for (int i = 0; i < SYNTH_FACTOR; i++) {
        individual_cp(pop[POP_SIZE - (1 + i)], synthetic);
    }

    // start by decreasing mutation rate -> search for local optimal, only then
    // increase mutation rate, looking for global optimal
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
        } else if (stall_num > 3 * STALL_MAX) {
            mut_rate *= 2;
        }
    } else {
        mut_rate = BASE_MUT_RATE;
        stall_num = 0;
    }
    if (mut_rate > MAX_MUT_RATE) {
        mut_rate = MAX_MUT_RATE;
    }
}

void avg_crossover(eval_ptr eval_fnt, individual &parent_1,
                   individual &parent_2, individual &new_ind, float mut_rate)
{

    int mutated_allele = rand() % GENE_NUM;

    for (int i = 0; i < GENE_NUM; i++) {
        new_ind.genes[i] = (parent_1.genes[i] + parent_2.genes[i]) / 2.00;
    }

    new_ind.genes[mutated_allele] +=
        ((gene_t)(rand() % MAX) - (MAX / 2)) * (gene_t)(mut_rate); // mutation
}

void central_pt_crossover(eval_ptr eval_fnt, individual &parent_1,
                          individual &parent_2, individual &new_ind,
                          float mut_rate)
{
    int mutated_allele = rand() % GENE_NUM;
    individual cand_1, cand_2;

    int mid = (GENE_NUM - 1) / 2;

    for (int i = 0; i <= mid; i++) {
        cand_1.genes[i] = parent_2.genes[i];
        cand_2.genes[i] = parent_1.genes[i];

        cand_1.genes[mid + i] = parent_1.genes[mid + i];
        cand_2.genes[mid + i] = parent_2.genes[mid + i];
    }

    if (eval_fnt(cand_1) > eval_fnt(cand_2)) {
        individual_cp(new_ind, cand_1);
    } else {
        individual_cp(new_ind, cand_2);
    }

    new_ind.genes[mutated_allele] +=
        ((gene_t)(rand() % MAX) - (MAX / 2)) * (gene_t)(mut_rate); // mutation
}

void elitist_selection(crossover_ptr crossover_type, eval_ptr eval_fnt,
                       individual *pop, individual &best_sol, float mut_rate)
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

void strictly_elitist_selection(crossover_ptr crossover_type, eval_ptr eval_fnt,
                                individual *pop, individual &best_sol,
                                float mut_rate)
{

    individual new_pop[POP_SIZE];
    individual_cp(new_pop[0], best_sol);

    individual new_ind;

    for (int i = 0; i < POP_SIZE; i++) {
        crossover_type(eval_fnt, best_sol, pop[i], new_pop[i], mut_rate);
    }

    population_cp(pop, new_pop);
    individual_cp(pop[0], best_sol);
}

void tournament_selection(crossover_ptr crossover_type, eval_ptr eval_fnt,
                          individual *pop, individual &best_sol, float mut_rate)
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

        crossover_type(eval_fnt, pop[mate_1], pop[mate_2], new_pop[i],
                       mut_rate);
    }

    // population_cp(pop, new_pop);
    for (int i = 0; i < POP_SIZE; i++) {
        if (pop[i].fitness < eval_fnt(new_pop[i])) {
            individual_cp(pop[i], new_pop[i]);
        }
    }
    individual_cp(pop[0], best_sol);
}

m_int_pair* prob_select(individual *pop, float& mate_1_p, float& mate_2_p)
{
    int j = 0;
    int mate_1 = -1;
    int mate_2 = -1;

    printf("m1p = %f, m2p = %f\n", mate_1_p, mate_2_p);

    while ((mate_1_p > 0 || mate_2_p > 0) && j < POP_SIZE-1) {
        if (mate_1_p > 0) {
            mate_1_p -= pop[j].fitness;
        } else {
            mate_1 = j;
            while (mate_2_p > 0) {
                mate_2_p -= pop[j].fitness;
                j++;
            }
            mate_2 = j;
            break;
        }
        if (mate_2_p > 0) {
            mate_2_p -= pop[j].fitness;
        } else {
            mate_2 = j;
            while (mate_1_p > 0) {
                mate_1_p -= pop[j].fitness;
                j++;
            }
            mate_1 = j;
            break;
        }
        j++;
    }

    if (mate_1 == -1)
        mate_1 = j;
    if (mate_2 == -1)
        mate_2 = j;

    m_int_pair* p = (m_int_pair*) malloc(sizeof(m_int_pair));
    p->first = mate_1;
    p->second = mate_2;

    return p;
}

void entropy_boltzmann_selection(crossover_ptr crossover_type,
                                 eval_ptr eval_fnt, individual *pop,
                                 individual &best_sol, float mut_rate)
{
    fitness_normalize(pop);
    double acc = 0.0;
    for (int i = 0; i < POP_SIZE; i++) {
        acc += pop[i].fitness;
    }
    
    std::random_device rd;
    std::mt19937 gen(rd());

    double k = M_PI * ((double)2.0 / (double)(pop[POP_SIZE / 2].fitness));
    acc /= k;
    
    std::exponential_distribution<> d(k);

    individual new_pop[POP_SIZE];
    for (int i = 0; i < POP_SIZE; i++) {
        float mate_1_p = d(gen) * acc;
        float mate_2_p = d(gen) * acc;

        m_int_pair* p = prob_select(pop, mate_1_p, mate_2_p);

        int mate_1 = p->first;
        int mate_2 = p->second;

        printf("m_1 = %d, m_2 = %d\n", mate_1, mate_2);
        crossover_type(eval_fnt, pop[mate_1], pop[mate_2], new_pop[i],
                       mut_rate);
    }

    population_cp(pop, new_pop);
    individual_cp(new_pop[0], best_sol);
}

void wheel_selection(crossover_ptr crossover_type, eval_ptr eval_fnt,
                     individual *pop, individual &best_sol, float mut_rate)
{
    sort_by_fitness(pop, POP_SIZE);
    fitness_normalize(pop);
    double norm = pop[0].fitness;
    double acc = 0.0;
    for (int i = 0; i < POP_SIZE; i++) {
        acc += pop[i].fitness;
    }

    individual new_pop[POP_SIZE];
    for (int i = 0; i < POP_SIZE; i++) {
        
        float r_1 = 0, r_2 = 0;
        r_1 = ((double)rand() / (double)(RAND_MAX)) * acc;
        r_2 = ((double)rand() / (double)(RAND_MAX)) * acc;

        m_int_pair* p = prob_select(pop, r_1, r_2);

        int mate_1 = p->first;
        int mate_2 = p->second;

        // int j = 0;
        // int mate_1 = -1;
        // int mate_2 = -1;

        // while (r_1 > 0 || r_2 > 0) {
        //     if (r_1 > 0) {
        //         r_1 -= pop[j].fitness;
        //     } else {
        //         mate_1 = j;
        //         while (r_2 > 0) {
        //             r_2 -= pop[j].fitness;
        //             j++;
        //         }
        //         mate_2 = j;
        //         break;
        //     }
        //     if (r_2 > 0) {
        //         r_2 -= pop[j].fitness;
        //     } else {
        //         mate_2 = j;
        //         while (r_1 > 0) {
        //             r_1 -= pop[j].fitness;
        //             j++;
        //         }
        //         mate_1 = j;
        //         break;
        //     }
        //     j++;
        // }

        // if (mate_1 == -1)
        //     mate_1 = j;
        // if (mate_2 == -1)
        //     mate_2 = j;

        // individual new_ind;

        crossover_type(eval_fnt, pop[mate_1], pop[mate_2], new_pop[i],
                       mut_rate);
    }

    population_cp(pop, new_pop);
    individual_cp(pop[0], best_sol);
}

void genocide(individual *pop)
{
    for (int i = 0; i < POP_SIZE; i++) {
        individual_generate(pop[i]);
    }
}

individual *interface(const char *selection, const char *crossover,
                      eval_ptr obj_fnt)
{

    individual best_sol;            // best solution
    individual best_last_sol;       // best solution of last generation
    float mut_rate = BASE_MUT_RATE; // starting mutation rate
    int stall_num = 0; // number of generations since best solution changed

    individual *pop =
        (individual *)malloc(sizeof(individual) * POP_SIZE); // population

    fitness_ptr eval_fnt = &fitness_fnt;
    selection_ptr selection_type;

    switch (selection[0]) {
    case 'b':
        selection_type = &entropy_boltzmann_selection;
        break;
    case 'e':
        selection_type = &strictly_elitist_selection;
        break;
    case 's':
        selection_type = &elitist_selection;
        break;
    case 't':
        selection_type = &tournament_selection;
        break;

    case 'w':
        selection_type = &wheel_selection;
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

    srand(time(NULL));

    for (int i = 0; i < POP_SIZE; i++) {
        individual_generate(pop[i]);
    }

    for (int i = 0; i < GENE_NUM; i++) {
        best_sol.genes[i] = INT32_MIN;
    }

    int t = 0;
    while (t < GENERATION_NUM || (stall_num < 3.5 * STALL_MAX)) {
        eval_fnt(obj_fnt, pop, best_sol, stall_num, mut_rate);
        selection_type(crossover_type, obj_fnt, pop, best_sol, mut_rate);
        t++;
        gen_log_print(t, best_sol, obj_fnt, mut_rate);

        // maybe change genocide condition
        if (t % GENOCIDE_TIME == 0)
            genocide(pop);
    }

    individual *solve = (individual *)malloc(sizeof(individual) * GENE_NUM);
    individual_cp(*solve, best_sol);
    return solve;
}