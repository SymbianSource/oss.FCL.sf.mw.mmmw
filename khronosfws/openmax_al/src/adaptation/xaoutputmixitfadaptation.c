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
#include "XAOutputMixAdaptCtx.h"
#include "XAOutputMixItfAdaptation.h"
#include "XAAdaptation.h"
#include "XAStaticCapsAdaptation.h"

/*
 * XAresult XAOutputMixItfAdapt_GetDestinationOutputDeviceIDs( XAAdaptationBaseCtx *bCtx,
 *                                                              XAint32 * pNumDevices,
 *                                                              XAuint32 * pDeviceIDs )
 * @param XAAdaptationBaseCtx *bCtx - Pointer to OutputMix adaptation context
 * @param XAint32 * pNumDevices - [in] Length of pDeviceIDs array
 *                                [out] number of destination devices
 * @param XAuint32 * pDeviceIDs - List of DeviceIDs
 * @return XAresult success value
 * Description: Returns audio output deviceIDs that are currently connected
 */
XAresult XAOutputMixItfAdapt_GetDestinationOutputDeviceIDs( XAAdaptationBaseCtx *bCtx, XAint32 * pNumDevices, XAuint32 * pDeviceIDs )
{
    XAOutputMixAdaptationCtx* mCtx = NULL;
    XAint32 iterator = 0;
    DEBUG_API_A1("->XAOutputMixItfAdapt_GetDestinationOutputDeviceIDs pNumDevices:%ld",*pNumDevices);
    if(!bCtx || bCtx->ctxId != XAOutputMixAdaptation )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAOutputMixItfAdapt_GetDestinationOutputDeviceIDs");
        return XA_RESULT_PARAMETER_INVALID;
    }
    mCtx = (XAOutputMixAdaptationCtx*) bCtx;
    if(!mCtx)
    {
        DEBUG_ERR("NULL context!");
        return XA_RESULT_PARAMETER_INVALID;
    }

    if ( pNumDevices )
    {
        *pNumDevices = mCtx->availableDevices->len;
    }
    else
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAOutputMixItfAdapt_GetDestinationOutputDeviceIDs");
        return XA_RESULT_PARAMETER_INVALID;
    }

    if(pDeviceIDs)
    {   /*query devices*/
        if ( *pNumDevices < mCtx->availableDevices->len )
        {    /* Lenght of pDeviceIDs is insufficient for all connected audio devices */
            DEBUG_ERR("XA_RESULT_BUFFER_INSUFFICIENT");
            DEBUG_API("<-XAOutputMixItfAdapt_GetDestinationOutputDeviceIDs");
            return XA_RESULT_BUFFER_INSUFFICIENT;
        }

        for ( iterator = 0; iterator <  mCtx->availableDevices->len; iterator++ )
        {
            pDeviceIDs[iterator] = g_array_index(mCtx->availableDevices,XAuint32,iterator);
        }
    }

    DEBUG_API("<-XAOutputMixItfAdapt_GetDestinationOutputDeviceIDs");
    return XA_RESULT_SUCCESS;
}


/*
 * XAresult XAOutputMixItfAdapt_ReRoute( XAAdaptationBaseCtx *bCtx,
 *                                       XAint32 numOutputDevices,
 *                                       XAuint32 * pOutputDeviceIDs)
 * @param XAAdaptationBaseCtx *bCtx - Ponter to OutputMix adaptation context
 * @param XAint32 numOutputDevices - Length of pOutputDeviceIDs
 * @param XAuint32 * pOutputDeviceIDs - List of requested audio output device IDs
 * @return XAresult success value
 * Description: Reroutes audio output to requested destination devices
 */
XAresult XAOutputMixItfAdapt_ReRoute( XAAdaptationBaseCtx *bCtx, XAint32 numOutputDevices, XAuint32 * pOutputDeviceIDs)
{
    XAresult ret = XA_RESULT_SUCCESS;
    gint32 idx = 0;
    XAOutputMixAdaptationCtx* mCtx = NULL;
    XAuint32 devId=0;
    GstElement* newsink = NULL;
    GstElement* current = NULL;
    gchar* currentname= NULL;
    XAStaticCapsData temp;

    DEBUG_API_A1("->XAOutputMixItfAdapt_ReRoute numOutputDevices:%ld",numOutputDevices);
    if(!bCtx ||
        bCtx->ctxId != XAOutputMixAdaptation ||
        !pOutputDeviceIDs )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }

    mCtx = (XAOutputMixAdaptationCtx*) bCtx;

    if( numOutputDevices > 1 )
    {
        /* currently, only routing to single output at time supported */
        return XA_RESULT_FEATURE_UNSUPPORTED;
    }
    devId = pOutputDeviceIDs[0];

    for ( idx = 0; idx < mCtx->connectedObjects->len; idx++ )
    {
        /*find wanted output plugin name*/
        if( XA_RESULT_SUCCESS ==
            XAStaticCapsAdapt_GetCapsById(XACAP_DEVSNK|XACAP_AUDIO, devId, &temp) )
        {
            XAAdaptationBaseCtx* ctx = g_array_index(mCtx->connectedObjects,XAOMixAdaptConnObj,idx).ctx;
            if(!ctx)
            {
                DEBUG_ERR_A1("Context in connected objects array (index %u) is NULL!", idx);
                return XA_RESULT_INTERNAL_ERROR;
            }
            /*check current output plugin name*/
            current = g_array_index(mCtx->connectedObjects,XAOMixAdaptConnObj,idx).currentSink;
            currentname = gst_element_get_name(current);
            DEBUG_INFO_A2("OMix pl%d - current output: \"%s\"", (int)idx, (char*)currentname);
            DEBUG_INFO_A3("OMix pl%d -  wanted output: \"%s\" (id 0x%x)", (int)idx, (char*)temp.adaptId, (int)devId);
            if(strcmp(currentname,(char*)temp.adaptId)!=0)
            {
                if(ret==XA_RESULT_SUCCESS)
                {
                    mCtx->currentrouting = devId;
                    g_array_index(mCtx->connectedObjects,XAOMixAdaptConnObj,idx).currentSink = newsink;
                }
                newsink = gst_bin_get_by_name(GST_BIN(ctx->bin), (char*)temp.adaptId);
                if(!newsink)
                {   /*not existing yet, create*/
                    newsink = gst_element_factory_make((char*)temp.adaptId,(char*)temp.adaptId);
                    gst_bin_add(GST_BIN(ctx->bin), newsink);
                }
                if(!newsink)
                {
                    DEBUG_ERR_A1("Could not create wanted sink \"%s\"!", (char*)temp.adaptId);
                    ret = XA_RESULT_PARAMETER_INVALID;
                }
                else
                {
                    /* switch routing: pause, block, unlink old, link new, unblock pipe, play*/
                    GstPad *sinkpad=NULL, *blockpad=NULL;
                    sinkpad = gst_element_get_static_pad(current,"sink");
                    if(sinkpad)
                    {
                        blockpad = gst_pad_get_peer(sinkpad);
                    }
                    if(blockpad && gst_pad_is_active(blockpad))
                    {
                        DEBUG_INFO("block pad");
                        gst_pad_set_blocked_async(blockpad,TRUE,XAAdaptationBase_PadBlockCb,NULL);
                    }
                    gst_pad_unlink(blockpad, sinkpad);
                    sinkpad = gst_element_get_static_pad(newsink,"sink");
                    gst_pad_link(blockpad, sinkpad);
                    if(gst_pad_is_blocked(blockpad))
                    {
                        DEBUG_INFO("unblock pad");
                        gst_pad_set_blocked_async(blockpad,FALSE,XAAdaptationBase_PadBlockCb,NULL);
                    }
                    /*set sink to same state as the mp bin*/
                    gst_element_sync_state_with_parent(newsink);
                    mCtx->currentrouting = devId;
                    g_array_index(mCtx->connectedObjects,XAOMixAdaptConnObj,idx).currentSink = newsink;
                }
            }
            else
            {
                DEBUG_INFO("No routing switch needed");
            }
        }
        else
        {
            DEBUG_ERR_A1("Could not find audio device by id 0x%x", (int)devId);
            ret = XA_RESULT_PARAMETER_INVALID;
        }
    }

    DEBUG_API("<-XAOutputMixItfAdapt_ReRoute");
    return ret;
}


