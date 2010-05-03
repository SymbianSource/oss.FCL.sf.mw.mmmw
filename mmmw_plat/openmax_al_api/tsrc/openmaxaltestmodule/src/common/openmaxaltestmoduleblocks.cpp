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
* Description: This file contains testclass implementation.
*
*/

// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <StifTestInterface.h>
#include "openmaxaltestmodule.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// COpenMAXALTestModule::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void COpenMAXALTestModule::Delete() 
    {
    if(m_URIName)
        {
        delete m_URIName;
        m_URIName = NULL;
        }
    if(m_MimeType)
        {
        delete m_MimeType;
        m_MimeType = NULL;
        }
    }

// -----------------------------------------------------------------------------
// COpenMAXALTestModule::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt COpenMAXALTestModule::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", COpenMAXALTestModule::ExampleL ),
        
        /* Common Utility Functions begins*/
        ENTRY( "al_SetDataSource", COpenMAXALTestModule::al_SetDataSource ),
        ENTRY( "al_SetDataSink", COpenMAXALTestModule::al_SetDataSink ),
        ENTRY( "al_SetDataLocator", COpenMAXALTestModule::al_SetDataLocator ),
        ENTRY( "al_SetDataFormat", COpenMAXALTestModule::al_SetDataFormat ),
        /* Common Utility Functions ends*/
        
        ENTRY( "al_createEngine", COpenMAXALTestModule::al_createEngine ),
        ENTRY( "al_queryNumSupportedEngineInterfaces", COpenMAXALTestModule::al_queryNumSupportedEngineInterfaces ),
        ENTRY( "al_querySupportedEngineInterfaces", COpenMAXALTestModule::al_querySupportedEngineInterfaces ),
        
        /*ObjectItf begins*/
        ENTRY( "al_objitf_Realize", COpenMAXALTestModule::al_objitf_Realize ),       
        ENTRY( "al_objitf_Resume", COpenMAXALTestModule::al_objitf_Resume ),
        ENTRY( "al_objitf_GetState", COpenMAXALTestModule::al_objitf_GetState ),
        ENTRY( "al_objitf_GetInterface", COpenMAXALTestModule::al_objitf_GetInterface ),
        ENTRY( "al_objitf_RegisterCallback", COpenMAXALTestModule::al_objitf_RegisterCallback ),
        ENTRY( "al_objitf_AbortAsyncOperation", COpenMAXALTestModule::al_objitf_AbortAsyncOperation ),
        ENTRY( "al_objitf_Destroy", COpenMAXALTestModule::al_objitf_Destroy ),
        ENTRY( "al_objitf_SetPriority", COpenMAXALTestModule::al_objitf_SetPriority ),
        ENTRY( "al_objitf_GetPriority", COpenMAXALTestModule::al_objitf_GetPriority ),
        ENTRY( "al_objitf_SetLossOfControlInterfaces", COpenMAXALTestModule::al_objitf_SetLossOfControlInterfaces ),
        /*ObjectItf ends*/

        /*DynItfMgmtItf begins*/
        ENTRY( "al_dimitf_AddInterface", COpenMAXALTestModule::al_dimitf_AddInterface ),
        ENTRY( "al_dimitf_RemoveInterface", COpenMAXALTestModule::al_dimitf_RemoveInterface ),
        ENTRY( "al_dimitf_ResumeInterface", COpenMAXALTestModule::al_dimitf_ResumeInterface ),
        ENTRY( "al_dimitf_RegisterCallback", COpenMAXALTestModule::al_dimitf_RegisterCallback ),
        /*DynItfMgmtItf ends*/
        
        
        /*EngineItf begins*/
        ENTRY( "al_engitf_CreateCameraDevice", COpenMAXALTestModule::al_engitf_CreateCameraDevice ),
        ENTRY( "al_engitf_CreateRadioDevice", COpenMAXALTestModule::al_engitf_CreateRadioDevice ),
        ENTRY( "al_engitf_CreateLEDDevice", COpenMAXALTestModule::al_engitf_CreateLEDDevice ),
        ENTRY( "al_engitf_CreateVibraDevice", COpenMAXALTestModule::al_engitf_CreateVibraDevice ),
        ENTRY( "al_engitf_CreateMediaPlayer", COpenMAXALTestModule::al_engitf_CreateMediaPlayer ),
        ENTRY( "al_engitf_CreateMediaRecorder", COpenMAXALTestModule::al_engitf_CreateMediaRecorder ),
        ENTRY( "al_engitf_CreateOutputMix", COpenMAXALTestModule::al_engitf_CreateOutputMix ),
        ENTRY( "al_engitf_CreateMetadataExtractor", COpenMAXALTestModule::al_engitf_CreateMetadataExtractor ),
        ENTRY( "al_engitf_CreateExtensionObject", COpenMAXALTestModule::al_engitf_CreateExtensionObject ),
        ENTRY( "al_engitf_GetImplementationInfo", COpenMAXALTestModule::al_engitf_GetImplementationInfo ),
        ENTRY( "al_engitf_QuerySupportedProfiles", COpenMAXALTestModule::al_engitf_QuerySupportedProfiles ),
        ENTRY( "al_engitf_QueryNumSupportedInterfaces", COpenMAXALTestModule::al_engitf_QueryNumSupportedInterfaces ),
        ENTRY( "al_engitf_QuerySupportedInterfaces", COpenMAXALTestModule::al_engitf_QuerySupportedInterfaces ),
        ENTRY( "al_engitf_QueryNumSupportedExtension", COpenMAXALTestModule::al_engitf_QueryNumSupportedExtension ),
        ENTRY( "al_engitf_QuerySupportedExtension", COpenMAXALTestModule::al_engitf_QuerySupportedExtension ),
        ENTRY( "al_engitf_IsExtensionSupported", COpenMAXALTestModule::al_engitf_IsExtensionSupported ),
        ENTRY( "al_engitf_QueryLEDCapabilities", COpenMAXALTestModule::al_engitf_QueryLEDCapabilities ),
        ENTRY( "al_engitf_QueryVibraCapabilities", COpenMAXALTestModule::al_engitf_QueryVibraCapabilities ),
        /*EngineItf ends*/
        
        /*AudioIODeviceCapabilitiesItf ends*/
        ENTRY( "al_audioiodevcapitf_GetAvailableAudioInputs", COpenMAXALTestModule::al_audioiodevcapitf_GetAvailableAudioInputs ),
        ENTRY( "al_audioiodevcapitf_QueryAudioInputCapabilities", COpenMAXALTestModule::al_audioiodevcapitf_QueryAudioInputCapabilities ),
        ENTRY( "al_audioiodevcapitf_RegisterAvailableAudioInputsChangedCallback", COpenMAXALTestModule::al_audioiodevcapitf_RegisterAvailableAudioInputsChangedCallback ),
        ENTRY( "al_audioiodevcapitf_GetAvailableAudioOutputs", COpenMAXALTestModule::al_audioiodevcapitf_GetAvailableAudioOutputs ),
        ENTRY( "al_audioiodevcapitf_QueryAudioOutputCapabilities", COpenMAXALTestModule::al_audioiodevcapitf_QueryAudioOutputCapabilities ),
        ENTRY( "al_audioiodevcapitf_RegisterAvailableAudioOutputsChangedCallback", COpenMAXALTestModule::al_audioiodevcapitf_RegisterAvailableAudioOutputsChangedCallback ),
        ENTRY( "al_audioiodevcapitf_RegisterDefaultDeviceIDMapChangedCallback", COpenMAXALTestModule::al_audioiodevcapitf_RegisterDefaultDeviceIDMapChangedCallback ),
        ENTRY( "al_audioiodevcapitf_GetAssociatedAudioInputs", COpenMAXALTestModule::al_audioiodevcapitf_GetAssociatedAudioInputs ),
        ENTRY( "al_audioiodevcapitf_GetAssociatedAudioOutputs", COpenMAXALTestModule::al_audioiodevcapitf_GetAssociatedAudioOutputs ),
        ENTRY( "al_audioiodevcapitf_GetDefaultAudioDevices", COpenMAXALTestModule::al_audioiodevcapitf_GetDefaultAudioDevices ),
        ENTRY( "al_audioiodevcapitf_QuerySampleFormatsSupported", COpenMAXALTestModule::al_audioiodevcapitf_QuerySampleFormatsSupported ),
        /*AudioIODeviceCapabilitiesItf ends*/

        /*AudioEncoderCapabilitiesItf ends*/
        ENTRY( "al_audioencodercapitf_GetAudioEncoders", COpenMAXALTestModule::al_audioencodercapitf_GetAudioEncoders ),
        ENTRY( "al_audioencodercapitf_GetAudioEncoderCapabilities", COpenMAXALTestModule::al_audioencodercapitf_GetAudioEncoderCapabilities ),
        /*AudioIODeviceCapabilitiesItf ends*/

        
        /*RecordItf ends*/
        ENTRY( "al_recorditf_SetRecordState", COpenMAXALTestModule::al_recorditf_SetRecordState ),
        ENTRY( "al_recorditf_GetRecordState", COpenMAXALTestModule::al_recorditf_GetRecordState ),
        ENTRY( "al_recorditf_SetDurationLimit", COpenMAXALTestModule::al_recorditf_SetDurationLimit ),
        ENTRY( "al_recorditf_GetPosition", COpenMAXALTestModule::al_recorditf_GetPosition ),
        ENTRY( "al_recorditf_RegisterCallback", COpenMAXALTestModule::al_recorditf_RegisterCallback ),
        ENTRY( "al_recorditf_SetCallbackEventsMask", COpenMAXALTestModule::al_recorditf_SetCallbackEventsMask ),
        ENTRY( "al_recorditf_GetCallbackEventsMask", COpenMAXALTestModule::al_recorditf_GetCallbackEventsMask ),
        ENTRY( "al_recorditf_SetMarkerPosition", COpenMAXALTestModule::al_recorditf_SetMarkerPosition ),
        ENTRY( "al_recorditf_ClearMarkerPosition", COpenMAXALTestModule::al_recorditf_ClearMarkerPosition ),
        ENTRY( "al_recorditf_GetMarkerPosition", COpenMAXALTestModule::al_recorditf_GetMarkerPosition ),
        ENTRY( "al_recorditf_SetPositionUpdatePeriod", COpenMAXALTestModule::al_recorditf_SetPositionUpdatePeriod ),
        ENTRY( "al_recorditf_GetPositionUpdatePeriod", COpenMAXALTestModule::al_recorditf_GetPositionUpdatePeriod ),
        /*RecordItf ends*/

        
        /*AudioEncoderItf ends*/
        ENTRY( "al_audioencoderitf_SetEncoderSettings", COpenMAXALTestModule::al_audioencoderitf_SetEncoderSettings ),
        ENTRY( "al_audioencoderitf_GetEncoderSettings", COpenMAXALTestModule::al_audioencoderitf_GetEncoderSettings ),
        /*AudioEncoderItf ends*/

        
        /*MetadataInsertionItf ends*/
        ENTRY( "al_metadatainsertionitf_CreateChildNode", COpenMAXALTestModule::al_metadatainsertionitf_CreateChildNode ),
        ENTRY( "al_metadatainsertionitf_GetSupportedKeysCount", COpenMAXALTestModule::al_metadatainsertionitf_GetSupportedKeysCount ),
        ENTRY( "al_metadatainsertionitf_GetKeySize", COpenMAXALTestModule::al_metadatainsertionitf_GetKeySize ),
        ENTRY( "al_metadatainsertionitf_GetKey", COpenMAXALTestModule::al_metadatainsertionitf_GetKey ),
        ENTRY( "al_metadatainsertionitf_GetFreeKeysEncoding", COpenMAXALTestModule::al_metadatainsertionitf_GetFreeKeysEncoding ),
        ENTRY( "al_metadatainsertionitf_InsertMetadataItem", COpenMAXALTestModule::al_metadatainsertionitf_InsertMetadataItem ),
        ENTRY( "al_metadatainsertionitf_RegisterCallback", COpenMAXALTestModule::al_metadatainsertionitf_RegisterCallback ),
        /*MetadataInsertionItf ends*/
        
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// COpenMAXALTestModule::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt COpenMAXALTestModule::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KOpenMAXALTestModule, "OpenMAXALTestModule" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KOpenMAXALTestModule, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KOpenMAXALTestModule, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }

TInt COpenMAXALTestModule::MapErr(XAresult xa_res)
    {
    TInt status(KErrGeneral);
    switch(xa_res)
        {
        case XA_RESULT_SUCCESS:
            //iLog->Log(_L("XA_RESULT_SUCCESS"));
            status = KErrNone;
            break;
        case XA_RESULT_PRECONDITIONS_VIOLATED:
            iLog->Log(_L("XA_RESULT_PRECONDITIONS_VIOLATED"));
            break;
        case XA_RESULT_PARAMETER_INVALID:
            iLog->Log(_L("XA_RESULT_PARAMETER_INVALID"));
            break;
        case XA_RESULT_MEMORY_FAILURE:
            iLog->Log(_L("XA_RESULT_MEMORY_FAILURE"));
            break;
        case XA_RESULT_RESOURCE_ERROR:
            iLog->Log(_L("XA_RESULT_RESOURCE_ERROR"));
            break;
        case XA_RESULT_RESOURCE_LOST:
            iLog->Log(_L("XA_RESULT_RESOURCE_LOST"));
            break;
        case XA_RESULT_IO_ERROR:
            iLog->Log(_L("XA_RESULT_IO_ERROR"));
            break;
        case XA_RESULT_BUFFER_INSUFFICIENT:
            iLog->Log(_L("XA_RESULT_BUFFER_INSUFFICIENT"));
            break;
        case XA_RESULT_CONTENT_CORRUPTED:
            iLog->Log(_L("XA_RESULT_CONTENT_CORRUPTED"));
            break;
        case XA_RESULT_CONTENT_UNSUPPORTED:
            iLog->Log(_L("XA_RESULT_CONTENT_UNSUPPORTED"));
            break;
        case XA_RESULT_CONTENT_NOT_FOUND:
            iLog->Log(_L("XA_RESULT_CONTENT_NOT_FOUND"));
            break;
        case XA_RESULT_PERMISSION_DENIED:
            iLog->Log(_L("XA_RESULT_PERMISSION_DENIED"));
            break;
        case XA_RESULT_FEATURE_UNSUPPORTED:
            iLog->Log(_L("XA_RESULT_FEATURE_UNSUPPORTED"));
            break;
        case XA_RESULT_INTERNAL_ERROR:
            iLog->Log(_L("XA_RESULT_INTERNAL_ERROR"));
            break;
        case XA_RESULT_UNKNOWN_ERROR:
            iLog->Log(_L("XA_RESULT_UNKNOWN_ERROR"));
            break;
        case XA_RESULT_OPERATION_ABORTED:
            iLog->Log(_L("XA_RESULT_OPERATION_ABORTED"));
            break;
        case XA_RESULT_CONTROL_LOST:
            iLog->Log(_L("XA_RESULT_CONTROL_LOST"));
            break;
        default:
            iLog->Log(_L("Unknown Error!!!"));
        }
    return status;
    }


XAInterfaceID COpenMAXALTestModule::MapInterface(TInt interface)
    {
    XAInterfaceID id(XA_IID_NULL);
    switch(interface)
        {
        case 1:
            id = XA_IID_NULL;
            break;
        case 2:
            id = XA_IID_OBJECT;
            break;
        case 3:
            id = XA_IID_CONFIGEXTENSION;
            break;
        case 4:
            id = XA_IID_DYNAMICINTERFACEMANAGEMENT;
            break;
        case 5:
            id = XA_IID_ENGINE;
            break;
        case 6:
            id = XA_IID_THREADSYNC;
            break;
        case 7:
            id = XA_IID_PLAY;
            break;
        case 8:
            id = XA_IID_PLAYBACKRATE;
            break;
        case 9:
            id = XA_IID_PREFETCHSTATUS;
            break;
        case 10:
            id = XA_IID_SEEK;
            break;
        case 11:
            id = XA_IID_VOLUME;
            break;
        case 12:
            id = XA_IID_IMAGECONTROLS;
            break;
        case 13:
            id = XA_IID_IMAGEEFFECTS;
            break;
        case 14:
            id = XA_IID_VIDEOPOSTPROCESSING;
            break;
        case 15:
            id = XA_IID_RECORD;
            break;
        case 16:
            id = XA_IID_SNAPSHOT;
            break;
        case 17:
            id = XA_IID_METADATAEXTRACTION;
            break;
        case 18:
            id = XA_IID_METADATAINSERTION;
            break;
        case 19:
            id = XA_IID_METADATATRAVERSAL;
            break;
        case 20:
            id = XA_IID_DYNAMICSOURCE;
            break;
        case 21:
            id = XA_IID_CAMERACAPABILITIES;
            break;
        case 22:
            id = XA_IID_CAMERA;
            break;
        case 23:
            id = XA_IID_AUDIOIODEVICECAPABILITIES;
            break;
        case 24:
            id = XA_IID_DEVICEVOLUME;
            break;
        case 25:
            id = XA_IID_EQUALIZER;
            break;
        case 26:
            id = XA_IID_OUTPUTMIX;
            break;
        case 27:
            id = XA_IID_RADIO;
            break;
        case 28:
            id = XA_IID_RDS;
            break;
        case 29:
            id = XA_IID_VIBRA;
            break;
        case 30:
            id = XA_IID_LED;
            break;
        case 31:
            id = XA_IID_AUDIODECODERCAPABILITIES;
            break;
        case 32:
            id = XA_IID_AUDIOENCODER;
            break;
        case 33:
            id = XA_IID_AUDIOENCODERCAPABILITIES;
            break;
        case 34:
            id = XA_IID_IMAGEENCODERCAPABILITIES;
            break;
        case 35:
            id = XA_IID_IMAGEDECODERCAPABILITIES;
            break;
        case 36:
            id = XA_IID_IMAGEENCODER;
            break;
        case 37:
            id = XA_IID_VIDEODECODERCAPABILITIES;
            break;
        case 38:
            id = XA_IID_VIDEOENCODER;
            break;
        case 39:
            id = XA_IID_VIDEOENCODERCAPABILITIES;
            break;
        case 40:
            id = XA_IID_STREAMINFORMATION;
            break;
        default:
            break;
        }
    return id;
    }

XAObjectItf COpenMAXALTestModule::GetObject(TInt object)
    {
    XAObjectItf itf;
    switch(object)
        {
        case XA_OBJECTID_ENGINE:
            itf = m_EOEngine;
            break;
        case XA_OBJECTID_LEDDEVICE:
            itf = m_MOLEDObject;
            break;
        case XA_OBJECTID_VIBRADEVICE:
            itf = m_MOVibraObject;
            break;           
        case XA_OBJECTID_MEDIAPLAYER:
            itf = m_MOPlayer;
            break;           
        case XA_OBJECTID_MEDIARECORDER:
            itf = m_MORecorder;
            break;
        case XA_OBJECTID_RADIODEVICE:
            itf = m_MORadioObject;
            break;
        case XA_OBJECTID_OUTPUTMIX:
            itf = m_MOOutputMix;
            break;
        case XA_OBJECTID_METADATAEXTRACTOR:
            itf = m_MOMetadataExtractor;
            break;
        case XA_OBJECTID_CAMERADEVICE:
            itf = m_MOCameraObject;
            break;
        case 10:
            /*Extension Object Defined own id as 10*/
            itf = m_MOExtnObject;
            break;
        default:
            itf = NULL;
            break;
        }
    return itf;
    }

TInt COpenMAXALTestModule::StoreInterface(TInt interface, void* id)
    {
    TInt status(KErrNone);
    switch(interface)
        {
        case 1:
/*            XA_IID_NULL;*/
            break;
        case 2:
/*            XA_IID_OBJECT;*/
            break;
        case 3:
/*            XA_IID_CONFIGEXTENSION;*/
            break;
        case 4:
            m_DIMItf = XADynamicInterfaceManagementItf(id);
/*            XA_IID_DYNAMICINTERFACEMANAGEMENT;*/
            break;
        case 5:
            m_EngineItf = XAEngineItf(id);
            break;
        case 6:
/*            XA_IID_THREADSYNC;*/
            break;
        case 7:
/*            XA_IID_PLAY;*/
            break;
        case 8:
/*            XA_IID_PLAYBACKRATE;*/
            break;
        case 9:
/*            XA_IID_PREFETCHSTATUS;*/
            break;
        case 10:
/*            XA_IID_SEEK;*/
            break;
        case 11:
/*            XA_IID_VOLUME;*/
            break;
        case 12:
/*            XA_IID_IMAGECONTROLS;*/
            break;
        case 13:
/*            XA_IID_IMAGEEFFECTS;*/
            break;
        case 14:
/*            XA_IID_VIDEOPOSTPROCESSING;*/
            break;
        case 15:
            m_RecordItf = XARecordItf(id);
            break;
        case 16:
/*            XA_IID_SNAPSHOT;*/
            break;
        case 17:
/*            XA_IID_METADATAEXTRACTION;*/
            break;
        case 18:
            m_MetadataInsertionItf = XAMetadataInsertionItf(id);
/*            XA_IID_METADATAINSERTION;*/
            break;
        case 19:
/*            XA_IID_METADATATRAVERSAL;*/
            break;
        case 20:
/*            XA_IID_DYNAMICSOURCE;*/
            break;
        case 21:
/*            XA_IID_CAMERACAPABILITIES;*/
            break;
        case 22:
/*            XA_IID_CAMERA;*/
            break;
        case 23:
            m_AIODevCapItf = XAAudioIODeviceCapabilitiesItf(id);
/*            XA_IID_AUDIOIODEVICECAPABILITIES;*/
            break;
        case 24:
/*            XA_IID_DEVICEVOLUME;*/
            break;
        case 25:
/*            XA_IID_EQUALIZER;*/
            break;
        case 26:
/*            XA_IID_OUTPUTMIX;*/
            break;
        case 27:
/*            XA_IID_RADIO;*/
            break;
        case 28:
/*            XA_IID_RDS;*/
            break;
        case 29:
/*            XA_IID_VIBRA;*/
            break;
        case 30:
/*            XA_IID_LED;*/
            break;
        case 31:
/*            XA_IID_AUDIODECODERCAPABILITIES;*/
            break;
        case 32:
            m_AudEncItf = XAAudioEncoderItf(id);
/*            XA_IID_AUDIOENCODER;*/
            break;
        case 33:
            m_AEncCapItf = XAAudioEncoderCapabilitiesItf(id);
/*            XA_IID_AUDIOENCODERCAPABILITIES;*/
            break;
        case 34:
/*            XA_IID_IMAGEENCODERCAPABILITIES;*/
            break;
        case 35:
/*            XA_IID_IMAGEDECODERCAPABILITIES;*/
            break;
        case 36:
/*            XA_IID_IMAGEENCODER;*/
            break;
        case 37:
/*            XA_IID_VIDEODECODERCAPABILITIES;*/
            break;
        case 38:
/*            XA_IID_VIDEOENCODER;*/
            break;
        case 39:
/*            XA_IID_VIDEOENCODERCAPABILITIES;*/
            break;
        case 40:
/*            XA_IID_STREAMINFORMATION;*/
            break;
        default:
            break;
            
        }
    return status;
    }


// -----------------------------------------------------------------------------
// COpenMAXALTestModule::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt COpenMAXALTestModule::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
