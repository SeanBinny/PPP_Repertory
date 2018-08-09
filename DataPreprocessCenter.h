#ifndef DATAPREPROCESSINGCENTER_H
#define DATAPREPROCESSINGCENTER_H
#include "GlobalDefinationCenter.h"
#include <math.h>

/*-----------------------------------------------------------------------------
 * Name     : CycleSlipCenter
 * Function : Deal with the cycle slip
 *----------------------------------------------------------------------------*/
class CycleSlipCenter
{
public:
    static void getLambda   (double F1,  double F2);                            // Get lambda 1 and lambda 2

    static bool WM_Combation(double cC1, double cP2, double cL1, double cL2,    // W-M comibination
                             double nC1, double nP2, double nL1, double nL2);
    static bool IR_Combation(double cC1, double cP2, double cL1, double cL2,    // Ionospheric residual combination
                             double nC1, double nP2, double nL1, double nL2);   // Geometric unrelated combination
private:
    static double lambda1;
    static double lambda2;
    static double f1;
    static double f2;
};
/*-----------------------------------------------------------------------------
 * Name     : ObsJudgeCenter
 * Function : Check all kinds of errors of observation
 *----------------------------------------------------------------------------*/
class ObsJudgeCenter
{
public:
    static char* isObservationValid(double CC1, double CP2, double CL1,         // Main judgement function
                                    double CL2, double NC1, double NP2,
                                    double NL1, double NL2, double WU1,         // WU is wind up, and AG is angle
                                    double WU2, double AG1, double AG2);
    static bool isObservationExist()                                            // Judge if observation are all exist
    {
        if (cC1 != 0 && cP2 != 0 && cL1 != 0  && cL2 !=0 &&
            nC1 != 0 && nP2 != 0 && nL1 != 0  && nL2 !=0 &&
            fabs(cC1 - cP2) < 50 && fabs(nC1 - nP2) < 50 )
            return true;
        else
            return false;
    }
    static bool isWindUpValid(double windUp1, double windUp2)                   // Judge if wind up is valid
    {
        if (fabs(windUp1 - windUp2) < 0.3)
            return true;
        else
            return false;
    }
    static bool isElevationAngleValid(double angle1, double angle2)             // Judge if elevation angle is valid
    {
        if ((PI/2 - angle1) > (eleMaskAngle * PI / 180) &&
            (PI/2 - angle2) > (eleMaskAngle * PI / 180))
            return true;
        else
            return false;
    }

private:
    static double nL1, nL2, nC1, nP2;
    static double cL1, cL2, cC1, cP2;
    static double eleMaskAngle;      /* whhy is 5*/  // Elevation mask angle
}

#endif // DATAPREPROCESSINGCENTER_H
