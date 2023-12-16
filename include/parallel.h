#pragma once

void parallel_fitness_fnt(eval_ptr obj_fnt, individual *pop,
                          individual &best_sol, int &stall_num,
                          float &mut_rate);

void island_ga(eval_ptr obj_fnt, crossover_ptr crossover_type,
               fitness_ptr fitness_fnt, selection_ptr selection_type,
               individual *pop, individual &best_sol, int stall_num,
               float mut_rate);