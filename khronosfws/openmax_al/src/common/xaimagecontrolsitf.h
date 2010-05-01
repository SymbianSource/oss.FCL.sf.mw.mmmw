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

#ifndef XAIMAGECONTROLSITF_H
#define XAIMAGECONTROLSITF_H

#include "xaadptbasectx.h"


/** MACROS **/
#define DEFAULT_BRIGHTNESS_VALUE    50
#define DEFAULT_CONTRAST_VALUE      0
#define DEFAULT_GAMMA_VALUE         1000
#define MAX_BRIGHTNESS_VALUE        100
#define MAX_CONTRAST_VALUE          100
#define MIN_CONTRAST_VALUE         -100

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XAImageControlsItf implementation */
typedef struct XAImageControlsItfImpl_
{
    /* parent interface */
    struct XAImageControlsItf_ itf;
    /* pointer to self */
    struct XAImageControlsItfImpl_* self;

    /* variables */
    XAuint32    brightness;
    XAint32     contrast;
    XApermille  gamma;

    /*Adaptation variables*/
    XAAdaptationBaseCtx *adapCtx;
   

} XAImageControlsItfImpl;

/** METHODS **/

XAresult XAImageControlsItfImpl_SetBrightness(XAImageControlsItf self,
                                              XAuint32 brightness);

XAresult XAImageControlsItfImpl_GetBrightness(XAImageControlsItf self,
                                              XAuint32 *pBrightness);

XAresult XAImageControlsItfImpl_SetContrast(XAImageControlsItf self,
                                            XAint32 contrast);

XAresult XAImageControlsItfImpl_GetContrast(XAImageControlsItf self,
                                            XAint32 *pContrast);

XAresult XAImageControlsItfImpl_SetGamma(XAImageControlsItf self,
                                         XApermille gamma);

XAresult XAImageControlsItfImpl_GetGamma(XAImageControlsItf self,
                                         XApermille *pGamma);

XAresult XAImageControlsItfImpl_GetSupportedGammaSettings(XAImageControlsItf self,
                                                          XApermille *pMinValue,
                                                          XApermille *pMaxValue,
                                                          XAuint32 *pNumSettings,
                                                          XApermille **ppSettings);

/* XAImageControlsItfImpl -specific methods */
XAImageControlsItfImpl* XAImageControlsItfImpl_Create(XAAdaptationBaseCtx *adapCtx);
void XAImageControlsItfImpl_Free(XAImageControlsItfImpl* self);

#endif /* XAIMAGECONTROLSITF_H */
