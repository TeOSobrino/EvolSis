#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

/**
 * Passos do AG:
 * 1. Inicializar a populcao
 * 2. Atribuir uma pontuacap p/ cada individuo (fitness)
 * 3. Crossover (cruzamento)¹ 
 * 4. Selecionar os melhores
 * 5. rearranjar populacao
 * 6. retornar ao passo 2 até atingir a resposta satisfatoria²
 * ¹ os indivíduos com melhor pontuacao devem ter maiores chances de cruzar
 * ² a resposta satisfatoria pode não existir, porem na pratica o algoritmo
 * deve ter a capacidade de halt para que sua resposta seja obtida.   
*/

#define GENE_NUM 1 //numero de genes (numero de variaveis da equacao)
#define POP_SIZE 10 //populacao com 10 indivíduos
#define GEN_NUM 100 //número de geracoes
#define MAXX 10000 //maior número a ser gerado
#define MUT_RATE 0.04 //taxa de mutacao base = 10%


typedef float gene_t; //o gene sera um float a principio, porem pode ser mudado
typedef gene_t individual[GENE_NUM]; //os individuos da populacao sao um vetor de genes

typedef gene_t (*eval_ptr) (individual); //ponteiro para função sendo analisada 
typedef void (*fitness_ptr) (eval_ptr eval_fnt, individual[POP_SIZE]); //ponteiro para a função de fitness

gene_t fitness_score[POP_SIZE]; //vetor de fitness
individual best_sol; //melhor solução armazenada de forma global

void printArr(int* arr, int size);
void printPop(individual* arr, int size);

gene_t hillFnt(individual ind);
gene_t parabola(individual ind);
void fitnessFnt(eval_ptr eval_fnt, individual pop[POP_SIZE]);

void elitistCrossover(individual pop[POP_SIZE]);
void tournamentCrossover(individual pop[POP_SIZE]);

int main (int argc, char** argv){

    //1. incialização da populção usar matriz mesmo?? alternativas -> vetorzão (melhor pra cahce)
    // -> (qual outra alternativa ??) quando tivermos +1 gene no indivíduo 
    //(vetor de struct parece ser pior) uns cara de stanford usaram matriz ent fodase
    srand(time(0));
    printf("time %lu\n", time(NULL));
    
    individual pop[POP_SIZE];
    for(int i = 0; i < POP_SIZE; i++)
        for(int j = 0; j < GENE_NUM; j++)
            pop[i][j] = rand()%MAXX + (rand()%MAXX)/MAXX;
    
    //2. fitness
    eval_ptr eval_fnt = &parabola;
    // eval_ptr eval_fnt = &hillFnt;
    fitness_ptr fitness_fnt = &fitnessFnt;

    int t = 0;
    while(t < 100){
        fitness_fnt(eval_fnt, pop);
        //elitistCrossover(pop);
        tournamentCrossover(pop);
        t++;
    }
    return 0;
}

void printArr(int* arr, int size){
    for(int i = 0; i < size; i++){
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void printPop(individual* arr, int size){
    for(int i = 0; i < size; i++){
        printf("%d ", arr[i][0]);
    }
    printf("\n");
}

gene_t hillFnt(individual ind){
    gene_t val = ind[0]; //tem q abstrair esse treco
    return (val > 500 ? 1000 - val : val); //haha ternário
}

//obs: para usar a parabóla verifique se MAXX^2 <= RAND_MAX (MAXX <= 2^16) 
gene_t parabola(individual ind){
    gene_t val = ind[0]; //abstrair
    gene_t y = -val*val + 12*val + 8;
    return y;
}

//para o exemplo simples, a função de fitness será apenas uma 
//normalização de variáveis x no intervalo [a-b] => n = (x-a)/(b-a) 
//buscando o valor que maximiza a função.
void fitnessFnt(eval_ptr eval_fnt, individual pop[POP_SIZE]){

    int ub = eval_fnt(pop[0]); //upper bound 
    int lb = ub; //lower bound
    fitness_score[0] = ub;
    best_sol[0] = pop[0][0];

    //encontrar lb e ub, atribuir a pontuação relativa
    for(int i = 1; i < POP_SIZE; i++){
        gene_t fit = eval_fnt(pop[i]);
        fitness_score[i] = fit;
        if(fit > ub){
            best_sol[0] = pop[i][0];
            ub = fit;
        }
        else if(fit < lb)
            lb = fit;
    }

    int range = ub - lb + 1;
    for(int i = 0; i < POP_SIZE; i++){
        fitness_score[i] = (((fitness_score[i] - lb)*100)/(range));
    } 

    printf("best sol = %.2f\n", best_sol[0]);  
}

void elitistCrossover(individual pop[POP_SIZE]){

    individual new_pop[POP_SIZE];
    new_pop[0][0] = best_sol[0]; //precisa abstrair aqui também

    for(int i = 1; i < POP_SIZE; i++){
        int candidate_1 = rand()%(POP_SIZE);
        int candidate_2 = rand()%(POP_SIZE);
        int mate;

        if(fitness_score[candidate_1] > fitness_score[candidate_2])
            mate = candidate_1;
        else 
            mate = candidate_2;
        
        new_pop[i][0] = (best_sol[0] + pop[mate][0])/2 
            + ((double) (rand()%MAXX)-MAXX/2)*MUT_RATE/1.0f; //mutação
    }

    for(int i = 0; i < POP_SIZE; i++){
        pop[i][0] = new_pop[i][0];
    }
}

void tournamentCrossover(individual pop[POP_SIZE]){

    individual new_pop[POP_SIZE];
    new_pop[0][0] = best_sol[0]; //precisa abstrair aqui também

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
        
        new_pop[i][0] = (pop[mate_1][0] + pop[mate_2][0])/2 //asbtrair aqui
            + ((double) (rand()%MAXX)-(MAXX/2))*MUT_RATE/1.0f; //mutação
    }

    for(int i = 0; i < POP_SIZE; i++){
        pop[i][0] = new_pop[i][0];
    }
}