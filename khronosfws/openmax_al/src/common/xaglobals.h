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
/*All global definitions and declarations here */

#ifndef XAGLOBALS_H
#define XAGLOBALS_H

#include "openmaxalwrapper.h"
#include "xadebug.h"

/**
GLOBAL ENUMERATIONS
 */
/**
 * enumeration for different adaptation event types.
 * add more types when needed, use single bit values.
 **/
typedef enum {
    XA_GENERICEVENTS = 0x1,
    XA_PLAYITFEVENTS = 0x2,
    XA_RECORDITFEVENTS = 0x4,
    XA_SNAPSHOTITFEVENTS = 0x8,
    XA_OUTPUTMIXITFEVENTS = 0x10,
    XA_CAMERAITFEVENTS = 0x20,
    XA_PREFETCHITFEVENTS = 0x40,
    XA_RADIOITFEVENTS = 0x80,
    XA_RDSITFEVENTS = 0x100,
    XA_METADATAEVENTS = 0x200,
    XA_ADDMORETYPES  = 0x400
}XAAdaptEventTypes;

#define XA_ADAPT_PU_INTERVAL 50                     /* position update interval */
#define XA_ADAPT_ASYNC_TIMEOUT 3000                 /* timeout to wait async events */
#define XA_ADAPT_ASYNC_TIMEOUT_SHORT 1000           /* timeout to wait async events */
#define XA_ADAPT_ASYNC_TIMEOUT_SHORT_NSEC 1000000   /* timeout to wait async events */

#define XA_ADAPT_POSITION_UPDATE_EVT 0xf0           /* position update event */
#define XA_ADAPT_SNAPSHOT_TAKEN 0xf1                /* snapshot taken event */
#define XA_ADAPT_SNAPSHOT_INITIATED 0xf2            /* snapshot intitiated event */
#define XA_ADAPT_MDE_TAGS_AVAILABLE 0xf3            /* metadata taglist changed */
#define XA_ADAPT_OMIX_DEVICESET_CHANGED 0xf4        /* Output mix device changed event */
#define XA_ADAPT_RADIO_FREQUENCY_CHANGED 0xf6       /* Radio frequency changed event */
#define XA_ADAPT_RADIO_FREQUENCY_RANGE_CHANGED 0xf7 /* Radio frequency range changed event */
#define XA_ADAPT_RADIO_SEEK_COMPLETE 0xf8           /* Radio seek complete changed event */
#define XA_ADAPT_RDS_GET_ODA_GROUP_DONE 0xf9        /* RDS get oda group done event */
#define XA_ADAPT_BUFFERING 0xfa
#define XA_ADAPT_MDE_TAGS_WRITTEN 0xfb
/* TYPEDEFS */

#define RADIO_DEFAULT_FREQ_RANGE XA_FREQRANGE_FMEUROAMERICA
#define RADIO_DEFAULT_FREQ 88000000


typedef enum
{
    XA_UNDEFINED = 0,
    XA_READBUFFER,
    XA_WRITEBUFFER,
    XA_READ,
    XA_WRITE
} XAConfigExtensionCpKey;

typedef enum
{
    XA_MEDIATYPE_AUDIO = 0,
    XA_MEDIATYPE_VIDEO,
    XA_MEDIATYPE_IMAGE,
    XA_MEDIATYPE_UNKNOWN
} XAMediaType;

#define CPCONFIGKEY "TestAppPipeConfig"
/**
 * GLOBAL METHODS
 */


/*
 * Engine
 */
XAresult XAEngineImpl_Create(XAObjectItf *pEngine,
                             XAuint32 numOptions,
                             const XAEngineOption *pEngineOptions,
                             XAuint32 numInterfaces,
                             const XAInterfaceID *pInterfaceIds,
                             const XAboolean *pInterfaceRequired);

XAresult XAEngineImpl_QueryNumSupportedInterfaces(XAuint32 *pNumSupportedInterfaces);

XAresult XAEngineImpl_QuerySupportedInterfaces(XAuint32 index,
                                               XAInterfaceID *pInterfaceId);

/*
 * Media Player
 */
XAresult XAMediaPlayerImpl_CreateMediaPlayer(XAObjectItf *pPlayer,
                                           XADataSource *pDataSrc,
                                           XADataSource *pBankSrc,
                                           XADataSink *pAudioSnk,
                                           XADataSink *pImageVideoSnk,
                                           XADataSink *pVibra,
                                           XADataSink *pLEDArray,
                                           XAuint32 numInterfaces,
                                           const XAInterfaceID *pInterfaceIds,
                                           const XAboolean *pInterfaceRequired);

XAresult XAMediaPlayerImpl_QueryNumSupportedInterfaces(XAuint32 *pNumSupportedInterfaces);

XAresult XAMediaPlayerImpl_QuerySupportedInterfaces(XAuint32 index,
                                                  XAInterfaceID *pInterfaceId);

/*
 * Metadata Extractor
 */
XAresult XAMetadataExtractorImpl_Create(XAObjectItf *pMetadataExtractor,
                                           XADataSource *pDataSource,
                                           XAuint32 numInterfaces,
                                           const XAInterfaceID *pInterfaceIds,
                                           const XAboolean *pInterfaceRequired);

XAresult XAMetadataExtractorImpl_QueryNumSupportedInterfaces(XAuint32 *pNumSupportedInterfaces);

XAresult XAMetadataExtractorImpl_QuerySupportedInterfaces(XAuint32 index,
                                            XAInterfaceID *pInterfaceId);

/*
 * Output Mix
 */
XAresult XAOMixImpl_CreateOutputMix(XAObjectItf *pMix,
                                    XAuint32 numInterfaces,
                                    const XAInterfaceID *pInterfaceIds,
                                    const XAboolean *pInterfaceRequired);

XAresult XAOMixImpl_QueryNumSupportedInterfaces(XAuint32 *pNumSupportedInterfaces);

XAresult XAOMixImpl_QuerySupportedInterfaces(XAuint32 index,
                                             XAInterfaceID *pInterfaceId);

/*
 * Media Recorder
 */
XAresult XAMediaRecorderImpl_CreateMediaRecorder(XAObjectItf* pRecorder,
                                                 XADataSource* pAudioSrc,
                                                 XADataSource* pImageVideoSrc,
                                                 XADataSink* pDataSnk,
                                                 XAuint32 numInterfaces,
                                                 const XAInterfaceID *pInterfaceIds,
                                                 const XAboolean *pInterfaceRequired);

XAresult XAMediaRecorderImpl_QueryNumSupportedInterfaces(XAuint32 *pNumSupportedInterfaces);

XAresult XAMediaRecorderImpl_QuerySupportedInterfaces(XAuint32 index,
                                                      XAInterfaceID *pInterfaceId);

/*
 * Camera Device
 */
XAresult XACameraDeviceImpl_CreateCameraDevice(XAObjectItf* pDevice,
                                               XAuint32 deviceID,
                                               XAuint32 numInterfaces,
                                               const XAInterfaceID * pInterfaceIds,
                                               const XAboolean * pInterfaceRequired);

XAresult XACameraDeviceImpl_QueryNumSupportedInterfaces(XAuint32 *pNumSupportedInterfaces);

XAresult XACameraDeviceImpl_QuerySupportedInterfaces(XAuint32 index,
                                                     XAInterfaceID *pInterfaceId);

/*
 * Radio Device
 */
XAresult XARadioDeviceImpl_CreateRadioDevice(XAObjectItf* pDevice,
                                             XAuint32 numInterfaces,
                                             const XAInterfaceID * pInterfaceIds,
                                             const XAboolean * pInterfaceRequired);

XAresult XARadioDeviceImpl_QueryNumSupportedInterfaces(XAuint32 *pNumSupportedInterfaces);

XAresult XARadioDeviceImpl_QuerySupportedInterfaces(XAuint32 index,
                                                    XAInterfaceID *pInterfaceId);

/*
 * Vibra Device
 */
XAresult XAVibraDeviceImpl_CreateVibraDevice(XAObjectItf* pDevice,
                                             XAuint32 deviceID,
                                             XAuint32 numInterfaces,
                                             const XAInterfaceID * pInterfaceIds,
                                             const XAboolean * pInterfaceRequired);

XAresult XAVibraDeviceImpl_QueryNumSupportedInterfaces(XAuint32 *pNumSupportedInterfaces);

XAresult XAVibraDeviceImpl_QuerySupportedInterfaces(XAuint32 index,
                                                    XAInterfaceID *pInterfaceId);

/*
 * LED Array
 */
XAresult XALEDArrayDeviceImpl_CreateLEDArrayDevice(XAObjectItf* pDevice,
                                                   XAuint32 deviceID,
                                                   XAuint32 numInterfaces,
                                                   const XAInterfaceID * pInterfaceIds,
                                                   const XAboolean * pInterfaceRequired);

XAresult XALEDArrayDeviceImpl_QueryNumSupportedInterfaces(XAuint32 *pNumSupportedInterfaces);

XAresult XALEDArrayDeviceImpl_QuerySupportedInterfaces(XAuint32 index,
                                                    XAInterfaceID *pInterfaceId);


/*
 * Check if interface IDs are equal
 */
XAboolean XACommon_EqualIIds(XAInterfaceID idA, XAInterfaceID idB);

/*
 * Check Data Source availability and type
 */
XAresult XACommon_CheckDataSource(XADataSource* source, XAMediaType* type);

XAresult XACommon_ValidateDataLocator( XAuint32 pCounter, void* pSnk, ... );
#endif /*XAGLOBALS_H */
