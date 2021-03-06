/*********************************************************************
 *  AUTHOR: Tomas Soltys                                             *
 *  FILE:   model_tree_simple.h                                      *
 *  GROUP:  Range                                                    *
 *  TYPE:   header file (*.h)                                        *
 *  DATE:   15-th May 2013                                           *
 *                                                                   *
 *  DESCRIPTION: Simple model tree class declaration                 *
 *********************************************************************/

#ifndef __MODEL_TREE_SIMPLE_H__
#define __MODEL_TREE_SIMPLE_H__

#include <QTreeWidget>

#include <rmlib.h>

#include "session_entity_id.h"

class ModelTreeSimple : public QTreeWidget
{

    Q_OBJECT

    public:

        //! Constructor.
        explicit ModelTreeSimple(uint modelID, REntityGroupTypeMask modelEntitiesMask, QWidget *parent = 0);

        //! Select entity.
        void selectEntity(uint modelID, REntityGroupType entityGroupType, uint entityID);

        //! Get list of selected entity IDs.
        QList<SessionEntityID> getSelected(void) const;

    private:

        //! Add model to the tree.
        void addModel(uint modelID, REntityGroupTypeMask modelEntitiesMask);
        
    signals:
        
    protected slots:

        //! Model selection has canged.
        void onSelectionChanged(void);
        
};

#endif // __MODEL_TREE_SIMPLE_H__
