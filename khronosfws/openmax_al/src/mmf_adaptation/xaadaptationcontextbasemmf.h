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

#ifndef XAADAPTATIONCONTEXTBASEMMF_H
#define XAADAPTATIONCONTEXTBASEMMF_H

#include <stdlib.h>
#include "xaglobals.h"
#include "openmaxalwrapper.h"
//#include "OpenMAXAL_ContentPipe.h"
#include "xaplatform.h"


/* ENUMERATIONS */
typedef enum {
    XAMediaPlayerAdaptationMMF = 0,
    XAMediaRecorderAdaptationMMF,
    XARadioAdaptationMMF,
    XACameraAdaptationMMF,
    XAOutputMixAdaptationMMF,
    XAVibraAdaptationMMF,
    XALedArrayAdaptationMMF,
    XAMDAdaptationMMF,
    XAEngineAdaptationMMF
}AdaptationContextMMFIDS;

/* Adaptation event structure */
typedef struct XAAdaptEventMMF_
{
    XAAdaptEventTypes  eventtype; /* what kind of event, e.g. playitf event  */
    XAuint32            eventid;   /* eventtype-specific event, e.g. XA_PLAYEVENT macro */
    XAuint32           datasize;  /* event data size */
    void*              data;      /* event data if needed */
} XAAdaptEventMMF;

/* Adaptation event callback */
typedef void (*xaAdaptEventHandlerMMF) ( void *pHandlerCtx, XAAdaptEventMMF* event );

/* Forward declaration of adaptation basecontext */
typedef struct XAAdaptationBaseMMFCtx_ XAAdaptationBaseMMFCtx;

/* FUNCTIONS */
XAresult XAAdaptationBaseMMF_Init( XAAdaptationBaseMMFCtx* pSelf, XAuint32 ctxId );
XAresult XAAdaptationBaseMMF_PostInit( XAAdaptationBaseMMFCtx* ctx );
XAresult XAAdaptationBaseMMF_AddEventHandler( XAAdaptationBaseMMFCtx* ctx, xaAdaptEventHandlerMMF evtHandler,
                                    XAuint32 evtTypes,void *pHandlerCtx );
XAresult XAAdaptationBaseMMF_RemoveEventHandler( XAAdaptationBaseMMFCtx* ctx, xaAdaptEventHandlerMMF evtHandler );
void XAAdaptationBaseMMF_Free( XAAdaptationBaseMMFCtx* ctx );
//XAresult XAAdaptationBase_SetCPConfiguration(XAAdaptationBaseCtx* ctx, XAConfigExtensionCpKey configValue);

#endif /*XAADAPTATIONCONTEXTBASEMMF_H*/

