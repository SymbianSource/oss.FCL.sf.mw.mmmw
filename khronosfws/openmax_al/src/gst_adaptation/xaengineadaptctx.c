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
#include <gst.h>
#include "xaengineadaptctx.h"
#include "xaadaptationgst.h"

/*
 * gboolean XAEngineAdapt_GstBusCb( GstBus *bus, GstMessage *message, gpointer data )
 * MediaPlayer Gst-bus message handler (Callback)
 */
gboolean XAEngineAdapt_GstBusCb( GstBus *bus, GstMessage *message, gpointer data )
{
    XAEngineAdaptationCtx* mCtx = (XAEngineAdaptationCtx*)data;
    DEBUG_API("->XAEngineAdapt_GstBusCb");

    /* only listen to pipeline messages */
    if(GST_MESSAGE_SRC(message)==(GstObject*)(mCtx->baseObj.bin) )
    {
        DEBUG_API_A2("->XAEngineAdapt_GstBusCb:\"%s\" from object \"%s\"",
                        GST_MESSAGE_TYPE_NAME(message), GST_OBJECT_NAME(GST_MESSAGE_SRC(message)));
    }

    DEBUG_API("<-XAEngineAdapt_GstBusCb");
    return TRUE;
}

/*
 * XAAdaptationGstCtx* XAEngineAdapt_Create()
 * Allocates memory for Engine Adaptation Context and makes 1st phase initialization
 * @returns XAEngineAdaptationCtx* - Pointer to created context
 */
XAAdaptationGstCtx* XAEngineAdapt_Create()
{
    XAEngineAdaptationCtx *pSelf = NULL;
    DEBUG_API("->XAEngineAdapt_Create");

    pSelf = (XAEngineAdaptationCtx*)calloc(1, sizeof(XAEngineAdaptationCtx));
    if ( pSelf)
    {
        if( XAAdaptationBase_Init(&(pSelf->baseObj.baseObj),XAEngineAdaptation)
                    != XA_RESULT_SUCCESS )
            {
                DEBUG_ERR("Failed to init base context!!!");
                free(pSelf);
                pSelf = NULL;
            }
            else
            {
                /* Init internal variables */
            }
    }

    DEBUG_API("<-XAEngineAdapt_Create");
    return (XAAdaptationGstCtx*)pSelf;
}

/*
 * XAresult XAEngineAdapt_PostInit()
 * 2nd phase initialization of engine Adaptation Context
 */
XAresult XAEngineAdapt_PostInit(XAAdaptationGstCtx* bCtx)
{
    XAresult ret = XA_RESULT_SUCCESS;
    GstStateChangeReturn gret = GST_STATE_CHANGE_SUCCESS;
    XAEngineAdaptationCtx* ctx = NULL;

    DEBUG_API("->XAEngineAdapt_PostInit");
    if( !bCtx || bCtx->baseObj.ctxId != XAEngineAdaptation )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEngineAdapt_PostInit");
        return XA_RESULT_PARAMETER_INVALID;
    }
    ctx = (XAEngineAdaptationCtx*)bCtx;

    if ( !ctx )
    {
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("<-XAEngineAdapt_PostInit");
        return XA_RESULT_INTERNAL_ERROR;
    }

    XAAdaptationBase_PostInit( &(ctx->baseObj.baseObj) );

    ctx->baseObj.bin = gst_pipeline_new("engine");
   // gst_bin_add(GST_BIN(ctx->baseObj.bin), gst_element_factory_make( "alsamixer", "alsamixer"));

    if ( !ctx->baseObj.bin )
    {
        DEBUG_ERR("Failed to create alsamixer");
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("<-XAEngineAdapt_PostInit");
        return XA_RESULT_INTERNAL_ERROR;
    }

    /* Create Gst bus listener. */
    ret = XAAdaptationGst_InitGstListener(&(ctx->baseObj));
    if( ret!=XA_RESULT_SUCCESS )
    {
        DEBUG_ERR_A1("Bus listener creation failed!! - (%d)", ret);
        DEBUG_API("<-XAEngineAdapt_PostInit");
        return ret;
    }

    /* Add Engine specific handler */
    if(ctx->baseObj.bus)
    {
        ctx->baseObj.busCb = XAEngineAdapt_GstBusCb;
    }
    else
    {
        DEBUG_ERR("Failed to create message bus");
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("<-XAEngineAdapt_PostInit");
        return XA_RESULT_INTERNAL_ERROR;
    }

    /* roll up bin */
    ctx->baseObj.binWantedState = GST_STATE_PAUSED;
    XAAdaptationGst_PrepareAsyncWait(&(ctx->baseObj));
    gret = gst_element_set_state( GST_ELEMENT(ctx->baseObj.bin), ctx->baseObj.binWantedState);
    if( gret == GST_STATE_CHANGE_ASYNC )
    {
        DEBUG_INFO("Wait for preroll");
        XAAdaptationGst_StartAsyncWait(&(ctx->baseObj));
        DEBUG_INFO("Preroll ready");
    }
    ctx->baseObj.waitingasyncop = XA_BOOLEAN_FALSE;

    //ret = XAStaticCapsAdapt_InitCaps();

    DEBUG_API("<-XAEngineAdapt_PostInit");
    return ret;
}

/*
 * void XAEngineAdapt_Destroy(XAEngineAdaptationCtx* ctx)
 * Destroys Engine Adaptation Context
 * @param ctx - Engine Adaptation context to be destroyed
 */
void XAEngineAdapt_Destroy(XAAdaptationGstCtx* bCtx)
{
	XAEngineAdaptationCtx* ctx = NULL;

	DEBUG_API("->XAEngineAdapt_Destroy");
	if( !bCtx || bCtx->baseObj.ctxId != XAEngineAdaptation )
	{
		DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
		DEBUG_API("<-XAEngineAdapt_Destroy");
		return;
	}
	ctx = (XAEngineAdaptationCtx*)bCtx;

    XAAdaptationBase_Free(&(ctx->baseObj.baseObj));

/*    gst_deinit(); */

    free(ctx);
    ctx = NULL;

    DEBUG_API("<-XAEngineAdapt_Destroy");
}
