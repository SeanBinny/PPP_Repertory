#ifndef DATAPROCESSINGCENTER_H
#define DATAPROCESSINGCENTER_H
#include "GlobalDefinationCenter.h"
#include "PublicDataCenter.h"
#include "ProductDataCenter.h"
#include "RinexDataCenter.h"
#include "ResultDataCenter.h"
#include "MyFunctionCenter.h"
#include "ErrorModelCenter.h"


/*------------------------------------------------------------------------------
 * Name     : CycleSlipCenter
 * Function : Deal with the cycle slip
 *-----------------------------------------------------------------------------*/
class CycleSlipCenter
{
public:
    static void getLambda   (double F1,  double F2);                            // Get lambda 1 and lambda 2

    static bool WM_Combation(double oC1, double oP2, double oL1, double oL2,    // W-M comibination
                             double nC1, double nP2, double nL1, double nL2);
    static bool IR_Combation(double oC1, double oP2, double oL1, double oL2,    // Ionospheric residual combination
                             double nC1, double nP2, double nL1, double nL2);   // Geometric unrelated combination
private:
    static double lambda1;
    static double lambda2;
    static double f1;
    static double f2;
};
/*------------------------------------------------------------------------------
 * Name     : ObsJudgeCenter
 * Function : Check all kinds of errors of observation
 *-----------------------------------------------------------------------------*/
class ObsJudgeCenter
{
public:
    static QString isObservationValid(double OC1, double OP2, double OL1,       // Main judgement function
                                      double OL2, double OP3, double OL3,
                                      double NC1, double NP2, double NL1,
                                      double NL2, double NP3, double NL3,
                                      double WU1, double WU2, double AG1,       // WU is wind up, and AG is angle
                                      double AG2, double F1,  double F2,
                                      double F3);

    static bool isObservationExist()                                            // Judge if observation are all exist
    {
        if (ModeFlag::TF &&
           (oL3 == 0 || nL3 == 0 || oP3 == 0 || nP3 == 0))
            return false;
        if (oC1 != 0 && oP2 != 0 && oL1 != 0  && oL2 !=0 &&
            nC1 != 0 && nP2 != 0 && nL1 != 0  && nL2 !=0 &&
            fabs(oC1 - oP2) < 50 && fabs(nC1 - nP2) < 50 )
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
        return true;
    }
    static bool isElevationAngleValid(double angle1, double angle2)             // Judge if elevation angle is valid
    {
        if ((PI/2 - angle1) > (eleMaskAngle * PI / 180) &&
            (PI/2 - angle2) > (eleMaskAngle * PI / 180))
            return true;
        else
            return false;
    }
    static bool isSystemTypeValid(const Sat_TypePrn &satTN);

private:
    static double nL1, nL2, nC1, nP2, nL3, nP3;
    static double oL1, oL2, oC1, oP2, oL3, oP3;                                 // Old observe value
public:
    static double eleMaskAngle;                                                 // Valid observe elevation mask angle
};


/*------------------------------------------------------------------------------
 * Name     : DataMatchingCenter
 * Function : Get data position from array
 *-----------------------------------------------------------------------------*/
class DataMatchingCenter
{
public:
    static int seekRecAntDataPos (const AntennaInfoFile &antennaDataBase,
                                  QString recType);
    static int seekOceanDataPos  (const OceanTideFile &oceanDatabase,
                                  QString markerName);
    static int seekErpDataPos    (const EarthRotationParameterFile &erpDataBase,
                                  double JD);
    static int seekPreEphDataPos (const PrecisionEphemerisFile
                                  &preEphDataBase, const double &matchTime,
                                  const int &lastPosInArray);
    static int seekPreClockPos   (const PrecisionClockFile
                                  &preClkDataBase, const double &matchTime,
                                  const int &lastPosInArray);
    static int seekApproxCoordPos(const IgsStationCoordinateFile &coordDataBase,
                                  QString stationName,      int  date);
    static Vector3d    seekSunPosition (const MyTime &tdtTime);
    static Vector3d    seekMoonPosition(const MyTime &tdtTime);
    static AntennaData seekSatAntData  (const AntennaInfoFile &antennaDataBase,
                                        const Sat_TypePrn &satTN,    const double &matchTime);
private:
    static Vector3d getSunPosCIS (const MyTime &tdtTime);
    static Vector3d getMoonPosCIS(const MyTime &tdtTime);

};
class isTimeMatch                                                               // Functors or Function Objects to judge if time matched
{
public:
    isTimeMatch(double t): matchTime(t) {}
    bool operator()(const AntennaData &antData) const
    {
        if (antData.VALID_UNTIL !=  0.0){
            if (antData.VALID_FROM  < matchTime &&
                antData.VALID_UNTIL > matchTime)
            return true;
        }else{
            if (antData.VALID_UNTIL < matchTime)
                return true;
        }
        return false;
     }
private:
    double matchTime;
};

template <typename T>
int epochMatch(const vector<T> &V, const double &matchTime, const int &lastPosInArray)               // template function to match current epoch data
{
    double  diff = 99999999;
    for (unsigned int i   = lastPosInArray; i < V.size(); i++)
    {
        if (diff < fabs(matchTime - V[i].myTime.MJD))
            return i - 1;
        else
            diff = fabs(matchTime - V[i].myTime.MJD);
    }
    return -1;
}

/*------------------------------------------------------------------------------
 * Name     : SatInformationCenter
 * Function : Get single satellite information and errors
 *-----------------------------------------------------------------------------*/
class SatInformationCenter
{
public:
    friend class ResultFile_PPP;
    static void  getSatelliteInformation(const vector<AS_clkData> &satClkDataArray,
                                         const vector<EpochPreciseOrbitData>
                                                                  &satCoordDataArray,
                                         SingleSatelliteData      &satData,
                                         const CoordSysParameter  &coordPar,
                                         const AntennaData        &satAnt, const AntennaData &recAnt,
                                         const Vector3d &antENU,
                                         const Vector3d &recApproxCoord,   const Vector3d &recBLH,
                                         const Vector3d &sunPos,           const Vector3d &tide,
                                         const MyTime   &myTime,           const MyTime   &TDT,
                                         const int      &satClockPos,
                                         const int      &satCoordPos,      int *K);
private:
    static double    interploteSatClock( const  vector<AS_clkData> &satClkDataArray,
                                         const  Sat_TypePrn        &satTN,
                                         const  double &accumulateGpsSec,
                                         const  int    &posInArray);
    static VectorXd  interploteSatCoord( const  vector<EpochPreciseOrbitData> &satCoordDataArray,
                                         const  Sat_TypePrn  &satTN,
                                         const  double &accumulateGpsSec,
                                         const  int    &posInArray);
    static MatrixXd  interploteSunMoon(  const  vector<SunMoonPosition> &sunMoonPos,
                                         const  double &accumulateGpsSec,
                                         const  int    &posInArray);
};



/*------------------------------------------------------------------------------
 * Name     : FilterProcessingCenter
 * Function : Deal with the filter processing cycle
 *-----------------------------------------------------------------------------*/
class FilterProcessingCenter
{
public:
    void filterProcessing(FinalDataFile &initialData, QTextStream &neuText);

private:
    void siftSatellite(FinalDataFile &pppFile, int pos);                        // Classification of satellites
private:

    vector <SatQuery> oSatContaine;                                             // Containe to save old  epoch valid satelites
    vector <SatQuery> nSatContaine;                                             // Containe to save next epoch valid satelites
    vector <SatQuery> sSatContaine;                                             // Containe to save satellites with cycle slip;
};

#endif // DATAPREPROCESSINGCENTER_H




