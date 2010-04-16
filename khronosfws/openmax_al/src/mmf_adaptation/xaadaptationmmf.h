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

#ifndef XAADAPTATION_H_
#define XAADAPTATION_H_

#include <pthread.h>
#include <semaphore.h>
#include <garray.h>
#include <string.h>
#include <unistd.h>
#include "openmaxalwrapper.h"
#include "xaadaptationcontextbasemmf.h"
#include "xaglobals.h"
//#include "OpenMAXAL_ContentPipe.h"
#include "xaplatform.h"

#ifdef XA_IMPL_MEASURE_GST_DELAY
#include <time.h>
#endif /* XA_IMPL_MEASURE_GST_DELAY */

/* MACROS */
#define FLIP_NONE               0
#define FLIP_CLOCKWISE          1 /* Rotate clockwise 90 degrees */
#define FLIP_ROTATE_180         2 /* Rotate 180 degrees */
#define FLIP_COUNTERCLOCKWISE   3 /* Rotate counter-clockwise 90 degrees */
#define FLIP_HORIZONTAL         4 /* Flip image horizontally */
#define FLIP_VERTICAL           5 /* Flip image vertically */

/* TYPEDEFS */
//typedef gboolean (*GstBusCb)( GstBus *bus, GstMessage *message, gpointer data );
#define XA_IMPL_SUPPORTED_AUDIO_OUT_NUM 3
#define XA_IMPL_OMIX_MAX_CONNECTED_MEDIAPLAYERS 10
#define CONTENT_PIPE_BUFFER_SIZE 1000
#define TEST_VIDEO_WIDTH     640
#define TEST_VIDEO_HEIGHT    480

typedef enum
{
    XA_AUDIO_WAVENC = 0,
    XA_AUDIO_VORBISENC,
    XA_AUDIO_PCM,
    XA_NUM_OF_AUDIOENCODERS /* Do not move this line */
} XAAudioEnc;

typedef enum
{
    XA_VIDEO_JPEGENC = 0,
    XA_VIDEO_THEORAENC,
    XA_NUM_OF_VIDEOENCODERS /* Do not move this line */
} XAVideoEnc;

typedef enum CP_STATE
{
	CPStateNull =0,
	CPStateInitialized,
	CPStatePrerolling,
	CPStateStarted,
	CPStateRunning,
	CPStatePaused,
	CPStateStopped,
	CPStateWaitForData,
	CPStateEOS,
	CPStateError
}CP_STATE;

typedef struct XAAdaptEvtHdlrMMF_
{
    xaAdaptEventHandlerMMF handlerfunc;    /* function handling the callback */
    void               *handlercontext; /* context of handler */
    XAuint32            eventtypes;     /* what kind of events this handles */
} XAAdaptEvtHdlrMMF;

/*
 * Structure that holds all common variables for every
 * mmf-Adaptation context structures.
 */
typedef struct XAAdaptationBaseMMFCtx_
{
    /* Common Variables for all adaptation elements */
    GArray*         evtHdlrs;    /* array of event handlers */
    XAuint32        ctxId;
    XAboolean       placeholder;

} XAAdaptationBaseMMFCtx_;

typedef struct XAEngineAdaptationMMFCtx_
{
    /* Parent*/
    XAAdaptationBaseMMFCtx_    baseObj;

} XAEngineAdaptationMMFCtx_;

/*
 * Structure for Media Player specific gst-adaptation.
 */
typedef struct XAMediaPlayerAdaptationMMFCtx_
{
    /* Parent*/
    XAAdaptationBaseMMFCtx_    baseObj;

    /* OMX-AL Variables */
    XADataSource            *xaSource, *xaBankSrc;
    XADataSink              *xaAudioSink, *xaVideoSink, *xaLEDArray, *xaVibra;

    /* MMF elements */
    XAboolean               isobjsrc;    /*is source another XA object?*/

    XAboolean               isobjasink;   /*is audio sink another XA object?*/
    XAboolean               isobjvsink;   /*is video sink another XA object?*/

    XAboolean               mute;
    XAuint32                imageEffectID;
    XAboolean               isStereoPosition;
    XAmillidegree           curRotation;
    XAuint32                curMirror;

    XAint32                 buffering;

    /* internals */
    XAboolean               trackpositionenabled;


    XAboolean               loopingenabled;

    XAboolean		            cameraSinkSynced;
    void*                   mmfContext;


} XAMediaPlayerAdaptationMMFCtx_;

typedef struct XAMediaRecorderAdaptationMMFCtx_
{
    /* Parent*/
    XAAdaptationBaseMMFCtx_ baseObj;

    /* OMX-AL Variables */
    XADataSource            *xaAudioSource, *xaVideoSource;
    XADataSink              *xaSink;
    XAuint8                 recModes;

    /* GST elements */
    XAboolean               isobjsink;   /*is sink another XA object?*/
    XAboolean               isobjasrc;    /*is audio source another XA object?*/
    XAboolean               isobjvsrc;    /*is video source another XA object?*/
    XAboolean               encodingchanged;

    XAboolean               mute;
    XAuint32                imageEffectID;
    XAboolean               isStereoPosition;
    XAuint32                xaRecordState;
    XAmillidegree           curRotation;
    XAuint32                curMirror;
    XAboolean               isRecord;

    /* internals */
    XAboolean               trackpositionenabled;
    gboolean                runpositiontimer;

    XAImplThreadHandle      recordingEventThr;

    /* Variables for encoders */
    XAAudioEncoderSettings  audioEncSettings;
    XAVideoSettings         videoEncSettings;
    XAImageSettings         imageEncSettings;
    
    void*                   mmfContext;


} XAMediaRecorderAdaptationMMFCtx_;


/* FUNCTIONS */
/*
 * gboolean XAAdaptationBase_GstBusCb( GstBus *bus, GstMessage *message, gpointer data );
 * Default CallBack handler for gst-bus messages. This will be called if object specific callback is
 * not implemented.
 */


void XAAdaptationBaseMMF_SendAdaptEvents(XAAdaptationBaseMMFCtx* ctx, XAAdaptEventMMF* event);

//void XAAdaptationBase_PrepareAsyncWait(XAAdaptationBaseCtx* ctx);
//void XAAdaptationBase_StartAsyncWait(XAAdaptationBaseCtx* ctx);
//gboolean XAAdaptationBase_CancelAsyncWait(gpointer ctx);
//void XAAdaptationBase_CompleteAsyncWait(XAAdaptationBaseCtx* ctx);


//XAresult XAMediaPlayerAdapt_UpdatePositionCbTimer(XAMediaPlayerAdaptationCtx_* mCtx);

//XAresult XAMediaRecorderAdapt_ChangeEncoders( XAMediaRecorderAdaptationCtx_* mCtx );
//XAresult XAMediaRecorderAdapt_CheckCodec( XAMediaRecorderAdaptationCtx_* mCtx, XACapsType encType, XAuint32 encoderId );

//XAresult XAMetadataAdapt_TryWriteTags(XAAdaptationBaseCtx* mCtx, GstBin* binToWriteTo);
//void XAMetadataAdapt_FreeVars(XAMetadataAdaptVars *vars);

//GstElement* XAOutputMixAdapt_GetSink(XAAdaptationBaseCtx* bCtx);


//XAresult XAOutputMixAdapt_ConnectObject(XAAdaptationBaseCtx* omCtx, XAAdaptationBaseCtx* bCtx, GstElement* usedMix);
//XAresult XAOutputMixAdapt_DisconnectObject(XAAdaptationBaseCtx* omCtx, XAAdaptationBaseCtx* bCtx);
//void* XAAdaptationBase_ContentPipeScrThrFunc( void* arg);
//void* XAAdaptationBase_ContentPipeSinkThrFunc( void* arg);
//CPresult XAAdaptationBase_ContentPipeSrcCb(CP_EVENTTYPE eEvent, CPuint iParam);
//CPresult XAAdaptationBase_ContentPipeSinkCb(CP_EVENTTYPE eEvent, CPuint iParam);

#endif /* XAADAPTATION_H_ */
