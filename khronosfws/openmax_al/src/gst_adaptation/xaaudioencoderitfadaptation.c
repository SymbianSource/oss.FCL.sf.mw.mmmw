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
 * Description: AudioEnc Intfc Adaptation for GStreamer
 *
 */

#include <gst/gst.h>
#include "xamediarecorderadaptctx.h"
#include "xaaudioencoderitfadaptation.h"
#include "xaadaptationgst.h"
//#include "XAStaticCapsAdaptation.h"

/*
 * XAresult XAAudioEncoderItfAdapt_SetEncoderSettings( XAAdaptationGstCtx *ctx,
 *                                                     XAAudioEncoderSettings *pSettings )
 * @param XAAdaptationGstCtx *ctx - pointer to Media Recorer adaptation contex
 * @param XAAudioEncoderSettings *pSettings - Settings to be setted for encoder
 * @return XAresult ret - return success value
 * Description: Sets preferred encoder settings for pipeline.
 */
XAresult XAAudioEncoderItfAdapt_SetEncoderSettings(XAAdaptationBaseCtx *ctx,
        XAAudioEncoderSettings *pSettings)
    {
    XAresult ret = XA_RESULT_SUCCESS;
    XAMediaRecorderAdaptationCtx* mCtx = NULL;

    DEBUG_API("->XAAudioEncoderItfAdapt_SetEncoderSettings");
    if (!ctx || (ctx->ctxId != XAMediaRecorderAdaptation) || !pSettings)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAAudioEncoderItfImpl_SetEncoderSettings");
        return XA_RESULT_PARAMETER_INVALID;
        }
    mCtx = (XAMediaRecorderAdaptationCtx*) ctx;

    ret = XAMediaRecorderAdapt_CheckCodec(mCtx, XACAP_AUDIO,
            pSettings->encoderId);
    if (ret == XA_RESULT_SUCCESS)
        {
        /*change of settings - pipeline must be regenerated**/
        mCtx->encodingchanged = XA_BOOLEAN_TRUE;
        memcpy(&mCtx->audioEncSettings, pSettings,
                sizeof(XAAudioEncoderSettings));
        }DEBUG_API("<-XAAudioEncoderItfAdapt_SetEncoderSettings");
    return ret;
    }

XAresult XAAudioEncoderItfAdapt_GetEncoderSettings(XAAdaptationBaseCtx *bCtx,
        XAAudioEncoderSettings *pSettings)
    {
    XAMediaRecorderAdaptationCtx* mCtx = NULL;
    XAresult ret = XA_RESULT_SUCCESS;

    DEBUG_API("->XAAudioEncoderItfAdapt_GetEncoderSettings");
    if (!bCtx || (bCtx->ctxId != XAMediaRecorderAdaptation) || !pSettings)
        {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
        }
    mCtx = (XAMediaRecorderAdaptationCtx*) bCtx;
    memcpy(pSettings, &(mCtx->audioEncSettings),
            sizeof(XAAudioEncoderSettings));
    DEBUG_API("<-XAAudioEncoderItfAdapt_GetEncoderSettings");
    return ret;
    }
