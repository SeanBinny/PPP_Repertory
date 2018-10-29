#ifndef ERRORMODELCENTER_H
#define ERRORMODELCENTER_H
#include "Eigen/Eigen"
#include "GlobalDefinationCenter.h"
#include "MyFunctionCenter.h"
#include "ProductDataCenter.h"


/*------------------------------------------------------------------------------
 * Name     : TroposphericDelayCorrection
 * Function : Provid all kinds of  models of tropospheric delay
 *-----------------------------------------------------------------------------*/
class TroposphericDelayCorrection
{
public:
    static VectorXd UNB3M(const Vector3d &recBLH, const double &Doy ,
                          const double &Elevation);

//    /*Tropospheric corrections*/
//    VectorXd UNB3M(Vector3d &obspos,double DAYOYEAR ,double ELEVRAD);
//    ////////////////////////////////////////////////////////////////////

//    VectorXd Modified_Hopfield(double P,double T,double e,double Z);

//    VectorXd relative_humidity(double T,double T0,double P0,double LATRAD,double DAYOYEAR);//////start

//    VectorXd gpt(double DOY,double dlat,double dlon,double dhgt);

//    VectorXd GMF(double DOY,double DLAT,double DLON,double DHGT,double ZD);

//    VectorXd seu_trop(Vector3d obspos, double doy, double zenith);
//    /////////////////////////////////////////////////////////////////////

//    VectorXd vmf1_ht(double ah, double aw, double dmjd, double dlat, double ht, double zd);

//    VectorXd gpt2(double dmjd,double dlat,double dlon,double hell,double nstat,double it,QString path);

//    VectorXd gpt_vmftrop(Vector3d obspos, double jd, double zenith,QString path);
};

/*--------------------------------------------------------------
 * Name     : ErrorModel
 * Function : Have all error model functions
 *-------------------------------------------------------------*/
class ErrorModel
{
public:
    static double   relativityCor(   const Vector3d &satXYZ,  const Vector3d &recXYZ,
                                     const Vector3d &satVel);
    static Vector2d antennaRecCor(const AntennaData &ant,     const Vector3d &DAZ);
    static double   antennaSatCor(const AntennaData &ant,     const Vector3d &recXYZ,
                                     const Vector3d &satXYZ,  const Vector3d &sunXYZ);
    static double   antennaHeightCor(const Vector3d &SAZ,     const Vector3d &ENU);
    static double   antennaWindUpCor(const Vector3d &recBLH,  const Vector3d &recXYZ,
                                     const Vector3d &satXYZ,  const Vector3d &sunXYZ,
                                     const QString  &antType, const Sat_TypePrn &sat);
    static double   sagnacCor   (    const Vector3d &satpos,  const Vector3d &obspos);

    static double   allTideCor  (const Vector3d  &DAZ,        const Vector3d &tide);
    static Vector3d oceanTideCor(const OceanData &oceanData,  const MyTime   &myTime);
    static Vector3d poleTideCor (const ErpData   &erpData,    const Vector3d &recBLH);
    static Vector3d solidTideCor(const Vector3d  &XSTA,       const Vector3d &sunposCTS,
                                 const Vector3d  &moonposCTS, const CoordSysParameter &coordPar,
                                 const MyTime    &t);
    static void     diffCodeBiasCor();

    static TroposphericDelayCorrection tropDelyCor;

private:
    static Vector2d getAntennaPCV(const AntennaData &ant, double elevation, int row = 0);
    static MatrixXd getOceanParameter(const MyTime &time);
    static Vector3d ST1DIU  (const Vector3d &XSTA, const Vector3d &XSUN, const Vector3d &XMON,
                             const double  &F2SUN, const double  &F2MON);
    static Vector3d ST1SEM  (const Vector3d &XSTA, const Vector3d &XSUN, const Vector3d &XMON,
                             const double  &F2SUN, const double  &F2MON);
    static Vector3d ST1L1   (const Vector3d &XSTA, const Vector3d &XSUN, const Vector3d &XMON,
                             const double  &F2SUN, const double  &F2MON);
    static Vector3d ST2LON  (const Vector3d &XSTA, const double  &T);
    static Vector3d STEP2DIU(const Vector3d &XSTA, const double  &FHR,   const double &T);

private:
    static double gPhaseSatellite[32];   // 后面看看再改改
    static double gPhaseStation  [32];
    static double rPhaseSatellite[32];
    static double rPhaseStation  [32];
    static double cPhaseSatellite[32];
    static double cPhaseStation  [32];
    static double ePhaseSatellite[32];
    static double ePhaseStation  [32];

};
#endif // ERRORMODELCENTER_H
