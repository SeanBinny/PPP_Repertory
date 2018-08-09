#include <math.h>
#include "DataPreprocessCenter.h"

/*                    ****************************************************                                       */
/*---------------------------------- Class CycleSlipCenter ------------------------------------------------------*/
/*                    ****************************************************                                       */

/*------------------ Define static member--------------------------------------------*/
double CycleSlipCenter::lambda1 = 0;
double CycleSlipCenter::lambda2 = 0;
double CycleSlipCenter::f1 = 0;
double CycleSlipCenter::f2 = 0;
/*-----------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------
 * Name     : getLambda
 * Function : Get lambda1, lambda2  and f1, f2 for slip detect
 * Input    : double F1, double F2 .(frequence of observation value)
 * Output   : NULL
 *-----------------------------------------------------------------------------------*/
void CycleSlipCenter::getLambda(double F1, double F2)
{
    lambda1 = C / F1;
    lambda2 = C / F2;
    f1      =     F1;
    f2      =     F2;
}
/*-------------------------------------------------------------------------------------
 * Name     : WM_Combation
 * Function : Using W-M combation function to detect slip
 * Input    : double cC1, double cP2, double cL1, double cL2,
              double nC1, double nP2, double nL1, double nL2 .(n is next, c is current)
 * Output   : bool (true :don't have slip)
 *------------------------------------------------------------------------------------*/
bool CycleSlipCenter::WM_Combation(double cC1, double cP2, double cL1, double cL2,
                                   double nC1, double nP2, double nL1, double nL2)
{
    double Nw2   = (nL1 - nL2)-((f1 - f2) / (f1 + f2))* (nC1 / lambda1 + nP2 / lambda2);
    double Nw1   = (cL1 - cL2)-((f1 - f2) / (f1 + f2))* (cC1 / lambda1 + cP2 / lambda2);
    double slip3 = fabs(Nw2 - Nw1);
    if (slip3 < 3.5)
        return true;
    else
        return false;
}
/*-------------------------------------------------------------------------------------
 * Name     : IR_Combation
 * Function : Using Ionospheric residual function to detect slip
 * Input    : double cC1, double cP2, double cL1, double cL2,
              double nC1, double nP2, double nL1, double nL2 .(n is next, c is current)
 * Output   : bool (true :don't have slip)
 *------------------------------------------------------------------------------------*/
bool CycleSlipCenter::IR_Combation(double cC1, double cP2, double cL1, double cL2,
                                   double nC1, double nP2, double nL1, double nL2)
{
    double slip1 = fabs (nL1 - cL1 - (f1 / f2) * (nL2 - cL2));
    double slip2 = fabs((lambda1 * nL1 - lambda2 * nL2 + nC1 - nP2) -
                        (lambda1 * cL1 - lambda2 * nL2 + nC1 - nP2));
    if (slip1 < 0.3 && slip2 <= 3.5)
        return true;
    else
        return false;
}


/*------------------------------------------------------------------------------------
 * Name     : isObservationValid
 * Function : Judege if observation is valid
 * Input    : double CC1, double CP2, double CL1,
              double CL2, double NC1, double NP2,
              double NL1, double NL2, double WU1,
              double WU2, double AG1, double AG2 .(Value need to be check)
 * Output   : char * (Error type)
 *-----------------------------------------------------------------------------------*/
char *ObsJudgeCenter::isObservationValid(double CC1, double CP2, double CL1,          // Main judgement function
                                         double CL2, double NC1, double NP2,
                                         double NL1, double NL2, double WU1,          // WU is wind up, and AG is angle
                                         double WU2, double AG1, double AG2)
{
    cC1 = CC1, cP2 = CP2, cL1 = CL1, cL2 = CL2;
    nC1 = NC1, nP2 = NP2, nL1 = NL1, nL2 = NL2;
    if (!isObservationExist())
        return "Enough";
    if (!CycleSlipCenter::IR_Combation (cC1, cP2, cL1, cL2, nC1, nP2, nL1, nL2) &&    // Check slip and wind up
        !CycleSlipCenter::WM_Combation (cC1, cP2, cL1, cL2, nC1, nP2, nL1, nL2) &&
                         !isWindUpValid(WU1, WU2))
        return "Slip";
    if (!isElevationAngleValid(AG1, AG2))                                             // Check elevation angle
        return "Angle";
    return "Valid";
}
