#ifndef RINEXDATACENTER_H
#define RINEXDATACENTER_H

#include <vector>
#include <Eigen/Eigen>

using namespace std;
using namespace Eigen;
#include "FileCenter.h"
#include "PublicDataCenter.h"

/*                    ****************************************************                                       */
/*******************************      Rinex observe file(*.o)    *************************************************/
/*                    ****************************************************                                       */
/*--------------------------------------------------------------
 * Function : Save observation data
 *-------------------------------------------------------------*/
struct ObservationData
{
    Sat_TypePrn satTN;                                           // Satellite Type and Satellite Number
    double  C1, P1, L1;                                          // Observation data (C, L, P)
    double  C2, P2, L2;
    double  C3, P3, L3;
    ObservationData();
};

/*--------------------------------------------------------------
 * Function : Save single epoch observation data
 *-------------------------------------------------------------*/
struct EpochObservationData
{
public:

    MyTime  myTime;                                              // Storage Standard Time
    int     sateNum;                                             // The number of satellites
    int     GPS_Num;
    int     BDS_Num;
    int     GLONASS_Num;
    int     Galileo_Num;
    int     QZSS_Num;
    int     SBAS_Num;
    vector <ObservationData> epochObserveData;                   // To save observation data in an epoch
    EpochObservationData()
        : sateNum(0),     GPS_Num(0),     BDS_Num(0),
          GLONASS_Num(0), Galileo_Num(0), QZSS_Num(0),
          SBAS_Num(0){}
};

/*--------------------------------------------------------------
 * Name     : ObservationFile
 * Function : Deal and save observation data of all epoch
 *-------------------------------------------------------------*/
class ObservationFile : public FileCenter
{
public:
    virtual bool readFile();
    virtual void setFilePath(const QString &FilePath){
                 filePath = FilePath;}

public:
    double   RINEX_VERSION;                                      // RINEX format version number
    QString  MARKER_NAME;                                        // Marker name
    QString  STATION_NAME;
    QString  REC;                                                // Receiver serial number
    QString  REC_TYPE;                                           // Receiver Ttpe
    QString  ANT;                                                // Antenna serial number
    QString  ANT_TYPE;                                           // Antenna Type
    QString  navigationType;
    double   APPROX_POSITION[3];                                 // Receiver outline coordinate
    double   ANTENNA_DELTA_H;                                    // Antenna height: above the mark at the height of the antenna surface
    double   ANTENNA_DELTA_E;                                    // Antenna center eccentric relative to the mark-up in the East
    double   ANTENNA_DELTA_N;                                    // Center of the antenna relative to the mark-up in the North eccentric
    double   TYPES_NUMBER;                                       // Number of types
    double   INTERVAL;                                           // Sampling rate

    int      GPS_ObserveNum;                                     // Number of observed value types
    int      BDS_ObserveNum;
    int      GLONASS_ObserveNum;
    int      Galileo_ObserveNum;
    int      QZSS_ObserveNum;
    int      SBAS_ObserveNum;


    vector <EpochObservationData> AllObservationData;            // Data stored for all epoch
    ObservationFile()
    {
        RINEX_VERSION   = 0, ANTENNA_DELTA_H = 0;
        ANTENNA_DELTA_E = 0, ANTENNA_DELTA_N = 0;
        TYPES_NUMBER    = 0, INTERVAL        = 0;
        GPS_ObserveNum  = 0, BDS_ObserveNum  = 0;
        QZSS_ObserveNum = 0, SBAS_ObserveNum = 0;
        GLONASS_ObserveNum = 0;
        Galileo_ObserveNum = 0;

    }

};


/*                    ****************************************************                                       */
/*******************************      Rinex navigation file(*.*)    **********************************************/
/*                    ****************************************************                                       */

/*-----The following section properties are GPS/BDS navigation file sharing section                              */
struct DataBlockOne
{
    double af0;                                                  // The clock offset(sec)
    double af1;                                                  // The clock rate (sec/sec)
    double af2;                                                  // The clock acceleration (sec/sec^2)
 /*...................................Broadcast Track 1..........*/

    double IODE;/*aode is BDS*/                                  // Age of ephemeris entry (sec)i.e, how long ago was ituploaded
    double Crs;                                                  // Radius corrections (m)
    double Delta_n;                                              // Correction to mean motion (radian/sec)
    double M0;                                                   // Mean anomaly(radians)
 /*...................................Broadcast Track 2..........*/

    double Cuc;                                                  // Correction to argument in latitude (rad)
    double ecc;                                                  // Eccentricity
    double Cus;                                                  // Correction to argument in latitude (rad)
    double sqrt_a;                                               // Square root of semi-major axis (m^.5)
 /*...................................Broadcast Track 3..........*/

    double TOE;                                                  // Time of ephemeris
    double Cic;                                                  // Corrections to inclination
    double Omega0;                                               // Longitude of the ascending node (rad) (Capital omega)
    double Cis;                                                  // Corrections to inclination
 /*...................................Broadcast Track 4..........*/

    double i0;                                                   // Inclination (rads)
    double Crc;                                                  // Radius corrections (m)
    double omega;                                                // Argument of perigee (rad) (lower case omega)
    double Omegadot;                                             // Time derivative of longitude of the ascending node(rad/sec)
 /*...................................Broadcast Track 5..........*/

    double idot;                                                 // Time derivative of inclination (rads/sec)
    double cflgL2;                                               // Flags (whose meaning is not clear)
    double weekNum;                                              // GPS Week Number
    double pflgL2;                                               // Flags (whose meaning is not clear)
 /*...................................Broadcast Track 6..........*/

    double sAccur;                                               // Range accuracy (m)
    double sHealth;                                              // Satellite health flag
    double TGD;                                                  // Group delay L2 bias(word 7 subframe 1)
    double IODC;                                                 // Age of clock parameter upload(sec)
 /*...................................Broadcast Track 7..........*/

    double transmitTime;                                         // Transmission time seconds of GPS week
};
/*-----The following section properties are GPS/BDS navigation file sharing section                              */
struct DataBlockTwo
{
    double TauN;                                                 // Satellite clock error (s)
    double GammaN;                                               // Satellite relative frequency deviation
    double TK;                                                   // Message frame time (tk)
 /*...................................Broadcast Track 1..........*/

    double X;                                                    // Satellite position X (km)
    double X_dot;                                                // Satellite velocity X (X dot) (km / s)
    double AC_X;                                                 // Acceleration satellite X direction (km / s2)
    double Bn;                                                   // Satellite health status (0 = OK) (Bn)
 /*...................................Broadcast Track 1..........*/

    double Y;                                                    // Satellite position Y (km)
    double Y_dot;                                                // Satellite velocity Y (Y dot) (km / s)
    double AC_Y;                                                 // Acceleration satellite Y direction (km / s2)
    int    K;                                                    // Satellite frequency number (1 to 24)
 /*...................................Broadcast Track 1..........*/

    double Z;                                                    // Satellite position Z (km)
    double Z_dot;                                                // Satellite velocity Z (Z dot) (km / s)
    double AC_Z;                                                 // Z direction acceleration satellite (km / s2)
    double E;                                                    // Run life information (days) (E)
};
/*--------------------------------------------------------------
 * Function : Save single epoch navigation data
 *-------------------------------------------------------------*/
struct EpochNavigationData
{
public:
    EpochNavigationData()
        :dataOnePtr(NULL), dataTwoPtr(NULL){}

    Sat_TypePrn    satTN;
    MyTime         myTime;
    DataBlockOne  *dataOnePtr;                                   // The pointer point to data block one or two
    DataBlockTwo  *dataTwoPtr;
};

/*--------------------------------------------------------------
 * Name     : NavigationFile
 * Function : Deal and save navigation data of all epoch
 *-------------------------------------------------------------*/
class NavigationFile :  public FileCenter
{
public:
    virtual bool readFile();
    virtual void setFilePath(const QString &FilePath){
                 filePath = FilePath;}
    void saveNavDataToArray(const EpochNavigationData &navData); // Can be use to push data into srray

public:
    vector < vector< EpochNavigationData> >  GPS;                // In order to store navigation file information
    vector < vector< EpochNavigationData> >  BDS;
    vector < vector< EpochNavigationData> >  GLONASS;
    vector < vector< EpochNavigationData> >  GALILEO;
    vector < vector< EpochNavigationData> >  QZSS;
    vector < vector< EpochNavigationData> >  SBAS;
    vector < vector< EpochNavigationData> >  IRNSS;

    int K[30];                                                   // Satellite frequency number (1 to 24)(GLONASS)*/

};
#endif // RINEXDATACENTER_H
