/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 20/06/2015
Purpose: Template Vector Map
*********************************************************
*/
#ifndef __VECTOR_MAP_H__
#define __VECTOR_MAP_H__

#include <vector>

namespace boost {
    namespace archive {
        class text_iarchive;
        class text_oarchive;
    };
};

template < typename T, typename U >
class VectorMap
{
public:
    typedef typename std::pair<T, U> pairtype;
    typedef typename std::vector<pairtype>::iterator iterator;
    typedef typename std::vector<pairtype>::const_iterator const_iterator;

    VectorMap() { mVector.reserve(20); }
    VectorMap(const std::vector<pairtype> & copy) { mVector = copy.mVector.clone(); }
    size_t size() const { return mVector.size(); }
    size_t capacity() const { return mVector.capacity(); }
    bool empty() const { return mVector.empty(); }
    void clear() { mVector.clear(); }
    iterator begin() { return mVector.begin(); }
    iterator end() { return mVector.end(); }
    const_iterator begin() const { return mVector.begin(); }
    const_iterator end() const { return mVector.end(); }
    iterator erase(const iterator& i) { return mVector.erase(i); }
    void serialize(boost::archive::text_iarchive& ar, const unsigned int /* version */)
    {
        ar >> mVector;
    }
    void serialize(boost::archive::text_oarchive& ar, const unsigned int /* version */)
    {
        ar << const_cast<const std::vector<pairtype>&>(mVector);
    }

    const_iterator find(const T& key) const
    {
        return const_cast<VectorMap*>(this)->find(key);
    }

    iterator find(const T& key)
    {
        iterator i = mVector.begin();
        iterator iend = mVector.end();
        for (; i != iend; ++i)
        {
            if ((*i).first == key)
            {
                return i;
            }
        }
        return iend;
    }

    U& operator[](const T& key)
    {
        iterator i = find(key);
        if (i != mVector.end())
        {
            return (*i).second;
        }
        U dummy;
        pairtype p(key, dummy);
        mVector.push_back(p);
        return mVector.back().second;
    }

    iterator erase(const T& key)
    {
        iterator i = find(key);

        if (i != mVector.end()) {
             i = mVector.erase(i);
        }
        return i;
    }

    bool contains(const T& key)
    {
    	const_iterator i = find(key);
    	const_iterator e = end();
    	return (i != e);
    }

private:
    std::vector<pairtype> mVector;
};


#endif //__VECTOR_MAP_H__
