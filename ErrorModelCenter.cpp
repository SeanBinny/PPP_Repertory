#include "ErrorModelCenter.h"
#include "IllegalProblemCenter.h"
using namespace Eigen;


/*------------------ Define static member----------------------------------------------------*/
double ErrorModel::gPhaseSatellite[32]{0};   // 后面看看再改改
double ErrorModel::gPhaseStation  [32]{0};
double ErrorModel::rPhaseSatellite[32]{0};
double ErrorModel::rPhaseStation  [32]{0};
double ErrorModel::cPhaseSatellite[32]{0};
double ErrorModel::cPhaseStation  [32]{0};
double ErrorModel::ePhaseSatellite[32]{0};
double ErrorModel::ePhaseStation  [32]{0};
/*------------------------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
 * Name     : relativityCor
 * Function : Get wave frequency of differeny system
 * Input    : const Vector3d &satPos, const Vector3d &recPos,
 *            const Vector3d &satVel
 * Output   : double(relativity correction)
 *-----------------------------------------------------------------------------*/
double ErrorModel::relativityCor(const Vector3d &satXYZ, const Vector3d &recXYZ,
                                 const Vector3d &satVel)
{
    Vector3d deltaCoord = recXYZ - satXYZ;
    double R       =  MyFunctionCenter::getCoordDistance(deltaCoord);
    double Rs      =  MyFunctionCenter::getCoordDistance(satXYZ);
    double Rr      =  MyFunctionCenter::getCoordDistance(recXYZ);

    VectorXd rv    =  satXYZ.transpose() * satVel;                               // staPos dot staVel

    double effect1 = -2 * rv[0] /     LIGHT_V ;
    double effect2 =  2 * GM    / pow(LIGHT_V, 2) * log((Rs+Rr+R) / (Rs+Rr-R));
    return effect1 + effect2;
}

/*------------------------------------------------------------------------------
 * Name     : getAntennaPCV
 * Function : Use linear interploation to get PCY from antenna file
 * Input    : AntennaData &ant, double zenAngle, int row
 * Output   : Vector2d (L1 and L2 PCV)
 *-----------------------------------------------------------------------------*/
Vector2d ErrorModel::getAntennaPCV(const AntennaData &ant, double zenAngle, int row)
{
    double zen1 = ant.ZEN1;
    double zen2 = ant.ZEN2;
    double dzen = ant.DZEN;
    zenAngle    = int(zenAngle); /*UnKnown*/
    if (dzen == 0)
        throw illegalParameterValue("There is a problem with the zenith angle increment");
    Vector2d resPCV(0,0);
    for (int i =  zen1, col =0; i <= zen2; i+= dzen, col ++)                    // Use linear interpolation to get PCV value
    {
        if ( i == zenAngle){
            resPCV(0) = ant.F1_NOAZI(row, col);
            resPCV(1) = ant.F2_NOAZI(row, col);
        }
        else if (i < zenAngle && zenAngle < (i + dzen)){
            resPCV(0) = MyFunctionCenter::linerInter(i,  ant.F1_NOAZI(row,col), zenAngle,
                                                     i + dzen , ant.F1_NOAZI(row, col+1));
            resPCV(1) = MyFunctionCenter::linerInter(i,  ant.F2_NOAZI(row,col), zenAngle,
                                                     i + dzen , ant.F2_NOAZI(row, col+1));
            break;
        }
    }
    return resPCV;
}
/*------------------------------------------------------------------------------
 * Name     : antennaSatCor
 * Function : Get satellite antenna correction
 * Input    : AntennaData &ant, Vector3d &recPos,
              Vector3d &satPos, Vector3d &sunPos
 * Output   : double (PCV correction )
 *-----------------------------------------------------------------------------*/
double ErrorModel::antennaSatCor(const AntennaData &ant, const Vector3d &recXYZ,
                                 const Vector3d &satXYZ, const Vector3d &sunXYZ)
{
    Vector3d ez = -satXYZ / MyFunctionCenter::getCoordDistance(satXYZ);          // Get star system unit vector
    Vector3d ex =  sunXYZ / MyFunctionCenter::getCoordDistance(sunXYZ);
    Vector3d ey =  ez.cross(ex);
             ex =  ey.cross(ez);
             ex =  ex     / MyFunctionCenter::getCoordDistance(ex);
    Vector3d rs =  recXYZ - satXYZ;                                             // Get vector(r) between receiver and satllite
    Vector3d r  = (recXYZ - satXYZ)
                          / MyFunctionCenter::getCoordDistance(rs);

    double ezr  =  r.dot(ez);
           ezr  =  ezr < -1.0 ? -1.0 : (ezr > 1.0 ? 1.0 : ezr);
    double theta=  acos(ezr) * 180 / PI;                                        // Get the angle(theta) between ez and r
    if (theta < ant.ZEN1 || theta > ant.ZEN2)
        return 0;                                                               // Return an ivalid value
//            throw illegalParameterValue("There is a problem with the elevation "
//                                        "angle when looking for PCV");
    double zPCV =  getAntennaPCV(ant, theta, 0)(0);                             // Get PCV from antenna file
    Vector3d satPCV(0, 0, zPCV);                                                // Get PCV vector
    Vector3d satPCO;                                                            // Get PCO vector
    satPCO[0]   = ant.F1_NEU[0];
    satPCO[1]   = ant.F1_NEU[1];
    satPCO[2]   = ant.F1_NEU[2];

    satPCO      = satPCO - satPCV;
    VectorXd PCC= ex * satPCO[0] + ey * satPCO[1] + ez * satPCO[2];
    double   s  = r.dot(PCC) / 1000;                                            // The distance from the satellite to the line of sight of the observatory (s)
    return   s;
}


/*------------------------------------------------------------------------------
 * Name     : antennaSatCor
 * Function : Get receiver antenna correction
 * Input    : const AntennaData &ant,
 *            const Vector3d &DAZ,
 *            const Sat_TypePrn &satTN,
 *            int *K
 * Output   : Vector2d (F1 and F2 PCV)
 *-----------------------------------------------------------------------------*/
Vector2d ErrorModel::antennaRecCor(const AntennaData &ant,  const Vector3d &DAZ)

{
    Vector3d  pco1;
    pco1[0] = ant.F1_NEU[0] / 1000;
    pco1[1] = ant.F1_NEU[1] / 1000;
    pco1[2] = ant.F1_NEU[2] / 1000;
    Vector3d  pco2;
    pco2[0] = ant.F2_NEU[0] / 1000;
    pco2[1] = ant.F2_NEU[1] / 1000;
    pco2[2] = ant.F2_NEU[2] / 1000;

    Vector2d PCV(0,0);                                                          // SAVE PCV1 and PCV2
    int    row   = 0,  col  = 0;
    double theta = int(DAZ[2] * 180 / PI);

    /*-In view of DZAN are common assigned with 5.0 and 0.0, just deal with these two case -------------------------------*/
    if (ant.F1_NOAZI.rows() == 1 && ant.F2_NOAZI.rows() == 1)
    {
        if (theta >= ant.ZEN2)
        {
            col    = (ant.ZEN2 - ant.ZEN1)/ ant.DZEN - 1;                       // Get penult element position
            PCV(0) =  MyFunctionCenter::linerInter(ant.ZEN2 - ant.DZEN, ant.F1_NOAZI(0, col),
                                          theta, ant.ZEN2, ant.F1_NOAZI(0, col + 1)) / 1000;
            PCV(1) =  MyFunctionCenter::linerInter(ant.ZEN2 - ant.DZEN, ant.F2_NOAZI(0, col),
                                          theta, ant.ZEN2, ant.F2_NOAZI(0, col + 1)) / 1000;
        }
        else
        {
            PCV    =  getAntennaPCV(ant, theta, 0);
            PCV(0) =  PCV(0) / 1000, PCV(1) = PCV(1) / 1000;
        }
    }
    else if (ant.F1_NOAZI.rows() == 74 && ant.F2_NOAZI.rows() == 74)
    {
        Vector2d pcvR1(0,0);                                                    // (row    : pcv1, pcv2)
        Vector2d pcvR2(0,0);                                                    // (row + 1: pcv1, pcv2)

        for (int j = 0; j <= 360; j += ant.DAZI, row ++)                        // Get initial number of rows
        {
            if (j * PI / 180 <= DAZ[1] && DAZ[1] <= (j + ant.DAZI) * PI / 180){
                break;
            }
        }
        if (row >= 73)
            row  = 72;
        if (theta   >=  ant.ZEN2)
        {
            col      = (ant.ZEN2 - ant.ZEN1)/ ant.DZEN - 1;                     // Get penult element position
            /*----------------- Save row and row+1 of PCV1---------------------*/
            pcvR1(0) =  MyFunctionCenter::linerInter(ant.ZEN2 - ant.DZEN, ant.F1_NOAZI(row,     col) ,
                                                   theta, ant.ZEN2, ant.F1_NOAZI(row,     col + 1));
            pcvR2(0) =  MyFunctionCenter::linerInter(ant.ZEN2 - ant.DZEN, ant.F1_NOAZI(row + 1, col) ,
                                                   theta, ant.ZEN2, ant.F1_NOAZI(row + 1, col + 1));
            /*----------------- Save row and row+1 of PCV2---------------------*/
            pcvR1(1) =  MyFunctionCenter::linerInter(ant.ZEN2 - ant.DZEN, ant.F2_NOAZI(row,     col) ,
                                                   theta, ant.ZEN2, ant.F2_NOAZI(row,     col + 1));
            pcvR2(1) =  MyFunctionCenter::linerInter(ant.ZEN2 - ant.DZEN, ant.F2_NOAZI(row + 1, col) ,
                                                   theta, ant.ZEN2, ant.F2_NOAZI(row + 1, col + 1));
        }
        else
        {
            pcvR1    =  getAntennaPCV(ant, theta, row  ) / 1000;
            pcvR2    =  getAntennaPCV(ant, theta, row+1) / 1000;
        }
        PCV(0)       =  MyFunctionCenter::linerInter(row,            pcvR1(0),   // Second polynomial interpolation
                                                     DAZ[1] * 180 / PI,
                                                     row + ant.DAZI, pcvR2(0));
        PCV(1)       =  MyFunctionCenter::linerInter(row,            pcvR1(1),
                                                     DAZ[1] * 180 / PI,
                                                     row + ant.DAZI, pcvR2(1));
    }

    double elevationAngle = PI / 2 - DAZ[2];
    double cosEl =  cos(elevationAngle);
    double sinEl =  sin(elevationAngle);
    double cosAz =  cos(DAZ[1]);
    double sinAz =  sin(DAZ[1]);

    PCV(0) = (-PCV(0) + pco1[0]*cosEl*cosAz + pco1[1]*cosEl*sinAz + pco1[2]*sinEl);
    PCV(1) = (-PCV(1) + pco2[0]*cosEl*cosAz + pco2[1]*cosEl*sinAz + pco2[2]*sinEl);
    return  PCV;

}

/*------------------------------------------------------------------------------
 * Name     : antennaHeightCor
 * Function : Get antenna height correction
 * Input    : const Vector3d &DAZ, const Vector3d& ENU
 * Output   : double (Antenna height correction)
 *-----------------------------------------------------------------------------*/
double   ErrorModel::antennaHeightCor(const Vector3d &DAZ, const Vector3d& ENU)
{
    double eleAngle = PI / 2 - DAZ[2];
    double cosEl    = cos(eleAngle);
    double sinEl    = sin(eleAngle);
    double cosAz    = cos(DAZ[1]);
    double sinAz    = sin(DAZ[1]);
    double distance = ENU[0]*cosEl*cosAz + ENU[1]*cosEl*sinAz + ENU[2]*sinEl;
    return distance;
}

/*------------------------------------------------------------------------------
 * Name     : antennaWindUpCor
 * Function : Get antenna wind up correction
 * Input    : const Vector3d &BLH ,   const Vector3d &recXYZ,
              const Vector3d &satXYZ, const Vector3d &sunXYZ,
              QString satType
 * Output   : double (Antenna wind up correction)
 *-----------------------------------------------------------------------------*/
double   ErrorModel::antennaWindUpCor(const Vector3d &recBLH, const Vector3d &recXYZ,
                                      const Vector3d &satXYZ, const Vector3d &sunXYZ,
                                      const QString  &antType, const Sat_TypePrn &sat)
{
     /*--------------------  Get satellite ex, ey ez  -------------------------*/
    Vector3d rss  =  (sunXYZ - satXYZ);
             rss  =   rss    / MyFunctionCenter::getCoordDistance(rss);
    Vector3d ez   = -(satXYZ / MyFunctionCenter::getCoordDistance(satXYZ));
    Vector3d ey   =   ez.cross(rss);
             ey   =   ey     / MyFunctionCenter::getCoordDistance(ey);
    Vector3d ex   =   ey.cross(ez);
             ex   =   ex     / MyFunctionCenter::getCoordDistance(ex);
                                                                                // Get rrho anf alpha1
    Vector3d rrho =  (recXYZ - satXYZ) ;
             rrho =   rrho   / MyFunctionCenter::getCoordDistance(rrho);
    double rrho_ez=   rrho.dot(ez);
    VectorXd dpp  =   rrho - rrho_ez*ez;
    double xk     =   dpp.dot(ex);
    double yk     =   dpp.dot(ey);
    double alpha1 =   atan2(yk,xk);
    /*---------------------  Get receiver ex, ey, ez --------------------------*/
    double B = recBLH(0), L = recBLH(1);
    Matrix3d d1, d2;
    d1 <<  cos(L) , -sin(L) ,  0.0
         , sin(L) ,  cos(L) ,  0.0
         ,  0.0   ,   0.0   ,  1.0;
    d2 <<  cos(B) ,   0.0   , -sin(B)
         ,  0.0   ,   1.0   ,  0.0
         , sin(B) ,   0.0   ,  cos(B);
    Vector3d d3;
    d3 <<   0.0   ,   0.0   ,  1.0;
    Vector3d d = d1 * d2 *d3;

    Vector3d ez_ = -(recXYZ / MyFunctionCenter::getCoordDistance(recXYZ));
    Vector3d ey_ =   ez_.cross(d);
             ey_ =   ey_    / MyFunctionCenter::getCoordDistance(ey_);
    Vector3d ex_ =   ey_.cross(ez_);
             ex_ =   ex_    / MyFunctionCenter::getCoordDistance(ex_);

    double rrho_ez_ = rrho.dot(ez_);
    VectorXd dpp_   = rrho - rrho_ez_*ez_;
    double xk_      = dpp_.dot(ex_);
    double yk_      = dpp_.dot(ey_);
    double alpha2   = atan2(yk_, xk_);


    if (antType.indexOf("IIR") >= 0)                                            // if satellite type is block IIR dealt alpha need to add PI
        alpha1 += PI;

    double *phaseStation   = NULL;
    double *phaseSatellite = NULL;
    if      (sat.Type == "G"){
        phaseSatellite = gPhaseSatellite;
        phaseStation   = gPhaseStation;
    }
    else if (sat.Type == "C"){
        phaseSatellite = cPhaseSatellite;
        phaseStation   = cPhaseStation;
    }
    else if (sat.Type == "R"){
        phaseSatellite = rPhaseSatellite;
        phaseStation   = rPhaseStation;
    }
    else if (sat.Type == "E"){
        phaseSatellite = ePhaseSatellite;
        phaseStation   = ePhaseStation;
    }

    double pStation   = phaseStation  [int(sat.PRN) - 1];
    double pSatellite = phaseSatellite[int(sat.PRN) - 1];
    double da1=alpha1 - pSatellite;
    double da2=alpha2 - pStation;

    pSatellite   += atan2(sin(da1),cos(da1));
    pStation     += atan2(sin(da2),cos(da2));
    double windUp = pSatellite - pStation;
           windUp = windUp     / (2 * PI);

    phaseSatellite[int(sat.PRN)-1] = pSatellite;
    phaseStation  [int(sat.PRN)-1] = pStation;

    return windUp;
}

/*------------------------------------------------------------------------------
 * Name     : sagnacCor
 * Function : Get earth rotation correction
 * Input    : const Vector3d &satXYZ, const Vector3d &recXYZ
 * Output   : double (Earth rotation correction)
 *-----------------------------------------------------------------------------*/
double ErrorModel::sagnacCor(const Vector3d &satXYZ, const Vector3d &recXYZ)
{
    double sagnacEffect = OMEGAE_DOT / LIGHT_V * (satXYZ[1]*(recXYZ[0] - satXYZ[0])
                         -satXYZ[0]  *(recXYZ[1] - satXYZ[1]));
    return sagnacEffect;
}


void ErrorModel::diffCodeBiasCor(/*QVector<commonview_file>& gongshifile2,system_DCB &DCB_data,
                         m_time &t, QString &type, double sats, double *val*/)
{
//    if(type=="G")
//    {
//        val[0]=DCB_data.GPS[sats-1].C1C_C2W[t.doy-1];
//        if(gongshifile2[0].GPS_TYPES[0]=="C1C")
//        {
//            val[2]=DCB_data.GPS[sats-1].C1C_C1W[t.doy-1];
//        }
//        if(gongshifile2[0].GPS_TYPES[4]=="C5Q")
//        {
//            val[1]=DCB_data.GPS[sats-1].C1C_C5Q[t.doy-1];
//        }
//        else
//        {
//            val[1]=DCB_data.GPS[sats-1].C1C_C5X[t.doy-1];
//        }
//    }
//    else if(type=="R")
//    {
//        if(gongshifile2[0].GLONASS_TYPES[0]=="C1C")
//        {
//            val[2]=DCB_data.GLONASS[sats-1].C1C_C1P[t.doy-1];
//        }
//        if(gongshifile2[0].GLONASS_TYPES[2]=="C2C")
//        {
//            val[0]=DCB_data.GLONASS[sats-1].C1C_C2C[t.doy-1];
//        }else{
//            val[0]=DCB_data.GLONASS[sats-1].C1C_C2P[t.doy-1];
//        }
//    }
//    else if(type=="C")
//    {
//        val[0]=DCB_data.BDS[sats-1].C2I_C7I[t.doy-1];
//        val[1]=DCB_data.BDS[sats-1].C2I_C6I[t.doy-1];
//        val[2]=DCB_data.BDS[sats-1].C7I_C6I[t.doy-1];
//    }
//    else if(type=="E")
//    {
//        if(gongshifile2[0].Galileo_TYPES[0]=="C1C")
//        {
//            val[0]=DCB_data.GALILEO[sats-1].C1C_C5Q[t.doy-1];
//            val[1]=DCB_data.GALILEO[sats-1].C1C_C7Q[t.doy-1];
//        }
//        else
//        {
//            val[0]=DCB_data.GALILEO[sats-1].C1X_C5X[t.doy-1];
//            val[1]=DCB_data.GALILEO[sats-1].C1X_C7X[t.doy-1];
//        }
//    }

}
/*------------------------------------------------------------------------------
 * Name     : getOceanParameter
 * Function : Get parameter of ocean tide correction
 * Input    : const MyTime &time
 * Output   : MatrixXd (parameter)
 *-----------------------------------------------------------------------------*/
MatrixXd ErrorModel::getOceanParameter(const MyTime &time)
{
   // const double TORAD = 0.017453;/////////////////////////////
    int oceanTidenum = 11;
    MatrixXd sig(1,  oceanTidenum);         /*  是不是可以提出来,或改成静态的 */

    sig(0, 0) = 1.40519e-4;
    sig(0, 1) = 1.45444e-4;
    sig(0, 2) = 1.37880e-4;
    sig(0, 3) = 1.45842e-4;
    sig(0, 4) = 0.72921e-4;
    sig(0, 5) = 0.67598e-4;
    sig(0, 6) = 0.72523e-4;
    sig(0, 7) = 0.64959e-4;
    sig(0, 8) = 0.053234e-4;
    sig(0, 9) = 0.026392e-4;
    sig(0,10) = 0.003982e-4;
    MatrixXd angfac(4, oceanTidenum);
    angfac<< 2.0, -2.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  2.0, -3.0,  1.0,
             0.0,  2.0,  0.0,  0.0,  0.0,  1.0,  0.0,  0.0,  0.25, 1.0, -2.0,
             0.0, -0.25,-1.0,  0.0,  0.0, -0.25, 1.0, -3.0,  1.0, -0.25, 0.0,
             2.0,  0.0,  0.0,  0.0,  1.0, -1.0,  0.0,  2.0,  0.0,  0.0,  0.0;

    MatrixXd arguments(1, oceanTidenum);

    double year     = time.EPT.year;
    double secOfDay = time.EPT.hour*3600 + time.EPT.minute*60 + time.EPT.second;// Time must be convert to seconds

    double d    =   time.DOY + 365.0 * (year - 1975.0) + floor((year  - 1973.0) / 4.0);
    double t    =  (27392.500528 + 1.000000035 * d) / 36525.0;
    double H0   =  (279.69668 + (36000.768930485 + 3.03e-4 * t) * t)  * ToRAD;
    double S0   =(((1.9e-6 * t - 0.001133) * t + 481267.88314137) * t + 270.434358) * ToRAD;
    double P0   =(((-1.2e-5* t - 0.010325) * t + 4069.0340329577) * t + 334.329653) * ToRAD;

    for (int k = 0; k < oceanTidenum; k++)
    {
        double temp = sig(0,k)*secOfDay + angfac(0,k)*H0 + angfac(1,k)*S0 +
                                          angfac(2,k)*P0 + angfac(3,k)*2*PI;
        arguments(0,k) = fmod(temp, 2*PI);
        if (arguments(0,k) < 0.0)
            arguments(0,k) = arguments(0,k) + 2*PI;
    }
    return arguments;
}
/*------------------------------------------------------------------------------
 * Name     : allTideCor
 * Function : Get all tide correction
 * Input    : const Vector3d  &DAZ, const Vector3d &tide
 * Output   : double (All tide correction)
 *-----------------------------------------------------------------------------*/
double ErrorModel::allTideCor(const Vector3d  &DAZ, const Vector3d &tide)
{
    double elev  = PI/2 - DAZ[2];
    double cosEl = cos(elev);
    double sinEl = sin(elev);
    double cosAz = cos(DAZ[1]);
    double sinAz = sin(DAZ[1]);
    double resTide  =  tide[0]*cosEl*sinAz +
                       tide[1]*cosEl*cosAz + tide[2]*sinEl;
    return resTide ;
}


/*------------------------------------------------------------------------------
 * Name     : oceanTideCor
 * Function : Get ocean tide correction
 * Input    : const OceanData &oceanData, const MyTime &myTime
 * Output   : Vector3d (Three directions of ocean tide correction)
 *-----------------------------------------------------------------------------*/
Vector3d ErrorModel::oceanTideCor(const OceanData &oceanData, const MyTime &myTime)
{
   // double TORAD=0.017453;//////////////////////////
    double directionNum  = 3;
    double oceanWaveNum  = 11;
    MatrixXd parameter   = getOceanParameter(myTime);
    Vector3d tideRWS;                                                           // Save radial, west and south

    for (int i = 0; i < directionNum; i++)
    {
        double temp = 0.0;
        for (int k = 0; k < oceanWaveNum; k++)
            temp += oceanData.tideData[k].observeValue[i] *
                    cos(parameter(0,k) -  oceanData.tideData[k].observeValue[i+3] * ToRAD);/* un known */
        tideRWS[i] = temp;
    }
    Vector3d tideENU;
    tideENU[0] = -tideRWS[1];
    tideENU[1] = -tideRWS[2];                                                   // Convert south to north
    tideENU[2] =  tideRWS[0];
    return tideENU;
}

/*------------------------------------------------------------------------------
 * Name     : poleTideCor
 * Function : Get pole tide correction
 * Input    : const ErpData &erpData, const Vector3d &recBLH
 * Output   : Vector3d (Three directions of pole tide correction)
 *-----------------------------------------------------------------------------*/
Vector3d ErrorModel::poleTideCor(const ErpData  &erpData,
                                 const Vector3d &recBLH)
{
    double Xp       =  erpData.xPole;
    double Yp       =  erpData.yPole;
    double timeDiff = (erpData.myTime.JD - ErpData::baseTimeJ2000.JD) / 365.25; // Get difference between current time and base time

    double Xpbar = 0.054 + timeDiff * 0.00083;
    double Ypbar = 0.357 + timeDiff * 0.00395;
    double m1    = Xp    - Xpbar;
    double m2    = Ypbar - Yp;
    double B     = recBLH[0];
    double L     = recBLH[1];

    Vector3d tideENU(0,0,0);
    tideENU[0] =  0.009*sin(  B)*(m1*sin(L) - m2*cos(L));
    tideENU[1] = -0.009*cos(2*B)*(m1*cos(L) + m2*sin(L));                       // Convert south to north
    tideENU[2] = -0.033*sin(2*B)*(m1*cos(L) + m2*sin(L));
    return tideENU;
}


/*------------------------------------------------------------------------------
 * Name     : solidTideCor
 * Function : Get solid tide correction
 * Input    : const Vector3d &XSTA, const Vector3d &sunposCTS,
              const Vector3d &moonposCTS, const MyTime &t
 * Output   : Vector3d (Three directions of solid tide correction)
 * Note     : All functions related to the tide of solids are borrowed
 *-----------------------------------------------------------------------------*/
Vector3d ErrorModel::solidTideCor(const Vector3d &XSTA,       const Vector3d &sunposCTS,
                                  const Vector3d &moonposCTS, const CoordSysParameter &coordPar,
                                  const MyTime   &t)
{
    /*% PURPOSE    :COMPUTATION OF TIDAL CORRECTIONS OF STATION DISPLACEMENTS
    %               CAUSED BY LUNAR AND SOLAR GRAVITATIONAL ATTRACTION
    %               (SEE IERS STANDARDS 2000)
    %               STEP 1 (HERE GENERAL DEGREE 2 AND 3 CORRECTIONS +
    %                       CALL ST1DIU + CALL ST1SEM + CALL ST1L1)
    %               + STEP 2 (CALL STEP2DIU + CALL ST2LON)
    %                         CORRECTION FOR POLAR MOTION
    %               IT HAS BEEN DECIDED THAT THE STEP 3 NON-CORRECTION FOR
    %               PERMANENT TIDE WOULD NOT BE APPLIED IN ORDER TO AVOID JUMP
    %               IN THE REFERENCE FRAME (THIS STEP 3 MUST ADDED IN ORDER TO
    %               GET THE NON-TIDAL STATION POSITION AND TO BE CONFORMED WITH
    %               THE IAG RESOLUTION.)
    %
    % PARAMETERS :
    %         IN :  XSTA(I),I=1,2,3,4: GEOCENTRIC POSITION OF THE STATION  R*8
    %               XSUN(I),I=1,2,3,4: GEOC. POSITION OF THE SUN           R*8
    %               XMON(I),I=1,2,3,4: GEOC. POSITION OF THE MOON          R*8
    %               DMJD             : MJD                                 R*8
    %               XPOL             : POLAR MOTION (X RAD)                R*8
    %               YPOL             : POLAR MOTION (Y RAD)                R*8
    %               IRCOCN           : OCEAN LOADING FLAG (0: APPLY OL)    I*4
    %               OCNAMP           : OCEAN LOADING AMPLITUDES (M)  R*8(3,MAXOCN)
    %               OCNPHS           : OCEAN LOADING PHASES   (RAD)  R*8(3,MAXOCN)
    %        OUT :  DXTIDE(I),I=1,2,3: DISPLACEMENT VECTOR
    %
    % SR CALLED  :  SPROD, ST1DIU, ST1SEM, ST1L1, STEP2DIU, ST2LON, DMLMTV, OCLOAD
    %
    % AUTHOR     :  V. DEHANT, S. MATHEWS AND J. GIPSON (IERS 1996)
    %                     (TEST BETWEEN TWO SUBROUTINES)
    % AUTHOR     :  V. DEHANT AND S. MATHEWS (IERS 2000)
    %                     (TEST IN THE BERNESE PROGRAM BY C. BRUYNINX)
    %
    % CREATED    :  23-MAR-1996           LAST MODIFIED :  10-JUN-2003
    %
    % CHANGES    :  06-OCT-1997 TS: ADDAPTED FOR THE BERNESE SOFTWARE
    %               10-MAR-1998 TS: ADDED OCEAN LOADING CORRECTIONS
    %               01-FEB-2001 CB: IERS200
    %               02-JUN-2003 CU: ADDAPTED FOR THE BERNESE SOFTWARE V5.0
    %               10-JUN-2003 HU: MASS RATIOS FROM CONST.
    %
    % COPYRIGHT  :  ASTRONOMICAL INSTITUTE
    %      2003     UNIVERSITY OF BERNE
    %               SWITZERLAND

    % NOMINAL SECOND DEGREE AND THIRD DEGREE LOVE NUMBERS AND SHIDA NUMBERS  */

    //SCALAR PRODUCT OF STATION VECTOR WITH SUN/MOON VECTOR
    double SCS  = XSTA.transpose()*sunposCTS;
    double RSTA = MyFunctionCenter::getCoordDistance(XSTA);
    double RSUN = MyFunctionCenter::getCoordDistance(sunposCTS);
    double SCM  = XSTA.transpose()*moonposCTS;
    double RMON = MyFunctionCenter::getCoordDistance(moonposCTS);

    //COMPUTATION OF NEW H2 AND L2
    double SCSUN  = SCS / RSTA / RSUN;
    double SCMON  = SCM / RSTA / RMON;
    //COMPUTATION OF NEW H2 AND L2
    double COSPHI = sqrt(XSTA[0]*XSTA[0] + XSTA[1]*XSTA[1]) / RSTA;
    double H2     = H20 - 0.0006*(1.0 - 1.5*COSPHI*COSPHI);
    double L2     = L20 + 0.0002*(1.0 - 1.5*COSPHI*COSPHI);
    //P2-TERM
    double P2SUN  = 3.0*(H2/2.0 - L2)*SCSUN*SCSUN - H2/2.0;
    double P2MON  = 3.0*(H2/2.0 - L2)*SCMON*SCMON - H2/2.0;
    //P3-TERM
    double P3SUN  = 2.5*(H3-3.0*L3)*SCSUN*SCSUN*SCSUN + 1.5*(L3-H3)*SCSUN;
    double P3MON  = 2.5*(H3-3.0*L3)*SCMON*SCMON*SCMON + 1.5*(L3-H3)*SCMON;
    //TERM IN DIRECTION OF SUN/MOON VECTOR
    double X2SUN  = 3.0*L2*SCSUN;
    double X2MON  = 3.0*L2*SCMON;
    double X3SUN  = 1.5*L3*(5.0*SCSUN*SCSUN - 1.0);
    double X3MON  = 1.5*L3*(5.0*SCMON*SCMON - 1.0);
    //FACTORS FOR SUN/MOON
    double temp1   = AE/RSUN;
    double temp2   = AE/RMON;
    double FAC2SUN = GMS/GM*AE*temp1*temp1*temp1;
    double FAC2MON = GMM/GM*AE*temp2*temp2*temp2;
    double FAC3SUN = FAC2SUN*temp1;
    double FAC3MON = FAC2MON*temp2;

    Vector3d DXTIDE;
    //TOTAL DISPLACEMENT
    for (int i = 0; i < 3; i++)
    {
        DXTIDE[i] = FAC2SUN*( X2SUN*sunposCTS[i]/RSUN  + P2SUN*XSTA[i]/RSTA )+
                    FAC2MON*( X2MON*moonposCTS[i]/RMON + P2MON*XSTA[i]/RSTA )+
                    FAC3SUN*( X3SUN*sunposCTS[i]/RSUN  + P3SUN*XSTA[i]/RSTA )+
                    FAC3MON*( X3MON*moonposCTS[i]/RMON + P3MON*XSTA[i]/RSTA );
    }
    //CORRECTIONS FOR THE OUT-OF-PHASE PART OF LOVE NUMBERS (PART H_2^(0)I
    //            AND L_2^(0)I )
    //FIRST, FOR THE DIURNAL BAND
    Vector3d  XCOSTA;
    XCOSTA  = ST1DIU(XSTA, sunposCTS, moonposCTS, FAC2SUN, FAC2MON);
    DXTIDE += XCOSTA;
    //SECOND, FOR THE SEMI-DIURNAL BAND
    XCOSTA  = ST1SEM(XSTA, sunposCTS, moonposCTS, FAC2SUN, FAC2MON);
    DXTIDE += XCOSTA;
    //CORRECTIONS FOR THE LATITUDE DEPENDENCE OF LOVE NUMBERS (PART L^(1) )
    XCOSTA  = ST1L1(XSTA,sunposCTS,moonposCTS,FAC2SUN,FAC2MON);
    DXTIDE += XCOSTA;
    //CONSIDER CORRECTIONS FOR STEP 2
    //CORRECTIONS FOR THE DIURNAL BAND:
    //FIRST, WE NEED TO KNOW THE DATE CONVERTED IN JULIAN CENTURIES
    double T   = (t.JD - 2451545.0) / 36525.0;
    //AND THE HOUR IN THE DAY
    double FHR =  t.EPT.hour + t.EPT.minute/60 + (t.EPT.second)/3600;

    XCOSTA  = STEP2DIU(XSTA,FHR,T);
    DXTIDE += XCOSTA;
    //CORRECTIONS FOR THE LONG-PERIOD BAND:
    XCOSTA  = ST2LON(XSTA,T);
    DXTIDE += XCOSTA;

    Matrix3d  enu = MyFunctionCenter::getMatrixToENU(coordPar, XSTA);
    DXTIDE  = enu*DXTIDE;
    return  DXTIDE;
}


Vector3d ErrorModel::ST1DIU(const Vector3d &XSTA, const Vector3d &XSUN,
      const Vector3d &XMON, const double   &F2SUN,const double  &F2MON)
{

/*  PURPOSE   :  THIS SUBROUTINE GIVES THE OUT-OF-PHASE CORRECTIONS INDUCED BY
                       MANTLE INELASTICITY IN THE DIURNAL BAND

        PARAMETERS  :
        IN :  XSTA,XSUN,XMON,F2SUN,F2MON
             IN/OUT :  XCOSTA

         REMARKS    :  ---

         AUTHOR     :  V. DEHANT, S. MATHEWS AND J. GIPSON

         VERSION    :  4.1

         CREATED    :  06-OCT-1997           LAST MODIFIED :  06-OCT-1997

         CHANGES    :  06-OCT-97 : TS: ADDEPTED FOR THE BERNESE SOFTWARE

         COPYRIGHT  :  ASTRONOMICAL INSTITUTE
              1997      UNIVERSITY OF BERNE
                            SWITZERLAND*/
    const double DHI = -0.0025;
    const double DLI = -0.0007;
    double RSTA   = MyFunctionCenter::getCoordDistance(XSTA);
    double SINPHI = XSTA[2]/RSTA;
    double COSPHI = sqrt(XSTA[0]*XSTA[0]+XSTA[1]*XSTA[1])/RSTA;
    double COS2PHI= COSPHI*COSPHI-SINPHI*SINPHI;
    double SINLA  = XSTA[1]/COSPHI/RSTA;
    double COSLA  = XSTA[0]/COSPHI/RSTA;
    double RMON   = MyFunctionCenter::getCoordDistance(XMON);
    double RSUN   = MyFunctionCenter::getCoordDistance(XSUN);

    double temp1 =  RSUN*RSUN;
    double temp2 =  RMON*RMON;
    double DRSUN = -3.0*DHI*SINPHI*COSPHI*F2SUN*XSUN[2]*(XSUN[0]*SINLA-XSUN[1]*COSLA)/temp1;
    double DRMON = -3.0*DHI*SINPHI*COSPHI*F2MON*XMON[2]*(XMON[0]*SINLA-XMON[1]*COSLA)/temp2;
    double DNSUN = -3.0*DLI*COS2PHI*F2SUN*XSUN[2]*(XSUN[0]*SINLA-XSUN[1]*COSLA)/temp1;
    double DNMON = -3.0*DLI*COS2PHI*F2MON*XMON[2]*(XMON[0]*SINLA-XMON[1]*COSLA)/temp2;
    double DESUN = -3.0*DLI*SINPHI*F2SUN*XSUN[2]*(XSUN[0]*COSLA+XSUN[1]*SINLA)/temp1;
    double DEMON = -3.0*DLI*SINPHI*F2MON*XMON[2]*(XMON[0]*COSLA+XMON[1]*SINLA)/temp2;

    double DR = DRSUN + DRMON;
    double DN = DNSUN + DNMON;
    double DE = DESUN + DEMON;

    Vector3d XCOSTA;
    XCOSTA[0] = DR*COSLA*COSPHI - DE*SINLA - DN*SINPHI*COSLA;
    XCOSTA[1] = DR*SINLA*COSPHI + DE*COSLA - DN*SINPHI*SINLA;
    XCOSTA[2] = DR*SINPHI+DN*COSPHI;
    return XCOSTA;
}

Vector3d ErrorModel::ST1SEM(const Vector3d &XSTA, const Vector3d &XSUN,
      const Vector3d &XMON, const double   &F2SUN,const double  &F2MON)
{

/*PURPOSE   :  .
            THIS SUBROUTINE GIVES THE OUT-OF-PHASE CORRECTIONS INDUCED BY
            MANTLE INELASTICITY IN THE DIURNAL BAND

            PARAMETERS :
            IN :  XSTA,XSUN,XMON,F2SUN,F2MON
            IN/OUT :  XCOSTA

            REMARKS    :  ---

            AUTHOR     :  V. DEHANT, S. MATHEWS AND J. GIPSON

            VERSION    :  4.1

            CREATED    :  06-OCT-1997           LAST MODIFIED :  06-OCT-1997

            CHANGES    :  06-OCT-97 : TS: ADDEPTED FOR THE BERNESE SOFTWARE

            COPYRIGHT  :  ASTRONOMICAL INSTITUTE
            1997      UNIVERSITY OF BERNE
            SWITZERLAND
*/
    const double DHI = -0.0022;
    const double DLI = -0.0007;
    double RSTA   = MyFunctionCenter::getCoordDistance(XSTA);
    double SINPHI = XSTA[2]/RSTA;
    double COSPHI = sqrt(XSTA[0]*XSTA[0]+XSTA[1]*XSTA[1])/RSTA;
    double SINLA  = XSTA[1]/COSPHI/RSTA;
    double COSLA  = XSTA[0]/COSPHI/RSTA;
    double CTWOLA = COSLA*COSLA-SINLA*SINLA;
    double STWOLA = 2.0*COSLA*SINLA;
    double RMON   = MyFunctionCenter::getCoordDistance(XMON);
    double RSUN   = MyFunctionCenter::getCoordDistance(XSUN);

    double temp1  = RSUN*RSUN;
    double temp2  = RMON*RMON;
    double DRSUN  = -3.0/4.0*DHI*COSPHI*COSPHI*F2SUN*
         ((XSUN[0]*XSUN[0]-XSUN[1]*XSUN[1])*STWOLA-2.0*XSUN[0]*XSUN[1]*CTWOLA)/temp1;
    double DRMON  = -3.0/4.0*DHI*COSPHI*COSPHI*F2MON*
         ((XMON[0]*XMON[0]-XMON[1]*XMON[1])*STWOLA-2.0*XMON[0]*XMON[1]*CTWOLA)/temp2;
    double DNSUN  =  3.0/2.0*DLI*SINPHI*COSPHI*F2SUN*
         ((XSUN[0]*XSUN[0]-XSUN[1]*XSUN[1])*STWOLA-2.0*XSUN[0]*XSUN[1]*CTWOLA)/temp1;
    double DNMON  =  3.0/2.0*DLI*SINPHI*COSPHI*F2MON*
         ((XMON[0]*XMON[0]-XMON[1]*XMON[1])*STWOLA-2.0*XMON[0]*XMON[1]*CTWOLA)/temp2;
    double DESUN  = -3.0/2.0*DLI*COSPHI*F2SUN*
         ((XSUN[0]*XSUN[0]-XSUN[1]*XSUN[1])*CTWOLA+2.0*XSUN[0]*XSUN[1]*STWOLA)/temp1;
    double DEMON  = -3.0/2.0*DLI*COSPHI*F2MON*
         ((XMON[0]*XMON[0]-XMON[1]*XMON[1])*CTWOLA+2.0*XMON[0]*XMON[1]*STWOLA)/temp2;

    double DR = DRSUN+DRMON;
    double DN = DNSUN+DNMON;
    double DE = DESUN+DEMON;
    Vector3d  XCOSTA;
    XCOSTA[0] = DR*COSLA*COSPHI - DE*SINLA - DN*SINPHI*COSLA;
    XCOSTA[1] = DR*SINLA*COSPHI + DE*COSLA - DN*SINPHI*SINLA;
    XCOSTA[2] = DR*SINPHI + DN*COSPHI;
    return XCOSTA;
}

Vector3d ErrorModel::ST1L1 (const Vector3d &XSTA, const Vector3d &XSUN,
      const Vector3d &XMON, const double   &F2SUN,const double  &F2MON)
{
/*PURPOSE   :  .
            THIS SUBROUTINE GIVES THE CORRECTIONS INDUCED BY THE LATITUDE DEPENDENCE
            GIVEN BY L^(1) IN MAHTEWS ET AL (1991)

            PARAMETERS :
            IN :  XSTA,XSUN,XMON,F3SUN,F3MON
            IN/OUT :  XCOSTA

            REMARKS    :  ---

            AUTHOR     :  V. DEHANT, S. MATHEWS AND J. GIPSON

            VERSION    :  4.1

            CREATED    :  06-OCT-1997           LAST MODIFIED :  06-OCT-1997

            CHANGES    :  06-OCT-97 : TS: ADDEPTED FOR THE BERNESE SOFTWARE

            COPYRIGHT  :  ASTRONOMICAL INSTITUTE
            1997      UNIVERSITY OF BERNE
            SWITZERLAND
*/
    Vector3d XCOSTA;
    const double L1D  = 0.00120;
    const double L1SD = 0.00240;
    double RSTA   = MyFunctionCenter::getCoordDistance(XSTA);
    double SINPHI = XSTA[2]/RSTA;
    double COSPHI = sqrt(XSTA[0]*XSTA[0]+XSTA[1]*XSTA[1])/RSTA;
    double SINLA  = XSTA[1]/COSPHI/RSTA;
    double COSLA  = XSTA[0]/COSPHI/RSTA;
    double RMON   = MyFunctionCenter::getCoordDistance(XMON);
    double RSUN   = MyFunctionCenter::getCoordDistance(XSUN);

    double temp1  = RSUN*RSUN;
    double temp2  = RMON*RMON;
    // FOR THE DIURNAL BAND
    double L1    =  L1D;
    double DNSUN = -L1*SINPHI*SINPHI*F2SUN*XSUN[2]*(XSUN[0]*
                    COSLA+XSUN[1]*SINLA)/temp1;
    double DNMON = -L1*SINPHI*SINPHI*F2MON*XMON[2]*(XMON[0]*
                    COSLA+XMON[1]*SINLA)/temp2;
    double DESUN =  L1*SINPHI*(COSPHI*COSPHI-SINPHI*SINPHI)*F2SUN*XSUN[2]*
                   (XSUN[0]*SINLA-XSUN[1]*COSLA)/temp1;
    double DEMON =  L1*SINPHI*(COSPHI*COSPHI-SINPHI*SINPHI)*F2MON*XMON[2]*
                   (XMON[0]*SINLA-XMON[1]*COSLA)/temp2;

    double DE =  3.0*(DESUN+DEMON);
    double DN =  3.0*(DNSUN+DNMON);
    XCOSTA[0] = -DE*SINLA-DN*SINPHI*COSLA;
    XCOSTA[1] =  DE*COSLA-DN*SINPHI*SINLA;
    XCOSTA[2] =  DN*COSPHI;
    //FOR THE SEMI-DIURNAL BAND
    L1    =   L1SD;
    double    CTWOLA = COSLA*COSLA-SINLA*SINLA;
    double    STWOLA = 2.0*COSLA*SINLA;
    DNSUN =  -L1/2.0*SINPHI*COSPHI*F2SUN*((XSUN[0]*XSUN[0]-XSUN[1]*XSUN[1])*
              CTWOLA+2.0*XSUN[0]*XSUN[1]*STWOLA)/temp1;
    DNMON =  -L1/2.0*SINPHI*COSPHI*F2MON*((XMON[0]*XMON[0]-XMON[1]*XMON[1])*
              CTWOLA+2.0*XMON[0]*XMON[1]*STWOLA)/temp2;
    DESUN =  -L1/2.0*SINPHI*SINPHI*COSPHI*F2SUN*((XSUN[0]*XSUN[0]-XSUN[1]*XSUN[1])*
              STWOLA-2.0*XSUN[0]*XSUN[1]*CTWOLA)/temp1;
    DEMON =  -L1/2.0*SINPHI*SINPHI*COSPHI*F2MON*((XMON[0]*XMON[0]-XMON[1]*XMON[1])*
              STWOLA-2.0*XMON[0]*XMON[1]*CTWOLA)/temp2;
    DE = 3.0*(DESUN+DEMON);
    DN = 3.0*(DNSUN+DNMON);

    XCOSTA[0] = XCOSTA[0]-DE*SINLA-DN*SINPHI*COSLA;
    XCOSTA[1] = XCOSTA[1]+DE*COSLA-DN*SINPHI*SINLA;
    XCOSTA[2] = XCOSTA[2]+DN*COSPHI;
    return XCOSTA;
}

Vector3d ErrorModel::STEP2DIU(const Vector3d &XSTA, const double &FHR,
                              const double &T)
{
/*PURPOSE    :  THESE ARE THE SUBROUTINES FOR THE STEP2 OF THE TIDAL
                CORRECTIONS. THEY ARE CALLED TO ACCOUNT FOR THE FREQUENCY
                DEPENDENCE OF THE LOVE NUMBERS.

                CONSISTENT WITH IERS CONVENTIONS 2000

                PARAMETERS :
                IN :  XSTA,FHR,T
                IN/OUT :  XCORSTA

                AUTHOR     :  V. DEHANT, S. MATHEWS AND J. GIPSON

                VERSION    :  4.1

                CREATED    :  17-MAY-2000           LAST MODIFIED :  02-JUN-2003

                CHANGES    :  02-JUN-2003 CU: ADDAPTED FOR THE BERNESE SOFTWARE V5.0
                              10-JUN-2003 HU: D0 ADDED

                COPYRIGHT  :  ASTRONOMICAL INSTITUTE
                              2003     UNIVERSITY OF BERNE
                              SWITZERLAND
*/
    Vector3d XCORSTA;
    double   DATDI[31][9] = {
    {-3.0, 0.0, 2.0, 0.0, 0.0,-0.010,-0.010, 0.00 , 0.00 },
    {-3.0, 2.0, 0.0, 0.0, 0.0,-0.010,-0.010, 0.00 , 0.00 },
    {-2.0, 0.0, 1.0,-1.0, 0.0,-0.020,-0.010, 0.00 , 0.00 },
    {-2.0, 0.0, 1.0, 0.0, 0.0,-0.080,-0.050,-0.010,-0.020},
    {-2.0, 2.0,-1.0, 0.0, 0.0,-0.020,-0.010, 0.00 , 0.00 },
    {-1.0, 0.0, 0.0,-1.0, 0.0,-0.100,-0.050, 0.00 ,-0.020},
    {-1.0, 0.0, 0.0, 0.0, 0.0,-0.510,-0.260,-0.020,-0.120},
    {-1.0, 2.0, 0.0, 0.0, 0.0, 0.010, 0.00 , 0.00 , 0.00 },
    { 0.0,-2.0, 1.0, 0.0, 0.0, 0.010, 0.00 , 0.00 , 0.00 },
    { 0.0, 0.0,-1.0, 0.0, 0.0, 0.020, 0.010, 0.0  , 0.0  },
    { 0.0, 0.0, 1.0, 0.0, 0.0, 0.060, 0.020, 0.000, 0.010},
    { 0.0, 0.0, 1.0, 1.0, 0.0, 0.010, 0.0  , 0.0  ,  0.0 },
    { 0.0, 2.0,-1.0, 0.0, 0.0, 0.010, 0.00 , 0.00 , 0.00 },
    { 1.0,-3.0, 0.0, 0.0, 1.0,-0.060, 0.00 , 0.00 , 0.00 },
    { 1.0,-2.0, 0.0,-1.0, 0.0, 0.010, 0.00 , 0.00 , 0.00 },
    { 1.0,-2.0, 0.0, 0.0, 0.0,-1.230,-0.050, 0.060,-0.060},
    { 1.0,-1.0, 0.0, 0.0,-1.0, 0.020, 0.00 , 0.00 , 0.00 },
    { 1.0,-1.0, 0.0, 0.0, 1.0, 0.040, 0.00 , 0.00 , 0.00 },
    { 1.0, 0.0, 0.0,-1.0, 0.0,-0.220, 0.010, 0.010, 0.00 },
    { 1.0, 0.0, 0.0, 0.0, 0.0,12.020,-0.450,-0.660, 0.170},
    { 1.0, 0.0, 0.0, 1.0, 0.0, 1.730,-0.070,-0.100, 0.020},
    { 1.0, 0.0, 0.0, 2.0, 0.0,-0.040, 0.00 , 0.00 , 0.00 },
    { 1.0, 1.0, 0.0, 0.0,-1.0,-0.500, 0.00 , 0.030, 0.00 },
    { 1.0, 1.0, 0.0, 0.0, 1.0, 0.010, 0.00 , 0.00 , 0.00 },
    { 0.0, 1.0, 0.0, 1.0,-1.0,-0.010, 0.00 , 0.00 , 0.00 },
    { 1.0, 2.0,-2.0, 0.0, 0.0,-0.010, 0.00 , 0.00 , 0.00 },
    { 1.0, 2.0, 0.0, 0.0, 0.0,-0.120, 0.010, 0.010, 0.00 },
    { 2.0,-2.0, 1.0, 0.0, 0.0,-0.010, 0.00 , 0.00 , 0.00 },
    { 2.0, 0.0,-1.0, 0.0, 0.0,-0.020, 0.020, 0.00 , 0.010},
    { 3.0, 0.0, 0.0, 0.0, 0.0, 0.00 , 0.010, 0.00 , 0.010},
    { 3.0, 0.0, 0.0, 1.0, 0.0, 0.00 , 0.010, 0.00 , 0.00}};
    double TT  = T*T;
    double TTT = T*T*T;
    double TTTT= T*T*T*T;
    double S   = 218.316645630+481267.881940*T-0.00146638890*TT+0.000001851390*TTT;
    double TAU = FHR*15.0+280.46061840+36000.77005360*T+0.000387930*TT-0.00000002580*TTT-S;
    double PR  = 1.3969712780*T+0.0003088890*TT+0.0000000210*TTT+0.0000000070*TTTT;
    S   +=PR;
    double H   = 280.466450+36000.76974890   *T + 0.000303222220*TT +
                             0.0000000200 * TTT - 0.000000006540*TTTT;
    double P   = 83.353243120+4069.013635250 *T - 0.010321722220*TT -
                             0.00001249910 *TTT + 0.000000052630*TTTT;
    double ZNS = 234.955444990+1934.136261970*T - 0.002075611110*TT -
                             0.000002139440*TTT + 0.000000016500*TTTT;
    double PS  = 282.937340980+1.719457666670*T + 0.000456888890*TT -
                             0.000000017780*TTT - 0.000000003340*TTTT;
    //REDUCE ANGLES TO BETWEEN 0 AND 360
    S   = fmod(S,  360.0);
    TAU = fmod(TAU,360.0);
    H   = fmod(H,  360.0);
    P   = fmod(P,  360.0);
    ZNS = fmod(ZNS,360.0);
    PS  = fmod(PS, 360.0);

    double RSTA   = MyFunctionCenter::getCoordDistance(XSTA);
    double SINPHI = XSTA[2]/RSTA;
    double COSPHI = sqrt(XSTA[0]*XSTA[0]+XSTA[1]*XSTA[1])/RSTA;
    double SINLA  = XSTA[1]/COSPHI/RSTA;
    double COSLA  = XSTA[0]/COSPHI/RSTA;
    double ZLA    = atan2(XSTA[1],XSTA[0]);
    for (int i = 0; i < 3; i++)
    {
            XCORSTA[i]=0.0;
    }
    double THETAF,DR,DN,DE;
    double COSSIN_2=COSPHI*COSPHI-SINPHI*SINPHI;
    for (int J = 0;J < 31; J++)
    {
            THETAF = (TAU+DATDI[J][0]*S+DATDI[J][1]*H+DATDI[J][2]*P+DATDI[J][3]*ZNS+DATDI[J][4]*PS)*D2R;
            DR = DATDI[J][5]*2.0*SINPHI*COSPHI*sin(THETAF+ZLA)+DATDI[J][6]*2.0*SINPHI*COSPHI*cos(THETAF+ZLA);
            DN = DATDI[J][7]*COSSIN_2*sin(THETAF+ZLA)+DATDI[J][8]*COSSIN_2*cos(THETAF+ZLA);
            DE = DATDI[J][7]*SINPHI*cos(THETAF+ZLA)+DATDI[J][8]*SINPHI*sin(THETAF+ZLA);
            XCORSTA[0] = XCORSTA[0]+DR*COSLA*COSPHI-DE*SINLA-DN*SINPHI*COSLA;
            XCORSTA[1] = XCORSTA[1]+DR*SINLA*COSPHI+DE*COSLA-DN*SINPHI*SINLA;
            XCORSTA[2] = XCORSTA[2]+DR*SINPHI+DN*COSPHI;
    }
    XCORSTA[0] /= 1000.0;
    XCORSTA[1] /= 1000.0;
    XCORSTA[2] /= 1000.0;
    return XCORSTA;
}

Vector3d ErrorModel::ST2LON(const Vector3d &XSTA, const double &T)
{
/*  PURPOSE    :  .

    PARAMETERS :
         IN :  XSTA,FHR,T,
             IN/OUT :  XCOSTA

         REMARKS    :  ---

         AUTHOR     :  V. DEHANT, S. MATHEWS AND J. GIPSON

         VERSION    :  4.1

         CREATED    :  06-OCT-1997           LAST MODIFIED :  06-OCT-1997

         CHANGES    :  06-OCT-97 : TS: ADDEPTED FOR THE BERNESE SOFTWARE

         COPYRIGHT  :  ASTRONOMICAL INSTITUTE
              1997      UNIVERSITY OF BERNE
                            SWITZERLAND*/
    Vector3d XCOSTA;
    double DATDI[5][9]={
            {0, 0, 0, 1, 0,   0.470, 0.230, 0.160, 0.070},
            {0, 2, 0, 0, 0,  -0.200,-0.120,-0.110,-0.050},
            {1, 0,-1, 0, 0,  -0.110,-0.080,-0.090,-0.040},
            {2, 0, 0, 0, 0,  -0.130,-0.110,-0.150,-0.070},
            {2, 0, 0, 1, 0,  -0.050,-0.050,-0.060,-0.030}};
    double TT  = T*T;
    double TTT = T*T*T;
    double TTTT= T*T*T*T;
    double S   = 218.316645630+481267.881940*T-0.00146638890*TT+0.000001851390*TTT;
    double PR  = 1.3969712780*T+0.0003088890*TT+0.0000000210*TTT+0.0000000070*TTTT;
    S+=PR;
    double H   = 280.466450+36000.76974890    *T + 0.000303222220*TT +
                                0.0000000200*TTT - 0.000000006540*TTTT;
    double P   = 83.353243120+4069.013635250  *T - 0.010321722220*TT -
                               0.00001249910*TTT + 0.000000052630*TTTT;
    double ZNS = 234.955444990 +1934.136261970*T - 0.002075611110*TT -
                              0.000002139440*TTT + 0.000000016500*TTTT;
    double PS  = 282.937340980+1.719457666670 *T + 0.000456888890*TT -
                              0.000000017780*TTT - 0.000000003340*TTTT;
    //REDUCE ANGLES TO BETWEEN 0 AND 360
    S   = fmod(S,  360.0);
    H   = fmod(H,  360.0);
    P   = fmod(P,  360.0);
    ZNS = fmod(ZNS,360.0);
    PS  = fmod(PS, 360.0);

    double RSTA   = MyFunctionCenter::getCoordDistance(XSTA);
    double SINPHI = XSTA[2]/RSTA;
    double COSPHI = sqrt(XSTA[0]*XSTA[0]+XSTA[1]*XSTA[1])/RSTA;
    double SINLA  = XSTA[1]/COSPHI/RSTA;
    double COSLA  = XSTA[0]/COSPHI/RSTA;
    XCOSTA[0] = 0.0;
    XCOSTA[1] = 0.0;
    XCOSTA[2] = 0.0;
    double THETAF, DR, DN, DE = 0.0;
    double SINPHI2 = SINPHI*SINPHI;
    for (int J = 0; J < 5; J++)
    {
            THETAF = (DATDI[J][0]*S+DATDI[J][1]*H+DATDI[J][2]*P+DATDI[J][3]*ZNS+DATDI[J][4]*PS)*D2R;
            DR = DATDI[J][5]*(3.0*SINPHI2-1.0)/2.0*cos(THETAF)+DATDI[J][7]*(3.0*SINPHI2-1.0)/2.0*sin(THETAF);
            DN = DATDI[J][6]*(COSPHI*SINPHI*2.0)*cos(THETAF)+DATDI[J][8]*(COSPHI*SINPHI*2.0)*sin(THETAF);

            XCOSTA[0] = XCOSTA[0]+DR*COSLA*COSPHI-DE*SINLA-DN*SINPHI*COSLA;
            XCOSTA[1] = XCOSTA[1]+DR*SINLA*COSPHI+DE*COSLA-DN*SINPHI*SINLA;
            XCOSTA[2] = XCOSTA[2]+DR*SINPHI+DN*COSPHI;
    }
    XCOSTA[0] /= 1000.0;
    XCOSTA[1] /= 1000.0;
    XCOSTA[2] /= 1000.0;
    return XCOSTA;
}



/*............................         Tropospheric delay Models          ........................................*/
/*                    -----------------------------------------------------                                       */
/*------------------------------------------------------------------------------
 * Name     : UNB3M
 * Function : Use UNB3 model to get Tropospheric delay correction
 * Input    : const Vector3d &recBLH .(Latitude, Longitude, Height)
 *            const double &Doy .(Day of year)
 *            const double &Elevation .(Elevation angle (radians))
 * Output   : VectorXd (Five results of tropospheric delay correction)
 *          .(HZD     Hydrostatic zenith delay (m)
              HMF     Hydrostatic Niell mapping function
              WZD     Non-hyd. zenith delay (m)
              WMF     Non-hyd. Niell mapping function
              RTROP   Total slant delay (m))
 *-----------------------------------------------------------------------------*/
VectorXd TroposphericDelayCorrection::UNB3M(const Vector3d &recBLH, const double &Doy,
                                            const double &Elevation)
{
    double LATRAD  = recBLH[0];
    double HEIGHTM = recBLH[2];

    MatrixXd AVG(5,6);
    AVG <<   15.0,  1013.25,  299.65,  75.00,  6.30,  2.77,
             30.0,  1017.25,  294.15,  80.00,  6.05,  3.15,
             45.0,  1015.75,  283.15,  76.00,  5.58,  2.57,
             60.0,  1011.75,  272.15,  77.50,  5.39,  1.81,
             75.0,  1013.00,  263.65,  82.50,  4.53,  1.55;
    MatrixXd AMP(5,6);
    AMP <<   15.0,  0.00,  0.00 ,  0.00,  0.00,  0.00,
             30.0, -3.75,  7.00 ,  0.00,  0.25,  0.33,
             45.0, -2.25,  11.00, -1.00,  0.32,  0.46,
             60.0, -1.75,  15.00, -2.50,  0.81,  0.74,
             75.0, -0.50,  14.50,  2.50,  0.62,  0.30;

    double EXCEN2 = 6.6943799901413e-03;
    double MD     = 28.9644;
    double MW     = 18.0152;
    double K1     = 77.604;
    double K2     = 64.79;
    double K3     = 3.776e5;
    double R      = 8314.34;
    double C1     = 2.2768e-03;
    double K2PRIM = K2 - K1*(MW/MD);
    double RD     = R / MD;

    double DOY2RAD=(0.31415926535897935601e01)*2/365.25;
    MatrixXd   ABC_AVG(5,4);
    ABC_AVG << 15.0,  1.2769934e-3,  2.9153695e-3,  62.610505e-3,
               30.0,  1.2683230e-3,  2.9152299e-3,  62.837393e-3,
               45.0,  1.2465397e-3,  2.9288445e-3,  63.721774e-3,
               60.0,  1.2196049e-3,  2.9022565e-3,  63.824265e-3,
               75.0,  1.2045996e-3,  2.9024912e-3,  64.258455e-3;
    MatrixXd   ABC_AMP(5,4);
    ABC_AMP << 15.0,  0.0         ,  0.0         ,  0.0         ,
               30.0,  1.2709626e-5,  2.1414979e-5,  9.0128400e-5,
               45.0,  2.6523662e-5,  3.0160779e-5,  4.3497037e-5,
               60.0,  3.4000452e-5,  7.2562722e-5,  84.795348e-5,
               75.0,  4.1202191e-5,  11.723375e-5,  170.37206e-5;

    double A_HT = 2.53e-5;
    double B_HT = 5.49e-3;
    double C_HT = 1.14e-3;
    double HT_TOPCON = 1 + A_HT/(1 + B_HT/(1 + C_HT));

    MatrixXd ABC_W2P0(5,4);
    ABC_W2P0<< 15.0,  5.8021897e-4,  1.4275268e-3,  4.3472961e-2,
               30.0,  5.6794847e-4,  1.5138625e-3,  4.6729510e-2,
               45.0,  5.8118019e-4,  1.4572752e-3,  4.3908931e-2,
               60.0,  5.9727542e-4,  1.5007428e-3,  4.4626982e-2,
               75.0,  6.1641693e-4,  1.7599082e-3,  5.4736038e-2;

    double LATDEG = LATRAD * ToDEG;
    double TD_O_Y = Doy;
    if (LATDEG < 0)
        TD_O_Y = TD_O_Y + 182.625;
    double COSPHS  = cos((TD_O_Y - 28) * DOY2RAD);
    double LAT     = abs (LATDEG);

    double P1 = 0, P2 = 0, M = 0;
    if      (LAT  >= 75)
        P1 = 5, P2 = 5, M = 0;
    else if (LAT  <= 15)
        P1 = 1, P2 = 1, M = 0;
    else
    {
        P1 = int((LAT - 15)/15) + 1;
        P2 = P1 + 1;
        double aa =  LAT - AVG(P1-1,0);
        double bb =  AVG(P2-1,0) - AVG(P1-1,0);
        M = (aa)  / (bb);
    }

    double PAVG      = M * (AVG(P2-1,1) - AVG(P1-1,1)) + AVG(P1-1,1);
    double TAVG      = M * (AVG(P2-1,2) - AVG(P1-1,2)) + AVG(P1-1,2);
    double EAVG      = M * (AVG(P2-1,3) - AVG(P1-1,3)) + AVG(P1-1,3);
    double BETAAVG   = M * (AVG(P2-1,4) - AVG(P1-1,4)) + AVG(P1-1,4);
    double LAMBDAAVG = M * (AVG(P2-1,5) - AVG(P1-1,5)) + AVG(P1-1,5);

    double PAMP      = M * (AMP(P2-1,1) - AMP(P1-1,1)) + AMP(P1-1,1);
    double TAMP      = M * (AMP(P2-1,2) - AMP(P1-1,2)) + AMP(P1-1,2);
    double EAMP      = M * (AMP(P2-1,3) - AMP(P1-1,3)) + AMP(P1-1,3);
    double BETAAMP   = M * (AMP(P2-1,4) - AMP(P1-1,4)) + AMP(P1-1,4);
    double LAMBDAAMP = M * (AMP(P2-1,5) - AMP(P1-1,5)) + AMP(P1-1,5);

    double P0   = PAVG - PAMP * COSPHS;
    double T0   = TAVG - TAMP * COSPHS;
    double E0   = EAVG - EAMP * COSPHS;
    double BETA = BETAAVG - BETAAMP * COSPHS;
    BETA = BETA / 1000;
    double LAMBDA = LAMBDAAVG - LAMBDAAMP * COSPHS;

    double ES = 0.01 * exp(1.2378847e-5 * (pow(T0 ,2))- 1.9121316e-2 * T0 +
                           3.393711047e1 - 6.3431645e3 * (pow(T0 ,-1)));
    double FW = 1.00062 + 3.14e-6 * P0 + 5.6e-7 * (pow((T0 - 273.15) , 2));
    E0 =  (E0 / 1.00e2) * ES * FW;

    double EP = 9.80665 / 287.054 / BETA;

    double T = T0 - BETA * HEIGHTM;
    double P = P0 * pow(( T / T0),  EP);
    double E = E0 * pow(( T / T0), (EP * (LAMBDA+1)));

    double GEOLAT = atan((1.0-EXCEN2)*tan(LATRAD));
    double DGREF  = 1.0 - 2.66e-03*cos(2.0*GEOLAT) - 2.8e-07*HEIGHTM;
    double GM     = 9.784 * DGREF;
    double DEN    =(LAMBDA + 1.0 ) * GM;

    double TM  = T * (1 - BETA * RD / DEN);

    double HZD = C1 / DGREF * P;

    double WZD = 1.0e-6 * (K2PRIM + K3/TM) * RD * E/DEN;

    double A_AVG = M * (ABC_AVG(P2-1,1) - ABC_AVG(P1-1,1)) + ABC_AVG(P1-1,1);
    double B_AVG = M * (ABC_AVG(P2-1,2) - ABC_AVG(P1-1,2)) + ABC_AVG(P1-1,2);
    double C_AVG = M * (ABC_AVG(P2-1,3) - ABC_AVG(P1-1,3)) + ABC_AVG(P1-1,3);

    double A_AMP = M * (ABC_AMP(P2-1,1) - ABC_AMP(P1-1,1)) + ABC_AMP(P1-1,1);
    double B_AMP = M * (ABC_AMP(P2-1,2) - ABC_AMP(P1-1,2)) + ABC_AMP(P1-1,2);
    double C_AMP = M * (ABC_AMP(P2-1,3) - ABC_AMP(P1-1,3)) + ABC_AMP(P1-1,3);

    double A     = A_AVG - A_AMP * COSPHS;
    double B     = B_AVG - B_AMP * COSPHS;
    double C     = C_AVG - C_AMP * COSPHS;
    double SINE  = sin(Elevation);

    double ALPHA  = B /(SINE + C );
    double GAMMA  = A /(SINE + ALPHA);
    double TOPCON =(1 + A/(1 + B/(1 + C)));
    double HMF    = TOPCON / ( SINE + GAMMA );

    ALPHA  = B_HT /(SINE + C_HT );
    GAMMA  = A_HT /(SINE + ALPHA);
    double HT_CORR_COEF = 1/SINE - HT_TOPCON/(SINE + GAMMA);
    double HT_CORR      = HT_CORR_COEF * HEIGHTM / 1000;
    HMF          = HMF + HT_CORR;

    A = M * ( ABC_W2P0(P2-1,1) - ABC_W2P0(P1-1,1) ) + ABC_W2P0(P1-1,1);
    B = M * ( ABC_W2P0(P2-1,2) - ABC_W2P0(P1-1,2) ) + ABC_W2P0(P1-1,2);
    C = M * ( ABC_W2P0(P2-1,3) - ABC_W2P0(P1-1,3)) + ABC_W2P0(P1-1,3);

    ALPHA  =      B/( SINE + C );
    GAMMA  =      A/( SINE + ALPHA);
    TOPCON = (1 + A/( 1 + B/(1 + C)));
    double WMF    = TOPCON / ( SINE + GAMMA );
    double RTROP  = HZD*HMF + WZD*WMF;

    VectorXd  RESULT(5);
    RESULT << RTROP, HZD, HMF, WZD, WMF;
    return RESULT;
}
