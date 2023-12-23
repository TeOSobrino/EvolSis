#include <sys/ipc.h>
#include <sys/shm.h>

#include <atomic>
#include <array>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <iostream>
#include <thread>
#include <vector>


#include "aux.h"
#include "ga.h"
#include "parallel.h"

void partial_fitness_calculate(eval_ptr obj_fnt, individual *pop,
                               individual *synthetic, int lo, int hi)
{
    for (int i = lo; i < hi; i++) {
        obj_fnt((pop)[i]);
        if (i <= POP_SIZE - SYNTH_FACTOR) {
            for (int j = 0; j < GENE_NUM; j++) {
                (*synthetic).genes[j] += ((pop)[i].genes[j] / POP_SIZE);
            }
        }
    }
}

void parallel_fitness_fnt(eval_ptr obj_fnt, individual *pop,
                          individual &best_sol, int &stall_num, float &mut_rate)
{

    char best_changed = 0;
    gene_t best_fit = obj_fnt(best_sol);

    std::vector<std::thread> v;
    std::array<individual, THREAD_NUM> synthetic;

    int step = POP_SIZE / THREAD_NUM;

    for (int i = 0; i < THREAD_NUM; i++) {
        int beg = (i * step);
        int end = (i + 1 * step);

        if (i + 1 == THREAD_NUM)
            end = POP_SIZE;

        std::thread t(partial_fitness_calculate, obj_fnt, pop, &(synthetic[i]),
                      beg, end);
        v.push_back(std::move(t));
    }
    for (auto &t : v) {
        t.join();
    }

    individual synth;
    for (int i = 0; i < THREAD_NUM; i++) {
        for (int j = 0; j < GENE_NUM; j++) {
            synth.genes[j] += synthetic[i].genes[j] / THREAD_NUM;
        }
    }

    sort_by_fitness(pop, POP_SIZE);

    // if (obj_fnt(pop[0]) > obj_fnt(best_sol)) {
    if (pop[0].fitness > best_sol.fitness) {

        printf("best changed! %lf > %lf ", pop[0].fitness, obj_fnt(best_sol));
        individual_print(pop[0]);
        printf("\n");
        individual_cp(best_sol, pop[0]);
        best_changed = 1;
    }

    synth.fitness = obj_fnt(synth);
    // replace the worst individuals for the average
    for (int i = 0; i < SYNTH_FACTOR; i++) {
        individual_cp(pop[POP_SIZE - (1 + i)], synth);
    }

    // start by decreasing mutation rate -> search for local optimal, only then
    // increase mutation rate, looking for global optimal
    update_mut_rate(best_changed, stall_num, mut_rate);
}

void island_method(eval_ptr obj_fnt, crossover_ptr crossover_type,
                   fitness_ptr fitness_fnt, selection_ptr selection_type,
                   int stall_num, float mut_rate,
                   std::vector<individual *> *best, int id)
{
    individual *b_s = (individual *)malloc(sizeof(individual));
    individual best_sol;
    for (int i = 0; i < GENE_NUM; i++) {
        best_sol.genes[i] = INT32_MIN;
    }

    individual *pop =
        (individual *)malloc(sizeof(individual) * POP_SIZE); // population

    for (int i = 0; i < POP_SIZE; i++) {
        individual_generate(pop[i]);
    }

    int t = 0;
    while (t < GENERATION_NUM || stall_num <= 3.5*STALL_MAX) {
        fitness_fnt(obj_fnt, pop, best_sol, stall_num, mut_rate);
        selection_type(crossover_type, obj_fnt, pop, best_sol, mut_rate);
        t++;
        gen_log_print(t, best_sol, obj_fnt, mut_rate);
    }
    *b_s = best_sol;
    (*best).emplace_back(b_s);
}

void island_ga(eval_ptr obj_fnt, crossover_ptr crossover_type,
               fitness_ptr fitness_fnt, selection_ptr selection_type,
               individual *pop, individual &best_sol, int stall_num,
               float mut_rate)
{
    using namespace boost::interprocess;

    shared_memory_object::remove("SHMM");
    shared_memory_object shm(create_only, "SHMM", read_write);
    shm.truncate(POP_SIZE * BASE_EXCHANGE_RATE * (ISLAND_NUM + 1) *
                 sizeof(individual));

    mapped_region rg(shm, read_write);

    std::vector<std::thread> v;
    std::vector<individual *> best;

    int id = 1;
    std::thread t1(island_method, obj_fnt, &avg_crossover, fitness_fnt,
                   &tournament_selection, stall_num, mut_rate, &best, id++);
    std::thread t2(island_method, obj_fnt, &avg_crossover, fitness_fnt,
                   &entropy_boltzmann_selection, stall_num, mut_rate, &best, id++);
    std::thread t3(island_method, obj_fnt, &central_pt_crossover, fitness_fnt,
                   &wheel_selection, stall_num, mut_rate, &best, id++);
    std::thread t4(island_method, obj_fnt, &avg_crossover, fitness_fnt,
                   &elitist_selection, stall_num, mut_rate, &best, id++);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    best_sol = *(best[0]);
    std::cout << ("Final solutions in Each Island: ");
    for (individual *i : best) {
        std::cout << "\n";
        individual_print(*i);
        std::cout << " fitness: " << i->fitness <<" ";
        if (i->fitness > best_sol.fitness) {
            best_sol = *i;
        }
    }
    printf("\n");

    shared_memory_object::remove("SHMM");
}