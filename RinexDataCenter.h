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
 * Function : Save precise orbit data
 *-------------------------------------------------------------*/
struct ObservationData
{
    int     PRN;                                                 // Satellite Number
    int     frquencyNum_R;                                       // Frequency number of GLONASS Satellite
    QString sateType;                                            // Satellite Type
    double  sateClock;                                           // Clock errors
    double  extraTime;                                           // Extrapolation time
    double  C1, P1, L1;                                          // Observation data (C, L, P)
    double  C2, P2, L2;
    double  C3, P3, L3;


    double sateVel[3];                                           // The velocity of the satellite
    double satePos[3];                                           // Satellite coordinates
    double coord_DAE[3];                                         // Storage distance, azimuth, elevation angle
    double anteCenterOffset[2];                                  // Receiver antenna phase center offset correction
    double tideCorre;                                            // Tidal correction
    double anteHeight;                                           // Antenna height correction
    double sagnacEffect;                                         // Sagnac effection
    double relativityEffect;                                     // Relativity effection
    double anteCenterCorre;                                      // Satellite antenna phase center deviation correction
    double phaseWinding;                                         // Phase winding correction
    double tropDryDelay;                                         // Tropospheric delay (dry weight)
    double tropWetDelayFunc;                                     // Tropospheric wet delay projection function
    double flagOfValid;                                          // To determine whether the Satellite can be used


    double Delta0; /* Unknown */                                 // This is calculated to PPP*/
    double Delta1;
    double Delta2;

    ObservationData();
};

/*--------------------------------------------------------------
 * Function : Save single epoch precise orbit data
 *-------------------------------------------------------------*/
struct EpochObservationData
{
public:

    MyTime  myTime;                                              // Storage Standard Time
    int     sateNum;                                             // The number of satellites
    vector <ObservationData> epochObserveData;                   // To save observation data in an epoch
};

/*--------------------------------------------------------------
 * Name     : PrecisionData
 * Function : Deal and save precise orbit data of all epoch
 *-------------------------------------------------------------*/
class ObservationFile : public FileCenter
{
public:
    bool readFile(const QString &filePath);

public:
    double   RINEX_VERSION;                                      // RINEX format version number
    QString  MARKER_NAME;                                        // Marker name
    QString  STATION_NAME;
    QString  REC;                                                // Receiver serial number
    QString  REC_TYPE;                                           // Receiver Ttpe
    QString  ANT;                                                // Antenna serial number
    QString  ANT_TYPE;                                           // Antenna Type
    double   APPROX_POSITION[3];                                 // Receiver outline coordinate
    double   ANTENNA_DELTA_H;                                    // Antenna height: above the mark at the height of the antenna surface*/
    double   ANTENNA_DELTA_E;                                    // Antenna center eccentric relative to the mark-up in the East*/
    double   ANTENNA_DELTA_N;                                    // Center of the antenna relative to the mark-up in the North eccentric*/
    double   TYPES_NUMBER;                                       // Number of types
    double   INTERVAL;                                           // Sampling rate

    int      GPS_ObserveNum;                                     // Number of observed value types
    int      BDS_ObserveNum;
    int      GLONASS_ObserveNum;
    int      Galileo_ObserveNum;
    int      QZSS_ObserveNum;
    int      SBAS_ObserveNum;

    QString pathname;  /* unknown*/ /*Output path*/

    QVector <EpochObservationData> AllObservationData;           // Data stored for all epoch
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
/*******************************      Rinex observe file(*.o)    *************************************************/
/*                    ****************************************************                                       */

//The following section properties are GPS/BDS navigation file sharing section
struct DataBlockOne
{

    double af0;                        //the clock offset(sec)
    double af1;                        //the clock rate (sec/sec)
    double af2;                        //the clock acceleration (sec/sec^2)

                                       //Broadcast Track 1
    double adoe;                       //age of ephemeris entry (sec)i.e, how long ago was ituploaded
    double crs;                        //radius corrections (m)
    double deltan;                     //correction to mean motion (radian/sec)
    double M0;                         //Mean anomaly(radians)

                                       //Broadcast Track 2
    double cuc;                        //correction to argument in latitude (rad)
    double ecc;                        //eccentricity
    double cus;                        //correction to argument in latitude (rad)
    double art;                        //square root of semi-major axis (m^.5)

                                       //Broadcast Track 3
    double toe;                        //time of ephemeris
    double cic;                        //corrections to inclination
    double Omega0;                     //longitude of the ascending node (rad) (Capital omega)
    double cis;                        //corrections to inclination

                                       //Broadcast Track 4
    double i0;                         //inclination (rads)
    double crc;                        //radius corrections (m)
    double omega;                      //argument of perigee (rad) (lower case omega)
    double Omegadot;                   //time derivative of longitude of the ascending node(rad/sec)

                                       //Broadcast Track 5
    double idot;                       //time derivative of inclination (rads/sec)
    double cflg12;                     //Flags (whose meaning is not clear)
    double weekno;                     //GPS Week Number
    double pflg12;                     //Flags (whose meaning is not clear)

                                       //Broadcast Track 6
    double svaccur;                    //range accuracy (m)
    double svhealth;                   //satellite health flag
    double tgd;                        //Group delay L2 bias(word 7 subframe 1)
    double aodc;                       //age of clock parameter upload(sec)

                                       //Broadcast Track 7
    double transmit;                   //Transmission time seconds of GPS week
};
 //This part of the property below this exclusive GLONASS navigation files
struct DataBlockTwo
{
    double TauN;                       //Satellite clock error (s)
    double GammaN;                     //Satellite relative frequency deviation
    double TK;                         //Message frame time (tk)

                                       //Broadcast Track 1
    double X;                          //Satellite position X (km)
    double X_dot;                      //Satellite velocity X (X dot) (km / s)
    double AC_X;                       //Acceleration satellite X direction (km / s2)
    double Bn;                         //Satellite health status (0 = OK) (Bn)

                                       //Broadcast Track 2
    double Y;                          //Satellite position Y (km)
    double Y_dot;                      //Satellite velocity Y (Y dot) (km / s)
    double AC_Y;                       //Acceleration satellite Y direction (km / s2)
    int K;                             //Satellite frequency number (1 to 24)

                                       //Broadcast Track 3
    double Z;                          //Satellite position Z (km)
    double Z_dot;                      //Satellite velocity Z (Z dot) (km / s)
    double AC_Z;                       //Z direction acceleration satellite (km / s2)
    double E;                          //Run life information (days) (E)
};

struct epochNavigationData
{
public:
    epochNavigationData();
   ~epochNavigationData();

    QString sats;                      //the type and the PRN number of satellite
    int     PRN;                       //the PRN number of satellite
                                       //year,month,day,hour,minute,second is the epoch to
                                       //which the clock parameters follow apply
    MyTime  myTime;
    void   *dataPtr;                    // point to block one or two
};


class NavigationFile
{
    QVector < QVector< epochNavigationData> >  Gps; /*In order to store navigation file information*/
    QVector < QVector< epochNavigationData> >  r;          //  看看是不是 一共40个空；

    int K[30];                                            /*Satellite frequency number (1 to 24)(GLONASS)*/
    QString navtype;
    QString clocktype;

    QString troptype;
    QString Tropospheric_Model;
    QString Tropospheric_Mapping_Function;
};
#endif // RINEXDATACENTER_H
