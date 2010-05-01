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

#include <string.h>
#include "xaledarrayadaptctx.h"
#include "xaadaptationgst.h"

/*
 * XALEDArrayAdaptationCtx* XALEDArrayAdapt_Create()
 * Allocates memory for LEDArray Adaptation Context and makes 1st phase initialization
 * @returns XALEDArrayAdaptationCtx* - Pointer to created context
 */
XAAdaptationBaseCtx* XALEDArrayAdapt_Create(XAuint32 deviceID)
{
    XALEDArrayAdaptationCtx *pSelf = (XALEDArrayAdaptationCtx *)calloc(1, sizeof(XALEDArrayAdaptationCtx));
    DEBUG_API("->XALEDArrayAdapt_Create");

    if ( pSelf)
    {
        if( XAAdaptationBase_Init(&(pSelf->baseObj.baseObj),XALedArrayAdaptation)
                    != XA_RESULT_SUCCESS )
        {
            DEBUG_ERR("Failed to init base context!!!");
            free(pSelf);
            pSelf = NULL;
        }
        else
        {
            pSelf->deviceID = deviceID;

        }
    }

    DEBUG_API("<-XALEDArrayAdapt_Create");
    return (XAAdaptationBaseCtx*)&pSelf->baseObj;
}

/*
 * XAresult XALEDArrayAdapt_PostInit()
 * 2nd phase initialization of LEDArray Adaptation Context
 */
XAresult XALEDArrayAdapt_PostInit(XAAdaptationGstCtx* bCtx)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XALEDArrayAdaptationCtx* ctx = NULL;
    DEBUG_API("->XALEDArrayAdapt_PostInit");
    if(bCtx == NULL || bCtx->baseObj.ctxId != XALedArrayAdaptation )
    {
        DEBUG_ERR("Invalid parameter!!");
        DEBUG_API("<-XALEDArrayAdapt_PostInit");
        return XA_RESULT_PARAMETER_INVALID;
    }
    ctx = (XALEDArrayAdaptationCtx*)bCtx;
    if ( !ctx )
    {
        return XA_RESULT_INTERNAL_ERROR;
    }

    XAAdaptationBase_PostInit( &(ctx->baseObj.baseObj) );

    DEBUG_API("<-XALEDArrayAdapt_PostInit");
    return ret;
}

/*
 * void XALEDArrayAdapt_Destroy(XALEDArrayAdaptationCtx* ctx)
 * Destroys LEDArray Adaptation Context
 * @param ctx - LEDArray Adaptation context to be destroyed
 */
void XALEDArrayAdapt_Destroy(XAAdaptationGstCtx* bCtx)
{
    XALEDArrayAdaptationCtx* ctx = NULL;
    DEBUG_API("->XALEDArrayAdapt_Destroy");
    if(bCtx == NULL || bCtx->baseObj.ctxId != XALedArrayAdaptation )
    {
        DEBUG_ERR("Invalid parameter!!");
        DEBUG_API("<-XALEDArrayAdapt_Destroy");
        return;
    }
    ctx = (XALEDArrayAdaptationCtx*)bCtx;
    DEBUG_API("->XALEDArrayAdapt_Destroy");

    XAAdaptationBase_Free(&(ctx->baseObj.baseObj));


    free(ctx);
    ctx = NULL;

    DEBUG_API("<-XALEDArrayAdapt_Destroy");
}
