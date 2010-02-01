/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: voip audio service -
*
*/


//#define __JITTER_BUFFER_TEST__

#ifndef CVoIPAudioServicesTestClass_H
#define CVoIPAudioServicesTestClass_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

#include <e32svr.h>
#include <e32base.h>
#include <e32cons.h>
#include <VoIPUtilityFactory.h>
#include <VoIPDownlinkStream.h>
#include <VoIPUplinkStream.h>
#include <VoIPFormatIntfc.h>
#include <VoIPG711DecoderIntfc.h>
#include <VoIPG711EncoderIntfc.h>
#include <VoIPG729DecoderIntfc.h>
#include <VoIPILBCDecoderIntfc.h>
#include <VoIPILBCEncoderIntfc.h>
#include <VoIPDtmfTonePlayer.h>
#include <VoIPRingTonePlayer.h>
#include <VoIPRingToneFactory.h>

#ifdef __JITTER_BUFFER_TEST__
#include <VoIPJitterBufferIntfc.h>
#endif //__JITTER_BUFFER_TEST__



#include "TimeoutController.h"


// CONSTANTS
const TInt KErrBadTestParameter= -1000;         // Error on configuration file

const TInt KErrEventPending =-2000;             // Timeout and not callback
const TInt KErrCallbackErrorCode = -2001;       // Callback return error code
const TInt KErrUnexpectedValue =-2002;          // Unexpected value at setting
const TInt KErrTimeoutController = -2007;       // Unexpected notification

const TInt KShortTimeout = 2000;
const TInt KMediumTimeout = 4000;
const TInt KLongTimeout = 100000;

#ifdef __JITTER_BUFFER_TEST__
const TUint KDefaultSampleRateInkHz = 8; // for sequence number calculation
const TUint KMaxSeqNumber = 0xFFFF; //65535
#endif //__JITTER_BUFFER_TEST__


_LIT( KMsgBadTestParameters, "[Error] No valid test case parameters");

// General
_LIT( KTagG711, "G711");
_LIT( KTagG729, "G729");
_LIT( KTagILBC, "iLBC");
_LIT( KTagAMRNB, "AMR-NB");
_LIT( KTagPCM16, "PCM16");
_LIT( KTagHandset, "Handset");
_LIT( KTagLoudSpeaker, "LoudSpeaker");
_LIT( KTagUplink, "Uplink");
_LIT( KTagDnlink, "Dnlink");
_LIT( KTagFile, "File");
_LIT( KTagRFile, "RFile");
_LIT( KTagProfile, "Profile");
_LIT( KTagDesL, "DesL");
_LIT( KTagMin, "MIN");
_LIT( KTagMax, "MAX");
_LIT( KTagULaw, "uLaw");
_LIT( KTagALaw, "aLaw");
_LIT( KTag20ms, "20ms");
_LIT( KTag30ms, "30ms");
_LIT( KTagFile1, "File1");
_LIT( KTagFile2, "File2");



// MACROS
//#define ?macro ?macro_def
// Logging path
#ifdef __WINSCW__
_LIT( KVoIPAudioServicesTestClassLogPath, "\\logs\\testframework\\" );
_LIT16(KTestFile1, "\\testing\\data\\alarm 1.aac");
_LIT16(KTestFile2, "\\testing\\data\\alarm.aac");
_LIT16(KTestFile3, "\\testing\\data\\test_8khz.wav");
#else
_LIT( KVoIPAudioServicesTestClassLogPath, "e:\\testing\\Log\\" );
_LIT16(KTestFile1, "e:\\testing\\data\\alarm 1.aac");
_LIT16(KTestFile2, "e:\\testing\\data\\alarm.aac");
_LIT16(KTestFile3, "e:\\testing\\data\\test_8khz.wav");
#endif

// Log file
_LIT( KVoIPAudioServicesTestClassLogFile, "CVoIPAudioServicesTestClass.txt" );
_LIT8(KMimetypeAAC, "audio/aac");
_LIT8(KMimetypeWAV, "audio/wav");
const TUint KTonesBufSize = 6;
const TUint KMimeStringSize = 32;

// FORWARD DECLARATIONS
class CVoIPAudioServicesTestClass;
class CVoIPDataBuffer;
#ifdef __JITTER_BUFFER_TEST__
class CVoIPJBDataBuffer;
#endif //__JITTER_BUFFER_TEST__



// CLASS DECLARATION

/**
*  CRadioUtilityTestClass test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series60_3_2
*/
NONSHARABLE_CLASS(CVoIPAudioServicesTestClass) :
                        public CScriptBase,
                        public MTimeoutObserver,
                        public MVoIPDownlinkObserver,
                        public MVoIPUplinkObserver,
                        public MVoIPFormatObserver,
                        public MRingToneObserver,
                        public MDTMFToneObserver
#ifdef __JITTER_BUFFER_TEST__
                        , public MVoIPJitterBufferObserver
#endif //__JITTER_BUFFER_TEST__

    {

    enum TAppState
        {
        ENotReady = 0,
        EReady,
        EStreaming
        };

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CVoIPAudioServicesTestClass* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CVoIPAudioServicesTestClass();

        enum TVoIPExpectedEvent
        {
            EOpenDownlinkComplete,
            EOpenUplinkComplete,
            EDownlinkClosed,
            EUplinkClosed,
            EOpenCompleteDNL,
            EOpenCompleteUPL,
            EOpenComplete,
            EPlaybackComplete,
            EEmptyBuffer,
            EFillBuffer,

        };

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


        // From MVoIPDownlinkObserver
        virtual void FillBuffer(const CVoIPAudioDownlinkStream& aSrc,
                            CVoIPDataBuffer* aBuffer);
        virtual void Event(const CVoIPAudioDownlinkStream& aSrc,
        					TInt aEventType,
        					TInt aError);

        // From MVoIPUplinkObserver
        virtual void EmptyBuffer(const CVoIPAudioUplinkStream& aSrc,
                             CVoIPDataBuffer* aBuffer);
        virtual void Event(const CVoIPAudioUplinkStream& aSrc,
                       		TInt aEventType,
                       		TInt aError);

        // From MVoIPFormatObserver
        virtual void Event(const CVoIPFormatIntfc& aSrc, TInt aEventType);

        // From MRingToneObserver

        /**
	     * ?description
	     *
	     * @since S60 v5.0
	     * @param CRingTonePlayer& - Ring tone player reference
	     * @param TInt - Event type
	     * @param TInt - Status
	     * @return void
	     */
        virtual void Event(const CRingTonePlayer& aSrc,
	                       TInt aEventType,
	                       TInt aError);

        // From MDTMFToneObserver

        /**
	     * ?description
	     *
	     * @since S60 v5.0
	     * @param CDTMFTonePlayer& - DTMF tone player reference
	     * @param TInt - Event type
	     * @param TInt - Status
	     * @return void
	     */
        virtual void Event(const CDTMFTonePlayer& aPlayer,
	                       TInt aEventType,
	                       TInt aError);

        // From MTimeoutObserver

        /**
        * @since ?Series60_version
        * @param none
        * Review if all the expected events have ocurred once the time is over
        */
        void HandleTimeout(TInt error);

#ifdef __JITTER_BUFFER_TEST__
	// From MVoIPJitterBufferObserver
	virtual void Event(const CVoIPJitterBufferIntfc& aSrc, TInt aEventType);
#endif //__JITTER_BUFFER_TEST__


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
        CVoIPAudioServicesTestClass( CTestModuleIf& aTestModuleIf );

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

        /**
        * Set an event as expected and set default timeout
        * @since ?Series60_version
        */
        void AddExpectedEvent(TVoIPExpectedEvent event, TInt ms);


        /**
        * Unset an event as expected
        * @since ?Series60_version
        */
        TBool RemoveExpectedEvent(TVoIPExpectedEvent event);


        /**
        * @since ?Series60_version
        * @param none
        * Removes all expected events
        */
        void RemoveAllExpectedEvents();

        /**
        * Verify that the event was expected, removes it from the list
        * Signal the TestScripter with the returned error code
        * @since ?Series60_version
        */
        void ProcessEvent(TVoIPExpectedEvent aEvent, TInt aError);

        /**
        * Maps a event with a descriptor with its name
        * @since ?Series60_version
        */
        TPtrC EventName( TInt aKey );

        /*
        * Test methods are listed below.
        */

        /**
        * Sets a timeout different since the default
        * @since Series60_3_2
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        TInt SetTimeout( CStifItemParser& aItem );

        /**
        *
        * @since Series60_3_2
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        TInt SetExpectedEvents( CStifItemParser& aItem );

        /**
        *
        * @since Series60_3_2
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        TInt SetAllowedPanic( CStifItemParser& aItem );

        /**
        * Connect to the default client to the server
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */

        TInt ExampleL( CStifItemParser& aItem );

        TInt CreateVoIPUtilityFactory( CStifItemParser& aItem );
        TInt CreateRingToneFactory( CStifItemParser& aItem );
        TInt CreateDownlinkStream( CStifItemParser& aItem );
        TInt GetDownlinkVersion( CStifItemParser& aItem );
        TInt CreateUplinkStream( CStifItemParser& aItem );
        TInt GetUplinkVersion( CStifItemParser& aItem );
        TInt CreateDTMFTonePlayer( CStifItemParser& aItem );
        TInt CreateRingTonePlayer( CStifItemParser& aItem );
        TInt GetSupportedDownlinkFormats( CStifItemParser& aItem );
        TInt GetSupportedUplinkFormats( CStifItemParser& aItem );
        TInt SetDownlinkFormat( CStifItemParser& aItem );
        TInt SetUplinkFormat( CStifItemParser& aItem );
        TInt OpenDownlink( CStifItemParser& aItem );
        TInt OpenUplink( CStifItemParser& aItem );

        TInt Start( CStifItemParser& aItem );
        TInt Stop( CStifItemParser& aItem );
        TInt Close( CStifItemParser& aItem );
        TInt Gain( CStifItemParser& aItem );
        TInt Volume( CStifItemParser& aItem );
        TInt SetDevice( CStifItemParser& aItem );

        TInt InitDTMFTonePlayer( CStifItemParser& aItem );
        TInt DTMFTonePlay( CStifItemParser& aItem );
        TInt CloseDTMFPlayer( CStifItemParser& aItem );
        TInt StopDTMFTonePlayer( CStifItemParser& aItem );

        TInt InitRingTonePlayer( CStifItemParser& aItem );
        TInt CloseRingTonePlayer( CStifItemParser& aItem );
        TInt PlayRingTone( CStifItemParser& aItem );
        TInt PauseRingTone( CStifItemParser& aItem );
        TInt ResumeRingTone( CStifItemParser& aItem );
        TInt StopRingTone( CStifItemParser& aItem );
        TInt GetBufferType( CStifItemParser& aItem );

        TInt GetSupportedBitrates( CStifItemParser& aItem );
        TInt SetBitrate( CStifItemParser& aItem );
        TInt GetBitrate( CStifItemParser& aItem );
        TInt GetVAD( CStifItemParser& aItem );
        TInt ToggleVAD( CStifItemParser& aItem );
        TInt GetFrameMode( CStifItemParser& aItem );
        TInt ToggleFrameMode( CStifItemParser& aItem );
        TInt GetFrameModeRqrdForEC( CStifItemParser& aItem );
        TInt ConcealErrForNextBuf( CStifItemParser& aItem );
        TInt GetMode( CStifItemParser& aItem );
        TInt SetMode( CStifItemParser& aItem );
        TInt GetCNG( CStifItemParser& aItem );
        TInt ToggleCNG( CStifItemParser& aItem );
        TInt GetPLC( CStifItemParser& aItem );
        TInt TogglePLC( CStifItemParser& aItem );
        TInt BadLsfNextBuffer( CStifItemParser& aItem );

        void DoLoopback();
        void DisplayFormat(TVoIPCodecFormat aFormat);
        void GetSupportedBitrates(RArray<TUint>& aBrArr);
        void SetBitrate(TUint aBitrate);
        void GetBitrate(TUint& aBitrate);

        TInt ConfigEncAudDevice( CStifItemParser& aItem );


#ifdef __JITTER_BUFFER_TEST__
        TInt ConfigureJB();
        TInt ResetJB();
        void DelayDown();
        void DelayUp();
#endif //__JITTER_BUFFER_TEST__

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data

    	// reference to TestModuleIf
    	CTestModuleIf& 				iTestModuleIf;

    	// Dn/Uplink, DTMF, Ring tone APIs
    	CVoIPUtilityFactory*        iFactory;
    	CVoIPAudioDownlinkStream*   iVoIPDnlink;
    	CVoIPAudioUplinkStream*     iVoIPUplink;
    	CDTMFTonePlayer*            iDTMFTonePlayerDn;
    	CDTMFTonePlayer*            iDTMFTonePlayerUp;
    	CRingToneFactory*           iRingToneFactory;
    	CRingTonePlayer*            iRingTonePlayer;


    	// Codec format APIs
    	CVoIPFormatIntfc*           iDecFormatIntfc;
    	CVoIPFormatIntfc*           iEncFormatIntfc;

    	CVoIPG711EncoderIntfc*      iG711EncFormatIntfc;
    	CVoIPG711DecoderIntfc*      iG711DecFormatIntfc;
    	CVoIPFormatIntfc*           iG729EncFormatIntfc;
    	CVoIPG729DecoderIntfc*      iG729DecFormatIntfc;
    	CVoIPILBCEncoderIntfc*      iILBCEncFormatIntfc;
    	CVoIPILBCDecoderIntfc*      iILBCDecFormatIntfc;
    	CVoIPFormatIntfc*           iAMRNBEncFormatIntfc;
    	CVoIPFormatIntfc*           iAMRNBDecFormatIntfc;
    	CVoIPFormatIntfc*           iPCM16EncFormatIntfc;
    	CVoIPFormatIntfc*           iPCM16DecFormatIntfc;

#ifdef __JITTER_BUFFER_TEST__
	    CVoIPJitterBufferIntfc*     iJBIntfc;
	    CVoIPJBDataBuffer*          iPlayJBuf;
	    TUint                       iSeqNum;
	    TUint						iSeqNumIncrement;
#endif //__JITTER_BUFFER_TEST__

	    // For internal app state control
	    TAppState                   iDnLinkStatus;
	    TAppState                   iUpLinkStatus;
	    TVoIPCodecFormat            iDnLinkCodec;
	    TVoIPCodecFormat            iUpLinkCodec;

	    // For storing codec configuration locally
	    TBool               iPlc;
	    TBool               iCng;
	    TBool               iVad;
	    TBool               iFrameMode;
	    TBool               iFrameModeReqEC;
	    TBool               iSpkr;
	    TInt                iMaxGain;
	    TInt                iGain;
	    TInt                iMaxVolume;
	    TInt                iVolume;
	    TUint               iBitrate;
	    RArray<TUint>       iBitratesArray;
	    TInt 								loopbackCount;

	    // For streaming buffer control
	    CVoIPDataBuffer*    iPlayBuf;
	    CVoIPDataBuffer*		iRecBuf;
	    TPtr8               iPlayBufPtr;
	    TPtr8               iRecBufPtr;

	    TBool               iPlayBufReady;
	    TBool               iRecBufReady;

	    // For ring tone playback
	    RFile               iRTFile;
	    RFs                 iFs;



	    // Active object with a timer to timeout the test case
	    CSimpleTimeout * iTimeoutController;

	    // Indicates if the test case use a normal exit reason
	    TBool iNormalExitReason;

	    // List of expected events
	    RArray<TVoIPExpectedEvent> iExpectedEvents;

	    // List of notification event (by the callback)
	    RArray<TVoIPExpectedEvent> iOcurredEvents;

	    CActiveSchedulerWait* iActive;
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

#endif      // CVoIPAudioServicesTestClass_H

// End of File
