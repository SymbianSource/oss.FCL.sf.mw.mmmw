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

#ifndef XAMEDIAPLAYERADAPTCTX_H
#define XAMEDIAPLAYERADAPTCTX_H

#include "xaadaptationgst.h"
#include "xametadataadaptation.h"
/* TYPEDEFS */

typedef struct XAMediaPlayerAdaptationCtx_ XAMediaPlayerAdaptationCtx;

/*
 * Structure for Media Player specific gst-adaptation.
 */
typedef struct XAMediaPlayerAdaptationCtx_
{
    /* Parent*/
    XAAdaptationGstCtx_    baseObj;

    /* OMX-AL Variables */
    XADataSource            *xaSource, *xaBankSrc;
    XADataSink              *xaAudioSink, *xaVideoSink, *xaLEDArray, *xaVibra;

    /* GST elements */
    GstElement              *source;
    XAboolean               isobjsrc;    /*is source another XA object?*/
    GstElement              *codecbin;
    GstElement              *audioppbin;
    GstElement              *videoppbin;
    GstElement              *filter;
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

    XAboolean               cameraSinkSynced;

    XAMetadataAdaptVars     *metadatavars;

} XAMediaPlayerAdaptationCtx_;


/* FUNCTIONS */
XAAdaptationBaseCtx* XAMediaPlayerAdapt_Create( XADataSource *pDataSrc, XADataSource *pBankSrc,
                                                       XADataSink *pAudioSnk, XADataSink *pImageVideoSnk,
                                                       XADataSink *pVibra, XADataSink *pLEDArray);
XAresult XAMediaPlayerAdapt_PostInit( XAAdaptationGstCtx* bCtx );
void XAMediaPlayerAdapt_Destroy( XAAdaptationGstCtx* bCtx );
XAresult XAMediaPlayerAdapt_UpdatePositionCbTimer(XAMediaPlayerAdaptationCtx_* mCtx);

XAresult XAMediaPlayerAdapt_InitContentPipeSrc(XAMediaPlayerAdaptationCtx* ctx);
#endif /*XAMEDIAPLAYERADAPTCTX_H*/

