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

#ifndef XAIMAGEEFFECTSITF_H
#define XAIMAGEEFFECTSITF_H

#include "xaadptbasectx.h"

/** MACROS **/
#define NO_IMAGE_EFFECTS 0

/** TYPES **/

/** ENUMERATIONS **/

/** STRUCTURES **/
/* Definition of XAImageEffectsItf implementation */
typedef struct XAImageEffectsItfImpl_
{
    /* parent interface */
    struct XAImageEffectsItf_ itf;
    /* pointer to self */
    struct XAImageEffectsItfImpl_* self;

    /* variables */
    XAuint32    index;
    XAboolean   enabled;
    XAuint32    imageEffectID;

    /*Adaptation variables*/
    XAAdaptationBaseCtx *adapCtx;


} XAImageEffectsItfImpl;

/** METHODS **/

/* Base interface XAImageEffectsItf implementation */
XAresult XAImageEffectsItfImpl_QuerySupportedImageEffects(XAImageEffectsItf self,
                                                          XAuint32 index,
                                                          XAuint32 *pImageEffectId);

XAresult XAImageEffectsItfImpl_EnableImageEffect(XAImageEffectsItf self,
                                                 XAuint32 imageEffectID);

XAresult XAImageEffectsItfImpl_DisableImageEffect(XAImageEffectsItf self,
                                                  XAuint32 imageEffectID);

XAresult XAImageEffectsItfImpl_IsImageEffectEnabled(XAImageEffectsItf self,
                                                    XAuint32 imageEffectID,
                                                    XAboolean *pEnabled);

   
/* XAImageEffectsItfImpl -specific methods */
XAImageEffectsItfImpl* XAImageEffectsItfImpl_Create(XAAdaptationBaseCtx *adapCtx);
void XAImageEffectsItfImpl_Free(XAImageEffectsItfImpl* self);
#endif /* XAIMAGEEFFECTSITF_H */
