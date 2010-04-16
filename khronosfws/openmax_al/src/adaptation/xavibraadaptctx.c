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

#include <gst.h>
#include "XAVibraAdaptCtx.h"
#include "XAAdaptation.h"

/*
 * XAAdaptationBaseCtx* XAVibraAdapt_Create()
 * Allocates memory for Vibra Adaptation Context and makes 1st phase initialization
 * @returns XAVibraAdaptationCtx* - Pointer to created context
 */
XAAdaptationBaseCtx* XAVibraAdapt_Create(XAuint32 deviceID)
{
    XAVibraAdaptationCtx *pSelf = calloc(1, sizeof(XAVibraAdaptationCtx));
    DEBUG_API("->XAVibraAdapt_Create");
    if ( pSelf)
    {
        if( XAAdaptationBase_Init(&(pSelf->baseObj),XAVibraAdaptation)
                    != XA_RESULT_SUCCESS )
        {
        DEBUG_ERR("Failed to init base context!!!");
        free(pSelf);
        pSelf = NULL;
        }
    }

    DEBUG_API("<-XAVibraAdapt_Create");
    return (XAAdaptationBaseCtx*)pSelf;
}

/*
 * XAresult XAVibraAdapt_PostInit()
 * 2nd phase initialization of Vibra Adaptation Context
 */
XAresult XAVibraAdapt_PostInit(XAAdaptationBaseCtx* bCtx)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAVibraAdaptationCtx* ctx = NULL;

    DEBUG_API("->XAVibraAdapt_PostInit");
    if(bCtx == NULL || bCtx->ctxId != XAVibraAdaptation )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVibraAdapt_PostInit");
        return XA_RESULT_PARAMETER_INVALID;
    }
    ctx = (XAVibraAdaptationCtx*)bCtx;

    XAAdaptationBase_PostInit( &(ctx->baseObj) );

    DEBUG_API("<-XAVibraAdapt_PostInit");
    return ret;
}

/*
 * void XAVibraAdapt_Destroy(XAVibraAdaptationCtx* ctx)
 * Destroys Vibra Adaptation Context
 * @param ctx - Vibra Adaptation context to be destroyed
 */
void XAVibraAdapt_Destroy(XAAdaptationBaseCtx* bCtx)
{
    XAVibraAdaptationCtx* ctx = NULL;
    DEBUG_API("->XAVibraAdapt_Destroy");

    if(bCtx == NULL || bCtx->ctxId != XAVibraAdaptation )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVibraAdapt_Destroy");
        return;
    }
    ctx = (XAVibraAdaptationCtx*)bCtx;
    XAAdaptationBase_Free(&(ctx->baseObj));

    free(ctx);
    ctx = NULL;

    DEBUG_API("<-XAVibraAdapt_Destroy");
}
