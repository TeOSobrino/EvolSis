#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include "ga.h"

static gene_t fitness_score[POP_SIZE]; //vetor de fitness
static individual pop[POP_SIZE]; //população
static individual best_sol; //melhor solução armazenada de forma global

void individual_cp(individual target, individual source){

    for(int i = 0; i < GENE_NUM; i++){
        target[i] = source[i];
    }
}

void population_cp(individual target_pop[POP_SIZE], individual source_pop[POP_SIZE]){
    
    for(int i = 0; i < POP_SIZE; i++){
        individual_cp( target_pop[i], source_pop[i]); 
    }
}

void fitness_fnt(eval_ptr eval_fnt, individual pop[POP_SIZE]){

    gene_t best_fit = eval_fnt(best_sol);
    for(int i = 0; i < POP_SIZE; i++){
        gene_t fit = eval_fnt(pop[i]);
        fitness_score[i] = fit;
        if(fit > best_fit){
            individual_cp(best_sol, pop[i]);
            best_fit = fit;
        }
    }
}

void crossover(individual parent_1, individual parent_2, individual new_ind){

    int mutated_allele = rand()%GENE_NUM;

    for(int i = 0; i < GENE_NUM; i++){
        new_ind[i] = (parent_1[i]+parent_2[i])/(gene_t)2.00;
    }

    new_ind[mutated_allele] += ((gene_t) (rand()%MAXX)-(MAXX/2)) * (gene_t)(MUT_RATE/1.00);//mutação
}


void elitist_crossover(individual pop[POP_SIZE]){

    individual new_pop[POP_SIZE];
    individual_cp(new_pop[0], best_sol);

    for(int i = 1; i < POP_SIZE; i++){
        int candidate_1 = rand()%(POP_SIZE);
        int candidate_2 = rand()%(POP_SIZE);
        int mate;

        if(fitness_score[candidate_1] > fitness_score[candidate_2])
            mate = candidate_1;
        else 
            mate = candidate_2;
        
        crossover(best_sol, pop[mate], new_pop[i]);
    }

    for(int i = 0; i < POP_SIZE; i++){
        pop[i][0] = new_pop[i][0];
    }
}

void strictly_elitist_crossover(individual pop[POP_SIZE]){

    individual new_pop[POP_SIZE];
    individual_cp(new_pop[0], best_sol);

    individual new_ind;

    for(int i = 0; i < POP_SIZE; i++){
        
        crossover(new_pop[0], new_pop[i], new_ind);
    }
}

void tournament_crossover(individual pop[POP_SIZE]){

    individual new_pop[POP_SIZE];
    individual_cp(new_pop[0], best_sol);

    for(int i = 1; i < POP_SIZE; i++){
        int candidate_11 = rand()%(POP_SIZE);
        int candidate_21 = rand()%(POP_SIZE);
        int mate_1;

        if(fitness_score[candidate_11] > fitness_score[candidate_21])
            mate_1 = candidate_11;
        else 
            mate_1 = candidate_21;
        
        int candidate_12 = rand()%(POP_SIZE);
        int candidate_22 = rand()%(POP_SIZE);
        int mate_2;

        if(fitness_score[candidate_12] > fitness_score[candidate_22])
            mate_2 = candidate_12;
        else 
            mate_2 = candidate_22;
        
        crossover(new_pop[mate_1], new_pop[mate_2], new_pop[i]);
    }

    for(int i = 0; i < POP_SIZE; i++){
        pop[i][0] = new_pop[i][0];
    }
}

void wheel_crossover(individual pop[POP_SIZE]){}


gene_t* interface(char selection_type[1], eval_ptr obj_fnt){

    fitness_ptr eval_fnt = &fitness_fnt;
    crossover_ptr crossover_type;
    
    switch (selection_type[0]){
    case 't':
        crossover_type = &tournament_crossover;    
        break;

    default:
        crossover_type = &elitist_crossover;
        
        break;
    }

    srand(time(0));
    
    for(int i = 0; i < POP_SIZE; i++)
        for(int j = 0; j < GENE_NUM; j++){
            pop[i][j] = ((gene_t)rand()/(gene_t)(RAND_MAX))*(gene_t)MAXX; //float
            best_sol[j] = INT32_MIN;
        }

    int t = 0;
    while(t < GENERATION_NUM){
        eval_fnt(obj_fnt, pop);
        crossover_type(pop);
        t++;
    }

    gene_t* solve = malloc(sizeof(gene_t)*GENE_NUM);
    individual_cp(solve, best_sol);
    return solve;
}