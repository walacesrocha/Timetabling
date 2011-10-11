#include "fitness.h"





//periodo aqui significa horario (ex: 14-15 horas)



float somaViolacoesHardTroca (Problema *p, Individuo *a, int pos1, int pos2){        
    return somaViolacoesHardLocal (p, a, pos1) + somaViolacoesHardLocal (p, a, pos2);
}


//Otimizar: forma de varrer a violacao hard 1
float somaViolacoesHard(Problema *p, Individuo *a) {
    int i;
    float violacoes1,  violacoes3, violacoes4, result;
    violacoes1 = 0.0;
    violacoes3 = 0.0;
    violacoes4 = 0.0;
    //printf("v1: %f, v3: %f, v4: %f\n", violacoes1, violacoes3, violacoes4);/////
    //percorre primeira sala para verificar restricoes hard 1 e 3
    for (i = 0; i < ((p->nDias) * (p->nPerDias)); i++) {
        violacoes4 += violacoesHard4(p, a, i);
        violacoes3 += violacoesHard3(p, a, i);
	violacoes1 += violacoesHard1(p, a, i);
    }
    
     //while (i < (a->n - (p->nDias * p->nPerDias)) ) {
       // violacoes4 += violacoesHard4(p, a, i);
       // i++;
    //}
    
    //percorre restante da tabela verificando restricoes hard 4
    while (i < a->n) {
        violacoes1+= violacoesHard1(p, a, i);
        violacoes4 += violacoesHard4(p, a, i);
        i++;
    }
    //printf ("Violations of lectures: %d\n", violacoes1);
    //printf ("Violations of conflicts: %d\n", violacoes3);
    //printf ("Violations of availability: %d\n", violacoes4);

    //printf ("soma violacoes hard: %d\n", violacoes1+ violacoes3+ violacoes4);//////
           
    return (violacoes1 + violacoes3 + violacoes4);
    
    
}

int estudantesExcedentesAula(Problema *p, int aula, int vagasSala) {
    int n;
    Disciplina *d = acessaDisciplina(p, aula);

    n = d->nAlunos - vagasSala;
    if (n < 0) return 0;
    return n;
}

Agenda **preparaVetorAgendas(Problema *p) {
    int i;
    Agenda **a;

    a = (Agenda**) malloc(p->nDisciplinas * sizeof (Agenda*));
    for (i = 0; i < p->nDisciplinas; i++) {
        a[i] = criaAgenda(p, &(p->disciplinas[i]));
        inicializaAgenda(p, a[i]);
    }
    return a;
}

void liberaVetorAgendas (Problema *p, Agenda **vetA){
    int i;
    for (i = 0; i < p->nDisciplinas; i++) {
        liberaAgenda (p, vetA[i]);       
    }
    free (vetA);
}

void imprimeVetorAgendas(Problema *p, Agenda **vetA) {
    int i;
    for (i = 0; i < p->nDisciplinas; i++) {
        imprimeAgenda(p, vetA[i]);
        printf("\n");
    }
}

int setVetorAgenda(Problema *p, Agenda **vetA, int aula, int sala, int dia, int horario) {
    int i, salaDiferente = 0;

    Disciplina *d = acessaDisciplina(p, aula);
    for (i = 0; i < p->nDisciplinas; i++) {
        if ((vetA[i])->disp == d) {
            salaDiferente = setAgenda(vetA[i], aula, sala, dia, horario);

        }
    }
    return salaDiferente;
}

int somaViolacoesMinDias(Problema *p, Agenda **vetA) {
    int i, soma;
    soma = 0;
    for (i = 0; i < p->nDisciplinas; i++) {
        soma += nRestricoesMinDias(p, vetA[i]);
    }
    return soma;
}



int curriculoIsoladoLado (Problema *p, Individuo *a, Curriculo *c, int dia, int horario){
    int sala, i, j, aula;
    Disciplina *d;

    if (horario >= 0 && horario < (p->nPerDias) ){
	for (sala = 0; sala < p->nSalas; sala++){
	    i = salaDiaHorarioParaPos (p, sala, dia, horario);
	    aula = a->aula[i];
	    if (	ehAula (p, aula)	){
		d = acessaDisciplina (p, aula);
		for (j=0; j< d->nCurriculos; j++){
		    if (	strcmp(d->curriculos[j]->nomeCurriculo, c->nomeCurriculo) ==0	){
			return 0;
			//printf ("Nao isolado: c1: %s, c2: %s, sala: %d, dia: %d, horario: %d\n", d->curriculos[j]->nomeCurriculo,c->nomeCurriculo, sala, dia, horario);//////////
		    }
		}
	    }
	}
    }
    return 1;
}


int disciplinaNaoVerificada (Problema *p, Individuo *a, int pos, Disciplina *d){
    int i, decremento = p->nDias * p->nPerDias;
    //printf ("verifica repeticao disciplina : %s\n", d->nomeDisciplina);//////////
    for (i = pos - decremento; i>= 0; i -=decremento){
	//printf ("	i: %d\n", i);//////////
	if (	acessaDisciplina(p, a->aula[i]) == d	){
	    //printf ("disciplina repetida: %s\n", d->nomeDisciplina);//////////
	    return 0;
	}
    }
    return 1;
}

int aulaIsolada (Problema *p, Individuo *a, int pos, int dia, int horario){
   int j;
   Disciplina *d;
   Curriculo *c;
   int nCurriculosIsolados;

   nCurriculosIsolados = 0;
   d = acessaDisciplina (p, a->aula[pos]);
   //printf("Isolamento, checa disp: %s\n", d->nomeDisciplina);///////
   if (disciplinaNaoVerificada (p, a, pos, d)){
       for (j=0; j< d->nCurriculos; j++){
	   c = d->curriculos[j];
	   //printf("		curriculo: %s\n", c->nomeCurriculo);///////
	   if (	curriculoIsoladoLado(p, a, c, dia, horario -1) && curriculoIsoladoLado(p, a, c, dia, horario +1)	){
	       nCurriculosIsolados = nCurriculosIsolados + 1;
	      // printf("		curriculo %s disp: %s. isolada dia: %d, horario: %d\n", c->nomeCurriculo, d->nomeDisciplina, dia, horario);///////
	   }
	}
    }
    return (2 * nCurriculosIsolados);

}


float somaViolacoesSoft(Problema *p, Individuo *a) {
    int sala, dia, horario, i, nAulasSala, aula, pos;
    int soma1, soma2, soma3, soma4;
    Agenda **vetAgenda;

    vetAgenda = preparaVetorAgendas(p);

    i = 0;
    soma1 = soma2 = soma3 = soma4 = 0;
    for (sala = 0; sala < p->nSalas; sala++) {
        pos = 0;
        for (dia = 0; dia < p->nDias; dia++) {
            for (horario = 0; horario < p->nPerDias; horario++) {
                aula = a->aula[i];
                if (ehAula(p, aula)) {
                    soma4 += setVetorAgenda(p, vetAgenda, aula, sala, dia, horario); //restricoes soft 4
		    //printf ("sala: %d, dia: %d, horario: %d, soma Instabilidade sala = %d\n", sala, dia, horario, soma4);//////
                    soma1 += estudantesExcedentesAula(p, aula, (p->salas[sala]).capacidade); //restricoes soft 1
                    soma3 += aulaIsolada (p, a, i, dia, horario); //restricuoes soft 3
                }
                i++;
            }
        }

    }

    soma2 += somaViolacoesMinDias(p, vetAgenda); //restricoes soft 2
    //printf ("Cost of RoomCapacity (soft) : %d\n", soma1);
    //printf ("Cost of MinWorkingDays (soft) : %d\n", soma2);
    //printf ("Cost of Isolated Lectures (soft) : %d\n", soma3);
    //printf ("Cost of Room Stability(soft) : %d\n", soma4);
    liberaVetorAgendas (p, vetAgenda);

    return (float)(soma1 + soma2 + soma3 + soma4);
}



float fitnessHard(Problema *p, Individuo *i) {
    //float divisor = somaViolacoesHard(p, i);
    //printf ("divisor apos somaviolacoes: %s\n", divisor);////
    //divisor =  divisor-1;
    //printf ("divisor apos multiplicacao: %f, ",divisor);////
    //divisor+=1;
    //if(divisor== 54) printf("divisor: %f, operacao: %f\n", divisor, divisor+1);//////
    //printf ("divisor apos soma 1: %f\n",divisor);////
    //printf ("somaViolacoes: %f, divisor: %f , divisao: %f.      ", somaViolacoesHard(p, i),divisor, 1000.0/divisor );////
    //printf ("func423 fitness hard: %f\n", 1000/ (0.999*somaViolacoesHard(p, i) + 1));///////
    return 1000 / (1.0*somaViolacoesHard(p, i) + 1);
}

float fitness(Problema *p, Individuo *i) {
    //printf ("func fitness : %f\n", 1000 / (0.999*somaViolacoesHard(p, i) + 0.001*somaViolacoesSoft(p, i) + 1));///////
    return 1000 / (0.999*somaViolacoesHard(p, i) + 0.001*somaViolacoesSoft(p, i) + 1);
}

