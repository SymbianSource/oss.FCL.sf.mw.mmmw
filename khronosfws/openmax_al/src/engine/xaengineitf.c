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
#include "xaengineitf.h"
#include "xavibraitf.h"
#include "xaledarrayitf.h"
#ifdef _GSTREAMER_BACKEND_  
#include "XAStaticCapsAdaptation.h"
#endif

/*static XAchar implementationText[] = "Implementation does not conform to AL Spec";*/


/**
 * Base interface XAEngineItf implementation
 */
XAresult XAEngineItfImpl_CreateCameraDevice(XAEngineItf self,
                                            XAObjectItf *pDevice,
                                            XAuint32 deviceID,
                                            XAuint32 numInterfaces,
                                            const XAInterfaceID *pInterfaceIds,
                                            const XAboolean *pInterfaceRequired)
{
    return XACameraDeviceImpl_CreateCameraDevice( pDevice, deviceID,numInterfaces,
                                                  pInterfaceIds, pInterfaceRequired );
}

XAresult XAEngineItfImpl_CreateRadioDevice(XAEngineItf self,
                                            XAObjectItf *pDevice,
                                            XAuint32 numInterfaces,
                                            const XAInterfaceID *pInterfaceIds,
                                            const XAboolean *pInterfaceRequired)
{
    return XARadioDeviceImpl_CreateRadioDevice( pDevice, numInterfaces,
                                                pInterfaceIds, pInterfaceRequired );
}

XAresult XAEngineItfImpl_CreateLEDDevice(XAEngineItf self,
                                         XAObjectItf *pDevice,
                                         XAuint32 deviceID,
                                         XAuint32 numInterfaces,
                                         const XAInterfaceID *pInterfaceIds,
                                         const XAboolean *pInterfaceRequired)
{
    return XALEDArrayDeviceImpl_CreateLEDArrayDevice( pDevice, deviceID, numInterfaces,
                                                      pInterfaceIds, pInterfaceRequired );
}

XAresult XAEngineItfImpl_CreateVibraDevice(XAEngineItf self,
                                           XAObjectItf *pDevice,
                                           XAuint32 deviceID,
                                           XAuint32 numInterfaces,
                                           const XAInterfaceID *pInterfaceIds,
                                           const XAboolean *pInterfaceRequired)
{
    return XAVibraDeviceImpl_CreateVibraDevice( pDevice, deviceID, numInterfaces,
                                                pInterfaceIds, pInterfaceRequired);
}

XAresult XAEngineItfImpl_CreateMediaPlayer(XAEngineItf self,
                                           XAObjectItf *pPlayer,
                                           XADataSource *pDataSrc,
                                           XADataSource *pBankSrc,
                                           XADataSink *pAudioSnk,
                                           XADataSink *pImageVideoSnk,
                                           XADataSink *pVibra,
                                           XADataSink *pLEDArray,
                                           XAuint32 numInterfaces,
                                           const XAInterfaceID *pInterfaceIds,
                                           const XAboolean *pInterfaceRequired)
{
    return XAMediaPlayerImpl_CreateMediaPlayer(pPlayer, pDataSrc, pBankSrc, pAudioSnk,
                                             pImageVideoSnk, pVibra, pLEDArray,
                                             numInterfaces, pInterfaceIds, pInterfaceRequired);
}

XAresult XAEngineItfImpl_CreateMediaRecorder(XAEngineItf self,
                                             XAObjectItf * pRecorder,
                                             XADataSource * pAudioSrc,
                                             XADataSource * pImageVideoSrc,
                                             XADataSink * pDataSnk,
                                             XAuint32 numInterfaces,
                                             const XAInterfaceID * pInterfaceIds,
                                             const XAboolean * pInterfaceRequired)
{
    return XAMediaRecorderImpl_CreateMediaRecorder(pRecorder, pAudioSrc, pImageVideoSrc, pDataSnk,
                                                   numInterfaces, pInterfaceIds, pInterfaceRequired);
}

XAresult XAEngineItfImpl_CreateOutputMix(XAEngineItf self,
                                         XAObjectItf *pMix,
                                         XAuint32 numInterfaces,
                                         const XAInterfaceID *pInterfaceIds,
                                         const XAboolean *pInterfaceRequired)
{
    return XAOMixImpl_CreateOutputMix(pMix, numInterfaces,
                                      pInterfaceIds, pInterfaceRequired);
}

XAresult XAEngineItfImpl_CreateMetadataExtractor(XAEngineItf self,
                                                 XAObjectItf *pMetadataExtractor,
                                                 XADataSource *pDataSource,
                                                 XAuint32 numInterfaces,
                                                 const XAInterfaceID *pInterfaceIds,
                                                 const XAboolean *pInterfaceRequired)
{
    return XAMetadataExtractorImpl_Create(pMetadataExtractor, pDataSource,
                                          numInterfaces, pInterfaceIds, pInterfaceRequired);
}

XAresult XAEngineItfImpl_CreateExtensionObject(XAEngineItf self,
                                               XAObjectItf *pObject,
                                               void *pParameters,
                                               XAuint32 objectID,
                                               XAuint32 numInterfaces,
                                               const XAInterfaceID *pInterfaceIds,
                                               const XAboolean *pInterfaceRequired)
{
    DEBUG_API("->XAEngineItfImpl_CreateExtensionObject");
    /* no supported extensions */
    DEBUG_API("<-XAEngineItfImpl_CreateExtensionObject - XA_RESULT_FEATURE_UNSUPPORTED");
    return XA_RESULT_FEATURE_UNSUPPORTED;
}

XAresult XAEngineItfImpl_GetImplementationInfo(XAEngineItf self,
                                               XAuint32 *pMajor,
                                               XAuint32 *pMinor,
                                               XAuint32 *pStep,
                                               const XAchar *pImplementationText)
{
    DEBUG_API("->XAEngineItfImpl_GetImplementationInfo");

    if( !pMajor || !pMinor || !pStep )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEngineItfImpl_GetImplementationInfo");
        return XA_RESULT_PARAMETER_INVALID;
    }
    /* OpenMAX AL API ver 1.0.1 */
    *pMajor = 1;
    *pMinor = 0;
    *pStep = 1;
    /*pImplementationText = (XAchar *) implementationText;*/

    DEBUG_API("<-XAEngineItfImpl_GetImplementationInfo");
    return XA_RESULT_SUCCESS;
}

XAresult XAEngineItfImpl_QuerySupportedProfiles(XAEngineItf self,
                                                XAint16 *pProfilesSupported)
{
    DEBUG_API("->XAEngineItfImpl_QuerySupportedProfiles");

    if( !pProfilesSupported )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEngineItfImpl_QuerySupportedProfiles");
        return XA_RESULT_PARAMETER_INVALID;
    }
    /* NOTE: enable +MIDI when/if XMF issues solved
     *        XA_PROFILES_MEDIA_PLAYER |
     *        XA_PROFILES_MEDIA_PLAYER_RECORDER |
     *        XA_PROFILES_PLUS_MIDI;
     */
    *pProfilesSupported = XA_PROFILES_MEDIA_PLAYER | XA_PROFILES_MEDIA_PLAYER_RECORDER;

    DEBUG_API("<-XAEngineItfImpl_QuerySupportedProfiles");
    return XA_RESULT_SUCCESS;
}

XAresult XAEngineItfImpl_QueryNumSupportedInterfaces(XAEngineItf self,
                                                     XAuint32 objectID,
                                                     XAuint32 *pNumSupportedInterfaces)
{
    XAresult res = XA_RESULT_SUCCESS;
    DEBUG_API("->XAEngineItfImpl_QueryNumSupportedInterfaces");

    if( !pNumSupportedInterfaces )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEngineItfImpl_QueryNumSupportedInterfaces");
        return XA_RESULT_PARAMETER_INVALID;
    }
    *pNumSupportedInterfaces=0;
    switch(objectID)
    {
        case XA_OBJECTID_ENGINE:
            res = XAEngineImpl_QueryNumSupportedInterfaces(
                                            pNumSupportedInterfaces);
            break;
        case XA_OBJECTID_MEDIAPLAYER:
            res = XAMediaPlayerImpl_QueryNumSupportedInterfaces(
                                            pNumSupportedInterfaces);
            break;
        case XA_OBJECTID_MEDIARECORDER:
            res = XAMediaRecorderImpl_QueryNumSupportedInterfaces(
                                            pNumSupportedInterfaces);
            break;
        case XA_OBJECTID_OUTPUTMIX:
            res = XAOMixImpl_QueryNumSupportedInterfaces(
                                            pNumSupportedInterfaces);
            break;
        case XA_OBJECTID_CAMERADEVICE:
            res = XACameraDeviceImpl_QueryNumSupportedInterfaces(
                                            pNumSupportedInterfaces);
            break;
        case XA_OBJECTID_RADIODEVICE:
            res = XARadioDeviceImpl_QueryNumSupportedInterfaces(
                                            pNumSupportedInterfaces);
            break;
        case XA_OBJECTID_LEDDEVICE:
            res = XALEDArrayDeviceImpl_QueryNumSupportedInterfaces(
                                            pNumSupportedInterfaces);
            break;
        case XA_OBJECTID_VIBRADEVICE:
            res = XAVibraDeviceImpl_QueryNumSupportedInterfaces(
                                            pNumSupportedInterfaces);
            break;
        case XA_OBJECTID_METADATAEXTRACTOR:
            res = XAMetadataExtractorImpl_QueryNumSupportedInterfaces(
                                            pNumSupportedInterfaces);
            break;
        default:
            res = XA_RESULT_FEATURE_UNSUPPORTED;
            break;
    }

    DEBUG_API_A1("<-XAEngineItfImpl_QueryNumSupportedInterfaces %lu",res);
    return res;
}

XAresult XAEngineItfImpl_QuerySupportedInterfaces(XAEngineItf self,
                                                  XAuint32 objectID,
                                                  XAuint32 index,
                                                  XAInterfaceID *pInterfaceId)
{
    XAresult res = XA_RESULT_SUCCESS;
    DEBUG_API("->XAEngineItfImpl_QuerySupportedInterfaces");

    if( !pInterfaceId )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEngineItfImpl_QuerySupportedInterfaces");
        return XA_RESULT_PARAMETER_INVALID;
    }
    *pInterfaceId = XA_IID_NULL;
    switch (objectID)
    {
        case XA_OBJECTID_ENGINE:
            res = XAEngineImpl_QuerySupportedInterfaces(
                                                index, pInterfaceId);
            break;
        case XA_OBJECTID_MEDIAPLAYER:
            res = XAMediaPlayerImpl_QuerySupportedInterfaces(
                                                index, pInterfaceId);
            break;
        case XA_OBJECTID_MEDIARECORDER:
            res = XAMediaRecorderImpl_QuerySupportedInterfaces(
                                                index, pInterfaceId);
            break;
        case XA_OBJECTID_OUTPUTMIX:
            res = XAOMixImpl_QuerySupportedInterfaces(
                                                index, pInterfaceId);
            break;
        case XA_OBJECTID_CAMERADEVICE:
            res = XACameraDeviceImpl_QuerySupportedInterfaces(
                                                index, pInterfaceId );
            break;
        case XA_OBJECTID_RADIODEVICE:
            res = XARadioDeviceImpl_QuerySupportedInterfaces(
                                                index, pInterfaceId );
            break;
        case XA_OBJECTID_LEDDEVICE:
            res = XALEDArrayDeviceImpl_QuerySupportedInterfaces(
                                                index, pInterfaceId );
            break;
        case XA_OBJECTID_VIBRADEVICE:
            res = XAVibraDeviceImpl_QuerySupportedInterfaces(
                                                index, pInterfaceId );
            break;
        case XA_OBJECTID_METADATAEXTRACTOR:
            res = XAMetadataExtractorImpl_QuerySupportedInterfaces(
                                                index, pInterfaceId);
            break;
        default:
            res = XA_RESULT_FEATURE_UNSUPPORTED;
    }

    DEBUG_API_A1("<-XAEngineItfImpl_QuerySupportedInterfaces %lu", res);
    return res;
}

XAresult XAEngineItfImpl_QueryLEDCapabilities(XAEngineItf self,
                                              XAuint32 *pIndex,
                                              XAuint32 *pLEDDeviceID,
                                              XALEDDescriptor *pDescriptor)
{
#ifdef _GSTREAMER_BACKEND_  
    XALEDDescriptor descriptor;
#endif    
    DEBUG_API("->XAEngineItfImpl_QueryLEDCapabilities");

    if( !pDescriptor )
    {   /*query number of devices*/
        if( !pIndex )
        {
            DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
            DEBUG_API("<-XAEngineItfImpl_QueryLEDCapabilities");
            return XA_RESULT_PARAMETER_INVALID;
        }
        /* Number of devices */
        *pIndex = 1;
    }
    else
    {   
#ifdef _GSTREAMER_BACKEND_  
        /* query device capabilities */
        if( pIndex )
        {
            if( *pIndex == 0 )
            {
                if( !pLEDDeviceID )
                {
                    DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
                    DEBUG_API("<-XAEngineItfImpl_QueryLEDCapabilities");
                    return XA_RESULT_PARAMETER_INVALID;
                }
                *pLEDDeviceID = XA_ADAPTID_LEDARRAY;
            }
            else
            {
                DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
                DEBUG_API("<-XAEngineItfImpl_QueryLEDCapabilities");
                return XA_RESULT_PARAMETER_INVALID;
            }
        }

        if( *pLEDDeviceID == XA_ADAPTID_LEDARRAY )
        {
            descriptor.colorMask = COLOR_MASK;
            descriptor.ledCount = LED_COUNT;
            descriptor.primaryLED = PRIMARY_LED;
            *pDescriptor = descriptor;
        }
#endif        
    }
    DEBUG_API("<-XAEngineItfImpl_QueryLEDCapabilities");
    return XA_RESULT_SUCCESS;
}

XAresult XAEngineItfImpl_QueryVibraCapabilities(XAEngineItf self,
                                                XAuint32 *pIndex,
                                                XAuint32 *pVibraDeviceID,
                                                XAVibraDescriptor *pDescriptor)
{
#ifdef _GSTREAMER_BACKEND_  
    XAVibraDescriptor descriptor;
#endif
    DEBUG_API("->XAEngineItfImpl_QueryVibraCapabilities");

    if( !pDescriptor  )
    {   /*query number of devices*/
        if( !pIndex )
        {
            DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
            DEBUG_API("<-XAEngineItfImpl_QueryVibraCapabilities");
            return XA_RESULT_PARAMETER_INVALID;
        }
        /* Number of devices */
        *pIndex = 1;
    }
    else
    {   
#ifdef _GSTREAMER_BACKEND_  
        /* query device capabilities */
        if( pIndex )
        {
            if( *pIndex == 0 )
            {
                if( !pVibraDeviceID )
                {
                    DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
                    DEBUG_API("<-XAEngineItfImpl_QueryVibraCapabilities");
                    return XA_RESULT_PARAMETER_INVALID;
                }

                *pVibraDeviceID = XA_ADAPTID_VIBRA;
            }
            else
            {
                DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
                DEBUG_API("<-XAEngineItfImpl_QueryVibraCapabilities");
                return XA_RESULT_PARAMETER_INVALID;
            }
        }

        if( *pVibraDeviceID == XA_ADAPTID_VIBRA )
        {
            descriptor.minFrequency = MIN_FREQUENCY;
            descriptor.maxFrequency = MAX_FREQUENCY;
            descriptor.supportsFrequency = XA_BOOLEAN_TRUE;
            descriptor.supportsIntensity = XA_BOOLEAN_TRUE;
            *pDescriptor = descriptor;
        }
#endif        
    }

    DEBUG_API("<-XAEngineItfImpl_QueryVibraCapabilities");
    return XA_RESULT_SUCCESS;
}

XAresult XAEngineItfImpl_QueryNumSupportedExtensions(XAEngineItf self,
                                                     XAuint32 *pNumExtensions)
{
    DEBUG_API("->XAEngineItfImpl_QuerySupportedExtension");
    if(pNumExtensions==NULL)
    {
       DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
       DEBUG_API("<-XAEngineItfImpl_QuerySupportedExtension");
       return XA_RESULT_PARAMETER_INVALID;
    }
    /* no supported extensions */
    *pNumExtensions=0;
    DEBUG_API("<-XAEngineItfImpl_QuerySupportedExtension");
    return XA_RESULT_SUCCESS;
}

XAresult XAEngineItfImpl_QuerySupportedExtension(XAEngineItf self,
                                               XAuint32 index,
                                               XAchar *pExtensionName,
                                               XAint16 *pNameLength)
{
    DEBUG_API("->XAEngineItfImpl_QuerySupportedExtension");
    /* no supported extensions => index is always wrong */
    DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
    DEBUG_API("<-XAEngineItfImpl_QuerySupportedExtension");
    return XA_RESULT_PARAMETER_INVALID;
}

XAresult XAEngineItfImpl_IsExtensionSupported(XAEngineItf self,
                                            const XAchar *pExtensionName,
                                            XAboolean *pSupported)
{
    DEBUG_API("->XAEngineItfImpl_IsExtensionSupported");
    if(pSupported==NULL)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAEngineItfImpl_IsExtensionSupported");
        return XA_RESULT_PARAMETER_INVALID;
    }
    /* no supported extensions */
    *pSupported=XA_BOOLEAN_FALSE;
    DEBUG_API("<-XAEngineItfImpl_IsExtensionSupported");
    return XA_RESULT_SUCCESS;
}

/**
 * XAEngineItfImpl -specific methods
 **/
XAEngineItfImpl* XAEngineItfImpl_Create()
{
    XAEngineItfImpl* self = (XAEngineItfImpl*)
        calloc(1,sizeof(XAEngineItfImpl));
    DEBUG_API("->XAEngineItfImpl_Create");

    if( self )
    {
        /* init itf default implementation */
        self->itf.CreateCameraDevice = XAEngineItfImpl_CreateCameraDevice;
        self->itf.CreateRadioDevice = XAEngineItfImpl_CreateRadioDevice;
        self->itf.CreateLEDDevice = XAEngineItfImpl_CreateLEDDevice;
        self->itf.CreateVibraDevice = XAEngineItfImpl_CreateVibraDevice;
        self->itf.CreateMediaPlayer = XAEngineItfImpl_CreateMediaPlayer;
        self->itf.CreateMediaRecorder = XAEngineItfImpl_CreateMediaRecorder;
        self->itf.CreateOutputMix = XAEngineItfImpl_CreateOutputMix;
        self->itf.CreateMetadataExtractor = XAEngineItfImpl_CreateMetadataExtractor;
        self->itf.CreateExtensionObject = XAEngineItfImpl_CreateExtensionObject;
        self->itf.GetImplementationInfo = XAEngineItfImpl_GetImplementationInfo;
        self->itf.QuerySupportedProfiles = XAEngineItfImpl_QuerySupportedProfiles;
        self->itf.QueryNumSupportedInterfaces = XAEngineItfImpl_QueryNumSupportedInterfaces;
        self->itf.QuerySupportedInterfaces = XAEngineItfImpl_QuerySupportedInterfaces;
        self->itf.QueryLEDCapabilities = XAEngineItfImpl_QueryLEDCapabilities;
        self->itf.QueryVibraCapabilities = XAEngineItfImpl_QueryVibraCapabilities;
        self->itf.QueryNumSupportedExtensions = XAEngineItfImpl_QueryNumSupportedExtensions;
        self->itf.QuerySupportedExtension = XAEngineItfImpl_QuerySupportedExtension;
        self->itf.IsExtensionSupported = XAEngineItfImpl_IsExtensionSupported;
        self->self = self;
    }
    DEBUG_API("<-XAEngineItfImpl_Create");
    return self;
}

void XAEngineItfImpl_Free(XAEngineItfImpl* self)
{
    DEBUG_API("->XAEngineItfImpl_Free");
    assert(self==self->self);
    free(self);
    DEBUG_API("<-XAEngineItfImpl_Free");
}
