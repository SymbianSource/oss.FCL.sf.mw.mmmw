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

#ifndef XAADAPTATIONMMF_H_
#define XAADAPTATIONMMF_H_

#include <pthread.h>
#include <semaphore.h>
#include <garray.h>
#include <string.h>
#include <unistd.h>
#include "openmaxalwrapper.h"
#include "xaglobals.h"
//#include "OpenMAXAL_ContentPipe.h"
#include "xaplatform.h"
#include "xaadptbasectx.h"
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

#define CONTENT_PIPE_BUFFER_SIZE 1000
#define TEST_VIDEO_WIDTH     640
#define TEST_VIDEO_HEIGHT    480

/*
typedef enum
{
    XA_AUDIO_WAVENC = 0,
    XA_AUDIO_VORBISENC,
    XA_AUDIO_PCM,
    XA_NUM_OF_AUDIOENCODERS  Do not move this line 
} XAAudioEnc;

typedef enum
{
    XA_VIDEO_JPEGENC = 0,
    XA_VIDEO_THEORAENC,
    XA_NUM_OF_VIDEOENCODERS  Do not move this line 
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
*/

/* Forward declaration of adaptation basecontext */
typedef struct XAAdaptationMMFCtx_ XAAdaptationMMFCtx;

/*
 * Structure that holds all common variables for every
 * mmf-Adaptation context structures.
 */
typedef struct XAAdaptationMMFCtx_
{
XAAdaptationBaseCtx baseObj;

} XAAdaptationMMFCtx_;

typedef struct XAEngineAdaptationMMFCtx_
{
    /* Parent*/
    XAAdaptationMMFCtx_    baseObj;

} XAEngineAdaptationMMFCtx_;

/*
 * Structure for Media Player specific gst-adaptation.
 */
typedef struct XAMediaPlayerAdaptationMMFCtx_
{
    /* Parent*/
    XAAdaptationMMFCtx_    baseObj;

    /* OMX-AL Variables */
    XADataSource            *xaSource, *xaBankSrc;
    XADataSink              *xaAudioSink, *xaVideoSink, *xaLEDArray, *xaVibra;

    /* MMF elements */
    XAboolean               isobjsrc;    /*is source another XA object?*/

    XAboolean               isobjasink;   /*is audio sink another XA object?*/
    XAboolean               isobjvsink;   /*is video sink another XA object?*/

    XAboolean               mute;
    XAuint32                premutevol;
    XAuint32                imageEffectID;
    XAboolean               isStereoPosition;
    XAmillidegree           curRotation;
    XAuint32                curMirror;
    XAint32                 buffering;

    /* internals */
    XAboolean               trackpositionenabled;
    XAboolean               loopingenabled;
    XAboolean		        cameraSinkSynced;
    void*                   mmfContext;
	void* 					mmfMetadataContext;
/*
    XAuint32                playerState;
*/

} XAMediaPlayerAdaptationMMFCtx_;


typedef struct XAMetadataAdaptationMMFCtx_
{
    /* Parent*/
    XAAdaptationMMFCtx_    baseObj;

    /* OMX-AL Variables */
    XADataSource            *xaSource;

    void*                   mmfContext;
} XAMetadataAdaptationMMFCtx_;

/* FUNCTIONS */
/*
 * gboolean XAAdaptationBase_GstBusCb( GstBus *bus, GstMessage *message, gpointer data );
 * Default CallBack handler for gst-bus messages. This will be called if object specific callback is
 * not implemented.
 */

XAresult XAAdaptationBaseMMF_Init( XAAdaptationMMFCtx* pSelf, XAuint32 ctxId );
XAresult XAAdaptationBaseMMF_PostInit( XAAdaptationMMFCtx* ctx );
void XAAdaptationBaseMMF_Free( XAAdaptationMMFCtx* ctx );

#endif /* XAADAPTATIONMMF_H_ */
