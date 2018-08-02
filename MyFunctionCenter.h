#ifndef MYFUNCIONCENTER_H
#define MYFUNCIONCENTER_H
#include <cmath>
#include "ProductDataCenter.h"


#define SEC_DAY 1.0 / 86400.0

class MyFuncionCenter
{
public:
    /*--------------------- Time convert functions ----------------------------------------------*/
    static MyTime   timeIntegrator(const MyTime &myTime);
    static MyTime   timeIntegrator(const EpochTime &Gre);
    static MyTime   timeIntegrator(const GpsTime   &Gps);
    static MyTime   timeIntegrator(const double    &JD );

    static double Gre_to_JD (const EpochTime &Gre);
    static double Gps_to_JD (const GpsTime   &Gps);
    static double Gre_to_Doy(const EpochTime &Gre);
    static void   JD_to_Gre (const double    &JD  , EpochTime &Gre);
    static void   JD_to_Gps (const double    &JD  , GpsTime   &Gps);

    /*--------------------- Time system convert functions ----------------------------------------------*/
    static int    leapSecond_TAI_UTC(const double    &JD);
    static MyTime GPS_to_UTC(const MyTime &gpsTime);
    static MyTime GPS_to_BDS(const MyTime &gpsTime);
    static MyTime UTC_to_TDT(const MyTime &utcTime);
    static MyTime UTC_to_SID(const MyTime &utcTime); /* Unknown */
};

#endif // MYFUNCIONCENTER_H
