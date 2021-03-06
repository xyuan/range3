/*********************************************************************
 *  AUTHOR: Tomas Soltys                                             *
 *  FILE:   rmatrixsolver.h                                          *
 *  GROUP:  RSolverLib                                               *
 *  TYPE:   header file (*.h)                                        *
 *  DATE:   23-rd January 2013                                       *
 *                                                                   *
 *  DESCRIPTION: Matrix solver class declaration                     *
 *********************************************************************/

#ifndef __RMATRIXSOLVER_H__
#define __RMATRIXSOLVER_H__

#include <rmlib.h>

#include "riterationinfo.h"
#include "rmatrixpreconditioner.h"

class RMatrixSolver
{

    protected:

        //! Matrix solver input.
        RMatrixSolverConf matrixSolverConf;
        //! Iteration ionformation.
        RIterationInfo iterationInfo;

    private:

        //! Internal initialization function.
        void _init(const RMatrixSolver *pMatrixSolver = 0);

    public:

        //! Constructor.
        RMatrixSolver(const RMatrixSolverConf &matrixSolverConf);

        //! Copy constructor.
        RMatrixSolver(const RMatrixSolver &matrixSolver);

        //! Destructor.
        ~RMatrixSolver();

        //! Assignment operator.
        RMatrixSolver & operator =(const RMatrixSolver &matrixSolver);

        //! Solve matrix system.
        void solve(const RSparseMatrix &A, const RRVector &b, RRVector &x, RMatrixPreconditionerType matrixPreconditionerType = R_MATRIX_PRECONDITIONER_NONE, unsigned int blockSize = 1);

        //! Disable convergence log file.
        void disableConvergenceLogFile(void);

    protected:

        //! ConjugateGradient solver.
        void solveCG(const RSparseMatrix &A, const RRVector &b, RRVector &x, RMatrixPreconditioner &P);

        //! Generalize minimal residual solver.
        void solveGMRES(const RSparseMatrix &A, const RRVector &b, RRVector &x, RMatrixPreconditioner &P);

};

#endif // __RMATRIXSOLVER_H__
