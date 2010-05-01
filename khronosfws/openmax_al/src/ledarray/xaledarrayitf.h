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

#ifndef XALEDARRAYITF_H
#define XALEDARRAYITF_H

#include "xaadptbasectx.h"

/** MACROS **/
#define LED_COUNT 1
#define PRIMARY_LED 0
#define COLOR_MASK 1

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XALEDArrayItf implementation */
typedef struct XALEDArrayItfImpl_
{
    /* parent interface */
    struct XALEDArrayItf_ itf;
    /* pointer to self */
    struct XALEDArrayItfImpl_* self;

    /* variables */
    XAuint32    lightMask;
    XAHSL       color;

 
    /*Adaptation variables*/
    XAAdaptationBaseCtx *adapCtx;

} XALEDArrayItfImpl;

/** METHODS **/

/* Base interface XALEDArrayItf implementation
 *See API Specification for method documentation
 */
XAresult XALEDArrayItfImpl_ActivateLEDArray ( XALEDArrayItf self, XAuint32 lightMask );
XAresult XALEDArrayItfImpl_IsLEDArrayActivated ( XALEDArrayItf self, XAuint32 * pLightMask );
XAresult XALEDArrayItfImpl_SetColor ( XALEDArrayItf self, XAuint8 index, const XAHSL * pColor );
XAresult XALEDArrayItfImpl_GetColor ( XALEDArrayItf self, XAuint8 index, XAHSL * pColor );
/* XALEDArrayItfImpl -specific methods */
 
XALEDArrayItfImpl* XALEDArrayItfImpl_Create( XAAdaptationBaseCtx *adapCtx );
void XALEDArrayItfImpl_Free(XALEDArrayItfImpl* self);


#endif /* XALEDARRAYITF_H */
