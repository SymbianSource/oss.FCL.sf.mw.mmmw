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

#ifndef XAVIBRAITF_H
#define XAVIBRAITF_H

#include "xaadptbasectx.h"

/** MACROS **/
#define MIN_INTENSITY 0
#define MAX_INTENSITY 1000
#define MIN_FREQUENCY 0x00000001
#define MAX_FREQUENCY 0xFFFFFFFF
/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/

/* Definition of XAXAVibraItfItf implementation */
typedef struct XAVibraItfImpl_
{
   /* parent interface */
    struct XAVibraItf_ itf;
    /* pointer to self */
    struct XAVibraItfImpl_* self;

    /* variables */
    XAboolean       vibrate;
    XAmilliHertz    frequency;
    XApermille      intensity;

    /*Adaptation variables*/
    XAAdaptationBaseCtx *adapCtx;

} XAVibraItfImpl;

/** METHODS **/

/* Base interface XAVibraItf implementation
 * See API Specification for method documentation
*/
XAresult XAVibraItfImpl_Vibrate ( XAVibraItf self, XAboolean vibrate );
XAresult XAVibraItfImpl_IsVibrating ( XAVibraItf self, XAboolean * pVibrating );
XAresult XAVibraItfImpl_SetFrequency ( XAVibraItf self, XAmilliHertz frequency );
XAresult XAVibraItfImpl_GetFrequency ( XAVibraItf self, XAmilliHertz * pFrequency );
XAresult XAVibraItfImpl_SetIntensity ( XAVibraItf self, XApermille intensity );
XAresult XAVibraItfImpl_GetIntensity ( XAVibraItf self, XApermille * pIntensity );

 
/* XAVibraItfImpl -specific methods*/
XAVibraItfImpl* XAVibraItfImpl_Create( XAAdaptationBaseCtx *adapCtx );

void XAVibraItfImpl_Free(XAVibraItfImpl* self);
#endif /* XAVIBRAITF_H */
