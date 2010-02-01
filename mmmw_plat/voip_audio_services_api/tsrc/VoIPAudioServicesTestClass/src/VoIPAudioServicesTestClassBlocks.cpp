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



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "VoIPAudioServicesTestClass.h"
#include "debug.h"

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
// CVoIPAudioServicesTestClass::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CVoIPAudioServicesTestClass::Delete()
{

    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::Delete")));

    iExpectedEvents.Close();
    iOcurredEvents.Close();

    iLog->Log(_L("Deleting test class..."));
	iLog->Log(_L(""));
	iLog->Log(_L(""));

	iBitratesArray.Reset();

    delete iG711DecFormatIntfc;
    delete iG711EncFormatIntfc;
    delete iG729EncFormatIntfc;
    delete iG729DecFormatIntfc;
    delete iILBCEncFormatIntfc;
    delete iILBCDecFormatIntfc;
    delete iAMRNBEncFormatIntfc;
    delete iAMRNBDecFormatIntfc;
    delete iPCM16EncFormatIntfc;
    delete iPCM16DecFormatIntfc;

    delete iRingTonePlayer;
    delete iRingToneFactory;
    delete iDTMFTonePlayerDn;
    delete iDTMFTonePlayerUp;

    delete iPlayBuf;

#ifdef __JITTER_BUFFER_TEST__
    delete iJBIntfc;
    delete iPlayJBuf;
#endif //__JITTER_BUFFER_TEST__

    if (iVoIPDnlink )
    	{
        if (iDnLinkStatus != ENotReady)
        	{
            iVoIPDnlink->Close();
        	}
        delete iVoIPDnlink;
        iVoIPDnlink = NULL;
    	}

    if (iVoIPUplink )
    	{
        if (iUpLinkStatus != ENotReady)
        	{
            iVoIPUplink->Close();
        	}
        delete iVoIPUplink;
        iVoIPUplink = NULL;
    	}

    delete iFactory;

    delete iDecFormatIntfc;
    delete iEncFormatIntfc;
}

// -----------------------------------------------------------------------------
// CVoIPAudioServicesTestClass::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CVoIPAudioServicesTestClass::RunMethodL( CStifItemParser& aItem )
{

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.

        ENTRY( "SetTimeout", CVoIPAudioServicesTestClass::SetTimeout ),
        ENTRY( "CreateFactory", CVoIPAudioServicesTestClass::CreateVoIPUtilityFactory ),
        ENTRY( "CreateRingToneFactory", CVoIPAudioServicesTestClass::CreateRingToneFactory ),
        ENTRY( "CreateDownlinkStream", CVoIPAudioServicesTestClass::CreateDownlinkStream ),
        ENTRY( "CreateUplinkStream", CVoIPAudioServicesTestClass::CreateUplinkStream ),
        ENTRY( "CreateDTMFTonePlayer", CVoIPAudioServicesTestClass::CreateDTMFTonePlayer ),
        ENTRY( "CreateRingTonePlayer", CVoIPAudioServicesTestClass::CreateRingTonePlayer ),
        ENTRY( "GetSupportedDownlinkFormats", CVoIPAudioServicesTestClass::GetSupportedDownlinkFormats ),
        ENTRY( "GetSupportedUplinkFormats", CVoIPAudioServicesTestClass::GetSupportedUplinkFormats ),
        ENTRY( "SetDownlinkFormat", CVoIPAudioServicesTestClass::SetDownlinkFormat ),
        ENTRY( "SetUplinkFormat", CVoIPAudioServicesTestClass::SetUplinkFormat ),
        ENTRY( "OpenDownlink", CVoIPAudioServicesTestClass::OpenDownlink ),
        ENTRY( "OpenUplink", CVoIPAudioServicesTestClass::OpenUplink ),
        ENTRY( "Gain", CVoIPAudioServicesTestClass::Gain ),
        ENTRY( "Volume", CVoIPAudioServicesTestClass::Volume ),
        ENTRY( "SetDevice", CVoIPAudioServicesTestClass::SetDevice ),
        ENTRY( "Close", CVoIPAudioServicesTestClass::Close ),
        ENTRY( "Start", CVoIPAudioServicesTestClass::Start ),
        ENTRY( "Stop", CVoIPAudioServicesTestClass::Stop ),
        ENTRY( "InitDTMFTonePlayer", CVoIPAudioServicesTestClass::InitDTMFTonePlayer ),
        ENTRY( "DTMFTonePlay", CVoIPAudioServicesTestClass::DTMFTonePlay ),
        ENTRY( "CloseDTMFPlayer", CVoIPAudioServicesTestClass::CloseDTMFPlayer ),
        ENTRY( "StopDTMFPlayer", CVoIPAudioServicesTestClass::StopDTMFTonePlayer ),
        ENTRY( "GetDownlinkVersion", CVoIPAudioServicesTestClass::GetDownlinkVersion ),
        ENTRY( "GetUplinkVersion", CVoIPAudioServicesTestClass::GetUplinkVersion ),
        ENTRY( "InitRingTonePlayer", CVoIPAudioServicesTestClass::InitRingTonePlayer ),
        ENTRY( "CloseRingTonePlayer", CVoIPAudioServicesTestClass::CloseRingTonePlayer ),
        ENTRY( "PlayRingTone", CVoIPAudioServicesTestClass::PlayRingTone ),
        ENTRY( "PauseRingTone", CVoIPAudioServicesTestClass::PauseRingTone ),
        ENTRY( "ResumeRingTone", CVoIPAudioServicesTestClass::ResumeRingTone ),
        ENTRY( "StopRingTone", CVoIPAudioServicesTestClass::StopRingTone ),
        ENTRY( "GetSupportedBitrates", CVoIPAudioServicesTestClass::GetSupportedBitrates ),
        ENTRY( "SetBitrate", CVoIPAudioServicesTestClass::SetBitrate ),
        ENTRY( "GetBitrate", CVoIPAudioServicesTestClass::GetBitrate ),
        ENTRY( "GetVAD", CVoIPAudioServicesTestClass::GetVAD ),
        ENTRY( "ToggleVAD", CVoIPAudioServicesTestClass::ToggleVAD ),
        ENTRY( "GetFrameMode", CVoIPAudioServicesTestClass::GetFrameMode ),
        ENTRY( "ToggleFrameMode", CVoIPAudioServicesTestClass::ToggleFrameMode ),
        ENTRY( "GetFrameModeRqrdForEC", CVoIPAudioServicesTestClass::GetFrameModeRqrdForEC ),
        ENTRY( "ConcealErrForNextBuf", CVoIPAudioServicesTestClass::ConcealErrForNextBuf ),
        ENTRY( "GetMode", CVoIPAudioServicesTestClass::GetMode ),
        ENTRY( "SetMode", CVoIPAudioServicesTestClass::SetMode ),
        ENTRY( "GetCNG", CVoIPAudioServicesTestClass::GetCNG ),
        ENTRY( "ToggleCNG", CVoIPAudioServicesTestClass::ToggleCNG ),
        ENTRY( "GetPLC", CVoIPAudioServicesTestClass::GetPLC ),
        ENTRY( "TogglePLC", CVoIPAudioServicesTestClass::TogglePLC ),
        ENTRY( "BadLsfNextBuffer", CVoIPAudioServicesTestClass::BadLsfNextBuffer ),
        ENTRY( "GetBufferType", CVoIPAudioServicesTestClass::GetBufferType ),

        ENTRY( "ConfigEncAudDevice", CVoIPAudioServicesTestClass::ConfigEncAudDevice ),

        ENTRY( "Example", CVoIPAudioServicesTestClass::ExampleL ),

#ifdef __JITTER_BUFFER_TEST__
        ENTRY( "ConfigureJB", CVoIPAudioServicesTestClass::ConfigureJB ),
        ENTRY( "ResetJB", CVoIPAudioServicesTestClass::ResetJB ),
        ENTRY( "DelayDown", CVoIPAudioServicesTestClass::DelayDown ),
        ENTRY( "DelayUp", CVoIPAudioServicesTestClass::DelayUp ),
#endif //__JITTER_BUFFER_TEST__

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

}


// -----------------------------------------------------------------------------
// CVoIPAudioServicesTestClass::EventName
// Return descriptor with the notification description
// -----------------------------------------------------------------------------
TPtrC CVoIPAudioServicesTestClass::EventName( TInt aKey )
{
    static TText* const badKeyword = (TText*)L"BadKeyword";
    static TText* const keywords[] =
    {

        (TText*)L"EOpenDownlinkComplete",
        (TText*)L"EOpenUplinkComplete",
        (TText*)L"EDownlinkClosed",
        (TText*)L"EUplinkClosed",
        (TText*)L"EOpenCompleteDNL",
        (TText*)L"EOpenCompleteUPL",
        (TText*)L"EOpenComplete",
        (TText*)L"EPlaybackComplete",
        (TText*)L"EEmptyBuffer",
        (TText*)L"EFillBuffer",

    };



    if( (TUint)aKey >= (sizeof( keywords )/sizeof(TText*)) )
    	{
        iLog->Log(_L("Keyword out of bounds"));
        TPtrC keyword( badKeyword );
        return keyword;
    	}
    else
    	{
        TPtrC keyword( keywords[aKey] );
        return keyword;
    	}
}

// -----------------------------------------------------------------------------
// CVoIPAudioServicesTestClass::AddExpectedEvent
// Add an event to the expected events' list
// -----------------------------------------------------------------------------
void CVoIPAudioServicesTestClass::AddExpectedEvent(TVoIPExpectedEvent event, TInt ms)
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::AddExpectedEvent")));
    iExpectedEvents.Append(event);
    TPtrC eventName = EventName(event);
    iLog->Log(_L("Adding expected event:(0x%02x)%S Total=%d"), event, &eventName, iExpectedEvents.Count() );

    if ( iTimeoutController && !iTimeoutController->IsActive() )
    	{
        if (ms > 0)
            {
            iTimeoutController->Start( TTimeIntervalMicroSeconds(ms * 1000) );
            }
        else
            {
            iLog->Log(_L("Timeout with default value (1s)"));
            iTimeoutController->Start( TTimeIntervalMicroSeconds(1000000) );
            }
    	}
}

// -----------------------------------------------------------------------------
// CVoIPAudioServicesTestClass::RemoveExpectedEvent
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
TBool CVoIPAudioServicesTestClass::RemoveExpectedEvent(TVoIPExpectedEvent aEvent)
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::RemoveExpectedEvent")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::RemoveExpectedEvent"));
    TBool match = EFalse;
    for (TUint i=0; i < iExpectedEvents.Count() ; i++)
    	{
        if (iExpectedEvents[i] == aEvent)
        	{
        	iLog->Log(_L("Expeted events: %d"), iExpectedEvents.Count());
            iExpectedEvents.Remove(i);
            match = ETrue;
            break;
        	}
    	}

    return match;
}


// -----------------------------------------------------------------------------
// CVoIPAudioServicesTestClass::RemoveAllExpectedEvents
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
void CVoIPAudioServicesTestClass::RemoveAllExpectedEvents()
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::RemoveAllExpectedEvents")));
    iLog->Log(_L("Removing all expected events"));

    iExpectedEvents.Reset();
    iOcurredEvents.Reset();
}


// -----------------------------------------------------------------------------
// CVoIPAudioServicesTestClass::ProcessEvent
// -----------------------------------------------------------------------------
void CVoIPAudioServicesTestClass::ProcessEvent(TVoIPExpectedEvent aEvent, TInt aError)
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::ProcessExpectedEvent")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::ProcessExpectedEvent"));
    TPtrC nameEvent = EventName(aEvent);

    if (aError == KErrNone)
    	{
        if (RemoveExpectedEvent(aEvent))
        	{
            iLog->Log(_L("Expected Event: (0x%02x)%S has ocurred Total=%d"), aEvent, &nameEvent,iExpectedEvents.Count());
        	}
        else
        	{
            iLog->Log(_L("Event: (0x%02x)%S has ocurred"), aEvent, &nameEvent);
            return;
        	}

        if (iExpectedEvents.Count() == 0 )
        	{
            Signal();
            iTimeoutController->Cancel();
        	}

    	}
    else
    	{
        iLog->Log(_L("[Error] Event: (0x%02x)%S return with error code=%d"), aEvent, &nameEvent, aError);
        if (iExpectedEvents.Count() != 0 )
        	{
            RemoveExpectedEvent(aEvent);
        	}
        iTimeoutController->Cancel();
        Signal(KErrCallbackErrorCode);
    	}
}

// -----------------------------------------------------------------------------
// CVoIPAudioServicesTestClass::HandleTimeout
// Review if all the expected events have ocurred once the time is over
// -----------------------------------------------------------------------------

void CVoIPAudioServicesTestClass::HandleTimeout(TInt aError)
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::HandleTimeout")));

    if (aError != KErrNone)
    	{
        if (iExpectedEvents.Count() == 0 )
        	{
            iLog->Log(_L("Timing out but events have ocurred"));
            Signal();
        	}
        else
        	{
            RemoveAllExpectedEvents();
            iLog->Log(_L("Timing out and events still pending"));
            Signal(KErrEventPending);
        	}
    	}
    else
    	{
        iLog->Log(_L("Timing out return a error %d"), aError);
        Signal(aError);
    	}
}

// -----------------------------------------------------------------------------
// CVoIPAudioServicesTestClass::SetTimeout
// Create a timer and set a timeout
// When the timeout is reached the test case is marked as failed
// It's used rather than the "timeout" keyword in the configuration file
// because in this way the log continues
// -----------------------------------------------------------------------------
TInt CVoIPAudioServicesTestClass::SetTimeout( CStifItemParser& aItem )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::SetTimeout")));
    TInt timeout=0;
    TInt error = aItem.GetNextInt(timeout) ;
    if ( iTimeoutController )
    	{
        if ( timeout > 0 )
        	{
            iTimeoutController->Start( TTimeIntervalMicroSeconds(timeout*1000) );
        	}
        else
        	{
            iTimeoutController->Start( TTimeIntervalMicroSeconds(1000000) );
        	}
    	}
    else
    	{
        iLog->Log(_L("Timeout Controller doesn't exist"));
        error = KErrTimeoutController;
    	}

    return error;
}


// -----------------------------------------------------------------------------
// Uses the TestModuleBase API to allow a panic as exit reason for a test case
// -----------------------------------------------------------------------------
TInt CVoIPAudioServicesTestClass::SetAllowedPanic( CStifItemParser& aItem )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::SetAllowedPanic")));
    TInt error = KErrNone;
    TInt panicCode;
    TPtrC panicType;
    if (  ( KErrNone == aItem.GetNextString(panicType) ) &&
    ( KErrNone == aItem.GetNextInt(panicCode) )  )
    	{
        iLog->Log(_L("Allowing panic: %S %d"), &panicType, panicCode);
        iTestModuleIf.SetExitReason( CTestModuleIf::EPanic, panicCode );
        iNormalExitReason = EFalse;
    	}
    else
    	{
        iLog->Log(KMsgBadTestParameters);
        error = KErrBadTestParameter;
    	}

    return error;
}

// -----------------------------------------------------------------------------
// CVoIPAudioServicesTestClass::SetExpectedEvents()
// -----------------------------------------------------------------------------
TInt CVoIPAudioServicesTestClass::SetExpectedEvents( CStifItemParser& aItem )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::SetExpectedEvents")));
    TInt error = KErrNone;
    TInt event=0;
    while ( KErrNone == aItem.GetNextInt(event))
    	{
        AddExpectedEvent(static_cast<TVoIPExpectedEvent>(event), 0); // Default timeout value
    	}

    return error;
}


// -----------------------------------------------------------------------------
// CVoIPAudioServicesTestClass::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CVoIPAudioServicesTestClass::ExampleL( CStifItemParser& aItem )
{

    // Print to UI
    _LIT( KVoIPAudioServicesTestClass, "VoIPAudioServicesTestClass" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KVoIPAudioServicesTestClass, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KVoIPAudioServicesTestClass, KParam, i, &string );
        i++;
        }

    return KErrNone;
}

TInt CVoIPAudioServicesTestClass::CreateVoIPUtilityFactory( CStifItemParser& /*aItem */)
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::CreateFactory")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::CreateFactory"));
    TInt error = KErrNone;

    error = CVoIPUtilityFactory::CreateFactory(iFactory);

    return error;
}


TInt CVoIPAudioServicesTestClass::CreateRingToneFactory( CStifItemParser& /*aItem */)
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::CreateRingToneFactory")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::CreateRingToneFactory"));
    TInt error = KErrNone;

    error = CRingToneFactory::CreateFactory(iRingToneFactory);

    return error;
}

TInt CVoIPAudioServicesTestClass::CreateDownlinkStream( CStifItemParser& /*aItem */)
{
	iLog->Log(_L("MVoIPUplinkObserver::KOpenComplete: iUpLinkStatus = %d"), iUpLinkStatus);
    FTRACE(FPrint(_L("MVoIPUplinkObserver::KOpenComplete: iUpLinkStatus = %d"), iUpLinkStatus));
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::CreateDownlinkStream")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::CreateDownlinkStream"));
    TInt error = KErrNone;

    const TVersion ver(1, 0, 0);

#ifdef __JITTER_BUFFER_TEST__
    // Create DNL with JB interface
    error = iFactory->CreateDownlinkStream(ver, iJBIntfc, iVoIPDnlink);

    if (error == KErrNone && iJBIntfc)
        {
        iJBIntfc->SetObserver(*this);
        }
#else  //__JITTER_BUFFER_TEST__
    // Create DNL without JB interface
    error = iFactory->CreateDownlinkStream(ver,
                                           CVoIPUtilityFactory::EVoIPCall,
                                           iVoIPDnlink);
#endif //__JITTER_BUFFER_TEST__

    return error;
}


TInt CVoIPAudioServicesTestClass::GetDownlinkVersion( CStifItemParser& /*aItem */)
{
	FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::GetDownlinkVersion")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::GetDownlinkVersion"));
    TInt error = KErrNone;

    TVersion ver(0,0,0);
    error = iVoIPDnlink->GetVersion(ver);

    return error;
}


TInt CVoIPAudioServicesTestClass::CreateUplinkStream( CStifItemParser& /*aItem */)
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::CreateUplinkStream")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::CreateUplinkStream"));
    TInt error = KErrNone;

    const TVersion ver(1, 0, 0);
    error = iFactory->CreateUplinkStream(ver,
                                       CVoIPUtilityFactory::EVoIPCall,
                                       iVoIPUplink);
    return error;
}


TInt CVoIPAudioServicesTestClass::GetUplinkVersion( CStifItemParser& /*aItem */)
{
	FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::GetUplinkVersion")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::GetUplinkVersion"));
    TInt error = KErrNone;

    TVersion ver(0,0,0);
    error = iVoIPUplink->GetVersion(ver);

    return error;
}


TInt CVoIPAudioServicesTestClass::CreateDTMFTonePlayer( CStifItemParser& /*aItem */)
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::CreateDTMFTonePlayer")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::CreateDTMFTonePlayer"));
    TInt error = KErrNone;

    if (iVoIPDnlink)
    	{
        error = iFactory->CreateDTMFTonePlayer(iDTMFTonePlayerDn);
    	}

    if (iVoIPUplink)
    	{
    	error = iFactory->CreateDTMFTonePlayer(iDTMFTonePlayerUp);
    	}

    return error;
}


TInt CVoIPAudioServicesTestClass::CreateRingTonePlayer( CStifItemParser& /*aItem */)
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::CreateRingTonePlayer")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::CreateRingTonePlayer"));
    TInt error = KErrNone;

    if (iRingToneFactory)
    	{
        error = iRingToneFactory->CreateRingTonePlayer(iRingTonePlayer);
    	}

    return error;
}



TInt CVoIPAudioServicesTestClass::GetSupportedDownlinkFormats( CStifItemParser& /*aItem */)
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::GetSupportedDownlinkFormats")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::GetSupportedDownlinkFormats"));
    TInt error = KErrNone;

    RArray<TVoIPCodecFormat> formats;
    formats.Reset();
    error = iFactory->GetSupportedDownlinkFormats(formats);
    if (error == KErrNone)
        {
        TInt count = formats.Count();
        if (count == 0)
            {
            iLog->Log(_L("No DNL codec available"));
            }
        else
            {
            iLog->Log(_L("Supported DNL codecs"));
            for (TInt i = 0; i < count; i++)
                {
                DisplayFormat(formats[i]);
                }
            }
        }

    formats.Close();

    return error;
}

TInt CVoIPAudioServicesTestClass::GetSupportedUplinkFormats( CStifItemParser& /*aItem */)
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::GetSupportedUplinkFormats")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::GetSupportedUplinkFormats"));
    TInt error = KErrNone;

    RArray<TVoIPCodecFormat> formats;
    formats.Reset();
    error = iFactory->GetSupportedUplinkFormats(formats);
    if (error == KErrNone)
        {
        TInt count = formats.Count();
        if (count == 0)
            {
            iLog->Log(_L("No UPL codec available"));
            }
        else
            {
            iLog->Log(_L("Supported UPL codecs"));
            for (TInt i = 0; i < count; i++)
                {
                DisplayFormat(formats[i]);
                }
            }
        }

    formats.Close();

    return error;
}





TInt CVoIPAudioServicesTestClass::SetDownlinkFormat( CStifItemParser& aItem )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::SetDownlinkFormat")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::SetDownlinkFormat"));
    TInt error = KErrNone;

    TPtrC decoderType;
    error = aItem.GetNextString(decoderType);
    if (error == KErrNone)
    	{
        CVoIPFormatIntfc* formatIntfc = NULL;

        // NOTE: We can now set either 10ms or 20ms G.711 frame;
        //	 	 call GetSupportedDownlinkFormats() to see if it
        //		 returns EG711 or EG711_10MS and set codec accordingly.
        if (decoderType == KTagG711)
        	{
        	error = iVoIPDnlink->SetFormat(EG711, formatIntfc);
        	if (error == KErrNone)
        		{
                if (iG711DecFormatIntfc)
                	{
                	delete iG711DecFormatIntfc;
                	iG711DecFormatIntfc = NULL;
                	}

		            iG711DecFormatIntfc = (CVoIPG711DecoderIntfc*)formatIntfc;
		            error = iG711DecFormatIntfc->SetObserver(*this);
		            iDnLinkCodec = EG711;

		            iLog->Log(_L("G.711 DNL Codec Set: error = %d"), error);
        		}
        	else
                {
                iLog->Log(_L("DNL Codec Setting Failed: error = %d"), error);
                }
        	}
        else if (decoderType == KTagG729)
        	{
        	error = iVoIPDnlink->SetFormat(EG729, formatIntfc);
        	if (error == KErrNone)
        		{
                if (iG729DecFormatIntfc)
                	{
		            delete iG729DecFormatIntfc;
		            iG729DecFormatIntfc = NULL;
		            }

	            iG729DecFormatIntfc = (CVoIPG729DecoderIntfc*)formatIntfc;
	            error = iG729DecFormatIntfc->SetObserver(*this);
	            iDnLinkCodec = EG729;

	            iLog->Log(_L("G.729 DNL Codec Set: error = %d"), error);
        		}
        	else
                {
                iLog->Log(_L("DNL Codec Setting Failed: error = %d"), error);
                }
        	}
        else if (decoderType == KTagILBC)
        	{
        	error = iVoIPDnlink->SetFormat(EILBC, formatIntfc);
        	if (error == KErrNone)
        		{
                if (iILBCDecFormatIntfc)
		            {
		            delete iILBCDecFormatIntfc;
		            iILBCDecFormatIntfc = NULL;
		            }

		            iILBCDecFormatIntfc = (CVoIPILBCDecoderIntfc*)formatIntfc;
		            error = iILBCDecFormatIntfc->SetObserver(*this);
		            iDnLinkCodec = EILBC;

		            iLog->Log(_L("iLBC DNL Codec Set: error = %d"), error);
        		}
        	else
                {
                iLog->Log(_L("DNL Codec Setting Failed: error = %d"), error);
                }
        	}
        else if (decoderType == KTagAMRNB)
        	{
        	error = iVoIPDnlink->SetFormat(EAMR_NB, formatIntfc);
        	if (error == KErrNone)
        		{
                if (iAMRNBDecFormatIntfc)
		            {
		            delete iAMRNBDecFormatIntfc;
		            iAMRNBDecFormatIntfc = NULL;
		            }

		            iAMRNBDecFormatIntfc = formatIntfc;
		            error = iAMRNBDecFormatIntfc->SetObserver(*this);
		            iDnLinkCodec = EAMR_NB;

		            iLog->Log(_L("AMR-NB DNL Codec Set: error = %d"), error);
        		}
        	else
                {
                iLog->Log(_L("DNL Codec Setting Failed: error = %d"), error);
                }
        	}
        else if (decoderType == KTagPCM16)
        	{
        	error = iVoIPDnlink->SetFormat(EPCM16, formatIntfc);
        	if (error == KErrNone)
        		{
                if (iPCM16DecFormatIntfc)
		            {
		            delete iPCM16DecFormatIntfc;
		            iPCM16DecFormatIntfc = NULL;
		            }

		            iPCM16DecFormatIntfc = formatIntfc;
		            error = iPCM16DecFormatIntfc->SetObserver(*this);
		            iDnLinkCodec = EPCM16;

		            iLog->Log(_L("PCM16 DNL Codec Set: error = %d"), error);
        		}
        	else
                {
                iLog->Log(_L("DNL Codec Setting Failed: error = %d"), error);
                }
        	}
        else
        	{
            iLog->Log(KMsgBadTestParameters);
            error = KErrBadTestParameter;
            return error;
        	}
    	}

    if (error == KErrNone)
        {
        delete iPlayBuf;
        iPlayBuf = NULL;

#ifdef __JITTER_BUFFER_TEST__
        iPlayJBuf = NULL;
        if (iDnLinkCodec != EPCM16)
            {
            iFactory->CreateBuffer(iPlayBuf,
                                   CVoIPDataBuffer::EJitterBuffer,
                                   iDnLinkCodec);
            iPlayJBuf = static_cast<CVoIPJBDataBuffer*>(iPlayBuf);
            }
#else  //__JITTER_BUFFER_TEST__
        iFactory->CreateBuffer(iPlayBuf,
                               CVoIPDataBuffer::EStandard,
                               iDnLinkCodec);
#endif //__JITTER_BUFFER_TEST__
        }

    return error;
}


TInt CVoIPAudioServicesTestClass::OpenDownlink(CStifItemParser& /*aItem */ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::OpenDownlink")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::OpenDownlink"));
    TInt error = KErrNone;

    if (iDnLinkStatus == ENotReady)
    	{
    	error = iVoIPDnlink->Open(*this);
    	if (error != KErrNone)
    		{
            iLog->Log(_L("DNL open error: error = %d"), error);
    		}
    	else
    		{
      		iDnLinkStatus = EReady;
            AddExpectedEvent(EOpenDownlinkComplete, KMediumTimeout);
    		}
    	}
    else
    	{
    	iLog->Log(_L("DNL already open: error = %d"), KErrInUse);
    	}

  return error;
}


TInt CVoIPAudioServicesTestClass::SetUplinkFormat( CStifItemParser& aItem )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::SetUplinkFormat")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::SetUplinkFormat"));
    TInt error = KErrNone;

    TPtrC encoderType;
    error = aItem.GetNextString(encoderType);
    if (error == KErrNone)
    	{
        CVoIPFormatIntfc* formatIntfc = NULL;
        if (encoderType == KTagG711)
        	{
		    // NOTE: We can now set either 10ms or 20ms G.711 frame;
		    //	 	 call GetSupportedDownlinkFormats() to see if it
		    //		 returns EG711 or EG711_10MS and set codec accordingly.
        	error = iVoIPUplink->SetFormat(EG711, formatIntfc);
        	if (error == KErrNone)
        		{
                if (iG711EncFormatIntfc)
		            {
		            delete iG711EncFormatIntfc;
		            iG711EncFormatIntfc = NULL;
		            }

                iG711EncFormatIntfc = (CVoIPG711EncoderIntfc*)formatIntfc;
                error = iG711EncFormatIntfc->SetObserver(*this);
                iUpLinkCodec = EG711;

                iLog->Log(_L("G.711 UPL Codec Set: error = %d"), error);
        		}
        	else
                {
                iLog->Log(_L("UPL Codec Setting Failed: error = %d"), error);
                }
        	}
        else if (encoderType == KTagG729)
        	{
        	error = iVoIPUplink->SetFormat(EG729, formatIntfc);
        	if (error == KErrNone)
        		{
                if (iG729EncFormatIntfc)
		            {
		            delete iG729EncFormatIntfc;
		            iG729EncFormatIntfc = NULL;
		            }

                iG729EncFormatIntfc = formatIntfc;
                error = iG729EncFormatIntfc->SetObserver(*this);
                iUpLinkCodec = EG729;

                iLog->Log(_L("G.729 UPL Codec Set: error = %d"), error);
        		}
        	else
                {
                iLog->Log(_L("UPL Codec Setting Failed: error = %d"), error);
                }
        	}
        else if (encoderType == KTagILBC)
        	{
        	error = iVoIPUplink->SetFormat(EILBC, formatIntfc);
        	if (error == KErrNone)
        		{
        		if (iILBCEncFormatIntfc)
		            {
		            delete iILBCEncFormatIntfc;
		            iILBCEncFormatIntfc = NULL;
		            }

        		iILBCEncFormatIntfc = (CVoIPILBCEncoderIntfc*)formatIntfc;
        		error = iILBCEncFormatIntfc->SetObserver(*this);
        		iUpLinkCodec = EILBC;

        		iLog->Log(_L("iLBC UPL Codec Set: error = %d"), error);
        		}
        	else
                {
                iLog->Log(_L("UPL Codec Setting Failed: error = %d"), error);
                }
        	}
        else if (encoderType == KTagAMRNB)
        	{
        	error = iVoIPUplink->SetFormat(EAMR_NB, formatIntfc);
        	if (error == KErrNone)
        		{
                if (iAMRNBEncFormatIntfc)
		            {
		            delete iAMRNBEncFormatIntfc;
		            iAMRNBEncFormatIntfc = NULL;
		            }

                iAMRNBEncFormatIntfc = formatIntfc;
                error = iAMRNBEncFormatIntfc->SetObserver(*this);
                iUpLinkCodec = EAMR_NB;

                iLog->Log(_L("AMR-NB UPL Codec Set: error = %d"), error);
        		}
        	else
                {
                iLog->Log(_L("UPL Codec Setting Failed: error = %d"), error);
                }
        	}
        else if (encoderType == KTagPCM16)
        	{
        	error = iVoIPUplink->SetFormat(EPCM16, formatIntfc);
        	if (error == KErrNone)
        		{
                if (iPCM16EncFormatIntfc)
		            {
		            delete iPCM16EncFormatIntfc;
		            iPCM16EncFormatIntfc = NULL;
		            }

                iPCM16EncFormatIntfc = formatIntfc;
                error = iPCM16EncFormatIntfc->SetObserver(*this);
                iUpLinkCodec = EPCM16;

                iLog->Log(_L("PCM16 UPL Codec Set: error = %d"), error);
        		}
        	else
                {
                iLog->Log(_L("UPL Codec Setting Failed: error = %d"), error);
                }
        	}
        else
        	{
            iLog->Log(KMsgBadTestParameters);
            error = KErrBadTestParameter;
            return error;
        	}
    	}

        iFactory->CreateBuffer(iRecBuf,
        											 CVoIPDataBuffer::EStandard,
                               iUpLinkCodec);
                               
    return error;
}



TInt CVoIPAudioServicesTestClass::OpenUplink(CStifItemParser& /*aItem */ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::OpenUplink")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::OpenUplink"));
    TInt error = KErrNone;

    if (iUpLinkStatus == ENotReady)
    	{
    	error = iVoIPUplink->Open(*this);
    	if (error != KErrNone)
    		{
            iLog->Log(_L("UPL open error: error = %d"), error);
    		}
    	else
    		{
            AddExpectedEvent(EOpenUplinkComplete, KMediumTimeout);
            iUpLinkStatus = EReady;
    		}
    	}
    else
    	{
    	iLog->Log(_L("UPL already open: error = %d"), KErrInUse);
    	}

    return error;
}

// ----------------------------------------------------------------------------
// CVoIPAudioServicesTestClass::DisplayFormat
//
// ----------------------------------------------------------------------------
//
void CVoIPAudioServicesTestClass::DisplayFormat(TVoIPCodecFormat aFormat)
{
    if (aFormat == EAMR_NB)
        {
        iLog->Log(_L("AMR-NB"));
        }
    else if (aFormat == EG711)
        {
        iLog->Log(_L("G.711"));
        }
    else if (aFormat == EG729)
        {
        iLog->Log(_L("G.729"));
        }
    else if (aFormat == EILBC)
        {
        iLog->Log(_L("iLBC"));
        }
    else if (aFormat == EPCM16)
        {
        iLog->Log(_L("PCM-16"));
        }
}


TInt CVoIPAudioServicesTestClass::Close(CStifItemParser& aItem  )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::Close")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::Close"));
    TInt error = KErrNone;

    TPtrC closeType;
    error = aItem.GetNextString(closeType);
    if (error == KErrNone)
    	{
        if (closeType == KTagUplink)
        	{
	    	iRecBufPtr.Set(NULL, 0, 0);
	        iRecBufReady = EFalse;
	        iUpLinkCodec = ENULL;
	        iVoIPUplink->Close();
	        iUpLinkStatus = ENotReady;
	        iLog->Log(_L("Close Uplink"));
	        AddExpectedEvent(EUplinkClosed, KMediumTimeout);
        	}
	    else if (closeType == KTagDnlink)
	    	{
	    	iPlayBufPtr.Set(NULL, 0, 0);
	        iVoIPDnlink->Close();
	        iDnLinkStatus = ENotReady;
	        iPlayBufReady = EFalse;
	        iDnLinkCodec = ENULL;
	        iLog->Log(_L("Close Dnlink"));
	        AddExpectedEvent(EDownlinkClosed, KMediumTimeout);
		    }
		else
		    {
		    iLog->Log(KMsgBadTestParameters);
		    error = KErrBadTestParameter;
		    }
    	}

    return error;
}


TInt CVoIPAudioServicesTestClass::Start(CStifItemParser& aItem  )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::Start")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::Start"));
    TInt error = KErrNone;

    TPtrC startType;
    error = aItem.GetNextString(startType);
    if (error == KErrNone)
    	{
        if (startType == KTagUplink)
        	{
        	if (iUpLinkStatus == EReady)
        		{
	            iVoIPUplink->Start();
	            iUpLinkStatus = EStreaming;
	            iLog->Log(_L("Start Uplink"));
	            AddExpectedEvent(EEmptyBuffer, KMediumTimeout);
        		}

        	else
        		{
	            iLog->Log(_L("UPL not ready"));
	            error = KErrNotReady;
        		}
        	}
        else if (startType == KTagDnlink)
        	{
        	if (iDnLinkStatus == EReady)
        		{
	            iVoIPDnlink->Start();
	            iDnLinkStatus = EStreaming;
	            iLog->Log(_L("Start Dnlink"));
	            AddExpectedEvent(EFillBuffer, KMediumTimeout);
        		}

        	else
        		{
	            iLog->Log(_L("DNL not ready"));
	            error = KErrNotReady;
        		}
        	}
        else
        	{
        	iLog->Log(KMsgBadTestParameters);
            error = KErrBadTestParameter;
        	}
    	}

    return error;
}


TInt CVoIPAudioServicesTestClass::Stop(CStifItemParser& aItem  )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::Stop")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::Stop"));
    TInt error = KErrNone;

    TPtrC stopType;
    error = aItem.GetNextString(stopType);
    if (error == KErrNone)
    	{
        if (stopType == KTagUplink)
        	{
        	if (iUpLinkStatus == EStreaming)
        		{
	            iVoIPUplink->Stop();
	            iUpLinkStatus = EReady;
	            iLog->Log(_L("Stop Uplink"));
        		}
        	else
        		{
	            iLog->Log(_L("UPL not started"));
	            error = KErrNotReady;
        		}
        	}
        else if (stopType == KTagDnlink)
        	{
        	if (iDnLinkStatus == EStreaming)
        		{
	            iVoIPDnlink->Stop();
	            iDnLinkStatus = EReady;
	            iLog->Log(_L("Stop Dnlink"));
        		}
        	else
        		{
        		iLog->Log(_L("DNL not started"));
        		error = KErrNotReady;
        		}
        	}
        else
        	{
        	iLog->Log(KMsgBadTestParameters);
            error = KErrBadTestParameter;
        	}
    	}

    return error;
}


TInt CVoIPAudioServicesTestClass::Gain(CStifItemParser& /*aItem */ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::Gain")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::Gain"));
    TInt error = KErrNone;

    iVoIPUplink->GetMaxGain(iMaxGain);
    iVoIPUplink->SetGain(iMaxGain);
    iLog->Log(_L("SetMaxGain: %d"), iMaxGain);
    iVoIPUplink->GetGain(iGain);
    if (iGain != iMaxGain)
    	{
        iLog->Log(_L("GetGain doesn't return expected MaxGain!!! returned Gain = %d"), iGain);
        return KErrUnexpectedValue;
    	}

    iVoIPUplink->SetGain(0);
    iLog->Log(_L("MuteMic"));
    iVoIPUplink->GetGain(iGain);

    if (iGain != 0)
    	{
        iLog->Log(_L("GetGain does not return expected Mute value!!! returned Gain = %d"), iGain);
        return KErrUnexpectedValue;
    	}

  return error;
}


TInt CVoIPAudioServicesTestClass::Volume(CStifItemParser& /*aItem */ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::Volume")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::Volume"));
    TInt error = KErrNone;

    iVoIPDnlink->GetMaxVolume(iMaxVolume);
    iVoIPDnlink->SetVolume(iMaxVolume);
    iLog->Log(_L("SetMaxVolume: %d"), iMaxVolume);
    iVoIPDnlink->GetVolume(iVolume);
    if (iVolume != iMaxVolume)
    	{
        iLog->Log(_L("GetVolume doesn't return expected MaxVolume!!! returned Volume = %d"), iVolume);
        return KErrUnexpectedValue;
    	}

    iVoIPDnlink->SetVolume(0);
    iLog->Log(_L("Mute Volume"));
    iVoIPDnlink->GetVolume(iVolume);

    if (iVolume != 0)
    	{
        iLog->Log(_L("GetVolume does not return expected Mute value!!! returned Volume = %d"), iVolume);
        return KErrUnexpectedValue;
    	}

  return error;
}



TInt CVoIPAudioServicesTestClass::SetDevice(CStifItemParser& aItem  )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::SetDevice")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::SetDevice"));
    TInt error = KErrNone;

    TPtrC deviceType;
    error = aItem.GetNextString(deviceType);
    if (error == KErrNone)
    	{
        if (deviceType == KTagHandset)
        	{
        	iVoIPDnlink->SetAudioDevice(CVoIPAudioDownlinkStream::EHandset);
        	}
        else if (deviceType == KTagLoudSpeaker)
        	{
        	iVoIPDnlink->SetAudioDevice(CVoIPAudioDownlinkStream::ELoudSpeaker);
        	}
        else
        	{
            iLog->Log(KMsgBadTestParameters);
            error = KErrBadTestParameter;
            return error;
        	}
    	}

    CVoIPAudioDownlinkStream::TVoIPOutputDevice device;
    iVoIPDnlink->GetAudioDevice(device);
    if ((device == CVoIPAudioDownlinkStream::EHandset) && (deviceType == KTagHandset))
    	{
        iLog->Log(_L("Routing to Private Spkr"));
    	}
    else if((device == CVoIPAudioDownlinkStream::ELoudSpeaker) && (deviceType == KTagLoudSpeaker))
    	{
        iLog->Log(_L("Routing to Loudspeaker"));
    	}
    else
    	{
    	iLog->Log(_L("SetDevice failed"));
    	}

    return error;
}


TInt CVoIPAudioServicesTestClass::InitDTMFTonePlayer(CStifItemParser& aItem  )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::InitDTMFTonePlayer")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::InitDTMFTonePlayer"));
    TInt error = KErrNone;

    TPtrC initLink;
    error = aItem.GetNextString(initLink);
    if (error == KErrNone)
    	{
        if (initLink == KTagDnlink)
        	{
        	if (iDTMFTonePlayerDn && (iDnLinkStatus == EReady || iDnLinkStatus == EStreaming))
        		{
        		error = iDTMFTonePlayerDn->AddVoIPAudioDownlinkStream(*iVoIPDnlink);

        		if (error == KErrNone)
        			{
        			error = iDTMFTonePlayerDn->Open(*this); //sync call
        			if (error == KErrNone)
        				{
        				iLog->Log(_L("DTMF DNL Initialized: error = %d"), error);
        				AddExpectedEvent(EOpenCompleteDNL, KMediumTimeout);
        				}
        			}

        		if (error != KErrNone)
        			{
        			iLog->Log(_L("DTMF DNL-PLR Error"), error);
        			}
        		}
        	else
        		{
        		iLog->Log(_L("Open DNL first"), KErrNotFound);
        		}
        	}
        else if (initLink == KTagUplink)
        	{
        	if (iDTMFTonePlayerUp && (iUpLinkStatus == EReady || iUpLinkStatus == EStreaming))
        		{
        		error = iDTMFTonePlayerUp->AddVoIPAudioUplinkStream(*iVoIPUplink);

        		if (error == KErrNone)
        			{
        			error = iDTMFTonePlayerUp->Open(*this); //sync call
        			if (error == KErrNone)
        				{
		                iLog->Log(_L("DTMF UPL Initialized: error = %d"), error);
		                AddExpectedEvent(EOpenCompleteUPL, KMediumTimeout);
		                }
        			}

        		if (error != KErrNone)
        			{
        			iLog->Log(_L("DTMF UPL-PLR Error"), error);
        			}
        		}
        	else
        		{
        		iLog->Log(_L("Open UPL first"), KErrNotFound);
        		}
        	}
        else
        	{
        	iLog->Log(KMsgBadTestParameters);
        	error = KErrBadTestParameter;
            }
    }

    return error;
}




TInt CVoIPAudioServicesTestClass::DTMFTonePlay(CStifItemParser& aItem  )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::DTMFTonePlay")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::DTMFTonePlay"));
    TInt error = KErrNone;

    TPtrC initLink;
    error = aItem.GetNextString(initLink);
    if (error == KErrNone)
    	{
        if (initLink == KTagDnlink)
        	{
        	if (iDTMFTonePlayerDn && (iDnLinkStatus == EReady || iDnLinkStatus == EStreaming))
        		{
	            TChar tone1 = '0';
	            TChar tone2 = '1';
	            TChar tone3 = '2';
	            TBufC<KTonesBufSize> tones;
	            TPtr ptr = tones.Des();
	            ptr.Append(tone1);
	            ptr.Append(tone2);
	            ptr.Append(tone3);
	            iDTMFTonePlayerDn->Play(ptr);
	            iLog->Log(_L("Start to play DTMF"));
        		}
            else
            	{
            	iLog->Log(_L("Open DNL first"), KErrNotFound);
            	}
        	}
        else if (initLink == KTagUplink)
        	{
        	if (iDTMFTonePlayerUp && (iUpLinkStatus == EReady || iUpLinkStatus == EStreaming))
        		{
	            TChar tone1 = '9';
	            TChar tone2 = '8';
	            TChar tone3 = '7';
	            TBufC<KTonesBufSize> tones;
	            TPtr ptr = tones.Des();
	            ptr.Append(tone1);
	            ptr.Append(tone2);
	            ptr.Append(tone3);

	            iDTMFTonePlayerUp->Play(ptr);
	            iLog->Log(_L("Start to play DTMF"));
        		}
            else
	            {
	            iLog->Log(_L("Open UPL first"), KErrNotFound);
	            }
        	}
        else
        	{
        	iLog->Log(KMsgBadTestParameters);
        	error = KErrBadTestParameter;
            }
    }

    return error;
}


TInt CVoIPAudioServicesTestClass::CloseDTMFPlayer(CStifItemParser& aItem  )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::CloseDTMFPlayer")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::CloseDTMFPlayer"));
    TInt error = KErrNone;

    TPtrC closedLink;
    error = aItem.GetNextString(closedLink);
    if (error == KErrNone)
    	{
        if (closedLink == KTagDnlink)
        	{
            if (iDTMFTonePlayerDn && (iDnLinkStatus == EReady || iDnLinkStatus == EStreaming))
            	{
                iDTMFTonePlayerDn->Close();
                error = iDTMFTonePlayerDn->RemoveVoIPAudioDownlinkStream(*iVoIPDnlink);
                iLog->Log(_L("DTMF DNL Closed: error = %d"), error);
            	}
            else
            	{
            	iLog->Log(_L("DTMF DNL-PLR not open"), KErrNotFound);
            	}
        	}
        else if (closedLink == KTagUplink)
        	{
            if (iDTMFTonePlayerUp && (iUpLinkStatus == EReady || iUpLinkStatus == EStreaming))
            	{
                iDTMFTonePlayerUp->Close();
                error = iDTMFTonePlayerUp->RemoveVoIPAudioUplinkStream(*iVoIPUplink);
                iLog->Log(_L("DTMF UPL Closed: error = %d"), error);
            	}
            else
            	{
            	iLog->Log(_L("DTMF UPL-PLR not open"), KErrNotFound);
            	}
        	}
        else
        	{
            iLog->Log(KMsgBadTestParameters);
            error = KErrBadTestParameter;
        	}
    }

    return error;
}


TInt CVoIPAudioServicesTestClass::StopDTMFTonePlayer( CStifItemParser& aItem )
{
	FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::StopDTMFTonePlayer")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::StopDTMFTonePlay"));
    TInt error = KErrNone;

    TPtrC stopLink;
    error = aItem.GetNextString(stopLink);
    if (error == KErrNone)
        {
        if (stopLink == KTagDnlink)
        	{
        	iDTMFTonePlayerDn->Stop();
        	iLog->Log(_L("DTMF DNL Stoped"));
        	}
        else if (stopLink == KTagUplink)
        	{
        	iDTMFTonePlayerUp->Stop();
        	iLog->Log(_L("DTMF UPL Stoped"));
        	}
        else
        	{
        	iLog->Log(KMsgBadTestParameters);
        	error = KErrBadTestParameter;
            }
        }

    return error;
}


TInt CVoIPAudioServicesTestClass::InitRingTonePlayer(CStifItemParser& aItem  )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::InitRingTonePlayer")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::InitRingTonePlayer"));
    TInt error = KErrNone;

    TPtrC file;
    TPtrC fileType;
    error = aItem.GetNextString(fileType);
    if (error == KErrNone && iRingTonePlayer)
    	{
        if (fileType == KTagFile)
        	{
        	iLog->Log(_L("InitRingTonePlayerFromFile"));
        	error = aItem.GetNextString(file);
        	if (error == KErrNone)
        		{
        		if (file == KTagFile1)
        			{
        			TBufC<KMaxFileName> file(KTestFile1);
        			error = iRingTonePlayer->Open(*this, file.Des());
        			AddExpectedEvent(EOpenComplete, KMediumTimeout);
        			}
        		else if (file == KTagFile2)
        			{
        			TBufC<KMaxFileName> file(KTestFile2);
        			error = iRingTonePlayer->Open(*this, file.Des());
        			AddExpectedEvent(EOpenComplete, KMediumTimeout);
        			}
        		}
        	else
        		{
        		iLog->Log(KMsgBadTestParameters);
        		error = KErrBadTestParameter;
        		}
        	}

        else if (fileType == KTagRFile)
        	{
        	iLog->Log(_L("InitRingTonePlayerFromRFile"));
        	error = iFs.Connect();

        	if (error == KErrNone)
	        	{
	            error = iFs.ShareProtected();
	        	}

	        if (error == KErrNone)
	        	{
	        	error = aItem.GetNextString(file);
	        	if (error == KErrNone)
	        		{
	        		if (file == KTagFile1)
	        			{
	        			error = iRTFile.Open(iFs, KTestFile1, EFileRead);
	        			}
	        		else if (file == KTagFile2)
	        			{
	        			error = iRTFile.Open(iFs, KTestFile2, EFileRead);
	        			}
	        		}
	        	else
	        		{
	        		iLog->Log(KMsgBadTestParameters);
	        		error = KErrBadTestParameter;
	        		}

	        	if (error == KErrNone)
	        		{
	                error = iRingTonePlayer->Open(*this, iRTFile);
	                AddExpectedEvent(EOpenComplete, KMediumTimeout);
	        		}
	        	}
        	}

	    else if (fileType == KTagProfile)
	    	{
	    	iLog->Log(_L("InitRingTonePlayerFromProfile"));
	    	error = iRingTonePlayer->Open(*this);
	    	AddExpectedEvent(EOpenComplete, KMediumTimeout);
	    	}

	    else if (fileType == KTagDesL)
	    	{
	    	iLog->Log(_L("InitRingTonePlayerFromDesL"));
	        TBufC8<KMimeStringSize> mimeType;
	        TPtr8 pMime = mimeType.Des();
	        pMime.Copy(KMimetypeWAV);
	        error = iFs.Connect();

	        if (error == KErrNone)
	        	{
	            error = iRTFile.Open(iFs, KTestFile3, EFileRead);
	            if (error == KErrNone)
	                {
	                TInt size;
	                iRTFile.Size(size);
	                HBufC8* tone = HBufC8::NewLC(size);
	                TPtr8 pTone = tone->Des();
	                iRTFile.Read(pTone);
	                iRTFile.Close();
	                error = iRingTonePlayer->Open(*this, pTone, pMime);
	                CleanupStack::PopAndDestroy(tone);
	                AddExpectedEvent(EOpenComplete, KMediumTimeout);
	                }
	            else
	            	{
	            	iLog->Log(_L("RT Open from DesL failed"));
	            	}
	            iFs.Close();
	        	}

	        else
	        	{
	        	iLog->Log(_L("RT Connect failed"));
	        	}

	    	}

	    else
	        {
	        iLog->Log(KMsgBadTestParameters);
	        error = KErrBadTestParameter;
	        }
    	}

    else
    	{
        if (!iRingTonePlayer)
        	{
            iLog->Log(_L("RT PLR Open failed"));
        	}
    	}

    return error;
}


TInt CVoIPAudioServicesTestClass::CloseRingTonePlayer(CStifItemParser& /*aItem*/  )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::CloseRingTonePlayer")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::CloseRingTonePlayer"));
    TInt error = KErrNone;

  if (iRingTonePlayer)
	  {
      iRTFile.Close();
      iFs.Close();
      iRingTonePlayer->Close();

      iLog->Log(_L("RT Player Closed"));
	  }
  else
	  {
      iLog->Log(_L("RT PLR Not Ready: error = %d"), KErrNotFound);
	  }

  return error;
}



TInt CVoIPAudioServicesTestClass::PlayRingTone( CStifItemParser& /*aItem*/ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::PlayRingTone")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::PlayRingTone"));
    TInt error = KErrNone;

    if (iRingTonePlayer)
    	{
        iRingTonePlayer->Play();
        AddExpectedEvent(EPlaybackComplete, KLongTimeout);  // Played ringing tone file has to be shorter than KLongTimeout
    	}
  else
	  {
      iLog->Log(_L("RT PLR Not Ready: error = %d"), KErrNotFound);
      error = KErrNotFound;
	  }

    return error;
}



TInt CVoIPAudioServicesTestClass::PauseRingTone( CStifItemParser& /*aItem*/ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::PauseRingTone")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::PauseRingTone"));
    TInt error = KErrNone;

    if (iRingTonePlayer)
    	{
        iRingTonePlayer->Pause();
    	}
    else
	  {
      iLog->Log(_L("RT PLR Not Ready: error = %d"), KErrNotFound);
      error = KErrNotFound;
	  }

    return error;
}


TInt CVoIPAudioServicesTestClass::ResumeRingTone( CStifItemParser& /*aItem*/ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::ResumeRingTone")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::ResumeRingTone"));
    TInt error = KErrNone;

    if (iRingTonePlayer)
    	{
        iRingTonePlayer->Resume();
    	}
    else
    	{
    	iLog->Log(_L("RT PLR Not Ready: error = %d"), KErrNotFound);
    	error = KErrNotFound;
    	}

    return error;
}



TInt CVoIPAudioServicesTestClass::GetSupportedBitrates( CStifItemParser& /*aItem*/ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::GetSupportedBitrates")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::GetSupportedBitrates"));
    TInt error = KErrNone;

    iBitratesArray.Reset();
    GetSupportedBitrates(iBitratesArray);

    TInt numOfItems = iBitratesArray.Count();

    for (TInt i = 0; i < numOfItems; i++)
        {
        iLog->Log(_L("BR %d"), iBitratesArray[i]);
        }

    return error;
}



// ----------------------------------------------------------------------------
// CVoIPTestEngine::GetSupportedBitrates
//
// ----------------------------------------------------------------------------
//
void CVoIPAudioServicesTestClass::GetSupportedBitrates(RArray<TUint>& aBrArr)
{
    switch (iUpLinkCodec)
        {
        case EG711:
            iG711EncFormatIntfc->GetSupportedBitRates(aBrArr);
            break;
        case EG729:
            iG729EncFormatIntfc->GetSupportedBitRates(aBrArr);
            break;
        case EILBC:
            iILBCEncFormatIntfc->GetSupportedBitRates(aBrArr);
            break;
        case EAMR_NB:
            iAMRNBEncFormatIntfc->GetSupportedBitRates(aBrArr);
            break;
        case EPCM16:
            iPCM16EncFormatIntfc->GetSupportedBitRates(aBrArr);
            break;
        default:
            break;
        }
}


TInt CVoIPAudioServicesTestClass::SetBitrate( CStifItemParser& aItem )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::SetBitrate")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::SetBitrate"));
    TInt error = KErrNone;

    TPtrC bitRate;
    error = aItem.GetNextString(bitRate);
    if (error == KErrNone)
    	{
        if (bitRate == KTagMin)
        	{
        	if (iBitratesArray.Count() > 0)
        		{
	            iBitrate = iBitratesArray[0];
	            SetBitrate(iBitrate);
	            iLog->Log(_L("BR set %d"), iBitrate);
        		}
        	}
        else if (bitRate == KTagMax)
        	{
        	TInt numOfItems = iBitratesArray.Count();
            if (numOfItems > 0)
            	{
	            iBitrate = iBitratesArray[numOfItems-1];
	            SetBitrate(iBitrate);
	            iLog->Log(_L("BR set %d"), iBitrate);
            	}
        	}
        else
        	{
        	iLog->Log(KMsgBadTestParameters);
        	error = KErrBadTestParameter;
        	}
    	}

    return error;
}


// ----------------------------------------------------------------------------
// CVoIPAudioServicesTestClass::SetBitrate
//
// ----------------------------------------------------------------------------
//
void CVoIPAudioServicesTestClass::SetBitrate(TUint aBitrate)
{
    switch (iUpLinkCodec)
        {
        case EG711:
            iG711EncFormatIntfc->SetBitRate(aBitrate);
            break;
        case EG729:
            iG729EncFormatIntfc->SetBitRate(aBitrate);
            break;
        case EILBC:
            iILBCEncFormatIntfc->SetBitRate(aBitrate);
            break;
        case EAMR_NB:
            iAMRNBEncFormatIntfc->SetBitRate(aBitrate);
            break;
        case EPCM16:
            iPCM16EncFormatIntfc->SetBitRate(aBitrate);
            break;
        default:
            break;
        }

    iLog->Log(_L("Set Uplink BR: %d "), iBitrate);
}




TInt CVoIPAudioServicesTestClass::GetBitrate( CStifItemParser& /*aItem*/ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::GetBitrate")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::GetBitrate"));
    TInt error = KErrNone;

    TUint currentBitrate;
    GetBitrate(currentBitrate);
    iLog->Log(_L("Current BR: %d"), currentBitrate);
    if (iBitrate != currentBitrate)
    	{
        iLog->Log(_L("returned bitRate doesn't match set bitRate: Failed"));
        error = KErrUnexpectedValue;
    	}

    return error;
}


// ----------------------------------------------------------------------------
// CVoIPAudioServicesTestClass::GetBitrate
//
// ----------------------------------------------------------------------------
//
void CVoIPAudioServicesTestClass::GetBitrate(TUint& aBitrate)
{
    switch (iUpLinkCodec)
        {
        case EG711:
            iG711EncFormatIntfc->GetBitRate(aBitrate);
            break;
        case EG729:
            iG729EncFormatIntfc->GetBitRate(aBitrate);
            break;
        case EILBC:
            iILBCEncFormatIntfc->GetBitRate(aBitrate);
            break;
        case EAMR_NB:
            iAMRNBEncFormatIntfc->GetBitRate(aBitrate);
            break;
        case EPCM16:
            iPCM16EncFormatIntfc->GetBitRate(aBitrate);
            break;
        default:
            break;
        }

    iLog->Log(_L("Get Uplink BR: %d "), aBitrate);
}



TInt CVoIPAudioServicesTestClass::GetVAD( CStifItemParser& /*aItem*/ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::GetVAD")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::GetVAD"));
    TInt error = KErrNone;

    TBool aVad;

    switch (iUpLinkCodec)
    	{
        case EG711:
            error = iG711EncFormatIntfc->GetVAD(aVad);
            break;
        case EG729:
            error = iG729EncFormatIntfc->GetVAD(aVad);
            break;
        case EILBC:
            error = iILBCEncFormatIntfc->GetVAD(aVad);
            break;
        case EAMR_NB:
            error = iAMRNBEncFormatIntfc->GetVAD(aVad);
            break;
        case EPCM16:
            error = iPCM16EncFormatIntfc->GetVAD(aVad);
            break;
        default:
            break;
    	}

     iLog->Log(_L("get VAD: %d"), aVad);

     if (aVad != iVad)
        {
        iLog->Log(_L("returned VAD doesn't match set VAD: Failed"));
        error = KErrUnexpectedValue;
        }

    return error;
}



TInt CVoIPAudioServicesTestClass::ToggleVAD( CStifItemParser& /*aItem*/ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::ToggleVAD")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::ToggleVAD"));
    TInt error = KErrNone;

    iVad = (iVad)? EFalse : ETrue;

    switch (iUpLinkCodec)
    	{
        case EG711:
            error = iG711EncFormatIntfc->SetVAD(iVad);
            break;
        case EG729:
            error = iG729EncFormatIntfc->SetVAD(iVad);
            break;
        case EILBC:
            error = iILBCEncFormatIntfc->SetVAD(iVad);
            break;
        case EAMR_NB:
            error = iAMRNBEncFormatIntfc->SetVAD(iVad);
            break;
        case EPCM16:
            error = iPCM16EncFormatIntfc->SetVAD(iVad);
            break;
        default:
            break;
    	}

    iLog->Log(_L("VAD set: %d"), iVad);

    return error;
}



TInt CVoIPAudioServicesTestClass::GetFrameMode( CStifItemParser& /*aItem*/ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::GetFrameMode")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::GetFrameMode"));
    TInt error = KErrNone;

    TBool aFrameMode = EFalse;
    switch (iDnLinkCodec)
        {
        case EG711:
            error = iG711DecFormatIntfc->GetFrameMode(aFrameMode);
            iLog->Log(_L("FrameMode: %d"), aFrameMode);
            break;
        case EG729:
            error = iG729DecFormatIntfc->GetFrameMode(aFrameMode);
            iLog->Log(_L("FrameMode: %d"), aFrameMode);
            break;
        case EILBC:
            error = iILBCDecFormatIntfc->GetFrameMode(iFrameMode);
            iLog->Log(_L("FrameMode: %d"), aFrameMode);
            break;
        case EAMR_NB:
            error = iAMRNBDecFormatIntfc->GetFrameMode(iFrameMode);
            iLog->Log(_L("FrameMode: %d"), aFrameMode);
            break;
        case EPCM16:
        default:
            break;
        }

      if (aFrameMode != iFrameMode)
    	  {
    	  iLog->Log(_L("returned FrameMode doesn't match set FrameMode: Failed"));
    	  error = KErrUnexpectedValue;
    	  }

    return error;
}



TInt CVoIPAudioServicesTestClass::ToggleFrameMode( CStifItemParser& /*aItem*/ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::ToggleFrameMode")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::ToggleFrameMode"));
    TInt error = KErrNone;

    iFrameMode = (iFrameMode)? EFalse : ETrue;

    switch (iDnLinkCodec)
        {
        case EG711:
            error = iG711DecFormatIntfc->SetFrameMode(iFrameMode);
            iLog->Log(_L("FrameMode Set: %d"), iFrameMode);
            break;
        case EG729:
            error = iG729DecFormatIntfc->SetFrameMode(iFrameMode);
            iLog->Log(_L("FrameMode Set: %d"), iFrameMode);
            break;
        case EILBC:
            error = iILBCDecFormatIntfc->SetFrameMode(iFrameMode);
            iLog->Log(_L("FrameMode Set: %d"), iFrameMode);
            break;
        case EAMR_NB:
            error = iAMRNBDecFormatIntfc->SetFrameMode(iFrameMode);
            iLog->Log(_L("FrameMode Set: %d"), iFrameMode);
            break;
        case EPCM16:
        default:
            break;
        }

    return error;
}



TInt CVoIPAudioServicesTestClass::GetFrameModeRqrdForEC( CStifItemParser& /*aItem*/ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::GetFrameModeRqrdForEC")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::GetFrameModeRqrdForEC"));
    TInt error = KErrNone;

    switch (iDnLinkCodec)
        {
        case EG711:
            error = iG711DecFormatIntfc->FrameModeRequiredForErrorConcealment(
                                 iFrameModeReqEC);
            iLog->Log(_L("FrmModeRqdForEC: %d"), iFrameModeReqEC);
            break;
        case EG729:
            error = iG729DecFormatIntfc->FrameModeRequiredForErrorConcealment(
                                 iFrameModeReqEC);
            iLog->Log(_L("FrmModeRqdForEC: %d"), iFrameModeReqEC);
            break;
        case EILBC:
            error = iILBCDecFormatIntfc->FrameModeRequiredForErrorConcealment(
                                 iFrameModeReqEC);
            iLog->Log(_L("FrmModeRqdForEC: %d"), iFrameModeReqEC);
            break;
        case EAMR_NB:
            error = iAMRNBDecFormatIntfc->FrameModeRequiredForErrorConcealment(
                                  iFrameModeReqEC);
            iLog->Log(_L("FrmModeRqdForEC: %d"), iFrameModeReqEC);
            break;
        case EPCM16:
        default:
            break;
        }

    return error;
}



TInt CVoIPAudioServicesTestClass::ConcealErrForNextBuf( CStifItemParser& /*aItem*/ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::ConcealErrForNextBuf")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::ConcealErrForNextBuf"));
    TInt error = KErrNone;

    switch (iDnLinkCodec)
        {
        case EG711:
            error = iG711DecFormatIntfc->ConcealErrorForNextBuffer();
            iLog->Log(_L("Conceal Error for next buf"));
            break;
        case EG729:
            error = iG729DecFormatIntfc->ConcealErrorForNextBuffer();
            iLog->Log(_L("Conceal Error for next buf"));
            break;
        case EILBC:
            error = iILBCDecFormatIntfc->ConcealErrorForNextBuffer();
            iLog->Log(_L("Conceal Error for next buf"));
            break;
        case EAMR_NB:
            error = iAMRNBDecFormatIntfc->ConcealErrorForNextBuffer();
            iLog->Log(_L("Conceal Error for next buf"));
            break;
        case EPCM16:
        default:
            break;
        }

    return error;
}



TInt CVoIPAudioServicesTestClass::GetMode( CStifItemParser& aItem )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::GetMode")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::GetMode"));
    TInt error = KErrNone;

    TPtrC linkType;
    error = aItem.GetNextString(linkType);
    if (error == KErrNone)
    	{
        if ((linkType == KTagUplink)&&(iUpLinkCodec == EG711))
        	{
        	CVoIPFormatIntfc::TG711CodecMode aMode;
        	iG711EncFormatIntfc->GetMode(aMode);
        	if (aMode == CVoIPFormatIntfc::EG711ALaw)
        		{
        		iLog->Log(_L("UPL Mode: aLaw"));
        		}
            else if (aMode == CVoIPFormatIntfc::EG711uLaw)
            	{
            	iLog->Log(_L("UPL Mode: uLaw"));
            	}
            else
            	{
                iLog->Log(_L("UPL Mode: unknown"));
            	}
        	}
        else if ((linkType == KTagUplink) && (iUpLinkCodec == EILBC) )
        	{
        	CVoIPFormatIntfc::TILBCCodecMode aMode;
            iILBCEncFormatIntfc->GetMode(aMode);
            if (aMode == CVoIPFormatIntfc::EiLBC20mSecFrame)
            	{
                iLog->Log(_L("UPL Mode: 20ms frame"));
            	}
            else if (aMode == CVoIPFormatIntfc::EiLBC30mSecFrame)
            	{
                iLog->Log(_L("UPL Mode: 30ms frame"));
            	}
            else
            	{
                iLog->Log(_L("UPL Mode: unknown"));
            	}
        	}
        else if ((linkType == KTagDnlink)&&(iDnLinkCodec == EG711))
        	{
        	CVoIPFormatIntfc::TG711CodecMode aMode;
        	iG711DecFormatIntfc->GetMode(aMode);
        	if (aMode == CVoIPFormatIntfc::EG711ALaw)
	            {
	            iLog->Log(_L("DNL Mode: aLaw"));
	            }
            else if (aMode == CVoIPFormatIntfc::EG711uLaw)
	            {
	            iLog->Log(_L("DNL Mode: uLaw"));
	            }
            else
            	{
                iLog->Log(_L("DNL Mode: unknown"));
            	}
        	}
        else if ((linkType == KTagDnlink) && (iDnLinkCodec == EILBC) )
        	{
        	CVoIPFormatIntfc::TILBCCodecMode aMode;
            iILBCDecFormatIntfc->GetMode(aMode);
            if (aMode == CVoIPFormatIntfc::EiLBC20mSecFrame)
            	{
                iLog->Log(_L("DNL Mode: 20ms frame"));
            	}
            else if (aMode == CVoIPFormatIntfc::EiLBC30mSecFrame)
            	{
                iLog->Log(_L("DNL Mode: 30ms frame"));
            	}
            else
            	{
                iLog->Log(_L("DNL Mode: unknown"));
            	}
        	}
        else
        	{
        	iLog->Log(KMsgBadTestParameters);
            error = KErrBadTestParameter;
        	}

    	}

    return error;
}




TInt CVoIPAudioServicesTestClass::SetMode( CStifItemParser& aItem )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::SetMode")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::SetMode"));
    TInt error = KErrNone;

    TPtrC linkType;
    error = aItem.GetNextString(linkType);
    if (error != KErrNone)
    	{
        return error;
    	}

    TPtrC mode;
    error = aItem.GetNextString(mode);
    if (error == KErrNone)
    	{
        if ((linkType == KTagUplink) && (iUpLinkCodec == EG711) && (mode == KTagALaw))
        	{
	        iG711EncFormatIntfc->SetMode(CVoIPFormatIntfc::EG711ALaw);
	        iLog->Log(_L("UPL Mode Set: aLaw"));
        	}
        else if ((linkType == KTagUplink) && (iUpLinkCodec == EG711) && (mode == KTagULaw))
        	{
        	iG711EncFormatIntfc->SetMode(CVoIPFormatIntfc::EG711uLaw);
        	iLog->Log(_L("UPL Mode Set: uLaw"));
        	}
        else if ((linkType == KTagUplink) && (iUpLinkCodec == EILBC)&& (mode == KTag20ms ))
        	{
        	iILBCEncFormatIntfc->SetMode(CVoIPFormatIntfc::EiLBC20mSecFrame);
        	iLog->Log(_L("UPL Mode Set: 20ms frame"));
        	}
        else if ((linkType == KTagUplink) && (iUpLinkCodec == EILBC)&& (mode == KTag30ms ))
        	{
        	iILBCEncFormatIntfc->SetMode(CVoIPFormatIntfc::EiLBC30mSecFrame);
        	iLog->Log(_L("UPL Mode Set: 30ms frame"));
        	}
        else if ((linkType == KTagDnlink) && (iDnLinkCodec == EG711) && (mode == KTagALaw) )
        	{
        	iG711DecFormatIntfc->SetMode(CVoIPFormatIntfc::EG711ALaw);
        	iLog->Log(_L("DNL Mode Set: aLaw"));
        	}
        else if ((linkType == KTagDnlink) && (iDnLinkCodec == EG711) && (mode == KTagULaw) )
        	{
        	iG711DecFormatIntfc->SetMode(CVoIPFormatIntfc::EG711uLaw);
	        iLog->Log(_L("DNL Mode Set: uLaw"));
        	}
        else if ((linkType == KTagDnlink) && (iDnLinkCodec == EILBC) && (mode == KTag20ms) )
        	{
        	iILBCDecFormatIntfc->SetMode(CVoIPFormatIntfc::EiLBC20mSecFrame);
        	iLog->Log(_L("DNL Mode Set: 20ms frame"));
        	}
        else if ((linkType == KTagDnlink) && (iDnLinkCodec == EILBC) && (mode == KTag30ms) )
        	{
        	iILBCDecFormatIntfc->SetMode(CVoIPFormatIntfc::EiLBC30mSecFrame);
        	iLog->Log(_L("DNL Mode Set: 30ms frame"));
        	}
        else
        	{
        	iLog->Log(KMsgBadTestParameters);
            error = KErrBadTestParameter;
        	}
    	}

    return error;
}



TInt CVoIPAudioServicesTestClass::GetCNG( CStifItemParser& /*aItem*/ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::GetCNG")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::GetCNG"));
    TInt error = KErrNone;

    TBool aCng = EFalse;
    switch (iDnLinkCodec)
        {
        case EG711:
            error = iG711DecFormatIntfc->GetCNG(aCng);
            iLog->Log(_L("CNG: %d"), aCng);
            break;
        case EILBC:
            error = iILBCDecFormatIntfc->GetCNG(aCng);
            iLog->Log(_L("CNG: %d"), aCng);
            break;
        default:
            break;
        }

      if (aCng != iCng)
    	  {
    	  iLog->Log(_L("returned Cng doesn't match set Cng: Failed"));
    	  error = KErrUnexpectedValue;
    	  }

    return error;
}



TInt CVoIPAudioServicesTestClass::ToggleCNG( CStifItemParser& /*aItem*/ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::ToggleCNG")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::ToggleCNG"));
    TInt error = KErrNone;

    iCng = (iCng)? EFalse : ETrue;

    switch (iDnLinkCodec)
        {
        case EG711:
            iG711DecFormatIntfc->SetCNG(iCng);
            iLog->Log(_L("CNG set: %d"), iCng);
            break;
        case EILBC:
            iILBCDecFormatIntfc->SetCNG(iCng);
            iLog->Log(_L("CNG set: %d"), iCng);
            break;
        default:
            break;
        }

    return error;
}



TInt CVoIPAudioServicesTestClass::GetPLC( CStifItemParser& /*aItem*/ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::GetPLC")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::GetPLC"));
    TInt error = KErrNone;

    TBool aPlc = EFalse;
    if (iDnLinkCodec == EG711)
    	{
        iG711DecFormatIntfc->GetPLC(aPlc);
        iLog->Log(_L("PLC: %d"), aPlc);
    	}

    return error;
}



TInt CVoIPAudioServicesTestClass::TogglePLC( CStifItemParser& /*aItem*/ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::TogglePLC")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::TogglePLC"));
    TInt error = KErrNone;

    iPlc = (iPlc)? EFalse : ETrue;

    if (iDnLinkCodec == EG711)
        {
        error = iG711DecFormatIntfc->SetPLC(ETrue);
        iLog->Log(_L("PLC set: %d"), iPlc);
        }

    return error;
}



TInt CVoIPAudioServicesTestClass::BadLsfNextBuffer( CStifItemParser& /*aItem*/ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::BadLsfNextBuffer")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::BadLsfNextBuffer"));
    TInt error = KErrNone;

    if (iDnLinkCodec == EG729)
        {
        error = iG729DecFormatIntfc->BadLsfNextBuffer();
        iLog->Log(_L("BadLsf for next buf"));
        }

    return error;
}



TInt CVoIPAudioServicesTestClass::StopRingTone( CStifItemParser& /*aItem*/ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::StopRingTone")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::StopRingTone"));
    TInt error = KErrNone;

    if (iRingTonePlayer)
    	{
        iRingTonePlayer->Stop();
        RemoveExpectedEvent(EPlaybackComplete);
    	}
    else
	  {
      iLog->Log(_L("RT PLR Not Ready: error = %d"), KErrNotFound);
      error = KErrNotFound;
	  }

    return error;
}


TInt CVoIPAudioServicesTestClass::GetBufferType( CStifItemParser& /*aItem*/ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::GetBufferType")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::GetBufferType"));
    TInt error = KErrNone;

    CVoIPDataBuffer::TVoIPBufferType BufType;
    iPlayBuf->GetBufferType(BufType);

    if (BufType != CVoIPDataBuffer::EStandard && BufType != CVoIPDataBuffer::EJitterBuffer)
    	{
    	error = KErrUnknown;
    	FTRACE(FPrint(_L("Buffer Type Unknown")));
        iLog->Log(_L("Buffer Type Unknown"));
    	}

    return error;
}


#ifdef __JITTER_BUFFER_TEST__
// ----------------------------------------------------------------------------
// CVoIPAudioServicesTestClass::ConfigureJB
//
// ----------------------------------------------------------------------------
//
TInt CVoIPAudioServicesTestClass::ConfigureJB()
{
    TInt err = KErrNotSupported;

    if (iJBIntfc && iDnLinkCodec != EPCM16)
        {
        TVoIPJBConfig jBConfig;

        // Select number of JB slots. Current VAS server's heap size limit
        // is 16k. This means, JB buffer with 100 slots, each 162-byte long
        // (G.711 case), will exceed allowed heap size and result in
        // KErrNoMemory. Too large JB will result in playback delay.
        jBConfig.iJBBufferLength = 25;

        // JB packet threashold; must be < iJBBufferLength
        jBConfig.iJBThreshold = 15;

        // Jitter delay - 40 ms packet latency
        jBConfig.iJitterLatency = 40;

        // Sample interval at which frames have been encoded at the
        // end point. This is only relevant for G.711, as we need to know
        // ahead of time whether we are dealing with 10 or 20 ms frames.
        jBConfig.iSampleInterval = 20;

        // For loopback sequence generation - simulation only
        iSeqNumIncrement = KDefaultSampleRateInkHz * jBConfig.iSampleInterval;

        // Relevant only to AMR (jb is reset upon talk-burst timeout)
        jBConfig.iJBInactivityTimeOut = 30;

        // Configure reset buffer tone playback params
//      jBConfig.iJitterBufPlayToneDuration = ?;
//      jBConfig.iJitterBufPlayToneFrequency = ?;
//      jBConfig.iJitterBufPlayToneTimeout = ?;

        err = iJBIntfc->ConfigureJitterBuffer(jBConfig);
        }

//    	DisplayText(_L("JB Initialized"));

    return err;
}

// ----------------------------------------------------------------------------
// CVoIPAudioServicesTestClass::ResetJB
//
// ----------------------------------------------------------------------------
//
TInt CVoIPAudioServicesTestClass::ResetJB()
{
    TInt err = KErrNotSupported;

    if (iJBIntfc && iDnLinkCodec != EPCM16)
        {
        err = iJBIntfc->ResetJitterBuffer(ETrue);
        }

    return err;
}

// ----------------------------------------------------------------------------
// CVoIPAudioServicesTestClass::DelayDown
//
// ----------------------------------------------------------------------------
//
void CVoIPAudioServicesTestClass::DelayDown()
{
    if (iJBIntfc && iDnLinkCodec != EPCM16)
        {
        iJBIntfc->DelayDown();
        }
}

// ----------------------------------------------------------------------------
// CVoIPAudioServicesTestClass::DelayUp
//
// ----------------------------------------------------------------------------
//
void CVoIPAudioServicesTestClass::DelayUp()
{
    if (iJBIntfc && iDnLinkCodec != EPCM16)
        {
        iJBIntfc->DelayUp();
        }
}
#endif //__JITTER_BUFFER_TEST__


// ----------------------------------------------------------------------------
// CVoIPTestEngine::DoLoopback
//
// ----------------------------------------------------------------------------
//
/*void CVoIPAudioServicesTestClass::DoLoopback()
    {
   	iLog->Log(_L("CVoIPAudioServicesTestClass::DoLoopback"));
 		if (iPlayBufReady && iRecBufReady)
        {
        iPlayBufPtr.Copy(iRecBufPtr);


#ifdef __JITTER_BUFFER_TEST__
        if (iDnLinkCodec != EPCM16)
            {
            // We need to generate sequence number for each frame.
            // Real client should get sequence number from the RTP header.
            // Max possible sequence number of RTP packet with standard
            // RTP header is 0xFFFF (16-bit value)
            if (iSeqNum < (KMaxSeqNumber - iSeqNumIncrement))
            	{
                iSeqNum += iSeqNumIncrement;
            	}
            else
            	{
                iSeqNum = iSeqNumIncrement; //wrap-around
            	}

            iPlayJBuf->SetPayloadPtr(iPlayBufPtr);
            iPlayJBuf->SetBufferSequence(iSeqNum);
            iVoIPDnlink->BufferFilled(iPlayJBuf);
            iPlayBufReady = ETrue; // no FillBuffer events in JB mode
            iRecBufReady = EFalse; // consumed, ready for EmptyBuffer
            }
        else
        	{
        	; // PCM not supported in Jitter Buffer mode
        	}
#else  //__JITTER_BUFFER_TEST__

        iPlayBuf->SetPayloadPtr(iPlayBufPtr);
        iVoIPDnlink->BufferFilled(iPlayBuf);
        iLog->Log(_L("CVoIPAudioServicesTestClass::played"));
        iPlayBufReady = EFalse;
        iRecBufReady = EFalse;
#endif //__JITTER_BUFFER_TEST__
        }
    iLog->Log(_L("CVoIPAudioServicesTestClass::DoLoopback END"));
    }
*/



// ----------------------------------------------------------------------------
// CVoIPTestEngine::DoLoopback
//
// ----------------------------------------------------------------------------
//
void CVoIPAudioServicesTestClass::DoLoopback()
{
    
  
	iLog->Log(_L("CVoIPAudioServicesTestClass::DoLoopback"));
	iLog->Log(_L("loopbackCount = %d"), loopbackCount);
  if ((0<loopbackCount) && (loopbackCount<= 20))
  {
    if (iPlayBufReady && iRecBufReady)
        {
        iPlayBufPtr.Copy(iRecBufPtr);


#ifdef __JITTER_BUFFER_TEST__
        if (iDnLinkCodec != EPCM16)
            {
            // We need to generate sequence number for each frame.
            // Real client should get sequence number from the RTP header.
            // Max possible sequence number of RTP packet with standard
            // RTP header is 0xFFFF (16-bit value)
            if (iSeqNum < (KMaxSeqNumber - iSeqNumIncrement))
            	{
                iSeqNum += iSeqNumIncrement;
            	}
            else
            	{
                iSeqNum = iSeqNumIncrement; //wrap-around
            	}

            iPlayJBuf->SetPayloadPtr(iPlayBufPtr);
            iPlayJBuf->SetBufferSequence(iSeqNum);
            iLog->Log(_L("CVoIPAudioServicesTestClass::DoLoopback, loopbackCount == %d"), loopbackCount);
            //sue add
            if (loopbackCount == 10)
            {
            	ResetJB();
            }
            if (loopbackCount == 12)
            {
            	DelayDown();
            }
            if (loopbackCount == 14)
            {
            	DelayUp();
            }
            // end sue
            iRecBuf->SetPayloadPtr(iRecBufPtr); //sue add
            iVoIPDnlink->BufferFilled(iPlayJBuf);
            iVoIPUplink->BufferEmptied(iRecBuf); 
            iPlayBufReady = ETrue; // no FillBuffer events in JB mode
            iRecBufReady = EFalse; // consumed, ready for EmptyBuffer
            loopbackCount++;	
            }
        else
        	{
        	; // PCM not supported in Jitter Buffer mode
        	}
#else  //__JITTER_BUFFER_TEST__

        iPlayBuf->SetPayloadPtr(iPlayBufPtr);
        iRecBuf->SetPayloadPtr(iRecBufPtr); 
        iVoIPDnlink->BufferFilled(iPlayBuf);
        iVoIPUplink->BufferEmptied(iRecBuf); 
        iPlayBufReady = EFalse;
        iRecBufReady = EFalse;
        loopbackCount++;	
        
        

#endif //__JITTER_BUFFER_TEST__
        }
	}
  else
  {
    loopbackCount = 0;
    iLog->Log(_L("CVoIPAudioServicesTestClass::DoLoopback test end"));
  }
 }



TInt CVoIPAudioServicesTestClass::ConfigEncAudDevice( CStifItemParser& /*aItem*/ )
{
    FTRACE(FPrint(_L("CVoIPAudioServicesTestClass::ConfigEncAudDevice")));
    iLog->Log(_L("CVoIPAudioServicesTestClass::ConfigEncAudDevice"));
    TInt error = KErrNone;
    if (iUpLinkStatus == EReady)
    	{
    // Now we can configure encoder and audio device
    	iVoIPUplink->GetMaxGain(iMaxGain);
    	iVoIPUplink->SetGain(iMaxGain);
    	}

    return error;
}



//  End of File
