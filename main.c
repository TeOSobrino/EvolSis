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
typedef int (*fitness_ptr) (eval_ptr eval_fnt, int[GENE_NUM]); //ponteiro para a função de fitness

#define MUT_RATE 0.1 //taxa de mutação base = 10%

int hillFnt(individual ind){

    int val = ind[0]; //tem q abstrair esse treco
    return (val > 500 ? 1000 - val : val); //haha ternário
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

    return 0;
}