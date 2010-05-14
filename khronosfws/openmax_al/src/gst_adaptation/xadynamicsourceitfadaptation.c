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
#include "xaadaptationgst.h"
#include "xamediaplayeradaptctx.h"
#include "xadynamicsourceitfadaptation.h"

/*
 * XAresult XADynamicSourceItfAdapt_SetSource(XAAdaptationGstCtx *bCtx, XADataSource *pDataSource)
 * @param XAAdaptationGstCtx *bCtx - Adaptation context, this will be casted to correct type regarding to contextID value
 * @param XADataSource *pDataSource - new data source
 * @return XAresult ret - Success value
 */
XAresult XADynamicSourceItfAdapt_SetSource(XAAdaptationGstCtx *bCtx, XADataSource *pDataSource)
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XADynamicSourceItfAdapt_SetSource");
    if( !bCtx || !pDataSource || !pDataSource->pLocator )
    {
        DEBUG_ERR("Invalid NULL parameter");
        ret = XA_RESULT_PARAMETER_INVALID;
    }
    else if(bCtx->baseObj.ctxId == XAMediaPlayerAdaptation || bCtx->baseObj.ctxId == XAMDAdaptation )
    {
        XAMediaPlayerAdaptationCtx* mCtx = (XAMediaPlayerAdaptationCtx*) bCtx;
        GstStateChangeReturn gret;
        GstState origState;
        GstElement* newSource = XAAdaptationGst_CreateGstSource( pDataSource, "datasrc", &(mCtx->isobjsrc), NULL, NULL);
        if(!newSource)
        {
            DEBUG_ERR("Could not create data source!!!");
            return XA_RESULT_CONTENT_NOT_FOUND;
        }
        DEBUG_INFO("Changing Playback Source");
        /* store current state */
        origState = GST_STATE(bCtx->bin);
        /* unroll pipeline */
        bCtx->binWantedState = GST_STATE_NULL;
        XAAdaptationGst_PrepareAsyncWait(bCtx);
        gret = gst_element_set_state( GST_ELEMENT(bCtx->bin), bCtx->binWantedState);
        if( gret == GST_STATE_CHANGE_ASYNC )
        {
            DEBUG_INFO("Wait for unroll");
            XAAdaptationGst_StartAsyncWait(bCtx);
            DEBUG_INFO("Unroll ready");
        }
        else if( gret == GST_STATE_CHANGE_FAILURE )
        {   /*not much we can do*/
             DEBUG_ERR("WARNING: Failed to unroll pipeline!!")
             return XA_RESULT_INTERNAL_ERROR;
        }
        bCtx->waitingasyncop = XA_BOOLEAN_FALSE;

        /* set new source */
        gst_element_unlink(mCtx->source,mCtx->codecbin);
        gst_bin_remove(GST_BIN(bCtx->bin), mCtx->source);
        mCtx->source = newSource;
        gst_bin_add(GST_BIN(bCtx->bin), mCtx->source);
        if(! gst_element_link(mCtx->source, mCtx->codecbin))
            {
                DEBUG_ERR("Could not link source to codecbin!!");
                return XA_RESULT_INTERNAL_ERROR;
            }
        mCtx->xaSource = pDataSource;

        /* restore pipeline state */
        bCtx->binWantedState = origState;
        DEBUG_INFO_A1("Changing pipeline back to state %s",gst_element_state_get_name(origState));
        XAAdaptationGst_PrepareAsyncWait(bCtx);
        gret = gst_element_set_state( GST_ELEMENT(bCtx->bin), bCtx->binWantedState);
        if( gret == GST_STATE_CHANGE_ASYNC )
        {
            DEBUG_INFO("Wait for state change");
            XAAdaptationGst_StartAsyncWait(bCtx);
        }
        else if( gret == GST_STATE_CHANGE_FAILURE )
        {
            DEBUG_ERR("State change FAILED");
            return XA_RESULT_INTERNAL_ERROR;
        }
        bCtx->waitingasyncop = XA_BOOLEAN_FALSE;
        DEBUG_INFO_A1("Pipeline in state %s",gst_element_state_get_name(GST_STATE(bCtx->bin)));

        if( GST_STATE(bCtx->bin) > GST_STATE_READY )
        {   /* let (possible) extraction itf to know new tags  */
            XAAdaptEvent event = {XA_METADATAEVENTS, XA_ADAPT_MDE_TAGS_AVAILABLE, 0, NULL };
            XAAdaptationBase_SendAdaptEvents(&bCtx->baseObj, &event );
        }
    }
    DEBUG_API("<-XADynamicSourceItfAdapt_SetSource");
    return ret;
}
