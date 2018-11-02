#include "FileCenter.h"
#include "MyFunctionCenter.h"
#include "PublicDataCenter.h"
#include "RinexDataCenter.h"
#include "MyFunctionCenter.h"
#include "DataProcessCenter.h"
#include "ResultDataCenter.h"



/*------------------------------------------------------------------------------
 * Name     : SingleSatelliteData
 * Function : Constructor of class SingleSatelliteData
 *-----------------------------------------------------------------------------*/
SingleSatelliteData::SingleSatelliteData()
{
    satClock      =  0,  extraTime     =  0;
    frquencyNum_R =  0,  Delta0        =  0, Delta1        =  0;
    Delta2        =  0,
    tideCorre     =  0,  anteHeight    =  0, sagnacEffect  =  0;
    flagOfValid   =  0,  phaseWinding  =  0, tropDryDelay  =  0;
    relativityEffect    =  0;
    anteCenterCorre     =  0;
    tropWetDelayFunc    =  0;

}
/*------------------ Define static member--------------------------------------*/
FinalDataFile ResultFile_PPP::finalDataFile;
/*-----------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 * Name     : getFilesPtr
 * Function : Initialize file pointer
 * Input    : PrecisionEphemerisFile     *eph_Fp,
 *            PrecisionClockFile         *clk_Fp,
 *            OceanTideFile              *oce_Fp,
 *            AntennaInfoFile            *ant_Fp,
 *            EarthRotationParameterFile *erp_Fp,
 *            ObservationFile            *obs_Fp,
 *            IgsStationCoordinateFile   *coord_Fp,
 *            vector<SunMoonPosition>    *smPos_p,
 *            int                        *K_p
 * Output   : bool (Check and pass)
 *-----------------------------------------------------------------------------*/
void ResultFile_PPP::getFilesPtr(PrecisionEphemerisFile     *eph_Fp,
                                 PrecisionClockFile         *clk_Fp,
                                 OceanTideFile              *oce_Fp,
                                 AntennaInfoFile            *ant_Fp,
                                 EarthRotationParameterFile *erp_Fp,
                                 ObservationFile            *obs_Fp,
                                 IgsStationCoordinateFile   *coord_Fp,
                                 vector<SunMoonPosition>    *smPos_p,
                                 int                        *K_p)
{
    obsDB_ptr   = obs_Fp,       antDB_ptr   = ant_Fp;
    clkDB_ptr   = clk_Fp,       oceDB_ptr   = oce_Fp;
    erpDB_ptr   = erp_Fp,       ephDB_ptr   = eph_Fp;
    smPosDB_ptr = smPos_p;      coordDB_ptr = coord_Fp;
    K_ptr       = K_p    ;
}



int ResultFile_PPP::epochSysSatNum(EpochObservationData &epochObsData)
{
    int satNum  = 0;
    if (ModeFlag::hasGPS)
        satNum += epochObsData.GPS_Num;
    if (ModeFlag::hasBDS)
        satNum += epochObsData.BDS_Num;
    if (ModeFlag::hasGLONASS)
        satNum += epochObsData.GLONASS_Num;
    if (ModeFlag::hasGalileo)
        satNum += epochObsData.Galileo_Num;

    return satNum;
}
/*------------------------------------------------------------------------------
 * Name     : dataSourceCheck
 * Function : Check if all data source are filled
 * Input    : NULL
 * Output   : bool (Check and pass)
 *-----------------------------------------------------------------------------*/
 bool ResultFile_PPP::dataSourceCheck()
 {
     if (obsDB_ptr == NULL || antDB_ptr == NULL || clkDB_ptr == NULL || K_ptr == NULL ||
         oceDB_ptr == NULL || erpDB_ptr == NULL || ephDB_ptr == NULL || smPosDB_ptr == NULL)
         return false;
     return true;
 }

/*------------------------------------------------------------------------------
 * Name     : outputFile
 * Function : Output the generate data file (*.ppp)
 * Input    : const QString &filePath
 * Output   : bool (if output success)
 *-----------------------------------------------------------------------------*/
bool ResultFile_PPP::outputFile()
{
    QString outFilePath = filePath+ obsDB_ptr->STATION_NAME + "my.PPP";
    QFile   outFile(outFilePath);
    if (!outFile.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(NULL,                   "warning",
                            "Output file can't be opend!",
                             QMessageBox::Yes, QMessageBox::Yes);
        return false;
    }
    QTextStream outText(&outFile);

    if (dataSourceCheck())
    {
        QElapsedTimer timer;  ////////////
        timer.start();////////////////////
        outputPPPFile(outText, finalDataFile);
        outFile.close();
        qDebug() << "The slow operation took" << timer.elapsed() << "milliseconds";//////////////////////
    }
    else
        throw illegalParameterValue("Lack of data files, data processing failed!");


    return true;
}

/*------------------------------------------------------------------------------
 * Name     : outputCommonData
 * Function : Output the common information into file
 * Input    : const ObservationFile            &obsFile,
 *            const AntennaInfoFile            &antennaDataBase,
 *            const PrecisionClockFile         &clockDataBase,
 *            vector <int> arrayK
 * Output   : QTextStream                      &outText,
 *-----------------------------------------------------------------------------*/
AntennaData ResultFile_PPP::outputCommonData(QTextStream  &outText)
{
    outText.setRealNumberNotation(QTextStream::FixedNotation);                  // The method of setting out the output floating point number
    outText.setFieldAlignment(QTextStream::AlignRight);                         // Set output alignment
    outText.setRealNumberPrecision(1);                                          // Set the accuracy of the output floating point number.
    outText << qSetFieldWidth(16) << QString("MARKER_NAME:")
            << qSetFieldWidth(4 ) << obsDB_ptr->MARKER_NAME
            << "               "  << "*Author:Yulong Ge,Shengli Wang;Email:geyulong.lv@qq.com, victory_wsl@163.com" << endl;
    outText << qSetFieldWidth(16) << QString("INTERVAL:")
            << qSetFieldWidth(3 ) << obsDB_ptr->INTERVAL
            << qSetFieldWidth(7 ) << "second" << endl;

    outText << "               "  << "*Where 0 represents the data integrity,1 represents data can not be used,2 represents data can not use the third frequency data,"    << endl;
    outText << qSetFieldWidth(16) << QString("ANT_TYPE:")
            << qSetFieldWidth(20) << obsDB_ptr->ANT_TYPE           << endl;
    outText << qSetFieldWidth(16) << QString("REC_TYPE:")
            << qSetFieldWidth(20) << obsDB_ptr->REC_TYPE           << endl;
    outText.setRealNumberPrecision(4);
    outText << qSetFieldWidth(16) << QString("APPROX_POSITION:")
            << qSetFieldWidth(15) << obsDB_ptr->APPROX_POSITION[0] << qSetFieldWidth(1) << ","
            << qSetFieldWidth(15) << obsDB_ptr->APPROX_POSITION[1] << qSetFieldWidth(1) << ","
            << qSetFieldWidth(15) << obsDB_ptr->APPROX_POSITION[2]
            << qSetFieldWidth(5 ) << "(m)"                         << endl;
    outText << qSetFieldWidth(16) << QString("ANTENNA_DELTA_H:")
            << qSetFieldWidth(15) << obsDB_ptr->ANTENNA_DELTA_H
            << qSetFieldWidth(5 ) << "(m)"                         << endl;
    outText << qSetFieldWidth(16) << QString("ANTENNA_DELTA_E:")
            << qSetFieldWidth(15) << obsDB_ptr->ANTENNA_DELTA_E
            << qSetFieldWidth(5 ) << "(m)"                         << endl;
    outText << qSetFieldWidth(16) << QString("ANTENNA_DELTA_N:")
            << qSetFieldWidth(15) << obsDB_ptr->ANTENNA_DELTA_N
            << qSetFieldWidth(5 ) << "(m)"                         << endl;

    posOfRecAnt      = DataMatchingCenter::seekRecAntDataPos(*antDB_ptr,
                                                    obsDB_ptr->ANT_TYPE);
    if (posOfRecAnt  == -1)
        throw illegalParameterValue("Can't match antenna data of receiver!");   // Error prevention
    AntennaData recAntData = antDB_ptr->antennaData_Other[posOfRecAnt];

    outText << qSetFieldWidth(16) << QString("ANTENNA_PCO1:");
    for (int i = 0; i < 3; i++){
        outText << qSetFieldWidth(8 ) << recAntData.F1_NEU[i] / 1000 <<
                   qSetFieldWidth(5 ) << "(m),";
    }
    outText << endl;
    outText << qSetFieldWidth(16) << QString("ANTENNA_PCO2:");
    for (int i = 0; i < 3; i++){
        outText << qSetFieldWidth(8 ) << recAntData.F2_NEU[i] / 1000 <<
                   qSetFieldWidth(5 ) << "(m),";
    }
    outText << endl;


//        outText << qSetFieldWidth(16) < <QString("GPS_TYPE:");
//        for(int rt=0;rt<6;rt++)
//        {
//            sw<<qSetFieldWidth(3)<<QString(observe[f].GPS_TYPES[rt])<<" ";
//        }
//        sw<<endl;

//        sw<<qSetFieldWidth(16)<<QString("GLONASS_TYPE:");
//        for(int rt=0;rt<6;rt++)
//        {
//            sw<<qSetFieldWidth(3)<<QString(observe[f].GLONASS_TYPES[rt])<<" ";
//        }
//        sw<<endl;

//        sw<<qSetFieldWidth(16)<<QString("BDS_TYPE:");
//        for(int rt=0;rt<6;rt++)
//        {
//            sw<<qSetFieldWidth(3)<<QString(observe[f].BDS_TYPES[rt])<<" ";
//        }
//        sw<<endl;

//        sw<<qSetFieldWidth(16)<<QString("GALILEO_TYPE:");

//        for(int rt=0;rt<6;rt++)
//        {
//            sw<<qSetFieldWidth(3)<<QString(observe[f].Galileo_TYPES[rt])<<" ";
//        }
//        sw<<endl;

//        if(systemtype.indexOf("NMF")>=0)
//        {
//            sw<<qSetFieldWidth(30)<<QString("Tropospheric Delay           : UNB3m Model")<<endl;
//            sw<<qSetFieldWidth(53)<<QString("Tropospheric Mapping Function: Neill Mapping Function")<<endl;
//        }
//        if(systemtype.indexOf("GMF")>=0)
//        {
//            sw<<qSetFieldWidth(30)<<QString("Tropospheric Delay           : GPT Model")<<endl;
//            sw<<qSetFieldWidth(55)<<QString("Tropospheric Mapping Function: Global Mapping Function")<<endl;
//        }
//        if(systemtype.indexOf("VMF")>=0)
//        {
//            sw<<qSetFieldWidth(30)<<QString("Tropospheric Delay           : GPT2 Model")<<endl;
//            sw<<qSetFieldWidth(55)<<QString("Tropospheric Mapping Function: Vienna Mapping Function")<<endl;
//        }

    outText.setRealNumberPrecision(1 );
    outText << qSetFieldWidth(16) << QString("Navigation type:")  <<
               qSetFieldWidth(3 ) << clkDB_ptr->navigationType    << endl;
    outText << qSetFieldWidth(16) << QString("     Clock type:")  <<
               qSetFieldWidth(3 ) << clkDB_ptr->navigationType    << /* unknown*/
               qSetFieldWidth(2 ) << clkDB_ptr->INTERVAL          <<
               qSetFieldWidth(1 ) << "s"                          << endl;
    outText.setRealNumberPrecision(0 );
    outText << qSetFieldWidth(10) << QString("GLONASS K:");

    for (int m = 0; m < 30; m++)
    {
        if (K_ptr == NULL)
            outText << qSetFieldWidth(8) << 0;
        else
            outText << qSetFieldWidth(8) << K_ptr[m];
    }
    outText << endl;
    {                                                                       // Define startTime and endTime as a local variable.
        EpochTime  startTime(obsDB_ptr->AllObservationData.front().myTime.EPT);
        outText << qSetFieldWidth(11) << QString("Start Time:");
        outText << qSetFieldWidth(5 ) << startTime.year
                << qSetFieldWidth(5 ) << startTime.month
                << qSetFieldWidth(5 ) << startTime.day
                << qSetFieldWidth(5 ) << startTime.hour
                << qSetFieldWidth(5 ) << startTime.minute
                << qSetFieldWidth(5 ) << startTime.second
                << endl;
        EpochTime  endTime(obsDB_ptr->AllObservationData.back().myTime.EPT);
        outText << qSetFieldWidth(11) << QString("End   Time:");
        outText << qSetFieldWidth(5 ) << endTime.year
                << qSetFieldWidth(5 ) << endTime.month
                << qSetFieldWidth(5 ) << endTime.day
                << qSetFieldWidth(5 ) << endTime.hour
                << qSetFieldWidth(5 ) << endTime.minute
                << qSetFieldWidth(5 ) << endTime.second
                << endl;
    }

        outText << qSetFieldWidth(327)<< QString("PRN,s: Satposition(X), Satposition(Y), Satposition(Z),   Sat Clock(m),   Elevation(°),     Azimuth(°),       P1(m),          P2(m),     L1(cycles),     L2(cycles),      Trop Delay,       Trop Map,  Relativity(m),      Sagnac(m), Tide Effect(m), Antenna Height, Sat Antenna(m),OffsetL1(cycles),OffsetL2(cycles),Windup(cycles),          P3(m),     L3(cycles)") << endl;
    return recAntData;
}
/*------------------------------------------------------------------------------
 * Name     : outputSingleEpochData
 * Function : Output the single epoch information into file
 * Input    : const EpochObservationData       &epochObsData,
 *            AntennaData                      &recAntData,
 *            const Vector3d                   &antENU,
 *            const Vector3d                   &recBLH,
 *            const Vector3d                   &recXYZ,
 *            const Vector3d                   &poleTide,
 *            const CoordSysParameter          &coordParameter
 * Output   : QTextStream                      &outText,
 *            EpochSatelliteData               &epochOutData,
 *-----------------------------------------------------------------------------*/
void ResultFile_PPP::outputSingleEpochData( QTextStream           &outText,
                                            EpochSatelliteData    &epochOutData,
                                            EpochObservationData  &epochObsData,
                                            AntennaData           &recAntData,
                                      const Vector3d              &antENU,
                                      const Vector3d              &recBLH,
                                      const Vector3d              &recXYZ,
                                      const Vector3d              &poleTide,
                                      const CoordSysParameter     &coordPar)
{
    epochOutData.myTime  = epochObsData.myTime;
    posOfPreEph = DataMatchingCenter::seekPreEphDataPos(                        // match precise orbit and clock
                 *ephDB_ptr, epochOutData.myTime.MJD, posOfPreEph);
    posOfPreClk = DataMatchingCenter::seekPreClockPos(
                 *clkDB_ptr, epochOutData.myTime.MJD, posOfPreClk);
    if (posOfErp == -1)
        throw illegalParameterValue("Precision Ephemeris File can't matched!");
    if (posOfErp == -1)
        throw illegalParameterValue("Precision Clock File can't matched!");

    int satNum = epochSysSatNum(epochObsData);
    outText << qSetFieldWidth(17) << QString("satellite Number:")
            << qSetFieldWidth(3 ) << satNum
            << qSetFieldWidth(23) << QString(",(yyyy-mm-dd-hh-mm-ss):");
    outText << qSetFieldWidth(4 ) << epochOutData.myTime.EPT.year   << qSetFieldWidth(1) << "-"
            << qSetFieldWidth(2 ) << epochOutData.myTime.EPT.month  << qSetFieldWidth(1) << "-"
            << qSetFieldWidth(2 ) << epochOutData.myTime.EPT.day    << qSetFieldWidth(1) << "-"
            << qSetFieldWidth(2 ) << epochOutData.myTime.EPT.hour   << qSetFieldWidth(1) << "-"
            << qSetFieldWidth(2 ) << epochOutData.myTime.EPT.minute << qSetFieldWidth(1) << "-"
            << qSetFieldWidth(7 ) << epochOutData.myTime.EPT.second;

    MyTime   UTC = MyFunctionCenter::GPS_to_UTC(epochOutData.myTime);
    MyTime   TDT = MyFunctionCenter::UTC_to_TDT(UTC);

    VectorXd NMF = ErrorModel::tropDelyCor.UNB3M(recBLH, TDT.DOY, PI/2);        // Calculate ztd at 90 degree   trop
    epochOutData.ztd = NMF[1];
    outText << qSetFieldWidth(5 ) << ",zhd:"
            << qSetFieldWidth(8 ) << epochOutData.ztd << endl;

    MatrixXd smPositon = SatInformationCenter::interploteSunMoon(*smPosDB_ptr,  // The interpolation of the sun and moon coordinates coordinates
                         epochObsData.myTime.GPT.sec, posOfPreEph); /*unknown*/
    Vector3d  sunPos,    moonPos;
    for ( int m = 0; m < 3; m++)
        sunPos[m]  = smPositon(m, 0), moonPos[m] = smPositon(m, 1);

 TDT.EPT.second = int(TDT.EPT.second);
    Vector3d  solidTide(0, 0, 0);                                               // Calculate stide and otide
    solidTide = ErrorModel::solidTideCor(recXYZ, sunPos, moonPos, coordPar, TDT);
    Vector3d  oceanTide(0, 0, 0);
    oceanTide = ErrorModel::oceanTideCor(
                       oceDB_ptr->allOceanData[posOfOcean], epochOutData.myTime);

    Vector3d tideCor = solidTide + oceanTide + poleTide;                        // Get all tide correction
    for (unsigned int num = 0; num < epochObsData.epochObserveData.size(); num++)
    {
         SingleSatelliteData       sinSatOutData;
         sinSatOutData.obsData   = epochObsData.epochObserveData[num];
         Sat_TypePrn  &satTN     = sinSatOutData.obsData.satTN;
         if ((satTN.Type == "G" &&  ModeFlag::hasGPS)     ||
             (satTN.Type == "C" &&  ModeFlag::hasBDS)     ||
             (satTN.Type == "R" &&  ModeFlag::hasGLONASS) ||
             (satTN.Type == "E" &&  ModeFlag::hasGalileo) )
         {
             AntennaData satAntData =  DataMatchingCenter::seekSatAntData
                          (*antDB_ptr, satTN, epochObsData.myTime.JD);
             SatInformationCenter::getSatelliteInformation(                    // Get satellite information
                           clkDB_ptr->satelliteClkData,
                           ephDB_ptr->allPrecionData,
                           sinSatOutData , coordPar,
                           satAntData    , recAntData, antENU,
                           recXYZ, recBLH, sunPos,tideCor,
                           epochObsData.myTime ,
                           TDT           , posOfPreClk,
                           posOfPreEph   , K_ptr);

             Vector3d rSat    = MyFunctionCenter::getUnitVector(sinSatOutData.satXYZ);
             Vector3d rSun    = MyFunctionCenter::getUnitVector(sunPos);
             double cosAngle  = rSun.dot(rSat);
             double threshold = cos(PI - (29 / 2.0)* ToRAD);
             bool   isDataValid  = true;
             isDataValid = outputSingleSatelliteData(outText, sinSatOutData,   // Output satellite information into text
                                     threshold, cosAngle);
             if (isDataValid)
             {
                 epochOutData.eSatData.push_back(sinSatOutData);
                 epochOutData.sateNum++;
             }
          }
     }
}

/*------------------------------------------------------------------------------
 * Name     : outputSingleSatelliteData
 * Function : Output the single satellite information into file
 * Input    : double threshold,
 *            double cosAngle,
 * Output   : QTextStream              &outText,
 *            SingleSatelliteData      &satData,
 *-----------------------------------------------------------------------------*/
bool ResultFile_PPP::outputSingleSatelliteData(QTextStream              &outText,
                                               SingleSatelliteData      &satData,
                                               const  double            &threshold,
                                               const  double            &cosAngle)
{
    outText << qSetFieldWidth(1) << satData.obsData.satTN.Type;
    outText << QString("%1").arg(satData.obsData.satTN.PRN, 2, 10, QLatin1Char('0'));

    if (satData.obsData.P1 != 0)                                                // If P1 exist, use P1
        satData.obsData.C1  = satData.obsData.P1;

    int  frequencyNum = 2;                                                      // Default is double frequency
    bool isDataValid  = true;

    if (satData.satClock         == 0.0  || cosAngle   <= threshold       ||
        satData.obsData.C1       == 0    || satData.obsData.P2 == 0       ||
        satData.obsData.L1       == 0    || satData.obsData.L2 == 0       ||
        satData.anteCenterCorre  == 0    || fabs(satData.obsData.C1 -
                                                 satData.obsData.P2)>  50 ||
        satData.satXYZ[0]        == 0.0  )                                      // Judge if data is valid, conditions are repeating
    {
        frequencyNum  = 1;                                                      // Not have enough frequency
        isDataValid   = false;
    }

    if (frequencyNum != 1 && satData.obsData.P3 != 0 && satData.obsData.L3 != 0)// Judge if it is 3 frequency
        frequencyNum  = 3;                                                      // Neither of the frequency 1 and 2 can be dispensed.
                                                                                // Only 1,2,3 or 1,2 is available
    outText << qSetFieldWidth(2 ) << frequencyNum                   << qSetFieldWidth(1) << ",";
    outText << qSetFieldWidth(15) << satData.satXYZ[0]              << qSetFieldWidth(1) << ",";
    outText << qSetFieldWidth(15) << satData.satXYZ[1]              << qSetFieldWidth(1) << ",";
    outText << qSetFieldWidth(15) << satData.satXYZ[2]              << qSetFieldWidth(1) << ",";
    outText << qSetFieldWidth(15) << satData.satClock*LIGHT_V       << qSetFieldWidth(1) << ",";
    outText << qSetFieldWidth(15) <<((PI/2 - satData.DAZ[2])*ToDEG) << qSetFieldWidth(1) << ",";
    outText << qSetFieldWidth(15) << satData.DAZ[1]*ToDEG           << qSetFieldWidth(1) << ",";
    outText << qSetFieldWidth(15) << satData.obsData.C1             << qSetFieldWidth(1) << ",";
    outText << qSetFieldWidth(15) << satData.obsData.P2             << qSetFieldWidth(1) << ",";
    outText << qSetFieldWidth(15) << satData.obsData.L1             << qSetFieldWidth(1) << ",";
    outText << qSetFieldWidth(15) << satData.obsData.L2             << qSetFieldWidth(1) << ",";

    outText << qSetFieldWidth(15) << satData.tropDryDelay           << qSetFieldWidth(1) << ",";
    outText << qSetFieldWidth(15) << satData.tropWetDelayFunc       << qSetFieldWidth(1) << ",";
    outText << qSetFieldWidth(15) << satData.relativityEffect       << qSetFieldWidth(1) << ",";
    outText << qSetFieldWidth(15) << satData.sagnacEffect           << qSetFieldWidth(1) << ",";
    outText << qSetFieldWidth(15) << satData.tideCorre              << qSetFieldWidth(1) << ",";
    outText << qSetFieldWidth(15) << satData.anteHeight             << qSetFieldWidth(1) << ",";
    outText << qSetFieldWidth(15) << satData.anteCenterCorre        << qSetFieldWidth(1) << ",";
    outText << qSetFieldWidth(15) << satData.anteCenterOffset[0]    << qSetFieldWidth(1) << ",";
    outText << qSetFieldWidth(15) << satData.anteCenterOffset[1]    << qSetFieldWidth(1) << ",";
    outText << qSetFieldWidth(15) << satData.phaseWinding           << qSetFieldWidth(1) << ",";

    outText << qSetFieldWidth(15) << satData.obsData.P3             << qSetFieldWidth(1) << ",";
    outText << qSetFieldWidth(15) << satData.obsData.L3             << endl;

    return isDataValid;
}

/*-------------------------------------------------------------------------------
 * Name     : outputPPPFile
 * Function : Output the single epoch information into file
 * Input    : NULL
 * Output   : QTextStream                      &outText,
 *            FinalDataFile                    &finalDataFile,
 *-----------------------------------------------------------------------------*/
bool ResultFile_PPP::outputPPPFile(QTextStream                &outText,
                                   FinalDataFile              &finalDataFile)
{
    int coordDate  =   obsDB_ptr->AllObservationData.front().myTime.GPT.week * 10 +
                   int(obsDB_ptr->AllObservationData.front().myTime.GPT.sec  * SEC_DAY);
        posOfCoord =   DataMatchingCenter::seekApproxCoordPos(*coordDB_ptr ,
                                         obsDB_ptr->MARKER_NAME, coordDate);
    if (posOfCoord == -1)                                                       // Get precise approx coordinate
        throw illegalParameterValue("Precision coordinate can't find !");
    else
    {
        obsDB_ptr->APPROX_POSITION[0] = coordDB_ptr->stationCoordinateData[posOfCoord].obsPos[0];
        obsDB_ptr->APPROX_POSITION[1] = coordDB_ptr->stationCoordinateData[posOfCoord].obsPos[1];
        obsDB_ptr->APPROX_POSITION[2] = coordDB_ptr->stationCoordinateData[posOfCoord].obsPos[2];
    }


    CoordSysParameter coordParameter;
    MyFunctionCenter::getCoordSysParameter(WGS_84, coordParameter);
    Vector3d recXYZ(obsDB_ptr->APPROX_POSITION[0], obsDB_ptr->APPROX_POSITION[1],
                    obsDB_ptr->APPROX_POSITION[2]);
    Vector3d recBLH = MyFunctionCenter::XYZToBLH(recXYZ, coordParameter);

    finalDataFile.MASTER_APPROX_POS[0] = obsDB_ptr->APPROX_POSITION[0];
    finalDataFile.MASTER_APPROX_POS[1] = obsDB_ptr->APPROX_POSITION[1];
    finalDataFile.MASTER_APPROX_POS[2] = obsDB_ptr->APPROX_POSITION[2];
    finalDataFile.MASTER_MARKER_NAME   = obsDB_ptr->MARKER_NAME;
    finalDataFile.INTERVAL             = obsDB_ptr->INTERVAL;
    finalDataFile.ANT_TYPE             = obsDB_ptr->ANT_TYPE;
    finalDataFile.STATION_NAME         = obsDB_ptr->STATION_NAME;
    finalDataFile.navigationType       = obsDB_ptr->navigationType;


    AntennaData recAntData             = outputCommonData(outText);             // Output common positioning information
    Vector3d    recAntENU(obsDB_ptr->ANTENNA_DELTA_E, obsDB_ptr->ANTENNA_DELTA_N,
                          obsDB_ptr->ANTENNA_DELTA_H);
    posOfOcean        = DataMatchingCenter::seekOceanDataPos(*oceDB_ptr,
                                                 obsDB_ptr->MARKER_NAME);
    posOfErp          = DataMatchingCenter::seekErpDataPos(*erpDB_ptr,
                           obsDB_ptr->AllObservationData.front().myTime.JD);
    if (posOfOcean == -1)
        throw illegalParameterValue("Can't match Ocean Tide data !");           // Error prevention
    if (posOfErp   == -1)
        throw illegalParameterValue("Can't match Earth Rotation Parameter data!");
    ErpData erpData   = erpDB_ptr->allErpData[posOfErp];                  // 时间赋值有点问题
    erpData.myTime.JD = obsDB_ptr->AllObservationData[1].myTime.JD;       // 为什么使用1
    Vector3d poleTide = ErrorModel::poleTideCor(erpData, recBLH);

    outText.setRealNumberPrecision(4);                                          // Set real number precision

    for (unsigned int i = 0; i < obsDB_ptr->AllObservationData.size(); i++)     // Circle to output all epoch observation information
    {

        EpochObservationData & epochObsData  = obsDB_ptr->AllObservationData[i];// Representing the current epoch data
        EpochSatelliteData     epochOutData;
        outputSingleEpochData (outText,   epochOutData, epochObsData, recAntData,
                               recAntENU, recBLH,       recXYZ,       poleTide,
                               coordParameter);
        finalDataFile.allSatelliteData.push_back(epochOutData);
        qDebug() << "The " << i << " epoch";
    }
    return true;
}

/*------------------------------------------------------------------------------
 * Name     : readFile
 * Function : read PPP information file (*.ppp)
 * Input    : const QString &filePath
 * Output   : bool (if read success)
 *-----------------------------------------------------------------------------*/
bool ResultFile_PPP::readFile  ()
{
    if (!fileCommonDeal("PPP File Open faild!"))
        return false;
    QTextStream inText(&inFile);
    inputPPPFile(inText, ResultFile_PPP::finalDataFile, K_ptr);

//    closeFile();
    inFile.close();
    return true;
}

/*-------------------------------------------------------------------------------
 * Name     : inputPPPFile
 * Function : Extraction PPP information from *.ppp file
 * Input    : const QTextStream      &inText,
 * Output   : FinalDataFile          &finalDataFile,
 *            QVector<int>           &arrayK)
 *-----------------------------------------------------------------------------*/
void ResultFile_PPP::inputPPPFile(QTextStream            &inText,
                                  FinalDataFile          &finalDataFile,
                                  int*                    K_ptr)
{

    QString lineQStr = "";
    /*------------------------- Read Header -----------------------------------*/
    while ((lineQStr = inText.readLine()) != "")
    {
        if (lineQStr.indexOf("MARKER_NAME:")  >= 0)
            finalDataFile.MASTER_MARKER_NAME   = lineQStr.mid(16,  4);
        else if (lineQStr.indexOf("APPROX_POSITION") >= 0)
        {
            finalDataFile.MASTER_APPROX_POS[0] = lineQStr.mid(16, 15).toDouble();
            finalDataFile.MASTER_APPROX_POS[1] = lineQStr.mid(32, 15).toDouble();
            finalDataFile.MASTER_APPROX_POS[2] = lineQStr.mid(48, 15).toDouble();
        }
        else if (lineQStr.indexOf("GLONASS K:") >= 0)
        {
            for (int j = 0; j < 30; j++)
                 K_ptr[j] = (lineQStr.mid(10+j*8,8).toInt());
        }
//        else if (lineQStr.indexOf("GPS_TYPE:")  >= 0)
//        {
//            for(int i=0;i<6;i++)
//            {
//                gongshifile2[0].GPS_TYPES[i]=line.mid(16+6*i,3);
//            }
//        }
//        else if(line.indexOf("GLONASS_TYPE:")>=0)
//        {
//            for(int i=0;i<6;i++)
//            {
//                gongshifile2[0].GLONASS_TYPES[i]=line.mid(16+6*i,3);
//            }
//        }
//        else if(line.indexOf("BDS_TYPE:")>=0)
//        {
//            for(int i=0;i<6;i++)
//            {
//                gongshifile2[0].BDS_TYPES[i]=line.mid(16+6*i,3);
//            }
//        }
//        else if(line.indexOf("GALILEO_TYPE:")>=0)
//        {
//            for(int i=0;i<6;i++)
//            {
//                gongshifile2[0].Galileo_TYPES[i]=line.mid(16+6*i,3);
//            }
//        }
        else if (lineQStr.indexOf("PRN") >= 0)
            break;
    }
    /*------------------------- Read Data -------------------------------------*/
    while ((lineQStr = inText.readLine()) != "")
    {
        EpochSatelliteData     epochData;

        epochData.myTime.EPT.year   = lineQStr.mid(43, 4).toDouble();
        epochData.myTime.EPT.month  = lineQStr.mid(48, 2).toDouble();
        epochData.myTime.EPT.day    = lineQStr.mid(51, 2).toDouble();
        epochData.myTime.EPT.hour   = lineQStr.mid(54, 2).toDouble();
        epochData.myTime.EPT.minute = lineQStr.mid(57, 2).toDouble();
        epochData.myTime.EPT.second = lineQStr.mid(60, 7).toDouble();
        epochData.ztd               = lineQStr.mid(72, 8).toDouble();

        epochData.myTime =  MyFunctionCenter::timeIntegrator(epochData.myTime.EPT);
        int allSatNum    =  lineQStr.mid(17, 3).toInt();
        int validSatNum  =  0;
        for (int n = 0; n < allSatNum; n++)
        {
            lineQStr = inText.readLine();
            SingleSatelliteData  satData;
            int  frequencyNum =  lineQStr.mid(4,1).toInt();                     // Only the comination of f is 1,2,3 or 1,2 is available
            if  (frequencyNum == 1)
                 continue;
            if ((ModeFlag::P1_P3 || ModeFlag::P2_P3 || ModeFlag ::frequency3) && /*unknown*/
                 frequencyNum == 3)
                 continue;


            validSatNum++;                                                      // Counting the number of valid satellite
            satData.obsData.satTN.Type = lineQStr.mid(0,   1);
            satData.obsData.satTN.PRN  = lineQStr.mid(1,   2).toDouble();
            satData.satXYZ[0]          = lineQStr.mid(6,  15).toDouble();
            satData.satXYZ[1]          = lineQStr.mid(22, 15).toDouble();
            satData.satXYZ[2]          = lineQStr.mid(38, 15).toDouble();
            satData.satClock           = lineQStr.mid(54, 15).toDouble();
            satData.DAZ[2]      = PI/2 - lineQStr.mid(70, 15).toDouble()*ToRAD;
            satData.DAZ[1]             = lineQStr.mid(86, 15).toDouble()*ToRAD;
            satData.obsData.C1         = lineQStr.mid(102,15).toDouble();
            satData.obsData.P2         = lineQStr.mid(118,15).toDouble();
            satData.obsData.L1         = lineQStr.mid(134,15).toDouble();
            satData.obsData.L2         = lineQStr.mid(150,15).toDouble();
            satData.tropDryDelay       = lineQStr.mid(166,15).toDouble();
            satData.tropWetDelayFunc   = lineQStr.mid(182,15).toDouble();
            satData.relativityEffect   = lineQStr.mid(198,15).toDouble();
            satData.sagnacEffect       = lineQStr.mid(214,15).toDouble();
            satData.tideCorre          = lineQStr.mid(230,15).toDouble();
            satData.anteHeight         = lineQStr.mid(246,15).toDouble();
            satData.anteCenterCorre    = lineQStr.mid(262,15).toDouble();
            satData.anteCenterOffset[0]= lineQStr.mid(278,15).toDouble();
            satData.anteCenterOffset[1]= lineQStr.mid(294,15).toDouble();
            satData.phaseWinding       = lineQStr.mid(310,15).toDouble();
            satData.obsData.P3         = lineQStr.mid(326,15).toDouble();
            satData.obsData.L3         = lineQStr.mid(342,15).toDouble();

            satData.Delta0 = satData.satClock         - satData.tropDryDelay +  // Calulate all error delay
                             satData.relativityEffect + satData.sagnacEffect +
                             satData.tideCorre        + satData.anteHeight   +
                             satData.anteCenterCorre;
            satData.Delta1 = satData.anteCenterOffset[0] - satData.phaseWinding;
            satData.Delta2 = satData.anteCenterOffset[1] - satData.phaseWinding;
            MyFunctionCenter::getFrequency(satData.obsData.satTN, K_ptr,
                                           satData.freq);
            epochData.eSatData.push_back(satData);
        }
        epochData.sateNum = validSatNum;
        finalDataFile.allSatelliteData.push_back(epochData);
    }

    int i =0;////////////////////////
}
