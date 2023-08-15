#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * Passos do AG:
 * 1. Inicializar a populção
 * 2. Atribuir uma pontuação p/ cada indivíduo (fitness)
 * 3. Crossover (cruzamento)¹ 
 * 4. Selecionar os melhores
 * 5. rearranjar população
 * 6. retornar ao passo 2 até atingir a resposta satisfatória²
 * ¹ os indivíduos com melhor pontuação deve ter maior chance de cruzar
 * ² a resposta satisfatória pode não existir, porém na prática o algoritmo
 * deve ter a capacidade de halt para que sua resposta seja obtida.   
*/

//para o exemplo inicial, consideramos uma função simples que altera a
//função de fitness, a função é:

// if(x <= 500) y = x;
// else y = 1000 - x;

typedef int gene; //o gene será um int (tbd), talvez seja ruim usar isso
#define GENE_NUM 1 //para o problema modelo o número de genes será 1 (uma variável)
typedef int individual[GENE_NUM];

individual best_sol; //melhor solução armazenada de forma global
#define POP_SIZE 10 //população com 10 indivíduos

typedef int (*eval_ptr) (individual); //ponteiro para função sendo analisada 
typedef void (*fitness_ptr) (eval_ptr eval_fnt, individual[POP_SIZE]); //ponteiro para a função de fitness

int fitness_score[POP_SIZE]; //vetor de fitness

#define MUT_RATE 0.1 //taxa de mutação base = 10%

void printArr(int* arr, int size){
    for(int i = 0; i < size; i++){
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int hillFnt(individual ind){

    int val = ind[0]; //tem q abstrair esse treco
    return (val > 500 ? 1000 - val : val); //haha ternário
}
//para o exemplo simples, a função de fitness será apenas uma 
//normalização de variáveis x no intervalo [a-b] => n = (x-a)/(b-a) 
void fitnessFnt(eval_ptr eval_fnt, individual pop[POP_SIZE]){

    int ub = eval_fnt(pop[0]); //upper bound 
    int lb = ub; //lower bound
    fitness_score[0] = ub;


    for(int i = 1; i < POP_SIZE; i++){
        int fit = eval_fnt(pop[i]);
        fitness_score[i] = fit;
        if(fit > ub){
            best_sol[0] = fit;
            ub = fit;
        }
        else if(fit < lb)
            lb = fit;
    }

    if(ub == lb){
        printf("Solução encontrada: %d\n", ub);
        exit(1);
    } //menor e maior soluções são iguais (convergiu)

    printf("[%d, %d]\n", lb, ub);
    //normalização 
    int range = ub - lb;
    for(int i = 0; i < POP_SIZE; i++){
        fitness_score[i] = (((fitness_score[i] - lb)*100)/(range));
    }   
}

int main (int argc, char** argv){

    //1. incialização da populção usar matriz mesmo?? alternativas -> vetorzão (melhor pra cahce)
    // -> (qual outra alternativa ??) quando tivermos +1 gene no indivíduo 
    //(vetor de struct parece ser pior) uns cara de stanford usaram matriz ent fodase
    srand(time(0));
    individual pop[POP_SIZE];
    for(int i = 0; i < POP_SIZE; i++)
        for(int j = 0; j < GENE_NUM; j++)
            pop[i][j] = rand()%100000;
    
    //2. fitness
    eval_ptr eval_fnt = &hillFnt;
    for(int i = 0; i < POP_SIZE; i++){
        printf("ind: %d fnt: %d\n", pop[i][0], eval_fnt(pop[i]));
    }
    fitness_ptr fitness_fnt = &fitnessFnt;
    fitness_fnt(eval_fnt, pop);

    printArr(fitness_score, POP_SIZE);

    return 0;
}