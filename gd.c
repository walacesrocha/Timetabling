#include <math.h>
#include "sa.h"
#include "fitness.h"
#include "util.h"
#include "buscalocal.h"

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
    float iUpperBoundRate=1.03,iLowerBoundRate=0.9,iCoolRate=0.999995;
    float iBound,iNrIdle=0;
    float pesoHard=1;



    //SolGD ← Sol;
    solGD = copiaIndividuo(p, indInicial);


    // SolbestGD ← Sol
    solBestGD = copiaIndividuo(p, indInicial);

    //f(SolGD) ← f(Sol);
    //f(SolbestGD)← f(Sol)
    fSolGD = funcaoObjetivo(p, indInicial,1);
    fSolBestGD = fSolGD;

    //Set number of iterations, NumOfIteGD;
    iteracoes = 0;
    iBound = fSolBestGD+15;//iUpperBoundRate+10;//*fSolBestGD;
    NumOfIteGD = 150000;

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
        //printf("[It %d]: SA: %f, Best:(H:%f,S:%f=%f), Bound: %f, [Lower=%f]\n", iteracoes, fSolGD, 
          //      somaViolacoesHard(p,solBestGD),somaViolacoesSoft(p,solBestGD),fSolBestGD, iBound, pow(iLowerBoundRate, 1 + iNrIdle) * fSolBestGD);
        /*Apply neighbourhood structure Ni where i Î {1,…,K} on SolGD,TempSolGDi;
          Calculate cost function f(TempSolGDi);
          Find the best solution among TempSolGDi where i Î {1,…,K} call new solution SolGD*;*/

        //vizinho = geraVizinho2(p, solGD);
        vizinho = buscaLocalGraspVNS(p, solGD);

        fViz = funcaoObjetivo(p, vizinho,1);

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
            //iBound = fSolBestGD*iUpperBoundRate;
            iBound = fSolBestGD+15;
            
            iNrIdle=0;
            

            notImprovingCounter = 0;
            level = level - deltaB;
            
            printf("GD: %f (%f)\n", fViz,somaViolacoesHard(p,vizinho));
            
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
        //iBound *= iCoolRate;

        /*if (iBound < (pow(iLowerBoundRate, 1 + iNrIdle) * fSolBestGD)) {
            iNrIdle++;
            //printf(" -<[%d]>- \n", iNrIdle);
            iBound = maxFloat(fSolBestGD + 2.0, pow(iUpperBoundRate, iNrIdle) * fSolBestGD);
        }*/


        printf("[It %d]: SA: %f, Best:(H:%f,S:%f=%f), Bound: %f, [Lower=%f]\n", iteracoes, fSolGD,
                somaViolacoesHard(p, solBestGD), somaViolacoesSoft(p, solBestGD), fSolBestGD, iBound, pow(iLowerBoundRate, 1 + iNrIdle) * fSolBestGD);

        //end do;
    }
    /* 
return SolbestGD;
     * 
     * */

    return solBestGD;

}
