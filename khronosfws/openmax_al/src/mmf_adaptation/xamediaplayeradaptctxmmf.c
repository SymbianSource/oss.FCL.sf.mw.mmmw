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
#include <stdlib.h>
#include "xamediaplayeradaptctxmmf.h"
#include "xaadaptationmmf.h"
#include "xaobjectitf.h"
#include "cmmfbackendengine.h"


/*
 * XAMediaPlayerAdaptationMMFCtx* XAMediaPlayerAdapt_Create()
 * Allocates memory for Media Player Adaptation Context and makes 1st phase initialization
 * @param XADataSource *pDataSrc - pointer to OMX-AL data source
 * @param XADataSource *pBankSrc - pointer to instrument bank structure in Mobile DLS, if NULL default will be used.
 * @param XADataSink *pAudioSnk - pointer to OMX-AL audio sink definition
 * @param XADataSink *pImageVideoSnk - pointer to OMX-AL image and video sink definition
 * @returns XAMediaPlayerAdaptationMMFCtx* - Pointer to created context, NULL if error occurs.
 */
XAAdaptationBaseMMFCtx* XAMediaPlayerAdaptMMF_Create(XADataSource *pDataSrc, XADataSource *pBankSrc,
                                                      XADataSink *pAudioSnk, XADataSink *pImageVideoSnk,
                                                      XADataSink *pVibra, XADataSink *pLEDArray)
{
    XAMediaPlayerAdaptationMMFCtx *pSelf = NULL;
    XAuint32 locType = 0;
    XAresult res;
    DEBUG_API("->XAMediaPlayerAdaptMMF_Create");
    
    pSelf = calloc(1, sizeof(XAMediaPlayerAdaptationMMFCtx));
    if ( pSelf)
    {
#ifdef _GSTREAMER_BACKEND_          
        if( XAAdaptationBaseMMF_Init(&(pSelf->baseObj),0)
            != XA_RESULT_SUCCESS )
        {
            DEBUG_ERR("Failed to init base context!!!");
            free(pSelf);
            pSelf = NULL;
        }
        else
#endif            
        {
            pSelf->xaSource = pDataSrc;
            pSelf->xaBankSrc = pBankSrc;
            pSelf->xaAudioSink = pAudioSnk;
            pSelf->xaVideoSink = pImageVideoSnk;
            pSelf->xaLEDArray = pLEDArray;
            pSelf->xaVibra = pVibra;
            pSelf->curMirror = XA_VIDEOMIRROR_NONE;
            pSelf->curRotation = 0;
            pSelf->isobjsrc = XA_BOOLEAN_FALSE;
            pSelf->cameraSinkSynced = XA_BOOLEAN_FALSE;
            /*pSelf->waitData = XA_BOOLEAN_FALSE;*/
        }

  
    if ( pDataSrc )
		{
			locType = *((XAuint32*)(pDataSrc->pLocator));
			if ( locType == XA_DATALOCATOR_IODEVICE  )
			{
			//XADataLocator_IODevice *ioDevice = (XADataLocator_IODevice*)(pDataSrc->pLocator);
			}
		}
    }
    
    if(pSelf)
        {
        res = mmf_backend_engine_init(&(pSelf->mmfContext));
        if(pSelf->mmfContext && res == XA_RESULT_SUCCESS)
            {
            XADataLocator_URI*   tempUri;
            XADataFormat_MIME* tempFormat;
            tempUri = (XADataLocator_URI*)(pSelf->xaSource->pLocator);
            tempFormat = (XADataFormat_MIME*)(pSelf->xaSource->pFormat);
            mmf_set_player_uri(pSelf->mmfContext, (char *)(tempUri->URI), tempFormat->containerType);       
            if(pImageVideoSnk)
              {
              mmf_setup_native_display(pSelf->mmfContext, pImageVideoSnk);
              }
            }
        else
            {
             DEBUG_ERR("Failed to init mmf context!!!");
             free(pSelf);
             pSelf = NULL; 
            }
        }

    DEBUG_API("<-XAMediaPlayerAdaptMMF_Create");
    return (XAAdaptationBaseMMFCtx*)pSelf;
}



/*
 * XAresult XAMediaPlayerAdaptMMF_PostInit()
 * 2nd phase initialization of Media Player Adaptation Context
 * @param XAMediaPlayerAdaptationMMFCtx* ctx - pointer to Media Player adaptation context
 * @return XAresult - Success value
 */
XAresult XAMediaPlayerAdaptMMF_PostInit( XAAdaptationBaseMMFCtx* bCtx )
{
  XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("<-XAMediaPlayerAdaptMMF_PostInit");
    return ret;
}

/*
 * void XAMediaPlayerAdaptMMF_Destroy( XAMediaPlayerAdaptationMMFCtx* ctx )
 * Destroys Media Player Adaptation Context
 * @param ctx - Media Player Adaptation context to be destroyed
 */
void XAMediaPlayerAdaptMMF_Destroy( XAAdaptationBaseMMFCtx* bCtx )
{
    XAMediaPlayerAdaptationMMFCtx* ctx = NULL;

    
    DEBUG_API("->XAMediaPlayerAdaptMMF_Destroy");
    if(bCtx == NULL)
    {
        DEBUG_ERR("Invalid parameter!!");
        DEBUG_API("<-XAMediaPlayerAdaptMMF_Destroy");
        return;
    }
    ctx = (XAMediaPlayerAdaptationMMFCtx*)bCtx;

#ifdef _GSTREAMER_BACKEND_  
    XAAdaptationBase_Free( bCtx );
#endif    
    free(ctx);
    ctx = NULL;

    DEBUG_API("<-XAMediaPlayerAdaptMMF_Destroy");
}


