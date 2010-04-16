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
#include <mixer.h>
#include "XADeviceVolumeItfAdaptation.h"
#include "XAEngineAdaptCtx.h"
#include "XAAdaptation.h"
#include "XAStaticCapsAdaptation.h"


/* XAresult XADeviceVolumeItfAdapt_SetVolume
 * Description: Sets the device's volume.
 */
XAresult XADeviceVolumeItfAdapt_SetVolume(XAAdaptationBaseCtx *bCtx, XAuint32 deviceID, XAint32 volume)
{
    XAEngineAdaptationCtx* ctx = NULL;
    GstElement* amixer = NULL;
    GstMixerTrack *mixerTrack = NULL;
    const GList *gList = NULL;
    gint volumeIdx = 0;

    DEBUG_API("->XADeviceVolumeItfAdapt_SetVolume");

    if(!bCtx )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XADeviceVolumeItfAdapt_SetVolume");
        return XA_RESULT_PARAMETER_INVALID;
    }

    ctx = (XAEngineAdaptationCtx*) bCtx;

    amixer = gst_bin_get_by_name( GST_BIN(ctx->baseObj.bin), "alsamixer");
    if( !amixer )
    {
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("<-XADeviceVolumeItfAdapt_SetVolume");
        return XA_RESULT_INTERNAL_ERROR;
    }

    gList = gst_mixer_list_tracks( GST_MIXER(amixer) );
    if( !gList )
    {
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("<-XADeviceVolumeItfAdapt_SetVolume");
        return XA_RESULT_INTERNAL_ERROR;
    }

    while( gList )
    {
        mixerTrack = (GstMixerTrack*)gList->data;
        if( !mixerTrack )
        {
            DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
            DEBUG_API("<-XADeviceVolumeItfAdapt_SetVolume");
            return XA_RESULT_INTERNAL_ERROR;
        }

        if( ((mixerTrack->flags & GST_MIXER_TRACK_INPUT ) && deviceID == XA_DEFAULTDEVICEID_AUDIOINPUT) ||
//            ((mixerTrack->flags & GST_MIXER_TRACK_INPUT ) && deviceID == XA_ADAPTID_ALSASRC) || //krishna
                ((mixerTrack->flags & GST_MIXER_TRACK_INPUT ) && deviceID == XA_ADAPTID_DEVSOUNDSRC) ||
            ((mixerTrack->flags & GST_MIXER_TRACK_INPUT ) && deviceID == XA_ADAPTID_AUDIOTESTSRC) ||
            ((mixerTrack->flags & GST_MIXER_TRACK_OUTPUT ) && deviceID == XA_DEFAULTDEVICEID_AUDIOOUTPUT) ||
            ((mixerTrack->flags & GST_MIXER_TRACK_OUTPUT ) && deviceID == XA_ADAPTID_JACKSINK) ||
//            ((mixerTrack->flags & GST_MIXER_TRACK_OUTPUT ) && deviceID == XA_ADAPTID_ALSASINK) )
            ((mixerTrack->flags & GST_MIXER_TRACK_OUTPUT ) && deviceID == XA_ADAPTID_DEVSOUNDSINK) )
        {
            gint *gVolume = calloc(mixerTrack->num_channels, sizeof(gint) );
            if( !gVolume )
            {
                DEBUG_ERR("XA_RESULT_MEMORY_FAILURE");
                DEBUG_API("<-XADeviceVolumeItfAdapt_SetVolume");
                return XA_RESULT_MEMORY_FAILURE;
            }

            for( volumeIdx = 0; volumeIdx < mixerTrack->num_channels; volumeIdx++ )
            {
                /* Set same volume level for all channels */
                gVolume[volumeIdx] = (gint)volume;
            }

            /* found master track */
            gst_mixer_set_volume( GST_MIXER(amixer), mixerTrack, gVolume );
            free( gVolume );
            gVolume = NULL;
            break;
        }
        gList = g_list_next(gList);
    }
    if ( amixer )
    {
        gst_object_unref( GST_OBJECT(amixer));
    }

    DEBUG_API("<-XADeviceVolumeItfAdapt_SetVolume");
    return XA_RESULT_SUCCESS;
}

/* XAresult XADeviceVolumeItfAdapt_IsDeviceIDSupported
 * Description: Check is request device ID supported.
 */
XAresult XADeviceVolumeItfAdapt_IsDeviceIDSupported(XAAdaptationBaseCtx *bCtx, XAuint32 deviceID, XAboolean *isSupported)
{
    DEBUG_API("->XADeviceVolumeItfAdapt_IsDeviceIDSupported");

    if(!bCtx || !isSupported )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XADeviceVolumeItfAdapt_IsDeviceIDSupported");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* check is device ID supported or not supported */
//    if( deviceID == XA_DEFAULTDEVICEID_AUDIOINPUT || deviceID == XA_ADAPTID_ALSASRC ||
    if( deviceID == XA_DEFAULTDEVICEID_AUDIOINPUT || deviceID == XA_ADAPTID_DEVSOUNDSRC ||
        deviceID == XA_ADAPTID_AUDIOTESTSRC || deviceID == XA_DEFAULTDEVICEID_AUDIOOUTPUT ||
//        deviceID == XA_ADAPTID_JACKSINK || deviceID == XA_ADAPTID_ALSASINK )
        deviceID == XA_ADAPTID_JACKSINK || deviceID == XA_ADAPTID_DEVSOUNDSINK )
    {
        *isSupported = XA_BOOLEAN_TRUE;
    }
    else
    {
        *isSupported = XA_BOOLEAN_FALSE;
    }

    DEBUG_API("<-XADeviceVolumeItfAdapt_IsDeviceIDSupported");
    return XA_RESULT_SUCCESS;
}
