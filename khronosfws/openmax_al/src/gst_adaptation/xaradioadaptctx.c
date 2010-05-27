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

#include <gst/gst.h>
#include "xaradioadaptctx.h"
#include "xaadaptationgst.h"
#include "assert.h"


/*
 * XAAdaptationGstCtx* XARadioAdapt_Create()
 * Allocates memory for Radio Adaptation Context and makes 1st phase initialization
 * @returns XARadioAdaptationCtx* - Pointer to created context
 */
XAAdaptationBaseCtx* XARadioAdapt_Create()
{
    XARadioAdaptationCtx *pSelf = (XARadioAdaptationCtx*)calloc(1, sizeof(XARadioAdaptationCtx));
    DEBUG_API("->XARadioAdapt_Create");

    if ( pSelf)
    {
        if( XAAdaptationBase_Init(&(pSelf->baseObj.baseObj),XARadioAdaptation)
                    != XA_RESULT_SUCCESS )
                {
                    DEBUG_ERR("Failed to init base context!!!");
                    free(pSelf);
                    pSelf = NULL;
                }
                else
                {
                    pSelf->state = XA_RADIO_IDLE;
                    pSelf->range = RADIO_DEFAULT_FREQ_RANGE;
                    pSelf->frequency = RADIO_DEFAULT_FREQ;

                    pSelf->rdsEmulationThread = 0;
                    pSelf->emulationThread = 0;
                }
    }

    DEBUG_API("<-XARadioAdapt_Create");
    return (XAAdaptationBaseCtx*)&pSelf->baseObj;
}

/*
 * XAresult XARadioAdapt_PostInit()
 * 2nd phase initialization of Radio Adaptation Context
 */
XAresult XARadioAdapt_PostInit(XAAdaptationBaseCtx* bCtx)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARadioAdaptationCtx* ctx = NULL;
    DEBUG_API("->XARadioAdapt_PostInit");
    if(bCtx == NULL || bCtx->ctxId != XARadioAdaptation )
    {
        DEBUG_ERR("Invalid parameter!!");
        DEBUG_API("<-XARadioAdapt_PostInit");
        return XA_RESULT_PARAMETER_INVALID;
    }
    ctx = (XARadioAdaptationCtx*)bCtx;

    assert(ctx);

    ret = XAAdaptationBase_PostInit( &ctx->baseObj.baseObj );
    if( ret!=XA_RESULT_SUCCESS )
    {
        DEBUG_ERR("Base context postinit failed!!");
        DEBUG_API("<-XARadioAdapt_PostInit");
        return ret;
    }

    ctx->baseObj.bin = gst_element_factory_make( "audiotestsrc", "audiosrc");
    if ( !ctx->baseObj.bin )
    {
        DEBUG_ERR("Unable to create test audio source!");
        DEBUG_API("<-XARadioAdapt_PostInit");
        return XA_RESULT_INTERNAL_ERROR;
    }
    g_object_set( G_OBJECT(ctx->baseObj.bin), "wave", 0x5, NULL );

    DEBUG_API("<-XARadioAdapt_PostInit");
    return ret;
}

/*
 * void XARadioAdapt_Destroy(XAAdaptationGstCtx* bCtx)
 * Destroys Radio Adaptation Context
 * @param ctx - Radio Adaptation context to be destroyed
 */
void XARadioAdapt_Destroy(XAAdaptationBaseCtx* bCtx)
{
    XARadioAdaptationCtx* ctx = NULL;
    DEBUG_API("->XARadioAdapt_Destroy");

    if(bCtx == NULL || bCtx->ctxId != XARadioAdaptation )
    {
        DEBUG_ERR("Invalid parameter!!");
        DEBUG_API("<-XARadioAdapt_Destroy");
        return;
    }
    ctx = (XARadioAdaptationCtx*)bCtx;
    XAAdaptationBase_Free(&(ctx->baseObj.baseObj));

    free(ctx);

    DEBUG_API("<-XARadioAdapt_Destroy");
}
