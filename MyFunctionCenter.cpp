#include "MyFunctionCenter.h"

/*                    ****************************************************                                       */
/*--------------------------------------- Time convert ----------------------------------------------------------*/
/*                    ****************************************************                                       */
/*------------------------------------------------------------------------------
 * Name     : timeIntegrator (1)
 * Function : Get all kinds of time
 * Input    : const MyTime &myTime
 * Output   : MyTime (if convert success)
 *-----------------------------------------------------------------------------*/
MyTime MyFunctionCenter::timeIntegrator(const MyTime &myTime)
{
    MyTime tmpTime;
    if (myTime.JD  > 0)
    {
        tmpTime    = timeIntegrator(myTime.JD);
        return tmpTime;
    }
    if (myTime.MJD > 0)
    {
        tmpTime.JD = myTime.MJD + JDtoMJD;
        tmpTime    = timeIntegrator(myTime.JD);
        return tmpTime;
    }
    if (myTime.EPT.year >  0 && myTime.EPT.month  >  0 && myTime.EPT.day    >  0 &&
        myTime.EPT.hour >= 0 && myTime.EPT.minute >= 0 && myTime.EPT.second >= 0)
    {
        tmpTime    = timeIntegrator(myTime.EPT);
        return tmpTime;
    }
    if (myTime.GPT.week > 0 && myTime.GPT.sec > 0)
    {
        tmpTime    = timeIntegrator(myTime.GPT);
        return tmpTime;
    }
    return tmpTime;                                                             // This return has no means
}

/*------------------------------------------------------------------------------
 * Name     : timeIntegrator (2)
 * Input    : const EpochTime &Gre
 *-----------------------------------------------------------------------------*/
MyTime MyFunctionCenter::timeIntegrator(const EpochTime &Gre)
{
     MyTime tempTime;
     tempTime.EPT = Gre;
     tempTime.JD  = Gre_to_JD (Gre);
     JD_to_Gps(tempTime.JD,  tempTime.GPT);
     tempTime.DOY = Gre_to_Doy(Gre);
     tempTime.MJD = tempTime.JD  - JDtoMJD;
     return tempTime;
}
/*------------------------------------------------------------------------------
 * Name     : timeIntegrator (3)
 * Input    : const GpsTime   &Gps
 *-----------------------------------------------------------------------------*/
MyTime MyFunctionCenter::timeIntegrator(const GpsTime   &Gps)
{
    MyTime tempTime;
    tempTime.GPT = Gps;
    tempTime.JD  = Gps_to_JD (Gps);
    JD_to_Gre(tempTime.JD, tempTime.EPT);
    tempTime.DOY = Gre_to_Doy(tempTime.EPT);
    tempTime.MJD = tempTime.JD  - JDtoMJD;
    return tempTime;
}
/*------------------------------------------------------------------------------
 * Name     : timeIntegrator (4)
 * Input    : const double    &JD
 *-----------------------------------------------------------------------------*/
MyTime MyFunctionCenter::timeIntegrator(const double    &JD )                   // MJD can not do this
{
    MyTime tempTime;
    tempTime.JD  =  JD;
    JD_to_Gre(JD,  tempTime.EPT);
    JD_to_Gps(JD,  tempTime.GPT);
    tempTime.DOY = Gre_to_Doy(tempTime.EPT);
    tempTime.MJD = tempTime.JD  - JDtoMJD;
    return tempTime;
}

/**the conversion is only valid in the time span
*from March 1900 to February 2100
* the jd of them respectively are 2.415078500000000e+006 and 2.488099500000000e+006.
*  mjd   */
/*------------------------------------------------------------------------------
 * Name     : Gre_to_JD
 * Function : Transform gregorian calendar into Julian date
 * Input    : const EpochTime &Gre
 * Output   : double (result of JD)
 *-----------------------------------------------------------------------------*/
double MyFunctionCenter::Gre_to_JD(const EpochTime &Gre)
{
    double UT = Gre.hour + Gre.minute/60.0 + Gre.second/3600.0;

    int y,m;
       if (Gre.month <= 2){
           y = Gre.year  - 1 ;
           m = Gre.month + 12;
       } else {
           y = Gre.year;
           m = Gre.month;
       }
    double JD = (int)(365.25 * y) + (int)(30.6001 * (m + 1)) +
                 Gre.day + UT / 24 + 1720981.5;
    return JD;
}

/*-----------------------------------------------------------------------------
 * Function : Judge if the year is a leap year
 * ---------------------------------------------------------------------------*/
static bool isLeapYear(int year)
{
    if ((year % 4   == 0 && year%100 != 0)||
        (year % 400 == 0))
        return true;
    else
        return false;
}
/*------------------------------------------------------------------------------
 * Name     : Gre_to_Doy
 * Function : Transform gregorian calendar into DOY
 * Input    : const EpochTime &Gre
 * Output   : double (result of DOY)
 *-----------------------------------------------------------------------------*/
double MyFunctionCenter::Gre_to_Doy(const EpochTime &Gre)
{
    int Month_Days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if(isLeapYear(Gre.year))
        Month_Days[1] = 29;

    int Doy = 0;
    for (int m = 1 ; m <= Gre.month-1; m++)
       Doy += Month_Days[m-1];

    Doy += Gre.day;
    return Doy;
}

/*------------------------------------------------------------------------------
 * Name     : Gps_to_JD
 * Function : Transform GPS Time into Julian date
 * Input    : const GpsTime   &Gps
 * Output   : double (result of JD)
 *-----------------------------------------------------------------------------*/
double MyFunctionCenter::Gps_to_JD(const GpsTime   &Gps)
{
    double JD = Gps.week * 7 + Gps.sec / 86400.0 + 2444244.5;
    return JD;
}
/*------------------------------------------------------------------------------
 * Name     : JD_to_Gre
 * Function : Transform Julian date into gregorian calendar
 * Input    : const double    &JD
 * Output   : EpochTime &Gre
 *-----------------------------------------------------------------------------*/
void   MyFunctionCenter::JD_to_Gre(const double    &JD , EpochTime &Gre)
{
    int a =  JD + 0.5;
    int b =  a  + 1537;
    int c = (b  - 122.1) / 365.25;
    int d =  365.25 * c;
    int e = (b - d) / 30.6001;

    Gre.day   = b - d - (int)(30.6001 * e) + ((JD + 0.5) - (int)(JD + 0.5));
    Gre.month = e - 1 - 12 * (int)(e / 14.0);
    Gre.year  = c - 4715 - (int)((7 + Gre.month) / 10.0);

    double JD_decimal = JD  - (int)JD - 0.5;
    if (JD_decimal < 0)
        JD_decimal++;

    Gre.hour   = int (JD_decimal * 24);
    Gre.minute = int((JD_decimal * 24 - Gre.hour) * 60);
    Gre.second =    ((JD_decimal * 24 - Gre.hour) * 60 - Gre.minute) * 60;
}
/*------------------------------------------------------------------------------
 * Name     : JD_to_Gps
 * Function : Transform Julian date into  GPS Time
 * Input    : const double    &Jd
 * Output   : GpsTime   &Gps
 *-----------------------------------------------------------------------------*/
void   MyFunctionCenter::JD_to_Gps(const double    &JD, GpsTime   &Gps)
{
    Gps.week = ((JD - 2444244.5) / 7.0);
    Gps.sec  =(((JD - 2444244.5) / 7.0)  -
           int ((JD - 2444244.5) / 7.0)) * 604800.0;
}


/*                    ****************************************************                                       */
/*---------------------------------- Time system convert --------------------------------------------------------*/
/*                    ****************************************************                                       */
/*------------------------------------------------------------------------------
 * Name     : leapSecond_TAI_UTC
 * Function : Get the leap seconds between TAI and UTC
 * Input    : const double    &JD
 * Output   : int (leap seconds)
 *-----------------------------------------------------------------------------*/
int MyFunctionCenter::leapSecond_TAI_UTC(const double    &JD)
{
    if (JD < 2415078.5||JD > 2488099.5)
    {
        QMessageBox::critical(NULL,"error","time is out of range, "
                                           "please check time!",QMessageBox::Ok);
        return 0;
    }
    int    leapSecond                =  0 ;
    int    leapSecondArray      [26] = {0};                                      // Save leap second in recent years
    double secondModefiedDate_JD[26] = {0};                                      // Save julian date of recent years
    EpochTime *tempTime = NULL;

    /* ---------------------------- Define leap second constant ----------------*/
    tempTime = new EpochTime(1972,1,1,0,0,0);
    leapSecondArray[0 ] = 10  , secondModefiedDate_JD[0 ] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1972,7,1,0,0,0);
    leapSecondArray[1 ] = 11  , secondModefiedDate_JD[1 ] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1973,1,1,0,0,0);
    leapSecondArray[2 ] = 12  , secondModefiedDate_JD[2 ] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1974,1,1,0,0,0);
    leapSecondArray[3 ] = 13  , secondModefiedDate_JD[3 ] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1975,1,1,0,0,0);
    leapSecondArray[4 ] = 14  , secondModefiedDate_JD[4 ] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1976,1,1,0,0,0);
    leapSecondArray[5 ] = 15  , secondModefiedDate_JD[5 ] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1977,1,1,0,0,0);
    leapSecondArray[6 ] = 16  , secondModefiedDate_JD[6 ] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1978,1,1,0,0,0);
    leapSecondArray[7 ] = 10  , secondModefiedDate_JD[7 ] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1979,1,1,0,0,0);
    leapSecondArray[8 ] = 10  , secondModefiedDate_JD[8 ] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1980,1,1,0,0,0);
    leapSecondArray[9 ] = 19  , secondModefiedDate_JD[9 ] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1981,7,1,0,0,0);
    leapSecondArray[10] = 20  , secondModefiedDate_JD[10] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1982,7,1,0,0,0);
    leapSecondArray[11] = 21  , secondModefiedDate_JD[11] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1983,7,1,0,0,0);
    leapSecondArray[12] = 22  , secondModefiedDate_JD[12] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1985,7,1,0,0,0);
    leapSecondArray[13] = 23  , secondModefiedDate_JD[13] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1988,1,1,0,0,0);
    leapSecondArray[14] = 24  , secondModefiedDate_JD[14] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1990,1,1,0,0,0);
    leapSecondArray[15] = 25  , secondModefiedDate_JD[15] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1991,1,1,0,0,0);
    leapSecondArray[16] = 26  , secondModefiedDate_JD[16] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1992,7,1,0,0,0);
    leapSecondArray[17] = 27  , secondModefiedDate_JD[17] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1993,7,1,0,0,0);
    leapSecondArray[18] = 28  , secondModefiedDate_JD[18] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1994,7,1,0,0,0);
    leapSecondArray[19] = 29  , secondModefiedDate_JD[19] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1996,1,1,0,0,0);
    leapSecondArray[20] = 30  , secondModefiedDate_JD[20] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1997,7,1,0,0,0);
    leapSecondArray[21] = 31  , secondModefiedDate_JD[21] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(1999,1,1,0,0,0);
    leapSecondArray[22] = 32  , secondModefiedDate_JD[22] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(2006,1,1,0,0,0);
    leapSecondArray[23] = 33  , secondModefiedDate_JD[23] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(2009,1,1,0,0,0);
    leapSecondArray[24] = 34  , secondModefiedDate_JD[24] = Gre_to_JD(*tempTime);

    tempTime = new EpochTime(2012,7,1,0,0,0);
    leapSecondArray[25] = 35  , secondModefiedDate_JD[25] = Gre_to_JD(*tempTime);
    /*-------------------------------------------------------------------------*/

    delete tempTime;
    int    rangeEnd = 25;                                                       // max size of array

    if (JD < secondModefiedDate_JD[0]){
        leapSecond = 0;
    }
    else if (JD > secondModefiedDate_JD[rangeEnd]){
        leapSecond = leapSecondArray[rangeEnd];
    }
    else{
        for (int i = 1; i <= rangeEnd; i++)
        {
            if (JD <= secondModefiedDate_JD[i] && JD > secondModefiedDate_JD[i-1])
            {
                leapSecond = leapSecondArray[i-1];
                break;
            }
        }
    }
    return leapSecond;
}
/*------------------------------------------------------------------------------
 * Name     : GPS_to_UTC
 * Function : Convert GPS time to UTC
 * Input    : const MyTime &gpsTime
 * Output   : MyTime  (UTC)
 *-----------------------------------------------------------------------------*/
MyTime MyFunctionCenter::GPS_to_UTC(const MyTime &gpsTime)
{
    double leapSecond = leapSecond_TAI_UTC(gpsTime.JD);
    MyTime UTC        =  gpsTime;
    UTC.JD            =  gpsTime.JD + (19.0 - leapSecond) * SEC_DAY;            //  UTC = TAI - leapSecond
    UTC = timeIntegrator(UTC.JD);
    return UTC;
}
/*------------------------------------------------------------------------------
 * Name     : GPS_to_BDS
 * Function : convert GPS time to BDS time
 * Input    : const MyTime &gpsTime
 * Output   : MyTime  (BDS)
 *-----------------------------------------------------------------------------*/
MyTime MyFunctionCenter::GPS_to_BDS(const MyTime &gpsTime)
{
    MyTime BDST = gpsTime;
    BDST.JD     = gpsTime.JD - 14.000003 * SEC_DAY;                             // BDST = GPST - 14.000003
    BDST        = timeIntegrator(BDST.JD);
    return BDST;
}
/*------------------------------------------------------------------------------
 * Name     : UTC_to_TDT
 * Function : Convert UTC to TDT (Temps Dynamique Terrestrique)
 * Input    : const MyTime &utcTime
 * Output   : MyTime (TDT)
 *-----------------------------------------------------------------------------*/
MyTime MyFunctionCenter::UTC_to_TDT(const MyTime &utcTime)
{
    MyTime TDT        = utcTime;
    double leapSecond = leapSecond_TAI_UTC(utcTime.JD);
    TDT.JD            = utcTime.JD + (leapSecond + 32.184) * SEC_DAY;           // TDT = TAI + 32.184
    TDT               = timeIntegrator(TDT.JD);
    return TDT;
}

/*------------------------------------------------------------------------------
 * Name     : TDT_to_SID
 * Function : Convert TDT to SID
 * Input    : const MyTime &utcTime
 * Output   : MyTime (SID)
 *-----------------------------------------------------------------------------*/
//double MyFunctionCenter::UTC_to_SID(const MyTime &utcTime) /* Unknown SID 是个什么时间 */
double MyFunctionCenter::TDT_to_SID(const MyTime &tdtTime) /* Unknown SID 是个什么时间 */
{
    double hourOfDay =  tdtTime.EPT.hour + tdtTime.EPT.minute / 60.0 +
                                           tdtTime.EPT.second / 3600.0;
    double tempT     = (tdtTime.JD  - 2451545.0) / 36525.0;                     // Temporal value, in centuries.
    double sid       =  24110.54841 + tempT*((8640184.812866) +   tempT *
                                            ((0.093104) - (6.2e-6*tempT)));
           sid       =  sid /3600.0 + hourOfDay;
           sid       =  fmod(sid,     24.0);                                    // Used to fetch (remaining) the floating point numbers
    if (sid < 0.0)      sid+=24.0;
    return sid;
}

/*                    ****************************************************                                       */
/*----------------------------------- Coordiante functions ------------------------------------------------------*/
/*                    ****************************************************                                       */

/*------------------------------------------------------------------------------
 * Name     : getCoordDistance (1)
 * Function : Get distance of (x, y, z) to (0, 0, 0)
 * Input    : double xyz[3]
 * Output   : double (distance)
 *-----------------------------------------------------------------------------*/
double MyFunctionCenter::getCoordDistance(const double xyz[3])
{
    double R = sqrt(pow(xyz[0], 2) + pow(xyz[1], 2) + pow(xyz[2], 2));
    return R;
}
/*------------------------------------------------------------------------------
 * Name     : getCoordDistance (2)
 * Input    : double xyz[3]
 *----------------------------------------------------------------------------*/
double MyFunctionCenter::getCoordDistance(const Vector3d &xyz)
{
    double R = sqrt(pow(xyz[0], 2) + pow(xyz[1], 2) + pow(xyz[2], 2));
    return R;
}

/*------------------------------------------------------------------------------
 * Name     : getCoordSysParameter
 * Function : Get some common parameter of common coordinate system
 * Input    : const CoordSysName &coordName
 * Output   : CoordSysParameter  &coordPar
 *----------------------------------------------------------------------------*/
void   MyFunctionCenter::getCoordSysParameter(const CoordSysName &coordName,
                                              CoordSysParameter  &coordPar)
{
    switch (coordName)
    {
    case BeiJing1954:
        coordPar.A    = 6378245.0;                                              // Ellipsoid long axis
        coordPar.Alfa = 1.0/298.3;                                              // Ellipticity of ellipsoid
        coordPar.E2   = 0.00669342162296594;
        coordPar.GM   = 0;      /* Unknown*/
        coordPar.Omega= 0;      /* unknown*/
        coordPar.UTM  = 1.0;    /* unknown*/
        break;
    case XiAn1980:
        coordPar.A    = 6378140.0;
        coordPar.UTM  = 1.0;
        coordPar.Alfa = 1.0/298.257;
        coordPar.GM   = 0;
        coordPar.Omega= 0;
        coordPar.E2   = 0.006694384999588;
        break;
    case WGS_84:
        coordPar.A    = 6378137.0;
        coordPar.Alfa = 1.0/298.257223563;
        coordPar.E2   = 0.00669437999014132;
        coordPar.UTM  = 1.0;
//        coodPar.X0   = 0;
//        coodPar.Y0   = 0;
//        coodPar.L0   = 0;
        coordPar.H0   = 0;
        coordPar.DN   = 0;
        coordPar.GM   = 0;
        coordPar.Omega= 0;
        break;
    case CGS2000:
        coordPar.A    = 6378137.0;
        coordPar.Alfa = 1.0/298.257222101;
        coordPar.E2   = 2*coordPar.Alfa*coordPar.Alfa;
        coordPar.GM   = 3.986004418*1e+14;
        coordPar.Omega= 7.292115*1e-5;
        coordPar.UTM  = 0;
        break;
    default:
        QMessageBox::warning(NULL,"error","the Mode isn't supported, "
                             "please select another!",QMessageBox::Ok);
    }
}

/*------------------------------------------------------------------------------
 * Name     : getN
 * Function : Get radius of curvature in prime vertical
 * Input    : const double &a, const double &e2,double B
 * Output   : double (N)
 *-----------------------------------------------------------------------------*/
double   MyFunctionCenter::getN(const double &a, const double &e2,double B)
{
    double W  = sqrt(1 - e2*sin(B)*sin(B));
    double N  = a / W;
    return N;
}
/*------------------------------------------------------------------------------
 * Name     : getAzimuth
 * Function : Get azimuth
 * Input    : const double &delatX, const double &delatY
 * Output   : double (Azimuth)
 *-----------------------------------------------------------------------------*/
double   MyFunctionCenter::getAzimuth(const double &delatX, const double &delatY)
{
    double Az = atan(delatY / delatX);
    if (delatX < 0)
        Az += PI;
    else
        if (delatY < 0)
            Az += 2 * PI;
    return Az;
}
/*------------------------------------------------------------------------------
 * Name     : getUnitVector(1)
 * Function : Get unit vector
 * Input    : const Vector3d &XYZ
 * Output   : Vector3d (unit vector)
 *-----------------------------------------------------------------------------*/
Vector3d MyFunctionCenter::getUnitVector(const Vector3d &XYZ)
{
    return(XYZ / MyFunctionCenter::getCoordDistance(XYZ));
}
/*------------------------------------------------------------------------------
 * Name     : getUnitVector(2)
 * Input    : const double XYZ[]
 *-----------------------------------------------------------------------------*/
Vector3d MyFunctionCenter::getUnitVector(const double XYZ[])
{
    Vector3d xyz(XYZ[0], XYZ[1], XYZ[2]);
    return(xyz / MyFunctionCenter::getCoordDistance(xyz));
}
/*------------------------------------------------------------------------------
 * Name     : BLHToXYZ
 * Function : Convert BLH system to XYZ system
 * Input    : const Vector3d &BLH, CoordSysParameter  &coordPar
 * Output   : Vector3d (XYZ)
 *-----------------------------------------------------------------------------*/
Vector3d MyFunctionCenter::BLHToXYZ(const Vector3d &BLH,
                                    const CoordSysParameter  &coordPar)
{
    Vector3d XYZ;
    double B  = BLH(0), L = BLH(1), H = BLH(2);
    double N  = getN(coordPar.A, coordPar.E2, B);
    double e2 = coordPar.E2;
    XYZ(0) = (N + H)* cos(B) * cos(L);
    XYZ(1) = (N + H)* cos(B) * sin(L);
    XYZ(2) = (N *(1 - e2)+H) * sin(B);
    return XYZ;
}
/*------------------------------------------------------------------------------
 * Name     : XYZToBLH
 * Function : Convert XYZ system to BLH system
 * Input    : const Vector3d &XYZ, CoordSysParameter  &coordPar
 * Output   : Vector3d (BLH)
 *-----------------------------------------------------------------------------*/
Vector3d MyFunctionCenter::XYZToBLH(const Vector3d &XYZ,
                              const CoordSysParameter  &coordPar)
{
    Vector3d    BLH;
    double X  = XYZ(0);
    double Y  = XYZ(1);
    double Z  = XYZ(2);
    BLH(1) = atan(Y / X);
    if (X < 0 && Y < 0)
        BLH(1) -= PI;
    if (X < 0 && Y > 0)
        BLH(1) += PI;

    double disc = 1.0, B2, N1;
    double B1   = atan(Z / sqrt(X*X + Y*Y));
    while (fabs(disc) >= 1e-12)
    {
         N1   = getN(coordPar.A, coordPar.E2, B1);
         B2   = atan((Z + N1 * coordPar.E2 * sin(B1)) / sqrt(X*X + Y*Y));
         disc = B1 - B2;
         B1   = B2;
    }
    BLH(0)    = B1;
    BLH(2) = (sqrt(X*X + Y*Y) / cos(B1)) - N1;
    return BLH;
}
/*------------------------------------------------------------------------------
 * Name     : XYZToENU(1)
 * Function : Convert XYZ system to ENU system
 * Input    : const Vector3d &recXYZ .(Base coordinate)
 *            const Vector3d &satXYZ .(Aim coordinate)
 *            CoordSysParameter  &coordPar
 * Output   : Vector3d (ENU)
 *-----------------------------------------------------------------------------*/
Vector3d MyFunctionCenter::XYZToENU(const Vector3d &recXYZ, const Vector3d &satXYZ,
                                    const CoordSysParameter  &coordPar)
{    
    Matrix3d ToENU =  getMatrixToENU(coordPar, recXYZ);
    Vector3d ENU   =  ToENU * (satXYZ - recXYZ);
    return   ENU;
}

/*------------------------------------------------------------------------------
 * Name     : ENUToDAZ
 * Function : Convert ENU system to DAZ system
 * Input    : const Vector3d &satENU
 * Output   : Vector3d (DAZ: Distance, Azimuth, Zenith)
 *-----------------------------------------------------------------------------*/
Vector3d  MyFunctionCenter::ENUToDAZ(const Vector3d &satENU)
{

    Vector3d DAZ;
    DAZ(0)    = sqrt(satENU(0)*satENU(0) + satENU(1)*satENU(1) + satENU(2)*satENU(2));
    DAZ(1)    = getAzimuth(satENU(1), satENU(0));                               // In measurement, N is X while E is y.
    double xy = sqrt(satENU(0)*satENU(0) + satENU(1)*satENU(1));
    DAZ(2)    = PI/2 - atan(satENU(2) / xy);
    return DAZ;
}
/*------------------------------------------------------------------------------
 * Name     : CIStoCTS
 * Function : Convert CIS time to CTS time
 * Input    : const Vector3d &posCIS, const MyTime &tdtTime
 * Output   : Vector3d (DAZ: Distance, Azimuth, Zenith)
 *-----------------------------------------------------------------------------*/
Vector3d MyFunctionCenter::CIStoCTS(const Vector3d &posCIS, const MyTime &tdtTime)
{
    double sid =  MyFunctionCenter::TDT_to_SID(tdtTime);
    double ts  =  sid * PI / 12;     /* ts? */
    Vector3d      posCTS;
    posCTS[0]  =  cos(ts)*posCIS[0]+sin(ts)*posCIS[1];
    posCTS[1]  = -sin(ts)*posCIS[0]+cos(ts)*posCIS[1];
    posCTS[2]  =  posCIS[2];        /* ?   */
    return        posCTS;
}


/*                    ****************************************************                                       */
/*----------------------------------- Mathmatical functions -----------------------------------------------------*/
/*                    ****************************************************                                       */
/*------------------------------------------------------------------------------
 * Name     : linerInter
 * Function : Use liner interploation to get the Y value at X
 * Input    : double x0, double y0 .(Base coordinate)
 *            double x,            .(Aim  coordinate)
 *            double x1, double y1 .(Base coordinate)
 * Output   : double (Interpolation value)
 *-----------------------------------------------------------------------------*/
double MyFunctionCenter::linerInter(double x0, double y0, double x,             // Linear interpolation
                                    double x1, double y1)
{
    double resValue = y0 + (y1 - y0) * (x - x0) / (x1 - x0);
    return resValue;
}

/*------------------------------------------------------------------------------
 * Name     : lagrangeInter(1)
 * Function : Use lagrange interploation to get the Y value at X
 * Input    : const double xVector[] .(X array)
 *            const double yVector[] .(Y array)
 *            const double &x        .(Points needed to be interpolated)
 *            const int    &order    .(Order of interpolation)
 * Output   : double (Interpolation value)
 *-----------------------------------------------------------------------------*/
double  MyFunctionCenter::lagrangeInter(const double xVector[], const double yVector[],
                                        const double &x,        const int    &order)
{
    double  y = 0.0;
    for(int j = 0; j < order; j++)
    {
        double l = 1.0;
        for(int i = 0; i < order; i++)
        {
            if(i != j)
                l *= (x - xVector[i]) / (xVector[j] - xVector[i]);
        }
        y += l * yVector[j];
    }
    return y;
}

/*------------------------------------------------------------------------------
 * Name     : lagrangeInter(2)
 * Function : Use lagrange interploation to get the X, Y, Z value at T
 * Input    : const double tVector[] .(T array)
 *            const double xVector[] .(X array)
 *            const double yVector[] .(Y array)
 *            const double zVector[] .(Z array)
 *            const double &t        .(Points needed to be interpolated)
 *            const int    &order    .(Order of interpolation)
 * Output   : double* (Interpolation values)
 *-----------------------------------------------------------------------------*/
X_Y_Z MyFunctionCenter::lagrangeInter(const double tVector[], const double xVector[],
                                      const double yVector[], const double zVector[],
                                      const double &t,        const int    &order)
{
    X_Y_Z result(0,0,0);
    for(int j = 0; j < order; j++)
    {
        double l   = 1.0;
        for(int i  = 0; i < order; i++)
        {
            if(i  != j)
               l  *= (t - tVector[i]) / (tVector[j] - tVector[i]);
        }
        result.X +=  l * xVector[j];
        result.Y +=  l * yVector[j];
        result.Z +=  l * zVector[j];
    }
    return result;
}

/*------------------------------------------------------------------------------
 * Name     : kalmanFilter
 * Function : Use lagrange interploation to get the Y value at X
 * args     : I    I   Unit matrix
 *            F    I   State transition matrix
 *            R    I   Observation vector weight matrix
 *            B    I   coefficient matrix
 *            L    I   Observation matrix
 *            Qw   I   Noise covariance matrix
 *            Q    IO  covariance matrix
 *            X    IO  State vector matrix
 *            V    O   Residual vector matrix
 *-----------------------------------------------------------------------------*/
void  MyFunctionCenter::kalmanFilter(MatrixXd  I,  MatrixXd  F, MatrixXd R,
                                     MatrixXd  B,  VectorXd  L, MatrixXd Qw,
                                     MatrixXd &Q,  VectorXd &X, VectorXd &V)
{
    /*---------------------Output predictive value----------------------------*/
    MatrixXd M = F*Q*F.transpose() + Qw;                                        // M is predictive value of Q
    //X = F * X;
    /*----------------------Output estimate value-----------------------------*/
    MatrixXd K = (M*B.transpose())*(B*M*B.transpose() + R).inverse();
    Q = (I - K *  B)* M;
    X =  X + K * (L - B * X);
    V =  B * X -  L;
}

/*                    ****************************************************                                       */
/*--------------------------------------- Other functions -------------------------------------------------------*/
/*                    ****************************************************                                       */
/*------------------------------------------------------------------------------
 * Name     : getFrequency
 * Function : Get wave frequency of differeny system
 * Input    : Sat_TypePrn satTN, int *K
 * Output   : double* (Frequency array)
 *-----------------------------------------------------------------------------*/
void MyFunctionCenter::getFrequency(const Sat_TypePrn &satTN, int *K,
                                    vector<double> &frequency)
{
    if     (satTN.Type == "G")
    {
        frequency.resize(3);
        frequency[0] = 154*10.23E6;
        frequency[1] = 120*10.23E6;
        frequency[2] = 115*10.23E6;
    }
    else if(satTN.Type == "C")
    {
        frequency.resize(3);
        frequency[0] = 1561.098*1E+6;
        frequency[1] = 1207.14*1E+6;
        frequency[2] = 1268.52*1E+6;
    }
    else if(satTN.Type == "R")
    {
        int k      = K[satTN.PRN-1];

        double f01 = 1602.0000*1e+6;
        double f02 = 1246.0000*1e+6;
        double f03 = 1204.7040*1e+6;
        double df1 = 562.5*1e+3;
        double df2 = 437.5*1e+3;
        double df3 = 423.0*1e+3;

        frequency.resize(3);
        frequency[0] = f01+df1*k;
        frequency[1] = f02+df2*k;
        frequency[2] = f03+df3*k;
    }
    else if(satTN.Type == "E")
    {
        frequency.resize(5);
        frequency[0] = 1575.420*1E+6;
        frequency[1] = 1176.450*1E+6;
        frequency[2] = 1207.140*1E+6;
        frequency[3] = 1191.795*1E+6;
        frequency[4] = 1278.750*1E+6;
    }
    else if(satTN.Type == "S")
    {
        frequency.resize(3);
        frequency[0] = 154*10.23E6;
        frequency[1] = 120*10.23E6;
        frequency[2] = 115*10.23E6;
    }
    else if(satTN.Type == "J")
    {
        frequency.resize(3);
        frequency[0] = 154*10.23e6;
        frequency[1] = 120*10.23E6;
        frequency[2] = 115*10.23E6;
    }
}

/*------------------------------------------------------------------------------
 * Name     : getMatrixToENU(1)
 * Function : Get a matrix help to convert to ENU coordinate
 * Input    : const Vector3d &BLH
 * Output   : Matrix3d(ENU)
 *-----------------------------------------------------------------------------*/
Matrix3d MyFunctionCenter::getMatrixToENU(const Vector3d &BLH)
{
    Matrix3d      ToENU;
    ToENU(0,0) = -sin(BLH[1]);
    ToENU(0,1) =  cos(BLH[1]);
    ToENU(0,2) =  0;
    ToENU(1,0) = -sin(BLH[0])*cos(BLH[1]);
    ToENU(1,1) = -sin(BLH[0])*sin(BLH[1]);
    ToENU(1,2) =  cos(BLH[0]);
    ToENU(2,0) =  cos(BLH[0])*cos(BLH[1]);
    ToENU(2,1) =  cos(BLH[0])*sin(BLH[1]);
    ToENU(2,2) =  sin(BLH[0]);
    return        ToENU;
}
/*------------------------------------------------------------------------------
 * Name     : getMatrixToENU(2)
 * Input    : const Vector3d &XYZ, CoordSysParameter &coordPar
 *-----------------------------------------------------------------------------*/
Matrix3d MyFunctionCenter::getMatrixToENU(const CoordSysParameter &coordPar,
                                          const Vector3d          &XYZ)
{
    Vector3d BLH   =  XYZToBLH(XYZ, coordPar);
    Matrix3d ToENU =  getMatrixToENU(BLH);
    return   ToENU;
}

/*------------------------------------------------------------------------------
 * Name     : isSatFound
 * Function : Judge if can find the aim elements
 * Input    : vector <SatQuery> satContaine,
              const   SatQuery &aimSat
 * Output   : int (index of aim elements)
 *-----------------------------------------------------------------------------*/
int MyFunctionCenter::isSatFound(vector <SatQuery> satContaine,
                                 const   SatQuery &aimSat)
{
    if (aimSat.index < satContaine.size())                                      // If the index is legal
        if (satContaine[aimSat.index] == aimSat)
            return aimSat.index;

    for(unsigned int j = 0; j < satContaine.size(); j++)                        // If we can't find the target in a simple way, use this.
    {
        if (satContaine[j] == aimSat)
            return j;
    }
    return -1;                                                                  // Can't find the target
}
