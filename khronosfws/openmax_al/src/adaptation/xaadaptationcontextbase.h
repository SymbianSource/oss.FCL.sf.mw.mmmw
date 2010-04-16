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

#ifndef XAADAPTATIONCONTEXTBASE_H
#define XAADAPTATIONCONTEXTBASE_H

#include <stdlib.h>
#include "XAGlobals.h"
#include "openmaxalwrapper.h"
#include "OpenMAXAL_ContentPipe.h"
#include "XAPlatform.h"

/* ENUMERATIONS */

typedef enum {
    XAMediaPlayerAdaptation = 0,
    XAMediaRecorderAdaptation,
    XARadioAdaptation,
    XACameraAdaptation,
    XAOutputMixAdaptation,
    XAVibraAdaptation,
    XALedArrayAdaptation,
    XAMDAdaptation,
    XAEngineAdaptation
}AdaptationContextIDS;

/**
 * enumeration for different adaptation event types.
 * add more types when needed, use single bit values.
 **/
typedef enum {
    XA_GENERICEVENTS = 0x1,
    XA_PLAYITFEVENTS = 0x2,
    XA_RECORDITFEVENTS = 0x4,
    XA_SNAPSHOTITFEVENTS = 0x8,
    XA_OUTPUTMIXITFEVENTS = 0x10,
    XA_CAMERAITFEVENTS = 0x20,
    XA_PREFETCHITFEVENTS = 0x40,
    XA_RADIOITFEVENTS = 0x80,
    XA_RDSITFEVENTS = 0x100,
    XA_METADATAEVENTS = 0x200,
    XA_ADDMORETYPES  = 0x400
}XAAdaptEventTypes;

#define XA_ADAPT_PU_INTERVAL 50                     /* position update interval */
#define XA_ADAPT_ASYNC_TIMEOUT 3000                 /* timeout to wait async events */
#define XA_ADAPT_ASYNC_TIMEOUT_SHORT 1000           /* timeout to wait async events */
#define XA_ADAPT_ASYNC_TIMEOUT_SHORT_NSEC 1000000   /* timeout to wait async events */

#define XA_ADAPT_POSITION_UPDATE_EVT 0xf0           /* position update event */
#define XA_ADAPT_SNAPSHOT_TAKEN 0xf1                /* snapshot taken event */
#define XA_ADAPT_SNAPSHOT_INITIATED 0xf2            /* snapshot intitiated event */
#define XA_ADAPT_MDE_TAGS_AVAILABLE 0xf3            /* metadata taglist changed */
#define XA_ADAPT_OMIX_DEVICESET_CHANGED 0xf4        /* Output mix device changed event */
#define XA_ADAPT_RADIO_FREQUENCY_CHANGED 0xf6       /* Radio frequency changed event */
#define XA_ADAPT_RADIO_FREQUENCY_RANGE_CHANGED 0xf7 /* Radio frequency range changed event */
#define XA_ADAPT_RADIO_SEEK_COMPLETE 0xf8           /* Radio seek complete changed event */
#define XA_ADAPT_RDS_GET_ODA_GROUP_DONE 0xf9        /* RDS get oda group done event */
#define XA_ADAPT_BUFFERING 0xfa
#define XA_ADAPT_MDE_TAGS_WRITTEN 0xfb
/* TYPEDEFS */

#define RADIO_DEFAULT_FREQ_RANGE XA_FREQRANGE_FMEUROAMERICA
#define RADIO_DEFAULT_FREQ 88000000

/* Adaptation event structure */
typedef struct XAAdaptEvent_
{
    XAAdaptEventTypes  eventtype; /* what kind of event, e.g. playitf event  */
    XAuint8            eventid;   /* eventtype-specific event, e.g. XA_PLAYEVENT macro */
    XAuint32           datasize;  /* event data size */
    void*              data;      /* event data if needed */
} XAAdaptEvent;

/* Adaptation event callback */
typedef void (*xaAdaptEventHandler) ( void *pHandlerCtx, XAAdaptEvent* event );

/* Forward declaration of adaptation basecontext */
typedef struct XAAdaptationBaseCtx_ XAAdaptationBaseCtx;

/* FUNCTIONS */
XAresult XAAdaptationBase_Init( XAAdaptationBaseCtx* pSelf, XAuint32 ctxId );
XAresult XAAdaptationBase_PostInit( XAAdaptationBaseCtx* ctx );
XAresult XAAdaptationBase_AddEventHandler( XAAdaptationBaseCtx* ctx, xaAdaptEventHandler evtHandler,
                                    XAuint32 evtTypes,void *pHandlerCtx );
XAresult XAAdaptationBase_RemoveEventHandler( XAAdaptationBaseCtx* ctx, xaAdaptEventHandler evtHandler );
void XAAdaptationBase_Free( XAAdaptationBaseCtx* ctx );
XAresult XAAdaptationBase_SetCPConfiguration(XAAdaptationBaseCtx* ctx, XAConfigExtensionCpKey configValue);

#endif /*XAADAPTATIONCONTEXTBASE_H*/

