#ifndef MYFUNCIONCENTER_H
#define MYFUNCIONCENTER_H
#include <math.h>
#include "GlobalDefinationCenter.h"
#include "ProductDataCenter.h"

/*-------------------------------------------------------------------------------------
 * Name     : MyFuncionCenter
 * Function : Save all functions personaly write
 *------------------------------------------------------------------------------------*/
class MyFunctionCenter
{
public:
    /*--------------------- Time convert functions -----------------------------------*/
    static MyTime   timeIntegrator(const MyTime &myTime);
    static MyTime   timeIntegrator(const EpochTime &Gre);
    static MyTime   timeIntegrator(const GpsTime   &Gps);
    static MyTime   timeIntegrator(const double    &JD );

    static double Gre_to_JD (const EpochTime &Gre);
    static double Gps_to_JD (const GpsTime   &Gps);
    static double Gre_to_Doy(const EpochTime &Gre);
    static void   JD_to_Gre (const double    &JD  , EpochTime &Gre);
    static void   JD_to_Gps (const double    &JD  , GpsTime   &Gps);

    /*--------------------- Time system convert functions ----------------------------*/
    static int    leapSecond_TAI_UTC(const double    &JD);                            // Get leap seconds between TAI and UTC
    static MyTime GPS_to_UTC(const MyTime &gpsTime);
    static MyTime GPS_to_BDS(const MyTime &gpsTime);
    static MyTime UTC_to_TDT(const MyTime &utcTime);
    static double TDT_to_SID(const MyTime &tdtTime); /* Unknown */


    /*--------------------- Coordiante functions -------------------------------------*/
    static double getCoordDistance(const double   xyz[]);                             // Note: BLH  [0]:B [1]:L [2]:H
    static double getCoordDistance(const Vector3d &xyz);                              //       ENU  [0]:E [1]:N [2]:U
    static void   getCoordSysParameter(const CoordSysName &coordName,                 //       XYZ  [0]:X [1]:Y [2]:Z
                                       CoordSysParameter  &coordPar);                 //       ESU  [0]:E [1]:S [2]:U
                                                                                      //       DAZ  [0]:D [1]:A [2]:Z
    static double   getN(const double &a, const double &e2, double B);                // Get radius of curvature in prime vertical
    static double   getAzimuth(const double &delatX,  const double &delatY);
    static Vector3d getUnitVector(const Vector3d &XYZ);
    static Vector3d getUnitVector(const double XYZ[]);
    static Vector3d BLHToXYZ(const Vector3d     &BLH,
                             const CoordSysParameter  &coordPar);
    static Vector3d XYZToBLH(const Vector3d     &XYZ,
                             const CoordSysParameter &coordPar);
    static Vector3d XYZToENU(const Vector3d     &recXYZ, const Vector3d &satXYZ,
                             const CoordSysParameter &coordPar);
    static Vector3d ENUToDAZ(const Vector3d     &satENU);

    static Vector3d CIStoCTS(const Vector3d     &posCIS, const MyTime   &tdtTime);
    /*--------------------- Mathmatical functions ------------------------------------*/
    static double  linerInter(double x0, double y0, double x,                         // Linear interpolation
                              double x1, double y1);
    static double  lagrangeInter(const   double xVector[], const double yVector[],
                                 const   double &x,        const int    &order);
    static X_Y_Z   lagrangeInter(const   double tVector[], const double xVector[],
                                 const   double yVector[], const double zVector[],
                                 const   double &t,        const int    &order);

    static void    kalmanFilter(MatrixXd  I, MatrixXd  F, MatrixXd R,
                                MatrixXd  B, VectorXd  L, MatrixXd Qw,
                                MatrixXd &Q, VectorXd &X, VectorXd &V);

    /*--------------------- Other functions ------------------------------------------*/
    static void getFrequency(const Sat_TypePrn &satTN, int* K, vector<double> &frequency);// Get wave frequency of differeny system
    static double  degTorad(double  deg, double  min, double  sec)
    {
        double rad = (deg / 180.0 * PI) +
                     (min / 180.0 / 60 * PI) +
                     (sec / 180.0 / 3600 * PI);
        return rad;
    }
    static void    radTodeg(double &deg, double &min, double &sec, double rad)
    {
        deg = int(rad / PI * 180);
        min = int(rad / PI * 180 * 60 - deg * 60);
        sec =    (rad / PI * 180 * 3600 - deg* 3600 - min*60);
    }

    static Matrix3d getMatrixToENU(const Vector3d          &BLH);
    static Matrix3d getMatrixToENU(const CoordSysParameter &coordPar, const Vector3d  &XYZ);
    static int isSatFound(vector <SatQuery> satContaine, const SatQuery &aimSat);     // Determine whether the target satellite has been found.

};

#endif // MYFUNCIONCENTER_H
