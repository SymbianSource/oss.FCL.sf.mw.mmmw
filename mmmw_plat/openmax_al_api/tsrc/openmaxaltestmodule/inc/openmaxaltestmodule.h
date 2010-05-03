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
* Description: STIF testclass declaration
*
*/

#ifndef OPENMAXALTESTMODULE_H
#define OPENMAXALTESTMODULE_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <OpenMAXAL.h>
// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
#define RET_ERR_IF_ERR(err) if (err!=KErrNone) return err;
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KOpenMAXALTestModuleLogPath, "\\logs\\testframework\\OpenMAXALTestModule\\" ); 
// Log file
_LIT( KOpenMAXALTestModuleLogFile, "OpenMAXALTestModule.txt" ); 
_LIT( KOpenMAXALTestModuleLogFileWithTitle, "OpenMAXALTestModule_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class COpenMAXALTestModule;

// DATA TYPES
#define MAX_NUMBER_OPTIONS 2
#define MAX_NUMBER_INTERFACES 20
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  COpenMAXALTestModule test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(COpenMAXALTestModule) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static COpenMAXALTestModule* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~COpenMAXALTestModule();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        COpenMAXALTestModule( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ExampleL( CStifItemParser& aItem );
        
        virtual TInt al_SetDataSource( CStifItemParser& aItem );
        virtual TInt al_SetDataSink( CStifItemParser& aItem );        
        virtual TInt al_SetDataLocator( CStifItemParser& aItem );        
        virtual TInt al_SetDataFormat( CStifItemParser& aItem );        
        
        virtual TInt al_createEngine( CStifItemParser& aItem );
        virtual TInt al_queryNumSupportedEngineInterfaces( CStifItemParser& aItem );
        virtual TInt al_querySupportedEngineInterfaces( CStifItemParser& aItem );
        virtual TInt al_objitf_Realize( CStifItemParser& aItem );
        virtual TInt al_objitf_Resume( CStifItemParser& aItem );
        virtual TInt al_objitf_GetState( CStifItemParser& aItem );
        virtual TInt al_objitf_GetInterface( CStifItemParser& aItem );
        virtual TInt al_objitf_RegisterCallback( CStifItemParser& aItem );
        virtual TInt al_objitf_AbortAsyncOperation( CStifItemParser& aItem );
        virtual TInt al_objitf_Destroy( CStifItemParser& aItem );
        virtual TInt al_objitf_SetPriority( CStifItemParser& aItem );
        virtual TInt al_objitf_GetPriority( CStifItemParser& aItem );
        virtual TInt al_objitf_SetLossOfControlInterfaces( CStifItemParser& aItem );
        
        virtual TInt al_dimitf_AddInterface( CStifItemParser& aItem );
        virtual TInt al_dimitf_RemoveInterface( CStifItemParser& aItem );
        virtual TInt al_dimitf_ResumeInterface( CStifItemParser& aItem );
        virtual TInt al_dimitf_RegisterCallback( CStifItemParser& aItem );
        
        
        virtual TInt al_engitf_CreateCameraDevice( CStifItemParser& aItem );
        virtual TInt al_engitf_CreateRadioDevice( CStifItemParser& aItem );
        virtual TInt al_engitf_CreateLEDDevice( CStifItemParser& aItem );
        virtual TInt al_engitf_CreateVibraDevice( CStifItemParser& aItem );
        virtual TInt al_engitf_CreateMediaPlayer( CStifItemParser& aItem );
        virtual TInt al_engitf_CreateMediaRecorder( CStifItemParser& aItem );
        virtual TInt al_engitf_CreateOutputMix( CStifItemParser& aItem );
        virtual TInt al_engitf_CreateMetadataExtractor( CStifItemParser& aItem );
        virtual TInt al_engitf_CreateExtensionObject( CStifItemParser& aItem );
        virtual TInt al_engitf_GetImplementationInfo( CStifItemParser& aItem );
        virtual TInt al_engitf_QuerySupportedProfiles( CStifItemParser& aItem );
        virtual TInt al_engitf_QueryNumSupportedInterfaces( CStifItemParser& aItem );
        virtual TInt al_engitf_QuerySupportedInterfaces( CStifItemParser& aItem );
        virtual TInt al_engitf_QueryNumSupportedExtension( CStifItemParser& aItem );
        virtual TInt al_engitf_QuerySupportedExtension( CStifItemParser& aItem );
        virtual TInt al_engitf_IsExtensionSupported( CStifItemParser& aItem );
        virtual TInt al_engitf_QueryLEDCapabilities( CStifItemParser& aItem );
        virtual TInt al_engitf_QueryVibraCapabilities( CStifItemParser& aItem );
        virtual TInt al_audioiodevcapitf_GetAvailableAudioInputs( CStifItemParser& aItem );
        virtual TInt al_audioiodevcapitf_QueryAudioInputCapabilities( CStifItemParser& aItem );
        virtual TInt al_audioiodevcapitf_RegisterAvailableAudioInputsChangedCallback( CStifItemParser& aItem );
        virtual TInt al_audioiodevcapitf_GetAvailableAudioOutputs( CStifItemParser& aItem );
        virtual TInt al_audioiodevcapitf_QueryAudioOutputCapabilities( CStifItemParser& aItem );
        virtual TInt al_audioiodevcapitf_RegisterAvailableAudioOutputsChangedCallback( CStifItemParser& aItem );
        virtual TInt al_audioiodevcapitf_RegisterDefaultDeviceIDMapChangedCallback( CStifItemParser& aItem );
        virtual TInt al_audioiodevcapitf_GetAssociatedAudioInputs( CStifItemParser& aItem );
        virtual TInt al_audioiodevcapitf_GetAssociatedAudioOutputs( CStifItemParser& aItem );
        virtual TInt al_audioiodevcapitf_GetDefaultAudioDevices( CStifItemParser& aItem );
        virtual TInt al_audioiodevcapitf_QuerySampleFormatsSupported( CStifItemParser& aItem );
        virtual TInt al_audioencodercapitf_GetAudioEncoders( CStifItemParser& aItem );
        virtual TInt al_audioencodercapitf_GetAudioEncoderCapabilities( CStifItemParser& aItem );
        virtual TInt al_recorditf_SetRecordState( CStifItemParser& aItem );
        virtual TInt al_recorditf_GetRecordState( CStifItemParser& aItem );
        virtual TInt al_recorditf_SetDurationLimit( CStifItemParser& aItem );
        virtual TInt al_recorditf_GetPosition( CStifItemParser& aItem );
        virtual TInt al_recorditf_RegisterCallback( CStifItemParser& aItem );
        virtual TInt al_recorditf_SetCallbackEventsMask( CStifItemParser& aItem );
        virtual TInt al_recorditf_GetCallbackEventsMask( CStifItemParser& aItem );
        virtual TInt al_recorditf_SetMarkerPosition( CStifItemParser& aItem );
        virtual TInt al_recorditf_ClearMarkerPosition( CStifItemParser& aItem );
        virtual TInt al_recorditf_GetMarkerPosition( CStifItemParser& aItem );
        virtual TInt al_recorditf_SetPositionUpdatePeriod( CStifItemParser& aItem );
        virtual TInt al_recorditf_GetPositionUpdatePeriod( CStifItemParser& aItem );
        virtual TInt al_audioencoderitf_SetEncoderSettings( CStifItemParser& aItem );
        virtual TInt al_audioencoderitf_GetEncoderSettings( CStifItemParser& aItem );
        virtual TInt al_metadatainsertionitf_CreateChildNode( CStifItemParser& aItem );
        virtual TInt al_metadatainsertionitf_GetSupportedKeysCount( CStifItemParser& aItem );
        virtual TInt al_metadatainsertionitf_GetKeySize( CStifItemParser& aItem );
        virtual TInt al_metadatainsertionitf_GetKey( CStifItemParser& aItem );
        virtual TInt al_metadatainsertionitf_GetFreeKeysEncoding( CStifItemParser& aItem );
        virtual TInt al_metadatainsertionitf_InsertMetadataItem( CStifItemParser& aItem );
        virtual TInt al_metadatainsertionitf_RegisterCallback( CStifItemParser& aItem );
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
        enum TObjectType
            {
            EEngine = 1,
            EMediaRecorder,
            ERecordItf,
            EEngineItf
            };

    protected:  // Data
        TInt MapErr(XAresult xa_res);
        XAInterfaceID MapInterface(TInt interface);
        XAObjectItf COpenMAXALTestModule::GetObject(TInt object);
        TInt StoreInterface(TInt interface, void* id);
        xaObjectCallback GetCallbackFunc(TInt object);
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        XAboolean     required[MAX_NUMBER_INTERFACES]; 
        XAInterfaceID iidArray[MAX_NUMBER_INTERFACES]; 
        XAEngineOption EngineOption[MAX_NUMBER_OPTIONS];
                                    
        XAObjectItf m_EOEngine;
        XAObjectItf m_MORecorder;
        XAObjectItf m_MOPlayer;
        XARecordItf m_RecordItf;
        XAObjectItf m_MOOutputMix;
        XAObjectItf m_MOMetadataExtractor;
        XAObjectItf m_MOExtnObject;
        XAObjectItf m_MOVibraObject;
        XAObjectItf m_MOLEDObject;
        XAObjectItf m_MORadioObject;
        XAObjectItf m_MOCameraObject;
        
        
        XADynamicInterfaceManagementItf m_DIMItf;
        
        
        XAEngineItf m_EngineItf;
        XAAudioIODeviceCapabilitiesItf m_AIODevCapItf;
        XAAudioEncoderCapabilitiesItf m_AEncCapItf;
        XAAudioEncoderItf m_AudEncItf;
        XAMetadataInsertionItf m_MetadataInsertionItf;
        
        /*Audio Source*/
        XADataSource m_AudioSource;
        XADataLocator_IODevice  m_IODevice;

        /*Image/Video Source*/
        XADataSource m_VideoSource;
       
        /*MetadataExtractor Source*/
        XADataSource m_MOMetadataExtractorSource;
        
        /*Data Sink*/
        XADataSink m_DataSink;
        XADataSink m_AudioSink;
        XADataSink m_VideoSink;
        XADataSink m_LEDSink;
        XADataSink m_VibraSink;
        
        
        XADataLocator_URI m_Uri;
        HBufC8* m_URIName;
        
        XADataFormat_MIME m_Mime;
        HBufC8* m_MimeType;
        
        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // OPENMAXALTESTMODULE_H

// End of File
