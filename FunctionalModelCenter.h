#ifndef FUNCTIONALMODELCENTER_H
#define FUNCTIONALMODELCENTER_H
#include <Eigen/Eigen>
#include <QString>
#include <QTextStream>
#include "ResultDataCenter.h"
using namespace Eigen;


/*----------------------------------------------------------------------------
 * Name     : FuncModel
 * Function : Function model for adjustment
 *---------------------------------------------------------------------------*/
class FuncModel
{
public:
    virtual void initKalman(int satNum) = 0;                                   // Initialize the matrix of kalman fliter
    virtual void modelSolution (FinalDataFile    &pppFile,                     // Main processing solution
                                vector <SatQuery> satContaine,
                                int               epoch) = 0;

    virtual void dealLost(int pos) = 0;                                        // Dealing with satellite landing
    virtual void dealSlip(int pos) = 0;                                        // Dealing with satellite sliping
    virtual void dealRise() = 0;                                               // Dealing with satellite rising
    virtual void outputResult(FinalDataFile        &pppFile,
                              X_Y_Z                &xyzCoord, /*unknown*/
                              int                  &sum,/*unknown*/
                              QTextStream          &neuText,
                        const int                  &epoch,
                        const CoordSysParameter    &coordParameter) = 0;
    virtual void set_I_F() = 0;                                                // Initialize matrix I and F

protected:
    virtual void add_Q () = 0;                                                 // Add a new row to matrix
    virtual void cut_Q (int pos) = 0;                                          // Delete an invalid row of matrix
    virtual void add_X () = 0;
    virtual void cut_X (int pos) = 0;
    virtual void add_Qw() = 0;
    virtual void cut_Qw() = 0;
    virtual void resetQ(int pos) = 0;                                          // Reset one row in matrix

public:
    MatrixXd Q;
    MatrixXd Qw;
    VectorXd X;
    MatrixXd F;
    MatrixXd I;
    MatrixXd B;
    MatrixXd R;
    VectorXd L;
    VectorXd V;

    int sumOfSys;                                                              // Number of chosen systems
    FuncModel() :sumOfSys(1) {}
};
/*----------------------------------------------------------------------------
 * Name     : UD_Model
 * Function : Un-Difference ionosphere-free combined model for adjustment
 *---------------------------------------------------------------------------*/
class UD_Model : public FuncModel
{
public:
    virtual void initKalman(int satNum);
    virtual void modelSolution (FinalDataFile    &pppFile,
                                vector <SatQuery> satContaine,
                                int               epoch);

    virtual void dealLost(int pos);
    virtual void dealSlip(int pos);
    virtual void dealRise();
    virtual void outputResult(FinalDataFile        &pppFile,
                              X_Y_Z                &xyzCoord, /*unknown*/
                              int                  &sum,/*unknown*/
                              QTextStream          &neuText,
                        const int                  &epoch,
                        const CoordSysParameter    &coordParameter);
    virtual void set_I_F();

protected:
    virtual void add_Q ();
    virtual void cut_Q (int pos);
    virtual void add_X ();
    virtual void cut_X (int pos);
    virtual void add_Qw();
    virtual void cut_Qw();
    virtual void resetQ(int pos);

};
/*----------------------------------------------------------------------------
 * Name     : UC_Model
 * Function : Un-Combined model for adjustment
 *---------------------------------------------------------------------------*/
class UC_Model : public FuncModel
{
public:
    virtual void initKalman(int satNum/*, int sumOfSys*/);
    virtual void modelSolution (FinalDataFile    &pppFile,
                                vector <SatQuery> satContaine,
                                int               epoch);

    virtual void dealLost(int pos);
    virtual void dealSlip(int pos);
    virtual void dealRise();
    virtual void outputResult(FinalDataFile        &pppFile,
                              X_Y_Z                &xyzCoord, /*unknown*/
                              int                  &sum,/*unknown*/
                              QTextStream          &neuText,
                        const int                  &epoch,
                        const CoordSysParameter    &coordParameter);
    virtual void set_I_F();

protected:
    virtual void add_Q ();
    virtual void cut_Q (int pos);
    virtual void add_X ();
    virtual void cut_X (int pos);
    virtual void add_Qw();
    virtual void cut_Qw();
    virtual void resetQ(int pos);

};
/*----------------------------------------------------------------------------
 * Name     : TF_UD_Model
 * Function : Triple frequence ionosphere-free combined model for adjustment
 *---------------------------------------------------------------------------*/
class TF_UD_Model : public FuncModel
{
public:
    virtual void initKalman(int satNum/*, int sumOfSys*/);
    virtual void modelSolution (FinalDataFile    &pppFile,
                                vector <SatQuery> satContaine,
                                int               epoch);

    virtual void dealLost(int pos);
    virtual void dealSlip(int pos);
    virtual void dealRise();
    virtual void outputResult(FinalDataFile        &pppFile,
                              X_Y_Z                &xyzCoord, /*unknown*/
                              int                  &sum,/*unknown*/
                              QTextStream          &neuText,
                        const int                  &epoch,
                        const CoordSysParameter    &coordParameter);
    virtual void set_I_F();

protected:
    virtual void add_Q ();
    virtual void cut_Q (int pos);
    virtual void add_X ();
    virtual void cut_X (int pos);
    virtual void add_Qw();
    virtual void cut_Qw();
    virtual void resetQ(int pos);

};
/*----------------------------------------------------------------------------
 * Name     : TF_UC_Model
 * Function : Triple frequence Un-Combined model model for adjustment
 *---------------------------------------------------------------------------*/
class TF_UC_Model : public FuncModel
{
public:
    virtual void initKalman(int satNum/*, int sumOfSys*/);
    virtual void modelSolution (FinalDataFile    &pppFile,
                                vector <SatQuery> satContaine,
                                int               epoch);

    virtual void dealLost(int pos);
    virtual void dealSlip(int pos);
    virtual void dealRise();
    virtual void outputResult(FinalDataFile        &pppFile,
                              X_Y_Z                &xyzCoord, /*unknown*/
                              int                  &sum,/*unknown*/
                              QTextStream          &neuText,
                        const int                  &epoch,
                        const CoordSysParameter    &coordParameter);
    virtual void set_I_F();

protected:
    virtual void add_Q ();
    virtual void cut_Q (int pos);
    virtual void add_X ();
    virtual void cut_X (int pos);
    virtual void add_Qw();
    virtual void cut_Qw();
    virtual void resetQ(int pos);

};

#endif // FUNCTIONALMODELCENTER_H
