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

#include "XAVibraAdaptCtx.h"
#include "XAVibraItfAdaptation.h"
#include "XAAdaptation.h"

/*
 * XAresult XAVibraItfAdapt_Vibrate( XAAdaptationBaseCtx *bCtx, XAboolean vibrate );
 */
XAresult XAVibraItfAdapt_Vibrate( XAAdaptationBaseCtx *bCtx, XAboolean vibrate )
{
    XAresult ret = XA_RESULT_SUCCESS;
    if(vibrate)
    {
        DEBUG_API("->XAVibraItfAdapt_Vibrate: ON");
    }
    else
        DEBUG_API("->XAVibraItfAdapt_Vibrate: OFF");


    if(!bCtx || bCtx->ctxId != XAVibraAdaptation )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVibraItfAdapt_Vibrate");
        return XA_RESULT_PARAMETER_INVALID;
    }

    DEBUG_INFO("No support > stubbed.");
    DEBUG_API("<-XAVibraItfAdapt_Vibrate");

    return ret;
}
/*
 * XAresult XAVibraItfAdapt_SetFrequency ( XAAdaptationBaseCtx *bCtx,   XAmilliHertz frequency );
 */
XAresult XAVibraItfAdapt_SetFrequency ( XAAdaptationBaseCtx *bCtx,  XAmilliHertz frequency )
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API_A1("->XAVibraItfAdapt_SetFrequency %ld", frequency);

    if(!bCtx || bCtx->ctxId != XAVibraAdaptation )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVibraItfAdapt_SetFrequency");
        return XA_RESULT_PARAMETER_INVALID;
    }

    DEBUG_INFO("No support > stubbed.");
    DEBUG_API("<-XAVibraItfAdapt_SetFrequency");
    return ret;
}
/*
 * XAresult XAVibraItfAdapt_SetIntensity ( XAAdaptationBaseCtx *bCtx,   XApermille intensity );
 */
XAresult XAVibraItfAdapt_SetIntensity ( XAAdaptationBaseCtx *bCtx,  XApermille intensity )
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API_A1("->XAVibraItfAdapt_SetIntensity %d", intensity);

    if(!bCtx || bCtx->ctxId != XAVibraAdaptation )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVibraItfAdapt_SetIntensity");
        return XA_RESULT_PARAMETER_INVALID;
    }

    DEBUG_INFO("No support > stubbed.");
    DEBUG_API("<-XAVibraItfAdapt_SetIntensity");
    return ret;
}
