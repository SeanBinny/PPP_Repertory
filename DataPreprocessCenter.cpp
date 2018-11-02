#include <math.h>
#include "DataProcessCenter.h"

/*                    ****************************************************                                       */
/*---------------------------------- Class CycleSlipCenter ------------------------------------------------------*/
/*                    ****************************************************                                       */

/*------------------ Define static member--------------------------------------------*/
double CycleSlipCenter::lambda1     = 0;
double CycleSlipCenter::lambda2     = 0;
double CycleSlipCenter::f1          = 0;
double CycleSlipCenter::f2          = 0;

double ObsJudgeCenter::oC1          = 0;
double ObsJudgeCenter::nC1          = 0;
double ObsJudgeCenter::oP2          = 0;
double ObsJudgeCenter::nP2          = 0;
double ObsJudgeCenter::oP3          = 0;
double ObsJudgeCenter::nP3          = 0;
double ObsJudgeCenter::oL1          = 0;
double ObsJudgeCenter::nL1          = 0;
double ObsJudgeCenter::oL2          = 0;
double ObsJudgeCenter::nL2          = 0;
double ObsJudgeCenter::oL3          = 0;
double ObsJudgeCenter::nL3          = 0;
double ObsJudgeCenter::eleMaskAngle = 0;
/*-----------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------
 * Name     : getLambda
 * Function : Get lambda1, lambda2, lambda3  and f1, f2, f3 for slip detect
 * Input    : double F1, double F2  double F3.(frequence of observation value)
 * Output   : NULL
 *-----------------------------------------------------------------------------------*/
void CycleSlipCenter::getLambda(double F1, double F2)
{
    lambda1 = LIGHT_V / F1;
    lambda2 = LIGHT_V / F2;
    f1      =           F1;
    f2      =           F2;
}

/*-------------------------------------------------------------------------------------
 * Name     : WM_Combation
 * Function : Using W-M combation function to detect slip
 * Input    : double cC1, double cP2, double cL1, double cL2,
              double nC1, double nP2, double nL1, double nL2 .(n is next, o is the old one)
 * Output   : bool (true :don't have slip)
 *------------------------------------------------------------------------------------*/
bool CycleSlipCenter::WM_Combation(double oC1, double oP2, double oL1, double oL2,
                                   double nC1, double nP2, double nL1, double nL2)
{
    double Nw2   = (nL1 - nL2)-((f1 - f2) / (f1 + f2))* (nC1 / lambda1 + nP2 / lambda2);
    double Nw1   = (oL1 - oL2)-((f1 - f2) / (f1 + f2))* (oC1 / lambda1 + oP2 / lambda2);
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
              double nC1, double nP2, double nL1, double nL2 .(n is next, o is old one)
 * Output   : bool (true :don't have slip)
 *------------------------------------------------------------------------------------*/
bool CycleSlipCenter::IR_Combation(double oC1, double oP2, double oL1, double oL2,
                                   double nC1, double nP2, double nL1, double nL2)
{
    double slip1 = fabs (nL1 - oL1 - (f1 / f2) * (nL2 - oL2));
    double slip2 = fabs((lambda1 * nL1 - lambda2 * nL2 + nC1 - nP2) -
                        (lambda1 * oL1 - lambda2 * oL2 + oC1 - oP2));
    if (slip1 < 0.3 && slip2 <= 3.5)
        return true;

    return false;
}

/*                    ****************************************************                                       */
/*---------------------------------- Class ObsJudgeCenter -------------------------------------------------------*/
/*                    ****************************************************                                       */
/*------------------------------------------------------------------------------------
 * Name     : isObservationValid
 * Function : Judege if observation is valid
 * Input    : double OC1, double OP2, double OL1,
 *            double OL2, double OP3, double OL3,
 *            double NC1, double NP2, double NL1,
 *            double NL2, double NP3, double NL3,
 *            double WU1, double WU2, double AG1,
 *            double AG2 .(Value need to be check)
 *            double F1,  double F2,  double F3
 * Output   : QString (Error type)
 *-----------------------------------------------------------------------------------*/
QString ObsJudgeCenter::isObservationValid(double OC1, double OP2, double OL1,        // Main judgement function
                                           double OL2, double OP3, double OL3,
                                           double NC1, double NP2, double NL1,
                                           double NL2, double NP3, double NL3,
                                           double WU1, double WU2, double AG1,        // WU is wind up, and AG is angle
                                           double AG2, double F1,  double F2,
                                           double F3)
{
    oC1 = OC1, oP2 = OP2, oL1 = OL1, oL2 = OL2, oP3 = OP3, oL3 = OL3;
    nC1 = NC1, nP2 = NP2, nL1 = NL1, nL2 = NL2, nP3 = NP3, nL3 = NL3;
    if (!isObservationExist())
        return "Not Enough";

    CycleSlipCenter::getLambda(F1, F2);                                               // Get f1, f2 for judgeing slip
    if (!CycleSlipCenter::IR_Combation (oC1, oP2, oL1, oL2, nC1, nP2, nL1, nL2) ||    // Check slip and wind up
        !CycleSlipCenter::WM_Combation (oC1, oP2, oL1, oL2, nC1, nP2, nL1, nL2) ||
                         !isWindUpValid(WU1, WU2))
        return "Slip";
    CycleSlipCenter::getLambda(F1, F3);                                               // Get f1, f2 for judgeing slip
    if ( ModeFlag::TF &&
       (!CycleSlipCenter::IR_Combation (oC1, oP3, oL1, oL3, nC1, nP3, nL1, nL3) ||    // Check triple frequence slip
        !CycleSlipCenter::WM_Combation (oC1, oP3, oL1, oL3, nC1, nP3, nL1, nL3)))
        return "Slip";

    if (!isElevationAngleValid(AG1, AG2))                                             // Check elevation angle
        return "Angle";
    return "Valid";
}

/*------------------------------------------------------------------------------------
 * Name     : isSystemTypeValid
 * Function : Judge if satellite type can match the chosen system
 * Input    : const Sat_TypePrn &satTN
 * Output   : bool (if satellite type is valid)
 *-----------------------------------------------------------------------------------*/
bool ObsJudgeCenter::isSystemTypeValid(const Sat_TypePrn &satTN)
{
    if (!(satTN.Type == "G"    ||  satTN.Type == "C" ||
          satTN.Type == "R"    ||  satTN.Type == "E"))
          return false;
    if  (!ModeFlag::hasGPS     &&  satTN.Type == "G")
          return false;
    if  (!ModeFlag::hasBDS     &&  satTN.Type == "C")
          return false;
    if  (!ModeFlag::hasGLONASS &&  satTN.Type == "R")
          return false;
    if  (!ModeFlag::hasGalileo &&  satTN.Type == "E")
          return false;

    return true;
}


/*                    ****************************************************                                       */
/*---------------------------------- Class SatInformationCenter -------------------------------------------------*/
/*                    ****************************************************                                       */

/*-----------------------------------------------------------------------------------
 * Name     : interploteSatClock
 * Function : Get satellite clock error
 * Input    : vector<AS_clkData>        .(Container for storing clock error data)
 *            Sat_TypePrn  satTN        .(Type and PRN of satellite)
 *            double accumulateGpsSec   .(Accumulate Gps Seconds)
 *            int posInArray            .(Position in clock data array)
 * Output   : double (Satellite clock error we want)
 *---------------------------------------------------------------------------------*/
double SatInformationCenter::interploteSatClock(const vector<AS_clkData> &satClkDataArray,
                                                const Sat_TypePrn        &satTN,
                                                const double &accumulateGpsSec,
                                                const int    &posInArray)
{
    double timeArray[3]{0,0,0};
    double clkArray [3]{0,0,0};

    for (int i = 0; i < 3; i++)
    {
        AS_clkData sClkData = satClkDataArray[posInArray-1 + i];
        timeArray[i]        = sClkData.accumulateGpsSec;

        if      (satTN.Type == "G")
            clkArray[i] = sClkData.clockData_GPS[satTN.PRN-1];
        else if (satTN.Type == "C")
            clkArray[i] = sClkData.clockData_BDS[satTN.PRN-1];
        else if (satTN.Type == "R")
            clkArray[i] = sClkData.clockData_GLONASS[satTN.PRN-1];
        else if (satTN.Type == "E")
            clkArray[i] = sClkData.clockData_Galileo[satTN.PRN-1];
        else if (satTN.Type == "J")
            clkArray[i] = sClkData.clockData_QZSS[satTN.PRN-1];
        else if (satTN.Type == "S")
            clkArray[i] = sClkData.clockData_SBAS[satTN.PRN-1];

    }
    double clk = MyFunctionCenter::lagrangeInter(timeArray, clkArray,                 // Use lagrange interplotation
                                                 accumulateGpsSec , 3);
    return clk;

}

/*-----------------------------------------------------------------------------------
 * Name     : interploteSatCoord
 * Function : Get satellite position
 * Input    : vector<EpochPreciseOrbitData> .(Container for storing satellite coordinate)
 *            Sat_TypePrn  satTN            .(Type and PRN of satellite)
 *            double accumulateGpsSec       .(Accumulate Gps Seconds)
 *            int posInArray                .(Position in clock data array)
 * Output   : VectorXd (Satellite coordinate we want)
 *---------------------------------------------------------------------------------*/
VectorXd SatInformationCenter::interploteSatCoord(const vector<EpochPreciseOrbitData>
                                                  &satCoordDataArray,
                                                  const  Sat_TypePrn &satTN,
                                                  const double &accumulateGpsSec,
                                                  const int &posInArray)
{
    double timeArray[9]{0};
    double xArray   [9]{0};
    double yArray   [9]{0};
    double zArray   [9]{0};

    VectorXd xyzvel;
    xyzvel.setZero(6, 1);
    for (int i = 0; i < 9; i++)
    {
        EpochPreciseOrbitData sCoordData = satCoordDataArray[posInArray-4 + i];
        timeArray[i] = sCoordData.accumulateGpsSec;

        if      (satTN.Type == "G"){
            int index = PrecisionEphemerisFile::getSatIndex
                       (sCoordData.OrbitData_GPS, satTN.PRN);
            if (index == -1)       return xyzvel;
            xArray[i] = sCoordData.OrbitData_GPS[ index].x;
            yArray[i] = sCoordData.OrbitData_GPS[ index].y;
            zArray[i] = sCoordData.OrbitData_GPS[ index].z;
        }
        else if (satTN.Type == "C"){
            int index = PrecisionEphemerisFile::getSatIndex
                       (sCoordData.OrbitData_BDS, satTN.PRN);
            if (index == -1)       return xyzvel;
            xArray[i] = sCoordData.OrbitData_BDS[ index].x;
            yArray[i] = sCoordData.OrbitData_BDS[ index].y;
            zArray[i] = sCoordData.OrbitData_BDS[ index].z;
        }
        else if (satTN.Type == "R"){
            int index = PrecisionEphemerisFile::getSatIndex
                       (sCoordData.OrbitData_GLONASS, satTN.PRN);
            if (index == -1)       return xyzvel;
            xArray[i] = sCoordData.OrbitData_GLONASS[ index].x;
            yArray[i] = sCoordData.OrbitData_GLONASS[ index].y;
            zArray[i] = sCoordData.OrbitData_GLONASS[ index].z;
        }
        else if (satTN.Type == "E"){
            int index = PrecisionEphemerisFile::getSatIndex
                       (sCoordData.OrbitData_Galileo, satTN.PRN);
            if (index == -1)       return xyzvel;
            xArray[i] = sCoordData.OrbitData_Galileo[ index].x;
            yArray[i] = sCoordData.OrbitData_Galileo[ index].y;
            zArray[i] = sCoordData.OrbitData_Galileo[ index].z;
        }

        if (xArray[i] == 0)// || yArray[i] == 0 || zArray[i] == 0)
            throw illegalParameterValue("Orbit data of satellite is (0, 0, 0) !");

    }

    X_Y_Z xyz0  = MyFunctionCenter::lagrangeInter(timeArray, xArray, yArray,
                                                  zArray, accumulateGpsSec,       9);

    X_Y_Z xyz1  = MyFunctionCenter::lagrangeInter(timeArray, xArray, yArray,
                                                  zArray, accumulateGpsSec + 0.5, 9);
    X_Y_Z xyz2  = MyFunctionCenter::lagrangeInter(timeArray, xArray, yArray,
                                                  zArray, accumulateGpsSec - 0.5, 9);
    X_Y_Z xyzDiff(xyz1.X - xyz2.X, xyz1.Y - xyz2.Y, xyz1.Z - xyz2.Z);

    xyzvel[0] = xyz0.X;
    xyzvel[1] = xyz0.Y;
    xyzvel[2] = xyz0.Z;
    xyzvel[3] = xyzDiff.X;
    xyzvel[4] = xyzDiff.Y;
    xyzvel[5] = xyzDiff.Z;
    return xyzvel;
}
/*-----------------------------------------------------------------------------------
 * Name     : interploteSunMoon
 * Function : Get sun and moon position
 * Input    : vector<SunMoonPosition> &sunMoonPos .(Container for storing sun and moon position)
 *            double accumulateGpsSec             .(Accumulate Gps Seconds)
 *            int    posInArray                   .(Position in clock data array)
 * Output   : MatrixXd (sun and moon position we want)
 *---------------------------------------------------------------------------------*/
MatrixXd SatInformationCenter::interploteSunMoon(const  vector<SunMoonPosition> &sunMoonPos,
                                                 const double &accumulateGpsSec,
                                                 const int    &posInArray)
{
    MatrixXd resSunMoonPos(3,2);
    double   timeArray [5];
    double   sunXArray [5]{0};
    double   sunYArray [5]{0};
    double   sunZArray [5]{0};
    double   moonXArray[5]{0};
    double   moonYArray[5]{0};
    double   moonZArray[5]{0};
    for (int i = 0; i < 5; i++)
    {
        SunMoonPosition smPos = sunMoonPos[posInArray - 2 + i];
        timeArray[i]  = smPos.accumulateGpsSec;
        sunXArray[i]  = smPos.sunPosition [0];
        sunYArray[i]  = smPos.sunPosition [1];
        sunZArray[i]  = smPos.sunPosition [2];
        moonXArray[i] = smPos.moonPosition[0];
        moonYArray[i] = smPos.moonPosition[1];
        moonZArray[i] = smPos.moonPosition[2];
    }
     X_Y_Z sunXYZ  = MyFunctionCenter::lagrangeInter(timeArray,  sunXArray,  sunYArray,
                                                     sunZArray,  accumulateGpsSec, 5);
     X_Y_Z moonXYZ = MyFunctionCenter::lagrangeInter(timeArray,  moonXArray, moonYArray,
                                                     moonZArray, accumulateGpsSec, 5);

     resSunMoonPos(0, 0) = sunXYZ.X,  resSunMoonPos(1, 0) = sunXYZ.Y,
     resSunMoonPos(2, 0) = sunXYZ.Z;
     resSunMoonPos(0, 1) = moonXYZ.X, resSunMoonPos(1, 1) = moonXYZ.Y,
     resSunMoonPos(2, 1) = moonXYZ.Z;
     return resSunMoonPos;
}

/*-----------------------------------------------------------------------------------
 * Name     : getSatlliteInformation
 * Function : Get temparary status information of satellite
 * Input    : cosnt vector<AS_clkData>            &satClkDataArray,
 *            const vector<EpochPreciseOrbitData> satCoordDataArray,
 *            SingleSatelliteData                 &satData,
              const CoordSysParameter             &coordPar,
              const AntennaData                   &satAnt,
              const AntennaData                   &recAnt,
              const Vector3d                      &antENU,
              const Vector3d                      &recXYZ,
              const Vector3d                      &recBLH,
              const Vector3d                      &sunXYZ,
              const Vector3d                      &tide,
              const MyTime                        &myTime,
              const MyTime                        &TDT,
              const int                           &satClockPos,
              const int                           &satCoordPos, int *K
 * Output   : SingleSatelliteData &satData  .(Essential data of single satellite)
 *---------------------------------------------------------------------------------*/
void SatInformationCenter::getSatelliteInformation(const vector<AS_clkData> &satClkDataArray,
                                                   const vector<EpochPreciseOrbitData>
                                                                           &satCoordDataArray,
                                                   SingleSatelliteData     &satData,
                                                   const CoordSysParameter &coordPar,
                                                   const AntennaData       &satAnt,
                                                   const AntennaData       &recAnt,
                                                   const Vector3d          &antENU,
                                                   const Vector3d &recXYZ, const Vector3d &recBLH,
                                                   const Vector3d &sunXYZ, const Vector3d &tide,
                                                   const MyTime   &myTime, const MyTime   &TDT,
                                                   const int      &satClockPos,
                                                   const int      &satCoordPos, int *K)
{
    int    obweek     = myTime.GPT.week - satCoordDataArray[0].myTime.GPT.week;
    double tempTime   = myTime.GPT.sec  + obweek * 604800 - satData.obsData.P2 / LIGHT_V; /* unknown 时间这还存在问题，看看是否直接用累积时间*/
    Sat_TypePrn &satTN= satData.obsData.satTN;
    MyFunctionCenter::getFrequency(satTN, K, satData.freq);

    satData.satClock  = interploteSatClock(satClkDataArray,   satTN, tempTime,
                                           satClockPos);
    VectorXd xyzvel   = interploteSatCoord(satCoordDataArray, satTN, tempTime -
                                           satData.satClock,  satCoordPos);

    if (xyzvel[0] == 0 || xyzvel[1] == 0 || xyzvel[2] == 0)                          // If xyz are 0, exit this sat
        return;
    satData.satXYZ[0] = xyzvel[0];
    satData.satXYZ[1] = xyzvel[1];
    satData.satXYZ[2] = xyzvel[2];
    satData.satVel[0] = xyzvel[3];
    satData.satVel[1] = xyzvel[4];
    satData.satVel[2] = xyzvel[5];

    Vector3d satVel (satData.satVel[0],  satData.satVel[1], satData.satVel[2]);
    Vector3d satXyz (satData.satXYZ[0],  satData.satXYZ[1], satData.satXYZ[2]);
    Vector3d ENU       = MyFunctionCenter::XYZToENU(recXYZ, satXyz, coordPar);
    Vector3d DAZ       = MyFunctionCenter::ENUToDAZ(ENU);
    satData.DAZ[0] = DAZ[0], satData.DAZ[1] = DAZ[1], satData.DAZ[2] = DAZ[2];
    Vector2d antOffset       = ErrorModel::antennaRecCor   (recAnt, DAZ);

    satData.sagnacEffect     = ErrorModel::sagnacCor       (satXyz, recXYZ);
    satData.relativityEffect = ErrorModel::relativityCor   (satXyz, recXYZ, satVel);
    satData.tideCorre        = ErrorModel::allTideCor      (DAZ,    tide);
    satData.anteHeight       = ErrorModel::antennaHeightCor(DAZ,    antENU);
    satData.anteCenterCorre  = ErrorModel::antennaSatCor   (satAnt, recXYZ, satXyz, sunXYZ);
    satData.phaseWinding     = ErrorModel::antennaWindUpCor(recBLH, recXYZ, satXyz, sunXYZ,
                                                        satAnt.antennaType, satTN);
    VectorXd NMF = ErrorModel::tropDelyCor.UNB3M(recBLH, TDT.DOY, PI/2 - DAZ[2]);
    satData.tropDryDelay     = NMF[1]*NMF[2];
    satData.tropWetDelayFunc = NMF[4];

    satData.anteCenterOffset[0] = antOffset[0];
    satData.anteCenterOffset[1] = antOffset[1];
    satData.Delta0 = satData.satClock*LIGHT_V - satData.tropDryDelay +               // Calulate all error delay
                     satData.relativityEffect + satData.sagnacEffect +
                     satData.tideCorre        + satData.anteHeight   +
                     satData.anteCenterCorre;

    double lambda1 = satData.freq[0] / LIGHT_V;
    double lambda2 = satData.freq[1] / LIGHT_V;
    satData.Delta1 = satData.anteCenterOffset[0]/lambda1 - satData.phaseWinding; /*No Use*/
    satData.Delta2 = satData.anteCenterOffset[1]/lambda2 - satData.phaseWinding;
}

