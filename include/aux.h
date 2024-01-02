#pragma once
#include <fstream>
#include <memory>

#define GENE_NUM 40 // equal to the number of variables in the objective function
#define POP_SIZE 10000
#define GENERATION_NUM 10000
#define BASE_MUT_RATE 0.02 // starting mutation rate
#define MAX_MUT_RATE 0.64  // max mutation rate
#define MAX 1            // initial pop starting range [-MAX, MAX]
#define GENOCIDE_TIME 200  // no idea which time would be optimal
#define STALL_MAX 10 // number of generations that best individual can't change
#define SYNTH_FACTOR                                                           \
    (POP_SIZE / 20) // # of individuals to be replaced by synthesis
#define TEMPERATURE 50 //used in entropy-boltzmann selection

#define THREAD_NUM 20 //number of threads in calculating the obj_fnt
#define ISLAND_NUM 4 //not yet modifiable
#define BASE_EXCHANGE_RATE 0.05 //not yet modifiable
#define EXCHANGE_IND_NUM POP_SIZE*BASE_EXCHANGE_RATE //not yet modifiable

//#define PRINT       // if defined will print a log in each generation

typedef float gene_t; // gene type can be changed, double can cause bloat

typedef struct ind_t {
    gene_t genes[GENE_NUM];
    float fitness = -1;

    bool operator>(const ind_t &str) const { return (fitness > str.fitness); }

} individual;

// pointers to functions
typedef gene_t (*eval_ptr)(individual &); // objective function

typedef void (*fitness_ptr)(eval_ptr obj_fnt, individual *, individual &,
                            int &, float &); // fitness

typedef void (*crossover_ptr)(eval_ptr obj_fnt, individual &, individual &,
                              individual &, float);

typedef void (*selection_ptr)(crossover_ptr crossover_type, eval_ptr obj_fnt,
                              individual *, individual &,
                              float); // avg_crossover

/**
 * @brief generates a random individual
 */
void individual_generate(individual &ind);

/**
 * @brief copies an individual from target to source
 */
void individual_cp(individual &target, individual &source);

/**
 * @brief copies the entire population from source to target
 */
void population_cp(individual* target_pop, std::shared_ptr<individual[]> source_pop);

void sort_by_fitness(individual *pop, int size);

//updates mutation
inline void update_mut_rate(char &best_changed, int &stall_num, float &mut_rate)
{
    if (!best_changed) {

        if (++stall_num == STALL_MAX) {
            mut_rate /= 2;
        } else if (stall_num == 2 * STALL_MAX) {
            mut_rate /= 2;
        } else if (mut_rate >= MAX_MUT_RATE) {
            mut_rate = BASE_MUT_RATE;
            stall_num = 0;
        } else if (stall_num == 3 * STALL_MAX) {
            mut_rate = BASE_MUT_RATE;
        } else if (stall_num == 4 * STALL_MAX) {
            mut_rate *= 2;
        } else if (stall_num == 5.5 * STALL_MAX) {
            mut_rate *= 2;
        } else if (stall_num == 6.5 * STALL_MAX) {
            mut_rate *= 2;
        } else if (stall_num == 8 * STALL_MAX) {
            mut_rate *= 2;
        } else if (stall_num == 9 * STALL_MAX) {
            mut_rate *= 2;
        }else if (stall_num == 9 * STALL_MAX) {
            mut_rate *= 2;
        } else if (stall_num == 15 * STALL_MAX) {
            mut_rate = 0.001;
            stall_num = 0;
            best_changed = 1;
        }
    } else {
        mut_rate = BASE_MUT_RATE;
        stall_num = 0;
    }
    if (mut_rate > 1.0) {
        mut_rate = 1.0;
    }
}

void file_print_individual(individual ind, int time, std::ofstream file);

/**
 * @brief prints an individual
 */
void individual_print(individual ind);

/**
 * @brief prints the population
 */
void population_print(individual *ind);

void gen_log_print(int time, individual ind, eval_ptr obj_fnt, float mut_rate);
