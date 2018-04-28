/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 20/06/2015
Purpose: Event Object's "params" tag Wrapper

*********************************************************
*/
#ifndef __PARAMS_H__
#define __PARAMS_H__

#include <string>
#include "STLHelpers.h" // should be removed, but breaks build
#include "Var.h"
#include "VectorMap.h"

typedef VectorMap<std::string, Var> Hashtable;
typedef Hashtable::iterator HashtableIter;

class Params
{
public:
    typedef Hashtable::iterator iterator;
    typedef Hashtable::const_iterator const_iterator;

    Params();
    Params(const Params& toCopy);
    virtual ~Params();

    enum ESortType {
        eUnsorted,
        eAscending,
        eDescending
    };

    Params& operator=(const Params& toCopy);
    bool operator==(const Params& toCompare) const;
    std::string toString() const;

    std::vector<std::string> getKeys(const ESortType sortType = eUnsorted) const;

    std::string get(const std::string& key, const std::string& def = RX_ESREF) const;
    void set(const std::string& key, const std::string& value);

    std::string getString(const std::string& key, const std::string& def = RX_ESREF) const;
    void setString(const std::string& key, const std::string& value);

    rxUInt getUInt(const std::string& key, rxUInt def = 0) const;
    void setUInt(const std::string& key, const rxUInt value);

    rxInt getInt(const std::string& key, rxInt def = 0) const;
    void setInt(const std::string& key, const rxInt value);

    rxULong getULong(const std::string& key, rxULong def = 0LL) const;
    void setULong(const std::string& key, const rxULong value);

    rxLong getLong(const std::string& key, rxLong def = 0LL) const;
    void setLong(const std::string& key, const rxLong value);

    rxFloat getFloat(const std::string& key, rxFloat def = 0.0) const;
    void setFloat(const std::string& key, const rxFloat value);

    rxDouble getDouble(const std::string& key, rxDouble def = 0.0) const;
    void setDouble(const std::string& key, const rxDouble value);

    bool getBool(const std::string& key, bool def = false) const;
    void setBool(const std::string& key, const bool value);

    void* getObject(const std::string& key, void* def = 0) const;
    void setObject(const std::string& key, void* value);

    Ref::Rep* getRef(const std::string& key, Ref::Rep* def = 0) const;
    void setRef(const std::string& key, Ref::Rep* value);

    Var& getVar(const std::string& key);
    Var& getVar(const std::string& key, Var& def);
    void setVar(const std::string& key, const Var& value);

    bool contains(const std::string& key) const;
    void remove(const std::string& key);
    rxUInt size() const;
    void clear();

    enum EMergePrefixOption {
        eMergeWithPrefix = 1,
        ePrefixKeepping  = 2,
        eOverrideExisting= 4
    };
    /*
     * set values from another Params
     * A = {a = 1, b.c=2, b.d='a'} B = {d = 1, e=2}
     * after B.merge(A, "b.");
     * B = {c=2, d='a', e=2}
     */
    void merge(const Params& from, const std::string& prefix, rxUInt o = 0);
    void merge(const Params& from, const char* prefix=0, rxUInt option = 0);

    HashtableIter begin();
    HashtableIter end();
    Hashtable::const_iterator begin() const;
    Hashtable::const_iterator end() const;
protected:
    Hashtable mData;

private:
    static Var NULL_VAR;
};

inline Params& Params::operator=(const Params& toCopy)
{
    mData = toCopy.mData;
    return *this;
}

inline std::string Params::getString(const std::string& key, const std::string& def) const
{
    return get(key, def);
}

inline void Params::setString(const std::string& key, const std::string& value)
{
    set(key, value);
}

inline rxUInt Params::size() const
{
    return mData.size();
}

inline void Params::clear()
{
    mData.clear();
}

inline HashtableIter Params::begin()
{
    return mData.begin();
}

inline HashtableIter Params::end()
{
    return mData.end();
}

inline Hashtable::const_iterator Params::begin() const
{
    return mData.begin();
}

inline Hashtable::const_iterator Params::end() const
{
    return mData.end();
}

inline void Params::merge(const Params& from, const std::string& prefix, rxUInt o)
{
	merge(from, prefix.c_str(), o);
}

#endif //__PARAMS_H__
