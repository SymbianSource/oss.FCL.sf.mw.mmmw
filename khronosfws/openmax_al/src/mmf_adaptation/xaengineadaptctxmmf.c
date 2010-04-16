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
#include "xaengineadaptctxmmf.h"
#include "xaadaptationmmf.h"

/*
 * XAAdaptationBaseCtx* XAEngineAdaptMMF_Create()
 * Allocates memory for Engine Adaptation Context and makes 1st phase initialization
 * @returns XAEngineAdaptationCtx* - Pointer to created context
 */
XAAdaptationBaseMMFCtx* XAEngineAdaptMMF_Create()
{
    XAEngineAdaptationMMFCtx *pSelf = NULL;
    DEBUG_API("->XAEngineAdaptMMF_Create");

    pSelf = calloc(1, sizeof(XAEngineAdaptationMMFCtx));
    if ( pSelf)
    {
        if( XAAdaptationBaseMMF_Init(&(pSelf->baseObj),XAEngineAdaptationMMF)
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

    DEBUG_API("<-XAEngineAdaptMMF_Create");
    return (XAAdaptationBaseMMFCtx*)pSelf;
}

/*
 * XAresult XAEngineAdaptMMF_PostInit()
 * 2nd phase initialization of engine Adaptation Context
 */
XAresult XAEngineAdaptMMF_PostInit(XAAdaptationBaseMMFCtx* bCtx)
{
    XAresult ret = XA_RESULT_SUCCESS;

    DEBUG_API("<-XAEngineAdaptMMF_PostInit");
    return ret;
}

/*
 * void XAEngineAdapt_Destroy(XAEngineAdaptationCtx* ctx)
 * Destroys Engine Adaptation Context
 * @param ctx - Engine Adaptation context to be destroyed
 */
void XAEngineAdaptMMF_Destroy(XAAdaptationBaseMMFCtx* bCtx)
{
	XAEngineAdaptationMMFCtx* ctx = NULL;

	DEBUG_API("->XAEngineAdaptMMF_Destroy");
//	if( !bCtx || bCtx->ctxId != XAEngineAdaptation )
//	{
//		DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
//		DEBUG_API("<-XAEngineAdaptMMF_Destroy");
//		return;
//	}
	ctx = (XAEngineAdaptationMMFCtx*)bCtx;
  
  XAAdaptationBaseMMF_Free(&(ctx->baseObj));


    free(ctx);
    ctx = NULL;

    DEBUG_API("<-XAEngineAdaptMMF_Destroy");
}
