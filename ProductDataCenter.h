#ifndef PRODUCTDATACENETR_H
#define PRODUCTDATACENETR_H
#include <vector>
#include <Eigen/Eigen>

using namespace std;
using namespace Eigen;
#include "FileCenter.h"
#include "PublicDataCenter.h"

/*                    ****************************************************                                       */
/*******************************      Precise ephemeris file(*.sp3)    *******************************************/
/*                    ****************************************************                                       */
/*--------------------------------------------------------------
 * Function : Save precise orbit data
 *-------------------------------------------------------------*/
struct PreciseOrbitData
{
    QString SatelliteName;
    double  x;
    double  y;
    double  z;
};

/*--------------------------------------------------------------
 * Function : Save single epoch precise orbit data
 *-------------------------------------------------------------*/
struct EpochPreciseOrbitData
{
    double                    accumulateGpsSec;                 // Accumulate gps seconds from the starting epoch
    MyTime                    myTime;
    vector <PreciseOrbitData> OrbitData_GPS;                    // GPS precise orbit data
    vector <PreciseOrbitData> OrbitData_GLONASS;                // GLONASS precise orbit data
    vector <PreciseOrbitData> OrbitData_BDS;                    // BDS precise orbit data
    vector <PreciseOrbitData> OrbitData_Galileo;
};

/*--------------------------------------------------------------
 * Name     : PrecisionData
 * Function : Deal and save precise orbit data of all epoch
 *-------------------------------------------------------------*/
class PrecisionEphemerisFile : public FileCenter
{
public:
    static  vector <EpochPreciseOrbitData> allPrecionData;      // Save all epoach precise data
    static  int     timeTrans;                                  // Judge if should use BDS time in thses file
public:
    virtual bool    readFile(const QString &filePath);          // Inherit function

private:
    static  int     epochNum;
    static  int     satelliteNum;
    static  double  julianDay;                                  // Julian date in header
    static  QString systemType;                                 // Judge the type of the clk(GPS or GLONASS or Mixed)
    static  QString productType;                                // Judge the type of the clk(igs or code or others)

};


/*                    ****************************************************                                       */
/*******************************      Precise Clock Data(*.clk)      *********************************************/
/*                    ****************************************************                                       */
/*--------------------------------------------------------------
 * Function : Save header information of precision clock file
 *-------------------------------------------------------------*/
struct ClkHeaderInfor /* No use */
{
    QString Rtype;                                              // receiver type
    double  X;                                                   // The location of the receiver
    double  Y;
    double  Z;
};
/*--------------------------------------------------------------
 * Function : Save receiver clock data of precision clock file
 *-------------------------------------------------------------*/
struct AR_clkData  /* No use */
{
    QString Atype;                                              // Used to judge the type(recevier type or satellite type)
    QString Rtype;                                              // The receiver type
    double  dataR_time[6];                                      // The data recording time
    double  recClkDiffer;                                       // The receiver difference
    double  recClkVelocity;                                     // The Velocity of the receiver
    double  GpsSecond;                                          // The GPS second
    double  GpsWeek;                                            // The GPS week
};

/*--------------------------------------------------------------
 * Function : Save satllite clock data of precision clock file
 *-------------------------------------------------------------*/
struct AS_clkData
{
    MyTime  myTime;                                             // All time types
    double  accumulateGpsSec;                                   // Accumulate gps seconds from the starting epoch
    double  clockData_GPS[32]     = {0};
    double  clockData_BDS[32]     = {0};
    double  clockData_GLONASS[32] = {0};
    double  clockData_Galileo[32] = {0};
    double  clockData_J[32]       = {0};
    double  clockData_S[32]       = {0};
    double  satClkDiffer;   /* No use */                        // The clock difference of satellite
    double  satClkVelocity;                                     // The Velocity of the satellite

};

/*--------------------------------------------------------------
 * Name     : PrecisionClockFile
 * Function : Deal and save precise clock data
 *-------------------------------------------------------------*/
class PrecisionClockFile : public FileCenter
{
public:
    virtual bool  readFile(const QString &filePath);            // inherit function

public:
    static  vector<AS_clkData> SatelliteClkData;                // satellite clock data

private:
    static  double  leapSecond;
    static  double  INTERVAL;
    static  QString navigationType;
};

/*                    ****************************************************                                       */
/*******************     Earth rotation parameter information file(*.erp)   **************************************/
/*                    ****************************************************                                       */
/*--------------------------------------------------------------
 * Function : Save earth rotation parameter data
 *-------------------------------------------------------------*/
struct ErpData
{
    MyTime myTime;                                              // used to save MJD and JD
    double xPole;
    double yPole;
    double UT1mUTC;
    double dPsi; /* UnKonwn*/
    double dEps; /* UnKonwn*/
    ErpData()
        :dPsi(0), dEps(0){}
};

/*--------------------------------------------------------------
 * Name     : EarthRotationParameterFile
 * Function : Deal and save precise earth rotation parameter data
 *-------------------------------------------------------------*/
class EarthRotationParameterFile : public FileCenter
{
public:
    virtual bool  readFile(const QString &filePath);            // inherit function
public:
    static  vector <ErpData> allErpData;
};


/*                    ****************************************************                                       */
/*******************************      Ocean tide  Data(*.blq)          *******************************************/
/*                    ****************************************************                                       */
/*--------------------------------------------------------------
 * Function : Save observe value of ocean tide data
 *-------------------------------------------------------------*/
struct ObserveData
{
    QString observeType;                                        // Type name of observe value
    double  observeValue[6] = {0};                              // order: list follow the file order
};
/*--------------------------------------------------------------
 * Function : Save ocean tide data
 *-------------------------------------------------------------*/
struct OceanData
{
    QString stationName;                                        // Site name
    MyTime  myTime;

    double  Comment; /* Unuse */
    double  Longitude;
    double  Latitude;
    double  Height;

    vector <ObserveData> tideData;                              // Ocean tide data
};
/*--------------------------------------------------------------
 * Name     : OceanTideFile
 * Function : Deal and save ocean tide data
 *-------------------------------------------------------------*/
class OceanTideFile : public FileCenter
{
public:
    static  vector <OceanData> allOceanData;
    static  bool               isModeEasy;                      // be set to choose one type of read functions
public:
    virtual bool  readFile(const QString &filePath);            // Inherit function
};

/*                    ****************************************************                                       */
/*******************************    Antenna phase  Data(*.atx)          ******************************************/
/*                    ****************************************************                                       */
/*--------------------------------------------------------------
 * Function : Save Antenna data
 *-------------------------------------------------------------*/
struct AntennaData
{
    QString  antennaType;                                        // Satellite type or Antenna type
    QString  COSPAR_ID;                                          // Committee on Space Research
    QString  calibrationMethod;                                  // Save method of calibration
    QString  agencyName;                                         // Name of agency
    MyTime   myTime;                                             // Only use year, month and day
    QString  sateOneType;                                        // Satellite one Type
    QString  sateTwoType;                                        // Satellite two Type
    int      sateOneNum;                                         // Satellite one number
    int      sateTwoNum;                                         // Satellite two number
    double   ZEN1;
    double   ZEN2;
    double   DAZI;                                               // Azimuth increment
    double   DZEN;                                               // Elevation increment
    double   VALID_FROM;                                         // The start julian date
    double   VALID_UNTIL;                                        // The end julian date
    QString  SINEX_CODE_Type;
    double   SINEX_CODE;
    double   OF_FREQUENCIES;
    double   F1_NEU[3];
    MatrixXd F1_NOAZI;                                           // Save data of frequency
    double   F2_NEU[3];
    MatrixXd F2_NOAZI;                                           // Save data of frequency

    AntennaData()
        : sateOneNum(0),  sateTwoNum(0),
          ZEN1(0.0),      ZEN2(0.0),
          DAZI(0.0),      DZEN(0.0),
          VALID_FROM(0.0),VALID_UNTIL(0.0),
          SINEX_CODE(0.0),OF_FREQUENCIES(0.0)
    {}
};
/*--------------------------------------------------------------
 * Name     : OceanTideFile
 * Function : Deal and save antenna data
 *-------------------------------------------------------------*/
class AntennaInfoFile : public FileCenter
{
public:
   ~AntennaInfoFile();
    virtual bool   readFile(const QString &filePath);            // Inherit function
public:
    static  AntennaData **antennaData_GPS;                       // 1) Use 2D array to conserve memory
    static  AntennaData **antennaData_BDS;                       // 2) Only Galileo need 60 rows, others are 32 rows
    static  AntennaData **antennaData_GLONASS;                   // 3) This array are not have eual length ,some of
    static  AntennaData **antennaData_Galileo;                   //    them has 0 element
    static  vector <AntennaData> antennaData_Other;

private:
    static double      ANTEX_VERSION;
    static QString     SYST;                                     // System type
    static QString     REFANT;
    static QString     PCV_TYPE;
    static QString     productType; /* Unuse*/

};

/*                    ****************************************************                                       */
/****************************    IGS weekly solution  Data(*.snx)          ***************************************/
/*                    ****************************************************                                       */
/*--------------------------------------------------------------
 * Function : Save IGS station coordinate data of  station
 *-------------------------------------------------------------*/
struct StationCoordData
{
    double   date;                                               // Gregory time

    QString  MARKER_NAME;
    Vector3d obsPos;                                             // Positon of observation
};

/*--------------------------------------------------------------
 * Name     : IgsWeeklySolutionFile
 * Function : Deal and save igs weekly solution data
 *-------------------------------------------------------------*/
class IgsWeeklySolutionFile : public FileCenter
{
public:
   ~IgsWeeklySolutionFile(){
        delete [] weeklySolutionData;}
    IgsWeeklySolutionFile(){
        weeklySolutionData = NULL;}
    virtual bool  readFile(const QString &filePath);            // Inherit function
public:
    StationCoordData *weeklySolutionData;
};

/*                    ****************************************************                                       */
/****************************    IGS coordiante Data(*.coord)     ************************************************/
/*                    ****************************************************                                       */
class IgsStationCoordinateFile : public FileCenter
{
public:
    virtual bool  readFile(const QString &filePath);            // Inherit function
public:
    vector <StationCoordData> stationCoordinateData;
};
/*                    ****************************************************                                       */
/****************************    Different Code Bias  Data(*.bsx)          ***************************************/
/*                    ****************************************************                                       */

/*--------------------------------------------------------------
 * Function : Save DCB values of GPS GLONASS Galileo BDS
 *-------------------------------------------------------------*/
struct DCB_GPS
{
    double* C1C_C1W;                                            // DCB values of GPS
    double* C1C_C2W;
    double* C2W_C2S;
    double* C2W_C2L;
    double* C2W_C2X;
    double* C1C_C5Q;
    double* C1C_C5X;
    double* C1W_C2W;
    DCB_GPS();
   ~DCB_GPS();
};

struct DCB_BDS
{
    double* C2I_C7I;                                            // DCB values of BDS
    double* C2I_C6I;
    double* C7I_C6I;
    DCB_BDS();
   ~DCB_BDS();
};

struct DCB_GLONASS
{
    double* C1C_C1P;                                            // DCB values of GLONASS
    double* C1C_C2C;
    double* C1C_C2P;
    double* C1P_C2P;
    double* C2C_C2P;
    DCB_GLONASS();
   ~DCB_GLONASS();
};
struct DCB_GALILEO
{
    double* C1C_C5Q;                                            // DCB values of GALILEO*/
    double* C1X_C5X;
    double* C1C_C7Q;
    double* C1X_C7X;
    double* C1C_C8Q;
    double* C1X_C8X;
    DCB_GALILEO();
   ~DCB_GALILEO();
};
/*--------------------------------------------------------------
 * Name     : DifferentCodeBiasFile
 * Function : Deal and save different code bias data
 *-------------------------------------------------------------*/
class DifferentCodeBiasFile : public FileCenter
{
public:
    virtual bool readFile(const QString &filePath);
public:

    DCB_GPS     *GPS_DCB;
    DCB_BDS     *BDS_DCB;
    DCB_GLONASS *GLONASS_DCB;
    DCB_GALILEO *GALILEO_DCB;
    DifferentCodeBiasFile();
   ~DifferentCodeBiasFile();
};

#endif // DATACENETR_H
