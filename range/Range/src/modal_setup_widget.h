/*********************************************************************
 *  AUTHOR: Tomas Soltys                                             *
 *  FILE:   modal_setup_widget.h                                     *
 *  GROUP:  Range                                                    *
 *  TYPE:   header file (*.h)                                        *
 *  DATE:   11-th May 2016                                           *
 *                                                                   *
 *  DESCRIPTION: Modal setup widget class declaration                *
 *********************************************************************/

#ifndef __MODAL_SETUP_WIDGET_H__
#define __MODAL_SETUP_WIDGET_H__

#include <QWidget>
#include <QSpinBox>

#include <rmlib.h>

class ModalSetupWidget : public QWidget
{
    Q_OBJECT

    protected:

        //! Modal setup.
        RModalSetup modalSetup;
        //! Number of iterations spin box.
        QSpinBox *spinNIterations;
        //! Number of modes to extract spin box.
        QSpinBox *spinNModesExtract;

    public:

        //! Constructor.
        explicit ModalSetupWidget(const RModalSetup &modalSetup, QWidget *parent = 0);

    signals:

        //! Modal setup has changed.
        void changed(const RModalSetup &modalSetup);

    private slots:

        void onModalMethodChanged(int index);

        void onNIterationsChanged(int nIterations);

        void onNModesExtractChanged(int nModes);

        void onConvergenceValueChanged(double convergenceValue);

};

#endif // __MODAL_SETUP_WIDGET_H__
