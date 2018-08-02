#ifndef PUBLICDATACENTER_H
#define PUBLICDATACENTER_H

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

#endif // PUBLICDATACENTER_H
