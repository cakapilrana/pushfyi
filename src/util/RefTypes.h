/*
*********************************************************
        This code is a proprietary of iN Technologies

Following may not be used in part on in whole without the
prior permission of iN Technologies

Author: Kapil Rana
Date: 18/05/2015
Purpose: Definition of what all may be used for ref count

*********************************************************
*/
#ifndef __REFTYPES_H__
#define __REFTYPES_H__

enum eRefType {
    eTUnknown, 

    eTEvent, 
    eTEventList, 
    eTEventIter, 

    eTBool, 
    eTInt, 
    eTUInt, 
    eTLong, 
    eTULong, 
    eTFloat, 
    eTDouble, 
    eTString, 
    eTRefVar, 

    eTEPCTag, 
    eTECSpec, 
    eTECReports, 
    eTCalendar, 
    eTRoutes, 
    eTXMLDocument, 
    eTXMLElement
};

#endif //__REFTYPES_H__
