#ifndef GA_H_
#define GA_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define GENE_NUM 1 //numero de genes (numero de variaveis da equacao)
#define POP_SIZE 10 //populacao com 10 indivíduos
#define GENERATION_NUM 250 //número de geracoes
#define MAXX 10000 //maior número a ser gerado
#define MUT_RATE 0.01 //taxa de mutacao base

typedef float gene_t; //o gene sera um float a principio, porem pode ser mudado
typedef gene_t individual[GENE_NUM]; //os individuos da populacao sao um vetor de genes

typedef gene_t (*eval_ptr) (individual); //ponteiro para função sendo analisada 
typedef void (*fitness_ptr) (eval_ptr eval_fnt, individual[POP_SIZE]); //ptr p funcao de fitness
typedef void (*crossover_ptr) (individual[POP_SIZE]); //ponteiro para funcao de crossover

void individual_cp(individual target, individual source);
void population_cp(individual target_pop[POP_SIZE], individual source_pop[POP_SIZE]);
void crossover(individual parent_1, individual parent_2, individual new_ind);


void fitness_fnt(eval_ptr eval_fnt, individual pop[POP_SIZE]);
void elitist_crossover(individual pop[POP_SIZE]);
void tournament_crossover(individual pop[POP_SIZE]);
void wheel_crossover(individual pop[POP_SIZE]);

gene_t* interface(char selection_type[1], eval_ptr obj_fnt);

#endif //!GA_H_