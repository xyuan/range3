/*********************************************************************
 *  AUTHOR: Tomas Soltys                                             *
 *  FILE:   rml_monitoring_point_manager.h                           *
 *  GROUP:  RML                                                      *
 *  TYPE:   header file (*.h)                                        *
 *  DATE:   12-th March 2014                                         *
 *                                                                   *
 *  DESCRIPTION: Monitoring point manager class declaration          *
 *********************************************************************/

#ifndef __RML_MONITORING_POINT_MANAGER_H__
#define __RML_MONITORING_POINT_MANAGER_H__

#include <vector>

#include "rml_monitoring_point.h"

class RMonitoringPointManager : public std::vector<RMonitoringPoint>
{

    private:

        //! Internal initialization function.
        void _init(const RMonitoringPointManager *pMonitoringPointManager = 0);

    public:

        //! Constructor.
        RMonitoringPointManager();

        //! Copy constructor.
        RMonitoringPointManager(const RMonitoringPointManager &monitoringPointManager);

        //! Destructor.
        ~RMonitoringPointManager();

        //! Assignment operator.
        RMonitoringPointManager & operator =(const RMonitoringPointManager &monitoringPointManager);

        //! Set output file name for all monitoring points.
        void setOutputFileName(const QString &fileName);

        //! Allow RFileIO to access private members.
        friend class RFileIO;

};

#endif // __RML_MONITORING_POINT_MANAGER_H__
