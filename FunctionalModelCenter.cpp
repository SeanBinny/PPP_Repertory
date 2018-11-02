#include "FunctionalModelCenter.h"
#include "DataProcessCenter.h"
#include "PublicDataCenter.h"
#include "ResultDataCenter.h"


/*                    ****************************************************                                       */
/*******************************            UD Model            **************************************************/
/*                    ****************************************************                                       */
/*----------------------------------------------------------------------------------
 * Name     : readFile
 * Function : read igs station coordinate file (*.coord)
 * Input    : const QString &filePath
 * Output   : bool (if read success)
 *---------------------------------------------------------------------------------*/
void UD_Model::initKalman(int satNum/*, int sumOfSys*/)
{
    /*-- dx,dy,dz dt + + and Ambiguity ------*/
    int dim        = 4 + sumOfSys + satNum;                                         // Dimension of matrix
    Q. resize(dim, dim); Q. setZero(dim, dim);
    Qw.resize(dim, dim); Qw.setZero(dim, dim);
    X. resize(dim, 1  ); X. setZero(dim,   1);
    set_I_F();

    for (int i = 0; i < dim; i++)                                                   // Initialize Q and Qw
    {
        if (i < 3){
            Q(i, i)  = 100000; /*unknown*/
            if (ModeFlag::dynamic)    /* Q的设置依据*/
                Qw(i, i) = 100;
            if ("static")
                Qw(i, i) = 0;
        }else if (i  < 4){
            Q (i, i) = 0.5;
            Qw(i, i) = 3*1E-8;
        }else if (i  < 4 + sumOfSys){
            Q (i, i) = 10E4;
            Qw(i, i) = 1E+6;
        }else{
            Q (i, i) = 10E5;
            Qw(i, i) = 0;
        }
    }
}
/*----------------------------------------------------------------------------------
 * Name     : modelSolution
 * Function : Set B , L , R and carry on single epoch ambiguity resolution
 * Input    : const QString &filePath
 * Output   : bool (if read success)
 *---------------------------------------------------------------------------------*/
void UD_Model::modelSolution (FinalDataFile    &pppFile,
                              vector <SatQuery> satContaine,
                              int               epoch)
{
    int satNum = satContaine.size();

    /*--------------------- Initialize related matrix -----------------------------*/
    B.resize(2*satNum,  4+sumOfSys+satNum); B. setZero(2*satNum, 4+sumOfSys+satNum);// 2 times is to save pseudo distance and carrier respectively.
    L.resize(2*satNum)                    ; L. setZero(2*satNum);
    R.resize(2*satNum,  2*satNum)         ; R. setZero(2*satNum, 2*satNum);
    MatrixXd B1(satNum, satNum)           ; B1.setZero(  satNum,   satNum);         // For single epoch ambiguity resolution
    VectorXd l1(satNum)                   ; l1.setZero(  satNum);
    MatrixXd B2(satNum, 4+sumOfSys+satNum); B2.setZero(  satNum, 4+sumOfSys);
    VectorXd l2(satNum)                   ; l2.setZero(  satNum);


    for (unsigned int i = 0; i < satContaine.size(); i++)
    {
        int index = -1;
        if (satContaine[i].index <  pppFile.allSatelliteData[epoch].eSatData.size()&&
            satContaine[i].satTN == pppFile.allSatelliteData[epoch].eSatData[satContaine[i].index].obsData.satTN)
            index = satContaine[i].index;
        else
        {
            for (unsigned int j = 0; j < pppFile.allSatelliteData[epoch].eSatData.size(); j++)
            {
                if (satContaine[i].satTN  == pppFile.allSatelliteData[epoch].
                                             eSatData[j].obsData.satTN){
                    index = j;
                    break;}
            }
        }
        if (index != -1)                                                            // If find the corresponding elements
        {
            SingleSatelliteData &satData = pppFile.allSatelliteData[epoch].eSatData[index];
            Vector3d ri (satData.satXYZ[0] - pppFile.MASTER_APPROX_POS[0],
                         satData.satXYZ[1] - pppFile.MASTER_APPROX_POS[1],
                         satData.satXYZ[2] - pppFile.MASTER_APPROX_POS[2]);
            double   Ri = MyFunctionCenter::getCoordDistance(ri);
             /*---------------------- set matrix B x,y,z --------------------------*/
            for(int j = 0; j < 3; j++)
            {
                B(i,        j) = ri[j] / Ri;
                B(i+satNum, j) = ri[j] / Ri;
            }

            double angleEle =  PI / 2 - satData.DAZ[2];
            if (angleEle > 10E-6)
            {
                /*---------------------- set matrix R ----------------------------*/
                if (satData.obsData.satTN.Type == "G"){
                        R(i       , i       ) =  0.006*0.006/(pow(sin(angleEle),2));
                        R(i+satNum, i+satNum) =    0.3*0.3  /(pow(sin(angleEle),2));
                }else if (satData.obsData.satTN.Type == "R"){
                        R(i       , i       ) =   0.01*0.01 /(pow(sin(angleEle),2));
                        R(i+satNum, i+satNum) =    0.5*0.5  /(pow(sin(angleEle),2));

                }else if (satData.obsData.satTN.Type == "C"){
                    if (satData.obsData.satTN.PRN <= 5){                            // GEO Type
                        R(i       , i       ) =  0.36*0.36  /(pow(sin(angleEle),2));
                        R(i+satNum, i+satNum) =    18*18    /(pow(sin(angleEle),2));
                    }else{
                        R(i       , i       ) =  0.06*0.006 /(pow(sin(angleEle),2));
                        R(i+satNum, i+satNum) =    0.3*0.3  /(pow(sin(angleEle),2));
                    }
                }else if (satData.obsData.satTN.Type == "E"){
                        R(i       , i       ) =    3.6*0.006/(pow(sin(angleEle),2));
                        R(i+satNum, i+satNum) =    180*0.3  /(pow(sin(angleEle),2));
                }
                /*---------------------- set matrix B system clock-----------------*/
                for (int s = 0; s < sumOfSys; s++){
                    B(i       , 4+s) = -1;
                    B(i+satNum, 4+s) = -1;
                }
            }


            vector<double> freq = satData.freq;
            double   C1 = 0, P2 = 0, L1 = 0,L2 = 0;
            double   f1 = 0, f2 = 0;

            double  baseC1 = satData.obsData.C1;
            double  baseP2 = satData.obsData.P2;
            double  baseP3 = satData.obsData.P3;


//            /*------------- Get DCB correction ----------------------------------*/
//            double  val[3] =  {0};
//            /*DCB correction*/
//            m_time t;
//            t.julday=gongshifile2[0].epoch[pos].julday;
//            check_time(t);
//            get_DCB(gongshifile2,DCB_data,t,gongshifile2[0].epoch[pos].Satellite[i].Sat_Type,
//                    gongshifile2[0].epoch[pos].Satellite[i].PRN,val);
//            /*val[0]=C1C C2W   val[1]=D13  val[2]=C1C C1W*/
//            C11-=line2*val[0];
//            P21+=line1*val[0];
//            P31-=line2*val[0]-val[1];

            /*------------- Get TGD correction -----------------------------------------------*/
//            Tgd：the time group delay
//            卫星内部 L1、L2信号从产生到发射的时延之差。当使用单频接收机时，用Tgd改正所观测的结果，以减小电离层效应影响提高定位精度；当采用双频接收机时，就不必要采用这个时延差改正。该值在定位计算中，最终应用在卫星钟差计算中。
            /*TGD correction*/
//            double gma=pow(f[0],2)/pow(f[1],2);
//            double bta=pow(f[0],2)/pow(f[2],2);
//            double afa=pow(f[1],2)/pow(f[2],2);
//            if(gongshifile2[0].epoch[pos].Satellite[i].Sat_Type=="G")
//            {
//                C11-=gongshifile2[0].GPS_TGD[gongshifile2[0].epoch[pos].Satellite[i].PRN-1];
//                P21-=gongshifile2[0].GPS_TGD[gongshifile2[0].epoch[pos].Satellite[i].PRN-1]*pow(f[0],2)/pow(f[1],2);

//                C1=C11-val[2];  //for C1-P1  DCB correction
//            }
//            else if(gongshifile2[0].epoch[pos].Satellite[i].Sat_Type=="C")
//            {
//                double tgd1=gongshifile2[0].BDS_TGD[gongshifile2[0].epoch[pos].Satellite[i].PRN-1][0];
//                double tgd2=gongshifile2[0].BDS_TGD[gongshifile2[0].epoch[pos].Satellite[i].PRN-1][1];
//                C11-=(tgd1-tgd2)*line2;
//                P21+=(tgd1-tgd2)*line1;
//                P31+=(line1*tgd1+line2*tgd2);
//            }
//            else if(gongshifile2[0].epoch[pos].Satellite[i].Sat_Type=="E")
//            {

//                double tgd1=gongshifile2[0].Galielo_TGD[gongshifile2[0].epoch[pos].Satellite[i].PRN-1][0];
//                double tgd2=gongshifile2[0].Galielo_TGD[gongshifile2[0].epoch[pos].Satellite[i].PRN-1][1];
//                C11+=tgd1*afa;
//                P21+=tgd1*afa*afa;
//                P31+=(afa*tgd1-(bta-1)*tgd2);
//            }

            /*-----------------Get ranging code and carrier wave ------------------*/
            if      (ModeFlag::P1_P2)
            {
                C1 = baseC1;
                P2 = baseP2;
                L1 = satData.obsData.L1;
                L2 = satData.obsData.L2;
                f1 = freq[0];
                f2 = freq[1];
            }
            else if (ModeFlag::P1_P3)
            {
                C1 = baseC1;
                P2 = baseP3;
                L1 = satData.obsData.L1;
                L2 = satData.obsData.L3;
                f1 = freq[0];
                f2 = freq[2];
            }
            else if (ModeFlag::P2_P3)
            {
                C1 = baseP2;
                P2 = baseP3;
                L1 = satData.obsData.L2;
                L2 = satData.obsData.L3;
                f1 = freq[1];
                f2 = freq[2];
            }


            double lambda1 =   LIGHT_V /  f1;
            double lambda2 =   LIGHT_V /  f2;
            double m       =  (f1*f1)  / (f1*f1 - f2*f2);
            double n       = -(f2*f2)  / (f1*f1 - f2*f2);

//            double Prange1 =   C1 + satData.anteCenterOffset[0];                    // Correction of antenna phase center for pseudorange
//            double Prange2 =   P2 + satData.anteCenterOffset[1];
//            double Phase1  =  (L1 + satData.anteCenterOffset[0]/lambda1 -           // Antenna phase center correction and phase winding correction for carrier phase.
//                                    satData.phaseWinding)*lambda1;
//            double Phase2  =  (L2 + satData.anteCenterOffset[1]/lambda2 -
//                                    satData.phaseWinding)*lambda2;
            double Prange1 =   C1 + (LIGHT_V / freq[0])*satData.anteCenterOffset[0];                    // Correction of antenna phase center for pseudorange
            double Prange2 =   P2 + (LIGHT_V / freq[1])*satData.anteCenterOffset[1];
            double Phase1  =  (L1 + satData.anteCenterOffset[0]*(LIGHT_V / freq[0])/lambda1 -           // Antenna phase center correction and phase winding correction for carrier phase.
                                    satData.phaseWinding)*lambda1;
            double Phase2  =  (L2 + satData.anteCenterOffset[1]*(LIGHT_V / freq[1])/lambda2 -
                                    satData.phaseWinding)*lambda2;



            double iMapTerm     = - satData.tropWetDelayFunc;                       // The i-th tropospheric wet delay mapping function
            double iErrorTerm   =   satData.Delta0;                                 // The i-th multiple error correction
            double iPhase       =   m*Phase1  + n*Phase2;                           // The i-th pseudo range linear combination
            double iPrange      =   m*Prange1 + n*Prange2;                          // The i-th carrier linear combination
            double lambda       =   m*lambda1 + n*lambda2;

            /*---------------------- set matrix L ---------------------------------*/
            L[i         ]       =   Ri - iPhase  - iErrorTerm;
            L[i + satNum]       =   Ri - iPrange - iErrorTerm;
            /*------------------ set matrix B vague and map------------------------*/
            B(i , 4+sumOfSys+i) =   lambda;                                         // Order: x y z map clock
            B(i         ,    3) =   iMapTerm;
            B(i + satNum,    3) =   iMapTerm;

            if(epoch == 0)                                                          //first epoch
            {
                l1[i]    = L[i] - L[i + satNum];                                    // The rest of the L array by using elimination method
                l2[i]    = L[i  + satNum];/*if use L[i]?*/
                B1(i, i) = lambda;                                                  // B1 is full of lambda
            }
        }
    }
    if (epoch == 0) /*------------ Single epoch ambiguity resolution --------------*/
    {
        /*------------------------  Get vague -------------------------------------*/
        VectorXd x = (B1.transpose()*B1).inverse()*(B1.transpose()*l1);
        for (int s = 0; s < satNum; s++)
        {
             X[4+sumOfSys + s] = x[s];
             for(int j = 0; j < 4+sumOfSys; j++)
                 B2(s, j) = B(s, j);
        }
        /*------------------------  Get x y z map clock sys -----------------------*/
        int  t1 = 0, t2 = 0, t3 = 0, t4 = 0;
        for (unsigned int s = 0; s < pppFile.allSatelliteData[0].eSatData.size(); s++)
        {
            QString type =   pppFile.allSatelliteData[0].eSatData[s].obsData.satTN.Type;
            if     (type == "G")
                t1 = 1;
            else if(type == "R")
                t2 = 1;
            else if(type == "C")
                t3 = 1;
            else if(type == "E")
                t4 = 1;
        }
        int typeNum = t1 + t2 + t3 +t4;
        if (typeNum == sumOfSys)/*unknown*/
        {
            x = (B2.transpose()*B2).lu().solve(B2.transpose()*l2); /*unknown*/
            for(int  s = 0; s < 4+sumOfSys; s++)
                X[s]   = x[s];
            X[3] = 0;                                                               // Trop map is set 0 at the beginning
        }
    }
}

/*----------------------------------------------------------------------------------
 * Name     : outputResult
 * Function : Output information of result
 * Input    : FinalDataFile            &pppFile,
 *            X_Y_Z                    &xyzCoord,
 *            int                      &sum,
 *            const int                &epoch,
 *            const CoordSysParameter  &coordParameter
 * Output   : QTextStream              &neuText,
 *---------------------------------------------------------------------------------*/
void UD_Model::outputResult(FinalDataFile        &pppFile,
                            X_Y_Z                &xyzCoord, /*unknown*/
                            int                  &sum,/*unknown*/
                            QTextStream          &neuText,
                      const int                  &epoch,
                      const CoordSysParameter    &coordParameter)
{
    Vector3d dxyz;
    for (int n  = 0; n < 3; n++)
         dxyz(n) = X(n);

    MatrixXd B1(B.rows(), 3);
    for (int m = 0; m < B.rows(); m++)  /* unUse*/
    {
        for (int n  = 0; n < 3;n++)
            B1(m,n) = B(m,n);
    }
    MatrixXd Q1 = (B1.transpose()*R*B1).inverse();

    if(B.rows()<8)        // Unknown
    {
        for(int  m  = 0; m < 3; m++)
            Q1(m,m) = 2;  // Unknown
    }
    /*------------------------  Set file head -------------------------------------*/
    if (epoch == 0){
        neuText << qSetFieldWidth(15) << "MJD"
                << qSetFieldWidth(15) << "N(m)"
                << qSetFieldWidth(15) << "E(m)"
                << qSetFieldWidth(15) << "U(m)"
                << qSetFieldWidth(15) << "sdn(m)"
                << qSetFieldWidth(15) << "sde(m)"
                << qSetFieldWidth(15) << "sdu(m)"
                << qSetFieldWidth(15) << "satnum"
                << qSetFieldWidth(15) << "PDOP"
                << qSetFieldWidth(15) << "B(°)"
                << qSetFieldWidth(15) << "L(°)"
                << qSetFieldWidth(15) << "H(m)"
                << qSetFieldWidth(15) << "X(m)"
                << qSetFieldWidth(15) << "Y(m)"
                << qSetFieldWidth(15) << "Z(m)"
                << qSetFieldWidth(15) << "ZTD(m)"
                << qSetFieldWidth(15) << "clock(s)" <<endl;
        neuText.setRealNumberNotation(QTextStream::FixedNotation);
    }

    Vector3d recXYZ = pppFile.MASTER_APPROX_POS + dxyz;
    Matrix3d ToENU  = MyFunctionCenter::getMatrixToENU(coordParameter, recXYZ);
    Vector3d ENU    = ToENU * dxyz;

    if (( ModeFlag::dynamic && !ModeFlag::back) ||                              // 看看是否可以去掉
        (!ModeFlag::dynamic && !ModeFlag::back))
    {
    neuText << qSetFieldWidth(15) << pppFile.allSatelliteData[epoch].myTime.MJD // 看看改不改成mjd
            << qSetFieldWidth(15) << ENU(1)
            << qSetFieldWidth(15) << ENU(0)
            << qSetFieldWidth(15) << ENU(2)
            << qSetFieldWidth(15) << sqrt(fabs(Q(0,0)))
            << qSetFieldWidth(15) << sqrt(fabs(Q(1,1)))
            << qSetFieldWidth(15) << sqrt(fabs(Q(2,2)))
            << qSetFieldWidth(15) << B.rows() / 2                               // Represent the number of satllites
            << qSetFieldWidth(15) << Q1(0,0) + Q1(1,1) + Q1(2,2)
            << qSetFieldWidth(15) << ""/*b1*180/pi*/
            << qSetFieldWidth(15) << ""/*l1*180/pi*/
            << qSetFieldWidth(15) << ""/*h1*/
            << qSetFieldWidth(15) << recXYZ[0]                                  // X
            << qSetFieldWidth(15) << recXYZ[1]                                  // Y
            << qSetFieldWidth(15) << recXYZ[2]                                  // Z
            << qSetFieldWidth(15) << pppFile.allSatelliteData[epoch].ztd + X[3] // Trop map
            << qSetFieldWidth(15) << X[4] / LIGHT_V << endl;


     qDebug() << "The fileter " << epoch << " ....";
    }

    if (!ModeFlag::dynamic && !ModeFlag::back)                                  // Static  model
    {
        if (fabs(dxyz(0)) < 0.25 && fabs(dxyz(1)) < 0.25)
        {
            xyzCoord.X += ENU(0)*ENU(0);
            xyzCoord.Y += ENU(1)*ENU(1);
            xyzCoord.Z += ENU(2)*ENU(2);
            sum++;
        }
    }
}

/*----------------------------------------------------------------------------------
 * Name     : add_Q
 * Function : Add a row for matrix Q
 * Input    : NULL
 * Output   : NULL
 *---------------------------------------------------------------------------------*/
void UD_Model::add_Q ()
{
    int  row  = Q.rows();
    int  col  = Q.cols();
    MatrixXd  q(row+1, col+1);
    q.setZero  (row+1, col+1);
    for(int i = 0; i < row;  i++)
    {
        for(int j = 0; j < col; j++)
            q(i,j)= Q (i,j);
    }
    for(int i = row; i < row+1; i++)
    {
        q(i,i)= 1E20;
        for(int j = 0; j < 4+sumOfSys; j++) /*unknown */
        {
            q(i,j)= 1;
            q(j,i)= 1;
        }
    }
    Q = q;
}
/*----------------------------------------------------------------------------------
 * Name     : cut_Q
 * Function : Cut off a row for matrix Q
 * Input    : int pos .(position of row)
 * Output   : NULL
 *---------------------------------------------------------------------------------*/
void UD_Model::cut_Q (int pos)
{
    int  row = Q.rows();
         row = row -  1;
    MatrixXd q(row, row);
    q.setZero (row, row);
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < row; j++)
        {
            if (i < pos){
                if (j <  pos)
                    q(i, j) = Q(i,   j  );
                else
                    q(i, j) = Q(i,   j+1);
            }else{
                if (j <  pos)
                    q(i, j) = Q(i+1, j  );
                else
                    q(i, j) = Q(i+1, j+1);
            }
        }
    }
    Q = q;
}
/*----------------------------------------------------------------------------------
 * Name     : add_X
 * Function : add row for matrix X
 * Input    : NULL
 * Output   : NULL
 *---------------------------------------------------------------------------------*/
void UD_Model::add_X ()
{
    int  num = X.rows();
    VectorXd x(num + 1);
    x.setZero (num + 1);
    for (int i = 0; i < num; i++)
        x[i] = X[i];
    X = x;
}
/*----------------------------------------------------------------------------------
 * Name     : cut_X
 * Function : Cut off a row for matrix X
 * Input    : int pos .(position of row)
 * Output   : NULL
 *---------------------------------------------------------------------------------*/
void UD_Model::cut_X (int pos)
{
    int  num = X.rows();
    VectorXd x(num - 1);
    for (int i = 0; i < num-1; i++)
    {
        if (i <  pos)
            x[i] = X[i];
        else
            x[i] = X[i+1];
    }
    X = x;
}
/*----------------------------------------------------------------------------------
 * Name     : add_Qw
 * Function : add row for matrix Qw
 * Input    : NULL
 * Output   : NULL
 *---------------------------------------------------------------------------------*/
void UD_Model::add_Qw()
{

    int  row = Qw.rows();
    int  col = Qw.cols();
    MatrixXd qw(row+1, col+1);
    qw.setZero (row+1, col+1);
    for (int i = 0; i < row;i++)
    {
        for (int j = 0; j < row; j++)
             qw(i,j)= Qw(i,j);
    }
    Qw = qw;
}
/*----------------------------------------------------------------------------------
 * Name     : cut_Qw
 * Function : Cut off a row for matrix Qw
 * Input    : int pos .(position of row)
 * Output   : NULL
 *---------------------------------------------------------------------------------*/
void UD_Model::cut_Qw()
{
    int  row  = Qw.rows();
    MatrixXd qw(row-1, row-1);
    qw.setZero (row-1, row-1);
    for(int i = 0; i < row-1 ; i++)
    {
        qw(i,i) = Qw(i,i);
    }
    Qw = qw;
}
/*----------------------------------------------------------------------------------
 * Name     : resetQ
 * Function : Reset a row for matrix Q
 * Input    : int pos .(position of row)
 * Output   : NULL
 *---------------------------------------------------------------------------------*/
void UD_Model::resetQ(int pos)
{
    Q(pos, pos) = 1E20;
    int  row    = Q.rows();
    for (int i  = pos+1; i < row; i++)
    {
        Q(i,pos)= 0;
        Q(pos,i)= 0;
    }
    for (int i  = 0; i < pos; i++)
    {
        if (i < 4+sumOfSys)
        {
            Q(i,pos) = 1;
            Q(pos,i) = 1;
        }
        else
        {
            Q(i,pos) = 0;
            Q(pos,i) = 0;
        }
    }
}
/*----------------------------------------------------------------------------------
 * Name     : dealLost
 * Function : Deal with the situation of stellite lost
 * Input    : int pos .(position of row to change)
 * Output   : NULL
 *---------------------------------------------------------------------------------*/
void UD_Model::dealLost(int pos)
{
    int    Pos = 4+sumOfSys+pos;
    cut_Q (Pos);
    cut_Qw();
    cut_X (Pos);
}
/*----------------------------------------------------------------------------------
 * Name     : dealSlip
 * Function : Deal with the situation of stellite slip
 * Input    : int pos .(position of row to change)
 * Output   : NULL
 *---------------------------------------------------------------------------------*/
void UD_Model::dealSlip(int  pos)
{
    int    Pos = 4+sumOfSys+pos;
         X[Pos]=0;
    resetQ(Pos);
}
/*----------------------------------------------------------------------------------
 * Name     : dealRise
 * Function : Deal with the situation of stellite rise
 * Input    : NULL
 * Output   : NULL
 *---------------------------------------------------------------------------------*/
void UD_Model::dealRise()
{
    add_Q ();
    add_Qw();
    add_X ();
}
/*----------------------------------------------------------------------------------
 * Name     : set_I_F
 * Function : Fill I and F matrix
 * Input    : NULL
 * Output   : NULL
 *---------------------------------------------------------------------------------*/
void UD_Model::set_I_F()
{
    I.resize(Q.rows(), Q.rows());
    I.setIdentity(Q.rows(), Q.rows());
    F.resize(Q.rows(), Q.rows());
    F.setIdentity(Q.rows(), Q.rows());
}

/*                    ****************************************************                                       */
/*******************************            UC Model            **************************************************/
/*                    ****************************************************                                       */

void UC_Model::initKalman(int satNum)
{}

void UC_Model::modelSolution (FinalDataFile    &pppFile,
                              vector <SatQuery> satContaine,
                              int               epoch)
{}

void UC_Model::outputResult(FinalDataFile        &pppFile,
                            X_Y_Z                &xyzCoord, /*unknown*/
                            int                  &sum,/*unknown*/
                            QTextStream          &neuText,
                      const int                  &epoch,
                      const CoordSysParameter    &coordParameter)
{}

void UC_Model::add_Q ()
{}
void UC_Model::cut_Q (int pos)
{}

void UC_Model::add_X ()
{}

void UC_Model::cut_X (int pos)
{}

void UC_Model::add_Qw()
{}

void UC_Model::cut_Qw()
{}

void UC_Model::resetQ(int pos)
{}

void UC_Model::dealLost(int pos)
{}
void UC_Model::dealSlip(int  pos)
{}
void UC_Model::dealRise()
{}

void UC_Model::set_I_F()
{}




void TF_UC_Model::initKalman(int satNum)
{}

void TF_UC_Model::modelSolution (FinalDataFile    &pppFile,
                              vector <SatQuery> satContaine,
                              int               epoch)
{}

void TF_UC_Model::outputResult(FinalDataFile        &pppFile,
                            X_Y_Z                &xyzCoord, /*unknown*/
                            int                  &sum,/*unknown*/
                            QTextStream          &neuText,
                      const int                  &epoch,
                      const CoordSysParameter    &coordParameter)
{}

void TF_UC_Model::add_Q ()
{}
void TF_UC_Model::cut_Q (int pos)
{}

void TF_UC_Model::add_X ()
{}

void TF_UC_Model::cut_X (int pos)
{}

void TF_UC_Model::add_Qw()
{}

void TF_UC_Model::cut_Qw()
{}

void TF_UC_Model::resetQ(int pos)
{}

void TF_UC_Model::dealLost(int pos)
{}
void TF_UC_Model::dealSlip(int  pos)
{}
void TF_UC_Model::dealRise()
{}

void TF_UC_Model::set_I_F()
{}



void TF_UD_Model::initKalman(int satNum)
{}

void TF_UD_Model::modelSolution (FinalDataFile    &pppFile,
                              vector <SatQuery> satContaine,
                              int               epoch)
{}

void TF_UD_Model::outputResult(FinalDataFile        &pppFile,
                            X_Y_Z                &xyzCoord, /*unknown*/
                            int                  &sum,/*unknown*/
                            QTextStream          &neuText,
                      const int                  &epoch,
                      const CoordSysParameter    &coordParameter)
{}

void TF_UD_Model::add_Q ()
{}
void TF_UD_Model::cut_Q (int pos)
{}

void TF_UD_Model::add_X ()
{}

void TF_UD_Model::cut_X (int pos)
{}

void TF_UD_Model::add_Qw()
{}

void TF_UD_Model::cut_Qw()
{}

void TF_UD_Model::resetQ(int pos)
{}

void TF_UD_Model::dealLost(int pos)
{}
void TF_UD_Model::dealSlip(int  pos)
{}
void TF_UD_Model::dealRise()
{}

void TF_UD_Model::set_I_F()
{}



///////////////////////////////////////////////下面的参考着删除//////////////////////////////////////////////////
//void Add_Matrixd(MatrixXd &Q,int num,int sys)
//{
//    MatrixXd q;
//    int row=Q.rows();
//    int col=Q.cols();
//    MatrixXd Q1=Q;
//    q.resize(row+num,col+num);
//    q.setZero(row+num,col+num);
//    for(int i=0;i<row;i++)
//    {
//        for(int j=0;j<col;j++)
//        {
//            q(i,j)=Q(i,j);
//        }
//    }
//    for(int i=row;i<row+num;i++)
//    {
//        q(i,i)=1E20;
//        for(int j=0;j<4+sys;j++)
//        {
//            q(i,j)=1;
//            q(j,i)=1;
//        }
//    }
//    Q=q;
//}
/*
void Add_Q(MatrixXd &Q, int num, int sys)
{
    MatrixXd q;
    int row=Q.rows();
    int col=Q.cols();
    int sum=(col-4-sys)/3;
    q.resize(row+3*num,col+3*num);
    q.setZero(row+3*num,col+3*num);

    q(4+sys+sum,4+sys+sum)=1E3;    //ion
    q(col+2*num,col+2*num)=1E20;   //N1
    q(4+sys+2*sum+1,4+sys+2*sum+1)=1E20;//N2

    for(int i=0;i<row+2;i++)
    {
        for(int j=0;j<col+2;j++)
        {
            if(i<4+sys+sum)
            {
                if(j<4+sys+sum)
                {
                    q(i,j)=Q(i,j);
                }
                else if(j>4+sys+sum && j<4+sys+2*sum+1)
                {
                    q(i,j)=Q(i,j-1);
                }
                else if(j>4+sys+2*sum+1)
                {
                    q(i,j)=Q(i,j-2);
                }
            }
            else if(i>4+sys+sum && i<4+sys+2*sum+1)
            {
                if(j<4+sys+sum)
                {
                    q(i,j)=Q(i-1,j);
                }
                else if(j>4+sys+sum && j<4+sys+2*sum+1)
                {
                    q(i,j)=Q(i-1,j-1);
                }
                else if(j>4+sys+2*sum+1)
                {
                    q(i,j)=Q(i-1,j-2);
                }
            }
            else if (i>4+sys+2*sum+1)
            {
                if(j<4+sys+sum)
                {
                    q(i,j)=Q(i-2,j);
                }
                else if(j>4+sys+sum && j<4+sys+2*sum+1)
                {
                    q(i,j)=Q(i-2,j-1);
                }
                else if(j>4+sys+2*sum+1)
                {
                    q(i,j)=Q(i-2,j-2);
                }
            }

        }
    }
    Q=q;
}

void Add_Q_TF_UC(MatrixXd &Q, int num, int sys)
{
    MatrixXd q;
    int row=Q.rows();
    int col=Q.cols();
    int sum=(col-4-sys)/4;
    q.resize(row+4*num,col+4*num);
    q.setZero(row+4*num,col+4*num);

    q(4+sys+sum,4+sys+sum)=1E3;    //ion
    q(4+sys+2*num+1,4+sys+2*num+1)=1E20;   //N1
    q(4+sys+3*sum+3,4+sys+3*sum+2)=1E20;//N2
    q(4+sys+4*sum+4,4+sys+4*sum+3)=1E20;//N3

    for(int i=0;i<row+3;i++)
    {
        for(int j=0;j<col+3;j++)
        {
            if(i<4+sys+sum)
            {
                if(j<4+sys+sum)
                {
                    q(i,j)=Q(i,j);
                }
                else if(j>4+sys+sum && j<4+sys+2*sum+1)
                {
                    q(i,j)=Q(i,j-1);
                }
                else if(j>4+sys+2*sum+1 && j<4+sys+2*sum+2)
                {
                    q(i,j)=Q(i,j-2);
                }
                else if(j>4+sys+2*sum+2)
                {
                    q(i,j)=Q(i,j-3);
                }
            }
            else if(i>4+sys+sum && i<4+sys+2*sum+1)
            {
                if(j<4+sys+sum)
                {
                    q(i,j)=Q(i-1,j);
                }
                else if(j>4+sys+sum && j<4+sys+2*sum+1)
                {
                    q(i,j)=Q(i-1,j-1);
                }
                else if(j>4+sys+2*sum+1 && j<4+sys+2*sum+2)
                {
                    q(i,j)=Q(i-1,j-2);
                }
                else if(j>4+sys+2*sum+2)
                {
                    q(i,j)=Q(i-1,j-3);
                }
            }
            else if (i>4+sys+2*sum+1 && i<4+sys+3*sum+2)
            {
                if(j<4+sys+sum)
                {
                    q(i,j)=Q(i-2,j);
                }
                else if(j>4+sys+sum && j<4+sys+2*sum+1)
                {
                    q(i,j)=Q(i-2,j-1);
                }
                else if(j>4+sys+2*sum+1 && j<4+sys+2*sum+2)
                {
                    q(i,j)=Q(i-2,j-2);
                }
                else if(j>4+sys+2*sum+2)
                {
                    q(i,j)=Q(i-2,j-3);
                }
            }
            else if(j>4+sys+2*sum+2)
            {
                if(j<4+sys+sum)
                {
                    q(i,j)=Q(i-3,j);
                }
                else if(j>4+sys+sum && j<4+sys+2*sum+1)
                {
                    q(i,j)=Q(i-3,j-1);
                }
                else if(j>4+sys+2*sum+1 && j<4+sys+2*sum+2)
                {
                    q(i,j)=Q(i-3,j-2);
                }
                else if(j>4+sys+2*sum+2)
                {
                    q(i,j)=Q(i-3,j-3);
                }
            }

        }
    }
    Q=q;
}

void Add_Q_TF_IF(MatrixXd &Q, int num, int sys)
{
    MatrixXd q;
    int row=Q.rows();
    int col=Q.cols();
    int sum=(col-4-sys)/2;
    q.resize(row+2*num,col+2*num);
    q.setZero(row+2*num,col+2*num);

    q(4+sys+sum,4+sys+sum)=1E20;    //N_f1f2 combination
    q(col+2*num-1,col+2*num-1)=1E20; //N_f1f3 combination

    for(int i=0;i<4+sys;i++)
    {
        q(i,4+sys+sum)=1;
        q(4+sys+sum,i)=1;
        q(col+2*num-1,i)=1;
        q(i,col+2*num-1)=1;
    }

    for(int i=0;i<row+1;i++)
    {
        for(int j=0;j<col+1;j++)
        {
            if(i<4+sys+sum)
            {
                if(j<4+sys+sum)
                {
                    q(i,j)=Q(i,j);
                }
                else if(j>4+sys+sum && j<4+sys+2*sum+1)
                {
                    q(i,j)=Q(i,j-1);
                }
            }
            else if(i>4+sys+sum && i<4+sys+2*sum+1)
            {
                if(j<4+sys+sum)
                {
                    q(i,j)=Q(i-1,j);
                }
                else if(j>4+sys+sum && j<4+sys+2*sum+1)
                {
                    q(i,j)=Q(i-1,j-1);
                }
            }
        }
    }
    Q=q;
}
*/
//void Add_Qx(MatrixXd &Q, int num)
//{
//    MatrixXd q;
//    int row=Q.rows();
//    int col=Q.cols();
//    q.resize(row+num,col+num);
//    q.setZero(row+num,col+num);
//    for(int i=0;i<row;i++)
//    {
//        for(int j=0;j<row;j++)
//        {
//            q(i,j)=Q(i,j);
//        }
//    }
//    Q=q;
//}
/*
void Add_Qx_TF_IF(MatrixXd &Q, int num)
{
    MatrixXd q;
    int row=Q.rows();
    int col=Q.cols();
    q.resize(row+2*num,col+2*num);
    q.setZero(row+2*num,col+2*num);
    for(int i=0;i<row;i++)
    {
        for(int j=0;j<row;j++)
        {
            q(i,j)=Q(i,j);
        }
    }
    Q=q;
}

void Add_Qx_un(MatrixXd &Q, int num)
{
    MatrixXd q;
    int row=Q.rows();
    int col=Q.cols();
    q.resize(row+3*num,col+3*num);
    q.setZero(row+3*num,col+3*num);
    q.topLeftCorner(row, col)=Q;
    Q=q;
}
*/
//void Add_x(VectorXd &x)
//{
//    int row=x.rows();
//    VectorXd X(row+1);
//    X.setZero(row+1);
//    for(int i=0;i<row;i++)
//    {
//        X[i]=x[i];
//    }
//    x=X;
//}
/*
void Add_x_un(VectorXd &x,int sys)
{
    int row=x.rows();
    int sum=(row-4-sys)/3;
    VectorXd X(row+3);
    X.setZero(row+3);
    for(int i=0;i<row+2;i++)
    {
        if(i<4+sys+sum)
        {
            X(i)=x(i);
        }
        else if(i>4+sys+sum && i<4+sys+2*sum+1)
        {
            X(i)=x(i-1);
        }
        else if(i>4+sys+2*sum+1)
        {
            X(i)=x(i-2);
        }
    }
    x=X;
}

void Add_X_TF_UC(VectorXd &x, int sys)
{
    int row=x.rows();
    int sum=(row-4-sys)/4;
    VectorXd X(row+4);
    X.setZero(row+4);
    for(int i=0;i<row+3;i++)
    {
        if(i<4+sys+sum)
        {
            X(i)=x(i);
        }
        else if(i>4+sys+sum && i<4+sys+2*sum+1)
        {
            X(i)=x(i-1);
        }
        else if(i>4+sys+2*sum+1 && i<4+sys+3*sum+2 )
        {
            X(i)=x(i-2);
        }
        else if(i>4+sys+3*sum+2)
        {
            X(i)=x(i-3);
        }
    }
    x=X;
}

void Add_X_TF_IF(VectorXd &x, int sys)
{
    int row=x.rows();
    int sum=(row-4-sys)/2;
    VectorXd X(row+2);
    X.setZero(row+2);
    for(int i=0;i<row+1;i++)
    {
        if(i<4+sys+sum)
        {
            X(i)=x(i);
        }
        else if(i>4+sys+sum && i<4+sys+2*sum+1)
        {
            X(i)=x(i-1);
        }
    }
    x=X;
}
*/
//void delete_Q(MatrixXd &Q, int pos)
//{
//    int N= Q.rows();
//    MatrixXd a=Q;
//    MatrixXd c(N-1,N-1);
//    c.setZero(N-1,N-1);
//    for(int i=0;i<N-1;i++)
//    {
//        for(int j=0;j<N-1;j++)
//        {
//            if (i<pos)
//            {
//                if(j<pos)
//                {
//                    c(i,j)=Q(i,j);
//                }
//                else {
//                    c(i,j)=Q(i,j+1);
//                }
//            }
//            else
//            {
//                if(j<pos)
//                {
//                    c(i,j)=Q(i+1,j);
//                }
//                else {
//                    c(i,j)=Q(i+1,j+1);
//                }
//            }
//        }
//    }

//    //    for (int i=0;i<pos;i++)
//    //    {
//    //        for (int j=0;j<pos;j++)
//    //        {
//    //            c(i,j)=a(i,j);
//    //        }
//    //    }
//    //    for (int i=pos+1;i<N;i++)
//    //    {
//    //        for (int j=0;j<pos;j++)
//    //        {
//    //            c(i-1,j)=a(i,j);
//    //        }
//    //    }
//    //    for (int j=pos+1;j<N;j++)
//    //    {
//    //        for (int i=0;i<pos;i++)
//    //        {
//    //            c(i,j-1)=a(i,j);
//    //        }
//    //    }
//    //    for (int i=pos;i<N-1;i++)
//    //    {
//    //        c(i,i)=a(i+1,i+1);
//    //    }
//    //    for (int i=pos+2;i<N;i++)
//    //    {
//    //        for (int j=pos+1;j<i;j++)
//    //        {
//    //            c(i-1,j-1)=a(i,j);
//    //        }
//    //    }
//    //    for (int j=pos+2;j<N;j++)
//    //    {
//    //        for (int i=pos+1;i<j;i++)
//    //        {
//    //            c(i-1,j-1)=a(i,j);
//    //        }
//    //    }
//    Q=c;
//}
/*
void delete_Q_un(MatrixXd &Q, int pos, int sys)
{
    int N= Q.rows();
    int sum=(N-4-sys)/3;
    MatrixXd c(N-3,N-3);   // where delete  three parameters , N1, N2, Ion
    c.setZero(N-3,N-3);
    for(int i=0;i<N-3;i++)
    {
        for(int j=0;j<N-3;j++)
        {
            if (i<pos)
            {
                if(j<pos)
                {
                    c(i,j)=Q(i,j);
                }
                else if(j>=pos && j<pos+sum-1)
                {
                    c(i,j)=Q(i,j+1);
                }
                else if( j>=pos+sum-1 && j<pos+2*sum-2)
                {
                    c(i,j)=Q(i,j+2);
                }
                else
                {
                    c(i,j)=Q(i,j+3);
                }
            }
            else if(i<pos+sum-1 && i>=pos)
            {
                if(j<pos)
                {
                    c(i,j)=Q(i+1,j);
                }
                else if(j>=pos && j<pos+sum-1)
                {
                    c(i,j)=Q(i+1,j+1);
                }
                else if( j>=pos+sum-1 && j<pos+2*sum-2)
                {
                    c(i,j)=Q(i+1,j+2);
                }
                else
                {
                    c(i,j)=Q(i+1,j+3);
                }
            }
            else if (i>=pos+sum-1 && i<pos+2*sum-2)
            {
                if(j<pos)
                {
                    c(i,j)=Q(i+2,j);
                }
                else if(j>=pos && j<pos+sum-1)
                {
                    c(i,j)=Q(i+2,j+1);
                }
                else if( j>=pos+sum-1 && j<pos+2*sum-2)
                {
                    c(i,j)=Q(i+2,j+2);
                }
                else
                {
                    c(i,j)=Q(i+2,j+3);
                }
            }
            else
            {
                if(j<pos)
                {
                    c(i,j)=Q(i+3,j);
                }
                else if(j>=pos && j<pos+sum-1)
                {
                    c(i,j)=Q(i+3,j+1);
                }
                else if( j>=pos+sum-1 && j<pos+2*sum-2)
                {
                    c(i,j)=Q(i+3,j+2);
                }
                else
                {
                    c(i,j)=Q(i+3,j+3);
                }
            }
        }
    }
    Q=c;

}


void delete_Q_TF_UC(MatrixXd &Q, int pos, int sys)
{
    int N= Q.rows();
    int sum=(N-4-sys)/4;
    MatrixXd c(N-4,N-4);   // where delete  three parameters ,Ion N1, N2, N3
    c.setZero(N-4,N-4);
    for(int i=0;i<N-4;i++)
    {
        for(int j=0;j<N-4;j++)
        {
            if (i<pos)
            {
                if(j<pos)
                {
                    c(i,j)=Q(i,j);
                }
                else if(j>=pos && j<pos+sum-1)
                {
                    c(i,j)=Q(i,j+1);
                }
                else if( j>=pos+sum-1 && j<pos+2*sum-2)
                {
                    c(i,j)=Q(i,j+2);
                }
                else
                {
                    c(i,j)=Q(i,j+3);
                }
            }
            else if(i<pos+sum-1 && i>=pos)
            {
                if(j<pos)
                {
                    c(i,j)=Q(i+1,j);
                }
                else if(j>=pos && j<pos+sum-1)
                {
                    c(i,j)=Q(i+1,j+1);
                }
                else if( j>=pos+sum-1 && j<pos+2*sum-2)
                {
                    c(i,j)=Q(i+1,j+2);
                }
                else
                {
                    c(i,j)=Q(i+1,j+3);
                }
            }
            else if (i>=pos+sum-1 && i<pos+2*sum-2)
            {
                if(j<pos)
                {
                    c(i,j)=Q(i+2,j);
                }
                else if(j>=pos && j<pos+sum-1)
                {
                    c(i,j)=Q(i+2,j+1);
                }
                else if( j>=pos+sum-1 && j<pos+2*sum-2)
                {
                    c(i,j)=Q(i+2,j+2);
                }
                else
                {
                    c(i,j)=Q(i+2,j+3);
                }
            }
            else if(i>=pos+2*sum-2 && i<pos+3*sum-3)
            {
                if(j<pos)
                {
                    c(i,j)=Q(i+3,j);
                }
                else if(j>=pos && j<pos+sum-1)
                {
                    c(i,j)=Q(i+3,j+1);
                }
                else if( j>=pos+sum-1 && j<pos+2*sum-2)
                {
                    c(i,j)=Q(i+3,j+2);
                }
                else
                {
                    c(i,j)=Q(i+3,j+3);
                }
            }
        }
    }
    Q=c;
}

void delete_Q_TF_IF(MatrixXd &Q, int pos, int sys)
{
    int N= Q.rows();
    int sum=(N-4-sys)/2;
    MatrixXd c(N-2,N-2);   // where delete  three parameters , N1, N2, Ion
    c.setZero(N-2,N-2);
    for(int i=0;i<N-2;i++)
    {
        for(int j=0;j<N-2;j++)
        {
            if (i<pos)
            {
                if(j<pos)
                {
                    c(i,j)=Q(i,j);
                }
                else if(j>=pos && j<pos+sum-1)
                {
                    c(i,j)=Q(i,j+1);
                }
                else if( j>=pos+sum-1 && j<pos+2*sum-2)
                {
                    c(i,j)=Q(i,j+2);
                }
            }
            else if(i<pos+sum-1 && i>=pos)
            {
                if(j<pos)
                {
                    c(i,j)=Q(i+1,j);
                }
                else if(j>=pos && j<pos+sum-1)
                {
                    c(i,j)=Q(i+1,j+1);
                }
                else if( j>=pos+sum-1 && j<pos+2*sum-2)
                {
                    c(i,j)=Q(i+1,j+2);
                }
            }
            else if (i>=pos+sum-1 && i<pos+2*sum-2)
            {
                if(j<pos)
                {
                    c(i,j)=Q(i+2,j);
                }
                else if(j>=pos && j<pos+sum-1)
                {
                    c(i,j)=Q(i+2,j+1);
                }
                else if( j>=pos+sum-1 && j<pos+2*sum-2)
                {
                    c(i,j)=Q(i+2,j+2);
                }
            }
        }
    }
    Q=c;
}
*/
//void delete_Qw(MatrixXd &Qw)
//{
//    int  row = Qw.rows();
//    MatrixXd qw(row-1,row-1);
//    qw.setZero(row-1,row-1);
//    for(int i=0;i<row-1;i++)
//    {
//        qw(i,i)=Qw(i,i);
//    }
//    Qw=qw;
//}
/*
void delete_Qw_un(MatrixXd &Qw)
{
    int row=Qw.rows();
    MatrixXd qw(row-2,row-2);
    qw.setZero(row-2,row-2);
    for(int i=0;i<row-2;i++)
    {
        qw(i,i)=Qw(i,i);
    }
    Qw=qw;
}

void delete_Qw_TF_IF(MatrixXd &Qw)
{
    int row=Qw.rows();
    MatrixXd qw(row-2,row-2);
    qw.setZero(row-2,row-2);
    for(int i=0;i<row-2;i++)
    {
        qw(i,i)=Qw(i,i);
    }
    Qw=qw;
}

void ResetQ(MatrixXd &Q, int pos,int sys)
{
    Q(pos,pos)=1E20;
    int row=Q.rows();
    for(int i=pos+1;i<row;i++)
    {
        Q(i,pos)=0;
        Q(pos,i)=0;
    }
    for(int i=0;i<pos;i++)
    {
        if(i<4+sys)
        {
            Q(i,pos)=1;
            Q(pos,i)=1;
        }
        else
        {
            Q(i,pos)=0;
            Q(pos,i)=0;
        }
    }
}

void ResetQ_un(MatrixXd &Q, int pos,int sys)
{

    int row=Q.rows();
    int sum=(row-4-sys)/3;
    int pos1=pos+sum;
    for(int i=4+sys;i<row;i++)
    {
        Q(i,pos)=0;
        Q(i,pos1)=0;
        Q(i,pos1+sum)=0;

        Q(pos,i)=0;
        Q(pos1,i)=0;
        Q(pos1+sum,i)=0;
    }
    for(int i=0;i<4+sys;i++)
    {
        Q(i,pos)=1;
        Q(i,pos1)=1;
        Q(i,pos1+sum)=1;

        Q(pos,i)=1;
        Q(pos1,i)=1;
        Q(pos1+sum,i)=1;
    }

    Q(pos,pos)=1E3;
    Q(pos1,pos1)=1E20;
    Q(pos1+sum,pos1+sum)=1E20;
}

void ResetQ_TF_UC(MatrixXd &Q, int pos, int sys)
{

    int row=Q.rows();
    int sum=(row-4-sys)/4;
    int pos1=pos+sum;
    for(int i=4+sys;i<row;i++)
    {
        Q(i,pos)=0;
        Q(i,pos1)=0;
        Q(i,pos1+sum)=0;
        Q(i,pos1+2*sum)=0;

        Q(pos,i)=0;
        Q(pos1,i)=0;
        Q(pos1+sum,i)=0;
        Q(pos1+2*sum,i)=0;
    }
    for(int i=0;i<4+sys;i++)
    {
        Q(i,pos)=1;
        Q(i,pos1)=1;
        Q(i,pos1+sum)=1;
        Q(i,pos1+2*sum)=1;

        Q(pos,i)=1;
        Q(pos1,i)=1;
        Q(pos1+sum,i)=1;
        Q(pos1+2*sum,i)=1;
    }
    Q(pos,pos)=1E3;
    Q(pos1,pos1)=1E20;
    Q(pos1+sum,pos1+sum)=1E20;
    Q(pos1+2*sum,pos1+2*sum)=1E20;

}

void resetQ_TF_IF(MatrixXd &Q, int pos, int sys)
{

    int row=Q.rows();
    int sum=(row-4-sys)/2;
    int pos1=pos+sum;
    for(int i=4+sys;i<row;i++)
    {
        Q(i,pos)=0;
        Q(i,pos1)=0;

        Q(pos,i)=0;
        Q(pos1,i)=0;
    }
    for(int i=0;i<4+sys;i++)
    {
        Q(i,pos)=1;
        Q(i,pos1)=1;

        Q(pos,i)=1;
        Q(pos1,i)=1;
    }
    Q(pos1,pos1)=1E20;
    Q(pos,pos)=1E20;
}
*/
//void delete_x(VectorXd &x,int pos)
//{
//    VectorXd X;
//    int num=x.rows();
//    X.resize(num-1);
//    int t=0;
//    for(int i=0;i<num;i++)
//    {
//        if(i!=pos)
//        {
//            X[t]=x[i];
//            t++;
//        }
//    }
//    x=X;
//}
/*
void delete_x_un(VectorXd &x,int pos,int sys)
{
    VectorXd X;
    int num=x.rows();
    X.resize(num-3);
    int m=(num-sys-4)/3;
    int t=0;
    for(int i=0;i<num;i++)
    {
        if(i!=pos && i!=pos+m && i!=pos+2*m)
        {
            X[t]=x[i];
            t++;
        }
    }
    x=X;
}

void delete_X_TF_UC(VectorXd &x, int pos, int sys)
{
    VectorXd X;
    int num=x.rows();
    X.resize(num-3);
    int m=(num-sys-4)/4;
    int t=0;
    for(int i=0;i<num;i++)
    {
        if(i!=pos && i!=pos+m && i!=pos+2*m && i!=pos+3*m)
        {
            X[t]=x[i];
            t++;
        }
    }
    x=X;
}

void delete_X_TF_IF(VectorXd &x, int pos, int sys)
{
    VectorXd X;
    int num=x.rows();
    X.resize(num-2);
    int m=(num-sys-4)/2;
    int t=0;
    for(int i=0;i<num;i++)
    {
        if(i!=pos && i!=pos+m )
        {
            X[t]=x[i];
            t++;
        }
    }
    x=X;
}

void delete_X_UC_IF(VectorXd &x, int pos, int sys)
{
    VectorXd X;
    int num=x.rows();
    X.resize(num-2);
    int m=(num-sys-4)/2;
    int t=0;
    for(int i=0;i<num;i++)
    {
        if(i!=pos && i!=pos+m )
        {
            X[t]=x[i];
            t++;
        }
    }
    x=X;
}

*/


