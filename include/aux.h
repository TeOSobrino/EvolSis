#define GENE_NUM 2 // equal to the number of variables in the objective function
#define POP_SIZE 100
#define GENERATION_NUM 500
#define BASE_MUT_RATE 0.01
#define MAXX 10000        // initial pop starting range [-MAXX/2, MAXX/2]
#define GENOCIDE_TIME 200 // no idea which time would be optimal
#define STALL_MAX 20      // number of generations that best individual doesn't
// change before mutation rate starts increasing (attempt to exit local
// minima/maxima)

#define PRINT 1 //if defined will print a log in each generation

typedef float gene_t; // gene type can be changed, double can cause bloat
typedef gene_t individual[GENE_NUM]; // individual carries their genes

// pointers to functions
typedef gene_t (*eval_ptr)(individual); // objective function
typedef void (*fitness_ptr)(eval_ptr eval_fnt, individual[POP_SIZE]); // fitness
typedef void (*crossover_ptr)(individual[POP_SIZE]); // crossover

static gene_t fitness_score[POP_SIZE]; // fitness array
static individual pop[POP_SIZE];       // population
static individual best_sol;            // best solution
static individual best_last_sol;       // best solution of last generation
static float mut_rate = BASE_MUT_RATE; // starting mutation rate
static int stall_num = 0; // number of generations since best solution changed

/**
 * @brief generates a random individual
 */
void individual_generate(individual ind);

/**
 * @brief copies an individual from target to source
 */
void individual_cp(individual target, individual source);

/**
 * @brief copies the entire population from source to target
 */
void population_cp(individual target_pop[POP_SIZE],
                   individual source_pop[POP_SIZE]);
/**
 * @brief prints an individual
 */
void individual_print(individual ind);

/**
 * @brief prints the population
 */
void population_print(individual ind[POP_SIZE]);

void gen_log_print(int time, individual ind, eval_ptr obj_fnt);