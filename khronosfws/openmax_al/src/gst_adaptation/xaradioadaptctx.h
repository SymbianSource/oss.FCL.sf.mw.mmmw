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

#ifndef XARADIOADAPTCTX_H
#define XARADIOADAPTCTX_H

#include "xaadaptationgst.h"

typedef enum {
  GST_AUDIO_TEST_SRC_WAVE_SINE,
  GST_AUDIO_TEST_SRC_WAVE_SQUARE,
  GST_AUDIO_TEST_SRC_WAVE_SAW,
  GST_AUDIO_TEST_SRC_WAVE_TRIANGLE,
  GST_AUDIO_TEST_SRC_WAVE_SILENCE,
  GST_AUDIO_TEST_SRC_WAVE_WHITE_NOISE,
  GST_AUDIO_TEST_SRC_WAVE_PINK_NOISE,
  GST_AUDIO_TEST_SRC_WAVE_SINE_TAB,
  GST_AUDIO_TEST_SRC_WAVE_TICKS
} GstAudioTestSrcWave;

typedef enum
{
    XA_RADIO_IDLE,
    XA_RADIO_SEEKING,
    XA_RADIO_SETTING_FREQUENCY,
    XA_RADIO_SETTING_FREQUENCY_RANGE
} XARadioState;

typedef enum
{
    XA_RADIO_RDS_IDLE,
    XA_RADIO_RDS_GETTING_ODA_GROUP
} XARadioRDSState;

#define RADIO_DEFAULT_FREQ_RANGE XA_FREQRANGE_FMEUROAMERICA
#define RADIO_DEFAULT_FREQ 88000000

/* TYPEDEFS */
typedef struct XARadioAdaptationCtx_ XARadioAdaptationCtx;

/*
 * Structure for Camera specific gst-adaptation variables
 */
typedef struct XARadioAdaptationCtx_ {

    /* Parent*/
    XAAdaptationGstCtx_ baseObj;

    /* OMX-AL Variables */
    XAuint32 frequency;
    XAuint8 range;

    pthread_t emulationThread;
    pthread_t rdsEmulationThread;
    XARadioRDSState rdsState;
    XARadioState state;

    /* GST Variables */
} XARadioAdaptationCtx_;

/* FUNCTIONS */
XAAdaptationBaseCtx* XARadioAdapt_Create(void);
XAresult XARadioAdapt_PostInit(XAAdaptationBaseCtx* bCtx);
void XARadioAdapt_Destroy(XAAdaptationBaseCtx* bCtx);

#endif /* XARADIOADAPTCTX_H */
