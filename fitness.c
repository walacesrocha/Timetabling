#include <string.h>

#include "fitness.h"





//periodo aqui significa horario (ex: 14-15 horas)

//pos nao pode pertencer a ultima sala
//posicoes da tabela onde ocorre repeticao ficam temporariamente com valor igual ao da aula + dimensao+10
// o valor 10 foi adicionado para tambem funcionar com encoding de aulas comecando em 0 em vez de 1
//deve-se restaurar as aulas apos as verificacoes 
//nColunas eh na realidade um incremento para saltar para a posicao abaixo da posicao atual

int eliminaAulasRepetidasAbaixo(Problema *p, Individuo *ind, int a1, int pos) {
    int i, a2, nColunas, nEliminacoes;

    nEliminacoes = 0;
    nColunas = ((p->nPerDias) * (p->nDias));

    for (i = pos + nColunas; i < (p->dimensao); i = i + nColunas) {
        //printf ("eliminando aulas abaixo -------- pos: %d, pos2: %d\n", pos, i); imprimePosicoesTabela(p, ind); ///////////////
        a2 = ind->aula[i];

        //a1 eh verificado se eh aula antes de chamar esta funcao (eliminaAulasRepetidasAbaixo)
        if (ehAula(p, a2)) {
            if (aulasMesmaDisciplina(p, a1, a2)) {
                ind->aula[i] = (p->dimensao) + a2 + 10;
                nEliminacoes = nEliminacoes + 1;
            }
        }

    }

    return nEliminacoes;
}

void restauraAulasMultiplas(Problema *p, Individuo *ind) {
    int i;
    for (i = 0; i < (p->dimensao); i++) {
        if ((ind->aula[i]) > (p->dimensao)) {
            ind->aula[i] = (ind->aula[i]) - (p->dimensao) - 10;
        }
    }
}

int eliminaAulasMultiplas(Problema *p, Individuo *ind) {
    int i, posf, a1, nEliminacoes;

    nEliminacoes = 0;
    posf = (p->dimensao) - ((p->nDias) * (p->nPerDias));
    //printf ("dimensao: %d, posf: %d\n", p->dimensao, posf); imprimePosicoesTabela(p, ind); exit(1);///////////////
    for (i = 0; i < posf; i++) {
        a1 = ind->aula[i];
        if (ehAula(p, a1)) {
            nEliminacoes = nEliminacoes + eliminaAulasRepetidasAbaixo(p, ind, a1, i);
        }
    }

    //printf ("eliminacoes (violacoes hard1: %d\n",nEliminacoes );/////////
    return nEliminacoes;
}

int eliminaAulasMultiplasColuna(Problema *p, Individuo *ind, int ini) {
    int i, posf, a1, nEliminacoes;
    int incremento = p->nPerDias * p->nDias;

    nEliminacoes = 0;
    posf = (p->dimensao) - ((p->nDias) * (p->nPerDias));


    for (i = ini; i < posf; i += incremento) {
        a1 = ind->aula[i];
        if (ehAula(p, a1)) {
            nEliminacoes = nEliminacoes + eliminaAulasRepetidasAbaixo(p, ind, a1, i);
        }
    }

    //printf ("eliminacoes (violacoes hard1: %d\n",nEliminacoes );/////////
    //printf("Depois eliminaAulasMultiplasLocal\n");imprimeIndividuo3 (p, ind);   printf("\n");imprimeIndividuo2 (p, ind);
    return nEliminacoes;
}


//soh conta eliminacoes da aula local

int eliminaAulasRepetidasAbaixoLocal(Problema *p, Individuo *ind, int aula, int a1, int pos) {
    int i, a2, nColunas, nEliminacoes;

    nEliminacoes = 0;
    nColunas = ((p->nPerDias) * (p->nDias));

    for (i = pos + nColunas; i < (p->dimensao); i = i + nColunas) {
        //printf ("eliminando aulas abaixo -------- pos: %d, pos2: %d\n", pos, i); imprimePosicoesTabela(p, ind); ///////////////
        a2 = ind->aula[i];

        //a1 eh verificado se eh aula antes de chamar esta funcao (eliminaAulasRepetidasAbaixo)
        if (ehAula(p, a2)) {
            if (aulasMesmaDisciplina(p, a1, a2)) {
                ind->aula[i] = (p->dimensao) + a2 + 10;
                if (aulasMesmaDisciplina(p, aula, a2)) {
                    //soh conta eliminacoes da aula local
                    //soh conta 1 eliminacao
                    //nEliminacoes = nEliminacoes + 1;
                    nEliminacoes = 1;
                }
            }
        }

    }

    return nEliminacoes;
}

int eliminaAulasMultiplasLocal(Problema *p, Individuo *ind, int ini, int aula) {
    int i, a1, nEliminacoes;
    int incremento = p->nPerDias * p->nDias;

    nEliminacoes = 0;
    for (i = ini; i < (p->dimensao); i += incremento) {
        a1 = ind->aula[i];

        if (ehAula(p, a1)) {
            nEliminacoes = nEliminacoes + eliminaAulasRepetidasAbaixoLocal(p, ind, aula, a1, i);
        }
    }

    //printf ("eliminacoes (violacoes hard1: %d\n",nEliminacoes );/////////
    //printf("Depois eliminaAulasMultiplasLocal\n");imprimeIndividuo3 (p, ind);   printf("\n");imprimeIndividuo2 (p, ind);    
    return nEliminacoes;

}

/*
int eliminaAulasMultiplasLocalbkp (Problema *p, Individuo *ind, int ini, int aula) {        
    int i, a1, nEliminacoes;
    int incremento = p->nPerDias * p->nDias;
    int primeiraAula;        
    
    
    nEliminacoes = 0;          
    primeiraAula = 1;
    for (i= ini; i< (p->dimensao); i+= incremento){
        a1 = ind->aula[i];
	
        if (	ehAula(p, a1)	){	    
                if (aulasMesmaDisciplina(p, a1, aula)){
                    if  (primeiraAula){
                        //nao elimina a primeira da coluna, somente as de baixo inclusive a propria aula inserida
                        primeiraAula = 0;
                    }else{
                        ind->aula[i] = (p->dimensao) + a1 + 10;
                        nEliminacoes = nEliminacoes + 1;
                    }
                }
	    
	    
        }
    }    
    
    //printf ("eliminacoes (violacoes hard1: %d\n",nEliminacoes );/////////
    //printf("Depois eliminaAulasMultiplasLocal\n");imprimeIndividuo3 (p, ind);   printf("\n");imprimeIndividuo2 (p, ind);    
    return nEliminacoes;  
    
}
//*/


void restauraAulasMultiplasLocal(Problema *p, Individuo *ind, int ini) {
    int i;
    int incremento = p->nPerDias * p->nDias;

    for (i = ini; i < (p->dimensao); i += incremento) {
        if ((ind->aula[i]) > (p->dimensao)) {
            ind->aula[i] = (ind->aula[i]) - (p->dimensao) - 10;
        }
    }
}

float somaViolacoesHardTroca(Problema *p, Individuo *a, int pos1, int pos2) {
    return somaViolacoesHardLocal(p, a, pos1) + somaViolacoesHardLocal(p, a, pos2);
}

float somaConflitosTeste(Problema *p, Individuo *a) {
    int i;
    float violacoes1, violacoes3, violacoes4, result;


    violacoes1 = (float) eliminaAulasMultiplas(p, a);

    violacoes3 = 0.0;
    violacoes4 = 0.0;
    //printf("v1: %f, v3: %f, v4: %f\n", violacoes1, violacoes3, violacoes4);/////
    //percorre primeira sala para verificar restricoes hard 1 e 3
    for (i = 0; i < ((p->nDias) * (p->nPerDias)); i++) {
        violacoes4 += violacoesHard4(p, a, i);
        violacoes3 += violacoesHard3(p, a, i);
        //violacoes1 += violacoesHard1(p, a, i);
    }

    //while (i < (a->n - (p->nDias * p->nPerDias)) ) {
    // violacoes4 += violacoesHard4(p, a, i);
    // i++;
    //}

    //percorre restante da tabela verificando restricoes hard 4
    while (i < a->n) {
        //violacoes1+= violacoesHard1(p, a, i);
        violacoes4 += violacoesHard4(p, a, i);
        i++;
    }

    //printf ("Violations of lectures: %f\n", violacoes1); printf ("Violations of conflicts: %f\n", violacoes3); printf ("Violations of availability: %f\n", violacoes4);  //////

    restauraAulasMultiplas(p, a);

    return (violacoes3);

}

float somaIndisponibilidadesTeste(Problema *p, Individuo *a) {
    int i;
    float violacoes1, violacoes3, violacoes4, result;


    violacoes1 = (float) eliminaAulasMultiplas(p, a);

    violacoes3 = 0.0;
    violacoes4 = 0.0;
    //printf("v1: %f, v3: %f, v4: %f\n", violacoes1, violacoes3, violacoes4);/////
    //percorre primeira sala para verificar restricoes hard 1 e 3
    for (i = 0; i < ((p->nDias) * (p->nPerDias)); i++) {
        violacoes4 += violacoesHard4(p, a, i);
        violacoes3 += violacoesHard3(p, a, i);
        //violacoes1 += violacoesHard1(p, a, i);
    }

    //while (i < (a->n - (p->nDias * p->nPerDias)) ) {
    // violacoes4 += violacoesHard4(p, a, i);
    // i++;
    //}

    //percorre restante da tabela verificando restricoes hard 4
    while (i < a->n) {
        //violacoes1+= violacoesHard1(p, a, i);
        violacoes4 += violacoesHard4(p, a, i);
        i++;
    }

    //printf ("Violations of lectures: %f\n", violacoes1); printf ("Violations of conflicts: %f\n", violacoes3); printf ("Violations of availability: %f\n", violacoes4);  //////

    restauraAulasMultiplas(p, a);

    return (violacoes4);

}

//Funcao oficial
//Otimizar: forma de varrer a violacao hard 1

float somaViolacoesHard(Problema *p, Individuo *a) {
    int i;
    float violacoes1, violacoes3, violacoes4, result;


    violacoes1 = (float) eliminaAulasMultiplas(p, a);

    violacoes3 = 0.0;
    violacoes4 = 0.0;
    //printf("v1: %f, v3: %f, v4: %f\n", violacoes1, violacoes3, violacoes4);/////
    //percorre primeira sala para verificar restricoes hard 1 e 3
    for (i = 0; i < ((p->nDias) * (p->nPerDias)); i++) {
        violacoes4 += violacoesHard4(p, a, i);
        violacoes3 += violacoesHard3(p, a, i);
        //violacoes1 += violacoesHard1(p, a, i);
    }

    //while (i < (a->n - (p->nDias * p->nPerDias)) ) {
    // violacoes4 += violacoesHard4(p, a, i);
    // i++;
    //}

    //percorre restante da tabela verificando restricoes hard 4
    while (i < a->n) {
        //violacoes1+= violacoesHard1(p, a, i);
        violacoes4 += violacoesHard4(p, a, i);
        i++;
    }

    //printf ("Violations of lectures: %f\n", violacoes1); printf ("Violations of conflicts: %f\n", violacoes3); printf ("Violations of availability: %f\n", violacoes4);  //////

    restauraAulasMultiplas(p, a);

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

void liberaVetorAgendas(Problema *p, Agenda **vetA) {
    int i;
    for (i = 0; i < p->nDisciplinas; i++) {
        liberaAgenda(p, vetA[i]);
    }
    free(vetA);
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

int curriculoIsoladoLado(Problema *p, Individuo *a, Curriculo *c, int dia, int horario) {
    int sala, i, j, aula;
    Disciplina *d;

    if (horario >= 0 && horario < (p->nPerDias)) {
        for (sala = 0; sala < p->nSalas; sala++) {
            i = salaDiaHorarioParaPos(p, sala, dia, horario);
            aula = a->aula[i];
            if (ehAula(p, aula)) {
                d = acessaDisciplina(p, aula);
                for (j = 0; j < d->nCurriculos; j++) {
                    if (strcmp(d->curriculos[j]->nomeCurriculo, c->nomeCurriculo) == 0) {
                        return 0;
                        //printf ("Nao isolado: c1: %s, c2: %s, sala: %d, dia: %d, horario: %d\n", d->curriculos[j]->nomeCurriculo,c->nomeCurriculo, sala, dia, horario);//////////
                    }
                }
            }
        }
    }
    return 1;
}

int disciplinaNaoVerificada(Problema *p, Individuo *a, int pos, Disciplina *d) {
    int i, decremento = p->nDias * p->nPerDias;
    //printf ("verifica repeticao disciplina : %s\n", d->nomeDisciplina);//////////
    for (i = pos - decremento; i >= 0; i -= decremento) {
        //printf ("	i: %d\n", i);//////////
        if (acessaDisciplina(p, a->aula[i]) == d) {
            //printf ("disciplina repetida: %s\n", d->nomeDisciplina);//////////
            return 0;
        }
    }
    return 1;
}

int aulaIsolada(Problema *p, Individuo *a, int pos, int dia, int horario) {
    int j;
    Disciplina *d;
    Curriculo *c;
    int nCurriculosIsolados;

    nCurriculosIsolados = 0;
    d = acessaDisciplina(p, a->aula[pos]);
    //printf("Isolamento, checa disp: %s\n", d->nomeDisciplina);///////
    if (disciplinaNaoVerificada(p, a, pos, d)) {
        for (j = 0; j < d->nCurriculos; j++) {
            c = d->curriculos[j];
            //printf("		curriculo: %s\n", c->nomeCurriculo);///////
            if (curriculoIsoladoLado(p, a, c, dia, horario - 1) && curriculoIsoladoLado(p, a, c, dia, horario + 1)) {
                nCurriculosIsolados = nCurriculosIsolados + 1;
                // printf("		curriculo %s disp: %s. isolada dia: %d, horario: %d\n", c->nomeCurriculo, d->nomeDisciplina, dia, horario);///////
            }
        }
    }
    return (2 * nCurriculosIsolados);

}

//Funcao oficial

float somaViolacoesSoft(Problema *p, Individuo *a) {
    int sala, dia, horario, i, nAulasSala, aula, pos, lixo;
    int soma1, soma2, soma3, soma4;
    int s1, s2, s3, s4;
    Agenda **vetAgenda;

    lixo = eliminaAulasMultiplas(p, a);
    vetAgenda = preparaVetorAgendas(p);

    a->nConflitos = 0;

    i = 0;
    pos = 0;
    soma1 = soma2 = soma3 = soma4 = 0;
    for (sala = 0; sala < p->nSalas; sala++) {
        for (dia = 0; dia < p->nDias; dia++) {
            for (horario = 0; horario < p->nPerDias; horario++) {
                aula = a->aula[i];
                if (ehAula(p, aula)) {
                    pos++;
                    s4 = setVetorAgenda(p, vetAgenda, aula, sala, dia, horario); //restricoes soft 4
                    //printf ("sala: %d, dia: %d, horario: %d, soma Instabilidade sala = %d\n", sala, dia, horario, soma4);//////
                    s1 = estudantesExcedentesAula(p, aula, (p->salas[sala]).capacidade); //restricoes soft 1
                    s3 = aulaIsolada(p, a, i, dia, horario); //restricuoes soft 3

                    soma1 += s1;
                    soma3 += s3;
                    soma4 += s4;

                }
                i++;
            }
        }

    }

    printf("Soft1: Contados: %d/%d\n", pos, i);

    soma2 += somaViolacoesMinDias(p, vetAgenda); //restricoes soft 2
    //printf ("Cost of RoomCapacity (soft) : %d\n", soma1); printf ("Cost of MinWorkingDays (soft) : %d\n", soma2); printf ("Cost of Isolated Lectures (soft) : %d\n", soma3); printf ("Cost of Room Stability(soft) : %d\n", soma4);/////


    liberaVetorAgendas(p, vetAgenda);

    restauraAulasMultiplas(p, a);
    a->soft1 = soma1;
    a->soft2 = soma2;
    a->soft3 = soma3;
    a->soft4 = soma4;
    return (float) (soma1 + soma2 + soma3 + soma4);
}

int somaAulasIsoladas(Problema *p, Individuo *ind) {
    int c, dia, periodo;
    int nAulas;
    int soma=0;

    for (c = 0; c < p->nCurriculos; c++) {
        for (dia = 0; dia < p->nDias; dia++) {
            for (periodo = 0; periodo < p->nPerDias; periodo++) {

                nAulas = ind->currDiasPeriodos[c][dia][periodo];
                // se o curriculo tem aula no dia/horario
                if (ind->currDiasPeriodos[c][dia][periodo]) {
                    if (periodo == 0) {
                        if (!ind->currDiasPeriodos[c][dia][periodo + 1]) {
                            soma += nAulas;
                        }
                    } else if (periodo == p->nPerDias - 1) {
                        if (!ind->currDiasPeriodos[c][dia][periodo - 1]) {
                            soma += nAulas;
                        }
                    } else {
                        if (!ind->currDiasPeriodos[c][dia][periodo - 1] && !ind->currDiasPeriodos[c][dia][periodo + 1]) {
                            soma += nAulas;
                        }
                    }
                }
            }
        }
    }

    return soma * 2; // peso 2

}

float somaViolacoesSoft2(Problema *p, Individuo *a) {
    int sala, dia, horario, i, nAulasSala, aula, pos;
    int soma1, soma2, soma3, soma4;
    int totalSalas, totalDias;
    int s1, s2, s3, s4;
    int **salasUsadas;
    int **diasOcupados;

    salasUsadas = (int**) malloc(p->nDisciplinas * sizeof (int*));
    for (i = 0; i < p->nDisciplinas; i++) {
        salasUsadas[i] = (int*) malloc(p->nSalas * sizeof (int));
        memset(salasUsadas[i], 0, p->nSalas * sizeof (int));
    }

    diasOcupados = (int**) malloc(p->nDisciplinas * sizeof (int*));
    for (i = 0; i < p->nDisciplinas; i++) {
        diasOcupados[i] = (int*) malloc(p->nDias * sizeof (int));
        memset(diasOcupados[i], 0, p->nDias * sizeof (int));
    }



    i = 0;
    pos = 0;
    soma1 = soma2 = soma3 = soma4 = 0;
    for (sala = 0; sala < p->nSalas; sala++) {
        for (dia = 0; dia < p->nDias; dia++) {
            for (horario = 0; horario < p->nPerDias; horario++) {
                aula = a->aula[i];
                if (ehAula(p, aula)) {
                    pos++;
                    Disciplina *disc = acessaDisciplina(p, aula);
                    salasUsadas[disc->pVetor][sala]++;
                    diasOcupados[disc->pVetor][dia]++;

                    //printf ("sala: %d, dia: %d, horario: %d, soma Instabilidade sala = %d\n", sala, dia, horario, soma4);//////
                    //s1 = estudantesExcedentesAula(p, aula, (p->salas[sala]).capacidade); //restricoes soft 1
                    s1 = s3 = 0;
                    if (disc->nAlunos > (p->salas[sala]).capacidade) {
                        s1 = disc->nAlunos - (p->salas[sala]).capacidade;
                    }
                    //s3 = aulaIsolada(p, a, i, dia, horario); //restricuoes soft 3
                    if (!temAdjacencia(p, a, i, aula)) {
                        s3 = 2;
                    }

                    soma1 += s1;
                    soma3 += s3;
                    //soma4 += s4;
                }
                i++;
            }
        }

    }

    soma4 = 0;
    for (i = 0; i < p->nDisciplinas; i++) {
        totalSalas = 0;
        for (sala = 0; sala < p->nSalas; sala++) {
            //printf("%d ", salasUsadas[i][sala]);
            if (salasUsadas[i][sala]) {
                totalSalas++;
            }
            //printf("\n");
        }
        //printf("%s: %d\n",(p->disciplinas+i)->nomeDisciplina,totalSalas);
        soma4 += (totalSalas - 1);

    }

    soma2 = 0;
    for (i = 0; i < p->nDisciplinas; i++) {
        totalDias = 0;
        for (dia = 0; dia < p->nDias; dia++) {
            //printf("%d ", diasOcupados[i][dia]);
            if (diasOcupados[i][dia]) {
                totalDias++;
            }
            //printf("\n");
        }
        //printf("%s: %d\n",(p->disciplinas+i)->nomeDisciplina,totalDias);
        if (totalDias < (p->disciplinas + i)->minDiasAula) {
            soma2 += 5 * ((p->disciplinas + i)->minDiasAula - totalDias);
        }

    }

    //zeraMatCurrDiasPeriodos(p, a);
    //inicializaMatCurrDiasPeriodos(p, a);
    soma3 = somaAulasIsoladas(p,a);

    //soma2 += somaViolacoesMinDias(p, vetAgenda); //restricoes soft 2
    //printf ("Cost of RoomCapacity (soft) : %d\n", soma1); printf ("Cost of MinWorkingDays (soft) : %d\n", soma2); printf ("Cost of Isolated Lectures (soft) : %d\n", soma3); printf ("Cost of Room Stability(soft) : %d\n", soma4);/////


    //liberaVetorAgendas(p, vetAgenda);


    for (i = 0; i < p->nDisciplinas; i++) {
        free(salasUsadas[i]);
        free(diasOcupados[i]);
    }
    free(salasUsadas);
    free(diasOcupados);


    a->soft1 = soma1;
    a->soft2 = soma2;
    a->soft3 = soma3;
    a->soft4 = soma4;
    return (float) (soma1 + soma2 + soma3 + soma4);
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
    return 1000 / (1.0 * somaViolacoesHard(p, i) + 1);
}

float fitness(Problema *p, Individuo *i) {
    //printf ("func fitness : %f\n", 1000 / (0.999*somaViolacoesHard(p, i) + 0.001*somaViolacoesSoft(p, i) + 1));///////
    return 1000 / (0.999 * somaViolacoesHard(p, i) + 0.001 * somaViolacoesSoft(p, i) + 1);
}

float fitnessSoft(Problema *p, Individuo *i) {
    //printf ("func fitness : %f\n", 1000 / (0.999*somaViolacoesHard(p, i) + 0.001*somaViolacoesSoft(p, i) + 1));///////
    return 1000 / (0.001 * somaViolacoesSoft(p, i) + 1);
}
