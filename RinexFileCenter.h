#ifndef RENIXFILECENTER_H
#define RENIXFILECENTER_H
#include "FileCenter.h"
#include "RinexDataCenter.h"

class RinexFileCenter
{
public :
    virtual bool  readObserveFile(ObservationFile &obFile,
                                  const QString &filePath) = 0;

    static  int   rinex_version(const QString &filePath)                                //
    {
        QFile readFile(filePath);
        if(!readFile.open(QIODevice::ReadOnly))
        {
            QMessageBox::warning(NULL,                   "warning",
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


struct Rinex3ObsTypeLibrary
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

class Rinex3_FileCenter : public RinexFileCenter
{
public:
    virtual bool readObserveFile(ObservationFile &obFile,
                                 const QString &filePath);
private:
    void initialObsTypeLibraty();
    int *getTypeIndex(QVector<QString> obsTypeOrder,
                      Rinex3ObsTypeLibrary system,
                      QString *obsTypeOfFile);

public:


private:

    Rinex3ObsTypeLibrary GPS_ObsTypes;
    Rinex3ObsTypeLibrary BDS_ObsTypes;
    Rinex3ObsTypeLibrary GLONASS_ObsTypes;
    Rinex3ObsTypeLibrary Galileo_ObsTypes;
    Rinex3ObsTypeLibrary QZSS_ObsTypes;
    Rinex3ObsTypeLibrary SBAS_ObsTypes;


    /*------------------- Help read observation -----------------*/
    int      GPS_ObsTypeIndex[9];                                // Match to the position of the observed value type*/
    int      BDS_ObsTypeIndex[9];
    int      GLONASS_ObsTypeIndex[9];
    int      Galileo_ObsTypeIndex[9];
    int      QZSS_ObsTypeIndex[9];
    int      SBAS_ObsTypeIndex[9];

    QString GPS_TYPES[6];       /*For Storing the observed type*/
    QString GLONASS_TYPES[6];
    QString BDS_TYPES[6];
    QString Galileo_TYPES[6];
    QString QZSS_TYPES[6];
    QString SBAS_TYPES[6];


};


class Rinex2_FileCenter : public RinexFileCenter
{
public:
    virtual bool readObserveFile(ObservationFile &obFile,
                                 const QString &filePath);

public:

private:

};
#endif // RENIXFILECENTER_H
