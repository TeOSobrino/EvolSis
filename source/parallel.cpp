#include <sys/ipc.h>
#include <sys/shm.h>

#include <array>
#include <atomic>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <parallel.hpp>

#include <eigen3/Eigen/Dense>
#include <climits>

void partial_fitness_calculate(eval_ptr obj_fnt, individual *pop,
                               individual *synthetic, int lo, int hi)
{
    for (int i = lo; i < hi; i++) {
        pop[i].fitness = obj_fnt((pop)[i]);
        if (i <= POP_SIZE - SYNTH_FACTOR) {
            for (int j = 0; j < GENE_NUM; j++) {
                (*synthetic).genes[j] += ((pop)[i].genes[j] / POP_SIZE);
            }
        }
    }
}

void parallel_fitness_fnt(eval_ptr obj_fnt, individual *pop,
                          individual &best_sol, int &stall_num, double &mut_rate)
{

    char best_changed = 0;
    //gene_t best_fit = best_sol.fitness;

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

        // printf("best changed! %lf > %lf ", pop[0].fitness,
        // obj_fnt(best_sol));
        individual_print(pop[0]);
        // printf("\n");
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

void migration(individual *pop, uint64_t rg, int id)
{
    uint64_t total_size = POP_SIZE * BASE_EXCHANGE_RATE * ISLAND_NUM;
    individual *migrating = (individual *)malloc(total_size * sizeof(individual));
    
    uint64_t write_base = rg + (uint64_t) (POP_SIZE * BASE_EXCHANGE_RATE * (id-1)*sizeof(individual));
    for(uint64_t i = 0; i < POP_SIZE * BASE_EXCHANGE_RATE; i++){
        memcpy((void*)(write_base + i*sizeof(individual)), (void*)(pop + i),  sizeof(individual));
    }

    for(uint64_t i = 0; i < total_size; i++){
        migrating[i] = *((individual*)(rg + (i*sizeof(individual))));
    }

    for(uint64_t i = 0; i < total_size; i++){
        pop[POP_SIZE - 1 - i] = migrating[i];
    }

}

void island_method(eval_ptr obj_fnt, crossover_ptr crossover_type,
                   fitness_ptr fitness_fnt, selection_ptr selection_type,
                   int stall_num, double mut_rate,
                   std::vector<individual *> *best, int id, uint64_t rg)
{
    individual *b_s = (individual *)malloc(sizeof(individual));
    individual best_sol;
Eigen::Matrix<gene_t, CONTROL_DIMENSION, CONTROL_HORIZON> tiny_sol;
      tiny_sol << -0.541191, -0.550994, -0.487262, -0.478921, 
-0.472821, -0.469001, -0.520948, -0.521498, 
-0.481374, -0.47856, -0.516279, -0.517094, 
-0.487599, -0.485089, -0.511175, -0.511827, 
-0.494913, -0.493243, -0.504971, -0.505679, 
-0.500623, -0.499001, -0.499347, -0.50047, 
-0.498376, -0.497136, -0.499978, -0.499428, 
-0.500185, -0.498629, -0.49981, -0.499507, 
-0.428775, -0.494373, -0.426474, -0.49289, 
0.122413, 0.0944071, 0.1319, 0.102992, 
0.41726, 0.413115, 0.428167, 0.423405, 
0.489558, 0.49051, 0.500356, 0.500943, 
0.49017, 0.490874, 0.500277, 0.500866, 
0.490951, 0.491046, 0.500233, 0.500366, 
0.491869, 0.491918, 0.500327, 0.500365, 
0.412322, 0.44777, 0.419093, 0.454802, 
0.336132, 0.362832, 0.341366, 0.368253, 
0.268994, 0.288502, 0.272773, 0.292539, 
0.212076, 0.225806, 0.214527, 0.228557, 
0.165131, 0.17428, 0.166422, 0.175887, 
0.127254, 0.132833, 0.127577, 0.133467, 
0.0971985, 0.100049, 0.0967406, 0.0998844, 
0.0736958, 0.07452, 0.0726524, 0.0737416, 
0.0555246, 0.0548955, 0.0540626, 0.0536651, 
0.0416068, 0.0399886, 0.0398754, 0.0384524, 
0.0310383, 0.0288037, 0.0291785, 0.0271026, 
0.0230612, 0.0204995, 0.0211695, 0.0187312, 
0.0170697, 0.0144025, 0.0152364, 0.0126603, 
0.0125952, 0.00998399, 0.0108863, 0.00833854, 
0.009261, 0.00681777, 0.00771246, 0.00530879, 
0.00677192, 0.00457185, 0.00540734, 0.0032258, 
0.00491782, 0.00300221, 0.00375178, 0.00183868, 
0.00353745, 0.00192315, 0.00257109, 0.000947183, 
0.00250769, 0.00119392, 0.00173581, 0.000403509, 
0.00174596, 0.000714493, 0.00114747, 9.22858e-05, 
0.00117885, 0.000405077, 0.000738838, -6.17349e-05, 
0.000759274, 0.000212721, 0.000456911, -0.000117521, 
0.000451246, 9.85533e-05, 0.000264667, -0.000115389, 
0.000227948, 3.52034e-05, 0.000135576, -8.28363e-05, 
6.73547e-05, 2.52369e-06, 5.23029e-05, -3.56809e-05, 
-3.90762e-05, -7.79516e-06, -4.10066e-06, 7.01281e-06, 
-0.000110521, -9.91138e-06, -3.67939e-05, 4.68569e-05, 
-0.000155241, -8.22481e-06, -5.30624e-05, 8.03866e-05, 
-0.000176228, -3.08101e-06, -6.09179e-05, 0.000101886, 
-0.000179638, 4.32714e-06, -6.55797e-05, 0.000110417, 
-0.000174267, 6.40876e-06, -6.05884e-05, 0.000115289, 
-0.000160581, 1.05547e-05, -5.75006e-05, 0.000110296, 
-0.000145836, 1.31393e-05, -5.41095e-05, 0.000102108, 
-0.000126868, 1.19287e-05, -4.61981e-05, 9.22524e-05, 
-0.000106342, 1.04049e-05, -3.81263e-05, 7.96933e-05, 
-8.57917e-05, 8.68438e-06, -3.07078e-05, 6.5722e-05, 
-6.62394e-05, 6.80027e-06, -2.40879e-05, 5.14332e-05, 
-4.83659e-05, 4.8412e-06, -1.82871e-05, 3.76693e-05, 
-3.25968e-05, 2.91674e-06, -1.32931e-05, 2.50386e-05, 
-1.73231e-05, 2.31913e-06, -1.01966e-05, 1.2214e-05, 
-7.8933e-06, -4.7921e-07, -5.49796e-06, 4.43916e-06, 
-1.00871e-07, -2.15813e-06, -2.37382e-06, -2.18686e-06, 
6.0744e-06, -3.17219e-06, -2.43411e-07, -7.5445e-06, 
1.06897e-05, -3.7695e-06, 1.22356e-06, -1.15984e-05, 
1.38557e-05, -4.08264e-06, 2.21589e-06, -1.43916e-05, 
1.57273e-05, -4.18552e-06, 2.84833e-06, -1.60273e-05, 
1.64889e-05, -4.12426e-06, 3.19787e-06, -1.66501e-05, 
1.63402e-05, -3.93285e-06, 3.32248e-06, -1.64291e-05, 
1.54827e-05, -3.64045e-06, 3.27026e-06, -1.55424e-05, 
1.41093e-05, -3.2742e-06, 3.08346e-06, -1.41658e-05, 
1.23963e-05, -2.85974e-06, 2.79992e-06, -1.24634e-05, 
1.04982e-05, -2.4208e-06, 2.45328e-06, -1.05811e-05, 
8.54484e-06, -1.97853e-06, 2.07276e-06, -8.64317e-06, 
5.83732e-06, -1.92706e-06, 2.0852e-06, -5.98199e-06, 
5.44531e-06, -6.83119e-07, 8.1475e-07, -5.54183e-06;

       for (int j = 0; j < tiny_sol.rows(); ++j) {
       //std::cout << tiny_sol.rows() << std::endl;
            for (int k = 0; k < tiny_sol.cols(); ++k) {
                best_sol.genes[j*tiny_sol.cols() + k] = tiny_sol(j, k); 
            } 
        }
        //std::cout << best_sol.fitness << std::endl;
        best_sol.fitness =  INT32_MIN; //obj_fnt(best_sol);//INT32_MIN;//std::numeric_limits<gene_t>::max();
        std::cout << best_sol.fitness << std::endl;

    /*for (int i = 0; i < GENE_NUM; i++) {
        best_sol.genes[i] = INT32_MIN;
    }*/

    std::string fit_filename = "build/bestfit";
    fit_filename += std::to_string(id);
    fit_filename += ".csv";

    std::ofstream genFile(fit_filename);

    std::string pop_filename = "build/bestfit";
    pop_filename += std::to_string(id);
    pop_filename += ".csv";

    //std::string pop_filename = std::to_string(id);
    //pop_filename += "bestfit.csv";
    std::ofstream popFile(pop_filename);
    popFile << "t,best_fit\n";

    individual *pop =
        (individual *)malloc(sizeof(individual) * POP_SIZE); // population

    for (int i = 0; i < POP_SIZE; i++) {
        individual_generate(pop[i]);
    }

    int t = 0;
    while (t < GENERATION_NUM || stall_num <= 3.5 * STALL_MAX) {

        fitness_fnt(obj_fnt, pop, best_sol, stall_num, mut_rate);

        if (t % 100 == 0){
            fitness_fnt(obj_fnt, pop, best_sol, stall_num, mut_rate);
            migration(pop, rg, id);
            fitness_fnt(obj_fnt, pop, best_sol, stall_num, mut_rate);
        }
        
        selection_type(crossover_type, obj_fnt, pop, best_sol, mut_rate);
        popFile << t << "," << best_sol.fitness << "\n";
        t++;

    }
    *b_s = best_sol;
    (*best).emplace_back(b_s);

    popFile.close();
    genFile.close();
}

void island_ga(eval_ptr obj_fnt, crossover_ptr crossover_type,
               fitness_ptr fitness_fnt, selection_ptr selection_type,
               individual *pop, individual &best_sol, int stall_num,
               double mut_rate)
{
    using namespace boost::interprocess;

    shared_memory_object::remove("SHMM");
    shared_memory_object shm(create_only, "SHMM", read_write);
    shm.truncate(POP_SIZE * BASE_EXCHANGE_RATE * (ISLAND_NUM + 1) *
                 sizeof(individual));

    mapped_region rg(shm, read_write);

    std::vector<std::thread> v;
    std::vector<individual *> best;

    uint64_t base = (uint64_t)rg.get_address();
    uint64_t offset = POP_SIZE * BASE_EXCHANGE_RATE * sizeof(individual);

    int id = 1;
    std::thread t1(island_method, obj_fnt, &avg_crossover, fitness_fnt,
                   &tournament_selection, stall_num, mut_rate, &best, id++,
                   base);

    std::thread t2(island_method, obj_fnt, &central_pt_crossover, fitness_fnt,
                   &entropy_boltzmann_selection, stall_num, mut_rate, &best,
                   id++, base);

    std::thread t3(island_method, obj_fnt, &central_pt_crossover, fitness_fnt,
                   &wheel_selection, stall_num, mut_rate, &best, id++, base);

    std::thread t4(island_method, obj_fnt, &avg_crossover, fitness_fnt,
                   &elitist_selection, stall_num, mut_rate, &best, id++, base);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    best_sol = *(best[0]);
    std::cout << ("Final solutions in Each Island: ");
    for (individual *i : best) {
        std::cout << "\n";
        individual_print(*i);
        std::cout << " fitness: " << i->fitness << " ";
        if (i->fitness > best_sol.fitness) {
            best_sol = *i;
        }
    }
    printf("\n");

    shared_memory_object::remove("SHMM");
}
