/*********************************************************************
 *  AUTHOR: Tomas Soltys                                             *
 *  FILE:   rbl_r3vector.h                                           *
 *  GROUP:  RBL                                                      *
 *  TYPE:   header file (*.h)                                        *
 *  DATE:   15-th February 2013                                      *
 *                                                                   *
 *  DESCRIPTION: Real vector[3] class declaration                    *
 *********************************************************************/

#ifndef __RBL_R3VECTOR_H__
#define __RBL_R3VECTOR_H__

#include "rbl_rvector.h"
#include "rbl_rmatrix.h"

class RR3Vector : public RRVector
{

    private:

        //! Internal initialization function.
        void _init ( const RR3Vector *pR3Vector = 0 );

    public:

        //! Constructor.
        RR3Vector ();

        //! Copy constructor.
        RR3Vector ( const RR3Vector &r3Vector );

        //! Position vector constructor.
        RR3Vector ( double x, double y, double z );

        //! Destructor.
        ~RR3Vector ();

        //! Assignment operator.
        RR3Vector & operator = ( const RR3Vector &r3Vector );

        //! Calculate rotation matrix from local to global coordinates.
        //! Vector is assumed to be in global coordinates.
        //! To get matrix from global to local just invert R matrix.
        void findRotationMatrix ( RRMatrix &R ) const;

        //! Return opposite vector.
        RR3Vector getOpposite(void) const;

        //! Find orthogonal vector.
        RR3Vector findOrthogonal(void) const;

        //! Clear vector.
        void clear(void);

        static double findDistance(const RR3Vector &v1,const RR3Vector &v2);

};

#endif // __RBL_R3VECTOR_H__
