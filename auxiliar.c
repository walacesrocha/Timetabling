
#include "auxiliar.h"

/*Matriz Aula-Sala  ____________________________________________________________________*/

void inicializaMatrizAulaSala (Problema *p){
    int i, j;
    
    for (i=0; i<= p->nAulas; i++){
        for (j = 0; j< p->nSalas; j++){
            p->matrizAulaSala[i][j] = 0;
        }
    }
}

void alocaMatrizAulaSala (Problema *p){
    int i;    
    p->matrizAulaSala = (char**) malloc ((p->nAulas + 1) * sizeof(char*));
    
    for (i=0; i<= p->nAulas; i++){
        p->matrizAulaSala[i] = (char*) malloc (p->nSalas * sizeof(char));
    }
    inicializaMatrizAulaSala (p);    
}

void imprimeMatrizAulaSala (Problema *p){
    int i, j;
    
    printf ("naulas: %d, nsalas: %d\n", p->nAulas, p->nSalas);////////
    for (i=0; i<= p->nAulas; i++){
        printf ("%d - %s     ", i, getDisciplina(p,i));
        for (j = 0; j< p->nSalas; j++){
            printf("%d ", p->matrizAulaSala[i][j]);            
        }
        printf ("\n");
    }
}

void preencheMatrizAulaSala (Problema *p){
    int i, j;
    Disciplina *disp;
    
    alocaMatrizAulaSala(p);
    
    for (i=1; i<= p->nAulas; i++){
        disp = acessaDisciplina (p, i);
        for (j = 0; j< p->nSalas; j++){
            if (        disp->nAlunos <= (p->salas[j]).capacidade       ){
                p->matrizAulaSala[i][j] = 1;
            }
        }
    }
}

/*nao verifica se a aula eh aula valida*/
int salaDisponivel (Problema *p, int aula, int sala){
    return p->matrizAulaAula[aula][sala];
}

/*Matriz Aula-Aula  ____________________________________________________________________*/

void inicializaMatrizAulaAula (Problema *p){
    int i, j;
    
    for (i=0; i<= p->nAulas; i++){
        for (j = 0; j<= p->nAulas; j++){
            p->matrizAulaAula[i][j] = 0;
        }
    }
}

void alocaMatrizAulaAula (Problema *p){
    int i;    
    p->matrizAulaAula = (char**) malloc ((p->nAulas + 1) * sizeof(char*));
    
    for (i=0; i<= p->nAulas; i++){
        p->matrizAulaAula[i] = (char*) malloc ((p->nAulas + 1) * sizeof(char));
    }
    inicializaMatrizAulaAula (p);    
}


void imprimeMatrizAulaAula (Problema *p){
    int i, j;
    
    printf ("naulas: %d\n", p->nAulas);////////
    for (i=0; i<= p->nAulas; i++){
        printf ("%d - %s     ", i, getDisciplina(p,i));
        for (j = 0; j<= p->nAulas; j++){
            printf("%d ", p->matrizAulaAula[i][j]);            
        }
        printf ("\n");
    }
}


/* 0 = aulas de curriculos diferentes
 * 1 = aulas do mesmo curriculo ou mesmo professor
 * 2 = aulas da mesma disciplina       
 obs: linha 0 e coluna 0 nao sao usadas, pois aulas comecam com 1*/
void preencheMatrizAulaAula (Problema *p){
    int i, j;
        
    alocaMatrizAulaAula(p);
    
    for (i=1; i<= p->nAulas; i++){    
        for (j = 1; j<= p->nAulas; j++){
            if (        aulasMesmoCurriculo2(p, i, j) || aulasMesmoProfessor(p, i,j)     ){
                p->matrizAulaAula[i][j] = 1;
                if (aulasMesmaDisciplina(p,i,j) ){
                    p->matrizAulaAula[i][j] = 2;
                }
            }
        }
    }
}

int aulasConflitantes (Problema *p, int a1, int a2){
    return (p->matrizAulaAula[a1][a2]) == 1;
}

int aulasMesmaDisciplina3 (Problema *p, int a1, int a2){
    if (a1 > (p->nAulas) || a2 > (p->nAulas)){      //se a1 ou a2 nao forem aula
        return 0;
    }
    return (p->matrizAulaAula[a1][a2]) == 2;
}

