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

#ifndef XAEQUALIZERITF_H
#define XAEQUALIZERITF_H

#include "openmaxalwrapper.h"
#include "xaglobals.h"
#ifdef _GSTREAMER_BACKEND_   
#include "XAAdaptationContextBase.h"
#include "XAEqualizerItfAdaptation.h"
#endif
/** MACROS **/
/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XAEqualizerItf implementation */
typedef struct XAEqualizerItfImpl_
{
    /* parent interface */
    struct XAEqualizerItf_ itf;
    /* pointer to self */
    struct XAEqualizerItfImpl_* self;

    /* variables */
    XAboolean   enabled;
    XAuint16    preset;
#ifdef _GSTREAMER_BACKEND_   
    XAmillibel  levels[EQUALIZER_NUM_OF_BANDS];
    XAboolean   changeLevel[EQUALIZER_NUM_OF_BANDS];
    /*Adaptation variables*/
    XAAdaptationBaseCtx *adapCtx;
#endif
} XAEqualizerItfImpl;

/** METHODS **/

/* Base interface XAEqualizerItf implementation */
XAresult XAEqualizerItfImpl_SetEnabled(XAEqualizerItf self,XAboolean enabled);

XAresult XAEqualizerItfImpl_IsEnabled(XAEqualizerItf self, XAboolean *pEnabled);

XAresult XAEqualizerItfImpl_GetNumberOfBands(XAEqualizerItf self,
                                             XAuint16 *pNumBands);

XAresult XAEqualizerItfImpl_GetBandLevelRange(XAEqualizerItf self,
                                              XAmillibel *pMin,
                                              XAmillibel *pMax);

XAresult XAEqualizerItfImpl_SetBandLevel(XAEqualizerItf self, XAuint16 band,
                                         XAmillibel level);

XAresult XAEqualizerItfImpl_GetBandLevel(XAEqualizerItf self, XAuint16 band,
                                         XAmillibel *pLevel);

XAresult XAEqualizerItfImpl_GetCenterFreq(XAEqualizerItf self, XAuint16 band,
                                          XAmilliHertz *pCenter);

XAresult XAEqualizerItfImpl_GetBandFreqRange(XAEqualizerItf self, XAuint16 band,
                                             XAmilliHertz *pMin,
                                             XAmilliHertz *pMax);

XAresult XAEqualizerItfImpl_GetBand(XAEqualizerItf self, XAmilliHertz frequency,
                                    XAuint16 *pBand);

XAresult XAEqualizerItfImpl_GetCurrentPreset(XAEqualizerItf self,
                                             XAuint16 *pPreset);

XAresult XAEqualizerItfImpl_UsePreset(XAEqualizerItf self, XAuint16 index);

XAresult XAEqualizerItfImpl_GetNumberOfPresets(XAEqualizerItf self,
                                               XAuint16 *pNumPresets);

XAresult XAEqualizerItfImpl_GetPresetName(XAEqualizerItf self, XAuint16 index,
                                          const XAchar **ppName);

#ifdef _GSTREAMER_BACKEND_  
/* XAEqualizerItfImpl -specific methods */
XAEqualizerItfImpl* XAEqualizerItfImpl_Create(XAAdaptationBaseCtx *adapCtx);
#endif
void XAEqualizerItfImpl_Free(XAEqualizerItfImpl* self);

#endif /* XAEQUALIZERITF_H */
