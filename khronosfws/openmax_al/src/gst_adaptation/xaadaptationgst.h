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
 * Description: Base Gstreamer Adaptation Source
 *
 */

#ifndef XAADAPTATIONGST_H_
#define XAADAPTATIONGST_H_

#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <gst/app/gstappbuffer.h>
#include "openmaxalwrapper.h"
#include "xaglobals.h"
#include "xaplatform.h"
#include "xaadptbasectx.h"
#include <stdlib.h>

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
typedef gboolean (*GstBusCb)(GstBus *bus, GstMessage *message, gpointer data);

#define CONTENT_PIPE_BUFFER_SIZE 1000
#define TEST_VIDEO_WIDTH     640
#define TEST_VIDEO_HEIGHT    480

/*typedef enum
 {
 XA_AUDIO_WAVENC = 0,
 XA_AUDIO_VORBISENC,
 XA_AUDIO_PCM,
 XA_NUM_OF_AUDIOENCODERS  Do not move this line 
 } XAAudioEnc;*/

/*typedef enum
 {
 XA_VIDEO_JPEGENC = 0,
 XA_VIDEO_THEORAENC,
 XA_NUM_OF_VIDEOENCODERS  Do not move this line 
 } XAVideoEnc;*/

/*typedef enum CP_STATE
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
 }CP_STATE;*/

/*typedef enum
 {
 XA_IMAGE_JPEGENC = 0,
 XA_IMAGE_RAW,
 XA_NUM_OF_IMAGEENCODERS  Do not move this line 
 } XAImageEnc;*/

/* STRUCTURES */

/* Forward declaration of adaptation basecontext */
typedef struct XAAdaptationGstCtx_ XAAdaptationGstCtx;

/*
 * Structure that holds all common variables for every
 * Gst-Adaptation context structures.
 */
typedef struct XAAdaptationGstCtx_
    {
    /* Common Variables for all adaptation elements */
    XAAdaptationBaseCtx baseObj;

    GstState binWantedState; /** requested gst-bin target state **/
    GstElement *bin; /** Container for all gst elements **/

    GstBus *bus; /** Gst-bus where gst sends messages **/
    GMainLoop *busloop; /** Gst-bus listener loop **/
    pthread_t busloopThr;
    GstBusCb busCb; /** Gst-Bus callback funtion*/

    XAboolean waitingasyncop;
    sem_t semAsyncWait;
    guint asynctimer;

#ifdef XA_IMPL_MEASURE_GST_DELAY
    clock_t startTime;
    clock_t endTime;
    double diff;
#endif /*XA_IMPL_MEASURE_GST_DELAY*/

    XAboolean thread_launched;
    pthread_mutex_t ds_mutex;
    pthread_cond_t ds_condition;
    XAboolean cond_mutx_inited;
    /* FUNCTIONS*/

    } XAAdaptationGstCtx_;

/* FUNCTIONS */
/*
 * gboolean XAAdaptationBase_GstBusCb( GstBus *bus, GstMessage *message, gpointer data );
 * Default CallBack handler for gst-bus messages. This will be called if object specific callback is
 * not implemented.
 */

XAresult XAAdaptationGst_Init(XAAdaptationGstCtx* pSelf, XAuint32 ctxId);
XAresult XAAdaptationGst_PostInit(XAAdaptationGstCtx* ctx);
void XAAdaptationGst_Free(XAAdaptationGstCtx* ctx);

gboolean XAAdaptationGst_GstBusCb(GstBus *bus, GstMessage *message,
        gpointer data);

XAresult XAAdaptationGst_InitGstListener(XAAdaptationGstCtx* ctx);
void* XAAdaptationGst_LaunchGstListener(void* args);
void XAAdaptationGst_StopGstListener(XAAdaptationGstCtx* ctx);

void XAAdaptationGst_PrepareAsyncWait(XAAdaptationGstCtx* ctx);
void XAAdaptationGst_StartAsyncWait(XAAdaptationGstCtx* ctx);
gboolean XAAdaptationGst_CancelAsyncWait(gpointer ctx);
void XAAdaptationGst_CompleteAsyncWait(XAAdaptationGstCtx* ctx);

GstElement* XAAdaptationGst_CreateGstSource(XADataSource* xaSrc,
        const char *name, XAboolean *isobj, XAboolean *isPCM,
        XAboolean *isRawImage);
GstElement* XAAdaptationGst_CreateGstSink(XADataSink* xaSrc,
        const char *name, XAboolean *isobj);
GstElement* XAAdaptationGst_CreateVideoPP(void);

GstElement* XAAdaptationGst_CreateVideoPPBlackScr(void);
GstElement* XAAdaptationGst_CreateInputSelector(void);
GstElement* XAAdaptationGst_CreateAudioPP(void);
void XAAdaptationGst_PadBlockCb(GstPad *pad, gboolean blocked,
        gpointer user_data);
void XAAdaptationGst_SetAllCaps(GstCaps * caps, char *field, ...);

/*XAresult XAMetadataAdapt_TryWriteTags(XAAdaptationGstCtx* mCtx, GstBin* binToWriteTo);*/

#endif /* XAADAPTATIONGST_H_ */
