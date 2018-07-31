#ifndef DATACENETR_H
#define DATACENETR_H
#include <vector>
#include <Eigen/Eigen>

using namespace std;
using namespace Eigen;
#include "filecenter.h"

#define JDtoMJD 2400000.5
/*                    ****************************************************                                       */
/*******************************         Public  Data           **************************************************/
/*                    ****************************************************                                       */
/*--------------------------------------------------------------
 * Function : Save single epoch time
 *-------------------------------------------------------------*/
struct EpochTime
{
    int    year;
    int    month;
    int    day;
    int    hour;
    int    minute;
    double second;

    EpochTime()
        : year(-1), month (-1), day   (-1),
          hour(-1), minute(-1), second(-1){}
    EpochTime(int Y, int M, int D,
              int h, int m, int s)
        : year(Y), month (M), day   (D),
          hour(h), minute(m), second(s){}
};
/*--------------------------------------------------------------
 * Function : Save GPS time
 *-------------------------------------------------------------*/
struct GpsTime
{
    int    week;
    double sec;
    GpsTime()
        :  week(-1), sec(-1) {}
};
/*--------------------------------------------------------------
 * Function : Save all time types
 *-------------------------------------------------------------*/
struct MyTime
{
    double    JD;                                                // Julian Date
    double    MJD;                                               // Modified Julian Date
    double    DOY;                                               // DOY-Day Of Year
    EpochTime EPT;                                               // Epoch Time (gregorian calendar)
    GpsTime   GPT;                                               // GPS Time

    MyTime()
        :JD(-1), MJD(-1), DOY(-1) {}

};
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
    QString  antennaType;                                        // satellite type or Antenna type
    QString  COSPAR_ID;                                          // Committee on Space Research
    QString  calibrationMethod;                                  // Save method of calibration
    QString  agencyName;                                         // Name of agency
    MyTime   myTime;                                             // Only use year, month and day
    QString  sateOneType;                                        // satellite one Type
    QString  sateTwoType;                                        // satellite two Type
    int      sateOneNum;                                         // satellite one number
    int      sateTwoNum;                                         // satellite two number
    double   DAZI;                                               // Azimuth increment
    double   ZEN1;
    double   ZEN2;
    double   DZEN;                                               // Elevation increment
    double   OF_FREQUENCIES;
    double   VALID_FROM;                                         // The start julian date
    double   VALID_UNTIL;                                        // The end julian date

    QString  SINEX_CODE_Type;
    double   SINEX_CODE;
    double   F1_NEU[3];
    MatrixXd F1_NOAZI;                                           // Save data of frequency
    double   F2_NEU[3];
    MatrixXd F2_NOAZI;                                           // Save data of frequency

};
/*--------------------------------------------------------------
 * Name     : OceanTideFile
 * Function : Deal and save antenna data
 *-------------------------------------------------------------*/
class AntennaInfoFile
{
public:
    virtual bool   readFile(const QString &filePath);            // Inherit function
public:
    static  AntennaData antennaData_GPS[32][3];
    static  AntennaData antennaData_BDS[32][3];
    static  AntennaData antennaData_GLONASS[32][3];
    static  AntennaData antennaData_Galileo[60][3];
    static  vector <AntennaData> antennaData_Other;

private:
    static double      ANTEX_VERSION;
    static QString     SYST;                                     // System type
    static QString     REFANT;
    static QString     PCV_TYPE;
    static QString     productType; /* Unuse*/


};

#endif // DATACENETR_H
