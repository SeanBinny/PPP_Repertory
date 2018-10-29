#include "ProductDataCenter.h"

/*----------------------------------------------------------------------------
 * Name     : readFile
 * Function : read different code bias file (*.bsx)
 * Input    : NULL
 * Output   : bool (if read success)
 *--------------------------------------------------------------------------*/
bool DifferentCodeBiasFile::readFile()
{
    if (!fileCommonDeal("Differernt Code Bias file open faild!"))
        return false;
    QTextStream inText(&inFile);
    QString lineQStr  = "";
    while ((lineQStr  = inText.readLine()).indexOf("+BIAS/SOLUTION") < 0);    // Ignore the unuseful line
    /*-----------------------  Read data ------------------------------------*/
    while ((lineQStr  = inText.readLine()) != "")
    {
       if (lineQStr.mid(1, 3) == "DCB")
       {
           if(lineQStr.mid(12,2) == "  ")
               break;

           QString system    = lineQStr.mid(6,  1);
           int     prnNum    = lineQStr.mid(12, 2).toInt();
           int     doy       = lineQStr.mid(43, 3).toInt();
           double  tempData  = lineQStr.mid(71,21).toDouble()*10E-10 * LIGHT_V;// Extract data from the line
           if      (system  == "G")
           {
                if (GPS_DCB == NULL)
                    GPS_DCB  = new DCB_GPS [32]();
                if     (lineQStr.indexOf("C1C  C1W")  >= 0)
                  GPS_DCB[prnNum - 1].C1C_C1W[doy - 1] = tempData;
                else if(lineQStr.indexOf("C1C  C2W")  >= 0)
                  GPS_DCB[prnNum - 1].C1C_C2W[doy - 1] = tempData;
                else if(lineQStr.indexOf("C2W  C2S")  >= 0)
                  GPS_DCB[prnNum - 1].C2W_C2S[doy - 1] = tempData;
                else if(lineQStr.indexOf("C2W  C2L")  >= 0)
                  GPS_DCB[prnNum - 1].C2W_C2L[doy - 1] = tempData;
                else if(lineQStr.indexOf("C2W  C2X")  >= 0)
                  GPS_DCB[prnNum - 1].C2W_C2X[doy - 1] = tempData;
                else if(lineQStr.indexOf("C1C  C5Q")  >= 0)
                  GPS_DCB[prnNum - 1].C1C_C5Q[doy - 1] = tempData;
                else if(lineQStr.indexOf("C1C  C5X")  >= 0)
                  GPS_DCB[prnNum - 1].C1C_C5X[doy - 1] = tempData;
                else if(lineQStr.indexOf("C1W  C2W")  >= 0)
                  GPS_DCB[prnNum - 1].C1W_C2W[doy - 1] = tempData;
           }
           else if (system  == "C")
           {
                if (BDS_DCB == NULL)
                    BDS_DCB  = new DCB_BDS [36]();
                if     (lineQStr.indexOf("C2I  C7I")  >= 0)
                  BDS_DCB[prnNum - 1].C2I_C7I[doy - 1] = tempData;
                else if(lineQStr.indexOf("C2I  C6I")  >= 0)
                  BDS_DCB[prnNum - 1].C2I_C6I[doy - 1] = tempData;
                else if(lineQStr.indexOf("C7I  C6I")  >= 0)
                  BDS_DCB[prnNum - 1].C7I_C6I[doy - 1] = tempData;
           }
           else if (system == "R")
           {
                if (GLONASS_DCB == NULL)
                    GLONASS_DCB = new DCB_GLONASS [32]();
                if     (lineQStr.indexOf("C1C  C2P") >= 0)
                  GLONASS_DCB[prnNum - 1].C1C_C1P[doy - 1] = tempData;
                else if(lineQStr.indexOf("C1C  C2C") >= 0)
                  GLONASS_DCB[prnNum - 1].C1C_C1P[doy - 1] = tempData;
                else if(lineQStr.indexOf("C1C  C2P") >= 0)
                  GLONASS_DCB[prnNum - 1].C1C_C2P[doy - 1] = tempData;
                else if(lineQStr.indexOf("C1P  C2P") >= 0)
                  GLONASS_DCB[prnNum - 1].C1P_C2P[doy - 1] = tempData;
                else if(lineQStr.indexOf("C2C  C2P") >= 0)
                  GLONASS_DCB[prnNum - 1].C2C_C2P[doy - 1] = tempData;
           }
           else if (system == "E")
           {
               if (GALILEO_DCB == NULL)
                   GALILEO_DCB = new DCB_GALILEO [36]();
               if     (lineQStr.indexOf("C1C  C5Q")   >= 0)
                   GALILEO_DCB[prnNum - 1].C1C_C5Q[doy - 1] = tempData;
               else if(lineQStr.indexOf("C1X  C5X")   >= 0)
                   GALILEO_DCB[prnNum - 1].C1X_C5X[doy - 1] = tempData;
               else if(lineQStr.indexOf("C1C  C7Q")   >= 0)
                   GALILEO_DCB[prnNum - 1].C1C_C7Q[doy - 1] = tempData;
               else if(lineQStr.indexOf("C1X  C7X")   >= 0)
                   GALILEO_DCB[prnNum - 1].C1X_C7X[doy - 1] = tempData;
               else if(lineQStr.indexOf("C1C  C8Q")   >= 0)
                   GALILEO_DCB[prnNum - 1].C1C_C8Q[doy - 1] = tempData;
               else if(lineQStr.indexOf("C1X  C8X")   >= 0)
                   GALILEO_DCB[prnNum - 1].C1X_C8X[doy - 1] = tempData;
           }
       }
    }
    closeFile();
    return true;
}

/*------------------------------ Constructors -------------------------------*/
DCB_GPS::DCB_GPS()
{
    C1C_C1W = new double [366], C1C_C2W = new double [366];
    C2W_C2S = new double [366], C2W_C2L = new double [366];
    C2W_C2X = new double [366], C1C_C5Q = new double [366];
    C1C_C5X = new double [366], C1W_C2W = new double [366];
}
DCB_BDS::DCB_BDS()
{
    C2I_C7I = new double [366], C2I_C6I = new double [366];
    C7I_C6I = new double [366];
}
DCB_GLONASS::DCB_GLONASS()
{
    C1C_C1P = new double [366], C1C_C2C = new double [366];
    C1C_C2P = new double [366], C1P_C2P = new double [366];
    C2C_C2P = new double [366];
}
DCB_GALILEO::DCB_GALILEO()
{
    C1C_C5Q = new double [366], C1X_C5X = new double [366];
    C1C_C7Q = new double [366], C1X_C7X = new double [366];
    C1C_C8Q = new double [366], C1X_C8X = new double [366];
}

DifferentCodeBiasFile::DifferentCodeBiasFile()
{
    GPS_DCB = NULL, GLONASS_DCB = NULL;
    BDS_DCB = NULL, GALILEO_DCB = NULL;
}

/*---------------------------- Disconstructors -------------------------------*/
DCB_GPS::~DCB_GPS()
{
    delete [] C1C_C1W;
    delete [] C1C_C2W;
    delete [] C2W_C2S;
    delete [] C2W_C2L;
    delete [] C2W_C2X;
    delete [] C1C_C5Q;
    delete [] C1C_C5X;
    delete [] C1W_C2W;
}
DCB_BDS::~DCB_BDS()
{
    delete [] C2I_C7I;
    delete [] C2I_C6I;
    delete [] C7I_C6I;
}
DCB_GLONASS::~DCB_GLONASS()
{
    delete [] C1C_C1P;
    delete [] C1C_C2C;
    delete [] C1C_C2P;
    delete [] C1P_C2P;
    delete [] C2C_C2P;
}
DCB_GALILEO::~DCB_GALILEO()
{
    delete [] C1C_C5Q;
    delete [] C1X_C5X;
    delete [] C1C_C7Q;
    delete [] C1X_C7X;
    delete [] C1C_C8Q;
    delete [] C1X_C8X;
}

DifferentCodeBiasFile::~DifferentCodeBiasFile()
{
    delete [] GPS_DCB;
    delete [] BDS_DCB;
    delete [] GLONASS_DCB;
    delete [] GALILEO_DCB;
}
