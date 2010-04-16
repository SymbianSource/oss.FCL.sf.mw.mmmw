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
#include "XAOutputMixAdaptCtx.h"
#include "XAAdaptation.h"
#include "XAStaticCapsAdaptation.h"


/*
 * XAAdaptationBaseCtx* XAOutputMixAdapt_Create()
 * @returns XAOutputMixAdaptationCtx* - Pointer to created context
 * Description: Allocates memory for Output Mix Adaptation Context and makes 1st phase initialization
 */
XAAdaptationBaseCtx* XAOutputMixAdapt_Create()
{
    XAOutputMixAdaptationCtx *pSelf = calloc(1, sizeof(XAOutputMixAdaptationCtx));

    DEBUG_API("->XAOutputMixAdapt_Create");
    if ( pSelf)
    {
        if( XAAdaptationBase_Init(&(pSelf->baseObj),XAOutputMixAdaptation)
                    != XA_RESULT_SUCCESS )
        {
            DEBUG_ERR("Failed to init base context!!!");
            free(pSelf);
            pSelf = NULL;
        }
        else
        {
            XAuint32 devId;
            pSelf->connectedObjects = g_array_new (FALSE, TRUE, sizeof (XAOMixAdaptConnObj));
            pSelf->availableDevices = g_array_new (FALSE, TRUE, sizeof (XAuint32));
            /*initially, only alsasink available*/
//            devId = XA_ADAPTID_ALSASINK;
            devId = XA_ADAPTID_DEVSOUNDSINK;
            g_array_append_val(pSelf->availableDevices, devId);
            pSelf->currentrouting = devId;
        }
    }
    DEBUG_API("<-XAOutputMixAdapt_Create");
    return (XAAdaptationBaseCtx* )pSelf;
}

/*
 * XAresult XAOutputMixAdapt_PostInit(XAAdaptationBaseCtx* bCtx)
 * 2nd phase initialization of Output Mix Adaptation Context
 */
XAresult XAOutputMixAdapt_PostInit(XAAdaptationBaseCtx* bCtx)
{
    XAOutputMixAdaptationCtx* ctx = NULL;

    DEBUG_API("->XAOutputMixAdapt_PostInit");
    if(bCtx == NULL || bCtx->ctxId != XAOutputMixAdaptation )
    {
        DEBUG_ERR("Invalid parameter!!");
        DEBUG_API("<-XAOutputMixAdapt_PostInit");
        return XA_RESULT_PARAMETER_INVALID;
    }
    ctx = (XAOutputMixAdaptationCtx*)bCtx;

    /* check and add devices to pSelf->availableDevices if such detection can be done */
    DEBUG_API("<-XAOutputMixAdapt_PostInit");
    return XA_RESULT_SUCCESS;
}

/*
 * void XAOutputMixAdapt_Destroy(XAAdaptationBaseCtx* bCtx)
 * Destroys Output Mix Adaptation Context
 * @param ctx - Output Mix Adaptation context to be destroyed
 */
void XAOutputMixAdapt_Destroy(XAAdaptationBaseCtx* bCtx)
{
    XAOutputMixAdaptationCtx* ctx = NULL;
    DEBUG_API("->XAOutputMixAdapt_Destroy");
    if(bCtx == NULL || bCtx->ctxId != XAOutputMixAdaptation )
    {
        DEBUG_ERR("Invalid parameter!!");
        DEBUG_API("<-XAOutputMixAdapt_Destroy");
        return;
    }
    ctx = (XAOutputMixAdaptationCtx*)bCtx;
    g_array_free(ctx->connectedObjects, TRUE);
    g_array_free(ctx->availableDevices, TRUE);

    XAAdaptationBase_Free( &(ctx->baseObj) );
    free(ctx);
    DEBUG_API("<-XAOutputMixAdapt_Destroy");
}

/*
 * GstElement* XAOutputMixAdapt_GetSink(XAAdaptationBaseCtx* bCtx)
 */
GstElement* XAOutputMixAdapt_GetSink(XAAdaptationBaseCtx* bCtx)
{
    XAOutputMixAdaptationCtx* mixCtx = NULL;
    /* create sink for current routing */
    GstElement* newsink=NULL;
    XAStaticCapsData temp;

    DEBUG_API("->XAOutputMixAdapt_GetSink");
    if(bCtx == NULL || bCtx->ctxId != XAOutputMixAdaptation )
    {
        DEBUG_ERR("Invalid parameter!!");
        DEBUG_API("<-XAOutputMixAdapt_GetSink");
        return NULL;
    }
    mixCtx = (XAOutputMixAdaptationCtx*)bCtx;

    if(!mixCtx)
    {
        DEBUG_ERR("NULL context!");
        return NULL;
    }

    if( XA_RESULT_SUCCESS ==
        XAStaticCapsAdapt_GetCapsById(XACAP_DEVSNK|XACAP_AUDIO, mixCtx->currentrouting, &temp) )
    {
        newsink = gst_element_factory_make((char*)temp.adaptId,(char*)temp.adaptId);
        if(!newsink)
        {
            DEBUG_ERR_A1("Could not create sink \"%s\"!", (char*)temp.adaptId);
        }
    }
    else
    {
        DEBUG_ERR_A1("Could not find audio device by id 0x%x", (int)mixCtx->currentrouting);
    }
    DEBUG_API("<-XAOutputMixAdapt_GetSink");
    return newsink;

}

/*
 * XAresult XAOutputMixAdapt_ConnectObject(XAAdaptationBaseCtx* omCtx, XAAdaptationBaseCtx* bCtx, GstElement* usedMix)
 */
XAresult XAOutputMixAdapt_ConnectObject(XAAdaptationBaseCtx* omCtx, XAAdaptationBaseCtx* bCtx, GstElement* usedMix)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAuint32 i=0;
    XAOutputMixAdaptationCtx* mixCtx =NULL;
    DEBUG_API("->XAOutputMixAdapt_ConnectObject");
    if( !omCtx || omCtx->ctxId!=XAOutputMixAdaptation || !bCtx || !usedMix )
    {
        DEBUG_ERR("Invalid attribute!!");
        DEBUG_API("<-XAOutputMixAdapt_ConnectObject");
        return XA_RESULT_PARAMETER_INVALID;
    }
    mixCtx = (XAOutputMixAdaptationCtx*)omCtx;
    if( !mixCtx || !bCtx || !usedMix )
    {
        DEBUG_ERR("Invalid attribute!!");
        DEBUG_API("<-XAOutputMixAdapt_DisconnectObject");
        return XA_RESULT_PARAMETER_INVALID;
    }
    for(i=0; i<mixCtx->connectedObjects->len; i++)
    {
        if( (g_array_index(mixCtx->connectedObjects, XAOMixAdaptConnObj, i)).ctx == bCtx )
        {
            /*item found already*/
            ret = XA_RESULT_PARAMETER_INVALID;
            break;
        }
    }
    if(i==mixCtx->connectedObjects->len)
    {
        XAOMixAdaptConnObj tmp;
        tmp.ctx= bCtx;
        tmp.currentSink = usedMix;
        g_array_append_val(mixCtx->connectedObjects, tmp);
    }

    DEBUG_API("<-XAOutputMixAdapt_ConnectObject");
    return ret;
}

/*
 * XAresult XAOutputMixAdapt_DisconnectObject(XAAdaptationBaseCtx* omCtx, XAAdaptationBaseCtx* bCtx)
 */
XAresult XAOutputMixAdapt_DisconnectObject(XAAdaptationBaseCtx* omCtx, XAAdaptationBaseCtx* bCtx)
{
    XAuint32 i=0;
    XAOutputMixAdaptationCtx* mixCtx =NULL;
    DEBUG_API("->XAOutputMixAdapt_DisconnectObject");

    if( !omCtx || omCtx->ctxId!=XAOutputMixAdaptation || !bCtx )
    {
        DEBUG_ERR("Invalid attribute!!");
        DEBUG_API("<-XAOutputMixAdapt_DisconnectObject");
        return XA_RESULT_PARAMETER_INVALID;
    }
    mixCtx = (XAOutputMixAdaptationCtx*)omCtx;

    if( !mixCtx || !bCtx )
    {
        DEBUG_ERR("Invalid attribute!!");
        DEBUG_API("<-XAOutputMixAdapt_DisconnectObject");
        return XA_RESULT_PARAMETER_INVALID;
    }
    for(i=0; i<mixCtx->connectedObjects->len; i++)
    {
        XAOMixAdaptConnObj* tmp = &(g_array_index(mixCtx->connectedObjects, XAOMixAdaptConnObj, i));
        if( tmp->ctx == bCtx  )
        {
            g_array_remove_index(mixCtx->connectedObjects, i);
            DEBUG_API("<-XAOutputMixAdapt_DisconnectObject");
            return XA_RESULT_SUCCESS;
        }
    }
    /*did not find, return error*/
    DEBUG_ERR("Object not found!");
    DEBUG_API("<-XAOutputMixAdapt_DisconnectObject");
    return XA_RESULT_PARAMETER_INVALID;
}

