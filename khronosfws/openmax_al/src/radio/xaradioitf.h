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

#ifndef XARADIOITF_H
#define XARADIOITF_H

#include "openmaxalwrapper.h"
#include "xaglobals.h"
#ifdef _GSTREAMER_BACKEND_
#include "XAAdaptationContextBase.h"
#include "XARadioItfAdaptation.h"
#endif
/** MACROS **/
#define RADIO_PRESET_NAME_MAX_LENGTH 20

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/

/* Definition of radio preset */
typedef struct RadioPreset_
{
    XAuint32    freq;
    XAuint8     range;
    XAuint32    stereoMode;
    char*       name;
} RadioPreset;

/* Definition of XAEqualizerItf implementation */
typedef struct XARadioItfImpl_
{
    /* parent interface */
    struct XARadioItf_ itf;
    /* pointer to self */
    struct XARadioItfImpl_* self;

    /* variables */
    XAboolean   squelch;
    XAuint32    stereoMode;

    XAuint32    preset;
    XAuint32    numOfPresets;

#ifdef _GSTREAMER_BACKEND_
    RadioPreset presets[RADIO_NUM_OF_PRESETS];
#endif
    XARadioItf         cbPtrToSelf;
    xaRadioCallback    callback;
    void               *context;

#ifdef _GSTREAMER_BACKEND_
    /*Adaptation variables*/
    XAAdaptationBaseCtx *adapCtx;
#endif
} XARadioItfImpl;

/** METHODS **/

/* Base interface XARadioItf implementation */
XAresult XARadioItfImpl_SetFreqRange(XARadioItf self, XAuint8 range);

XAresult XARadioItfImpl_GetFreqRange(XARadioItf self, XAuint8 * pRange);

XAresult XARadioItfImpl_IsFreqRangeSupported(XARadioItf self,
                                             XAuint8 range,
                                             XAboolean * pSupported);

XAresult XARadioItfImpl_GetFreqRangeProperties(XARadioItf self,
                                               XAuint8 range,
                                               XAuint32 * pMinFreq,
                                               XAuint32 * pMaxFreq,
                                               XAuint32 * pFreqInterval);

XAresult XARadioItfImpl_SetFrequency(XARadioItf self, XAuint32 freq);

XAresult XARadioItfImpl_CancelSetFrequency(XARadioItf self);

XAresult XARadioItfImpl_GetFrequency(XARadioItf self, XAuint32 * pFreq);

XAresult XARadioItfImpl_SetSquelch(XARadioItf self, XAboolean squelch);

XAresult XARadioItfImpl_GetSquelch(XARadioItf self, XAboolean * pSquelch);

XAresult XARadioItfImpl_SetStereoMode(XARadioItf self, XAuint32 mode);

XAresult XARadioItfImpl_GetStereoMode(XARadioItf self, XAuint32 * pMode);

XAresult XARadioItfImpl_GetSignalStrength(XARadioItf self, XAuint32 * pStrength);

XAresult XARadioItfImpl_Seek(XARadioItf self, XAboolean upwards);

XAresult XARadioItfImpl_StopSeeking(XARadioItf self);

XAresult XARadioItfImpl_GetNumberOfPresets(XARadioItf self, XAuint32 * pNumPresets);

XAresult XARadioItfImpl_SetPreset(XARadioItf self,
                                  XAuint32 preset,
                                  XAuint32 freq,
                                  XAuint8 range,
                                  XAuint32 mode,
                                  const XAchar * name);

XAresult XARadioItfImpl_GetPreset(XARadioItf self,
                                  XAuint32 preset,
                                  XAuint32 * pFreq,
                                  XAuint8 * pRange,
                                  XAuint32 * pMode,
                                  XAchar * pName,
                                  XAuint16 * pNameLength);

XAresult XARadioItfImpl_RegisterRadioCallback(XARadioItf self,
                                              xaRadioCallback callback,
                                              void * pContext);
#ifdef _GSTREAMER_BACKEND_

/* XARadioItfImpl -specific methods */
XARadioItfImpl* XARadioItfImpl_Create(XAAdaptationBaseCtx *adapCtx);
void XARadioItfImpl_AdaptCb( void *pHandlerCtx, XAAdaptEvent *event );
#endif
void XARadioItfImpl_Free(XARadioItfImpl* self);

#endif /* XARADIOITF_H */
