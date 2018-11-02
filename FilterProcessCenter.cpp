#include <math.h>
#include "DataProcessCenter.h"
#include "FunctionalModelCenter.h"

/*------------------ Define static member---------------------------------------*/
bool ModeFlag::hasGPS     = false;
bool ModeFlag::hasBDS     = false;
bool ModeFlag::hasGLONASS = false;
bool ModeFlag::hasGalileo = false;

bool ModeFlag::P1_P2      = false;
bool ModeFlag::P1_P3      = false;
bool ModeFlag::P2_P3      = false;

bool ModeFlag::frequency3 = false; /* unknown*/

bool ModeFlag::Model_UD   = false;                                              // Un-Difference ionosphere-free combined model
bool ModeFlag::Model_UC   = false;                                              // Un-Combined Model
bool ModeFlag::TF         = false;                                              // Triple frequence
bool ModeFlag::SF_PPP     = false; /*unknown*/                                  // Single frequence

bool ModeFlag::dynamic    = false;                                              // Real-time processing
bool ModeFlag::back       = false;                                              // Post processing

int  ModeFlag::systemNum  = false;

/*------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 * Name     : siftSatellite
 * Function : Sift, classfy and save valid satellite
 * Input    : FinalDataFile &pppFile,
 *            int pos, .(current epoch)
 * Output   : int (illegal satellite to be changed)
 *-----------------------------------------------------------------------------*/
void FilterProcessingCenter::siftSatellite(FinalDataFile &pppFile, int pos)
{
    nSatContaine.clear();
    sSatContaine.clear();
    for (int n = 0; n < pppFile.allSatelliteData[pos].sateNum; n++)             // Recyle current epoch
    {
        if (!ObsJudgeCenter::isSystemTypeValid(                                 // Judge if it is one of the four systems
             pppFile.allSatelliteData[pos].eSatData[n].obsData.satTN))
             continue;

        double oC1 = 0, oP2 = 0, oL1 = 0, oL2 = 0, oP3 = 0, oL3 = 0;
        double nC1 = 0, nP2 = 0, nL1 = 0, nL2 = 0, nP3 = 0, nL3 = 0;

        double oWu = 0, oAg = 0, nWu = 0, nAg = 0, f1  = 0, f2  = 0, f3 = 0;
        if      (ModeFlag::P1_P2)
        {
            nC1 = pppFile.allSatelliteData[pos].eSatData[n].obsData.C1;
            nP2 = pppFile.allSatelliteData[pos].eSatData[n].obsData.P2;
            nL1 = pppFile.allSatelliteData[pos].eSatData[n].obsData.L1;
            nL2 = pppFile.allSatelliteData[pos].eSatData[n].obsData.L2;
        }
        else if (ModeFlag::P1_P3)
        {
            nC1 = pppFile.allSatelliteData[pos].eSatData[n].obsData.C1;
            nP2 = pppFile.allSatelliteData[pos].eSatData[n].obsData.P3;
            nL1 = pppFile.allSatelliteData[pos].eSatData[n].obsData.L1;
            nL2 = pppFile.allSatelliteData[pos].eSatData[n].obsData.L3;
        }
        else if (ModeFlag::P2_P3)
        {
            nC1 = pppFile.allSatelliteData[pos].eSatData[n].obsData.P2;
            nP2 = pppFile.allSatelliteData[pos].eSatData[n].obsData.P3;
            nL1 = pppFile.allSatelliteData[pos].eSatData[n].obsData.L2;
            nL2 = pppFile.allSatelliteData[pos].eSatData[n].obsData.L3;
        }
        else if (ModeFlag::TF)
        {
            nC1 = pppFile.allSatelliteData[pos].eSatData[n].obsData.C1;
            nP2 = pppFile.allSatelliteData[pos].eSatData[n].obsData.P2;
            nL1 = pppFile.allSatelliteData[pos].eSatData[n].obsData.L1;
            nL2 = pppFile.allSatelliteData[pos].eSatData[n].obsData.L2;
            nP3 = pppFile.allSatelliteData[pos].eSatData[n].obsData.P3;
            nL3 = pppFile.allSatelliteData[pos].eSatData[n].obsData.L3;
        }
        nAg     = pppFile.allSatelliteData[pos].eSatData[n].DAZ[2];
        nWu     = pppFile.allSatelliteData[pos].eSatData[n].phaseWinding;

        /*-------------- Deal with the first epoch ---------------------------*/
        if (pos == 0)
        {
            oC1 = nC1, oP2 = nP2, oL1 = nL1, oL2 = nL2, f1 = f2 = f3 = 0;
            oP3 = nP3, oL3 = nL3, oAg = nAg, oWu = nWu;
        }
        else /*-------------- Deal with the other epoch ----------------------*/
        {
            for (int j = 0; j < pppFile.allSatelliteData[pos-1].sateNum; j++)
            {
                if (pppFile.allSatelliteData[pos    ].eSatData[n].obsData.satTN !=
                    pppFile.allSatelliteData[pos - 1].eSatData[j].obsData.satTN)
                    continue;

                if      (ModeFlag::P1_P2)
                {
                    f1  = pppFile.allSatelliteData[pos-1].eSatData[j].freq[0];
                    f2  = pppFile.allSatelliteData[pos-1].eSatData[j].freq[1];
                    oC1 = pppFile.allSatelliteData[pos-1].eSatData[j].obsData.C1;
                    oP2 = pppFile.allSatelliteData[pos-1].eSatData[j].obsData.P2;
                    oL1 = pppFile.allSatelliteData[pos-1].eSatData[j].obsData.L1;
                    oL2 = pppFile.allSatelliteData[pos-1].eSatData[j].obsData.L2;
                }
                else if (ModeFlag::P1_P3)
                {
                    f1  = pppFile.allSatelliteData[pos-1].eSatData[j].freq[0];
                    f2  = pppFile.allSatelliteData[pos-1].eSatData[j].freq[2];
                    oC1 = pppFile.allSatelliteData[pos-1].eSatData[j].obsData.C1;
                    oP2 = pppFile.allSatelliteData[pos-1].eSatData[j].obsData.P3;
                    oL1 = pppFile.allSatelliteData[pos-1].eSatData[j].obsData.L1;
                    oL2 = pppFile.allSatelliteData[pos-1].eSatData[j].obsData.L3;
                }
                else if (ModeFlag::P2_P3)
                {
                    f1  = pppFile.allSatelliteData[pos-1].eSatData[j].freq[1];
                    f2  = pppFile.allSatelliteData[pos-1].eSatData[j].freq[2];
                    oC1 = pppFile.allSatelliteData[pos-1].eSatData[j].obsData.P2;
                    oP2 = pppFile.allSatelliteData[pos-1].eSatData[j].obsData.P3;
                    oL1 = pppFile.allSatelliteData[pos-1].eSatData[j].obsData.L2;
                    oL2 = pppFile.allSatelliteData[pos-1].eSatData[j].obsData.L3;
                }
                else if (ModeFlag::TF)
                {
                    f1  = pppFile.allSatelliteData[pos-1].eSatData[j].freq[0];
                    f2  = pppFile.allSatelliteData[pos-1].eSatData[j].freq[1];
                    f3  = pppFile.allSatelliteData[pos-1].eSatData[j].freq[2];

                    oC1 = pppFile.allSatelliteData[pos-1].eSatData[j].obsData.C1;
                    oP2 = pppFile.allSatelliteData[pos-1].eSatData[j].obsData.P2;
                    oL1 = pppFile.allSatelliteData[pos-1].eSatData[j].obsData.L1;
                    oL2 = pppFile.allSatelliteData[pos-1].eSatData[j].obsData.L2;
                    oP3 = pppFile.allSatelliteData[pos-1].eSatData[j].obsData.P3;
                    oL3 = pppFile.allSatelliteData[pos-1].eSatData[j].obsData.L3;
                }
                oWu     = pppFile.allSatelliteData[pos-1].eSatData[j].phaseWinding;
                oAg     = pppFile.allSatelliteData[pos-1].eSatData[j].DAZ[2];
                break;
            }
        }
        /*------------------------ Classfy and save -----------------------------*/
        QString judgeResult = ObsJudgeCenter::isObservationValid( oC1, oP2, oL1,
                                        oL2, oP3, oL3, nC1, nP2, nL1,  nL2, nP3,
                                        nL3, oWu, nWu, oAg, nAg, f1,   f2,  f3);
        if (pos == 0 && judgeResult != "Not Enough")
            judgeResult = "Valid";                                             // Delete first epoch judge error
        SatQuery satQuery(pppFile.allSatelliteData[pos].eSatData[n].
                                                 obsData.satTN,  n);
        if (     judgeResult == "Valid")
            nSatContaine.push_back(satQuery);     
        else if (judgeResult == "Slip")
            sSatContaine.push_back(satQuery);                                  // Return the illegal epoch

    }
}

/*------------------------------------------------------------------------------
 * Name     : filterProcessing
 * Function : Function of main processing
 * Input    : FinalDataFile &initialData,
 *            int           *K
 * Output   : QTextStream   &rmsText,
 *            QTextStream   &neuText,
 *            QTextStream   &ionText,
 *            QTextStream   &rTraceText,
 *            QTextStream   &mTraceText,
 *-----------------------------------------------------------------------------*/
void FilterProcessingCenter::filterProcessing(FinalDataFile &initialData,
//                                              QTextStream   &rmsText,
                                              QTextStream   &neuText
//                                              QTextStream   &ionText,
//                                              QTextStream   &rTraceText,
//                                              QTextStream   &mTraceText,
                                              /*int *K*/)
{


//    QString sh="";
//    if(flaggps==1)
//        sh+="GPS";
//    if(flagbds==1)
//        sh+=" BDS";
//    if(flagglonass==1)
//        sh+=" GLONASS";
//    if(flagGalileo==1)
//        sh+=" GALILEO";

//    MatrixXd Q,Qw,F,I;
//    VectorXd x,V;
//    VectorXd x_h;//
//    QVector<int> sats1;
//    QVector<QString>satstype1;


//    QString pptah  = gongshifile2[0].Folder_Path+gongshifile2[0].station+sh+".NEU";
//    QString pptah1 = gongshifile2[0].Folder_Path+gongshifile2[0].station+sh+".RMS";

//    QString pptah2 = gongshifile2[0].Folder_Path+gongshifile2[0].station+sh+"Residuals.trace";

//    QString pptah3 = gongshifile2[0].Folder_Path+gongshifile2[0].station+sh+"Matrix.traceM";

//    QString ppath4 = gongshifile2[0].Folder_Path+gongshifile2[0].station+sh+".ion";

//    QTextStream sw2(&openfile2);
//    sw2.setRealNumberNotation(QTextStream::FixedNotation);
//    sw2.setFieldAlignment(QTextStream::AlignRight);
//    sw2.setRealNumberPrecision(6);

//    QTextStream sw3(&openfile3);
//    sw3.setRealNumberNotation(QTextStream::FixedNotation);
//    sw3.setFieldAlignment(QTextStream::AlignRight);
//    sw3.setRealNumberPrecision(10);

//    QTextStream sw1(&openfile1);
//    sw1.setRealNumberNotation(QTextStream::FixedNotation);
//    sw1.setFieldAlignment(QTextStream::AlignRight);
//    sw1.setRealNumberPrecision(10);


//    QTextStream sw(&openfile);
//    sw.setRealNumberNotation(QTextStream::FixedNotation);
//    sw.setFieldAlignment(QTextStream::AlignRight);
//    sw.setRealNumberPrecision(6);


//    QTextStream sw4(&openfile4);
//    sw4.setRealNumberNotation(QTextStream::FixedNotation);
//    sw4.setFieldAlignment(QTextStream::AlignRight);
//    sw4.setRealNumberPrecision(10);

//    sw<<qSetFieldWidth(24)<<sh;

//    sw<<qSetFieldWidth(4)<<"N(m)"<<qSetFieldWidth(15)<<"E(m)"<<qSetFieldWidth(15)<<"U(m)"<<qSetFieldWidth(15)<<"sdn(m)"
//     <<qSetFieldWidth(15)<<"sde(m)"<<qSetFieldWidth(15)<<"sdu(m)"<<qSetFieldWidth(15)<<"satnum"<<qSetFieldWidth(15)<<"PDOP"
//    <<qSetFieldWidth(15)<<"B(°)"<<qSetFieldWidth(15)<<"L(°)"<<qSetFieldWidth(15)<<"H(m)"
//    <<qSetFieldWidth(20)<<"X(m)"<<qSetFieldWidth(20)<<"Y(m)"<<qSetFieldWidth(20)<<"Z(m)"
//    <<qSetFieldWidth(15)<<"        ZTD(m)"<<qSetFieldWidth(20)<<"clock(s)"<<endl;//<<qSetFieldWidth(15)<<"clock C-G(m)"<<qSetFieldWidth(15)<<"clock R-G(m)"<<qSetFieldWidth(15)<<"clock E-G(m)"<<endl;


//    write_residuals_header(sw2);

//    double sumv=0;////////////////////////////
//    int mt=0;  ///////////////////////////////


    FuncModel *theModel_ptr = NULL;

    if      (ModeFlag::Model_UD)                                                   // Choose current processing model
        theModel_ptr = new UD_Model;
    else if (ModeFlag::Model_UC)
        theModel_ptr = new UC_Model;
    else if (ModeFlag::Model_UD && ModeFlag::TF)
        theModel_ptr = new TF_UD_Model;
    else if (ModeFlag::Model_UC && ModeFlag::TF)
        theModel_ptr = new TF_UC_Model;

    theModel_ptr->sumOfSys =  ModeFlag::systemNum;                                 // Number of different system satellites
    if (ModeFlag::TF)   /*unknow*/                                                 // Triple frequency IF PPP add IFB parameter
        theModel_ptr->sumOfSys++;

    X_Y_Z xyzCoord(0,0,0);
    int   sum = 0;/* unknown*/
    CoordSysParameter coordParameter;
    MyFunctionCenter::getCoordSysParameter(WGS_84, coordParameter);                // Get global Coordinate frame parameters
    for (int epoch  = 0; epoch <  initialData.allSatelliteData.size(); epoch++)
    {

        siftSatellite(initialData, epoch);
        bool isSlipOrRise = false;                                                 // Classify valid and slip satellite
        if (0 == epoch) /*-------- Deal with the  first epoch ---------------------*/{

            int satNum   = initialData.allSatelliteData[epoch].sateNum;
            theModel_ptr->initKalman(satNum);
            oSatContaine = nSatContaine;                                           // Save the old status
        }
        else /*-------- Deal with the  first epoch --------------------------------*/
        {      
            /*__________According state of satellite to Reset Matrix ______________*/
            for (int i = 0; i < oSatContaine.size(); i++)
            {
                int lostSat = MyFunctionCenter::isSatFound(nSatContaine, oSatContaine[i]);
                if (lostSat == -1)                                                 // It shows satllite is not in next epoch that means lost or slip
                {
                    int slipSat = MyFunctionCenter::isSatFound(sSatContaine, oSatContaine[i]);
                    if (slipSat == -1){                                            // Lost
                        theModel_ptr->dealLost(i);
                        oSatContaine.erase(oSatContaine.begin()+i--);
                    }else{
                        theModel_ptr->dealSlip(i);                                 // Slip
                        isSlipOrRise = true;}
                }
            }
            for (int i = 0; i < nSatContaine.size(); i++)
            {
                if (MyFunctionCenter::isSatFound(oSatContaine, nSatContaine[i]) == -1)
                {   theModel_ptr->dealRise();                                      // Rise
                    oSatContaine.push_back(nSatContaine[i]);
                    isSlipOrRise = true;    }
            }
            /*____________________________________________________________________*/
         }
        theModel_ptr->set_I_F();
        theModel_ptr->modelSolution(initialData, oSatContaine, epoch);

        if(isSlipOrRise){ // Unknown
            MyFunctionCenter::kalmanFilter(
                    theModel_ptr->I, theModel_ptr->F, theModel_ptr->R,
                    theModel_ptr->B, theModel_ptr->L, theModel_ptr->Qw,
                    theModel_ptr->Q, theModel_ptr->X, theModel_ptr->V);
        }
        MyFunctionCenter::kalmanFilter(
                theModel_ptr->I, theModel_ptr->F, theModel_ptr->R,
                theModel_ptr->B, theModel_ptr->L, theModel_ptr->Qw,
                theModel_ptr->Q, theModel_ptr->X, theModel_ptr->V);

        //sumv=sumv+V.transpose()*R.inverse()*V; // un Use
        theModel_ptr->outputResult(initialData, xyzCoord, sum, neuText,
                                   epoch,       coordParameter);
    }
}


//int flaggps=0;
//int flagglonass=0;
//int flagbds=0;
//int flagGalileo=0;
//int flagmodel=0;
//int back=0;
//int flagUD_model=0;
//int flagUcom=0;
//int flagTF_IF=0;
//int flagTF_un=0;
//int flagSF=0;
//int flaguncom=0;
//if(system.indexOf("UD_model")>=0)
//{
//    flagUD_model=1;
//}
//if(system.indexOf("Uncombination")>=0)
//{
//    flagUcom=1;
//}
//if(system.indexOf("GPS")>=0)
//{
//    flaggps=1;
//}
//if(system.indexOf("GLONASS")>=0)
//{
//    flagglonass=1;
//}
//if(system.indexOf("BDS")>=0)
//{
//    flagbds=1;
//}
//if(system.indexOf("Galileo")>=0)
//{
//    flagGalileo=1;
//}
//if(type_model.indexOf("dynamic")>=0)
//{
//    flagmodel=1;
//}
//if(system.indexOf("back")>=0)
//{
//    back=1;
//}
//if(system.indexOf("TF_IF_model")>=0)
//{
//    flagTF_IF=1;
//}
//if(system.indexOf("TF_un_model")>=0)
//{
//    flagTF_un=1;
//}
//if(system.indexOf("SF_PPP")>=0)
//{
//    flagSF=1;
//}
//if(system.indexOf("Uncombination")>=0)
//{
//    flaguncom=1;
//}
//int ssum=flagbds+flagGalileo+flagglonass+flaggps;
//if(flagTF_IF==1)   // triple frequency IF PPP add IFB parameter
//{
//    ssum+=1;
//}

//QString sh="";

//if(flaggps==1)
//{
//    sh+="GPS";
//}
//if(flagbds==1)
//{
//    sh+=" BDS";
//}
//if(flagglonass==1)
//{
//    sh+=" GLONASS";
//}
//if(flagGalileo==1)
//{
//    sh+=" GALILEO";
//}

//MatrixXd Q,Qw,F,I;
//VectorXd x,V;
//VectorXd x_h;//


//QVector<int> sats1;
//QVector<QString>satstype1;
//QString pptah=gongshifile2[0].Folder_Path+gongshifile2[0].station+sh+".NEU";
//QString pptah1=gongshifile2[0].Folder_Path+gongshifile2[0].station+sh+".RMS";

//QString pptah2=gongshifile2[0].Folder_Path+gongshifile2[0].station+sh+"Residuals.trace";

//QString pptah3=gongshifile2[0].Folder_Path+gongshifile2[0].station+sh+"Matrix.traceM";

//QString ppath4=gongshifile2[0].Folder_Path+gongshifile2[0].station+sh+".ion";

//QFile openfile1(pptah1);                           //for calculating RMS
//if(!openfile1.open(QIODevice::WriteOnly))
//{
//    cout<<"can't write PPP file!"<<endl;
//}

//QFile openfile(pptah);
//if(!openfile.open(QIODevice::WriteOnly))
//{
//    cout<<"can't write PPP file!"<<endl;
//}

//QFile openfile2(pptah2);
//if(!openfile2.open(QIODevice::WriteOnly))
//{
//    cout<<"can't write PPP file!"<<endl;
//}

//QFile openfile3(pptah3);
//if(!openfile3.open(QIODevice::WriteOnly))
//{
//    cout<<"can't write PPP file!"<<endl;
//}

//QFile openfile4(ppath4);
//if(!openfile4.open(QIODevice::WriteOnly))
//{
//    cout<<"can't write ion file!"<<endl;
//}

//QTextStream sw2(&openfile2);
//sw2.setRealNumberNotation(QTextStream::FixedNotation);
//sw2.setFieldAlignment(QTextStream::AlignRight);
//sw2.setRealNumberPrecision(6);

//QTextStream sw3(&openfile3);
//sw3.setRealNumberNotation(QTextStream::FixedNotation);
//sw3.setFieldAlignment(QTextStream::AlignRight);
//sw3.setRealNumberPrecision(10);

//QTextStream sw1(&openfile1);
//sw1.setRealNumberNotation(QTextStream::FixedNotation);
//sw1.setFieldAlignment(QTextStream::AlignRight);
//sw1.setRealNumberPrecision(10);


//QTextStream sw(&openfile);
//sw.setRealNumberNotation(QTextStream::FixedNotation);
//sw.setFieldAlignment(QTextStream::AlignRight);
//sw.setRealNumberPrecision(6);


//QTextStream sw4(&openfile4);
//sw4.setRealNumberNotation(QTextStream::FixedNotation);
//sw4.setFieldAlignment(QTextStream::AlignRight);
//sw4.setRealNumberPrecision(10);

//sw<<qSetFieldWidth(24)<<sh;

//sw<<qSetFieldWidth(4)<<"N(m)"<<qSetFieldWidth(15)<<"E(m)"<<qSetFieldWidth(15)<<"U(m)"<<qSetFieldWidth(15)<<"sdn(m)"
// <<qSetFieldWidth(15)<<"sde(m)"<<qSetFieldWidth(15)<<"sdu(m)"<<qSetFieldWidth(15)<<"satnum"<<qSetFieldWidth(15)<<"PDOP"
//<<qSetFieldWidth(15)<<"B(°)"<<qSetFieldWidth(15)<<"L(°)"<<qSetFieldWidth(15)<<"H(m)"
//<<qSetFieldWidth(20)<<"X(m)"<<qSetFieldWidth(20)<<"Y(m)"<<qSetFieldWidth(20)<<"Z(m)"
//<<qSetFieldWidth(15)<<"        ZTD(m)"<<qSetFieldWidth(20)<<"clock(s)"<<endl;//<<qSetFieldWidth(15)<<"clock C-G(m)"<<qSetFieldWidth(15)<<"clock R-G(m)"<<qSetFieldWidth(15)<<"clock E-G(m)"<<endl;


//write_residuals_header(sw2);

//kalman_filter kf;

//double X1=0,Y1=0,Z1=0; int sum=0;double sumv=0;
//int mt=0;  //
//for(int i=0; i<gongshifile2[0].epoch.size(); i++)     //
//{
//    bool cflag=false;
//    bool flagq=false;
//    cout<<"filter epoch:"<<i+1<<endl;
//    QVector<int>slip_sats;
//    QVector<QString>slip_type;
//    if(i==73)
//    {
//        int afafa=1;
//    }
//    if(mt==0)
//    {
//        if(flagUD_model==1)
//        {
//            select_sats(gongshifile2,mt,5,system,K,sats1,satstype1,slip_sats,slip_type);
//            int sum=sats1.size();
//            UD_initKalman(Q,Qw,x,F,I,sum,system,type_model);
//        }
//        else if(flaguncom==1)
//        {
//            select_sats(gongshifile2,mt,5,system,K,sats1,satstype1,slip_sats,slip_type);
//            int sum=sats1.size();
//            Un_initKalman(Q,Qw,x,F,I,sum,system,type_model);
//        }
//        else if(flagTF_IF==1)
//        {
//            select_sats_TF(gongshifile2,mt,5,system,K,sats1,satstype1,slip_sats,slip_type);
//            int sum=sats1.size();
//            TF_UD_initKalman(Q,Qw,x,F,I,sum,system,type_model);
//        }
//        else if(flagTF_un==1)
//        {
//            select_sats_TF(gongshifile2,mt,5,system,K,sats1,satstype1,slip_sats,slip_type);
//            int sum=sats1.size();
//            TF_UC_initKalman(Q,Qw,x,F,I,sum,system,type_model);
//        }
//        mt++;
//    }
//    else
//    {
//        QVector<int> sats2;
//        QVector<QString>satstype2;
//        if( flagUD_model==1 || flaguncom==1)
//        {
//            select_sats(gongshifile2,i,5,system,K,sats2,satstype2,slip_sats,slip_type);
//        }
//        else if(flagTF_IF==1 || flagTF_un==1)
//        {
//            select_sats_TF(gongshifile2,i,5,system,K,sats2,satstype2,slip_sats,slip_type);
//        }

//        for(int k=0;k<sats1.size();k++)//Analyzing satellite is lost or cycle slip occurred, the event cycle slips or satellite lost re-set matrix
//        {
//            int pos2=-1;
//            int poss=-1;
//            for(int j=0;j<sats2.size();j++)
//            {
//                if((satstype1[k]==satstype2[j]) &&  (sats1[k]==sats2[j]))
//                {
//                    pos2=k;
//                    break;
//                }
//            }
//            if(pos2==-1)
//            {
//                for(int j=0;j<slip_sats.size();j++)
//                {
//                    if((sats1[k]==slip_sats[j]) &&  (satstype1[k]==slip_type[j]))
//                    {
//                        poss=k;
//                        break;
//                    }
//                }
//                if(poss==-1)
//                {
//                    sats1.remove(k);    // 旧星落下
//                    satstype1.remove(k);
//                    if(flagUD_model==1)
//                    {
//                        delete_Q(Q,4+ssum+k);
//                        delete_Qw(Qw);
//                        delete_x(x,4+ssum+k);
//                    }
//                    else if(flagTF_IF==1)
//                    {
//                        delete_Q_TF_IF(Q,4+ssum+k,ssum);
//                        delete_Qw_TF_IF(Qw);
//                        delete_X_TF_IF(x,4+ssum+k,ssum);
//                    }
//                    else if(flaguncom==1)
//                    {
//                        delete_Q_un(Q,4+ssum+k,ssum);
//                        delete_x_un(x,4+ssum+k,ssum);
//                    }
//                    else if(flagTF_un==1)
//                    {
//                        delete_Q_TF_UC(Q,4+ssum+k,ssum);
//                        delete_X_TF_UC(x,4+ssum+k,ssum);
//                    }

//                    k=k-1;
//                }
//                else
//                {
//                    if(flagUD_model==1)
//                    {
//                        ResetQ(Q,4+ssum+k,ssum);
//                        x[4+ssum+k]=0;
//                    }
//                    else if(flagTF_IF==1)
//                    {
//                        resetQ_TF_IF(Q,4+ssum+k,ssum);
//                        int row=(x.rows()-4-ssum)/2;
//                        x[4+ssum+k+row]=0;
//                        x[4+ssum+k]=0;
//                    }
//                    else if(flaguncom==1)
//                    {
//                        ResetQ_un(Q,4+ssum+k,ssum);
//                        int row=(x.rows()-4-ssum)/3;
//                        x[4+ssum+k+row]=0;
//                        x[4+ssum+k+2*row]=0;
//                    }
//                    else if(flagTF_un==1)
//                    {
//                        ResetQ_TF_UC(Q,4+ssum+k,ssum);
//                        int row=(x.rows()-4-ssum)/4;
//                        x[4+ssum+k+row]=0;
//                        x[4+ssum+k+2*row]=0;
//                        x[4+ssum+k+3*row]=0;
//                    }

//                    //                         ReSetN(gongshifile2,i,k,system,sats1,satstype1,x,K);
//                    flagq=true;
//                }
//            }
//        }

//        for(int k=0;k<sats2.size();k++)//New satellite rises
//        {
//            int pos2=-1;
//            for(int j=0;j<sats1.size();j++)
//            {
//                if((satstype1[j]==satstype2[k]) &&  (sats1[j]==sats2[k]))
//                {
//                    pos2=k;
//                    break;
//                }
//            }
//            if(pos2==-1)
//            {
//                sats1.push_back(sats2[k]);
//                satstype1.push_back(satstype2[k]);
//                if(flagUD_model==1)
//                {
//                    Add_Qx(Qw,1);
//                    Add_Matrixd(Q,1,ssum);
//                    Add_x(x);
//                }
//                else if(flagTF_IF==1)
//                {
//                    Add_Q_TF_IF(Q,1,ssum);
//                    Add_X_TF_IF(x,ssum);
//                    Add_Qx_TF_IF(Qw,1);
//                }
//                else if(flaguncom==1)
//                {
//                    Add_Q(Q,1,ssum);
//                    Add_x_un(x,ssum);
//                }
//                else if(flagTF_un==1)
//                {
//                    Add_Q_TF_UC(Q,1,ssum);
//                    Add_X_TF_UC(x,ssum);
//                }

//                //     ReSetN(gongshifile2,i,pk,system,sats1,satstype1,x,K);
//                flagq=true;
//            }
//        }
//        mt++;
//    }

//    I.resize(Q.rows(),Q.rows());
//    I.setIdentity(Q.rows(),Q.rows());
//    F.resize(Q.rows(),Q.rows());
//    F.setIdentity(Q.rows(),Q.rows());
//    //        F(4,4)=0;F(5,5)=0;
//    MatrixXd B,R;
//    VectorXd L; //////////////////////  前面旧星落下  现在新星升起
//    if(flagUD_model==1)
//    {
//        UD_Model(gongshifile2,i,system,sats1,satstype1,B,L,R,K,DCB_data,x);
//    }
//    else if(flagTF_IF==1)
//    {
//        TF_IF_model(gongshifile2,i,system,sats1,satstype1,B,L,R,K,DCB_data,x);
//    }
//    else if(flaguncom==1)
//    {
//        UnCombination(gongshifile2,i,system,sats1,satstype1,B,L,R,K,DCB_data,x);
//        change_Qw_un(gongshifile2,i,ssum,sats1,satstype1,Qw);
//        if(flagmodel==1)
//        {
//            for(int k1=0;k1<3;k1++)
//            {
//                Qw(k1,k1)=100;
//            }
//        }
//    }
//    else if(flagTF_un==1)
//    {
//        TF_UC_model(gongshifile2,i,system,sats1,satstype1,B,L,R,K,DCB_data,x);
//        change_Qw_UC(gongshifile2,i,ssum+1,sats1,satstype1,Qw);   // where 1 represent IFB parameter
//        if(flagmodel==1)
//        {
//            for(int k1=0;k1<3;k1++)
//            {
//                Qw(k1,k1)=100;
//            }
//        }
//    }
//    //       write_matrix(sw3,i+1,sats1,satstype1,B,Q,L,x);
//    //         cout<<B<<endl;
//    x_h=x;
//    if(flagq)
//    {
//        kf.kalman(I,F,Q,Qw,B,L,x,R,V);
//    }
//    kf.kalman(I,F,Q,Qw,B,L,x,R,V);
//    //        write_matrix(sw3,i+1,sats1,satstype1,B,Q,L,x);
//    //        bool vf=ReSetP(V,Q,ssum,x_h);
//    //        if(vf)
//    //        {
//    //            x=x_h;
//    ////            for (int mn=0;mn<3;mn++)
//    ////            {
//    ////                x(mn)=x_h(mn);
//    ////            }
//    //            kf.kalman(I,F,Q,Qw,B,L,x,R,V);
//    //        }
//    //      write_residuals(sw2,gongshifile2[0].epoch[i].gpst,i+1,sats1,satstype1,V,flagUD_model,x);

//    sumv=sumv+V.transpose()*R.inverse()*V;
//    Vector3d x1;
//    for(int n=0;n<3;n++)
//    {
//        x1(n)=x(n);
//    }


//    if (fabs(x1(0))<0.25 && fabs(x1(1))<0.25)
//    {
//        cflag=true;
//    }

//    MatrixXd B1(B.rows(),3);
//    for(int m=0;m<B.rows();m++)
//    {
//        for(int n=0;n<3;n++)
//        {
//            B1(m,n)=B(m,n);
//        }
//    }
//    MatrixXd Q1=(B1.transpose()*R*B1).inverse();
//    if(flagUD_model)
//    {
//        if(B.rows()<8)
//        {
//            for(int m=0;m<3;m++)
//            {
//                Q1(m,m)=2;
//            }
//        }
//    }
//    else
//    {
//        if(B.rows()<16)
//        {
//            for(int m=0;m<3;m++)
//            {
//                Q1(m,m)=2;
//            }
//        }
//    }
//    CS cs;
//    double b1=0,l1=0,h1=0;
//    Vector3d xyz=gongshifile2[0].MASTER_APPROX_POS+x1;

//    // kinematic  model
//    if(flagmodel==1 && back!=1)
//    {
//        gongshifile2[0].MASTER_APPROX_POS+=x1;
//        GetCoordSys(1984,cs);
//        XYZ_BLH(xyz,cs,b1,l1,h1);   //change to BLH

//        Matrix3d neu= Get_Mat_XYZ_NEU(gongshifile2[0].MASTER_APPROX_POS);
//        Vector3d NEU=neu*x1;
//        cout<<neu<<endl;
//        sw<<qSetFieldWidth(15)<<gongshifile2[0].epoch[i].gpst<<qSetFieldWidth(15)<<NEU(0)<<qSetFieldWidth(15)<<NEU(1)<<qSetFieldWidth(15)<<NEU(2)
//            //                 sw<<qSetFieldWidth(15)<<gongshifile2[0].epoch[i].mjulday<<qSetFieldWidth(15)<<NEU(0)<<qSetFieldWidth(15)<<NEU(1)<<qSetFieldWidth(15)<<NEU(2)
//         <<qSetFieldWidth(15)<<sqrt(fabs(Q(0,0)))<<qSetFieldWidth(15)<<sqrt(fabs(Q(1,1)))<<qSetFieldWidth(15)<<sqrt(fabs(Q(2,2)))<<qSetFieldWidth(15)<<sats1.size()<<qSetFieldWidth(15)<<Q1(0,0)+Q1(1,1)+Q1(2,2)
//        <<qSetFieldWidth(15)<<b1*180/pi<<qSetFieldWidth(15)<<l1*180/pi<<qSetFieldWidth(15)<<h1<<qSetFieldWidth(15)<<xyz[0]<<qSetFieldWidth(15)<<xyz[1]<<qSetFieldWidth(15)<<xyz[2]
//        <<gongshifile2[0].epoch[i].ztd+x[3]<<qSetFieldWidth(15)<<x[4]<<endl;
//    }

//    //static  model
//    if(flagmodel!=1 && back!=1)
//    {
//        GetCoordSys(1984,cs);
//        XYZ_BLH(xyz,cs,b1,l1,h1);   //change to BLH

//        Matrix3d neu= Get_Mat_XYZ_NEU(gongshifile2[0].MASTER_APPROX_POS);
//        Vector3d NEU=neu*x1;
//        if (cflag)
//        {
//            X1=X1+NEU(0)*NEU(0);
//            Y1=Y1+NEU(1)*NEU(1);
//            Z1=Z1+NEU(2)*NEU(2);
//            sum++;
//        }
//        cout<<x1<<endl;
//        int wd=gongshifile2[0].epoch[i].week-gongshifile2[0].epoch[0].week;

//        if(flaguncom==1)
//        {
//            sw<<qSetFieldWidth(20)<<gongshifile2[0].epoch[i].gpst+wd*604800<<qSetFieldWidth(15)<<NEU(0)<<qSetFieldWidth(15)<<NEU(1)<<qSetFieldWidth(15)<<NEU(2)
//                // sw<<qSetFieldWidth(20)<<gongshifile2[0].epoch[i].mjulday<<qSetFieldWidth(15)<<NEU(0)<<qSetFieldWidth(15)<<NEU(1)<<qSetFieldWidth(15)<<NEU(2)
//             <<qSetFieldWidth(15)<<sqrt(fabs(Q(0,0)))<<qSetFieldWidth(15)<<sqrt(fabs(Q(1,1)))<<qSetFieldWidth(15)<<sqrt(fabs(Q(2,2)))<<qSetFieldWidth(15)<<sats1.size()<<qSetFieldWidth(15)<<Q1(0,0)+Q1(1,1)+Q1(2,2)
//            <<qSetFieldWidth(15)<<b1*180/pi<<qSetFieldWidth(15)<<l1*180/pi<<qSetFieldWidth(15)<<h1<<qSetFieldWidth(20)<<xyz[0]<<qSetFieldWidth(20)<<xyz[1]<<qSetFieldWidth(20)<<xyz[2]
//            <<gongshifile2[0].epoch[i].ztd+x[3];
//            sw.setRealNumberPrecision(15);
//            sw<<qSetFieldWidth(30)<<x[4]/v_light<<endl;
//            sw.setRealNumberPrecision(6);

//            write_ion(sw4,sats1,satstype1,x,ssum);

//        }
//        else    //UD_model
//        {
//            //       sw<<qSetFieldWidth(20)<<gongshifile2[0].epoch[i].gpst+wd*604800<<qSetFieldWidth(15)<<NEU(0)<<qSetFieldWidth(15)<<NEU(1)<<qSetFieldWidth(15)<<NEU(2)
//            sw<<qSetFieldWidth(20)<<gongshifile2[0].epoch[i].mjulday<<qSetFieldWidth(15)<<NEU(0)<<qSetFieldWidth(15)<<NEU(1)<<qSetFieldWidth(15)<<NEU(2)
//             <<qSetFieldWidth(15)<<sqrt(fabs(Q(0,0)))<<qSetFieldWidth(15)<<sqrt(fabs(Q(1,1)))<<qSetFieldWidth(15)<<sqrt(fabs(Q(2,2)))<<qSetFieldWidth(15)<<sats1.size()<<qSetFieldWidth(15)<<Q1(0,0)+Q1(1,1)+Q1(2,2)
//            <<qSetFieldWidth(15)<<b1*180/pi<<qSetFieldWidth(15)<<l1*180/pi<<qSetFieldWidth(15)<<h1<<qSetFieldWidth(20)<<xyz[0]<<qSetFieldWidth(20)<<xyz[1]<<qSetFieldWidth(20)<<xyz[2]
//            <<gongshifile2[0].epoch[i].ztd+x[3];
//            sw.setRealNumberPrecision(15);
//            sw<<qSetFieldWidth(30)<<x[4]/v_light<<endl;
//            sw.setRealNumberPrecision(6);

//        }

//    }
//}
//sumv=sumv/gongshifile2[0].epoch.size();
//sw1<<qSetFieldWidth(20)<<sqrt(X1/sum)<<qSetFieldWidth(20)<<sqrt(Y1/sum)<<qSetFieldWidth(20)<<sqrt(Z1/sum)<<endl;
//openfile1.close();


////   back
//if (back==1)
//{
//    for(int i=int(gongshifile2[0].epoch.size()-1);i>0;i--)
//    {
//        bool flagq=false;
//        cout<<"filter epoch:"<<i+1<<endl;
//        QVector<int>slip_sats;
//        QVector<QString>slip_type;

//        QVector<int> sats2;
//        QVector<QString>satstype2;
//        select_sats(gongshifile2,i,10,system,K,sats2,satstype2,slip_sats,slip_type);
//        for(int k=0;k<sats1.size();k++)//Analyzing satellite is lost or cycle slip occurred, the event cycle slips or satellite lost re-set matrix
//        {
//            int pos2=-1;
//            int poss=-1;
//            for(int j=0;j<sats2.size();j++)
//            {
//                if((satstype1[k]==satstype2[j]) &&  (sats1[k]==sats2[j]))
//                {
//                    pos2=k;
//                    break;
//                }
//            }
//            if(pos2==-1)
//            {
//                for(int j=0;j<slip_sats.size();j++)
//                {
//                    if((sats1[k]==slip_sats[j]) &&  (satstype1[k]==slip_type[j]))
//                    {
//                        poss=k;
//                        break;
//                    }
//                }
//                if(poss==-1)
//                {
//                    sats1.remove(k);
//                    satstype1.remove(k);
//                    delete_Q(Q,4+ssum+k);
//                    delete_Qw(Qw);
//                    F.resize(Q.rows(),Q.rows());
//                    F.setIdentity(Q.rows(),Q.rows());
//                    //                        for (int m=4;m<4+ssum;m++)
//                    //                        {
//                    //                            F(m,m)=0;
//                    //                        }
//                    I.resize(Q.rows(),Q.rows());
//                    I.setIdentity(Q.rows(),Q.rows());
//                    delete_x(x,4+ssum+k);
//                    k=k-1;
//                }
//                else
//                {
//                    ResetQ(Q,4+ssum+k,ssum);
//                    x[4+ssum+k]=0;
//                    //                        ReSetN(gongshifile2,i,k,system,sats1,satstype1,x,K);
//                    flagq=true;
//                }
//            }
//        }

//        for(int k=0;k<sats2.size();k++)//New satellite rises
//        {
//            int pos2=-1;
//            for(int j=0;j<sats1.size();j++)
//            {
//                if((satstype1[j]==satstype2[k]) &&  (sats1[j]==sats2[k]))
//                {
//                    pos2=k;
//                    break;
//                }
//            }
//            if(pos2==-1)
//            {
//                sats1.push_back(sats2[k]);
//                satstype1.push_back(satstype2[k]);
//                Add_Qx(Qw,1);
//                Add_Matrixd(Q,1,ssum);
//                Add_x(x);
//                //                     ReSetN(gongshifile2,i,pk,system,sats1,satstype1,x,K);
//                flagq=true;
//                F.resize(Q.rows(),Q.rows());
//                F.setIdentity(Q.rows(),Q.rows());
//                I.resize(Q.rows(),Q.rows());
//                I.setIdentity(Q.rows(),Q.rows());
//            }
//        }

//        MatrixXd B,R;
//        VectorXd L;
//        //        if((2*sats1.size())>=(4+ssum+sats1.size()))
//        //        {
//        UD_Model(gongshifile2,i,system,sats1,satstype1,B,L,R,K,DCB_data,x);
//        kalman_filter kf;
//        if(flagq)
//        {
//            kf.kalman(I,F,Q,Qw,B,L,x,R,V);
//        }
//        kf.kalman(I,F,Q,Qw,B,L,x,R,V);
//        write_residuals(sw2,gongshifile2[0].epoch[i].gpst,i+1,sats1,satstype1,V,flagUD_model,x);
//        Vector3d x1;
//        for(int n=0;n<3;n++)
//        {
//            x1(n)=x(n);
//        }

//        MatrixXd B1(B.rows(),3);
//        for(int m=0;m<B.rows();m++)
//        {
//            for(int n=0;n<3;n++)
//            {
//                B1(m,n)=B(m,n);
//            }
//        }
//        MatrixXd Q1=(B1.transpose()*R*B1).inverse();

//        if(B.rows()<8)
//        {
//            for(int m=0;m<3;m++)
//            {
//                Q1(m,m)=2;
//            }
//        }
//        CS cs;
//        double b1=0,l1=0,h1=0;
//        Vector3d xyz=gongshifile2[0].MASTER_APPROX_POS+x1;
//        if(flagmodel==1)
//        {
//            gongshifile2[0].MASTER_APPROX_POS+=x1;
//        }
//        GetCoordSys(1984,cs);
//        XYZ_BLH(xyz,cs,b1,l1,h1);   //change to BLH

//        Matrix3d neu= Get_Mat_XYZ_NEU(gongshifile2[0].MASTER_APPROX_POS);
//        Vector3d NEU=neu*x1;
//        cout<<NEU<<endl;
//        sw<<qSetFieldWidth(15)<<gongshifile2[0].epoch[i].gpst<<qSetFieldWidth(15)<<NEU(0)<<qSetFieldWidth(15)<<NEU(1)<<qSetFieldWidth(15)<<NEU(2)
//         <<qSetFieldWidth(15)<<sqrt(fabs(Q(0,0)))<<qSetFieldWidth(15)<<sqrt(fabs(Q(1,1)))<<qSetFieldWidth(15)<<sqrt(fabs(Q(2,2)))<<qSetFieldWidth(15)<<sats1.size()<<qSetFieldWidth(15)<<Q1(0,0)+Q1(1,1)+Q1(2,2)
//        <<qSetFieldWidth(15)<<b1*180/pi<<qSetFieldWidth(15)<<l1*180/pi<<qSetFieldWidth(15)<<h1<<qSetFieldWidth(15)<<xyz[0]<<qSetFieldWidth(15)<<xyz[1]<<qSetFieldWidth(15)<<xyz[2]
//        <<gongshifile2[0].epoch[i].ztd+x[3]<<qSetFieldWidth(15)<<x[4]<<endl;
//    }
//}
//openfile2.close();
//openfile.close();
//openfile3.close();
