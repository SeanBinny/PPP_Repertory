#ifndef RESULTDATACENTER_H
#define RESULTDATACENTER_H
#include "FileCenter.h"
#include "ProductDataCenter.h"
#include "RinexDataCenter.h"
/*--------------------------------------------------------------
 * Function : Save single epoch satellite essential data
 *-------------------------------------------------------------*/
struct SingleSatelliteData
{
    ObservationData obsData;
    int      frquencyNum_R;                                      // Frequency number of GLONASS Satellite
    double   satClock;                                           // Clock errors
    double   extraTime;                                          // Extrapolation time
    double   tideCorre;                                          // Tidal correction
    double   anteHeight;                                         // Antenna height correction
    double   sagnacEffect;                                       // Sagnac effection
    double   relativityEffect;                                   // Relativity effection
    double   anteCenterCorre;                                    // Satellite antenna phase center deviation correction
    double   phaseWinding;                                       // Phase winding correction
    double   tropDryDelay;                                       // Tropospheric delay (dry weight)
    double   tropWetDelayFunc;                                   // Tropospheric wet delay projection function
    double   flagOfValid;                                        // To determine whether the Satellite can be used

    double   Delta0;                                             // This is calculated to PPP*/
    double   Delta1;
    double   Delta2;
    vector  <double> freq;                                       // Save frequence of observe value
    double   anteCenterOffset[2];                                // Receiver antenna phase center offset correction

    double   satVel[3];                                          // The velocity of the satellite
    double   satXYZ[3];                                          // Satellite coordinates
    double   DAZ[3];                                             // Storage distance, azimuth, zenith angle

    SingleSatelliteData();
};

/*--------------------------------------------------------------
 * Function : Save essential data in an epoach
 *-------------------------------------------------------------*/
struct EpochSatelliteData
{
    MyTime  myTime;                                              // Storage Standard Time
    int     sateNum;                                             // The number of satellites
    double  ztd;                                                 // Zenith tropospheric delay
    vector <SingleSatelliteData> eSatData;                       // Store all essential data of a satellite
    EpochSatelliteData()
        :sateNum(0), ztd(0) {}
};

/*--------------------------------------------------------------
 * Function : Save all information of this positioning process
 *-------------------------------------------------------------*/
struct FinalDataFile
{
    vector  <EpochSatelliteData> allSatelliteData;

    QString  navigationType;
    QString  STATION_NAME;
    QString  MASTER_MARKER_NAME;
    QString  ROVER_MARKER_NAME;
    QString  ANT_TYPE;
    Vector3d ANTENNA_ENU;
    Vector3d MASTER_APPROX_POS;
    Vector3d ROVER_APPROX_POS;
    double   INTERVAL;

    QString  GPS_TYPES    [6];
    QString  GLONASS_TYPES[6];
    QString  BDS_TYPES    [6];
    QString  Galileo_TYPES[6];
    QString  QZSS_TYPES   [6];
    QString  SBAS_TYPES   [6];

    double   GPS_TGD      [40];
    double   BDS_TGD      [40][2];
    double   Galielo_TGD  [40][2];

    vector  <double> ALPHA;
    vector  <double> BETA;
    vector  <double> BDSA;
    vector  <double> BDSB;
};
/*                    ****************************************************                                       */
/*******************************      PPP result file(*.ppp)     *************************************************/
/*                    ****************************************************                                       */
/*------------------------------------------------------------------------------
 * Name     : ResultFile_PPP
 * Function : Output the result of PPP data
 *-----------------------------------------------------------------------------*/
class ResultFile_PPP : public FileCenter
{
public:
    virtual bool  outputFile();
    virtual bool  readFile  ();
    virtual void  setFilePath(const QString &FilePath){
                  filePath = FilePath;}
            void  getFilesPtr(PrecisionEphemerisFile       *eph_Fp,
                                PrecisionClockFile           *clk_Fp,
                                OceanTideFile                *oce_Fp,
                                AntennaInfoFile              *ant_Fp,
                                EarthRotationParameterFile   *erp_Fp,
                                ObservationFile              *obs_Fp,
                                IgsStationCoordinateFile     *coord_Fp,
                                vector<SunMoonPosition>      *smPos_p,
                                int                          *K_p);             // Note: This function must be preferred to get all file pointer
private:
    int  epochSysSatNum(        EpochObservationData          &epochObsData);
    bool dataSourceCheck();
    void inputPPPFile(          QTextStream                   &inText,
                                FinalDataFile                 &finalDataFile,
                                int *K_ptr);

    bool outputPPPFile(         QTextStream                   &outText,
                                FinalDataFile                 &finalDataFile);
    AntennaData outputCommonData(QTextStream                  &outText);
    void outputSingleEpochData( QTextStream                   &outText,
                                EpochSatelliteData            &epochOutData,
                                EpochObservationData          &epochObsData,
                                AntennaData                   &recAntData,
                          const Vector3d                      &antENU,
                          const Vector3d                      &recBLH,
                          const Vector3d                      &recXYZ,
                          const Vector3d                      &poleTide,
                          const CoordSysParameter             &coordPar);
    bool outputSingleSatelliteData(QTextStream                &outFile,
                                   SingleSatelliteData        &satData,
                                   const double               &threshold,
                                   const double               &cosAngle);

public: static FinalDataFile    finalDataFile;
    ResultFile_PPP(){ posOfPreEph = 0, posOfPreClk = 0,  posOfCoord  = 0;
                      posOfOcean  = 0, posOfErp    = 0,  posOfRecAnt = 0;}
private:
    int      posOfPreEph, posOfPreClk;
    int      posOfOcean , posOfErp;
    int      posOfRecAnt, posOfCoord;
    /*---------- Resource file pointer ----------*/
    int                        *K_ptr;                                          // The size of K will be set as 30
    ObservationFile            *obsDB_ptr;
    AntennaInfoFile            *antDB_ptr;
    PrecisionClockFile         *clkDB_ptr;
    OceanTideFile              *oceDB_ptr;
    EarthRotationParameterFile *erpDB_ptr;
    PrecisionEphemerisFile     *ephDB_ptr;
    IgsStationCoordinateFile   *coordDB_ptr;
    vector<SunMoonPosition>    *smPosDB_ptr;

};



//class ResultFile_NEU : public FileCenter
//{
//public:
//    virtual bool outputFile();
//    virtual void setFilePath(const QString &FilePath){
//        //filePath = FilePath + station+sh+".neu";
//    }

//};

//class ResultFile_RMS : public FileCenter
//{
//public:
//    virtual bool outputFile();
//    virtual void setFilePath(const QString &FilePath){
//       // filePath = FilePath + station+sh+".rms";
//    }
//};

//class ResultFile_ION : public FileCenter
//{
//public:
//    virtual bool outputFile();
//    virtual void setFilePath(const QString &FilePath){
//       // filePath = FilePath + station+sh+".ion";
//    }
//};
//class ResultFile_ResidualsTrace : public FileCenter
//{
//public:
//    virtual bool outputFile();
//    virtual void setFilePath(const QString &FilePath){
//       // filePath = FilePath + station+sh+".Residuals.rtrace";
//    }
//};

//class ResultFile_MatrixTrace : public FileCenter
//{
//public:
//    virtual bool outputFile();
//    virtual void setFilePath(const QString &FilePath){
//       // filePath = FilePath + station+sh+".Residuals.mtrace";
//    }
//};

#endif // OUTPUTDATACENTER_H
