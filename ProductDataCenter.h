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
    Sat_TypePrn satTN;
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
    vector <EpochPreciseOrbitData> allPrecionData;              // Save all epoach precise data
    static int    timeTrans;                                    // Judge if should use BDS time in thses file
public:
    virtual bool  readFile();
    virtual void  setFilePath(const QString &FilePath){
                  filePath = FilePath;}
    static  int   getSatIndex(const vector<PreciseOrbitData> &aimArray,
                              const int &aimPrn);
private:
    int     epochNum;
    int     satelliteNum;
    double  julianDay;                                          // Julian date in header
    QString systemType;                                         // Judge the type of the clk(GPS or GLONASS or Mixed)
    QString productType;                                        // Judge the type of the clk(igs or code or others)

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
    double  X;                                                  // The location of the receiver
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
    double  clockData_QZSS[32]    = {0};
    double  clockData_SBAS[32]    = {0};

    double  satClkDiffer;   /* Un use */                        // The clock difference of satellite
    double  satClkVelocity;                                     // The Velocity of the satellite

};

/*--------------------------------------------------------------
 * Name     : PrecisionClockFile
 * Function : Deal and save precise clock data
 *-------------------------------------------------------------*/
class PrecisionClockFile : public FileCenter
{
public:
    virtual bool  readFile();                                   // inherit function
    virtual void  setFilePath(const QString &FilePath){
                  filePath = FilePath;}
public:
    vector<AS_clkData> satelliteClkData;                        // satellite clock data
    double  leapSecond;
    double  INTERVAL;
    QString navigationType;
};

/*                    ****************************************************                                       */
/*******************     Earth rotation parameter information file(*.erp)   **************************************/
/*                    ****************************************************                                       */
/*--------------------------------------------------------------
 * Function : Save earth rotation parameter data
 *-------------------------------------------------------------*/
struct ErpData
{
    MyTime myTime;                                              // Used to save MJD and JD
    double xPole;
    double yPole;
    double UT1mUTC;
    double dPsi; /* UnKonwn*/
    double dEps; /* UnKonwn*/
    ErpData()
        :dPsi(0), dEps(0){}

    static MyTime baseTimeJ2000;
};

/*--------------------------------------------------------------
 * Name     : EarthRotationParameterFile
 * Function : Deal and save precise earth rotation parameter data
 *-------------------------------------------------------------*/
class EarthRotationParameterFile : public FileCenter
{
public:
    virtual bool  readFile();                                   // inherit function
    virtual void  setFilePath(const QString &FilePath){
                  filePath = FilePath;}

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
    vector <OceanData> allOceanData;
    static  bool       isModeEasy;                              // be set to choose one type of read functions
public:
    virtual bool  readFile();                                   // Inherit function
    virtual void  setFilePath(const QString &FilePath){
                  filePath = FilePath;}
};

/*                    ****************************************************                                       */
/*******************************    Antenna phase  Data(*.atx)          ******************************************/
/*                    ****************************************************                                       */
/*--------------------------------------------------------------
 * Function : Save Antenna data
 *-------------------------------------------------------------*/
struct AntennaData
{
    QString  antennaType;                                       // Satellite type or Antenna type
    QString  COSPAR_ID;                                         // Committee on Space Research
    QString  calibrationMethod;                                 // Save method of calibration
    QString  agencyName;                                        // Name of agency
    MyTime   myTime;                                            // Only use year, month and day
    QString  sNNType;                                           // Satellite system flag
    QString  sNNNType;                                          // satellite system flag
    int      sNN;                                               // Satellite PRN number
    int      sNNN;                                              // SVN number (GPS), GLONASS number, GSAT number (Galileo) or SVN number (QZSS); blank (Compass, SBAS)
    double   ZEN1;                                              // Zenith distance
    double   ZEN2;
    double   DAZI;                                              // Azimuth increment 360°must be divisible by 'DAZI'(0 - 360)
    double   DZEN;                                              // Elevation increment 'ZEN1' and 'ZEN2' always have to be multiples of 'DZEN' (ZEN1 - ZEN2)
    double   VALID_FROM;                                        // The start date
    double   VALID_UNTIL;                                       // The end date
    QString  FREQUENCY_Type;                                    // Name of antenna calibration model to be used in the  SINEX format
    double   FREQUENCY_CODE;
    double   OF_FREQUENCIES;                                    // Number of frequencies
    Vector3d F1_NEU;
    MatrixXd F1_NOAZI;                                          // Save data of frequency
    Vector3d F2_NEU;
    MatrixXd F2_NOAZI;                                          // Save data of frequency

    AntennaData()
        : sNN (0),            sNNN(0),
          ZEN1(0.0),          ZEN2(0.0),
          DAZI(0.0),          DZEN(0.0),
          VALID_FROM(0.0),    VALID_UNTIL(0.0),
          FREQUENCY_CODE(0.0),OF_FREQUENCIES(0.0)
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
    AntennaInfoFile(){
        antennaData_GPS     = NULL,  antennaData_GLONASS = NULL;
        antennaData_BDS     = NULL,  antennaData_Galileo = NULL;
    }
    virtual bool  readFile();                                   // Inherit function
    virtual void  setFilePath(const QString &FilePath){
                  filePath = FilePath;}

public:
    vector <AntennaData> antennaData_Other;                     // Save antenna data of receiver, SBAS, and JAZZ

    vector <AntennaData> **antennaData_GPS;                     // 1) Use 2D array to conserve memory
    vector <AntennaData> **antennaData_BDS;                     // 2) Only Galileo need 60 rows, others are 32 rows
    vector <AntennaData> **antennaData_GLONASS;                 // 3) This array are not have eual length ,some of
    vector <AntennaData> **antennaData_Galileo;                 //    them has 0 element

private:
    double      ANTEX_VERSION;
    QString     SYST;                                           // System type (G, R, ... M)
    QString     REFANT;                                         // Reference antenna type for relative values (blank : AOAD/M_T)
    QString     PCV_TYPE;                                       // A absolute values \ R relative values
};

/*                    ****************************************************                                       */
/****************************    IGS weekly solution  Data(*.snx)       ******************************************/
/*                    ****************************************************                                       */
/*--------------------------------------------------------------
 * Function : Save IGS station coordinate data of  station
 *-------------------------------------------------------------*/
struct StationCoordData
{
    double   date;                                              // Gregory time

    QString  MARKER_NAME;
    Vector3d obsPos;                                            // Positon of observation
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
    virtual bool  readFile();
    virtual void  setFilePath(const QString &FilePath){
                  filePath = FilePath;}
public:
    StationCoordData *weeklySolutionData;
};

/*                    ****************************************************                                       */
/****************************    IGS coordiante Data(*.coord)     ************************************************/
/*                    ****************************************************                                       */
class IgsStationCoordinateFile : public FileCenter
{
public:
    virtual bool  readFile();                                     // Inherit function
    virtual void  setFilePath(const QString &FilePath){
                  filePath = FilePath;}
public:
    vector <StationCoordData> stationCoordinateData;
};
/*                    ****************************************************                                       */
/****************************    Different Code Bias  Data(*.bsx)     ********************************************/
/*                    ****************************************************                                       */

/*--------------------------------------------------------------
 * Function : Save DCB values of GPS GLONASS Galileo BDS
 *-------------------------------------------------------------*/
struct DCB_GPS
{
    double* C1C_C1W;                                             // DCB values of GPS
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
    double* C2I_C7I;                                             // DCB values of BDS
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
    virtual bool readFile();
    virtual void setFilePath(const QString &FilePath){
                 filePath = FilePath;}
public:

    DCB_GPS     *GPS_DCB;
    DCB_BDS     *BDS_DCB;
    DCB_GLONASS *GLONASS_DCB;
    DCB_GALILEO *GALILEO_DCB;
    DifferentCodeBiasFile();
   ~DifferentCodeBiasFile();
};

/*                    ****************************************************                                       */
/****************************    Sun and moon position data       ************************************************/
/*                    ****************************************************                                       */
/*--------------------------------------------------------------
 * Function : Save sun and moon position data
 *-------------------------------------------------------------*/
struct SunMoonPosition
{
    double   accumulateGpsSec;
    Vector3d sunPosition;                                       // The coordinates of the sun
    Vector3d moonPosition;                                      // The coordinates of the moon
};

extern vector<SunMoonPosition> sunMoonPos;                      // Define a global vector
#endif // DATACENETR_H
