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
#include <assert.h>
#include "xamediarecorderadaptctxmmf.h"
#include "xaadaptationmmf.h"
#include "cmmfbackendengine.h"
//#include "XAMetadataAdaptation.h"
//#include "XAStaticCapsAdaptation.h"


#define XA_ADAPTID_UNINITED 0
#ifdef _GSTREAMER_BACKEND_ 
extern XAboolean cameraRealized;
#endif
//extern XACameraAdaptationCtx_* cameraCtx;



/*
 * XAAdaptationBaseCtx* XAMediaRecorderAdaptMMF_Create()
 * Allocates memory for Media Recorder Adaptation Context and makes 1st phase initialization
 * @param XADataSource *pAudioSrc - pointer to OMX-AL audio source
 * @param XADataSource *pImageVideoSrc - pointer image/video source
 * @param XADataSink *pDataSnk - pointer to OMX-AL sink
 * @returns XAMediaRecorderAdaptationMMFCtx* - Pointer to created context, NULL if error occurs.
 */
XAAdaptationBaseMMFCtx* XAMediaRecorderAdaptMMF_Create(XADataSource* pAudioSrc,
        XADataSource* pImageVideoSrc, XADataSink* pDataSnk, XAuint8 recModes)
    {
    XAMediaRecorderAdaptationMMFCtx *pSelf = NULL;
    XAuint32 locType = 0;
    XAresult res;
    XADataLocator_IODevice *ioDevice;
    DEBUG_API("->XAMediaRecorderAdaptMMF_Create");

    pSelf = calloc(1, sizeof(XAMediaRecorderAdaptationMMFCtx));
    if (pSelf)
        {            
        if (XAAdaptationBaseMMF_Init(&(pSelf->baseObj),
                XAMediaRecorderAdaptationMMF) != XA_RESULT_SUCCESS)
            {
            DEBUG_ERR("Failed to init base context!!!");
            free(pSelf);
            pSelf = NULL;
            }
        else        
            {
            pSelf->xaAudioSource = pAudioSrc;
            pSelf->xaVideoSource = pImageVideoSrc;
            pSelf->xaSink = pDataSnk;
            pSelf->xaRecordState = XA_RECORDSTATE_STOPPED;
            pSelf->curMirror = XA_VIDEOMIRROR_NONE;
            pSelf->curRotation = 0;
            pSelf->recModes = recModes;
            pSelf->isRecord = XA_BOOLEAN_FALSE;

            /* defaults from API spec */
            pSelf->imageEncSettings.width = 640;
            pSelf->imageEncSettings.height = 480;
            pSelf->imageEncSettings.compressionLevel = 0;
            pSelf->imageEncSettings.encoderId = XA_IMAGECODEC_JPEG;
            pSelf->imageEncSettings.colorFormat = XA_COLORFORMAT_UNUSED;
            /* no specified defaults for rest, determined later from container type */
            pSelf->videoEncSettings.encoderId = XA_ADAPTID_UNINITED;
            pSelf->videoEncSettings.width = 640;
            pSelf->videoEncSettings.height = 480;
            pSelf->videoEncSettings.frameRate = 15;
            pSelf->audioEncSettings.encoderId = XA_ADAPTID_UNINITED;
            pSelf->audioEncSettings.channelsIn = 2;
            pSelf->audioEncSettings.channelsOut = 2;
            pSelf->audioEncSettings.bitsPerSample = 8;
            pSelf->audioEncSettings.bitRate = 128;
            pSelf->audioEncSettings.sampleRate = 44100;
            }

        if (pImageVideoSrc)
            {
            locType = *((XAuint32*) (pImageVideoSrc->pLocator));
            if (locType == XA_DATALOCATOR_IODEVICE)
                {
                ioDevice
                        = (XADataLocator_IODevice*) (pImageVideoSrc->pLocator);
                if (ioDevice->deviceType == XA_IODEVICE_CAMERA
#ifdef _GSTREAMER_BACKEND_ 
                        && !cameraRealized
#endif                        
                        )
                    {
                    DEBUG_ERR("Preconditions violated - Camera object not realized");
                    XAAdaptationBaseMMF_Free(&pSelf->baseObj);
                    free(pSelf);
                    pSelf = NULL;
                    }
                }
            }
        }
    
    if(pSelf)
        {
        res = mmf_backend_engine_init(&(pSelf->mmfContext) );
        if(!(pSelf->mmfContext) || (res != XA_RESULT_SUCCESS))
            {
            DEBUG_ERR("Failed to init mmf context!!!");
            free(pSelf);
            pSelf = NULL;           
            }   
        }
    
    DEBUG_API("<-XAMediaRecorderAdaptMMF_Create");
    return (XAAdaptationBaseMMFCtx*) pSelf;
    }

/*
 * XAresult XAMediaRecorderAdaptMMF_PostInit()
 * 2nd phase initialization of Media Recorder Adaptation Context
 * @param XAAdaptationBaseCtx* ctx - pointer to Media Recorder adaptation context
 * @return XAresult - Success value
 */
XAresult XAMediaRecorderAdaptMMF_PostInit(XAAdaptationBaseMMFCtx* bCtx)
    {

    XAresult ret = XA_RESULT_SUCCESS;
    XADataLocator_URI*   tempUri;
    XADataFormat_MIME* tempFormat;
    XAMediaRecorderAdaptationMMFCtx *pSelf = (XAMediaRecorderAdaptationMMFCtx*)bCtx;
    DEBUG_API("->XAMediaRecorderAdapt_PostInit");
    if(pSelf->mmfContext)
        {
        tempUri = (XADataLocator_URI*)(pSelf->xaSink->pLocator);
        tempFormat = (XADataFormat_MIME*)(pSelf->xaSink->pFormat);
        mmf_set_recorder_uri(pSelf->mmfContext, (char *)(tempUri->URI), tempFormat->containerType);  
        mmf_set_adapt_context(pSelf->mmfContext, &(pSelf->baseObj));
        }

    DEBUG_API("<-XAMediaRecorderAdapt_PostInit");
    return ret;
    }

/*
 * void XAMediaRecorderAdaptMMF_Destroy( XAAdaptationBaseMMFCtx* bCtx )
 * Destroys Media Recorder Adaptation Context
 * @param ctx - Media Recorder Adaptation context to be destroyed
 */
void XAMediaRecorderAdaptMMF_Destroy(XAAdaptationBaseMMFCtx* bCtx)
    {
    XAMediaRecorderAdaptationMMFCtx* ctx = NULL;
    
    DEBUG_API("->XAMediaRecorderAdaptMMF_Destroy");

    if (bCtx == NULL)
    {
        DEBUG_ERR("Invalid parameter!!");DEBUG_API("<-XAMediaRecorderAdaptMMF_Destroy");
        return;
    }
    ctx = (XAMediaRecorderAdaptationMMFCtx*) bCtx;

    if(ctx->mmfContext)
        {
        mmf_close(ctx->mmfContext);
        }
    XAAdaptationBaseMMF_Free(bCtx);

    free(ctx);
    ctx = NULL;

    DEBUG_API("<-XAMediaRecorderAdaptMMF_Destroy");
    }

/***************** INTERNAL FUNCTIONS *******************************/

