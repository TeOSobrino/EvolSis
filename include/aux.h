#define GENE_NUM 3 // equal to the number of variables in the objective function
#define POP_SIZE 50
#define GENERATION_NUM 200
#define BASE_MUT_RATE 0.01
#define MAX_MUT_RATE 0.64
#define MAXX 10000        // initial pop starting range [-MAXX/2, MAXX/2]
#define GENOCIDE_TIME 200 // no idea which time would be optimal
#define STALL_MAX 10 // number of generations that best individual can't change
#define SYNTH_FACTOR (POP_SIZE/20) //# of individuals to be replaced by synthesis
//#define PRINT // if defined will print a log in each generation

typedef float gene_t; // gene type can be changed, double can cause bloat

typedef struct ind_t
{
    gene_t genes[GENE_NUM];
    float fitness;

    bool operator > (const ind_t& str) const
    {
        return (fitness > str.fitness);
    }

}individual;

// pointers to functions
typedef gene_t (*eval_ptr)(individual&); // objective function

typedef void (*fitness_ptr)(eval_ptr eval_fnt, individual *,
                            individual&, int &, float &); // fitness

typedef void (*crossover_ptr)(eval_ptr eval_fnt, individual *,
                              individual&, float); // crossover

/**
 * @brief generates a random individual
 */
void individual_generate(individual& ind);

/**
 * @brief copies an individual from target to source
 */
void individual_cp(individual& target, individual& source);

/**
 * @brief copies the entire population from source to target
 */
void population_cp(individual& target_pop, individual& source_pop);

void sort_by_fitness(individual* pop, int size);

/**
 * @brief prints an individual
 */
void individual_print(individual ind);

/**
 * @brief prints the population
 */
void population_print(individual *ind);

void gen_log_print(int time, individual ind, eval_ptr obj_fnt, float mut_rate);