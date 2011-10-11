#include "agenda.h"


void inicializaAgenda (Problema *p, Agenda *a){
    int i, j;
    for (i=0; i< p->nDias; i++){
	for (j=0; j< p->nPerDias; j++){
	    a->deh[i][j] = HORARIOVAZIO;

	}
    }
}

Agenda *criaAgenda (Problema *p, Disciplina *d){
    int i;
    Agenda *a;

    a = (Agenda*) malloc (sizeof (Agenda));
    a->disp = d;
    a->deh = (int**) malloc ((p->nDias) * sizeof(int*));
    for (i=0; i< p->nDias; i++){
	a->deh[i] =  (int*) malloc((p->nPerDias) * sizeof (int));
    }

    a->salas = (int*) malloc(d->nAulas * sizeof (int));
    a->nSalas =0;
    return a;

}

void liberaAgenda (Problema *p, Agenda *a){
    int i;
    for (i=0; i< p->nDias; i++){
        free (a->deh[i]);
    }
    free(a->deh);
    free(a->salas);
    free(a);
}


int insereSala (Agenda *a, int sala){
    int i;

    for (i=0; i< a->nSalas; i++){
	if (sala == a->salas[i]){
	    return 0;
	}
    }
    a->salas[i] = sala;
    (a->nSalas)++;

    if (i)  return 1;	//se eh a primeira sala colocada, retorna 0

    return 0;
}

int setAgenda(Agenda *a, int aula, int sala, int dia, int horario){
    if (a->deh[dia][horario] == HORARIOVAZIO){
	a->deh[dia][horario] = aula;
	return insereSala (a, sala);
    }else{
	//duas aulas no mesmo horario e em salas diferentes
	return 0;
    }
}



int quantidadeDiasAula (Problema *p, Agenda *a){
    int i, j, somaHorarios, somaDias;
    somaDias =0;
    for (i=0; i< p->nDias; i++){
	somaHorarios = 0;
	for (j=0; j< p->nPerDias; j++){
	    if (a->deh[i][j] != HORARIOVAZIO){
		somaHorarios++;
	    }
	}
	if (somaHorarios){
	    somaDias++;
	}
    }
    return somaDias;
}

int nRestricoesMinDias (Problema *p, Agenda *a){
    int diasAmenos =    (a->disp->minDiasAula) - quantidadeDiasAula(p,a);
    if (diasAmenos > 0){
	return 5* diasAmenos;
    }
    return 0;
}


void imprimeAgenda (Problema *p, Agenda *a){
    int i, j;
    char *nomeDisp;
    printf ("Disp: %s\n", a->disp->nomeDisciplina);
    printf ("salas (%d):\n", a->nSalas);
    for (i=0; i< a->nSalas; i++){
	printf ("s%d ", a->salas[i]);
    }
    printf ("\n");

    for (i=0; i< p->nDias; i++){
	printf ("Dia %d\n", i);
	for (j=0; j< p->nPerDias; j++){
	    printf ("h%d:%d ",j, a->deh[i][j]);
	}
	printf ("\n\n");
    }

}
