#ifndef RENIXFILECENTER_H
#define RENIXFILECENTER_H
#include "FileCenter.h"
#include "RinexDataCenter.h"

/*---------------------------------------------------------------------
 * Name     : RinexFileCenter
 * Function : To deal rinex files of all types and all versions
 *--------------------------------------------------------------------*/
class RinexFileCenter
{
public :    
    virtual bool  readObserveFile(ObservationFile &obFile,
                                  const QString &filePath)  = 0;
    virtual bool  readNavigateFile(ObservationFile &obFile,
                                   const QString &filePath) = 0;

    static  int   readRinexVersion(const QString &filePath)               // Judge version of current rinex file
    {
        QFile readFile(filePath);
        if(!readFile.open(QIODevice::ReadOnly))
        {
            QMessageBox::warning(NULL,             "warning",
                              "Observation File Open faild!",
                               QMessageBox::Yes, QMessageBox::Yes);
            return false;
        }
        QString lineQStr = readFile.readLine();
        double  fVersion = lineQStr.mid(0,20).toDouble();
        int     iVersion = floor(fVersion);
        return  iVersion;
    }
};


/*                    ****************************************************                                       */
/*****************************    Rinex version 3 format  files     **********************************************/
/*                    ****************************************************                                       */
/*--------------------------------------------------------------
 * Function : Provides data help to read version 3 files
 *-------------------------------------------------------------*/
struct Rinex3ObsTypeLibrary                         /*only use L1 P1 6*/
{
    int      numC1, numL1,
             numP2, numL2,
             numP3, numL3;
    QString *C1_type;  /*Store the type of C1 */
    QString *L1_type;  /*Store the type of L1 */
    QString *P2_type;  /*Store the type of P2 */
    QString *L2_type;  /*Store the type of L2 */
    QString *P3_type;  /*Store the type of P3 */
    QString *L3_type;  /*Store the type of L3 */

    Rinex3ObsTypeLibrary()
    {
        C1_type = NULL, L1_type = NULL, P2_type = NULL,
        L2_type = NULL, P3_type = NULL, L3_type = NULL;
        numC1   = 0,    numL1   = 0,    numP2   = 0,
        numL2   = 0,    numP3   = 0,    numL3   = 0;
    }
   ~Rinex3ObsTypeLibrary()
    {
        delete [] C1_type;
        delete [] L1_type;
        delete [] P2_type;
        delete [] L2_type;
        delete [] P3_type;
        delete [] L3_type;
    }
};
/*---------------------------------------------------------------------
 * Name     : Rinex3_FileCenter
 * Function : Provides read functions of all type files in version 3
 *--------------------------------------------------------------------*/
class Rinex3_FileCenter : public RinexFileCenter
{
public:
    virtual bool readObserveFile(ObservationFile &obFile,              // Read Ofiles
                                 const QString &filePath);
    virtual bool readNavigateFile(ObservationFile &naFile,
                                   const QString &filePath) {}
private:
    void initialObsTypeLibraty();                                      // Initialize observation types libraty for version 3
    void getObserveDataInLine(ObservationData &ObsData,                // Split and get data in a line
                              int typePosArray[9],
                              QString lineQStr);

    int *getTypeIndex(QVector<QString> obsTypeOrder,                   // Get essential types index of this file's order
                      Rinex3ObsTypeLibrary system,
                      QString *obsTypeOfFile);
    int *setTypePos(QString system, int tempPosArray[6]);              // Set all types index in this file
private:

    Rinex3ObsTypeLibrary GPS_ObsTypes;
    Rinex3ObsTypeLibrary BDS_ObsTypes;
    Rinex3ObsTypeLibrary GLONASS_ObsTypes;
    Rinex3ObsTypeLibrary Galileo_ObsTypes;
    Rinex3ObsTypeLibrary QZSS_ObsTypes;
    Rinex3ObsTypeLibrary SBAS_ObsTypes;

    QString GPS_TYPES[6];                                               // Save types show in this files
    QString GLONASS_TYPES[6];
    QString BDS_TYPES[6];
    QString Galileo_TYPES[6];
    QString QZSS_TYPES[6];
    QString SBAS_TYPES[6];
};


/*                    ****************************************************                                       */
/*****************************    Rinex version 3 format  files     **********************************************/
/*                    ****************************************************                                       */

//class Rinex2_FileCenter : public RinexFileCenter
//{
//public:
//    virtual bool readObserveFile(ObservationFile &obFile,
//                                 const QString &filePath);

//public:

//private:

//};


#endif // RENIXFILECENTER_H
