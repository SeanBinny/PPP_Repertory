#include "MyFunctionCenter.h"

/*                    ****************************************************                                       */
/*---------------------------------- Time system convert --------------------------------------------------------*/
/*                    ****************************************************                                       */
/*------------------------------------------------------------------------------
 * Name     : timeIntegrator (1)
 * Function : Get all kinds of time
 * Input    : const MyTime &myTime
 * Output   : MyTime (if convert success)
 *-----------------------------------------------------------------------------*/
MyTime MyFuncionCenter::timeIntegrator(const MyTime &myTime)
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
    if (myTime.EPT.year > 0 && myTime.EPT.month  > 0 && myTime.EPT.day    > 0 &&
        myTime.EPT.hour > 0 && myTime.EPT.minute > 0 && myTime.EPT.second > 0)
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
 * Function : Use gregorian calendar to get all kinds of time
 * Input    : const EpochTime &Gre
 * Output   : MyTime (if convert success)
 *-----------------------------------------------------------------------------*/
MyTime MyFuncionCenter::timeIntegrator(const EpochTime &Gre)
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
 * Function : Use GPS time to get all kinds of time
 * Input    : const GpsTime   &Gps
 * Output   : MyTime (if convert success)
 *-----------------------------------------------------------------------------*/
MyTime MyFuncionCenter::timeIntegrator(const GpsTime   &Gps)
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
 * Function : Use julian date to get all kinds of time
 * Input    : const double    &JD
 * Output   : MyTime (if convert success)
 *-----------------------------------------------------------------------------*/
MyTime MyFuncionCenter::timeIntegrator(const double    &JD )                    // MJD can not do this
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
double MyFuncionCenter::Gre_to_JD(const EpochTime &Gre)
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
 * Function : Judge if he year is a leap year
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
double MyFuncionCenter::Gre_to_Doy(const EpochTime &Gre)
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
double MyFuncionCenter::Gps_to_JD(const GpsTime   &Gps)
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
void   MyFuncionCenter::JD_to_Gre(const double    &JD , EpochTime &Gre)
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
void   MyFuncionCenter::JD_to_Gps(const double    &JD, GpsTime   &Gps)
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
int MyFuncionCenter::leapSecond_TAI_UTC(const double    &JD)
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
    /*--------------------------------------------------------------------------*/

    delete tempTime;
    int    rangeEnd = 25;                                                        // max size of array

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
MyTime MyFuncionCenter::GPS_to_UTC(const MyTime &gpsTime)
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
MyTime MyFuncionCenter::GPS_to_BDS(const MyTime &gpsTime)
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
MyTime MyFuncionCenter::UTC_to_TDT(const MyTime &utcTime)
{
    MyTime TDT        = utcTime;
    double leapSecond = leapSecond_TAI_UTC(utcTime.JD);
    TDT.JD            = utcTime.JD + (leapSecond + 32.184) * SEC_DAY;           // TDT = TAI + 32.184
    TDT               = timeIntegrator(TDT.JD);
    return TDT;
}

MyTime MyFuncionCenter::UTC_to_SID(const MyTime &utcTime) /* Unknown */
{
//    double h=t.sod/3600.0;         //hour of day
//    //double frofday=t.sod/86400.0;
//    double tt=(t.julday-2451545.0)/36525.0; //Temporal value, in centuries.
//    double sid=24110.54841+tt*((8640184.812866)+tt*((0.093104)-(6.2e-6*tt)));
//    sid=sid/3600.0+h;
//    sid=fmod(sid,24.0);
//    if (sid<0.0)
//    {
//        sid+=24.0;
//    }
//    return sid;
}


