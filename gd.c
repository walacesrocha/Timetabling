#include <math.h>
#include "sa.h"
#include "fitness.h"
#include "util.h"

/*Construction of Course Timetables Based on Great
Deluge and Tabu Search*/

float maxFloat(float f1, float f2){
    if (f1 > f2){
        return f1;
    }else{
        return f2;
    }
}

Individuo *greatDeluge(Problema*p, Individuo *indInicial) {
    Individuo *solGD, *solBestGD, *vizinho;
    float fSolGD, fSolBestGD, fViz, level;
    float OptimalRate;
    float deltaB;
    int iteracoes, NumOfIteGD;
    int notImprovingCounter, notImprovingCounterLength;
    float iUpperBoundRate=1.05,iLowerBoundRate=0.9,iCoolRate=0.999995;
    float iBound,iNrIdle=0;



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
    iBound = iUpperBoundRate*fSolBestGD;
    NumOfIteGD = 50000;

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

            liberaIndividuo(solBestGD);
            liberaIndividuo(solGD);
            solBestGD = copiaIndividuo(p, vizinho);
            solGD = copiaIndividuo(p, vizinho);
            
            fSolBestGD = fSolGD = fViz;
            iBound = fSolBestGD*iUpperBoundRate;
            
            iNrIdle=0;
            

            notImprovingCounter = 0;
            level = level - deltaB;
            
            printf("GD: %f\n", fViz);
            
            //scanf("%d\n",&notImprovingCounter);
        }/*else
        if (f(SolGD*)≤ level)
        SolGD ← SolGD*;
        not_improving_counter ← 0;*/
        else if (fViz <= iBound) {
            liberaIndividuo(solGD);
            solGD = copiaIndividuo(p,vizinho);
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

        liberaIndividuo(vizinho);

        //Increase iteration by 1;
        iteracoes++;
        iBound *= iCoolRate;

        if (iBound < (pow(iLowerBoundRate, 1 + iNrIdle) * fSolBestGD)) {
            iNrIdle++;
            //printf(" -<[%d]>- \n", iNrIdle);
            iBound = maxFloat(fSolBestGD + 2.0, pow(iUpperBoundRate, iNrIdle) * fSolBestGD);
        }


        //printf("[%d]: %f, (%f), %f, [Lower=%f]\n", iteracoes, fSolGD, fSolBestGD, iBound, pow(iLowerBoundRate, 1 + iNrIdle) * fSolBestGD);

        //end do;
    }
    /* 
return SolbestGD;
     * 
     * */

    return solBestGD;

}
