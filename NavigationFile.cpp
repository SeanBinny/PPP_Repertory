#include "RinexDataCenter.h"
#include "RinexFileCenter.h"
#include "MyFunctionCenter.h"
#include <QFileInfo>

/*------------------------------------------------------------------------------
 * Name     : saveNavDataToArray
 * Function : save navigation data in specific system array
 * Input    : QString systemType .(To judge type)
 *            const EpochNavigationData &navData .(Data to be saved)
 * Output   : None
 *-----------------------------------------------------------------------------*/
void NavigationFile::saveNavDataToArray(const EpochNavigationData &navData)
{

    if     (navData.satType == "G")
    {
        if (GPS.empty())
            GPS.resize(32);
        GPS[navData.PRN-1].push_back(navData);
    }
    else if(navData.satType == "C")
    {
        if (BDS.empty())
            BDS.resize(35);
        BDS[navData.PRN-1].push_back(navData);
    }
    else if(navData.satType == "E")
    {
        if (GALILEO.empty())
            GALILEO.resize(40);
        GALILEO[navData.PRN-1].push_back(navData);
    }
    else if(navData.satType == "J")
    {
        if (QZSS.empty())
            QZSS.resize(40);
        QZSS[navData.PRN-1].push_back(navData);
    }
    else if(navData.satType == "I")
    {
        if (IRNSS.empty())
            IRNSS.resize(40);
        IRNSS[navData.PRN-1].push_back(navData);
    }
    else if(navData.satType == "R")
    {
        if (GLONASS.empty())
            GLONASS.resize(40), memset(K,0,sizeof(K));
        GLONASS[navData.PRN-1].push_back(navData);
        K[navData.PRN] = navData.dataTwoPtr->K;
    }
    else if(navData.satType == "S")
    {
        if (SBAS.empty())
            SBAS.resize(50);
        SBAS[navData.PRN-1].push_back(navData);
    }
}
/*-------------------------------------------------------------------------------
 * Name     : readFile
 * Function : Dynamically judging version and reading navigation file (*.*)
 * Input    : const QString &filePath
 * Output   : bool (if read success)
 *------------------------------------------------------------------------------*/
bool NavigationFile::readFile(const QString &filePath)
{
    int version = RinexFileCenter::readRinexVersion(filePath);                   // Judge version
    RinexFileCenter *rinexFilePtr = NULL;                                        // Dynamically reading file
    if (version == 2)
        rinexFilePtr = new Rinex2_FileCenter;
    if (version == 3)
        rinexFilePtr = new Rinex3_FileCenter;

    rinexFilePtr->readNavigateFile(*this, filePath);

    delete rinexFilePtr;
    return true;
}

/*                    ****************************************************                                       */
/*******************************    Rinex version 3 format files     *********************************************/
/*                    ****************************************************                                       */
/*-------------------------------------------------------------------------------
 * Name     : readNavigateFile
 * Function : Reading navigate file of version 3 format(*.*)
 * Input    : const QString &filePath
 *            Observation &nasFile .(Save navigation data)
 * Output   : bool (if read success)
 *------------------------------------------------------------------------------*/
bool Rinex3_FileCenter::readNavigateFile(NavigationFile &naFile,
                                         const QString  &filePath)
{

    QString systemType = RinexFileCenter::readRinexType(filePath);               // Get navigation file type

    if (systemType == "None")
    {
        QMessageBox::warning(NULL,              "warning",
                            "Navigation type read faild!",
                             QMessageBox::Yes, QMessageBox::Yes);
        return false;
    }

    /*------------- Type one --------------------------------------------------*/

    if (systemType == "G" || systemType == "C" ||
        systemType == "E" || systemType == "J")
        readTypeOneNavFile(filePath, systemType, naFile);
    /*------------- Type two --------------------------------------------------*/

    if (systemType == "R" || systemType ==  "S")
        readTypeTwoNavFile(filePath, systemType, naFile);
    /*------------ Type Mixed -------------------------------------------------*/

    if (systemType == "MIXED")
        readTypeMixedNavFile(filePath, naFile);
    return true;
}

/*-------------------------------------------------------------------------------
 * Name     : readTypeOneData
 * Function : Reading type one data block of navigation file
 * Input    : QFile &inFile .(Current file)
 *            const QString &filePath
 * Output   : EpochNavigationData    &navData .(Save epoch navigation data)
 *------------------------------------------------------------------------------*/
void Rinex3_FileCenter::readTypeOneData(QFile &inFile, QString &lineQStr,
                                        EpochNavigationData    &navData)
{
    navData.myTime.EPT.year   = lineQStr.mid(4, 4).toInt();
    navData.myTime.EPT.month  = lineQStr.mid(8, 3).toInt();
    navData.myTime.EPT.day    = lineQStr.mid(11,3).toInt();
    navData.myTime.EPT.hour   = lineQStr.mid(14,3).toInt();
    navData.myTime.EPT.minute = lineQStr.mid(17,3).toInt();
    navData.myTime.EPT.second = lineQStr.mid(20,3).toDouble();
    navData.myTime = MyFuncionCenter::timeIntegrator(navData.myTime.EPT);

    DataBlockOne       dataOne;
    dataOne.af0     =  lineQStr.mid(23,19).toDouble();
    dataOne.af1     =  lineQStr.mid(42,19).toDouble();
    dataOne.af2     =  lineQStr.mid(61,19).toDouble();

 /*...................................Broadcast Track 1.........................*/
    lineQStr        =  inFile.readLine();
    dataOne.Crs     =  lineQStr.mid(23,19).toDouble();
    dataOne.Delta_n =  lineQStr.mid(42,19).toDouble();
    dataOne.M0      =  lineQStr.mid(61,19).toDouble();

/*...................................Broadcast Track 2.........................*/
    lineQStr        =  inFile.readLine();
    dataOne.Cuc     =  lineQStr.mid(4, 19).toDouble();
    dataOne.ecc     =  lineQStr.mid(23,19).toDouble();
    dataOne.Cus     =  lineQStr.mid(42,19).toDouble();
    dataOne.sqrt_a  =  lineQStr.mid(61,19).toDouble();

/*...................................Broadcast Track 3.........................*/
    lineQStr        =  inFile.readLine();
    dataOne.TOE     =  lineQStr.mid(4, 19).toDouble();
    dataOne.Cic     =  lineQStr.mid(23,19).toDouble();
    dataOne.Omega0  =  lineQStr.mid(42,19).toDouble();
    dataOne.Cis     =  lineQStr.mid(61,19).toDouble();

/*...................................Broadcast Track 4.........................*/
    lineQStr        =  inFile.readLine();
    dataOne.i0      =  lineQStr.mid(4, 19).toDouble();
    dataOne.Crc     =  lineQStr.mid(23,19).toDouble();
    dataOne.omega   =  lineQStr.mid(42,19).toDouble();
    dataOne.Omegadot=  lineQStr.mid(61,19).toDouble();

/*...................................Broadcast Track 5.........................*/
    lineQStr        =  inFile.readLine();
    dataOne.idot    =  lineQStr.mid(4, 19).toDouble();
    dataOne.cflgL2  =  lineQStr.mid(23,19).toDouble();
    dataOne.weekNum =  lineQStr.mid(42,19).toDouble();
    dataOne.pflgL2  =  lineQStr.mid(61,19).toDouble();

/*...................................Broadcast Track 6.........................*/
    lineQStr        =  inFile.readLine();
    dataOne.sAccur  =  lineQStr.mid(4, 19).toDouble();
    dataOne.sHealth =  lineQStr.mid(23,19).toDouble();                          // Satellite health status(0=OK)
    dataOne.TGD     =  lineQStr.mid(42,19).toDouble();
    dataOne.IODC    =  lineQStr.mid(61,19).toDouble();

/*...................................Broadcast Track 7.........................*/
    lineQStr        =  inFile.readLine();
    dataOne.transmitTime
                    =  lineQStr.mid(4, 19).toDouble();

    navData.dataOnePtr =  new DataBlockOne();
   *navData.dataOnePtr =  dataOne;
}
/*-------------------------------------------------------------------------------
 * Name     : readTypeTwoData
 * Function : Reading type two data block of navigation file
 * Input    : QFile &inFile .(Current file)
 *            const QString &filePath
 * Output   : EpochNavigationData    &navData .(Save epoch navigation data)
 *------------------------------------------------------------------------------*/
void Rinex3_FileCenter::readTypeTwoData(QFile &inFile, QString &lineQStr,
                                        EpochNavigationData    &navData)
{
    navData.myTime.EPT.year   = lineQStr.mid(4, 4).toInt();
    navData.myTime.EPT.month  = lineQStr.mid(8, 3).toInt();
    navData.myTime.EPT.day    = lineQStr.mid(11,3).toInt();
    navData.myTime.EPT.hour   = lineQStr.mid(14,3).toInt();
    navData.myTime.EPT.minute = lineQStr.mid(17,3).toInt();
    navData.myTime.EPT.second = lineQStr.mid(20,3).toDouble();
    navData.myTime = MyFuncionCenter::timeIntegrator(navData.myTime.EPT);

    DataBlockTwo      dataTwo;
    dataTwo.TauN   =  lineQStr.mid(23,19).toDouble();
    dataTwo.GammaN =  lineQStr.mid(42,19).toDouble();
    dataTwo.TK     =  lineQStr.mid(61,19).toDouble();

/*...................................Broadcast Track 1.........................*/
    lineQStr       =  inFile.readLine();
    dataTwo.X      =  lineQStr.mid(4, 19).toDouble() * 1000.0;
    dataTwo.X_dot  =  lineQStr.mid(23,19).toDouble() * 1000.0;
    dataTwo.AC_X   =  lineQStr.mid(42,19).toDouble() * 1000.0;
    dataTwo.Bn     =  lineQStr.mid(61,19).toDouble();

/*...................................Broadcast Track 2.........................*/
    lineQStr       =  inFile.readLine();
    dataTwo.Y      =  lineQStr.mid(4, 19).toDouble() * 1000.0;
    dataTwo.Y_dot  =  lineQStr.mid(23,19).toDouble() * 1000.0;
    dataTwo.AC_Y   =  lineQStr.mid(42,19).toDouble() * 1000.0;
    dataTwo.K      =  lineQStr.mid(61,19).toDouble();
    dataTwo.K      =  int(dataTwo.K );

/*...................................Broadcast Track 3.........................*/
    lineQStr       =  inFile.readLine();
    dataTwo.Z      =  lineQStr.mid(4, 19).toDouble() * 1000.0;
    dataTwo.Z_dot  =  lineQStr.mid(23,19).toDouble() * 1000.0;
    dataTwo.AC_Z   =  lineQStr.mid(42,19).toDouble() * 1000.0;
    dataTwo.E      =  lineQStr.mid(61,19).toDouble();

    navData.dataTwoPtr =  new DataBlockTwo();
   *navData.dataTwoPtr =  dataTwo;
}


/*-------------------------------------------------------------------------------
 * Name     : readTypeOneNavFile
 * Function : Reading type one navigation file
 * Input    : const QString &system .(Help to choose type)
 *            const QString &filePath
 * Output   : NavigationFile &naFile .(Final data struct)
 *            bool (if read success)
 *------------------------------------------------------------------------------*/
bool Rinex3_FileCenter::readTypeOneNavFile(const QString  &filePath,
                                           const QString  &system,
                                           NavigationFile &naFile)
{
    QFile navInFile(filePath);
    if(!navInFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(NULL,              "warning",
                            "Navigation file open faild!",
                             QMessageBox::Yes, QMessageBox::Yes);
        return false;
    }

    QString lineQStr = "";
    while ((lineQStr = navInFile.readLine()).indexOf("END OF HEADER") < 0);      // Ignore the header block
    /*--------------------- Read data ------------------------------------------*/
    while ((lineQStr = navInFile.readLine()) != "")
    {
        EpochNavigationData navData;                                             // Get type and satellite number
        navData.satType  =  system;
        navData.PRN      =  lineQStr.mid(1,2).toInt();

        readTypeOneData(navInFile, lineQStr, navData);

        if (navData.dataOnePtr->sHealth != 0)                                    // Judge if satellite is healthy
            continue;
        naFile.saveNavDataToArray(navData);                                      // Store data
    }
    navInFile.close();
    return true;
}
/*-------------------------------------------------------------------------------
 * Name     : readTypeTwoNavFile
 * Function : Reading type two navigation file
 * Input    : const QString &system .(Help to choose type)
 *            const QString &filePath
 * Output   : NavigationFile &naFile .(Final data struct)
 *            bool (if read success)
 *------------------------------------------------------------------------------*/
bool Rinex3_FileCenter::readTypeTwoNavFile(const QString  &filePath,
                                           const QString  &system,
                                           NavigationFile &naFile)
{
    QFile navInFile(filePath);
    if(!navInFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(NULL,              "warning",
                            "Navigation file open faild!",
                             QMessageBox::Yes, QMessageBox::Yes);
        return false;
    }

    QString lineQStr = "";

    while ((lineQStr = navInFile.readLine()).indexOf("END OF HEADER") < 0);      // Ignore the header block
    /*--------------------- Read data ------------------------------------------*/
    while ((lineQStr = navInFile.readLine()) != "")
    {
        EpochNavigationData navData;                                             // Get type and satellite number
        navData.satType  =  system;
        navData.PRN      =  lineQStr.mid(1,2).toInt();

        readTypeTwoData(navInFile, lineQStr, navData);

        if (navData.dataTwoPtr->Bn != 0)                                         // Judge if satellite is healthy
            continue;
        naFile.saveNavDataToArray(navData);
    }

    navInFile.close();
    return true;
}
/*-------------------------------------------------------------------------------
 * Name     : readTypeMixedNavFile
 * Function : Reading type Mixed navigation file
 * Input    : const QString &system .(Help to choose type)
 *            const QString &filePath
 * Output   : NavigationFile &naFile .(Final data struct)
 *            bool (if read success)
 *------------------------------------------------------------------------------*/
bool Rinex3_FileCenter::readTypeMixedNavFile(const QString  &filePath,
                                             NavigationFile &naFile)
{
    QFile navInFile(filePath);
    if(!navInFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(NULL,              "warning",
                            "Navigation file open faild!",
                             QMessageBox::Yes, QMessageBox::Yes);
        return false;
    }

    QString lineQStr = "";

    while ((lineQStr = navInFile.readLine()).indexOf("END OF HEADER") < 0);      // Ignore the header block
    /*--------------------- Read data ------------------------------------------*/
    while ((lineQStr = navInFile.readLine()) != "")
 //   for (int i = 0; i < 500;i ++)
    {
        EpochNavigationData navData;                                             // Get type and satellite number
        navData.satType     =  lineQStr.mid(0,1);
        navData.PRN         =  lineQStr.mid(1,2).toInt();
        if (navData.satType == "G" || navData.satType == "C" ||
            navData.satType == "E" || navData.satType == "J" ||
            navData.satType == "I")
        {
            readTypeOneData(navInFile, lineQStr,navData);
            if (navData.dataOnePtr->sHealth != 0)                                // Judge if satellite is healthy
                continue;
            naFile.saveNavDataToArray(navData);
        }
        else if (navData.satType == "R" || navData.satType == "S" )
        {
            readTypeTwoData(navInFile, lineQStr, navData);
            if (navData.dataTwoPtr->Bn != 0)                                     // Judge if satellite is healthy
                continue;
            naFile.saveNavDataToArray(navData);
        }
    }
    navInFile.close();
    return true;
}

/*                    ****************************************************                                       */
/*******************************    Rinex version 2 format files     *********************************************/
/*                    ****************************************************                                       */
