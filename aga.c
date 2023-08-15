#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#include <string.h>

#define maxx 1000
#define TamPop 10
float TaxMut = 2;   // 10 = 10%

double ind[TamPop + 1]; //cromossomo dos individuos
double indtemp[TamPop + 1]; //vetor para armazenar os individuos temporariamente para o torneio de 2
double fit[TamPop + 1];
//double bestfit[]
double maxfit = 0.0;
double media = 0.0;
int    i, maxi = 0;
int gen = 0;

void Draw(void) {

    //background colour
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
 
    glClear(GL_COLOR_BUFFER_BIT);
 
 	glPointSize(10.0f); //point size
    glLineWidth(5.0f); //line width


  	glPointSize(3.0f); //point size
    ////draw Linhas dos graficos

    //glBegin(GL_LINE_STRIP); //draw lines in sequence
    //    glColor3f(0.5f, 0.5f, 0.5f); //define gray
    //    glVertex2f(0, 500);
    //    glVertex2f(1000, 500);	
    //glEnd();
 
    //draw Funcao
    glBegin(GL_POINTS);
   		glColor3f(0.4f, 0.4f, 0.0f); //define yellow

		for (int x=0;x<1000;x++)
	    	{
				//------- Função doida -------//
				float y = (2*cos(0.039*x) + 5*sin(0.05*x) + 0.5*cos(0.01*x) + 10*sin(0.07*x) + 5*sin(0.1*x) + 5*sin(0.035*x))*10+500;
				//------ Montanha -------//
				//float y = x; 
				//if(x>500)
				//	y=1000-x;
				glVertex2f((float) x, y);	
	    	}
    glEnd();

	// Print generation to screen
    int mutation = TaxMut;
	char genQty[50]={'\0'};
	sprintf(genQty, "Generation: %d   TaxMut: %d", gen, mutation);// Text
	glColor3f(0.0,1.0,0.0);// Text color
	glRasterPos2f(0,980);// Print position
	// Print each char
	for(int j=0;j<30;j++){
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, genQty[j]);
	}

	// Print best ind 
	char bestInd[30]={'\0'};
	sprintf(bestInd, "Best fitness: %f", fit[maxi]);// Text
	glColor3f(0.0,1.0,0.0);// Text color
	glRasterPos2f(0,960);// Print position
	// Print each char
	for(int j=0;j<30;j++){
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, bestInd[j]);
	}

	// Print help
	char help[60]={'\0'};
	sprintf(help, "Enter->Next / r->Restart / .->TaxMut*2 / ,->TaxMut/2");// Text
	glColor3f(0.0,1.0,0.0);// Text color
	glRasterPos2f(0,940);// Print position
	// Print each char
	for(int j=0;j<60;j++){
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, help[j]);
	}

 	glPointSize(4.0f); //point size
    glBegin(GL_POINTS);
   	glColor3f(0.0f, 1.0f, 0.0f); //define red
    for (i=1;i<=TamPop;i++)
    {                         
        glVertex2f(ind[i], fit[i]);	

    }
    glEnd();

    // Printa Melhor de Todos em Vermelho
 	glPointSize(10.0f); //point size
    glBegin(GL_POINTS);
    	glColor3f(1.0f, 0.0f, 0.0f); //define red
    	glVertex2f(ind[maxi], fit[maxi]); //draw goal
    glEnd();

    //send data to draw
    glFlush();
}

void avalia()
{
    float x;
	printf("Generation %d\n",gen);
    for (i=1;i<=TamPop;i++)
    {
        x=ind[i];
		//------- Função doida -------//
		float y = (2*cos(0.039*x) + 5*sin(0.05*x) + 0.5*cos(0.01*x) + 10*sin(0.07*x) + 5*sin(0.1*x) + 5*sin(0.035*x))*10+500;
		//------ Montanha -------//
		//float y = x; 
		//if(x>500)
		//	y=1000-x;
	    fit[i] = y;
		printf("\tFitness %d (%f)= %f\n",i,ind[i],fit[i]);
    }
}

void initpop()
{
    for (i=1;i<=TamPop;i++)
    {                         // nr entre -10 e +10 -> rand() %20 - 10;
        ind[i] = (double) (rand() % maxx);
    }
}


void elitismo() // Melhor transa com todos
{
    maxfit = fit[1];
    maxi   = 1;

    for (i=2;i<=TamPop;i++)  // Busca pelo melhor individuo
    {
        if (fit[i]>maxfit)
        {
            maxfit = fit[i];
            maxi = i;
        }
    }

    for (i=1;i<=TamPop;i++)
    {
        if (i==maxi)        // Protege o melhor individuo
            continue;

        // Crossover
        ind[i] = (ind[i] + ind[maxi])/ 2.0;

        // Mutacao                    | nr = 0-40 |    - 20    |  0,02  |  10%
        ind[i] = ind[i] + ((double) (rand()%maxx)-maxx/2)*TaxMut/100.0f;
		if(ind[i]>maxx)
			ind[i]=maxx;
		if(ind[i]<0)
			ind[i]=ind[i]*-1.0;
    }
}

void torneio()  // Torneio de 2
{
    int a, b, pai1, pai2;
    maxfit = fit[1];
    maxi   = 1;
    for (i=2;i<=TamPop;i++)    // Busca pelo melhor individuo para protege-lo
    {
        if (fit[i]>maxfit)
        {
            maxfit = fit[i];
            maxi = i;
        }
    }

    for (i=1;i<=TamPop;i++)
        indtemp[i] = ind[i];  // Backup dos individuos
    
    // Torneio
    for (i=1;i<=TamPop;i++)
    {
        if (i==maxi)    // Protege o melhor individuo
            continue;

        // Sorteia dois individuos para 1ro torneio
        a = (rand() %TamPop) + 1;
        b = (rand() %TamPop) + 1;
        if (fit[a] > fit[b])
            pai1 = a;
        else
            pai1 = b;

        // Sorteia mais dois individuos para 2do torneio
        a = (rand() %TamPop) + 1;
        b = (rand() %TamPop) + 1;
        if (fit[a] > fit[b])
            pai2 = a;
        else
            pai2 = b;

        // Crossover
        ind[i] = (indtemp[pai1] + indtemp[pai2])/ 2.0;  // Pegar os pais antigos, pois posso sortear os novos filhos que foram alterados

        // Mutacao                    | nr = 0-40 |    - 20    |  0,02  |  10%
        ind[i] = ind[i] + (double) (((rand() %maxx - (maxx/2.0))/100.0) * TaxMut);
    }
}



//just explain the commands
void ag(){
    
    elitismo();    // Seleciona o melhor individuo para transar com todos e Faz Crossover e Mutacao e ja' substitui na Populacao
    avalia();       // Avalia os individuos da Populacao

    //torneio();    // FAz torneio de 2 entre os individuos, e Faz Crossover e Mutacao e ja' substitui na Populacao

    // Final do Algoritmo Evolutivo
	gen++;
}

void Keyboard(unsigned char key, int x, int y)
{

//	if(gen%50==0){
//		TaxMut = TaxMut==60 ? 2 : 60;
//	}

    //ENTER
    if (key == 13){
	 	ag();
	 	glutPostRedisplay();
    }
//.
    if (key == 46){
	 	TaxMut=TaxMut*2;
	 	glutPostRedisplay();
    }
//,
    if (key == 44){
	 	TaxMut=TaxMut/2;
	 	glutPostRedisplay();
    }
    // r
	if(key == 114){
		gen = 0;
		TaxMut = 2;
		i = 0;
		maxi = 0;
		media = 0;
		maxfit = 0;
		initpop();
	 	glutPostRedisplay();
	}
}


int main(int argc, char *argv[]){

	srand(time(NULL));

	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
   // glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - 800) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - 800) / 2);
    glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH), glutGet(GLUT_SCREEN_HEIGHT));
    glutCreateWindow("Evolutive Systems - Alice");

    initpop();

    glutDisplayFunc(Draw);

    glutKeyboardFunc(Keyboard);
 
    gluOrtho2D(0, 1000, 0, 1000);

	glutMainLoop();

	return 0;
}

