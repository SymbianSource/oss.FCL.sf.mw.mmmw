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
#include <photography.h>
#include "XAAdaptation.h"
#include "XAMediaPlayerAdaptCtx.h"
#include "XAMediaRecorderAdaptCtx.h"
#include "XACameraAdaptCtx.h"
#include "XAImageEffectsItfAdaptation.h"

/*
 * XAresult XAImageEffectsItfAdapt_QuerySupportedImageEffects(XAAdaptationBaseCtx *bCtx,
 *                                                            XAuint32 index,
 *                                                            XAuint32 *pImageEffectId)
 */
XAresult XAImageEffectsItfAdapt_QuerySupportedImageEffects(XAAdaptationBaseCtx *bCtx,
                                                           XAuint32 index,
                                                           XAuint32 *pImageEffectId)
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XAImageEffectsItfAdapt_QuerySupportedImageEffects")

    if(!bCtx || (bCtx->ctxId != XAMediaPlayerAdaptation && bCtx->ctxId != XAMediaRecorderAdaptation
       && bCtx->ctxId != XACameraAdaptation) || index > NUM_SUPPORTED_EFFECTS - 1 || !pImageEffectId)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAImageEffectsItfAdapt_QuerySupportedImageEffects")
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* check which effect is supported */
    if(bCtx->ctxId == XAMediaPlayerAdaptation || bCtx->ctxId == XAMediaRecorderAdaptation)
    {
        switch( index )
        {
            case 0:
                *pImageEffectId = XA_IMAGEEFFECT_MONOCHROME;
                break;
            case 1:
                *pImageEffectId = XA_IMAGEEFFECT_NEGATIVE;
                break;
            case 2:
                *pImageEffectId = XA_IMAGEEFFECT_SEPIA;
                break;
            case 3:
                *pImageEffectId = XA_IMAGEEFFECT_EMBOSS;
                break;
            case 4:
                *pImageEffectId = XA_IMAGEEFFECT_PAINTBRUSH;
                break;
            case 5:
                *pImageEffectId = XA_IMAGEEFFECT_SOLARIZE;
                break;
            case 6:
                *pImageEffectId = XA_IMAGEEFFECT_CARTOON;
                break;
            default:
                ret = XA_RESULT_PARAMETER_INVALID;
                break;
        }
    }
    if(bCtx->ctxId == XACameraAdaptation)
    {
        XACameraAdaptationCtx* mCtx = (XACameraAdaptationCtx*) bCtx;

        if(GST_IS_PHOTOGRAPHY(mCtx->baseObj.bin))
        {
            /* real image effects for camera  */
            switch( index )
            {
                case 0:
                    *pImageEffectId = XA_IMAGEEFFECT_NEGATIVE;
                    break;
                case 1:
                    *pImageEffectId = XA_IMAGEEFFECT_SEPIA;
                    break;
                case 3:
                    *pImageEffectId = XA_IMAGEEFFECT_SOLARIZE;
                    break;
                default:
                    ret = XA_RESULT_PARAMETER_INVALID;
                    break;
            }
        }
        else
        {
            /* Stubbed image effects for camera */
            switch( index )
            {
                case 0:
                    *pImageEffectId = XA_IMAGEEFFECT_MONOCHROME;
                    break;
                case 1:
                    *pImageEffectId = XA_IMAGEEFFECT_NEGATIVE;
                    break;
                case 2:
                    *pImageEffectId = XA_IMAGEEFFECT_SEPIA;
                    break;
                case 3:
                    *pImageEffectId = XA_IMAGEEFFECT_EMBOSS;
                    break;
                case 4:
                    *pImageEffectId = XA_IMAGEEFFECT_PAINTBRUSH;
                    break;
                case 5:
                    *pImageEffectId = XA_IMAGEEFFECT_SOLARIZE;
                    break;
                case 6:
                    *pImageEffectId = XA_IMAGEEFFECT_CARTOON;
                    break;
                default:
                    ret = XA_RESULT_PARAMETER_INVALID;
                    break;
            }
        }
    }

    DEBUG_API("<-XAImageEffectsItfAdapt_QuerySupportedImageEffects")
    return ret;
}

/*
 * XAresult XAImageEffectsItfAdapt_EnableImageEffect(XAAdaptationBaseCtx *bCtx,
 *                                                   XAuint32 imageEffectID)
 */
XAresult XAImageEffectsItfAdapt_EnableImageEffect(XAAdaptationBaseCtx *bCtx,
                                                  XAuint32 imageEffectID)
{
    XAresult ret = XA_RESULT_SUCCESS;
    gdouble tempImageEffect = 0;
    GstElement *balanceElement = NULL;
    DEBUG_API("->XAImageEffectsItfAdapt_EnableImageEffect")

    if(!bCtx || (bCtx->ctxId != XAMediaPlayerAdaptation && bCtx->ctxId != XAMediaRecorderAdaptation
       && bCtx->ctxId != XACameraAdaptation))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAImageEffectsItfAdapt_EnableImageEffect")
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* check which effect is enabled (STUB for player and recorder - all effect are done
     * via videobalance plugin. Used different saturations values). */
    if(bCtx->ctxId == XAMediaPlayerAdaptation || bCtx->ctxId == XAMediaRecorderAdaptation)
    {
        switch( imageEffectID )
        {
            case XA_IMAGEEFFECT_MONOCHROME:
                tempImageEffect = IMAGEEFFECT_MONOCHROME;
                break;
            case XA_IMAGEEFFECT_NEGATIVE:
                tempImageEffect = IMAGEEFFECT_NEGATIVE;
                break;
            case XA_IMAGEEFFECT_SEPIA:
                tempImageEffect = IMAGEEFFECT_SEPIA;
                break;
            case XA_IMAGEEFFECT_EMBOSS:
                tempImageEffect = IMAGEEFFECT_EMBOSS;
                break;
            case XA_IMAGEEFFECT_PAINTBRUSH:
                tempImageEffect = IMAGEEFFECT_PAINTBRUSH;
                break;
            case XA_IMAGEEFFECT_SOLARIZE:
                tempImageEffect = IMAGEEFFECT_SOLARIZE;
                break;
            case XA_IMAGEEFFECT_CARTOON:
                tempImageEffect = IMAGEEFFECT_CARTOON;
                break;
            default:
            {
                DEBUG_ERR("XA_RESULT_FEATURE_UNSUPPORTED");
                DEBUG_API("<-XAImageEffectsItfAdapt_EnableImageEffect")
                return XA_RESULT_FEATURE_UNSUPPORTED;
            }
        }

        if(bCtx->ctxId == XAMediaPlayerAdaptation)
        {
            XAMediaPlayerAdaptationCtx* mCtx = (XAMediaPlayerAdaptationCtx*) bCtx;

            balanceElement = gst_bin_get_by_name( GST_BIN(mCtx->baseObj.bin), "pp_balance");

            if( balanceElement && (imageEffectID != mCtx->imageEffectID) )
            {
                g_object_set(G_OBJECT(balanceElement), "saturation", (gdouble)tempImageEffect, NULL);
                mCtx->imageEffectID  = imageEffectID;
                ret = XAImageEffectsItfAdapt_HandleImageType(bCtx);
            }
        }

        if(bCtx->ctxId == XAMediaRecorderAdaptation)
        {
            XAMediaRecorderAdaptationCtx* mCtx = (XAMediaRecorderAdaptationCtx*) bCtx;

            balanceElement = gst_bin_get_by_name( GST_BIN(mCtx->baseObj.bin), "pp_balance");

            if( balanceElement && (imageEffectID != mCtx->imageEffectID) )
            {
                g_object_set(G_OBJECT(balanceElement), "saturation", (gdouble)tempImageEffect, NULL);
                mCtx->imageEffectID  = imageEffectID;
            }
        }
    }

    if(bCtx->ctxId == XACameraAdaptation )
    {
        XACameraAdaptationCtx* mCtx = (XACameraAdaptationCtx*) bCtx;
        gint gstColorToneMode = GST_PHOTOGRAPHY_COLOUR_TONE_MODE_NORMAL;

        if(GST_IS_PHOTOGRAPHY(mCtx->baseObj.bin))
        {
            switch( imageEffectID )
            {
                case XA_IMAGEEFFECT_NEGATIVE:
                {
                    gstColorToneMode = GST_PHOTOGRAPHY_COLOUR_TONE_MODE_NEGATIVE;
                    break;
                }
                case XA_IMAGEEFFECT_SEPIA:
                {
                    gstColorToneMode = GST_PHOTOGRAPHY_COLOUR_TONE_MODE_SEPIA;
                    break;
                }
                case XA_IMAGEEFFECT_SOLARIZE:
                {
                    gstColorToneMode = GST_PHOTOGRAPHY_COLOUR_TONE_MODE_SOLARIZE;
                    break;
                }
                default:
                {
                    mCtx->imageEffectID = 0;
                    DEBUG_ERR("XA_RESULT_FEATURE_UNSUPPORTED");
                    DEBUG_API("<-XAImageEffectsItfAdapt_EnableImageEffect")
                    return XA_RESULT_FEATURE_UNSUPPORTED;
                }
            }

            if( imageEffectID != mCtx->imageEffectID )
            {
                if( !gst_photography_set_colour_tone_mode( GST_PHOTOGRAPHY(mCtx->baseObj.bin), gstColorToneMode ) )
                {
                    DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
                    DEBUG_API("<-XAImageEffectsItfAdapt_EnableImageEffect")
                    return XA_RESULT_INTERNAL_ERROR;
                }
                mCtx->imageEffectID = imageEffectID;
            }
        }
        else
        {
            GstElement *videoPP = NULL;
            /* Stubbed image effects for camera */
            switch( imageEffectID )
            {
                case XA_IMAGEEFFECT_MONOCHROME:
                    tempImageEffect = IMAGEEFFECT_MONOCHROME;
                    break;
                case XA_IMAGEEFFECT_NEGATIVE:
                    tempImageEffect = IMAGEEFFECT_NEGATIVE;
                    break;
                case XA_IMAGEEFFECT_SEPIA:
                    tempImageEffect = IMAGEEFFECT_SEPIA;
                    break;
                case XA_IMAGEEFFECT_EMBOSS:
                    tempImageEffect = IMAGEEFFECT_EMBOSS;
                    break;
                case XA_IMAGEEFFECT_PAINTBRUSH:
                    tempImageEffect = IMAGEEFFECT_PAINTBRUSH;
                    break;
                case XA_IMAGEEFFECT_SOLARIZE:
                    tempImageEffect = IMAGEEFFECT_SOLARIZE;
                    break;
                case XA_IMAGEEFFECT_CARTOON:
                    tempImageEffect = IMAGEEFFECT_CARTOON;
                    break;
                default:
                {
                    DEBUG_ERR("XA_RESULT_FEATURE_UNSUPPORTED");
                    DEBUG_API("<-XAImageEffectsItfAdapt_EnableImageEffect")
                    return XA_RESULT_FEATURE_UNSUPPORTED;
                }
            }

            videoPP = gst_bin_get_by_name( GST_BIN(mCtx->baseObj.bin), "videopp_camera");
            if ( !videoPP )
            {
                DEBUG_ERR("Could not receive videopp from camerabin!");
            }
            else
            {
               /* Get camera balance element */
               balanceElement = gst_bin_get_by_name( GST_BIN(videoPP), "pp_balance");

               if( balanceElement && (imageEffectID != mCtx->imageEffectID) )
               {
                   g_object_set(G_OBJECT(balanceElement), "saturation", (gdouble)tempImageEffect, NULL);
                   mCtx->imageEffectID  = imageEffectID;
               }
            }

            if ( videoPP )
            {
            	gst_object_unref( videoPP );
            }
        }
    }

    if ( balanceElement )
    {
        gst_object_unref(balanceElement);
    }

    DEBUG_API("<-XAImageEffectsItfAdapt_EnableImageEffect")
    return ret;
}

/*
 * XAresult XAImageEffectsItfAdapt_DisableImageEffect(XAAdaptationBaseCtx *bCtx,
 *                                                    XAuint32 imageEffectID)
 */
XAresult XAImageEffectsItfAdapt_DisableImageEffect(XAAdaptationBaseCtx *bCtx,
                                                   XAuint32 imageEffectID)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAuint32 tempImageEffect = 0;
    GstElement *balanceElement = NULL;
    DEBUG_API("->XAImageEffectsItfAdapt_DisableImageEffect")

    if(!bCtx || (bCtx->ctxId != XAMediaPlayerAdaptation && bCtx->ctxId != XAMediaRecorderAdaptation
       && bCtx->ctxId != XACameraAdaptation))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAImageEffectsItfAdapt_DisableImageEffect")
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* check which effect is disabled (STUB for player and recorder - all effect are done
     * via videobalance plugin. Used saturation default value to disabling effects). */
    if(bCtx->ctxId == XAMediaPlayerAdaptation || bCtx->ctxId == XAMediaRecorderAdaptation)
    {
        switch( imageEffectID )
        {
            case XA_IMAGEEFFECT_MONOCHROME:
            case XA_IMAGEEFFECT_NEGATIVE:
            case XA_IMAGEEFFECT_SEPIA:
            case XA_IMAGEEFFECT_EMBOSS:
            case XA_IMAGEEFFECT_PAINTBRUSH:
            case XA_IMAGEEFFECT_SOLARIZE:
            case XA_IMAGEEFFECT_CARTOON:
                tempImageEffect = NO_IMAGEEFFECT;
                break;
            default:
            {
                DEBUG_ERR("XA_RESULT_FEATURE_UNSUPPORTED")
                DEBUG_API("<-XAImageEffectsItfAdapt_DisableImageEffect")
                return XA_RESULT_FEATURE_UNSUPPORTED;
            }
        }

        if(bCtx->ctxId == XAMediaPlayerAdaptation)
        {
            XAMediaPlayerAdaptationCtx* mCtx = (XAMediaPlayerAdaptationCtx*) bCtx;

            balanceElement = gst_bin_get_by_name( GST_BIN(mCtx->baseObj.bin), "pp_balance");

            if( balanceElement )
            {
                g_object_set(G_OBJECT(balanceElement), "saturation", (gdouble)GST_NO_IMAGEEFFECT, NULL);
                mCtx->imageEffectID = tempImageEffect;
                ret = XAImageEffectsItfAdapt_HandleImageType(bCtx);
            }
        }

        if(bCtx->ctxId == XAMediaRecorderAdaptation)
        {
            XAMediaRecorderAdaptationCtx* mCtx = (XAMediaRecorderAdaptationCtx*) bCtx;

            balanceElement = gst_bin_get_by_name( GST_BIN(mCtx->baseObj.bin), "pp_balance");

            if( balanceElement )
            {
                g_object_set(G_OBJECT(balanceElement), "saturation", (gdouble)GST_NO_IMAGEEFFECT, NULL);
                mCtx->imageEffectID = tempImageEffect;
            }
        }
    }

    if(bCtx->ctxId == XACameraAdaptation)
    {
        XACameraAdaptationCtx* mCtx = (XACameraAdaptationCtx*) bCtx;

        if(GST_IS_PHOTOGRAPHY(mCtx->baseObj.bin))
        {
            switch( imageEffectID )
            {
                case XA_IMAGEEFFECT_NEGATIVE:
                case XA_IMAGEEFFECT_SEPIA:
                case XA_IMAGEEFFECT_SOLARIZE:
                {
                    /* Set color tone to normal */
                    if( !gst_photography_set_colour_tone_mode( GST_PHOTOGRAPHY(mCtx->baseObj.bin),
                        GST_PHOTOGRAPHY_COLOUR_TONE_MODE_NORMAL ) )
                    {
                        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
                        DEBUG_API("<-XAImageEffectsItfAdapt_EnableImageEffect")
                        return XA_RESULT_INTERNAL_ERROR;
                    }
                    mCtx->imageEffectID = NO_IMAGEEFFECT;
                    break;
                }
                default:
                {
                    DEBUG_ERR("XA_RESULT_FEATURE_UNSUPPORTED")
                    DEBUG_API("<-XAImageEffectsItfAdapt_DisableImageEffect")
                    return XA_RESULT_FEATURE_UNSUPPORTED;
                }
            }
        }
        else
        {
            GstElement *videoPP = NULL;
            /* stubbed camera values */
            switch( imageEffectID )
            {
                case XA_IMAGEEFFECT_MONOCHROME:
                case XA_IMAGEEFFECT_NEGATIVE:
                case XA_IMAGEEFFECT_SEPIA:
                case XA_IMAGEEFFECT_EMBOSS:
                case XA_IMAGEEFFECT_PAINTBRUSH:
                case XA_IMAGEEFFECT_SOLARIZE:
                case XA_IMAGEEFFECT_CARTOON:
                    mCtx->imageEffectID = NO_IMAGEEFFECT;
                    break;
                default:
                {
                    DEBUG_ERR("XA_RESULT_FEATURE_UNSUPPORTED")
                    DEBUG_API("<-XAImageEffectsItfAdapt_DisableImageEffect")
                    return XA_RESULT_FEATURE_UNSUPPORTED;
                }
            }

            videoPP = gst_bin_get_by_name( GST_BIN(mCtx->baseObj.bin), "videopp_camera");
            if ( !videoPP )
            {
                DEBUG_ERR("Could not receive videopp from camerabin!");
            }
            else
            {
               /* Get camera balance element */
               balanceElement = gst_bin_get_by_name( GST_BIN(videoPP), "pp_balance");

               if( balanceElement && (imageEffectID != mCtx->imageEffectID) )
               {
                   g_object_set(G_OBJECT(balanceElement), "saturation", (gdouble)GST_NO_IMAGEEFFECT, NULL);
                   mCtx->imageEffectID = tempImageEffect;
               }
            }
            if ( videoPP )
            {
                gst_object_unref(videoPP);
            }
        }
    }

    if ( balanceElement )
    {
        gst_object_unref(balanceElement);
    }

    DEBUG_API("<-XAImageEffectsItfAdapt_DisableImageEffect")
    return ret;
}

/*
 * XAresult XAImageEffectsItfAdapt_IsImageEffectEnabled(XAAdaptationBaseCtx *bCtx,
 *                                                      XAuint32 imageEffectID,
 *                                                      XAboolean *pEnabled)
 */
XAresult XAImageEffectsItfAdapt_IsImageEffectEnabled(XAAdaptationBaseCtx *bCtx,
                                                     XAuint32 imageEffectID,
                                                     XAboolean *pEnabled)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAuint32 tempImageEffect = 0;
    DEBUG_API("->XAImageEffectsItfAdapt_IsImageEffectEnabled")

    if(!bCtx || (bCtx->ctxId != XAMediaPlayerAdaptation && bCtx->ctxId != XAMediaRecorderAdaptation
       && bCtx->ctxId != XACameraAdaptation) || !pEnabled)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAImageEffectsItfAdapt_IsImageEffectEnabled")
        return XA_RESULT_PARAMETER_INVALID;
    }

    if(bCtx->ctxId == XAMediaPlayerAdaptation)
    {
        XAMediaPlayerAdaptationCtx* mCtx = (XAMediaPlayerAdaptationCtx*) bCtx;
        tempImageEffect = mCtx->imageEffectID;
    }

    if(bCtx->ctxId == XAMediaRecorderAdaptation)
    {
        XAMediaRecorderAdaptationCtx* mCtx = (XAMediaRecorderAdaptationCtx*) bCtx;
        tempImageEffect = mCtx->imageEffectID;
    }

    /* check is effect enabled for player and recorder */
    if(bCtx->ctxId == XAMediaPlayerAdaptation || bCtx->ctxId == XAMediaRecorderAdaptation)
    {
        switch( imageEffectID )
        {
            case XA_IMAGEEFFECT_MONOCHROME:
            case XA_IMAGEEFFECT_NEGATIVE:
            case XA_IMAGEEFFECT_SEPIA:
            case XA_IMAGEEFFECT_EMBOSS:
            case XA_IMAGEEFFECT_PAINTBRUSH:
            case XA_IMAGEEFFECT_SOLARIZE:
            case XA_IMAGEEFFECT_CARTOON:
            {
                /* Check is wanted effect currently enabled */
                if( tempImageEffect == imageEffectID )
                {
                    *pEnabled = XA_BOOLEAN_TRUE;
                }
                else
                {
                    *pEnabled = XA_BOOLEAN_FALSE;
                }
                break;
            }
            default:
                ret = XA_RESULT_FEATURE_UNSUPPORTED;
                break;
        }
    }

    if(bCtx->ctxId == XACameraAdaptation)
    {
        XACameraAdaptationCtx* mCtx = (XACameraAdaptationCtx*) bCtx;

        if(GST_IS_PHOTOGRAPHY(mCtx->baseObj.bin))
        {
            switch( imageEffectID )
            {
                case XA_IMAGEEFFECT_NEGATIVE:
                case XA_IMAGEEFFECT_SEPIA:
                case XA_IMAGEEFFECT_SOLARIZE:
                {
                    /* Check is wanted effect currently enabled */
                    if( mCtx->imageEffectID == imageEffectID )
                    {
                        *pEnabled = XA_BOOLEAN_TRUE;
                    }
                    else
                    {
                        *pEnabled = XA_BOOLEAN_FALSE;
                    }
                    break;
                }
                default:
                    ret = XA_RESULT_FEATURE_UNSUPPORTED;
                    break;
            }
        }
        else
        {
            /* stubbed camera values */
            switch( imageEffectID )
            {
                case XA_IMAGEEFFECT_MONOCHROME:
                case XA_IMAGEEFFECT_NEGATIVE:
                case XA_IMAGEEFFECT_SEPIA:
                case XA_IMAGEEFFECT_EMBOSS:
                case XA_IMAGEEFFECT_PAINTBRUSH:
                case XA_IMAGEEFFECT_SOLARIZE:
                case XA_IMAGEEFFECT_CARTOON:
                {
                    /* Check is wanted effect currently enabled */
                    if( mCtx->imageEffectID == imageEffectID )
                    {
                        *pEnabled = XA_BOOLEAN_TRUE;
                    }
                    else
                    {
                        *pEnabled = XA_BOOLEAN_FALSE;
                    }
                    break;
                }
                default:
                    ret = XA_RESULT_FEATURE_UNSUPPORTED;
                    break;
            }
        }
    }

    DEBUG_API("<-XAImageEffectsItfAdapt_IsImageEffectEnabled");
    return ret;
}
/*
 * XAresult XAImageEffectsItfAdapt_ThreadEntry(XAAdaptationBaseCtx *adapCtx)
 * Description: Safety thread entry
 */
XAresult XAImageEffectsItfAdapt_ThreadEntry(XAAdaptationBaseCtx *adaptCtx)
{
    DEBUG_API("->XAImageEffectsItfAdapt_ThreadEntry");
    if((!adaptCtx ) ||( adaptCtx->ctxId != XAMediaPlayerAdaptation &&
                        adaptCtx->ctxId != XAMediaRecorderAdaptation &&
                        adaptCtx->ctxId != XACameraAdaptation ))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAImageEffectsItfAdapt_ThreadEntry");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;

    }

    if ( adaptCtx->ctxId == XAMediaPlayerAdaptation )
    {
        XA_IMPL_THREAD_SAFETY_ENTRY(XATSMediaPlayer);
    }
    else if( adaptCtx->ctxId == XAMediaRecorderAdaptation )
    {
        XA_IMPL_THREAD_SAFETY_ENTRY(XATSMediaRecorder);
    }
    else if (adaptCtx->ctxId == XACameraAdaptation )
    {
        XA_IMPL_THREAD_SAFETY_ENTRY(XATSCamera);
    }

    DEBUG_API("<-XAImageEffectsItfAdapt_ThreadEntry");
    return XA_RESULT_SUCCESS;
}
/*
 * XAresult XAImageEffectsItfAdapt_ThreadExit(XAAdaptationBaseCtx *adaptCtx)
 * Description: Safety thread exit
 */
XAresult XAImageEffectsItfAdapt_ThreadExit(XAAdaptationBaseCtx *adaptCtx)
{
    DEBUG_API("->XAImageEffectsItfAdapt_ThreadExit");
    if((!adaptCtx ) ||( adaptCtx->ctxId != XAMediaPlayerAdaptation &&
                        adaptCtx->ctxId != XAMediaRecorderAdaptation &&
                        adaptCtx->ctxId != XACameraAdaptation ))
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAImageEffectsItfAdapt_ThreadExit");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;

    }

    if ( adaptCtx->ctxId == XAMediaPlayerAdaptation )
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSMediaPlayer);
    }
    else if( adaptCtx->ctxId == XAMediaRecorderAdaptation )
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSMediaRecorder);
    }
    else if (adaptCtx->ctxId == XACameraAdaptation )
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
    }


    DEBUG_API("<-XAImageEffectsItfAdapt_ThreadExit");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XAImageEffectsItfAdapt_HandleImageType(XAAdaptationBaseCtx *bCtx)
 * Description: this run gstreamer for image type
 *
 */
XAresult XAImageEffectsItfAdapt_HandleImageType(XAAdaptationBaseCtx *bCtx)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XADataSource *dataSrc = NULL;
    GstStateChangeReturn gstRet = GST_STATE_CHANGE_SUCCESS;
    GstState gstOrigState = GST_STATE_PLAYING;
    GstState gstTmpState = GST_STATE_PLAYING;
    XAMediaPlayerAdaptationCtx* mCtx = NULL;
    DEBUG_API("->XAImageEffectsItfAdapt_HandleImageType");

    if( !bCtx || (bCtx->ctxId != XAMediaPlayerAdaptation) )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAImageEffectsItfAdapt_HandleImageType")
        return XA_RESULT_PARAMETER_INVALID;
    }

    mCtx = (XAMediaPlayerAdaptationCtx*) bCtx;

    dataSrc = mCtx->xaSource;

    if( dataSrc )
    {
        XAMediaType mediaType;
        ret = XACommon_CheckDataSource(dataSrc, &mediaType);
        if( ret == XA_RESULT_SUCCESS && mediaType == XA_MEDIATYPE_IMAGE )
        {
            gstOrigState = GST_STATE(bCtx->bin);

            DEBUG_INFO_A1("Sending change state request to state %d", GST_STATE_READY);
            gstRet = gst_element_set_state( GST_ELEMENT(bCtx->bin), GST_STATE_READY);
            gstTmpState = GST_STATE(bCtx->bin);

            DEBUG_INFO_A1("Sending change state request to state %d", gstOrigState);
            gstRet = gst_element_set_state( GST_ELEMENT(bCtx->bin), gstOrigState);
        }
    }
    DEBUG_API("<-XAImageEffectsItfAdapt_HandleImageType");
    return ret;
}
