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

#include <assert.h>
#include <string.h>
#include "xaadaptationcontextbasemmf.h"
#include "xaadaptationmmf.h"
#include "xaobjectitf.h"
#include "xamediaplayeradaptctxmmf.h"

/*
 * XAAdaptationBaseCtx* XAAdaptationBase_Create()
 * 1st phase initialization function for Adaptation Base context structure.
 * Reserves memory for base context and initializes GStreamer FW.
 */
XAresult XAAdaptationBaseMMF_Init( XAAdaptationBaseMMFCtx* pSelf, XAuint32 ctxId )
{
    DEBUG_API("->XAAdaptationBase_Init");

    if ( pSelf )
    {
    pSelf->evtHdlrs = g_array_new (FALSE, FALSE, sizeof (XAAdaptEvtHdlrMMF));
    pSelf->ctxId = ctxId;
    }
    else
    {
        DEBUG_ERR("Invalid Adaptation Base Context.")
        return XA_RESULT_PARAMETER_INVALID;
    }

    DEBUG_API("<-XAAdaptationBase_Init");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XAAdaptationBase_PostInit()
 * 2nd phase initialization for Adaptation Base.
 */
XAresult XAAdaptationBaseMMF_PostInit( XAAdaptationBaseMMFCtx* ctx )
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XAAdaptationBase_PostInit");

   

    DEBUG_API("<-XAAdaptationBase_PostInit");
    return ret;
}

/*
 * void XAAdaptationBase_Free( XAAdaptationBaseCtx* ctx )
 * Frees all Base context variables .
 */
void XAAdaptationBaseMMF_Free( XAAdaptationBaseMMFCtx* ctx )
{
    g_array_free(ctx->evtHdlrs, TRUE);
    DEBUG_API("<-XAAdaptationBase_Free");
}

/*
 * XAresult XAAdaptationBase_AddEventHandler
 * Adds event handler for certain event types.
 */
XAresult XAAdaptationBaseMMF_AddEventHandler( XAAdaptationBaseMMFCtx* ctx, xaAdaptEventHandlerMMF evtHandler,
                                    XAuint32 evtTypes, void *pHandlerCtx )
{
    XAuint32 i;
    XAAdaptEvtHdlrMMF tmp;
    DEBUG_API("->XAAdaptationBase_AddEventHandler");
    if(!ctx)
    {
        DEBUG_ERR("no context");
        return XA_RESULT_PARAMETER_INVALID;
    }

    for(i=0; i<ctx->evtHdlrs->len; i++)
    {
        if( (g_array_index(ctx->evtHdlrs, XAAdaptEvtHdlrMMF, i)).handlerfunc == evtHandler )
        {
            return XA_RESULT_PARAMETER_INVALID;
        }
    }
    tmp.handlerfunc = evtHandler;
    tmp.handlercontext = pHandlerCtx;
    tmp.eventtypes = evtTypes;
    g_array_append_val(ctx->evtHdlrs, tmp);
    DEBUG_API("<-XAAdaptationBase_AddEventHandler");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XAAdaptationBase_RemoveEventHandler
 * Removes event handler for certain event types.
 */
XAresult XAAdaptationBaseMMF_RemoveEventHandler( XAAdaptationBaseMMFCtx* ctx, xaAdaptEventHandlerMMF evtHandler)
{
    XAuint32 i;
    DEBUG_API("->XAAdaptationBase_RemoveEventHandler");
    if(!ctx)
    {
        DEBUG_ERR("no context");
        return XA_RESULT_PARAMETER_INVALID;
    }
    for(i=0; i<ctx->evtHdlrs->len; i++)
    {
        if( (g_array_index(ctx->evtHdlrs, XAAdaptEvtHdlrMMF, i)).handlerfunc == evtHandler )
        {
            g_array_remove_index(ctx->evtHdlrs, i);
            return XA_RESULT_SUCCESS;
        }
    }
    DEBUG_API("<-XAAdaptationBase_RemoveEventHandler");
    /*did not find, return error*/
    return XA_RESULT_PARAMETER_INVALID;
}

void XAAdaptationBaseMMF_SendAdaptEvents(XAAdaptationBaseMMFCtx* ctx, XAAdaptEventMMF* event)
{
    XAuint32 i;
    XAAdaptEvtHdlrMMF* tmp;
    for(i=0; i<ctx->evtHdlrs->len; i++)
    {
        tmp = &g_array_index(ctx->evtHdlrs, XAAdaptEvtHdlrMMF, i);
        if( tmp->eventtypes & event->eventtype )
        {
            (tmp->handlerfunc)(tmp->handlercontext, event);
        }
    }
}
