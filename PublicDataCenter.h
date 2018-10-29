#ifndef PUBLICDATACENTER_H
#define PUBLICDATACENTER_H
#include <QString>
#include <algorithm>
#include "GlobalDefinationCenter.h"
#include "IllegalProblemCenter.h"


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
    EpochTime (const EpochTime & Ep)
        : year  (Ep.year),   month (Ep.month),
          day   (Ep.day),    hour  (Ep.hour),
          minute(Ep.minute), second(Ep.second){}
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

/*--------------------------------------------------------------
 * Function : Save 3D coordinate
 *-------------------------------------------------------------*/
struct X_Y_Z
{
    double X;
    double Y;
    double Z;
    X_Y_Z(double x, double y, double z)
        :X(x), Y(y), Z(z){}
};
/*--------------------------------------------------------------
 * Function : Save single satellite PRN number and system type
 *-------------------------------------------------------------*/
struct Sat_TypePrn
{
    QString Type;
    int     PRN;
    Sat_TypePrn(QString st = "", int pn = 0)
        : Type(st), PRN(pn)  {}
    bool operator ==(const Sat_TypePrn &s)
    {
        if (this->Type == s.Type && this->PRN == s.PRN)
            return true;
        else
            return false;
    }
    bool operator !=(const Sat_TypePrn &s)
    {
        if (*this == s) return false;
        else            return true;
    }
};


/*--------------------------------------------------------------
 * Function : Store commonly coordinate system parameters and
 *            coordinate system frames
 *-------------------------------------------------------------*/
enum CoordSysName                                               // Coordinate system frames name
{
    BeiJing1954,
    XiAn1980,
    WGS_84,
    CGS2000
};
struct CoordSysParameter
{
    double A;                                                   // Ellipsoidal long radius
    double Alfa;                                                // Ellipsoidal flattening rate
    double E2;                                                  // Ellipsoidal first eccentricity squared
    double UTM;
//    double X0,Y0;
//    double L0;
    double H0;                                                  // Projection surface elevation
    double DN;                                                  // Elevation anomaly
    double GM;
    double Omega;
};

/*--------------------------------------------------------------
 * Function : Mode selection flag
 *-------------------------------------------------------------*/
struct ModeFlag
{
    static bool hasGPS;
    static bool hasBDS;
    static bool hasGLONASS;
    static bool hasGalileo;

    static bool P1_P2;
    static bool P1_P3;
    static bool P2_P3;

    static bool frequency3;

    static bool Model_UD;                                       // Un-Difference ionosphere-free combined model
    static bool Model_UC;                                       // Un-Combined Model
    static bool TF;                                             // Triple frequence
    static bool SF_PPP;/*unknown*/                              // Single frequence

    static bool dynamic;                                        // Real-time processing
    static bool back;                                           // Post processing

    static int  systemNum;
    static int  getSystemNum(){
        if (hasGPS)     systemNum++;
        if (hasBDS)     systemNum++;
        if (hasGLONASS) systemNum++;
        if (hasGalileo) systemNum++;
        return systemNum;
    }
};

/*--------------------------------------------------------------
 * Function : For efficient search of satellites
 *-------------------------------------------------------------*/
struct SatQuery
{
    Sat_TypePrn      satTN;
    unsigned int     index;                                     // Help to inmprove the query velocity
    SatQuery(QString st = "", int pn = 0, int id = -1)
        : satTN(st,  pn), index(id) {}
    SatQuery(Sat_TypePrn st,  int id = -1)
        : index(id) {satTN =  st;}

    bool operator ==(const SatQuery &s)
    {
        if (this->satTN == s.satTN)
            return true;
        else
            return false;
    }
};

#endif // PUBLICDATACENTER_H
