#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include "ga.h"

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

gene_t hill_fnt(individual ind){
    gene_t val = ind[0];
    return (val > 10 ? 20 - val : val);
}

//obs: para usar a parabóla verifique se MAXX^2 <= RAND_MAX -> (MAXX << 2^16) 
gene_t parabola(individual ind){
    gene_t val = ind[0];
    gene_t y = -val*val + 12*val + 8;
    return y;
}

gene_t senoid(individual ind){

    gene_t x = ind[0];
    gene_t y = (2*cos(0.039*x) + 5*sin(0.05*x) + 0.5*cos(0.01*x) 
        + 10*sin(0.07*x) + 5*sin(0.1*x) + 5*sin(0.035*x))*10+500;

    return y;
}


int main(int argc, char** argv){

    gene_t* best_s = interface("t", &hill_fnt);

    printf("best sol: %.2f\n", best_s[0]);
    
    free(best_s);
    
    return 0;
}