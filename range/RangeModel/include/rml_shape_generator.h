/*********************************************************************
 *  AUTHOR: Tomas Soltys                                             *
 *  FILE:   rml_shape_generator.h                                    *
 *  GROUP:  RML                                                      *
 *  TYPE:   header file (*.h)                                        *
 *  DATE:   17-th May 2013                                           *
 *                                                                   *
 *  DESCRIPTION: Shape generator class declaration                   *
 *********************************************************************/

#ifndef __RML_SHAPE_GENERATOR_H__
#define __RML_SHAPE_GENERATOR_H__

#include "rml_model_raw.h"

class RShapeGenerator
{

    private:

        //! Internal initialization function.
        void _init ( const RShapeGenerator *pShapeGenerator = 0 );

    public:

        //! Constructor.
        RShapeGenerator ();

        //! Copy constructor.
        RShapeGenerator (const RShapeGenerator &shapeGenerator);

        //! Destructor.
        ~RShapeGenerator ();

        //! Assignment operator.
        RShapeGenerator & operator = ( const RShapeGenerator &shapeGenerator );

        //! Generate arrow.
        static RModelRaw generateArrow(const RR3Vector &v1, const RR3Vector &v2, double scale = 1.0);

};

#endif /* __RML_SHAPE_GENERATOR_H__ */
