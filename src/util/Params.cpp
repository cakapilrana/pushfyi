/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: Event Object's "params" tag Wrapper

*********************************************************
*/
#include <algorithm>
#include <string.h>
#include "Params.h"
#include "Bool.h"
#include "Double.h"
#include "Float.h"
#include "Int.h"
#include "Long.h"
#include "RefVar.h"
#include "String.h"
#include "STLHelpers.h"
#include "UInt.h"
#include "ULong.h"

using namespace std;

Var Params::NULL_VAR = 0;

Params::Params() {
	// nop
}

Params::Params(const Params& toCopy) :
	mData(toCopy.mData) {
	// nop
}

Params::~Params() {
	// nop
}

vector<string> Params::getKeys(const ESortType sortType) const {
	Hashtable::const_iterator iter = mData.begin();
	vector<string> result;

	for (; iter != mData.end(); iter++) {
		result.push_back(iter->first);
	}

	if (sortType == eAscending) {
		sort(result.begin(), result.end(), StringUtil::stringLT);
	} else if (sortType == eDescending) {
		sort(result.begin(), result.end(), StringUtil::stringGT);
	}

	return result;
}

string Params::get(const string& key, const string& def) const {
	Hashtable::const_iterator iter = mData.find(key);

	if (iter != mData.end() && !(*iter).second.isNull()) {
		return (*iter).second->toString();
	}

	return def;
}

void Params::set(const string& key, const string& value) {
	mData[key] = new StringRep(value);
}

rxUInt Params::getUInt(const string& key, rxUInt def) const {
	Hashtable::const_iterator iter = mData.find(key);

	if (iter != mData.end() && !(*iter).second.isNull()) {
		return (*iter).second->toUInt();
	}

	return def;
}

void Params::setUInt(const string& key, const rxUInt value) {
	mData[key] = new UIntRep(value);
}

rxInt Params::getInt(const string& key, rxInt def) const {
	Hashtable::const_iterator iter = mData.find(key);

	if (iter != mData.end() && !(*iter).second.isNull()) {
		return (*iter).second->toInt();
	}

	return def;
}

void Params::setInt(const string& key, const rxInt value) {
	mData[key] = new IntRep(value);
}

rxULong Params::getULong(const string& key, rxULong def) const {
	Hashtable::const_iterator iter = mData.find(key);

	if (iter != mData.end() && !(*iter).second.isNull()) {
		return (*iter).second->toULong();
	}

	return def;
}

void Params::setULong(const string& key, const rxULong value) {
	mData[key] = new ULongRep(value);
}

rxLong Params::getLong(const string& key, rxLong def) const {
	Hashtable::const_iterator iter = mData.find(key);

	if (iter != mData.end() && !(*iter).second.isNull()) {
		return (*iter).second->toLong();
	}

	return def;
}

void Params::setLong(const string& key, const rxLong value) {
	mData[key] = new LongRep(value);
}

rxFloat Params::getFloat(const string& key, rxFloat def) const {
	Hashtable::const_iterator iter = mData.find(key);

	if (iter != mData.end() && !(*iter).second.isNull()) {
		return (*iter).second->toFloat();
	}

	return def;
}

void Params::setFloat(const string& key, const rxFloat value) {
	mData[key] = new FloatRep(value);
}

rxDouble Params::getDouble(const string& key, rxDouble def) const {
	Hashtable::const_iterator iter = mData.find(key);

	if (iter != mData.end() && !(*iter).second.isNull()) {
		return (*iter).second->toDouble();
	}

	return def;
}

void Params::setDouble(const string& key, const rxDouble value) {
	mData[key] = new DoubleRep(value);
}

bool Params::getBool(const string& key, bool def) const {
	Hashtable::const_iterator iter = mData.find(key);

	if (iter != mData.end() && !(*iter).second.isNull()) {
		return (*iter).second->toBool();
	}

	return def;
}

void Params::setBool(const string& key, const bool value) {
	mData[key] = new BoolRep(value);
}

void* Params::getObject(const string& key, void* def) const {
	//rxUInt value = getUInt(key, 0);
	rxULong value = getULong(key, 0);

	if (value == 0) {
		return def;
	}

	return (void*) value;
}

void Params::setObject(const string& key, void* value) {
	//setUInt(key, (rxUInt) value);
	setULong(key, (rxULong) value);
}

Ref::Rep* Params::getRef(const string& key, Ref::Rep* def) const {
	Hashtable::const_iterator iter = mData.find(key);

	if (iter != mData.end() && !(*iter).second.isNull()) {
		return (*iter).second->toRep();
	}

	return def;
}

void Params::setRef(const string& key, Ref::Rep* value) {
	mData[key] = new RefVarRep(value);
}

Var& Params::getVar(const string& key) {
	Hashtable::iterator iter = mData.find(key);

	if (iter != mData.end()) {
		return (*iter).second;
	}

	return NULL_VAR;
}

Var& Params::getVar(const string& key, Var& def) {
	Hashtable::iterator iter = mData.find(key);

	if (iter != mData.end()) {
		return (*iter).second;
	}

	return def;
}

void Params::setVar(const string& key, const Var& value) {
	mData[key] = value;
}

bool Params::contains(const std::string& key) const {
	return mData.find(key) != mData.end();
}

void Params::remove(const std::string& key) {
	mData.erase(key);
}

bool Params::operator==(const Params& toCompare) const {
	if (mData.size() != toCompare.mData.size()) {
		return false;
	}
	bool ok = true;
	Hashtable::const_iterator i = mData.begin();
	for (; i != mData.end(); i++) {
		std::string key = (*i).first;
		Hashtable::const_iterator j = toCompare.mData.find(key);
		if (j == toCompare.mData.end() || (*i).second->toString()
				!= (*j).second->toString()) {
			ok = false;
			break;
		}
	}
	return ok;
}

std::string Params::toString() const {
	string ret = "{";
	vector<string> keys = getKeys(eAscending);
	vector<string>::const_iterator i = keys.begin();
	for (; i != keys.end(); i++) {
		if (i != keys.begin()) {
			ret += ", ";
		}
		ret += (*i);
		Hashtable::const_iterator k = mData.find(*i);
		if (k != mData.end()) {
			ret += ": " + (*k).second->toString();
		}
	}
	ret += "}";
	return ret;
}

void Params::merge(const Params& from, const char* prefix, rxUInt o) {
	if (prefix == 0) {
		o &= ~eMergeWithPrefix;
		o &= ~ePrefixKeepping;
	}
	Hashtable::const_iterator i = from.begin();

	if (o & eMergeWithPrefix) {
		rxUInt len = strlen(prefix);

		for (; i != from.end(); ++i) {
			std::string key = (*i).first;
			if (key.find(prefix) != 0) {
				continue;
			}

			if (o & ePrefixKeepping) {

			} else {
				key = key.substr(len);
			}
			if (!contains(key) || (o & eOverrideExisting)) {
				mData[key] = (*i).second;
			}
		}
	}
	else {
		for (; i != from.end(); ++i) {
			std::string key = (*i).first;
			if (o & ePrefixKeepping) {
				if(::isalnum(*(prefix+strlen(prefix)-1))){
					key.insert(0, 1, '.');
				}
				key.insert(0, prefix);
			}
			if (!contains(key) || (o & eOverrideExisting)) {
				mData[key] = (*i).second;
			}
		}
	}
}
