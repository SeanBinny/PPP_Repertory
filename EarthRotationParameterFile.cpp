#include "DataCenter.h"

/*------------------ Define static member--------------------------------------*/
vector <ErpData> EarthRotationParameterFile::allErpData;
/*-----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * Name     : readFile
 * Function : read earth rotation parameter file (*.erp)
 * Input    : const QString &filePath
 * Output   : bool (if read success)
 *-----------------------------------------------------------------------------*/
bool EarthRotationParameterFile::readFile(const QString &filePath)
{
    QFile EarthFile(filePath);
    if(!  EarthFile.open( QIODevice::ReadOnly ))
    {
          QMessageBox::warning(NULL,                   "warning",
                              "Precision earth rotation parameter "
                              "File Open faild!",
                               QMessageBox::Yes, QMessageBox::Yes);
          return false;
    }

    bool flag1 = false;  /* Unknown */
    bool flag2 = false;  /* Unknown */
    /*--------------------------  Read Head -----------------------------------*/
    QString lineQStr = "";
    while((lineQStr  = EarthFile.readLine())!= "")
    {
        if (lineQStr.indexOf("dpsi") >= 0)
            flag1  = true;
        if (lineQStr.indexOf("deps") >= 0)
            flag2  = true;
        if (lineQStr.indexOf("MJD")  >= 0){
            lineQStr = EarthFile.readLine();                                   // To Skip a unuseful line
            break;
        }
    }
    /*--------------------------  Read Data -----------------------------------*/
    while((lineQStr = EarthFile.readLine())!= "")
    {
        ErpData erpData;
        erpData.myTime.MJD = lineQStr.mid(0,  8).toDouble();
        erpData.myTime.JD  = erpData.myTime.MJD + JDtoMJD;

        erpData.xPole      = lineQStr.mid(8,  9).toDouble() * 1e-6;
        erpData.yPole      = lineQStr.mid(17, 9).toDouble() * 1e-6;
        erpData.UT1mUTC    = lineQStr.mid(26,10).toDouble();

        if (flag1)
            erpData.dPsi   = lineQStr.mid(112,7).toDouble();
        if (flag2)
            erpData.dEps   = lineQStr.mid(119,8).toDouble();

        allErpData.push_back(erpData);
    }

    EarthFile.close();
    return true;
}
