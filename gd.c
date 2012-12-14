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
    Individuo *solGD, *solBestGD;
    float fSolGD, fSolBestGD, fViz;
    float OptimalRate;
    float deltaB;
    int iteracoes, NumOfIteGD;
    int notImprovingCounter, notImprovingCounterLength;
    float iUpperBoundRate=1.05,iLowerBoundRate=0.9,iCoolRate=0.999995;
    float iBound,iNrIdle=0;
    float pesoHard=p->pesoHard;
    float pViz,deltaF;
    Neighbour *mov;



    //SolGD ← Sol;
    solGD = indInicial;


    // SolbestGD ← Sol
    solBestGD = copiaIndividuo(p, indInicial);

    //f(SolGD) ← f(Sol);
    //f(SolbestGD)← f(Sol)
    fSolGD = funcaoObjetivo(p, solGD,pesoHard);
    fSolBestGD = fSolGD;

    //Set number of iterations, NumOfIteGD;
    iteracoes = 0;
    iBound = fSolBestGD+5;
    NumOfIteGD = 15;

    //todo Set optimal rate of final solution, Optimalrate;
    OptimalRate = 0; // configurar por instancia


    //Set initial level: level ← f(SolGD);

    //Set decreasing rate B = ((f(SolGD)–Optimalrate)/(NumOfIteGD);
    deltaB = (fSolGD - OptimalRate) / NumOfIteGD;

    
    //do while (iteration < NumOfIteGD)
    while (iteracoes < NumOfIteGD) {
        //printf("[It %d]: SA: %f, Best:(H:%f,S:%f=%f), Bound: %f, [Lower=%f]\n", iteracoes, fSolGD, 
          //      somaViolacoesHard(p,solBestGD),somaViolacoesSoft(p,solBestGD),fSolBestGD, iBound, pow(iLowerBoundRate, 1 + iNrIdle) * fSolBestGD);
        /*Apply neighbourhood structure Ni where i Î {1,…,K} on SolGD,TempSolGDi;
          Calculate cost function f(TempSolGDi);
          Find the best solution among TempSolGDi where i Î {1,…,K} call new solution SolGD*;*/

        //vizinho = buscaLocalGraspVNS(p, solGD);
        
        pViz = (float) rand() / RAND_MAX;

        if (pViz < 0.2) {
                mov = geraIsolatedLectureMove(p, indInicial);
            } else if (pViz < 0.4) {
                mov = geraMove(p, indInicial);
            } else if (pViz < 0.6) {
                mov = geraTimeMove(p, indInicial);
            } else if (pViz < 0.8) {
                mov = geraRoomMove(p, indInicial);
            } else {
                mov = geraMinWorkingDaysMove(p, indInicial);
            }

            deltaF = mov->deltaHard * pesoHard + mov->deltaSoft;

        if (fSolGD + deltaF < fSolBestGD) {

            troca_par_completo(p, solGD, mov->p1,mov->p2);
            liberaIndividuo(solBestGD);
            solBestGD = copiaIndividuo(p, solGD);
            
            fSolBestGD = fSolGD + deltaF;
            iBound += deltaF;//*iUpperBoundRate;
            
            iNrIdle=0;
            
            //scanf("%d\n",&notImprovingCounter);
        }/*else
        if (f(SolGD*)≤ level)
        SolGD ← SolGD*;
        not_improving_counter ← 0;*/
        else if (fSolGD + deltaF <= iBound) {

            troca_par_completo(p, solGD, mov->p1, mov->p2);
            fSolGD += deltaF;
        }/*else
        not_improving_counter++;
        if (not_improving_counter == not_improving_length_GDA)
        level= level + random(0,3);*/


        //Increase iteration by 1;
        iteracoes++;
        //iBound *= iCoolRate;

        /*if (iBound < (pow(iLowerBoundRate, 1 + iNrIdle) * fSolBestGD)) {
            iNrIdle++;
            //printf(" -<[%d]>- \n", iNrIdle);
            iBound = maxFloat(fSolBestGD + 2.0, pow(iUpperBoundRate, iNrIdle) * fSolBestGD);
        }*/




        printf("[It %d]: SA: %f, SB: %f, Bound: %f\n", iteracoes, fSolGD, fSolBestGD, iBound);

        //end do;
    }
    /* 
return SolbestGD;
     * 
     * */

    return solBestGD;

}
