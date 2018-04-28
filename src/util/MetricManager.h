/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: System Metric Manager. Keeps Metric in Cache.

*********************************************************
*/
#ifndef __METRIC_MGR_H__
#define __METRIC_MGR_H__

//#include <ext/hash_map>
#include <map>
#include <vector>
#include "pushfyi-types.h"
#include "Metric.h"
#include "Mutex.h"

/**
 * @ingroup base
 * MetricManager is a singleton object used for global metric management.
 * Objects register with the MetricManager to obtain
 * unique metric ids. This unique metric id is then used by the object to
 * report metric information.
 *
 */

using namespace std;

class MetricManager
{
public:
    /** Singleton - get the one and only instance. Create one if none exists
     *  @return MetricManager instance
     **/
    static MetricManager* instance();



    /** Destroy the current MetricManager instance
     **/
    ~MetricManager();


    /** Add the metric objectName and return a unique metric key
     *
     * @param objectName    - slee, device type
     * @param metricName    - device id, service context etc.
     * @return key          - unique key on success, -1 on error
     **/
    rxInt add(const std::string& objectName, const std::string& metricName);

    /** Deregister entry  - also removes the metric entry
     * @param metricId    - metric id
     * @return key        - 0 on success, -1 on error
     **/
    rxInt remove(rxInt metricId);

    /** Return the metric id for objectName and metricName
     * @param objectName - slee, device type etc.
     * @param metricName - device id, service context etc.
     * @return key       - unique key on success, -1 on error if no entry exists
     **/
    rxInt getMetricId(const std::string& objectName, const std::string& metricName);

    /** Given a metric id, return the metric data
     *
     * @param id     - metric id
     * @param metric - structure to be filled in
     * @return val   - 0 on success, -1 on error
     **/
    rxInt getMetric(rxInt metricId, Metric& metric);

    /**
     *  Get metric info returns the metric configuration  of all currently registered entries
     * @param entries - array of metric entries
     * @return val    - number of items in the list
     **/

    rxInt getMetricEntries(std::vector<Metric>& entries);

    /** Get the metric value for metric id
     *  @param metricId
     *  @return val
     **/
    rxInt getVal(rxInt metricId);

    /** Set the metric value for metric id
     *  @param metricId
     *  @param val
     **/
    void  setVal(rxInt metricId, rxInt val);


    /** Increment the metric value for metric id by delta
     *  @param metricId
     *  @param delta  - default 1
     *  @return value - current metric value
     **/
    rxInt inc(rxInt metricId, rxInt delta = 1);

    /** Decrement the metric value for metric id by delta
     *  @param metricId
     *  @param delta  - default 1
     *  @return value - current metric value
     **/
    rxInt dec(rxInt metricId, rxInt delta = 1);

    /** Reset the metric value for metric id to 0 - default or a number
     *  @param val
     **/
    void resetMetrics(rxInt val = 0);

private:
    //typedef __gnu_cxx::hash_map<rxUInt, Metric> MetricMap;
    typedef std::map<rxUInt, Metric> MetricMap;
    typedef MetricMap::iterator MetricIter;

    MetricManager();
    rxInt getNextId();

    Mutex          mMutex;
    rxInt          mUniqueId;

    MetricMap      mMetric;
};

inline rxInt MetricManager::getNextId()
{
    return mUniqueId++;
}

#endif //  __METRIC_MGR_H__

