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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "xamediarecorder.h"
#include "xaaudioencoderitf.h"
#include "xaconfigextensionsitf.h"
#include "xadynintmgmtitf.h"
#include "xaequalizeritf.h"
#include "xaimagecontrolsitf.h"
#include "xaimageeffectsitf.h"
#include "xaimageencoderitf.h"
#include "xametadatainsertionitf.h"
#include "xarecorditf.h"
#include "xasnapshotitf.h"
#include "xavideoencoderitf.h"
#include "xavideopostprocessingitf.h"
#include "xavolumeitf.h"
#include "xametadataextractionitf.h"
#include "xametadatatraversalitf.h"
#include "xathreadsafety.h"
#include <string.h>
/* Static mapping of enumeration XAMediaRecorderInterfaces to interface iids */
static const XAInterfaceID* xaMediaRecorderItfIIDs[MR_ITFCOUNT]=
{
    &XA_IID_OBJECT,
    &XA_IID_AUDIOENCODER,
    &XA_IID_CONFIGEXTENSION,
    &XA_IID_DYNAMICINTERFACEMANAGEMENT,
    &XA_IID_EQUALIZER,
    &XA_IID_IMAGECONTROLS,
    &XA_IID_IMAGEEFFECTS,
    &XA_IID_IMAGEENCODER,
    &XA_IID_METADATAINSERTION,
    &XA_IID_RECORD,
    &XA_IID_SNAPSHOT,
    &XA_IID_VIDEOENCODER,
    &XA_IID_VIDEOPOSTPROCESSING,
    &XA_IID_VOLUME,
    &XA_IID_METADATAEXTRACTION,
    &XA_IID_METADATATRAVERSAL
};

/* Global methods */

/* XAResult XAMediaRecorderImpl_Create
 * Create object
 */
XAresult XAMediaRecorderImpl_CreateMediaRecorder(XAObjectItf* pRecorder,
                                                 XADataSource* pAudioSrc,
                                                 XADataSource* pImageVideoSrc,
                                                 XADataSink* pDataSnk,
                                                 XAuint32 numInterfaces,
                                                 const XAInterfaceID *pInterfaceIds,
                                                 const XAboolean *pInterfaceRequired)
{
    XAMediaRecorderImpl* pImpl = NULL;
    XAObjectItfImpl* pBaseObj = NULL;
    XAuint8 itfIdx;
    XAMediaType mediaType = XA_MEDIATYPE_UNKNOWN;
    XAresult ret=XA_RESULT_SUCCESS;
    XADataLocator_URI* tmpUri;

    DEBUG_API("->XAMediaRecorderImpl_CreateMediaRecorder");
    XA_IMPL_THREAD_SAFETY_ENTRY( XATSMediaRecorder );

    if( !pRecorder )
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaRecorder );
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAMediaRecorderImpl_CreateMediaRecorder");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* check sink&source parameters */
    ret = XACommon_ValidateDataLocator(3, pAudioSrc, pImageVideoSrc, pDataSnk);
    if(ret!=XA_RESULT_SUCCESS)
    {
        XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaRecorder );
        DEBUG_API("<-XAMediaRecorderImpl_CreateMediaRecorder");
        return ret;
    }

    /* instantiate object implementation */
    pImpl = (XAMediaRecorderImpl*)calloc(1,sizeof(XAMediaRecorderImpl));
    if( !pImpl )
    {

        /* memory allocation failed */
        XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaRecorder );
        DEBUG_ERR("XA_RESULT_MEMORY_FAILURE");
        DEBUG_API("<-XAMediaRecorderImpl_CreateMediaRecorder");
        return XA_RESULT_MEMORY_FAILURE;
    }
    pBaseObj = &pImpl->baseObj;

    /* Initialize base object default implementation */
    XAObjectItfImpl_Init(pBaseObj,
                         MR_ITFCOUNT,
                         xaMediaRecorderItfIIDs,
                         XAMediaRecorderImpl_DoRealize,
                         XAMediaRecorderImpl_DoResume,
                         XAMediaRecorderImpl_FreeResources);

    /* Mark interfaces that need to be exposed */
    /* Implicit and mandated interfaces */
    pBaseObj->interfaceMap[MR_RECORDITF].required = XA_BOOLEAN_TRUE;
    if(pAudioSrc && mediaType!=XA_MEDIATYPE_IMAGE)
    {
        pBaseObj->interfaceMap[MR_AUDIOENCODERITF].required = XA_BOOLEAN_TRUE;
    }
    if(pImageVideoSrc && mediaType!=XA_MEDIATYPE_AUDIO)
    {
        pBaseObj->interfaceMap[MR_VIDEOENCODER].required = XA_BOOLEAN_TRUE;
        pBaseObj->interfaceMap[MR_IMAGEENCODERITF].required = XA_BOOLEAN_TRUE;
        pBaseObj->interfaceMap[MR_SNAPSHOTITF].required = XA_BOOLEAN_TRUE;
    }
    pBaseObj->interfaceMap[MR_DIMITF].required = XA_BOOLEAN_TRUE;

    /* Explicit interfaces */
    if((numInterfaces != 0) && pInterfaceIds && pInterfaceRequired)
    {
        /* check only sink media type, do not care about return value (availability) */
        XACommon_CheckDataSource((XADataSource*)pDataSnk,&mediaType);
        /* Check required interfaces */
        for( itfIdx = 0; itfIdx < numInterfaces; itfIdx++)
        {
            /* If mapEntry is null then required interface is not supported.*/
            XAObjItfMapEntry *entry =
                XAObjectItfImpl_GetItfEntry((XAObjectItf)&(pBaseObj), pInterfaceIds[itfIdx]);
            if( !entry  )
            {
                if( pInterfaceRequired[itfIdx] )
                {
                    /* required interface cannot be accommodated - fail creation */
                    DEBUG_ERR("Required interface not found - abort creation!");
                    ret = XA_RESULT_FEATURE_UNSUPPORTED;
                    break;
                }
                else
                {
                    DEBUG_INFO("Requested (not required) interface not found - continue creation");
                }
            }
            else
            {   /* weed out unsupported content-aware itf's */
                if( ( (mediaType==XA_MEDIATYPE_IMAGE || !pAudioSrc) &&
                             (entry->mapIdx==MR_EQUALIZERITF ||
                              entry->mapIdx==MR_VOLUMEITF ||
                              entry->mapIdx==MR_AUDIOENCODERITF))
                        ||
                    ( (mediaType==XA_MEDIATYPE_AUDIO || !pImageVideoSrc) &&
                             (entry->mapIdx==MR_IMAGECONTROLSITF ||
                              entry->mapIdx==MR_IMAGEEFFECTSITF ||
                              entry->mapIdx==MR_VIDEOPOSTPROCESSINGITF ||
                              entry->mapIdx==MR_VIDEOENCODER ||
                              entry->mapIdx==MR_IMAGEENCODERITF ||
                              entry->mapIdx==MR_SNAPSHOTITF)) )
                {
                    entry->required = XA_BOOLEAN_FALSE;
                    if( pInterfaceRequired[itfIdx] )
                    {
                        DEBUG_ERR("Required interface not supported for given media - abort creation!");
                        ret = XA_RESULT_FEATURE_UNSUPPORTED;
                        break;
                    }
                }
                else
                {
                    entry->required = XA_BOOLEAN_TRUE;
                }

                if(entry->mapIdx==MR_SNAPSHOTITF)
                {
                    DEBUG_ERR("SnapshotItf requested - support still mode");
                    pImpl->recModes |= XA_RECMODE_STILL;
                }
                if(entry->mapIdx==MR_RECORDITF)
                {
                    DEBUG_ERR("RecordItf requested - support stream mode");
                    pImpl->recModes |= XA_RECMODE_STREAM;
                }
            }
        }
    }
    if( pImpl->recModes ==0 )
    {
        DEBUG_ERR("Warning!!! No recording interfaces requested - unable to record!!");
    }
    /*check sink and src parameters*/
    if(ret==XA_RESULT_SUCCESS)
    {
        ret = XACommon_ValidateDataLocator(3, pDataSnk, pAudioSrc, pImageVideoSrc);
        /*datasink ignored if only snapshotitf is used*/
        if(!pDataSnk && (pImpl->recModes & XA_RECMODE_STREAM))
        {
            DEBUG_ERR("No datasink to record to!");
            ret = XA_RESULT_PARAMETER_INVALID;
        }
        if(!pAudioSrc && !pImageVideoSrc)
        {
            DEBUG_ERR("No data sources set!");
            ret = XA_RESULT_PARAMETER_INVALID;
        }
    }

    if(ret!=XA_RESULT_SUCCESS)
    {   /* creation fails */
        XAObjectItfImpl_Destroy((XAObjectItf)&(pBaseObj));
        XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaRecorder );
        DEBUG_API("<-XAMediaRecorderImpl_CreateMediaRecorder");
        return ret;
    }

    /* Mark interfaces that can be handled dynamically */
    /* Mandated dynamic itfs */
    pBaseObj->interfaceMap[MR_METADATAINSERTIONITF].isDynamic = XA_BOOLEAN_TRUE;
    pBaseObj->interfaceMap[MR_EQUALIZERITF].isDynamic = XA_BOOLEAN_TRUE;
    pBaseObj->interfaceMap[MR_IMAGEEFFECTSITF].isDynamic = XA_BOOLEAN_TRUE;


    /* Initialize XAMediaRecorderImpl variables */
    pImpl->audioSrc = pAudioSrc;
    pImpl->dataSnk = pDataSnk;
    pImpl->imageVideoSrc = pImageVideoSrc;
    
    /* Set ObjectItf to point to newly created object */
    *pRecorder = (XAObjectItf)&(pBaseObj->self);    
    
    
    tmpUri = (XADataLocator_URI*)(pImpl->dataSnk->pLocator);
    XAMediaRecorderImpl_DetermineRecordEngine(*pRecorder, tmpUri);
    
    if(pImpl->isMMFRecord)
    {    
        
       pImpl->adaptationCtxMMF = XAMediaRecorderAdaptMMF_Create(pImpl->audioSrc,
                                                             pImpl->imageVideoSrc,
                                                             pImpl->dataSnk,
                                                             pImpl->recModes);      
    }
    else
    {

#ifdef _GSTREAMER_BACKEND_
        pImpl->adaptationCtx = XAMediaRecorderAdapt_Create(pImpl->audioSrc,
                                                           pImpl->imageVideoSrc,
                                                           pImpl->dataSnk,
                                                           pImpl->recModes);
#endif
    }


    XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaRecorder );
    DEBUG_API("<-XAMediaRecorderImpl_CreateMediaRecorder");
    return XA_RESULT_SUCCESS;
}

/* XAResult XAMediaRecorderImpl_QueryNumSupportedInterfaces
 * Statically query supported interfaces
 */
XAresult XAMediaRecorderImpl_QueryNumSupportedInterfaces(XAuint32 *pNumSupportedInterfaces)
{
    DEBUG_API("->XAMediaRecorderImpl_QueryNumSupportedInterfaces");
    if( pNumSupportedInterfaces )
    {
        *pNumSupportedInterfaces = MR_ITFCOUNT;

        DEBUG_API("<-XAMediaRecorderImpl_QueryNumSupportedInterfaces");
        return XA_RESULT_SUCCESS;
    }
    else
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }
}
/* XAResult XAMediaRecorderImpl_QuerySupportedInterfaces
 * Statically query supported interfaces
 */
XAresult XAMediaRecorderImpl_QuerySupportedInterfaces(XAuint32 index,
                                                      XAInterfaceID *pInterfaceId)
{
    DEBUG_API("->XAMediaRecorderImpl_QuerySupportedInterfaces");
    if( index >= MR_ITFCOUNT || !pInterfaceId )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        return XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
        *pInterfaceId = *(xaMediaRecorderItfIIDs[index]);

        DEBUG_API("<-XAMediaRecorderImpl_QuerySupportedInterfaces");
        return XA_RESULT_SUCCESS;
    }
}

/*****************************************************************************
 * base object XAObjectItfImpl methods
 *****************************************************************************/

/* XAresult XAMediaRecorderImpl_DoRealize( XAObjectItf self )
 * Description: Realize all implicit and explicitly wanted interfaces.
 * Create and initialize implementation-specific variables.
 * Called from base object XAObjectItfImpl
 */
XAresult XAMediaRecorderImpl_DoRealize( XAObjectItf self )
{
    XAuint8 itfIdx;
    XAObjectItfImpl* pObj = (XAObjectItfImpl*)(*self);
    XAMediaRecorderImpl* pObjImpl = (XAMediaRecorderImpl*)(pObj);
    XAresult ret = XA_RESULT_SUCCESS;
    XADataLocator_URI* tmpUri;

    DEBUG_API("->XAMediaRecorderImpl_DoRealize");
    XA_IMPL_THREAD_SAFETY_ENTRY( XATSMediaRecorder );

    /* check casting from correct pointer type */
    if( !pObjImpl || pObj != pObjImpl->baseObj.self )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaRecorder );
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* init adaptation */
    if(pObjImpl->isMMFRecord)
    {
       ret = XAMediaRecorderAdaptMMF_PostInit( pObjImpl->adaptationCtxMMF );
    }
    else
    {
#ifdef _GSTREAMER_BACKEND_
       ret = XAMediaRecorderAdapt_PostInit( pObjImpl->adaptationCtx );
#endif
    }

    if( ret != XA_RESULT_SUCCESS )
    {
        XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaRecorder );
        return ret;
    }

    /* Realize all implicit and explicitly wanted interfaces */
    for( itfIdx = 0; itfIdx < MR_ITFCOUNT; itfIdx++ )
    {
        if( !(pObj->interfaceMap[itfIdx].pItf) &&
            pObj->interfaceMap[itfIdx].required )
        {
            void *pItf = NULL;
            switch( itfIdx )
            {
                case MR_DIMITF:
                    pItf = XADIMItfImpl_Create();
                    if(pItf)
                    {
                        XADIMItfImpl_Init(pItf, self,
                                XAMediaRecorderImpl_DoAddItf,
                                XAMediaRecorderImpl_DoResumeItf,
                                XAMediaRecorderImpl_DoRemoveItf);
                    }
                    break;
                case MR_RECORDITF:
                    pItf = XARecordItfImpl_Create( pObjImpl );
                    tmpUri = (XADataLocator_URI*)(pObjImpl->dataSnk->pLocator);
                    XARecordItfImpl_DetermineRecordEngine(pItf, tmpUri);                
                    break;
                    

                case MR_AUDIOENCODERITF:
                    pItf = XAAudioEncoderItfImpl_Create(
#ifdef _GSTREAMER_BACKEND_
                            pObjImpl->adaptationCtx,
#endif
                            pObjImpl
                            );
                    break;
                case MR_SNAPSHOTITF:
                    pItf = XASnapshotItfImpl_Create(
#ifdef _GSTREAMER_BACKEND_
                            pObjImpl->adaptationCtx
#endif
                            );
                    break;
                case MR_VIDEOENCODER:
                    pItf = XAVideoEncoderItfImpl_Create(
#ifdef _GSTREAMER_BACKEND_
                            pObjImpl->adaptationCtx
#endif
                            );
                    break;  
                case MR_IMAGEENCODERITF:
                    pItf = XAImageEncoderItfImpl_Create(
#ifdef _GSTREAMER_BACKEND_
                            pObjImpl->adaptationCtx
#endif
                            );
                    break; 
                case MR_METADATAINSERTIONITF:
                    pItf = XAMetadataInsertionItfImpl_Create(
#ifdef _GSTREAMER_BACKEND_
                            pObjImpl->adaptationCtx
#endif
                            );
                    break;                    
#ifdef _GSTREAMER_BACKEND_
                    case MR_CONFIGEXTENSIONITF:
                    pItf = XAConfigExtensionsItfImpl_Create();
                    XAConfigExtensionsItfImpl_SetContext( pItf, pObjImpl->adaptationCtx);
                    break;
                case MR_EQUALIZERITF:
                    pItf = XAEqualizerItfImpl_Create( pObjImpl->adaptationCtx );
                    break;
                case MR_IMAGECONTROLSITF:
                    pItf = XAImageControlsItfImpl_Create( pObjImpl->adaptationCtx );
                    break;
                case MR_IMAGEEFFECTSITF:
                    pItf = XAImageEffectsItfImpl_Create( pObjImpl->adaptationCtx );
                    break;
                case MR_VIDEOPOSTPROCESSINGITF:
                    pItf = XAVideoPostProcessingItfImpl_Create( pObjImpl->adaptationCtx );
                    break;
                case MR_VOLUMEITF:
                    pItf = XAVolumeItfImpl_Create( pObjImpl->adaptationCtx );
                    break;
                case MR_METADATAEXTRACTIONITF:
                    pItf = XAMetadataExtractionItfImpl_Create( pObjImpl->adaptationCtx );
                    break;
                case MR_METADATATRAVERSALITF:
                    pItf = XAMetadataTraversalItfImpl_Create( pObjImpl->adaptationCtx );
                    break;
#endif                    
                default:
                    break;
            }
            if( !pItf )
            {
                DEBUG_ERR("XA_RESULT_MEMORY_FAILURE");
                /* memory allocation failed */
                XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaRecorder );
                return XA_RESULT_MEMORY_FAILURE;
            }
            else
            {
                pObj->interfaceMap[itfIdx].pItf = pItf;
            }
        }
    }

    pObj->state = XA_OBJECT_STATE_REALIZED;
    DEBUG_API("<-XAMediaRecorderImpl_DoRealize");
    XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaRecorder );
    return XA_RESULT_SUCCESS;
}

/* XAresult XAMediaRecorderImpl_DoResume
 * Description: Resume object from suspended state
 */
XAresult XAMediaRecorderImpl_DoResume(XAObjectItf self)
{
    DEBUG_API("->XAMediaRecorderImpl_DoResume");
    DEBUG_API("<-XAMediaRecorderImpl_DoResume");
    /* This implementation does not support suspended state */
    return XA_RESULT_PRECONDITIONS_VIOLATED;
}

/* void XAMediaRecorderImpl_FreeResources
 * Description: Free all resources reserved at XAMediaRecorderImpl_DoRealize()
 */
void XAMediaRecorderImpl_FreeResources(XAObjectItf self)
{
    XAObjectItfImpl* pObj = (XAObjectItfImpl*)(*self);
    XAMediaRecorderImpl* pObjImpl = (XAMediaRecorderImpl*)(pObj);
    XAuint8 itfIdx;

    DEBUG_API("->XAMediaRecorderImpl_FreeResources");
    XA_IMPL_THREAD_SAFETY_ENTRY_FOR_VOID_FUNCTIONS( XATSMediaRecorder );
#ifdef _GSTREAMER_BACKEND_
    XAMediaRecorderImpl* pImpl = (XAMediaRecorderImpl*)(*self);
    assert( pObj && pImpl && pObj == pObj->self );
#endif
    /* free all allocated interfaces */
    for(itfIdx = 0; itfIdx < MR_ITFCOUNT; itfIdx++)
    {
        void *pItf = pObj->interfaceMap[itfIdx].pItf;
        if(pItf)
        {
            switch(itfIdx)
            {
                case MR_AUDIOENCODERITF:
                    XAAudioEncoderItfImpl_Free(pItf);
                    break;
                case MR_CONFIGEXTENSIONITF:
                    XAConfigExtensionsItfImpl_Free(pItf);
                    break;
                case MR_DIMITF:
                    XADIMItfImpl_Free(pItf);
                    break;
                case MR_EQUALIZERITF:
                    XAEqualizerItfImpl_Free(pItf);
                    break;
                case MR_IMAGECONTROLSITF:
                    XAImageControlsItfImpl_Free(pItf);
                    break;
                case MR_IMAGEEFFECTSITF:
                    XAImageEffectsItfImpl_Free(pItf);
                    break;
                case MR_IMAGEENCODERITF:
                    XAImageEncoderItfImpl_Free(pItf);
                    break;
                case MR_METADATAINSERTIONITF:
                    XAMetadataInsertionItfImpl_Free(pItf);
                    break;
                case MR_RECORDITF:
                    XARecordItfImpl_Free(pItf);
                    break;
                case MR_SNAPSHOTITF:
                    XASnapshotItfImpl_Free(pItf);
                    break;
                case MR_VIDEOENCODER:
                    XAVideoEncoderItfImpl_Free(pItf);
                    break;
                case MR_VIDEOPOSTPROCESSINGITF:
                    XAVideoPostProcessingItfImpl_Free(pItf);
                    break;
                case MR_VOLUMEITF:
                    XAVolumeItfImpl_Free(pItf);
                    break;
                case MR_METADATAEXTRACTIONITF:
                    XAMetadataExtractionItfImpl_Free(pItf);
                    break;
                case MR_METADATATRAVERSALITF:
                    XAMetadataTraversalItfImpl_Free(pItf);
                    break;
                default:
                    break;
            }
            pObj->interfaceMap[itfIdx].pItf = NULL;
        }
    }

#ifdef _GSTREAMER_BACKEND_
    if ( pImpl->adaptationCtx != NULL )
    {
        XAMediaRecorderAdapt_Destroy( pImpl->adaptationCtx );
        pImpl->adaptationCtx = NULL;
    }
#endif
    XAMediaRecorderAdaptMMF_Destroy( pObjImpl->adaptationCtxMMF );
    /* free all other allocated resources*/

    DEBUG_API("<-XAMediaRecorderImpl_FreeResources");
    XA_IMPL_THREAD_SAFETY_EXIT_FOR_VOID_FUNCTIONS( XATSMediaRecorder );
    return;
}

/*****************************************************************************
 * MediaRecorderImpl -specific methods
 *****************************************************************************/

/* XAMediaRecorderImpl_DoAddItf
 * Dynamically add an interface, object specific parts
 */
XAresult XAMediaRecorderImpl_DoAddItf(XAObjectItf self, XAObjItfMapEntry *mapEntry  )
{
#ifdef _GSTREAMER_BACKEND_
    XAObjectItfImpl* pObj = (XAObjectItfImpl*)(*self);
    XAMediaRecorderImpl* pImpl = (XAMediaRecorderImpl*)(pObj);
#endif
    XAresult ret = XA_RESULT_SUCCESS;

    DEBUG_API("->XAMediaRecorderImpl_DoAddItf");
    XA_IMPL_THREAD_SAFETY_ENTRY( XATSMediaRecorder );

    if(mapEntry)
    {
        switch( mapEntry->mapIdx )
        {

        case MR_METADATAINSERTIONITF:
            mapEntry->pItf = XAMetadataInsertionItfImpl_Create(
#ifdef _GSTREAMER_BACKEND_
                    pImpl->adaptationCtx
#endif
                    );
            break;
#ifdef _GSTREAMER_BACKEND_
        case MR_EQUALIZERITF:
            mapEntry->pItf = XAEqualizerItfImpl_Create( pImpl->adaptationCtx );
            break;
        case MR_IMAGEEFFECTSITF:
            mapEntry->pItf = XAImageEffectsItfImpl_Create( pImpl->adaptationCtx );
            break;
#endif
        default:
            DEBUG_ERR("XAMediaRecorderImpl_DoAddItf unknown id");
            ret = XA_RESULT_FEATURE_UNSUPPORTED;
            break;
        }
        
        if( !mapEntry->pItf && ret == XA_RESULT_SUCCESS)
        {
            DEBUG_ERR("XAMediaRecorderImpl_DoAddItf itf creation failed");
            ret = XA_RESULT_MEMORY_FAILURE;
        }
    }
    else
    {
        ret = XA_RESULT_PARAMETER_INVALID;
    }

    XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaRecorder );
    DEBUG_API("<-XAMediaRecorderImpl_DoAddItf");
    return ret;
}

/* XAMediaRecorderImpl_DoResumeItf
 * Try to resume lost interface, object specific parts
 */
XAresult XAMediaRecorderImpl_DoResumeItf(XAObjectItf self, XAObjItfMapEntry *mapEntry  )
{
    XAresult ret;
    /* For now, no difference between suspended and unrealised itfs */
    DEBUG_API("->XAMediaRecorderImpl_DoResumeItf");
    XA_IMPL_THREAD_SAFETY_ENTRY( XATSMediaRecorder );
    ret = XAMediaRecorderImpl_DoAddItf(self,mapEntry);
    XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaRecorder );
    DEBUG_API("<-XAMediaRecorderImpl_DoResumeItf");
    return ret;
}

/* XAMediaRecorderImpl_DoRemoveItf
 * Dynamically remove an interface, object specific parts
 */
XAresult XAMediaRecorderImpl_DoRemoveItf(XAObjectItf self, XAObjItfMapEntry *mapEntry )
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XAMediaRecorderImpl_DoRemoveItf");
    XA_IMPL_THREAD_SAFETY_ENTRY( XATSMediaRecorder );
    if(mapEntry)
    {
        switch( mapEntry->mapIdx )
        {
        case MR_METADATAINSERTIONITF:
            XAMetadataInsertionItfImpl_Free( mapEntry->pItf );
            break;
        case MR_EQUALIZERITF:
            XAEqualizerItfImpl_Free( mapEntry->pItf );
            break;
        case MR_IMAGEEFFECTSITF:
            XAImageEffectsItfImpl_Free( mapEntry->pItf );
            break;
        default:
            DEBUG_ERR("XAMediaRecorderImpl_DoRemoveItf unknown id");
            ret = XA_RESULT_FEATURE_UNSUPPORTED;
            break;
        }
        mapEntry->pItf = NULL;
    }
    else
    {
        ret = XA_RESULT_PARAMETER_INVALID;
    }

    XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaRecorder );
    DEBUG_API("<-XAMediaRecorderImpl_DoRemoveItf");
    return ret;
}

XAresult XAMediaRecorderImpl_DetermineRecordEngine(XAObjectItf self, XADataLocator_URI *uri)
{

  XAresult ret = XA_RESULT_SUCCESS;
  char* tempPtr = NULL;
  char extension[5];
  
  XAObjectItfImpl* pObj = (XAObjectItfImpl*)(*self);
  XAMediaRecorderImpl* pImpl = (XAMediaRecorderImpl*)(pObj);  
  
    
  DEBUG_API("->XAMediaRecorderImpl_DetermineRecordEngine");
    
  //need to move to configuration file and add more in final class
  
  pImpl->isMMFRecord = XA_BOOLEAN_TRUE;
	
  tempPtr = strchr((char*)(uri->URI), '.');
  strcpy(extension, tempPtr);
	
	//TODO:
	//temp everything goes to mmf. in future wav format to go to gst
  //if(!strcmp(extension, ".wav"))  
  //{
  //   pImpl->isMMFRecord = XA_BOOLEAN_FALSE;
  //}

  DEBUG_API("<-XAMediaRecorderImpl_DetermineRecordEngine"); 
  return ret;  
  
}

/* END OF FILE */
