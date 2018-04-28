/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: System Metric Manager. Keeps Metric in Cache.

*********************************************************
*/
#include "MetricManager.h"
#include "Metric.h"
#include "Synchronized.h"

using namespace std;

static MetricManager* theMgr;


MetricManager::MetricManager()
    : mUniqueId(0)
{
}

MetricManager* MetricManager::instance()
{
    if(theMgr == NULL){
        theMgr = new MetricManager();
    }

    return theMgr;
}

MetricManager::~MetricManager()
{
    theMgr = NULL;
}


    /** Return a unique metric key for object id and metricId
     * @param objectId  - device id, service id, etc.
     * @param metricId  - metric id - tagSeen, FirmRead, Failure, ...
     * @return key      - unique key on success, -1 on error if no entry exists
     **/
rxInt MetricManager::getMetricId(const std::string& objectName, const std::string& metricName)
{
    Synchronized lock(mMutex);

    MetricIter it = mMetric.begin();

    for(; it != mMetric.end(); it++){
        if( (it->second.getObjectName() == objectName) && (it->second.getMetricName() == metricName) ){
            //DBG(Log::eProcess, "Entry for objectId(%s):metricName(%s) found: metricName(%d)", 
            //    objectName.c_str(), metricName.c_str(), it->second.getId());
            return it->second.getId();
        }
    }

    //DBG(Log::eProcess, "Entry for objectName(%s):metricName(%s) NOT found", 
    //    objectName.c_str(), metricName.c_str());

    return -1;
}

    /** Add the objectId and return a unique key
     * @param objectId    - slee, device etc.
     * @param metricId - device id, service context etc.
     * @return key      - unique key on success, -1 on error
     **/
rxInt MetricManager::add(const std::string& objectId, const std::string& metricId)
{
    Synchronized lock(mMutex);

    rxInt id = getMetricId(objectId, metricId);

    //if(id >= 0){  // already exists
    //
    //    DBGFINEST(Log::eProcess, "Metric entry already registered. objectId(%s), metricId(%s)",
    //        objectId.c_str(), metricId.c_str());
    //
    //}else{

    if(id < 0){

        id = getNextId();
        Metric entry(id, objectId, metricId);

        //DBGFINEST(Log::eProcess, "Entry for objectId(%s):metricId(%s) added", 
        //    objectId.c_str(), metricId.c_str());

        mMetric.insert(pair<rxUInt, Metric>(id, entry));
    }
    return id;
}
    
    /** Deregister entry - also removes the tracing key from the 
     * list of traceId's.
     * @param metric id
     * @return rc  - 0 on success, -1 on error
     **/
rxInt MetricManager::remove(rxInt id)
{
    Synchronized lock(mMutex);

    MetricIter it = mMetric.find(id);

    if(it != mMetric.end()){
        mMetric.erase(it);
        //DBGFINEST(Log::eProcess, "Entry with  metric Id(%d) found and deleted", id);
        return 0;
    }

    //DBG(Log::eProcess, "No entry for metric Id(%d) found", id);
    return -1;
}

    
    /** Given a unique id key, return the Metric entry
     *
     * @param traceId   - unique key
     * @param metric    - metric entry to be filled in
     * @return val      - 0 success, -1 failure
     **/
rxInt MetricManager::getMetric(rxInt id, Metric& metric)
{
    Synchronized lock(mMutex);

    MetricIter it = mMetric.find(id);

    if(it != mMetric.end()){
        metric = it->second;
        //DBG(Log::eProcess, "Entry with  metric Id(%d) found", id);
        return 0;
    }

    //DBG(Log::eProcess, "No entry for metric Id(%d) found", id);
    return -1;
}
    

    /* Get metric info returns the metric configuration  of all currently registered entries
     * @param entries - array of metric entries
     * @return val    - number of items in the list
     **/

rxInt MetricManager::getMetricEntries(std::vector<Metric>& entries)
{
    Synchronized lock(mMutex);

    MetricIter it = mMetric.begin();
    //DBG(Log::eProcess, "Retrieving %d metric entries", mMetric.size());

    for(; it != mMetric.end(); it++){
        entries.push_back(it->second);
    }

    return mMetric.size();
}


    /** Get the metric value for metric id
     *  @param metricId
     *  @return val
     **/
rxInt MetricManager::getVal(rxInt metricId)
{
    Synchronized lock(mMutex);

    MetricIter it = mMetric.find(metricId);

    if(it != mMetric.end()){
        return it->second.getVal();
    }else{
        //DBG(Log::eProcess, "Metric id: %d not found", metricId);
        return -1;
    }
}


    /** Set the metric value for metric id
     *  @param metricId
     *  @param val
     **/
void MetricManager:: setVal(rxInt metricId, rxInt val)
{
    Synchronized lock(mMutex);

    MetricIter it = mMetric.find(metricId);

    if(it != mMetric.end()){
        it->second.setVal(val);
    }else{
        //DBG(Log::eProcess, "Metric id: %d not found", metricId);
    }
}

    /** Increment the metric value for metric id by delta
     *  @param metricId
     *  @param delta  - default 1
     *  @return value - current metric value
     **/
rxInt MetricManager::inc(rxInt metricId, rxInt delta)
{
    Synchronized lock(mMutex);

    MetricIter it = mMetric.find(metricId);

    if(it != mMetric.end()){
        return it->second.inc(delta);
    }else{
        //DBG(Log::eProcess, "Metric id: %d not found", metricId);
        return -1;
    }
}


    /** Decrement the metric value for metric id by delta
     *  @param metricId
     *  @param delta  - default 1
     *  @return value - current metric value
     **/
rxInt MetricManager::dec(rxInt metricId, rxInt delta)
{
    Synchronized lock(mMutex);

    MetricIter it = mMetric.find(metricId);

    if(it != mMetric.end()){
        return it->second.dec(delta);
    }else{
        //DBG(Log::eProcess, "Metric id: %d not found", metricId);
        return -1;
    }
}



void MetricManager::resetMetrics(rxInt val)
{
    Synchronized lock(mMutex);

    for(rxUInt i = 0; i < mMetric.size(); i++){
        setVal(i, val);
    }
}




        
