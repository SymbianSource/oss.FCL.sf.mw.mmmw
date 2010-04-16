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
#include <string.h>
#include <unistd.h>
#include "openmaxalwrapper.h"
#include "XAAdaptationContextBase.h"
#include "XAGlobals.h"
#include <gst.h>
#include <gstappsrc.h>
#include <gstappsink.h>
#include <gstappbuffer.h>
#include <photography.h>
#include <OpenMAXAL_ContentPipe.h>
#include "XAPlatform.h"
#include "XAStaticCapsAdaptation.h"

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
typedef gboolean (*GstBusCb)( GstBus *bus, GstMessage *message, gpointer data );
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

typedef enum
{
    XA_IMAGE_JPEGENC = 0,
    XA_IMAGE_RAW,
    XA_NUM_OF_IMAGEENCODERS /* Do not move this line */
} XAImageEnc;

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
/* STRUCTURES */

typedef struct XAAdaptEvtHdlr_
{
    xaAdaptEventHandler handlerfunc;    /* function handling the callback */
    void               *handlercontext; /* context of handler */
    XAuint32            eventtypes;     /* what kind of events this handles */
} XAAdaptEvtHdlr;

/* Structure for content pipe thread context */
typedef struct XAAdaptCpThrCtx_
{
	XADataLocator_ContentPipe* pipe; 	/* pointer to content Pipe structure (Sink/Source) */
	CPhandle	dataHandle; 			/* Handle to content pipe data */
	CP_STATE	state;
	XAConfigExtensionCpKey cpConfig; 	/* enum value for CP using e.g. ReadBuffer or WriteBuffer */
	XAImplSemHandle stateSem;
	XAImplSemHandle mpStateSem;
	XAImplSemHandle mpPrerollStateSem;

	GstAppSrc	*appSrc; 				/* Pointer to appSrc where data is read from. Not owned */
	GstAppSink  *appSink; 				/* Pointer to AppSink where dats is fed. Not owned */
}XAAdaptCpThrCtx;

/* context to track buffer insufficient event */
typedef struct recordItfCtx_{
    XAImplSemHandle bufInsufficientSem;
    XAboolean buffer_insufficient;
}recodtItfCtx;

/*
 * Structure that holds all common variables for every
 * Gst-Adaptation context structures.
 */
typedef struct XAAdaptationBaseCtx_
{
    /* Common Variables for all adaptation elements */
    XAuint32        ctxId;
    GstState        binWantedState;   /** requested gst-bin target state **/
    GstElement      *bin;       /** Container for all gst elements **/

    GstBus          *bus;       /** Gst-bus where gst sends messages **/
    GMainLoop       *busloop;       /** Gst-bus listener loop **/
    pthread_t       busloopThr;
    GstBusCb        busCb;      /** Gst-Bus callback funtion*/

    GArray*         evtHdlrs;    /* array of event handlers */

    XAboolean       waitingasyncop;
    sem_t           semAsyncWait;
    guint           asynctimer;

    XAConfigExtensionCpKey cpConfig; /* enum value for CP using e.g. ReadBuffer or WriteBuffer */

	/* Content Pipe variables */
	XAImplThreadHandle	pipeSrcThr, pipeSinkThr;
	XAAdaptCpThrCtx		pipeSrcThrCtx, pipeSinkThrCtx;

#ifdef XA_IMPL_MEASURE_GST_DELAY
    clock_t     startTime;
    clock_t     endTime;
    double      diff;
#endif /*XA_IMPL_MEASURE_GST_DELAY*/
    // VASU MOD BEGINS
    XAboolean thread_launched;
    pthread_mutex_t ds_mutex;
    pthread_cond_t ds_condition;
    XAboolean cond_mutx_inited;
    // VASU MOD ENDS
    /* FUNCTIONS*/

} XAAdaptationBaseCtx_;

typedef struct XAEngineAdaptationCtx_
{
    /* Parent*/
    XAAdaptationBaseCtx_    baseObj;

} XAEngineAdaptationCtx_;

typedef struct XAMetadataAdaptVars_
{

    XAuint32        childcount;
    XAuint32        nodedepth;
    XAuint32        traversemode;

    GstTagList*     generaltags;
    GstStructure*   audiotags;
    GstStructure*   videotags;
    GstStructure**   currentchild; /*points to one of previous 3*/

} XAMetadataAdaptVars;


/*
 * Structure for Media Player specific gst-adaptation.
 */
typedef struct XAMediaPlayerAdaptationCtx_
{
    /* Parent*/
    XAAdaptationBaseCtx_    baseObj;

    /* OMX-AL Variables */
    XADataSource            *xaSource, *xaBankSrc;
    XADataSink              *xaAudioSink, *xaVideoSink, *xaLEDArray, *xaVibra;

    /* GST elements */
    GstElement              *source;
    XAboolean               isobjsrc;    /*is source another XA object?*/
    GstElement              *codecbin;
    GstElement              *audioppbin;
    GstElement              *videoppbin;
    GstElement				*filter;
    GstElement              *videoppBScrbin;
    GstElement              *inputSelector;
    GstPad                  *blackScrSrcPad;
    GstPad                  *videoScrSrcPad;
    GstPad                  *blackScrSinkPad;
    GstPad                  *videoScrSinkPad;
    GstElement              *audiosink;
    XAboolean               isobjasink;   /*is audio sink another XA object?*/
    GstElement              *videosink;
    XAboolean               isobjvsink;   /*is video sink another XA object?*/

    XAboolean               mute;
    XAuint32                imageEffectID;
    XAboolean               isStereoPosition;
    XAmillidegree           curRotation;
    XAuint32                curMirror;

    XAint32                 buffering;

    /* internals */
    XAboolean               trackpositionenabled;
    guint                   runpositiontimer;
    GSourceFunc             positionCb;
    gint64                  lastpos;

    XAboolean               loopingenabled;
    gint64                  loopstart;
    gint64                  loopend;

    gdouble                 playrate;
    guint32                 rateprops;

    XAboolean				cameraSinkSynced;

    XAMetadataAdaptVars     *metadatavars;

} XAMediaPlayerAdaptationCtx_;

typedef struct XASnapshotItfVars_
{

    /*User variables*/
    XAuint32        numpics;
    XAuint32        fps;
    XAboolean       freeze;
    gchar*          fnametemplate;
    XADataSink*     xaSink;

    /* internal variables */
    gboolean        waitforbuffer;
    gboolean        parsenegotiated;
    XAuint32        numpicstaken;

    GstBus*         ssbus;
    GstElement*     sspipeline;
    GstElement*     ssbuffersrc;
    GstElement*     ssparser;
    GstElement*     ssscaler;
    GstElement*     ssfilter;
    GstElement*     ssencoder;
    GstElement*     sstagger;
    GstElement*     sssink;
    gulong          sighandler;

    GstBuffer*      snapshotbuffer;

} XASnapshotItfVars;

typedef struct XAMediaRecorderAdaptationCtx_
{
    /* Parent*/
    XAAdaptationBaseCtx_ baseObj;

    /* OMX-AL Variables */
    XADataSource            *xaAudioSource, *xaVideoSource;
    XADataSink              *xaSink;
    XAuint8                 recModes;

    /* GST elements */
    GstElement              *datasink;
    XAboolean               isobjsink;   /*is sink another XA object?*/
    GstElement              *codecbin;
    GstElement              *audioppbin;
    GstElement              *audiofilter;
    GstElement              *videoppbin;
    GstElement              *videofilter;
    GstElement              *videoextract;
    GstElement              *audiosource;
    GstElement              *audioqueue;
    XAboolean               isobjasrc;    /*is audio source another XA object?*/
    GstElement              *videosource;
    GstState				vsrcOrigState;
    GstElement              *videoqueue;
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
    GSourceFunc             positionCb;

    XAImplThreadHandle      recordingEventThr;
    recodtItfCtx            recThrCtx;


    /* Variables for snapshot */
    XASnapshotItfVars       snapshotVars;

    XAMetadataAdaptVars     *metadatavars;

    /* Variables for encoders */
    XAAudioEncoderSettings  audioEncSettings;
    XAVideoSettings         videoEncSettings;
    XAImageSettings         imageEncSettings;

    /*buffersink variable*/
    guint64                 writepos;

} XAMediaRecorderAdaptationCtx_;


/*
 * Structure for Camera specific gst-adaptation variables
 */
typedef struct XACameraAdaptationCtx_ {

    /* Parent*/
    XAAdaptationBaseCtx_ baseObj;

    /* OMX-AL Variables */
    XAuint32        deviceID;
    XAuint32        imageEffectID;
    XAmillidegree   curRotation;
    XAuint32        curMirror;

    /* GST Variables */
    GstFocusStatus focusStatus;

    /* Internals */
    XAboolean		recording;
    XAboolean		playing;
    XAboolean		snapshotting;
} XACameraAdaptationCtx_;

/*
 * Structure for Camera specific gst-adaptation variables
 */
typedef struct XARadioAdaptationCtx_ {

    /* Parent*/
    XAAdaptationBaseCtx_ baseObj;

    /* OMX-AL Variables */
    XAuint32 frequency;
    XAuint8 range;

    pthread_t emulationThread;
    pthread_t rdsEmulationThread;
    XARadioRDSState rdsState;
    XARadioState state;

    /* GST Variables */
} XARadioAdaptationCtx_;


typedef struct XAOMixAdaptConnObj_
{
    /*pointer to connected context*/
    XAAdaptationBaseCtx* ctx;
    /*pointer to sink currently used by connected ctx*/
    GstElement* currentSink;
} XAOMixAdaptConnObj;

typedef struct XAOutputMixAdaptationCtx_
{
    /* Parent*/
    XAAdaptationBaseCtx_ baseObj;
    /* Internal variables */
    XAboolean   isStereoPosition;
    XAboolean	mute;

    XAuint32    currentrouting;

    GArray* 	availableDevices;
    GArray*     connectedObjects;
} XAOutputMixAdaptationCtx_;

/*
 * Structure for Ledarray specific gst-adaptation variables
 */
typedef struct XALEDArrayAdaptationCtx_ {

    /* Parent*/
    XAAdaptationBaseCtx_ baseObj;

    /* OMX-AL Variables */
    XAuint32    deviceID;

    /* GST Variables */

} XALedarrayAdaptationCtx_;

/*
 * Structure for Vibra specific gst-adaptation variables
 */
typedef struct XAVibraAdaptationCtx_ {

    /* Parent*/
    XAAdaptationBaseCtx_ baseObj;

    /* OMX-AL Variables */
    XAuint32    deviceID;

    /* GST Variables */
} XAVibraAdaptationCtx_;

/* FUNCTIONS */
/*
 * gboolean XAAdaptationBase_GstBusCb( GstBus *bus, GstMessage *message, gpointer data );
 * Default CallBack handler for gst-bus messages. This will be called if object specific callback is
 * not implemented.
 */
gboolean XAAdaptationBase_GstBusCb( GstBus *bus, GstMessage *message, gpointer data );

XAresult XAAdaptationBase_InitGstListener(XAAdaptationBaseCtx* ctx);
void * XAAdaptationBase_LaunchGstListener(void* args);
void XAAdaptationBase_StopGstListener(XAAdaptationBaseCtx* ctx);
void XAAdaptationBase_SendAdaptEvents(XAAdaptationBaseCtx* ctx, XAAdaptEvent* event);

void XAAdaptationBase_PrepareAsyncWait(XAAdaptationBaseCtx* ctx);
void XAAdaptationBase_StartAsyncWait(XAAdaptationBaseCtx* ctx);
gboolean XAAdaptationBase_CancelAsyncWait(gpointer ctx);
void XAAdaptationBase_CompleteAsyncWait(XAAdaptationBaseCtx* ctx);

GstElement* XAAdaptationBase_CreateGstSource( XADataSource* xaSrc, const char *name, XAboolean *isobj, XAboolean *isPCM, XAboolean *isRawImage );
GstElement* XAAdaptationBase_CreateGstSink( XADataSink* xaSrc, const char *name, XAboolean *isobj );
GstElement* XAAdaptationBase_CreateVideoPP( );

GstElement* XAAdaptationBase_CreateVideoPPBlackScr( );
GstElement* XAAdaptationBase_CreateInputSelector( );
GstElement* XAAdaptationBase_CreateAudioPP( );
void XAAdaptationBase_PadBlockCb(GstPad *pad, gboolean blocked, gpointer user_data);
void XAAdaptationBase_SetAllCaps (GstCaps * caps, char *field, ...);

XAresult XAMediaPlayerAdapt_UpdatePositionCbTimer(XAMediaPlayerAdaptationCtx_* mCtx);

XAresult XAMediaRecorderAdapt_ChangeEncoders( XAMediaRecorderAdaptationCtx_* mCtx );
XAresult XAMediaRecorderAdapt_CheckCodec( XAMediaRecorderAdaptationCtx_* mCtx, XACapsType encType, XAuint32 encoderId );

XAresult XAMetadataAdapt_TryWriteTags(XAAdaptationBaseCtx* mCtx, GstBin* binToWriteTo);
void XAMetadataAdapt_FreeVars(XAMetadataAdaptVars *vars);

GstElement* XAOutputMixAdapt_GetSink(XAAdaptationBaseCtx* bCtx);


XAresult XAOutputMixAdapt_ConnectObject(XAAdaptationBaseCtx* omCtx, XAAdaptationBaseCtx* bCtx, GstElement* usedMix);
XAresult XAOutputMixAdapt_DisconnectObject(XAAdaptationBaseCtx* omCtx, XAAdaptationBaseCtx* bCtx);
void* XAAdaptationBase_ContentPipeScrThrFunc( void* arg);
void* XAAdaptationBase_ContentPipeSinkThrFunc( void* arg);
CPresult XAAdaptationBase_ContentPipeSrcCb(CP_EVENTTYPE eEvent, CPuint iParam);
CPresult XAAdaptationBase_ContentPipeSinkCb(CP_EVENTTYPE eEvent, CPuint iParam);

#endif /* XAADAPTATION_H_ */
