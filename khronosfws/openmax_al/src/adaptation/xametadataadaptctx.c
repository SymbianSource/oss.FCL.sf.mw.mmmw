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
#include "XAMetadataAdaptCtx.h"
#include "XAAdaptation.h"

/*
 * XAAdaptationBaseCtx* XAMetadataAdapt_Create()
 * Allocates memory for Metadata Adaptation Context and makes 1st phase initialization
 * @returns XAMetadataAdaptationCtx* - Pointer to created context
 */
XAAdaptationBaseCtx* XAMetadataAdaptCtx_Create(XADataSource* pDataSrc)
{
    /* use media player context with NULL sinks
     * (no specific MDE context needed, because in GStreamer,
     *  playback resources are needed for getting stream tags)
     **/
    return XAMediaPlayerAdapt_Create(pDataSrc,NULL,NULL,NULL,NULL,NULL);
}

/*
 * XAresult XAMetadataAdapt_PostInit()
 * 2nd phase initialization of Metadata Adaptation Context
 */
XAresult XAMetadataAdaptCtx_PostInit(XAAdaptationBaseCtx* bCtx)
{
    /* pipe to media player adaptation */
    return XAMediaPlayerAdapt_PostInit(bCtx);
}

/*
 * void XAMetadataAdapt_Destroy(XAMetadataAdaptationCtx* ctx)
 * Destroys Metadata Adaptation Context
 * @param ctx - Metadata Adaptation context to be destroyed
 */
void XAMetadataAdaptCtx_Destroy(XAAdaptationBaseCtx* bCtx)
{
    /* pipe to media player adaptation */
    XAMediaPlayerAdapt_Destroy(bCtx);
}
