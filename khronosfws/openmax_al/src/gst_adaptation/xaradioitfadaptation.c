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

#include <unistd.h>
#include <gst.h>
#include "xaadaptationgst.h"
#include "xaradioitfadaptation.h"
#include "xaradioadaptctx.h"

#define NUM_OF_SUPPORTED_RANGES 2
#define NUM_OF_CHANNELS 2
const char* RadioPresetDefaultName = "Default";

/* emulated frequency range data */
typedef struct FreqRange_
{
    XAuint8     id;
    XAuint32    minFreq;
    XAuint32    maxFreq;
    XAuint32    freqInterval;
    XAuint32    stereoMode;
} FreqRange;

/* emulated radio channel data */
typedef struct Channel_
{
    XAuint32    freq;
    XAuint32    strength;
} Channel;

/* emulated radio state */
typedef enum
{
    RADIO_STATE_ON_CHANNEL,
    RADIO_STATE_OFF_CHANNEL
} RadioState;

/* data structure to pass onto async functions */
typedef struct
{
    XAAdaptationGstCtx *bCtx;
    XAuint32             value;
} ThreadArgs;

/* emulation variables and constraints */
static const FreqRange supportedFreqRanges[NUM_OF_SUPPORTED_RANGES] = {
        { XA_FREQRANGE_FMEUROAMERICA, 87500000, 108000000, 100000, XA_STEREOMODE_MONO },
        {XA_FREQRANGE_FMJAPAN, 76000000, 90000000, 100000, XA_STEREOMODE_STEREO }};

static const Channel channels[NUM_OF_CHANNELS] = { {88500000, 80},
                                    {89300000, 60 }};

static XAuint8 freqRangeIndex = 0;
static XAboolean squelchMode = XA_BOOLEAN_FALSE;
static XAint32 currentChannel = -1;
static RadioState radioState = RADIO_STATE_OFF_CHANNEL;

static void* asyncdata = NULL;

/* FORWARD DECLARATIONS */
void * XARadioItfAdapt_AsyncSetFrequencyRange(void* args);
XAresult XARadioItfAdapt_SyncSetFrequencyRange(XAAdaptationGstCtx *bCtx, XAuint8 range);
void * XARadioItfAdapt_AsyncSetFrequency(void* args);
XAresult XARadioItfAdapt_SyncSetFrequency(XAAdaptationGstCtx *bCtx, XAuint32 freq);
void * XARadioItfAdapt_AsyncSeek(void* args);

/*
 * XAresult XARadioItfAdapt_SetFreqRange(XAAdaptationGstCtx *bCtx, XAuint8 range)
 */
XAresult XARadioItfAdapt_SetFreqRange(XAAdaptationGstCtx *bCtx, XAuint8 range)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAuint16 index=0;
    XARadioAdaptationCtx* mCtx = (XARadioAdaptationCtx*) bCtx;
    int pt_ret=0;
    DEBUG_API("->XARadioItfAdapt_SetFreqRange");

    if(!bCtx || range < XA_FREQRANGE_FMEUROAMERICA || range > XA_FREQRANGE_AMSW)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARadioItfAdapt_SetFreqRange");
        return XA_RESULT_PARAMETER_INVALID;
    }

    for (index = 0; index < NUM_OF_SUPPORTED_RANGES; index++)
    {
        if (supportedFreqRanges[index].id == range)
        {
            if (mCtx->range != range)
            {
                if (mCtx->state != XA_RADIO_IDLE)
                {
                  //  pthread_cancel(mCtx->emulationThread);
                    if (asyncdata)
                    {
                        free(asyncdata);
                        asyncdata = NULL;
                    }
                    mCtx->state = XA_RADIO_IDLE;
                }
                {
                    ThreadArgs* args = (ThreadArgs*)calloc(1, sizeof(ThreadArgs));
                    if (!args)
                    {
                        DEBUG_ERR("Calloc failed")
                        DEBUG_API("<-XARadioItfAdapt_SetFreqRange");
                        return XA_RESULT_INTERNAL_ERROR;
                    }
                    args->bCtx = bCtx;
                    args->value = range;
                    mCtx->state = XA_RADIO_SETTING_FREQUENCY_RANGE;
                    pt_ret = pthread_create(&(mCtx->emulationThread), NULL, (XARadioItfAdapt_AsyncSetFrequencyRange),(void*)args);

                    if(pt_ret)
                    {
                        DEBUG_ERR_A1("could not create thread!! (%d)",ret)
                        DEBUG_API("<-XARadioItfAdapt_SetFreqRange");
                        return XA_RESULT_INTERNAL_ERROR;
                    }
                }
            }
            break;
        }
    }

    DEBUG_API("<-XARadioItfAdapt_SetFreqRange");
    return ret;
}

/*
 * void * XARadioItfAdapt_AsyncSetFrequencyRange(void* args)
 */
void * XARadioItfAdapt_AsyncSetFrequencyRange(void* args)
{
    XAresult ret = XA_RESULT_SUCCESS;
    ThreadArgs* tArgs = NULL;
    XARadioAdaptationCtx* mCtx = NULL;

    DEBUG_API("->XARadioItfAdapt_AsyncSetFrequencyRange");
    asyncdata = args;
    tArgs = args;

    mCtx = (XARadioAdaptationCtx*) tArgs->bCtx;

    DEBUG_INFO("Setting frequency async")
    usleep(100000); /* 1/10 seconds */

    ret = XARadioItfAdapt_SyncSetFrequencyRange(tArgs->bCtx, tArgs->value);
    if (ret != XA_RESULT_SUCCESS)
    {
        DEBUG_ERR("XARadioItfAdapt_SyncSetFrequencyRange FAILED");
    }

    /* Send event to RadioItf */
    {
        XAAdaptEvent event = {XA_RADIOITFEVENTS, XA_ADAPT_RADIO_FREQUENCY_RANGE_CHANGED, 0, NULL};
        XAAdaptationBase_SendAdaptEvents(&tArgs->bCtx->baseObj, &event );
    }
    mCtx->state = XA_RADIO_IDLE;

    free(args);
    asyncdata = NULL;
    DEBUG_API("<-XARadioItfAdapt_AsyncSetFrequencyRange");
    pthread_exit(NULL);
    return NULL;
}

/*
 * XAresult XARadioItfAdapt_SyncSetFrequencyRange(XAAdaptationGstCtx *bCtx, XAuint8 range)
 */
XAresult XARadioItfAdapt_SyncSetFrequencyRange(XAAdaptationGstCtx *bCtx, XAuint8 range)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARadioAdaptationCtx* mCtx=NULL;
    XAuint8 index=0;
    DEBUG_API("->XARadioItfAdapt_SyncSetFrequencyRange");
    if(!bCtx)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARadioItfAdapt_SyncSetFrequencyRange");
        return XA_RESULT_PARAMETER_INVALID;
    }

    mCtx = (XARadioAdaptationCtx*) bCtx;

    if (range != mCtx->range)
    {
        DEBUG_INFO("Frequency range changed!")

        mCtx->range = range;
        for(index = 0; index < NUM_OF_SUPPORTED_RANGES; index++)
        {
            if (supportedFreqRanges[index].id == mCtx->range)
            {
                freqRangeIndex = index;
            }
        }

        ret = XARadioItfAdapt_SyncSetFrequency( bCtx, RADIO_DEFAULT_FREQ );
    }

    DEBUG_API("<-XARadioItfAdapt_SyncSetFrequencyRange");
    return ret;
}

/*
 * XAresult XARadioItfAdapt_GetFreqRange(XAAdaptationGstCtx *bCtx,
 *                                       XAuint8 * pFreqRange)
 */
XAresult XARadioItfAdapt_GetFreqRange(XAAdaptationGstCtx *bCtx,
                                      XAuint8 * pFreqRange)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARadioAdaptationCtx* mCtx=NULL;
    DEBUG_API("->XARadioItfAdapt_GetFreqRange");

    if(!bCtx || !pFreqRange)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARadioItfAdapt_GetFreqRange");
        return XA_RESULT_PARAMETER_INVALID;
    }

    mCtx = (XARadioAdaptationCtx*) bCtx;

    *pFreqRange = mCtx->range;

    DEBUG_API("<-XARadioItfAdapt_GetFreqRange");
    return ret;
}



/*
 *  XAresult XARadioItfAdapt_IsFreqRangeSupported(XAAdaptationGstCtx *bCtx,
 *                                                XAuint8 range,
 *                                                XAboolean * pSupported)
 */
XAresult XARadioItfAdapt_IsFreqRangeSupported(XAAdaptationGstCtx *bCtx,
                                             XAuint8 range,
                                             XAboolean * pSupported)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAuint16 index=0;
    DEBUG_API("->XARadioItfAdapt_IsFreqRangeSupported");

    if(!bCtx || !pSupported || range < XA_FREQRANGE_FMEUROAMERICA || range > XA_FREQRANGE_AMSW)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARadioItfAdapt_IsFreqRangeSupported");
        return XA_RESULT_PARAMETER_INVALID;
    }

    *pSupported = XA_BOOLEAN_FALSE;
    for (index = 0; index < NUM_OF_SUPPORTED_RANGES; index++)
    {
        if (supportedFreqRanges[index].id == range)
        {
            *pSupported = XA_BOOLEAN_TRUE;
            break;
        }
    }

    DEBUG_API("<-XARadioItfAdapt_IsFreqRangeSupported");
    return ret;
}

/*
 * XAresult XARadioItfAdapt_GetFreqRangeProperties(XAAdaptationGstCtx *bCtx,
 *                                                 XAuint8 range,
 *                                                 XAuint32 * pMinFreq,
 *                                                 XAuint32 * pMaxFreq,
 *                                                 XAuint32 * pFreqInterval)
 */
XAresult XARadioItfAdapt_GetFreqRangeProperties(XAAdaptationGstCtx *bCtx,
                                               XAuint8 range,
                                               XAuint32 * pMinFreq,
                                               XAuint32 * pMaxFreq,
                                               XAuint32 * pFreqInterval)
{
    XAresult ret = XA_RESULT_SUCCESS;
    
    XAuint16 index=0;
    DEBUG_API("->XARadioItfAdapt_GetFreqRangeProperties");

    if(!bCtx)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARadioItfAdapt_GetFreqRangeProperties");
        return XA_RESULT_PARAMETER_INVALID;
    }

    for (index = 0; index < NUM_OF_SUPPORTED_RANGES; index++)
    {
        if (supportedFreqRanges[index].id == range)
        {
            *pMinFreq = supportedFreqRanges[index].minFreq;
            *pMaxFreq = supportedFreqRanges[index].maxFreq;
            *pFreqInterval = supportedFreqRanges[index].freqInterval;
            break;
        }
    }

    DEBUG_API("<-XARadioItfAdapt_GetFreqRangeProperties");
    return ret;
}

/*
 * XAresult XARadioItfAdapt_SetFrequency(XAAdaptationGstCtx *bCtx, XAuint32 freq)
 */
XAresult XARadioItfAdapt_SetFrequency(XAAdaptationGstCtx *bCtx, XAuint32 freq)
{
    XAresult ret = XA_RESULT_SUCCESS;
    int pt_ret=0;
    XARadioAdaptationCtx* mCtx=NULL;
    DEBUG_API("->XARadioItfAdapt_SetFrequency");

    if(!bCtx || freq < supportedFreqRanges[freqRangeIndex].minFreq
            || freq > supportedFreqRanges[freqRangeIndex].maxFreq
            || ((freq - supportedFreqRanges[freqRangeIndex].minFreq)
                    % supportedFreqRanges[freqRangeIndex].freqInterval) != 0)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARadioItfAdapt_SetFrequency");
        return XA_RESULT_PARAMETER_INVALID;
    }

    mCtx = (XARadioAdaptationCtx*) bCtx;

    if (freq != mCtx->frequency)
    {
        if (mCtx->state != XA_RADIO_IDLE)
        {
           // pthread_cancel(mCtx->emulationThread);
            if (asyncdata)
            {
                free(asyncdata);
                asyncdata = NULL;
            }
        }

        {
            ThreadArgs* args = (ThreadArgs*)calloc(1, sizeof(ThreadArgs));
            if (!args)
            {
                DEBUG_ERR("Calloc failed")
                DEBUG_API("<-XARadioItfAdapt_SetFrequency");
                return XA_RESULT_INTERNAL_ERROR;
            }
            args->bCtx = bCtx;
            args->value = freq;
            mCtx->state = XA_RADIO_SETTING_FREQUENCY;
            pt_ret = pthread_create(&(mCtx->emulationThread), NULL, (XARadioItfAdapt_AsyncSetFrequency),(void*)args);

            if(pt_ret)
            {
                DEBUG_ERR_A1("could not create thread!! (%d)",ret)
                DEBUG_API("<-XARadioItfAdapt_SetFrequency");
                return XA_RESULT_INTERNAL_ERROR;
            }
        }
    } /* else do nothing freq was same */

    DEBUG_API("<-XARadioItfAdapt_SetFrequency");
    return ret;
}

/*
 * void * XARadioItfAdapt_AsyncSetFrequency(void* args)
 */
void * XARadioItfAdapt_AsyncSetFrequency(void* args)
{
    ThreadArgs* tArgs = NULL;
    XARadioAdaptationCtx* mCtx = NULL;
    asyncdata = args;
    tArgs = args;

    mCtx = (XARadioAdaptationCtx*) tArgs->bCtx;
    DEBUG_API("->XARadioItfAdapt_AsyncSetFrequency");
    DEBUG_INFO("Setting frequency async")
    usleep(100000); /* 1/10 seconds */

    XARadioItfAdapt_SyncSetFrequency(tArgs->bCtx, tArgs->value);

    /* Send event to RadioItf */
    {
        XAAdaptEvent event = {XA_RADIOITFEVENTS, XA_ADAPT_RADIO_FREQUENCY_CHANGED, 0, NULL };
        XAAdaptationBase_SendAdaptEvents(&tArgs->bCtx->baseObj, &event );
    }
    mCtx->state = XA_RADIO_IDLE;

    free(args);
    asyncdata = NULL;
    DEBUG_API("<-XARadioItfAdapt_AsyncSetFrequency");
    pthread_exit(NULL);
    return NULL;
}

/*
 * XAresult XARadioItfAdapt_SyncSetFrequency(XAAdaptationGstCtx *bCtx, XAuint32 freq)
 */
XAresult XARadioItfAdapt_SyncSetFrequency(XAAdaptationGstCtx *bCtx, XAuint32 freq)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAuint8 index=0;
    XAboolean channelFound = XA_BOOLEAN_FALSE;
    XARadioAdaptationCtx* mCtx=NULL;

    DEBUG_API("->XARadioItfAdapt_SyncSetFrequency");

    if(!bCtx || freq < supportedFreqRanges[freqRangeIndex].minFreq
            || freq > supportedFreqRanges[freqRangeIndex].maxFreq
            || ((freq - supportedFreqRanges[freqRangeIndex].minFreq)
                    % supportedFreqRanges[freqRangeIndex].freqInterval) != 0)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARadioItfAdapt_SyncSetFrequency");
        return XA_RESULT_PARAMETER_INVALID;
    }

    mCtx = (XARadioAdaptationCtx*) bCtx;

    if (freq != mCtx->frequency)
    {
        for (index = 0; index < NUM_OF_CHANNELS; index++)
        {
            if (channels[index].freq == freq)
            {
                currentChannel = index;
                channelFound = XA_BOOLEAN_TRUE;
                break;
            }
        }
        if ( channelFound == XA_BOOLEAN_FALSE && radioState == RADIO_STATE_ON_CHANNEL)
        {
            if (squelchMode == XA_BOOLEAN_FALSE)
            {
                DEBUG_API("Channel not found, squelch mode off, set radio to white noise");
                g_object_set( G_OBJECT(bCtx->bin), "wave", GST_AUDIO_TEST_SRC_WAVE_WHITE_NOISE, NULL );
            }
            else
            {
                DEBUG_API("Channel not found, squelch mode off, set radio to silence");
                g_object_set( G_OBJECT(bCtx->bin), "wave", GST_AUDIO_TEST_SRC_WAVE_SILENCE, NULL );
            }
            radioState = RADIO_STATE_OFF_CHANNEL;
        }
        else if ( channelFound == XA_BOOLEAN_TRUE && radioState == RADIO_STATE_OFF_CHANNEL )
        {
            DEBUG_API("Channel found, set radio to sine wave");
            g_object_set( G_OBJECT(bCtx->bin), "wave", GST_AUDIO_TEST_SRC_WAVE_SINE, NULL );
            radioState = RADIO_STATE_ON_CHANNEL;
        }
        else
        {
            /* do nothing */
        }

        DEBUG_INFO("Frequency changed!")

        mCtx->frequency = freq;
    } /* else frequency not changed do nothing */

    DEBUG_API("<-XARadioItfAdapt_SyncSetFrequency");
    return ret;
}

/*
 * XAresult XARadioItfAdapt_GetFrequency(XAAdaptationGstCtx *bCtx,
 *                                       XAuint32 * pFrequency)
 */
XAresult XARadioItfAdapt_GetFrequency(XAAdaptationGstCtx *bCtx,
                                      XAuint32 * pFrequency)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARadioAdaptationCtx* mCtx=NULL;
    DEBUG_API("->XARadioItfAdapt_GetFrequency");

    if(!bCtx || !pFrequency)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARadioItfAdapt_GetFrequency");
        return XA_RESULT_PARAMETER_INVALID;
    }

    mCtx = (XARadioAdaptationCtx*) bCtx;

    *pFrequency = mCtx->frequency;

    DEBUG_API("<-XARadioItfAdapt_GetFrequency");
    return ret;
}


/*
 * XAresult XARadioItfAdapt_CancelSetFrequency(XAAdaptationGstCtx *bCtx)
 */
XAresult XARadioItfAdapt_CancelSetFrequency(XAAdaptationGstCtx *bCtx)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARadioAdaptationCtx* mCtx=NULL;
    DEBUG_API("->XARadioItfAdapt_CancelSetFrequency");

    if(!bCtx)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARadioItfAdapt_CancelSetFrequency");
        return XA_RESULT_PARAMETER_INVALID;
    }

    mCtx = (XARadioAdaptationCtx*) bCtx;


    if (mCtx->state == XA_RADIO_SEEKING)
    {
      //  pthread_cancel(mCtx->emulationThread);
        mCtx->state = XA_RADIO_IDLE;
    }

    DEBUG_API("<-XARadioItfAdapt_CancelSetFrequency");
    return ret;
}

/*
 * XAresult XARadioItfAdapt_SetSquelch(XAAdaptationGstCtx *bCtx, XAboolean squelch)
 */
XAresult XARadioItfAdapt_SetSquelch(XAAdaptationGstCtx *bCtx, XAboolean squelch)
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XARadioItfAdapt_SetSquelch");

    if(!bCtx)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARadioItfAdapt_SetSquelch");
        return XA_RESULT_PARAMETER_INVALID;
    }

    if (squelch != squelchMode)
    {
        if (radioState == RADIO_STATE_OFF_CHANNEL)
        {
            if (squelch == XA_BOOLEAN_FALSE)
            {
                DEBUG_API("Squelch untoggled, no channel, set radio to white noise")
                g_object_set( G_OBJECT(bCtx->bin), "wave", GST_AUDIO_TEST_SRC_WAVE_WHITE_NOISE, NULL );
            }
            else
            {
                DEBUG_API("Squelch toggled, no channel, set radio to silence")
                g_object_set( G_OBJECT(bCtx->bin), "wave", GST_AUDIO_TEST_SRC_WAVE_SILENCE, NULL );
            }
        }
        else
        {
            DEBUG_API("Squelch toggled, radio on channel, radio not changed")
        }
        squelchMode = squelch;
    } /* else do nothing as mode not changed */

    DEBUG_API("<-XARadioItfAdapt_SetSquelch");
    return ret;
}

/*
 * XAresult XARadioItfAdapt_SetStereoMode(XAAdaptationGstCtx *bCtx, XAuint32 mode)
 */
XAresult XARadioItfAdapt_SetStereoMode(XAAdaptationGstCtx *bCtx, XAuint32 mode)
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XARadioItfAdapt_SetStereoMode");

    if(!bCtx || mode > XA_STEREOMODE_AUTO )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARadioItfAdapt_SetStereoMode");
        return XA_RESULT_PARAMETER_INVALID;
    }

    switch ( mode )
    {
        case XA_STEREOMODE_MONO:
            /* always works */
            break;
        case XA_STEREOMODE_STEREO:
            if ( supportedFreqRanges[freqRangeIndex].stereoMode == XA_STEREOMODE_MONO )
            {
                DEBUG_ERR("StereoMode stereo unsupported");
                ret = XA_RESULT_FEATURE_UNSUPPORTED;
            }
            break;
        case XA_STEREOMODE_AUTO:
            if ( supportedFreqRanges[freqRangeIndex].stereoMode != XA_STEREOMODE_AUTO )
            {
                DEBUG_ERR("StereoMode auto unsupported");
                ret = XA_RESULT_FEATURE_UNSUPPORTED;
            }
            break;
        default :
            DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
            ret = XA_RESULT_INTERNAL_ERROR; /* shouldn't come here */
            break;
    }

    DEBUG_API("<-XARadioItfAdapt_SetStereoMode");
    return ret;
}

/*
 * XAresult XARadioItfAdapt_GetSignalStrength(XAAdaptationGstCtx *bCtx, XAuint32 * pStrength)
 */
XAresult XARadioItfAdapt_GetSignalStrength(XAAdaptationGstCtx *bCtx, XAuint32 * pStrength)
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XARadioItfAdapt_GetSignalStrength");

    if(!bCtx || !pStrength)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARadioItfAdapt_GetSignalStrength");
        return XA_RESULT_PARAMETER_INVALID;
    }

    if (currentChannel != -1)
    {
        *pStrength = channels[currentChannel].strength;
    }
    else
    {
        *pStrength = 0;
    }

    DEBUG_API("<-XARadioItfAdapt_GetSignalStrength");
    return ret;
}

/*
 * XAresult XARadioItfAdapt_Seek(XAAdaptationGstCtx *bCtx, XAboolean upwards)
 */
XAresult XARadioItfAdapt_Seek(XAAdaptationGstCtx *bCtx, XAboolean upwards)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARadioAdaptationCtx* mCtx = NULL;
    int pt_ret=0;
    DEBUG_API("->XARadioItfAdapt_Seek");

    if(!bCtx)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARadioItfAdapt_Seek");
        return XA_RESULT_PARAMETER_INVALID;
    }

    mCtx = (XARadioAdaptationCtx*) bCtx;

    if (mCtx->state != XA_RADIO_IDLE)
    {
       // pthread_cancel(mCtx->emulationThread);
        if (asyncdata)
        {
            free(asyncdata);
            asyncdata = NULL;
        }
        mCtx->state = XA_RADIO_IDLE;
    }

    mCtx->state = XA_RADIO_SEEKING;
    pt_ret = pthread_create(&(mCtx->emulationThread), NULL, (XARadioItfAdapt_AsyncSeek),(void*)bCtx);

    if(pt_ret)
    {
        DEBUG_ERR_A1("could not create thread!! (%d)",ret)
        DEBUG_API("<-XARadioItfAdapt_Seek");
        return XA_RESULT_INTERNAL_ERROR;
    }

    DEBUG_API("<-XARadioItfAdapt_Seek");
    return ret;
}

/*
 * void * XARadioItfAdapt_AsyncSeek(void* args)
 */
void * XARadioItfAdapt_AsyncSeek(void* args)
{
    XAresult ret = XA_RESULT_SUCCESS;

    XAAdaptationGstCtx *bCtx = (XAAdaptationGstCtx*)args;

    XARadioAdaptationCtx* mCtx = (XARadioAdaptationCtx*) bCtx;

    XAuint8 index=0;
    XAint8 currentChannel = 0;
    XAuint32 freq=0;
    mCtx->state = XA_RADIO_SEEKING;
    usleep(100000); /* 1/10 seconds */

    DEBUG_INFO("Seek done!")

    for( index = 0; index < NUM_OF_CHANNELS; index++)
    {
        if (channels[index].freq == mCtx->frequency)
        {
            currentChannel = index;
            break;
        }
    }

    freq = channels[(currentChannel + 1)%NUM_OF_CHANNELS].freq;

    ret = XARadioItfAdapt_SyncSetFrequency( bCtx, freq );
    if (ret != XA_RESULT_SUCCESS)
    {
        DEBUG_ERR("XARadioItfAdapt_SyncSetFrequency failed")
    }

    /* Send event to RadioItf */
    {
        XAAdaptEvent event = {XA_RADIOITFEVENTS, XA_ADAPT_RADIO_SEEK_COMPLETE, 0, NULL };
        XAAdaptationBase_SendAdaptEvents(&bCtx->baseObj, &event );
    }
    mCtx->state = XA_RADIO_IDLE;

    pthread_exit(NULL);
    return NULL;
}

/*
 * XAresult XARadioItfAdapt_StopSeeking(XAAdaptationGstCtx *bCtx)
 */
XAresult XARadioItfAdapt_StopSeeking(XAAdaptationGstCtx *bCtx)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARadioAdaptationCtx* mCtx = NULL;
    DEBUG_API("->XARadioItfAdapt_StopSeeking");

    if(!bCtx)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARadioItfAdapt_StopSeeking");
        return XA_RESULT_INTERNAL_ERROR;
    }

    mCtx = (XARadioAdaptationCtx*) bCtx;

    if (mCtx->state == XA_RADIO_SEEKING)
    {
        mCtx->state = XA_RADIO_IDLE;
      //  pthread_cancel(mCtx->emulationThread);

        /* just put some random non-channel freq */
        ret = XARadioItfAdapt_SyncSetFrequency( bCtx, channels[0].freq+supportedFreqRanges[freqRangeIndex].freqInterval);
        if (ret != XA_RESULT_SUCCESS)
        {
            ret = XA_RESULT_INTERNAL_ERROR;
        }
    }

    DEBUG_API("<-XARadioItfAdapt_StopSeeking");
    return ret;
}

/*
 * void XARadioItfAdapt_Free(XAAdaptationGstCtx *bCtx)
 */
void XARadioItfAdapt_Free(XAAdaptationBaseCtx *bCtx)
{
    XARadioAdaptationCtx* mCtx = (XARadioAdaptationCtx*) bCtx;
    DEBUG_API("->XARadioItfAdapt_Free");

    if (mCtx->state != XA_RADIO_IDLE)
    {
       // pthread_cancel(mCtx->emulationThread);
        if (asyncdata)
        {
            free(asyncdata);
            asyncdata = NULL;
        }
        mCtx->state = XA_RADIO_IDLE;
    }

    DEBUG_API("<-XARadioItfAdapt_Free");
}
