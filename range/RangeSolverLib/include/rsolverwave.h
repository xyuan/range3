/*********************************************************************
 *  AUTHOR: Tomas Soltys                                             *
 *  FILE:   rsolverwave.h                                            *
 *  GROUP:  RSolverLib                                               *
 *  TYPE:   header file (*.h)                                        *
 *  DATE:   18-th August 2015                                        *
 *                                                                   *
 *  DESCRIPTION: Wave solver class declaration                       *
 *********************************************************************/

#ifndef __RSOLVERWAVE_H__
#define __RSOLVERWAVE_H__

#include "rsolvergeneric.h"

class RSolverWave : public RSolverGeneric
{

    protected:

        //! Element wave speed.
        RRVector elementWaveSpeed;
        //! Element wave displacement.
        RRVector elementWaveDisplacement;
        //! Element node displacement.
        RRVector nodeWaveDisplacement;

    private:

        //! Internal initialization function.
        void _init(const RSolverWave *pWaveSolver = 0);

    public:

        //! Constructor.
        explicit RSolverWave(RModel *pModel, const QString &modelFileName, const QString &convergenceFileName, RSolverSharedData &sharedData);

        //! Copy constructor.
        RSolverWave(const RSolverWave &waveSolver);

        //! Destructor.
        ~RSolverWave();

        //! Assignment operator.
        RSolverWave & operator =(const RSolverWave &waveSolver);

        //! Check if solver has converged.
        bool hasConverged(void) const;

    protected:

        //! Update scales.
        void updateScales(void);

        //! Recover previously computed results.
        void recover(void);

        //! Prepare solver.
        void prepare(void);

        //! Run matrix solver.
        void solve(void);

        //! Process solver results.
        void process(void);

        //! Store solver results.
        void store(void);

        //! Process statistics.
        void statistics(void);

};

#endif // __RSOLVERWAVE_H__
