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

#ifndef XASNAPSHOTITF_H
#define XASNAPSHOTITF_H

#include <semaphore.h>
#include "openmaxalwrapper.h"
#include "xaglobals.h"
#ifdef _GSTREAMER_BACKEND_
#include "XAAdaptationContextBase.h"
#endif
/** MACROS **/

/** TYPES **/

/*Callbacks*/


/** ENUMERATIONS **/

/** STRUCTURES **/


/* Definition of XASnapshotItf implementation */
typedef struct XASnapshotItfImpl_
{
    /* parent interface */
    struct XASnapshotItf_ itf;
    /* pointer to self */
    struct XASnapshotItfImpl_* self;

    /* variables */
    XAuint32 numberofpictures;
    XAboolean shutterfeedback;
    xaSnapshotInitiatedCallback initcallback;
    xaSnapshotTakenCallback takencallback;
    void* usercontext;
    XASnapshotItf cbPtrToSelf;

#ifdef _GSTREAMER_BACKEND_
    XAAdaptationBaseCtx *adapCtx;
#endif
    XAboolean           snapshotongoing;
    XAboolean           initongoing;
    XAboolean           initialized;
    XAboolean			freezevf;

} XASnapshotItfImpl;

/** METHODS **/

/* Base interface XASnapshotItf implementation */
XAresult XASnapshotItfImpl_InitiateSnapshot(XASnapshotItf self,
                                            XAuint32 numberOfPictures,
                                            XAuint32 fps,
                                            XAboolean freezeViewFinder,
                                            XADataSink sink,
                                            xaSnapshotInitiatedCallback initiatedCallback,
                                            xaSnapshotTakenCallback takenCallback,
                                            void * pContext);

XAresult XASnapshotItfImpl_TakeSnapshot(XASnapshotItf self);

XAresult XASnapshotItfImpl_CancelSnapshot(XASnapshotItf self);

XAresult XASnapshotItfImpl_ReleaseBuffers(XASnapshotItf self,
                                          XADataSink *image);

XAresult XASnapshotItfImpl_GetMaxPicsPerBurst(XASnapshotItf self,
                                              XAuint32 *maxNumberOfPictures);

XAresult XASnapshotItfImpl_GetBurstFPSRange(XASnapshotItf self,
                                            XAuint32 *minFPS,
                                            XAuint32 *maxFPS);

XAresult XASnapshotItfImpl_SetShutterFeedback(XASnapshotItf self,
                                              XAboolean enabled);

XAresult XASnapshotItfImpl_GetShutterFeedback(XASnapshotItf self,
                                              XAboolean *enabled);

/* XASnapshotItfImpl -specific methods */
XASnapshotItfImpl* XASnapshotItfImpl_Create(
#ifdef _GSTREAMER_BACKEND_
        XAAdaptationBaseCtx *adaptCtx,
#else
        void
#endif
        );
#ifdef _GSTREAMER_BACKEND_
void XASnapshotItfImpl_AdaptCb( void *pHandlerCtx, XAAdaptEvent *event );
#endif
void XASnapshotItfImpl_Free(XASnapshotItfImpl* self);

#endif /* XASNAPSHOTITF_H */
