
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

void liberaMatrizAulaSala (Problema *p){
    int i;    
       
    for (i=0; i<= p->nAulas; i++){
        free (p->matrizAulaSala[i]);
    }
    free (p->matrizAulaSala);
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

void liberaMatrizAulaAula (Problema *p){
    int i;    
        
    for (i=0; i<= p->nAulas; i++){
        free (p->matrizAulaAula[i]);
    }
    
    free(p->matrizAulaAula);
}

/*Matriz Aula-Indisponibilidades  ____________________________________________________________________*/
//linhas representam aulas
//colunas repersentam dias e horarios
//valor 1 = indisponível. Valor 0 = disponivel


void inicializaMatrizAulaIndisponibilidades (Problema *p){
    int i, j;
    
    for (i=0; i<= p->nAulas; i++){
        for (j = 0; j< p->nDiasEperiodos; j++){
            p->matrizAulaIndisponibilidades[i][j] = 0;
        }
    }
}

void alocaMatrizAulaIndisponibilidades (Problema *p){
    int i;    
    p->matrizAulaIndisponibilidades = (char**) malloc ((p->nAulas + 1) * sizeof(char*));
    
    for (i=0; i<= p->nAulas; i++){
        p->matrizAulaIndisponibilidades[i] = (char*) malloc ((p->nDias * p->nPerDias) * sizeof(char));
    }
    inicializaMatrizAulaIndisponibilidades (p);    
}


void preencheMatrizAulaIndisponibilidades (Problema *p){
    int i, j, aula, diaEperiodo;    
    Restricao *r;
    Disciplina *d;

    alocaMatrizAulaIndisponibilidades (p);
   
    
    for (i = 0; i < p->nDisciplinas; i++) {
        d = &(p->disciplinas[i]);        
        for (j = 0; j < p->nRestricoes; j++) {
            r = &(p->restricoes[j]);
            if (strcmp(d->nomeDisciplina, r->nomeDisciplina) == 0) {
                for (aula = d->aulaInicial; aula < (d->aulaInicial) + (d->nAulas); aula++){		    
		    diaEperiodo = (	(r->dia) * p->nPerDias	) + (r->periodo);
		    //printf ("Disp: %s, aula: %d, dia e periodo: %d\n", d->nomeDisciplina, aula, diaEperiodo);///////
		    p->matrizAulaIndisponibilidades[aula][diaEperiodo] = 1;
		}
            }
        }        
        
    }
 
        
}

int horarioIndisponivel(Problema *p, int aula, int pos) {    
    return (p->matrizAulaIndisponibilidades[aula][getTimeSlotFromPos(pos, p)] );
}

void imprimeMatrizAulaIndisponibilidades (Problema *p){
    int i,j;
    
    for (i=0; i<= p->nAulas; i++){
	printf ("%s, aula: %d    ", getDisciplina (p, i), i);
        for (j = 0; j< p->nDiasEperiodos; j++){
            printf ("%d ", p->matrizAulaIndisponibilidades[i][j]);
        }
        printf ("\n");
    }
}

void liberaMatrizAulaIndisponibilidades (Problema *p){
    int i;    
        
    for (i=0; i<= p->nAulas; i++){
        free (p->matrizAulaIndisponibilidades[i]);
    }
    free (p->matrizAulaIndisponibilidades);
}

/*Slots Disponiveis  ____________________________________________________________________________________________________ */
//vetor de posicoes disponiveis da tabela horario para uma dada disciplina

void inicializaSlotsDisponiveis (Problema *p, Disciplina *d){
    int i;
    for (i=0; i< p->dimensao; i++){
	d->slotsDisponiveis[i] = -1;
    }
    d->nSlotsDisponiveis = 0;
}

void alocaSlotsDisponiveis (Problema *p, Disciplina *d){
    d->slotsDisponiveis = (int*) malloc (p->dimensao * sizeof(int));
    inicializaSlotsDisponiveis (p, d);
}

void preencheSlotsDisponiveis (Problema *p, Disciplina *d){
    int i;
    
    alocaSlotsDisponiveis (p, d);
    
    d->nSlotsDisponiveis = 0;
    for (i = 0; i < p->dimensao; i++){
	if (	!horarioIndisponivel(p, d->aulaInicial, i)	){
	    d->slotsDisponiveis[d->nSlotsDisponiveis] = i;
	    d->nSlotsDisponiveis = (d->nSlotsDisponiveis) + 1;
	    
	}
    }
    
}

void preencheSlotsDisponiveisTodasDisciplinas (Problema *p){
    int i;
    Disciplina *d;
    for (i = 0; i < p->nDisciplinas; i++) {
	d = &(p->disciplinas[i]);
	preencheSlotsDisponiveis (p, d);
    }
    
}

void imprimeSlotsDisponiveis (Problema *p, Disciplina *d){
    int i;
    
    for (i = 0; i< (d->nSlotsDisponiveis); i++){
	printf ("%d ", d->slotsDisponiveis[i]);
    }
    printf ("\n%d slots disponiveis\n", d->nSlotsDisponiveis);
    
}

void imprimeSlotsDisponiveisTodasDisciplinas (Problema *p){
    int i;
    Disciplina *d;
    for (i = 0; i < p->nDisciplinas; i++) {
	d = &(p->disciplinas[i]);
	printf ("%s   ", d->nomeDisciplina);
	imprimeSlotsDisponiveis (p, d);
	printf ("\n");
    }
}

void liberaSlotsDisponiveis (Problema *p, Disciplina *d){
    free (d->slotsDisponiveis);    
}

void liberaSlotsDisponiveisTodasDisciplinas (Problema *p){
    int i;
    Disciplina *d;
    for (i = 0; i < p->nDisciplinas; i++) {
	d = &(p->disciplinas[i]);
	liberaSlotsDisponiveis (p, d);
    }
    
}

