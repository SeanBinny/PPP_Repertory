#include "DataProcessCenter.h"
#include <algorithm>



/*-------------------------------------------------------------------------------
 * Name     : seekRecAntDataPos
 * Function : Get index of proper recvier antenna data from data array
 * Input    : const AntennaInfoFile &antennaDataBase,
 *            QString               &recType
 * Output   : int (data position, -1 means faild)
 *------------------------------------------------------------------------------*/
int DataMatchingCenter::seekRecAntDataPos(const AntennaInfoFile
                                          &antennaDataBase, QString recType)
{
    for (unsigned int i = 0; i < antennaDataBase.antennaData_Other.size(); i++)
    {
        if (antennaDataBase.antennaData_Other[i].antennaType == recType)
            return i;
    }
    return -1;
}

/*-------------------------------------------------------------------------------
 * Name     : seekOceanDataPos
 * Function : Get index of proper ocean tide data from data array
 * Input    : const OceanTideFile   &oceanDatabase,
 *            QString               markerName
 * Output   : int (data position, -1 means faild)
 *------------------------------------------------------------------------------*/
int DataMatchingCenter::seekOceanDataPos(const OceanTideFile
                                         &oceanDatabase, QString markerName)
{
    for (unsigned int i = 0; i < oceanDatabase.allOceanData.size(); i++)
    {
        if (oceanDatabase.allOceanData[i].stationName.
            compare(markerName, Qt::CaseInsensitive) == 0)                        // Not sensitive to case
            return i;
    }
    return -1;
}

/*-------------------------------------------------------------------------------
 * Name     : seekErpDataPos
 * Function : Get index of proper earth rotation parameter data from data array
 * Input    : const EarthRotationParameterFile  &erpDataBase,
 *            double    JD
 * Output   : int (data position, -1 means faild)
 *------------------------------------------------------------------------------*/
int DataMatchingCenter::seekErpDataPos(const EarthRotationParameterFile
                                       &erpDataBase, double JD)
{
    for (unsigned int i = 0; i < erpDataBase.allErpData.size(); i++)
    {
        if (fabs(JD - erpDataBase.allErpData[i].myTime.JD) < 1)
            return i;
    }
    return -1;
}

/*-------------------------------------------------------------------------------
 * Name     : seekPreEphDataPos
 * Function : Get index of proper precision ephemeris data from data array
 * Input    : const PrecisionEphemerisFile  &preEphDataBase,
 *            double matchTime,          int lastPosInArray
 * Output   : int (data position, -1 means faild)
 *------------------------------------------------------------------------------*/
int DataMatchingCenter::seekPreEphDataPos(const PrecisionEphemerisFile
                                          &preEphDataBase, const double &matchTime,
                                          const int &lastPosInArray)
{
    int resPos = epochMatch(preEphDataBase.allPrecionData,
                                matchTime, lastPosInArray);
    return resPos;
}

/*-------------------------------------------------------------------------------
 * Name     : seekPreClockPos
 * Function : Get index of proper precision clock error data from data array
 * Input    : const PrecisionClockFile   &preClkDataBase,
 *            double matchTime,          int lastPosInArray
 * Output   : int (data position, -1 means faild)
 *------------------------------------------------------------------------------*/
int DataMatchingCenter::seekPreClockPos(const PrecisionClockFile
                                        &preClkDataBase, const double &matchTime,
                                        const int &lastPosInArray)
{
    int resPos = epochMatch(preClkDataBase.satelliteClkData,
                                  matchTime, lastPosInArray);
    return resPos;
}
/*-------------------------------------------------------------------------------
 * Name     : seekApproxCoordPos
 * Function : Get index of proper precision approx coordinate
 * Input    : const IgsStationCoordinateFile &coordDataBase,
 *            QString stationName,
 *            int  date
 * Output   : int (data position, -1 means faild)
 *------------------------------------------------------------------------------*/
int DataMatchingCenter::seekApproxCoordPos(const IgsStationCoordinateFile &coordDataBase,
                                                 QString stationName, int  date)
{
    for(unsigned int  i = 0; i < coordDataBase.stationCoordinateData.size(); i++)
    {
        if(coordDataBase.stationCoordinateData[i].MARKER_NAME.
           compare(stationName, Qt::CaseInsensitive)    == 0 &&
           coordDataBase.stationCoordinateData[i].date  == date)
        {
            return i;
        }
    }
    return -1;
}
/*-------------------------------------------------------------------------------
 * Name     : seekSunPosition
 * Function : Returns the position of Sun ECEF coordinates (meters) at the
 *            indicated time.
 * Input    : const MyTime &tdtTime .(The time to look up)
 * Output   : Vector3d (the position of the Sun at time(as a Triple))
 *------------------------------------------------------------------------------*/
Vector3d DataMatchingCenter::seekSunPosition(const MyTime &tdtTime)
{
    if ((tdtTime.JD < 2415077.5 || tdtTime.JD > 2488129.5))
        QMessageBox::critical(NULL,"error","time is out of range, "
                              "please check time!",QMessageBox::Ok);
    Vector3d posCIS = getSunPosCIS(tdtTime);
    Vector3d posCTS = MyFunctionCenter::CIStoCTS(posCIS, tdtTime);
    return   posCTS;
}

/*-------------------------------------------------------------------------------
 * Name     : seekSunPosition
 * Function : Returns the position of Moon ECEF coordinates (meters) at the
 *            indicated time.
 * Input    : const MyTime &tdtTime .(The time to look up)
 * Output   : Vector3d (the position of the Sun at time(as a Triple))
 *------------------------------------------------------------------------------*/
Vector3d DataMatchingCenter::seekMoonPosition(const MyTime &tdtTime)
{
    if ((tdtTime.JD < 2415077.5 || tdtTime.JD > 2488129.5))
        QMessageBox::critical(NULL,"error","time is out of range, "
                              "please check time!",QMessageBox::Ok);
    Vector3d posCIS = getMoonPosCIS(tdtTime);
    Vector3d posCTS = MyFunctionCenter::CIStoCTS(posCIS, tdtTime);
    return   posCTS;
}

/*-------------------------------------------------------------------------------
 * Name     : getSunPosCIS
 * Function : Function to compute Sun position in CIS system (coordinates in meters)
 * Input    : const MyTime &tdtTime .(The time to look up)
 * Output   : Vector3d (the position of the Sun at time(as a Triple))
 *------------------------------------------------------------------------------*/
Vector3d DataMatchingCenter::getSunPosCIS(const MyTime &tdtTime)
{
    double MeanEarthMoonBary = 3.12e-5;                                           // Mean Earth-Moon barycenter (EMB) distance (AU).

    /*--Compute the years, and fraction of year, pased since J1900.0-------------*/
    int    y        =  tdtTime.EPT.year;                                          // Current year
    int    doy      =  tdtTime.DOY;                                               // Day of current year
    double secOfDay =  tdtTime.EPT.hour*3600 + tdtTime.EPT.minute*60 +            // Time must be convert to seconds
                                               tdtTime.EPT.second;
    double dayFrac  =  secOfDay / 86400.0;                                        // Compute fraction of day
    int    years    =  y - 1900;                                                  // Integer number of years since J1900.0
    int    iy4      =  y % 4;
    double yearFrac =((4*(doy - 1.0/(iy4 + 1)) - iy4 - 2) + 4.0*dayFrac) / 1461.0;//Compute fraction of year
    double times    =  years + yearFrac;
    /*---------------------------------------------------------------------------*/

    double elm      =  fmod((4.881628 + 2*PI*yearFrac + 0.0001342*times), 2*PI);
    double gamma    =        4.90823  + 0.00030005*times;
    double em       =  elm - gamma;
    double eps0     =  0.40931975 - 2.27e-6*times;
    double e        =  0.016751   - 4.2e-7 *times;
    double esq      =  e*e;
    double v        =  em  + 2.0*e*sin(em) + 1.25*esq*sin(2.0*em);
    double elt      =  v   + gamma;
    double r        = (1.0 - esq)/(1.0+e*cos(v));
    double elmm     =  fmod((4.72 + 83.9971*times), 2*PI);

    double w1       = -sin(elt )*r;
    double selmm    =  sin(elmm);
    double celmm    =  cos(elmm);

    Vector3d            sunposCIS;
    sunposCIS[0]    = ( r*cos(elt ) + MeanEarthMoonBary*celmm)*AU_CONST;
    sunposCIS[1]    = ( MeanEarthMoonBary*selmm-w1)*cos(eps0 )*AU_CONST;
    sunposCIS[2]    = (-w1*sin(eps0))*AU_CONST;
    return              sunposCIS;

}
/*-------------------------------------------------------------------------------
 * Name     : getMoonPosCIS
 * Function : Function to compute Moon position in CIS system (coordinates in meters)
 * Input    : const MyTime &tdtTime .(The time to look up)
 * Output   : Vector3d (the position of the Sun at time(as a Triple))
 *------------------------------------------------------------------------------*/
Vector3d DataMatchingCenter::getMoonPosCIS(const MyTime &tdtTime)
{
    double TL[50];
    double TB[45];
    double TP[31];
    int ITL[5][50];
    int ITB[5][45];
    int ITP[5][31];

    TL[0]= +6.288750;
    ITL[0][0]=0;ITL[1][0]=1;ITL[2][0]=0;ITL[3][0]=0;ITL[4][0]=0;

    TL[1]= +1.274018;
    ITL[0][1]=0;ITL[1][1]=-1;ITL[2][1]=2;ITL[3][1]=0;ITL[4][1]=0;

    TL[2]= +0.658309;
    ITL[0][2]=0;ITL[1][2]=0;ITL[2][2]=2;ITL[3][2]=0;ITL[4][2]=0;

    TL[3]= +0.213616;
    ITL[0][3]=0;ITL[1][3]=2;ITL[2][3]=0;ITL[3][3]=0;ITL[4][3]=0;

    TL[4]=-0.185596;
    ITL[0][4]=1;ITL[1][4]=0;ITL[2][4]=0;ITL[3][4]=0;ITL[4][4]=1;

    TL[5]=-0.114336;
    ITL[0][5]=0;ITL[1][5]=0;ITL[2][5]=0;ITL[3][5]=2;ITL[4][5]=0;

    TL[6]=+0.058793;
    ITL[0][6]=0;ITL[1][6]=-2;ITL[2][6]=2;ITL[3][6]=0;ITL[4][6]=0;

    TL[7]=+0.057212;
    ITL[0][7]=-1;ITL[1][7]=-1;ITL[2][7]=2;ITL[3][7]=0;ITL[4][7]=1;

    TL[8]=+0.053320;
    ITL[0][8]=0;ITL[1][8]=1;ITL[2][8]=2;ITL[3][8]=0;ITL[4][8]=0;

    TL[9]=+0.045874;
    ITL[0][9]=-1;ITL[1][9]=0;ITL[2][9]=2;ITL[3][9]=0;ITL[4][9]=1;

    TL[10]=+0.041024;
    ITL[0][10]=-1;ITL[1][10]=1;ITL[2][10]=0;ITL[3][10]=0;ITL[4][10]=1;

    TL[11]=-0.034718;
    ITL[0][11]=0;ITL[1][11]=0;ITL[2][11]=1;ITL[3][11]=0;ITL[4][11]=0;

    TL[12]= -0.030465;
    ITL[0][12]=1;ITL[1][12]=1;ITL[2][12]=0;ITL[3][12]=0;ITL[4][12]=1;

    TL[13]=+0.015326;
    ITL[0][13]=0;ITL[1][13]=0;ITL[2][13]=2;ITL[3][13]=-2;ITL[4][13]=0;

    TL[14]= -0.012528;
    ITL[0][14]=0;ITL[1][14]=1;ITL[2][14]=0;ITL[3][14]=2;ITL[4][14]=0;

    TL[15]= -0.010980;
    ITL[0][15]=0;ITL[1][15]=-1;ITL[2][15]=0;ITL[3][15]=2;ITL[4][15]=0;

    TL[16]= +0.010674;
    ITL[0][16]=0;ITL[1][16]=-1;ITL[2][16]=4;ITL[3][16]=0;ITL[4][16]=0;

    TL[17]=+0.010034;
    ITL[0][17]=0;ITL[1][17]=3;ITL[2][17]=0;ITL[3][17]=0;ITL[4][17]=0;

    TL[18]=+0.008548;
    ITL[0][18]=0;ITL[1][18]=-2;ITL[2][18]=4;ITL[3][18]=0;ITL[4][18]=0;

    TL[19]=-0.007910;
    ITL[0][19]=1;ITL[1][19]=-1;ITL[2][19]=2;ITL[3][19]=0;ITL[4][19]=1;

    TL[20]= -0.006783;
    ITL[0][20]=1;ITL[1][20]=0;ITL[2][20]=2;ITL[3][20]=0;ITL[4][20]=1;

    TL[21]= +0.005162;
    ITL[0][21]=0;ITL[1][21]=1;ITL[2][21]=-1;ITL[3][21]=0;ITL[4][21]=0;

    TL[22]=  +0.005000;
    ITL[0][22]=1;ITL[1][22]=0;ITL[2][22]=1;ITL[3][22]=0;ITL[4][22]=1;

    TL[23]= +0.004049;
    ITL[0][23]=-1;ITL[1][23]=1;ITL[2][23]=2;ITL[3][23]=0;ITL[4][23]=1;

    TL[24]=+0.003996;
    ITL[0][24]=0;ITL[1][24]=2;ITL[2][24]=2;ITL[3][24]=0;ITL[4][24]=0;

    TL[25]=+0.003862;
    ITL[0][25]=0;ITL[1][25]=0;ITL[2][25]=4;ITL[3][25]=0;ITL[4][25]=0;

    TL[26]= +0.003665;
    ITL[0][26]=0;ITL[1][26]=-3;ITL[2][26]=2;ITL[3][26]=0;ITL[4][26]=0;

    TL[27]= +0.002695;
    ITL[0][27]=-1;ITL[1][27]=2;ITL[2][27]=0;ITL[3][27]=0;ITL[4][27]=1;

    TL[28]= +0.002602;
    ITL[0][28]=0;ITL[1][28]=1;ITL[2][28]=-2;ITL[3][28]=-2;ITL[4][28]=0;

    TL[29]= +0.002396;
    ITL[0][29]=-1;ITL[1][29]=-2;ITL[2][29]=2;ITL[3][29]=0;ITL[4][29]=1;

    TL[30]= -0.002349;
    ITL[0][30]=0;ITL[1][30]=1;ITL[2][30]=1;ITL[3][30]=0;ITL[4][30]=0;

    TL[31]=+0.002249;
    ITL[0][31]=-2;ITL[1][31]=0;ITL[2][31]=2;ITL[3][31]=0;ITL[4][31]=2;

    TL[32]=-0.002125;
    ITL[0][32]=1;ITL[1][32]=2;ITL[2][32]=0;ITL[3][32]=0;ITL[4][32]=1;

    TL[33]=-0.002079;
    ITL[0][33]=2;ITL[1][33]=0;ITL[2][33]=0;ITL[3][33]=0;ITL[4][33]=2;

    TL[34]= +0.002059;
    ITL[0][34]=-2;ITL[1][34]=-1;ITL[2][34]=2;ITL[3][34]=0;ITL[4][34]=2;

    TL[35]=-0.001773;
    ITL[0][35]=0;ITL[1][35]=1;ITL[2][35]=2;ITL[3][35]=-2;ITL[4][35]=0;

    TL[36]=-0.001595;
    ITL[0][36]=0;ITL[1][36]=0;ITL[2][36]=2;ITL[3][36]=2;ITL[4][36]=0;

    TL[37]=+0.001220;
    ITL[0][37]=-1;ITL[1][37]=-1;ITL[2][37]=4;ITL[3][37]=0;ITL[4][37]=1;

    TL[38]=-0.001110;
    ITL[0][38]=0;ITL[1][38]=2;ITL[2][38]=0;ITL[3][38]=2;ITL[4][38]=0;

    TL[39]=+0.000892;
    ITL[0][39]=0;ITL[1][39]=1;ITL[2][39]=-3;ITL[3][39]=0;ITL[4][39]=0;

    TL[40]=-0.000811;
    ITL[0][40]=1;ITL[1][40]=1;ITL[2][40]=2;ITL[3][40]=0;ITL[4][40]=1;

    TL[41]=+0.000761;
    ITL[0][41]=-1;ITL[1][41]=-2;ITL[2][41]=4;ITL[3][41]=0;ITL[4][41]=1;

    TL[42]=+0.000717;
    ITL[0][42]=-2;ITL[1][42]=1;ITL[2][42]=0;ITL[3][42]=0;ITL[4][42]=2;

    TL[43]=+0.000704;
    ITL[0][43]=-2;ITL[1][43]=1;ITL[2][43]=-2;ITL[3][43]=0;ITL[4][43]=2;

    TL[44]= +0.000693;
    ITL[0][44]=1;ITL[1][44]=-2;ITL[2][44]=2;ITL[3][44]=0;ITL[4][44]=1;

    TL[45]=+0.000598;
    ITL[0][45]=-1;ITL[1][45]=0;ITL[2][45]=2;ITL[3][45]=-2;ITL[4][45]=1;

    TL[46]=+0.000550;
    ITL[0][46]=0;ITL[1][46]=1;ITL[2][46]=4;ITL[3][46]=0;ITL[4][46]=0;

    TL[47]=+0.000538;
    ITL[0][47]=0;ITL[1][47]=4;ITL[2][47]=0;ITL[3][47]=0;ITL[4][47]=0;

    TL[48]=+0.000521;
    ITL[0][48]=-1;ITL[1][48]=0;ITL[2][48]=4;ITL[3][48]=0;ITL[4][48]=1;

    TL[49]=+0.000486;
    ITL[0][49]=0;ITL[1][49]=2;ITL[2][49]=-1;ITL[3][49]=0;ITL[4][49]=0;


    TB[0]= +5.128189;
    ITB[0][0]=0;ITB[1][0]=0;ITB[2][0]=0;ITB[3][0]=1;ITB[4][0]=0;

    TB[1]=+0.280606;
    ITB[0][1]=0;ITB[1][1]=1;ITB[2][1]=0;ITB[3][1]=1;ITB[4][1]=0;

    TB[2]=+0.277693;
    ITB[0][2]=0;ITB[1][2]=1;ITB[2][2]=0;ITB[3][2]=-1;ITB[4][2]=0;

    TB[3]=+0.173238;
    ITB[0][3]=0;ITB[1][3]=0;ITB[2][3]=2;ITB[3][3]=-1;ITB[4][3]=0;

    TB[4]= +0.055413;
    ITB[0][4]=0;ITB[1][4]=-1;ITB[2][4]=2;ITB[3][4]=1;ITB[4][4]=0;

    TB[5]=+0.046272;
    ITB[0][5]=0;ITB[1][5]=-1;ITB[2][5]=2;ITB[3][5]=-1;ITB[4][5]=0;

    TB[6]=+0.032573;
    ITB[0][6]=0;ITB[1][6]=0;ITB[2][6]=2;ITB[3][6]=1;ITB[4][6]=0;

    TB[7]= +0.017198;
    ITB[0][7]=0;ITB[1][7]=2;ITB[2][7]=0;ITB[3][7]=1;ITB[4][7]=0;

    TB[8]= +0.009267;
    ITB[0][8]=0;ITB[1][8]=1;ITB[2][8]=2;ITB[3][8]=-1;ITB[4][8]=0;

    TB[9]=+0.008823;
    ITB[0][9]=0;ITB[1][9]=2;ITB[2][9]=0;ITB[3][9]=-1;ITB[4][9]=0;

    TB[10]= +0.008247;
    ITB[0][10]=-1;ITB[1][10]=0;ITB[2][10]=2;ITB[3][10]=-1;ITB[4][10]=1;

    TB[11]=+0.004323;
    ITB[0][11]=0;ITB[1][11]=-2;ITB[2][11]=2;ITB[3][11]=-1;ITB[4][11]=0;

    TB[12]=+0.004200;
    ITB[0][12]=0;ITB[1][12]=1;ITB[2][12]=2;ITB[3][12]=1;ITB[4][12]=0;

    TB[13]=+0.003372;
    ITB[0][13]=-1;ITB[1][13]=0;ITB[2][13]=-2;ITB[3][13]=1;ITB[4][13]=1;

    TB[14]=+0.002472;
    ITB[0][14]=-1;ITB[1][14]=-1;ITB[2][14]=2;ITB[3][14]=1;ITB[4][14]=1;

    TB[15]=+0.002222;
    ITB[0][15]=-1;ITB[1][15]=0;ITB[2][15]=2;ITB[3][15]=1;ITB[4][15]=1;

    TB[16]=+0.002072;
    ITB[0][16]=-1;ITB[1][16]=-1;ITB[2][16]=2;ITB[3][16]=-1;ITB[4][16]=1;

    TB[17]= +0.001877;
    ITB[0][17]=-1;ITB[1][17]=1;ITB[2][17]=0;ITB[3][17]=1;ITB[4][17]=1;

    TB[18]=+0.001828;
    ITB[0][18]=0;ITB[1][18]=-1;ITB[2][18]=4;ITB[3][18]=-1;ITB[4][18]=0;

    TB[19]=-0.001803;
    ITB[0][19]=1;ITB[1][19]=0;ITB[2][19]=0;ITB[3][19]=1;ITB[4][19]=1;

    TB[20]=-0.001750;
    ITB[0][20]=0;ITB[1][20]=0;ITB[2][20]=0;ITB[3][20]=3;ITB[4][20]=0;

    TB[21]= +0.001570;
    ITB[0][21]=-1;ITB[1][21]=1;ITB[2][21]=0;ITB[3][21]=-1;ITB[4][21]=1;

    TB[22]=-0.001487;
    ITB[0][22]=0;ITB[1][22]=0;ITB[2][22]=1;ITB[3][22]=1;ITB[4][22]=0;

    TB[23]= -0.001481;
    ITB[0][23]=1;ITB[1][23]=1;ITB[2][23]=0;ITB[3][23]=1;ITB[4][23]=1;

    TB[24]= +0.001417;
    ITB[0][24]=-1;ITB[1][24]=-1;ITB[2][24]=0;ITB[3][24]=1;ITB[4][24]=1;

    TB[25]=+0.001350;
    ITB[0][25]=-1;ITB[1][25]=0;ITB[2][25]=0;ITB[3][25]=1;ITB[4][25]=1;

    TB[26]=+0.001330;
    ITB[0][26]=0;ITB[1][26]=0;ITB[2][26]=-1;ITB[3][26]=1;ITB[4][26]=0;

    TB[27]=+0.001106;
    ITB[0][27]=0;ITB[1][27]=3;ITB[2][27]=0;ITB[3][27]=1;ITB[4][27]=0;

    TB[28]=+0.001020;
    ITB[0][28]=0;ITB[1][28]=0;ITB[2][28]=4;ITB[3][28]=-1;ITB[4][28]=0;

    TB[29]=+0.000833;
    ITB[0][29]=0;ITB[1][29]=-1;ITB[2][29]=4;ITB[3][29]=1;ITB[4][29]=0;

    TB[30]=+0.000781;
    ITB[0][30]=0;ITB[1][30]=1;ITB[2][30]=0;ITB[3][30]=-3;ITB[4][30]=0;

    TB[31]=+0.000670;
    ITB[0][31]=0;ITB[1][31]=-2;ITB[2][31]=4;ITB[3][31]=1;ITB[4][31]=0;

    TB[32]=+0.000606;
    ITB[0][32]=0;ITB[1][32]=0;ITB[2][32]=2;ITB[3][32]=-3;ITB[4][32]=0;

    TB[33]=+0.000597;
    ITB[0][33]=0;ITB[1][33]=2;ITB[2][33]=2;ITB[3][33]=-1;ITB[4][33]=0;

    TB[34]=+0.000492;
    ITB[0][34]=-1;ITB[1][34]=1;ITB[2][34]=2;ITB[3][34]=-1;ITB[4][34]=1;

    TB[35]=+0.000450;
    ITB[0][35]=0;ITB[1][35]=2;ITB[2][35]=-2;ITB[3][35]=-1;ITB[4][35]=0;

    TB[36]=+0.000439;
    ITB[0][36]=0;ITB[1][36]=3;ITB[2][36]=0;ITB[3][36]=-1;ITB[4][36]=0;

    TB[37]=+0.000423;
    ITB[0][37]=0;ITB[1][37]=2;ITB[2][37]=2;ITB[3][37]=1;ITB[4][37]=0;

    TB[38]=+0.000422;
    ITB[0][38]=0;ITB[1][38]=-3;ITB[2][38]=2;ITB[3][38]=-1;ITB[4][38]=0;

    TB[39]= -0.000367;
    ITB[0][39]=1;ITB[1][39]=-1;ITB[2][39]=2;ITB[3][39]=1;ITB[4][39]=1;

    TB[40]=-0.000353;
    ITB[0][40]=1;ITB[1][40]=0;ITB[2][40]=2;ITB[3][40]=1;ITB[4][40]=1;

    TB[41]=+0.000331;
    ITB[0][41]=0;ITB[1][41]=0;ITB[2][41]=4;ITB[3][41]=1;ITB[4][41]=0;

    TB[42]=+0.000317;
    ITB[0][42]=-1;ITB[1][42]=1;ITB[2][42]=2;ITB[3][42]=1;ITB[4][42]=1;

    TB[43]=+0.000306;
    ITB[0][43]=-2;ITB[1][43]=0;ITB[2][43]=2;ITB[3][43]=-1;ITB[4][43]=2;

    TB[44]=-0.000283;
    ITB[0][44]=0;ITB[1][44]=1;ITB[2][44]=0;ITB[3][44]=3;ITB[4][44]=0;


    TP[0]=+0.950724;
    ITP[0][0]=0;ITP[1][0]=0;ITP[2][0]=0;ITP[3][0]=0;ITP[4][0]=0;

    TP[1]=+0.051818;
    ITP[0][1]=0;ITP[1][1]=1;ITP[2][1]=0;ITP[3][1]=0;ITP[4][1]=0;

    TP[2]=+0.009531;
    ITP[0][2]=0;ITP[1][2]=-1;ITP[2][2]=2;ITP[3][2]=0;ITP[4][2]=0;

    TP[3]=+0.007843;
    ITP[0][3]=0;ITP[1][3]=0;ITP[2][3]=2;ITP[3][3]=0;ITP[4][3]=0;

    TP[4]=+0.002824;
    ITP[0][4]=0;ITP[1][4]=2;ITP[2][4]=0;ITP[3][4]=0;ITP[4][4]=0;

    TP[5]=+0.000857;
    ITP[0][5]=0;ITP[1][5]=1;ITP[2][5]=2;ITP[3][5]=0;ITP[4][5]=0;

    TP[6]=+0.000533;
    ITP[0][6]=-1;ITP[1][6]=0;ITP[2][6]=2;ITP[3][6]=0;ITP[4][6]=1;

    TP[7]=+0.000401;
    ITP[0][7]=-1;ITP[1][7]=-1;ITP[2][7]=2;ITP[3][7]=0;ITP[4][7]=1;

    TP[8]=+0.000320;
    ITP[0][8]=-1;ITP[1][8]=1;ITP[2][8]=0;ITP[3][8]=0;ITP[4][8]=1;

    TP[9]=-0.000271;
    ITP[0][9]=0;ITP[1][9]=0;ITP[2][9]=1;ITP[3][9]=0;ITP[4][9]=0;

    TP[10]=-0.000264;
    ITP[0][10]=1;ITP[1][10]=1;ITP[2][10]=0;ITP[3][10]=0;ITP[4][10]=1;

    TP[11]=-0.000198;
    ITP[0][11]=0;ITP[1][11]=-1;ITP[2][11]=0;ITP[3][11]=2;ITP[4][11]=0;

    TP[12]=+0.000173;
    ITP[0][12]=0;ITP[1][12]=3;ITP[2][12]=0;ITP[3][12]=0;ITP[4][12]=0;

    TP[13]=+0.000167;
    ITP[0][13]=0;ITP[1][13]=-1;ITP[2][13]=4;ITP[3][13]=0;ITP[4][13]=0;

    TP[14]=-0.000111;
    ITP[0][14]=1;ITP[1][14]=0;ITP[2][14]=0;ITP[3][14]=0;ITP[4][14]=1;

    TP[15]=+0.000103;
    ITP[0][15]=0;ITP[1][15]=-2;ITP[2][15]=4;ITP[3][15]=0;ITP[4][15]=0;

    TP[16]= -0.000084;
    ITP[0][16]=0;ITP[1][16]=2;ITP[2][16]=-2;ITP[3][16]=0;ITP[4][16]=0;

    TP[17]=-0.000083;
    ITP[0][17]=1;ITP[1][17]=0;ITP[2][17]=2;ITP[3][17]=0;ITP[4][17]=1;

    TP[18]=+0.000079;
    ITP[0][18]=0;ITP[1][18]=2;ITP[2][18]=2;ITP[3][18]=0;ITP[4][18]=0;

    TP[19]=+0.000072;
    ITP[0][19]=0;ITP[1][19]=0;ITP[2][19]=4;ITP[3][19]=0;ITP[4][19]=0;

    TP[20]=+0.000064;
    ITP[0][20]=-1;ITP[1][20]=1;ITP[2][20]=2;ITP[3][20]=0;ITP[4][20]=1;

    TP[21]=-0.000063;
    ITP[0][21]=1;ITP[1][21]=-1;ITP[2][21]=2;ITP[3][21]=0;ITP[4][21]=1;

    TP[22]=+0.000041;
    ITP[0][22]=1;ITP[1][22]=0;ITP[2][22]=1;ITP[3][22]=0;ITP[4][22]=1;

    TP[23]=+0.000035;
    ITP[0][23]=-1;ITP[1][23]=2;ITP[2][23]=0;ITP[3][23]=0;ITP[4][23]=1;

    TP[24]=-0.000033;
    ITP[0][24]=0;ITP[1][24]=3;ITP[2][24]=-2;ITP[3][24]=0;ITP[4][24]=0;

    TP[25]=-0.000030;
    ITP[0][25]=0;ITP[1][25]=1;ITP[2][25]=1;ITP[3][25]=0;ITP[4][25]=0;

    TP[26]=-0.000029;
    ITP[0][26]=0;ITP[1][26]=0;ITP[2][26]=-2;ITP[3][26]=2;ITP[4][26]=0;

    TP[27]=-0.000029;
    ITP[0][27]=1;ITP[1][27]=2;ITP[2][27]=0;ITP[3][27]=0;ITP[4][27]=1;

    TP[28]=+0.000026;
    ITP[0][28]=-2;ITP[1][28]=0;ITP[2][28]=2;ITP[3][28]=0;ITP[4][28]=2;

    TP[29]=-0.000023;
    ITP[0][29]=0;ITP[1][29]=1;ITP[2][29]=-2;ITP[3][29]=2;ITP[4][29]=0;

    TP[30]=+0.000019;
    ITP[0][30]=-1;ITP[1][30]=-1;ITP[2][30]=4;ITP[3][30]=0;ITP[4][30]=1;


    double tt   = (tdtTime.MJD - 15019.5) / 36525.0;

    double ELP  =  D2R*fmod((ELP0 + (ELP1 + (ELP2 + ELP3*tt)*tt)*tt), 360.0);
    double DELP =  D2R*(ELP1 +  (2.0*ELP2 + 3.0*ELP3*tt)*tt);

    double EM   =  D2R*fmod((EM0  + (EM1  + (EM2  + EM3 *tt)*tt)*tt), 360.0);
    double DEM  =  D2R*(EM1  +  (2.0*EM2  + 3.0*EM3 *tt)*tt);

    double EMP  =  D2R*fmod((EMP0 + (EMP1 + (EMP2 + EMP3*tt)*tt)*tt), 360.0);
    double DEMP =  D2R*(EMP1 +  (2.0*EMP2 + 3.0*EMP3*tt)*tt);

    double D    =  D2R*fmod((D0   + (D1   + (D2   + D3  *tt)*tt)*tt), 360.0);
    double DD   =  D2R*(D1   +  (2.0*D2   + 3.0*D3  *tt)*tt);

    double F    =  D2R*fmod((F0   + (F1   + (F2   + F3  *tt)*tt)*tt), 360.0);
    double DF   =  D2R*(F1   +  (2.0*F2   + 3.0*F3  *tt)*tt);

    double OM   =  D2R*fmod((OM0  + (OM1  + (OM2  + OM3 *tt)*tt)*tt), 360.0);
    double DOM  =  D2R*(OM1  +  (2.0*OM2  + 3.0*OM3 *tt)*tt);


    double SINOM  = sin(OM);
    double COSOM  = cos(OM);
    double DOMCOM = DOM*COSOM;
    double THETA  = D2R*(PA0 +   PA1*tt);
    double WA     = sin(THETA);
    double DWA    = D2R* PA1*cos(THETA);
           THETA  = D2R*(PE0 +  (PE1 + PE2*tt)*tt);
    double WB     = PEC*sin(THETA);
    double DWB    = D2R*PEC*(PE1 + 2.0*PE2*tt)*cos(THETA);

    ELP    = ELP  + D2R*(PAC*WA  + WB  + PFC*SINOM );
    DELP   = DELP + D2R*(PAC*DWA + DWB + PFC*DOMCOM);
    EM     = EM   + D2R* PBC*WA;
    DEM    = DEM  + D2R* PBC*DWA;
    EMP    = EMP  + D2R*(PCC*WA  + WB  + PGC*SINOM);
    DEMP   = DEMP + D2R*(PCC*DWA + DWB + PGC*DOMCOM);
    D      = D    + D2R*(PDC*WA  + WB  + PHC*SINOM);
    DD     = DD   + D2R*(PDC*DWA + DWB + PHC*DOMCOM);


    double WOM    = OM +D2R*(PJ0 + PJ1*tt);
    double DWOM   = DOM+D2R*PJ1;
    double SINWOM = sin(WOM);
    double COSWOM = cos(WOM);
    F     = F     + D2R*(WB  + cPIC*SINOM  + PJC*SINWOM);
    DF    = DF    + D2R*(DWB + cPIC*DOMCOM + PJC*DWOM*COSWOM);

    double E      = 1.0 + (E1 + E2*tt)*tt;
    double ESQ    = E*E;
    double V      = 0.0;
    double COEFF;
    double EN;
    int    EMN, EMPN, DN, FN, I;
    for (int n = 49; n >= 0; n--)
    {
        COEFF  = TL    [n];
        EMN    = ITL[0][n];
        EMPN   = ITL[1][n];
        DN     = ITL[2][n];
        FN     = ITL[3][n];
        I      = ITL[4][n];
        if  (0 == I)
            EN = 1.0;
        else if (I == 1)
            EN = E;
        else
            EN = ESQ;
        THETA  = EMN*EM + EMPN*EMP + DN*D + FN*F;
            V += COEFF*sin(THETA)*EN;
    }
    double EL  = ELP + D2R*V;

    V = 0.0;
    for (int n = 44; n >= 0; n--)
    {
        COEFF  = TB    [n];
        EMN    = ITB[0][n];
        EMPN   = ITB[1][n];
        DN     = ITB[2][n];
        FN     = ITB[3][n];
        I      = ITB[4][n];

        if  (0 == I)
            EN = 1.0;
        else if (I == 1)
            EN = E;
        else
            EN = ESQ;

        THETA  = EMN*EM + EMPN*EMP + DN*D + FN*F;
            V += COEFF*sin(THETA)*EN;
    }
    double BF  = 1.0 - CW1*COSOM - CW2*COSWOM;
    double B   = D2R*V*BF;

    V = 0.0;
    for (int n = 30; n >= 0; n--)
    {
        COEFF  = TP    [n];
        EMN    = ITP[0][n];
        EMPN   = ITP[1][n];
        DN     = ITP[2][n];
        FN     = ITP[3][n];
        I      = ITP[4][n];

        if  (0 == I)
            EN = 1.0;
        else if (I == 1)
            EN = E;
        else
            EN = ESQ;
        THETA  = EMN*EM + EMPN*EMP + DN*D + FN*F;
            V += COEFF*cos(THETA)*EN;
    }
    double P   = D2R*V;

    double SP  = sin(P);
    double R   = ERADAU/SP;

    /*------------------Longitude, latitude to x,y,z (AU)-----------------------*/
    double SEL = sin(EL);
    double CEL = cos(EL);
    double SB  = sin(B);
    double CB  = cos(B);
    double RCB = R*CB;
    double X   = RCB*CEL;
    double Y   = RCB*SEL;
    double Z   = R*SB;
           tt  =(tdtTime.MJD - 51544.5) / 36525.0;

    double EPJ    = 2000.0 + tt*100.0;
    double EQCOR  = DS2R* (0.035 + 0.00085*(EPJ-B1950));

    double EPS    = DAS2R*(84381.448 + (-46.8150 + (-0.00059+0.001813*tt)*tt)*tt);

    double SINEPS = sin(EPS);
    double COSEPS = cos(EPS);
    double ES     = EQCOR*SINEPS;
    double EC     = EQCOR*COSEPS;
    Vector3d monposCIS;
    monposCIS[0]  =(X - EC*Y + ES*Z)*AU_CONST;
    monposCIS[1]  =(EQCOR*X  + Y*COSEPS - Z*SINEPS)*AU_CONST;
    monposCIS[2]  =(Y*SINEPS + Z*COSEPS)*AU_CONST;
    return monposCIS;
}
/*-------------------------------------------------------------------------------
 * Name     : seekAntennaData
 * Function : Get needed satellite antenna data
 * Input    : const AntennaInfoFile &antennaDataBase,
 *            Sat_TypePrn satTN,
 *            double      matchTime,
 * Output   : AntennaData
 *------------------------------------------------------------------------------*/
AntennaData DataMatchingCenter::seekSatAntData(const AntennaInfoFile &antennaDataBase,
                                               const Sat_TypePrn     &satTN,
                                               const double          &matchTime)
{
    int     idex = satTN.PRN - 1;
    vector <AntennaData> *tempV_ptr = NULL;
    vector <AntennaData>::iterator tempData = antennaDataBase.antennaData_GPS[idex]->end();

    if      (satTN.Type == "G" && antennaDataBase.antennaData_GPS[idex] != NULL)
        tempV_ptr = antennaDataBase.antennaData_GPS[idex];
    else if (satTN.Type == "C" && antennaDataBase.antennaData_BDS[idex] != NULL)
        tempV_ptr = antennaDataBase.antennaData_BDS[idex];
    else if (satTN.Type == "R" && antennaDataBase.antennaData_GLONASS[idex] != NULL)
        tempV_ptr = antennaDataBase.antennaData_GLONASS[idex];
    else if (satTN.Type == "E" && antennaDataBase.antennaData_Galileo[idex] != NULL)
        tempV_ptr = antennaDataBase.antennaData_Galileo[idex];
    else
        throw illegalParameterValue("Antenna data of satellite can't match!");

    tempData = find_if(tempV_ptr->begin(), tempV_ptr->end(),
                       isTimeMatch(matchTime));

    if (tempData == tempV_ptr->end())                                              // If didn't find the proper data
        throw illegalParameterValue("Antenna data of satellite can't match!");

    return *tempData;
}
