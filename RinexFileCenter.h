#ifndef RENIXFILECENTER_H
#define RENIXFILECENTER_H
#include "FileCenter.h"
#include "RinexDataCenter.h"
#include <QFileInfo>

/*-----------------------------------------------------------------------
 * Name     : RinexFileCenter
 * Function : To deal rinex files of all types and all versions
 *----------------------------------------------------------------------*/
class RinexFileCenter
{
public :    
    virtual bool  readObserveFile (ObservationFile &obFile,
                                   const QString   &filePath) = 0;
    virtual bool  readNavigateFile(NavigationFile  &naFile,
                                   const QString   &filePath) = 0;

    static  int   readRinexVersion(const QString &filePath)              // Judge version of current rinex file
    {
        QFile readFile(filePath);
        if (!readFile.open(QIODevice::ReadOnly))
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

    static  QString readRinexType (const QString &filePath)              // Be used to get the type of file
    {

        QFileInfo fileInfo =  QFileInfo(filePath);
        QString fileName   =  fileInfo.absoluteFilePath();
        QString typeName   =  fileName.right(1);
        QString systemType = "NULL";

        /*------------- Type one ---------------------------------------*/
        if     (typeName == "N" || typeName == "n")                      // Deal with (*.n) (*.c) (*.l) (*.q)
            systemType    = "G";
        else if(typeName == "C" || typeName == "c")
            systemType    = "C";
        else if(typeName == "L" || typeName == "l")
            systemType    = "E";
        else if(typeName == "Q" || typeName == "q")
            systemType    = "J";
        /*------------- Type two ---------------------------------------*/
        if     (typeName == "G" || typeName == "g")                      // Deal with (*.g) (*.h)
            systemType    = "R";
        else if(typeName == "H" || typeName == "h")
            systemType    = "S";
        /*------------ Type mixed --------------------------------------*/
        if     (typeName == "P" || typeName == "p")                      // Deal with (*.p) [mixed type]
            systemType    = "MIXED";

        return systemType;
    }
protected:

    virtual bool readTypeOneNavFile  (const QString  &filePath,          // Function to read (*.n) (*.c) (*.l) (*.q)
                                      const QString  &system,
                                     NavigationFile  &naFile) = 0;
    virtual bool readTypeTwoNavFile  (const QString  &filePath,          // Function to read (*.g) (*.h)
                                      const QString  &system,
                                     NavigationFile  &naFile) = 0;
    virtual bool readTypeMixedNavFile(const QString  &filePath,          // Function to read (*.p)
                                     NavigationFile  &naFile) = 0;
};


/*                    ****************************************************                                                   */
/*****************************    Rinex version 3 format  files     **********************************************************/
/*                    ****************************************************                                                   */
/*-----------------------------------------------------------------------
 * Function : Help to read version 3 files
 *----------------------------------------------------------------------*/
struct Rinex3ObsTypeLibrary                                              // Only use C1 L1 P2 L2 P3 L3
{
    int      numC1, numL1,
             numP2, numL2,
             numP3, numL3;
    QString *C1_type;                                                    // Store the type of C1
    QString *L1_type;                                                    // Store the type of L1
    QString *P2_type;                                                    // Store the type of P2
    QString *L2_type;                                                    // Store the type of L2
    QString *P3_type;                                                    // Store the type of P3
    QString *L3_type;                                                    // Store the type of L3

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
/*------------------------------------------------------------------------
 * Name     : Rinex3_FileCenter
 * Function : Provides read functions of all type files in version 3
 *-----------------------------------------------------------------------*/
class Rinex3_FileCenter : public RinexFileCenter
{
public:
    virtual bool readObserveFile (ObservationFile &obFile,               // Read Observation files
                                  const QString &filePath);
    virtual bool readNavigateFile(NavigationFile &naFile,                // Read Navigation files
                                  const QString &filePath);
private:
    /*---------------- Functions for observation file ------------------*/
    void initialObsTypeLibraty();                                        // Initialize observation types libraty for version 3
    void getObserveDataInLine(ObservationData &ObsData,                  // Split and get data in a line
                              int typePosArray[9],
                              QString lineQStr);

    int *getTypeIndex(QVector<QString> obsTypeOrder,                     // Get essential types index of this file's order
                      Rinex3ObsTypeLibrary system,
                      QString *obsTypeOfFile);
    int *setTypePos(QString system, int tempPosArray[6]);                // Set all types index in this file
protected:
    /*---------------- Functions for Navigation file -------------------*/
    virtual bool readTypeOneNavFile  (const QString  &filePath,          // Function can read (*.n) (*.c) (*.l) (*.q)
                                      const QString  &system,
                                      NavigationFile &naFile);
    virtual bool readTypeTwoNavFile  (const QString  &filePath,          // Function can read (*.g) (*.h)
                                      const QString  &system,
                                      NavigationFile &naFile);
    virtual bool readTypeMixedNavFile(const QString  &filePath,          // Function to read (*.p)
                                      NavigationFile &naFile);
private:
    void readTypeOneData(QTextStream &inText, QString &lineQStr,
                         EpochNavigationData &navData);
    void readTypeTwoData(QTextStream &inText, QString &lineQStr,
                         EpochNavigationData &navData);

private:
    /*--------------- Helper data for observation file -----------------*/
    Rinex3ObsTypeLibrary GPS_ObsTypes;
    Rinex3ObsTypeLibrary BDS_ObsTypes;
    Rinex3ObsTypeLibrary GLONASS_ObsTypes;
    Rinex3ObsTypeLibrary Galileo_ObsTypes;
    Rinex3ObsTypeLibrary QZSS_ObsTypes;
    Rinex3ObsTypeLibrary SBAS_ObsTypes;

    QString GPS_TYPES[6];                                                // These array are used to save types show in this files
    QString GLONASS_TYPES[6];
    QString BDS_TYPES[6];
    QString Galileo_TYPES[6];
    QString QZSS_TYPES[6];
    QString SBAS_TYPES[6];

};


/*                    ****************************************************                                                   */
/*****************************    Rinex version 3 format  files     **********************************************************/
/*                    ****************************************************                                                   */

class Rinex2_FileCenter : public RinexFileCenter
{
public:
    virtual bool readObserveFile (ObservationFile &obFile,
                                  const QString   &filePath);
    virtual bool readNavigateFile(NavigationFile  &naFile,
                                  const QString   &filePath) {}

protected:

    virtual bool readTypeOneNavFile  (const QString  &filePath,
                                      const QString  &system,
                                      NavigationFile &naFile){}          // Function can read (*.n) (*.c) (*.l) (*.q)
    virtual bool readTypeTwoNavFile  (const QString  &filePath,
                                      const QString  &system,
                                      NavigationFile &naFile){}          // Function can read (*.g) (*.h)
    virtual bool readTypeMixedNavFile(const QString  &filePath,
                                      NavigationFile &naFile){}
};


#endif // RENIXFILECENTER_H
