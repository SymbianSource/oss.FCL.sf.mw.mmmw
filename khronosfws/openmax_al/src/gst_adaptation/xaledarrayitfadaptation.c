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

#include "xaledarrayadaptctx.h"
#include "xaledarrayitfadaptation.h"
#include "xaadaptationgst.h"

/*
 * XAresult XALEDArrayItfAdapt_ActivateLEDArray ( XAAdaptationGstCtx *bCtx, XAuint32 lightMask )
 */
XAresult XALEDArrayItfAdapt_ActivateLEDArray ( XAAdaptationGstCtx *bCtx, XAuint32 lightMask )
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API_A1("->XALEDArrayItfAdapt_ActivateLEDArray %ld", lightMask);

    if(!bCtx || bCtx->baseObj.ctxId != XALedArrayAdaptation )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }

    DEBUG_INFO("No support > stubbed.");
    DEBUG_API("<-XALEDArrayItfAdapt_ActivateLEDArray");
    return ret;
}
/*
 * XAresult XALEDArrayItfAdapt_SetColor ( XAAdaptationGstCtx *bCtx, XAuint8 index, const XAHSL * pColor )
 */
XAresult XALEDArrayItfAdapt_SetColor ( XAAdaptationGstCtx *bCtx, XAuint8 index, const XAHSL * pColor )
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XALEDArrayItfAdapt_SetColor");

    if(!bCtx || bCtx->baseObj.ctxId != XALedArrayAdaptation )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }

    DEBUG_INFO("No support > stubbed.");
    DEBUG_API("<-XALEDArrayItfAdapt_SetColor");
    return ret;
}
