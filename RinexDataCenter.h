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
    int       PRN;                                               // Satellite Number
    int       frquencyNum_R;                                     // Frequency number of GLONASS Satellite
    QString   sateType;                                          // Satellite Type
    double    sateClock;                                         // Clock errors
    double    extraTime;                                         // Extrapolation time
    double    C1;                                                // Observation data (C, L, P)
    double    P1;
    double    L1;
    double    C2;
    double    P2;
    double    L2;
    double    C3;
    double    P3;
    double    L3;


    Vector3d  sateVel;                                           // The velocity of the satellite
    Vector3d  satePos;                                           // Satellite coordinates
    Vector3d  coord_DAE;                                         // Storage distance, azimuth, elevation angle
    Vector2d  anteCenterOffset;                                  // Receiver antenna phase center offset correction
    double    tideCorre;                                         // Tidal correction
    double    anteHeight;                                        // Antenna height correction
    double    sagnacEffect;                                      // Sagnac effection
    double    relativityEffect;                                  // Relativity effection
    double    anteCenterCorre;                                   // Satellite antenna phase center deviation correction
    double    phaseWinding;                                      // Phase winding correction
    double    tropDryDelay;                                      // Tropospheric delay (dry weight)
    double    tropWetDelayFunc;                                  // Tropospheric wet delay projection function
    double    flagOfValid;                                       // To determine whether the Satellite can be used


    double    Delta0; /* Unknown */                              // This is calculated to PPP*/
    double    Delta1;
    double    Delta2;

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

    //    double julday;                        // Julian Day
    //    double mjd;
    //    double gpst;                          /*GPS time*/
    //    double week;                          /*GPS Week*/
    //    double doy;                           /*Day of year*/

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
    Vector3d APPROX_POSITION;                                    // Receiver outline coordinate
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



    QString pathname;  /* unknown*/                                         /*Output path*/


//    int C1;
//    int P1;
//    int L1;
//    int C2;
//    int P2;
//    int L2;
//    int C3;
//    int P3;
//    int L3;

    QVector <EpochObservationData> AllObservationData;           // Data stored for all epoch

};
#endif // RINEXDATACENTER_H
