/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: System Metric Container

*********************************************************
*/
#include "Metric.h"

using namespace std;


Metric::Metric()
    : mId(-1), mValue(0)
{
}

Metric::Metric(rxInt id, const std::string& objectName, const std::string& metricName, rxInt val)
    : mId(id), mObjectName(objectName), mMetricName(metricName), mValue(val)
{
}
 
Metric::~Metric()
{
}

Metric::Metric(const Metric& toCopy)
{
    *this = toCopy;
}

Metric& Metric::operator =(const Metric& toCopy)
{
    mId         = toCopy.mId;
    mObjectName = toCopy.mObjectName;
    mMetricName = toCopy.mMetricName;
    mValue      = toCopy.mValue;

    return *this;
}




    
    
    
  
        
