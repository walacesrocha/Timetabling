#include <math.h>
#include "sa.h"
#include "fitness.h"
#include "util.h"

/*Construction of Course Timetables Based on Great
Deluge and Tabu Search*/

Individuo *greatDeluge(Problema*p, Individuo *indInicial) {
    Individuo *solGD, *solBestGD, *vizinho;
    float fSolGD, fSolBestGD, fViz, level;
    float OptimalRate;
    float deltaB;
    int iteracoes, NumOfIteGD;
    int notImprovingCounter, notImprovingCounterLength;



    //SolGD ← Sol;
    solGD = copiaIndividuo(p, indInicial);


    // SolbestGD ← Sol
    solBestGD = copiaIndividuo(p, indInicial);

    //f(SolGD) ← f(Sol);
    //f(SolbestGD)← f(Sol)
    fSolGD = funcaoObjetivo(p, indInicial);
    fSolBestGD = fSolGD;

    //Set number of iterations, NumOfIteGD;
    iteracoes = 0;
    NumOfIteGD = 10000;

    //todo Set optimal rate of final solution, Optimalrate;
    OptimalRate = 0; // configurar por instancia


    //Set initial level: level ← f(SolGD);
    level = fSolGD;

    //Set decreasing rate B = ((f(SolGD)–Optimalrate)/(NumOfIteGD);
    deltaB = (fSolGD - OptimalRate) / NumOfIteGD;

    //Set not_improving_counter ← 0, not_improving_ length_GDA;
    notImprovingCounter = 0;
    notImprovingCounterLength = 5; //configurar

    //do while (iteration < NumOfIteGD)
    while (iteracoes < NumOfIteGD) {
        /*Apply neighbourhood structure Ni where i Î {1,…,K} on SolGD,TempSolGDi;
          Calculate cost function f(TempSolGDi);
          Find the best solution among TempSolGDi where i Î {1,…,K} call new solution SolGD*;*/

        vizinho = geraVizinho2(p, solGD);

        fViz = somaViolacoesSoft(p, vizinho);

        /*if (f(SolGD*) < f(SolbestGD))
         SolGD ← SolGD*;
         SolbestGD ← SolGD*;
         not_improving_counter ← 0;
         level = level - B;*/

        if (fViz < fSolBestGD) {
            solGD = vizinho;

            liberaIndividuo(solBestGD);
            solBestGD = copiaIndividuo(p, vizinho);
            fSolBestGD = fSolGD = fViz;

            notImprovingCounter = 0;
            level = level - deltaB;
            
            printf("Melhorou: %f\n", fViz);
        }/*else
        if (f(SolGD*)≤ level)
        SolGD ← SolGD*;
        not_improving_counter ← 0;*/
        else if (fViz <= level) {
            solGD = vizinho;
            fSolGD = fViz;
            notImprovingCounter = 0;
        }/*else
        not_improving_counter++;
        if (not_improving_counter == not_improving_length_GDA)
        level= level + random(0,3);*/
        else {
            notImprovingCounter++;
            if (notImprovingCounter == notImprovingCounterLength) {
                level = level + (rand() % 3) + 1;
            }
        }

        //Increase iteration by 1;
        iteracoes++;

        printf("[%d]: %f, %f\n", iteracoes, fSolGD, level);

        //end do;
    }
    /* 
return SolbestGD;
     * 
     * */

    return solBestGD;

}