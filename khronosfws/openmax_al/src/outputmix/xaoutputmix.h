/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/

#ifndef XAOUTPUTMIX_H
#define XAOUTPUTMIX_H

#include "openmaxalwrapper.h"
#include "xaobjectitf.h"
#include "xaglobals.h"
#include "xaadptbasectx.h"

/** MACROS **/


/** TYPES **/


/** ENUMERATIONS **/
/* Enumeration for interfaces that OutputMix supports.  */
typedef enum
{
    OMIX_OBJECTITF,
    OMIX_DIMITF,
    OMIX_CONFIGEXTENSIONITF,
    OMIX_OUTPUTMIXITF,
    OMIX_EQUALIZERITF,
    OMIX_VOLUMEITF,
    OMIX_ITFCOUNT
} XAOMixInterfaces;


/** STRUCTURES **/
/* Specification for  MediaPlayerImpl.*/
typedef struct XAOMixImpl_
{
    /* Parent for XAMediaPlayerImpl */
    XAObjectItfImpl baseObj;

    /* variables */
    XAAdaptationBaseCtx* adaptationCtx;

} XAOMixImpl;


/** METHODS **/

/* base object XAObjectItfImpl methods */
XAresult XAOMixImpl_DoRealize(XAObjectItf self);
XAresult XAOMixImpl_DoResume(XAObjectItf self);
void XAOMixImpl_FreeResources(XAObjectItf self);

/* XAOMixImpl -specific methods*/
XAresult XAOMixImpl_DoAddItf(XAObjectItf self, XAObjItfMapEntry *mapEntry  );
XAresult XAOMixImpl_DoResumeItf(XAObjectItf self, XAObjItfMapEntry *mapEntry  );
XAresult XAOMixImpl_DoRemoveItf(XAObjectItf self, XAObjItfMapEntry *mapEntry );

#endif /* XAOUTPUTMIX_H */
