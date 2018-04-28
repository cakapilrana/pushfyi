/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 18/05/2015
Purpose: System Metric Container

*********************************************************
*/
#ifndef METRIC_H
#define METRIC_H

#include <string>
#include "pushfyi-types.h"

    /**
     * @class Metric
     * Single integer metric utility class.
     */

class Metric
{
public:
    Metric();
    Metric(const Metric& toCopy);
    Metric(rxInt id, const std::string& objectName, const std::string& metricName, rxInt val = 0);
    ~Metric();

    Metric& operator=(const Metric &toCopy);

    void     setId(rxInt id);
    rxInt    getId();

    void setObjectName(std::string& name);
    std::string getObjectName();

    void setMetricName(std::string& name);
    std::string getMetricName();

    rxInt getVal();
    void  setVal(rxInt val);

    rxInt operator++();
    rxInt operator--();

    rxInt inc(rxInt delta = 1);
    rxInt dec(rxInt delta = 1);

private:
    rxInt       mId;
    std::string mObjectName;
    std::string mMetricName;
    rxInt       mValue;
};

inline  void Metric::setId(rxInt id)
{
    mId = id;
}

inline rxInt Metric::getId()
{
    return mId;
}

inline void Metric::setObjectName(std::string& name)
{
    mObjectName = name;
}

inline std::string Metric::getObjectName()
{
    return mObjectName;
}

inline void Metric::setMetricName(std::string& name)
{
    mMetricName = name;
}

inline std::string Metric::getMetricName()
{
    return mMetricName;
}

inline rxInt Metric::getVal()
{
    return mValue;
}

inline void  Metric::setVal(rxInt val)
{
    mValue = val;
}

inline rxInt Metric::operator++()
{
    return ++mValue;
}

inline rxInt Metric::operator--()
{
    return --mValue;
}

inline rxInt Metric::inc(rxInt inc)
{
    mValue += inc;
    return mValue;
}

inline rxInt Metric::dec(rxInt dec)
{
    mValue -= dec;
    return mValue;
}


#endif // METRIC

