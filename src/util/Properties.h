/*
*********************************************************
        This code is a proprietary of Kapil Rana

Following may not be used in part or in whole without the
prior permission of Kapil Rana

Author: Kapil Rana
Date: 20/06/2015
Purpose: System configuration wrapper class

*********************************************************
*/
#ifndef __PROPERTIES_H__
#define __PROPERTIES_H__

#include <list>
#include <string>
#include <fstream>
#include "pushfyi-types.h"
#include "Params.h"

class Properties
{
public:
    Properties();
    Properties(const Params& params);
    Properties(const std::string& filename);

    virtual ~Properties();

    const Params& getParams();

    rxInt load(std::ifstream& propstream);

    std::string get(const std::string& key, const std::string& value = RX_ESREF);
    void set(const std::string& key, const std::string& value);

    rxUInt getUInt(const std::string& key, rxUInt value = 0);
    void setUInt(const std::string& key, rxUInt value);

    rxInt getInt(const std::string& key, rxInt value = 0);
    void setInt(const std::string& key, rxInt value);

    rxULong getULong(const std::string& key, rxULong value = 0LL);
    void setULong(const std::string& key, rxULong value);

    rxLong getLong(const std::string& key, rxLong value = 0LL);
    void setLong(const std::string& key, rxLong value);

    rxFloat getFloat(const std::string& key, rxFloat value = 0.0);
    void setFloat(const std::string& key, rxFloat value);

    rxDouble getDouble(const std::string& key, rxDouble value = 0.0);
    void setDouble(const std::string& key, rxDouble value);

    bool getBool(const std::string& key, bool value = false);
    void setBool(const std::string& key, bool value);

    bool contains(const std::string& key);

    void remove(const std::string& key);

    HashtableIter begin();
    HashtableIter end();

    bool save();
    bool save(const std::string& filename);

private:
    typedef std::list<std::string> List;
    typedef List::iterator ListIter;

    std::string mFilename;
    Params      mParams;
    List        mOrder;
};

inline const Params& Properties::getParams()
{
    return mParams;
}

inline std::string Properties::get(const std::string& key, const std::string& value)
{
    return mParams.get(key, value);
}

inline void Properties::set(const std::string& key, const std::string& value)
{
    if (!mParams.contains(key)) {
        mOrder.push_back(key);
    }

    return mParams.set(key, value);
}

inline rxUInt Properties::getUInt(const std::string& key, rxUInt value)
{
    return mParams.getUInt(key, value);
}

inline void Properties::setUInt(const std::string& key, rxUInt value)
{
    if (!mParams.contains(key)) {
        mOrder.push_back(key);
    }

    return mParams.setUInt(key, value);
}

inline rxInt Properties::getInt(const std::string& key, rxInt value)
{
    return mParams.getInt(key, value);
}

inline void Properties::setInt(const std::string& key, rxInt value)
{
    if (!mParams.contains(key)) {
        mOrder.push_back(key);
    }

    return mParams.setInt(key, value);
}

inline rxULong Properties::getULong(const std::string& key, rxULong value)
{
    return mParams.getULong(key, value);
}

inline void Properties::setULong(const std::string& key, rxULong value)
{
    if (!mParams.contains(key)) {
        mOrder.push_back(key);
    }

    return mParams.setULong(key, value);
}

inline rxLong Properties::getLong(const std::string& key, rxLong value)
{
    return mParams.getLong(key, value);
}

inline void Properties::setLong(const std::string& key, rxLong value)
{
    if (!mParams.contains(key)) {
        mOrder.push_back(key);
    }

    return mParams.setLong(key, value);
}

inline rxFloat Properties::getFloat(const std::string& key, rxFloat value)
{
    return mParams.getFloat(key, value);
}

inline void Properties::setFloat(const std::string& key, rxFloat value)
{
    if (!mParams.contains(key)) {
        mOrder.push_back(key);
    }

    return mParams.setFloat(key, value);
}

inline rxDouble Properties::getDouble(const std::string& key, rxDouble value)
{
    return mParams.getDouble(key, value);
}

inline void Properties::setDouble(const std::string& key, rxDouble value)
{
    if (!mParams.contains(key)) {
        mOrder.push_back(key);
    }

    return mParams.setDouble(key, value);
}

inline bool Properties::getBool(const std::string& key, bool value)
{
    return mParams.getBool(key, value);
}

inline void Properties::setBool(const std::string& key, bool value)
{
    if (!mParams.contains(key)) {
        mOrder.push_back(key);
    }

    return mParams.setBool(key, value);
}

inline bool Properties::contains(const std::string& key)
{
    return mParams.contains(key);
}

inline void Properties::remove(const std::string& key)
{
    ListIter iter = mOrder.begin();

    for ( ; iter != mOrder.end(); iter++) {
        if (*iter == key) {
            mOrder.erase(iter);
            break;
        }
    }

    mParams.remove(key);
}

inline bool Properties::save(const std::string& filename)
{
    mFilename = filename;

    return save();
}

inline HashtableIter Properties::begin()
{
    return mParams.begin();
}

inline HashtableIter Properties::end()
{
    return mParams.end();
}

#endif //__PROPERTIES_H__
