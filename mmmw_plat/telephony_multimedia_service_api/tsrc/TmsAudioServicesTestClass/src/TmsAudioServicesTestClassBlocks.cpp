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
 * Description: Telephony Multimedia Service - STIF TEST
 *
 */

// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "TmsAudioServicesTestClass.h"
#include "debug.h"

// -----------------------------------------------------------------------------
// CTmsAudioServicesTestClass::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CTmsAudioServicesTestClass::Delete()
    {
    FTRACE(FPrint(_L("CTmsAudioServicesTestClass::Delete")));

    if (iFactory && iTmsBuffer)
        {
        iFactory->DeleteBuffer(iTmsBuffer);
        }
    if (iFactory && iTmsClientSource)
        {
        iFactory->DeleteSource(iTmsClientSource);
        }
    if (iFactory && iTmsModemSource)
        {
        iFactory->DeleteSource(iTmsModemSource);
        }
    if (iFactory && iTmsMicSource)
        {
        iFactory->DeleteSource(iTmsMicSource);
        }
    if (iFactory && iTmsClientSink)
        {
        iFactory->DeleteSink(iTmsClientSink);
        }
    if (iFactory && iTmsModemSink)
        {
        iFactory->DeleteSink(iTmsModemSink);
        }
    if (iFactory && iTmsSpkrSink)
        {
        iFactory->DeleteSink(iTmsSpkrSink);
        }
    if (iFactory && iTmsFormat)
        {
        iFactory->DeleteFormat(iTmsFormat);
        }
    if (iFactory && iTmsFormatUp)
        {
        iFactory->DeleteFormat(iTmsFormatUp);
        }
    if (iFactory && iTmsFormatDn)
        {
        iFactory->DeleteFormat(iTmsFormatDn);
        }
    if (iFactory && iTmsEffect)
        {
        iFactory->DeleteEffect(iTmsEffect);
        }
    if (iTmsUplink && iTmsCall)
        {
        iTmsCall->DeleteStream(iTmsUplink);
        }
    if (iTmsDnlink && iTmsCall)
        {
        iTmsCall->DeleteStream(iTmsDnlink);
        }
    if (iFactory && iTmsCall)
        {
        iFactory->DeleteCall(iTmsCall);
        }
    if (iFactory && iTmsDnlinkEffect)
        {
        iFactory->DeleteEffect(iTmsDnlinkEffect);
        }
    if (iFactory && iTmsUplinkEffect)
        {
        iFactory->DeleteEffect(iTmsUplinkEffect);
        }
    if (iFactory && iGlobalVol)
        {
        iFactory->DeleteEffect(iGlobalVol);
        }
    if (iFactory && iGlobalGain)
        {
        iFactory->DeleteEffect(iGlobalGain);
        }
    if (iFactory && iTmsGlobalRouting)
        {
        iFactory->DeleteGlobalRouting(iTmsGlobalRouting);
        }

    iExpectedEvents.Close();
    iOcurredEvents.Close();

    iLog->Log(_L("Deleti ng test class..."));
    iLog->Log(_L(""));
    iLog->Log(_L(""));

    // delete iPlayBuf;

    if (iTmsDnlink)
        {
        if (iDnLinkStatus != UNINITIALIZED)
            {
            iTmsDnlink->Deinit();
            }
        delete iTmsDnlink;
        iTmsDnlink = NULL;
        }

    if (iTmsUplink)
        {
        if (iUpLinkStatus != UNINITIALIZED)
            {
            iTmsUplink->Deinit();
            }
        delete iTmsUplink;
        iTmsUplink = NULL;
        }

    delete iFactory;
    }

// -----------------------------------------------------------------------------
// CTmsAudioServicesTestClass::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CTmsAudioServicesTestClass::RunMethodL(CStifItemParser& aItem)
    {
    static TStifFunctionInfo const
    KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.

        ENTRY( "SetTimeout", CTmsAudioServicesTestClass::SetTimeout ),
        ENTRY( "CreateFactory", CTmsAudioServicesTestClass::CreateTmsFactory ),
        ENTRY( "CreateCall", CTmsAudioServicesTestClass::CreateCall ),
        ENTRY( "CreateFormat", CTmsAudioServicesTestClass::CreateFormat ),
        ENTRY( "CreateBuffer", CTmsAudioServicesTestClass::CreateBuffer ),
        ENTRY( "CreateEffect", CTmsAudioServicesTestClass::CreateEffect ),
        ENTRY( "CreateGlobalRouting", CTmsAudioServicesTestClass::CreateGlobalRouting ),
        ENTRY( "CreateSource", CTmsAudioServicesTestClass::CreateSource ),
        ENTRY( "DeleteSource", CTmsAudioServicesTestClass::DeleteSource ),
        ENTRY( "CreateSink", CTmsAudioServicesTestClass::CreateSink ),
        ENTRY( "DeleteSink", CTmsAudioServicesTestClass::DeleteSink ),
        ENTRY( "GetSourceType", CTmsAudioServicesTestClass::GetSourceType ),
        ENTRY( "GetSinkType", CTmsAudioServicesTestClass::GetSinkType ),
        ENTRY( "GetEffectType", CTmsAudioServicesTestClass::GetEffectType ),
        ENTRY( "CreateDownlinkStream", CTmsAudioServicesTestClass::CreateDownlinkStream ),
        ENTRY( "CreateUplinkStream", CTmsAudioServicesTestClass::CreateUplinkStream ),
        ENTRY( "GetSupportedFormats", CTmsAudioServicesTestClass::GetSupportedFormats ),
        ENTRY( "IsCallTypeSupported", CTmsAudioServicesTestClass::IsCallTypeSupported ),
        ENTRY( "SetDownlinkFormat", CTmsAudioServicesTestClass::SetDownlinkFormat ),
        ENTRY( "SetUplinkFormat", CTmsAudioServicesTestClass::SetUplinkFormat ),
        ENTRY( "ReSetDownlinkFormat", CTmsAudioServicesTestClass::ReSetDownlinkFormat ),
        ENTRY( "ReSetUplinkFormat", CTmsAudioServicesTestClass::ReSetUplinkFormat ),
        ENTRY( "OpenDownlink", CTmsAudioServicesTestClass::OpenDownlink ),
        ENTRY( "OpenUplink", CTmsAudioServicesTestClass::OpenUplink ),
        ENTRY( "Gain", CTmsAudioServicesTestClass::Gain ),
        ENTRY( "AddGlobleGainEffectToStream", CTmsAudioServicesTestClass::AddGlobleGainEffectToStream),
        ENTRY( "RemoveGlobleGainEffectToStream", CTmsAudioServicesTestClass::RemoveGlobleGainEffectToStream),
        ENTRY( "Volume", CTmsAudioServicesTestClass::Volume ),
        ENTRY( "AddGlobleVolumeEffectToStream", CTmsAudioServicesTestClass::AddGlobleVolumeEffectToStream),
        ENTRY( "RemoveGlobleVolumeEffectToStream", CTmsAudioServicesTestClass::RemoveGlobleVolumeEffectToStream),
        ENTRY( "CreateGlobleGainEffect", CTmsAudioServicesTestClass::CreateGlobleGainEffect),
        ENTRY( "Close", CTmsAudioServicesTestClass::Close ),
        ENTRY( "Start", CTmsAudioServicesTestClass::Start ),
        ENTRY( "Pause", CTmsAudioServicesTestClass::Pause ),
        ENTRY( "Stop", CTmsAudioServicesTestClass::Stop ),
        ENTRY( "GetBitRateList", CTmsAudioServicesTestClass::GetBitRateList ),
        ENTRY( "SetBitrates", CTmsAudioServicesTestClass::SetBitrates ),
        ENTRY( "GetBitrates", CTmsAudioServicesTestClass::GetBitrates ),
        ENTRY( "GetDownlinkVersion", CTmsAudioServicesTestClass::GetDownlinkVersion ),
        ENTRY( "GetUplinkVersion", CTmsAudioServicesTestClass::GetUplinkVersion ),
        ENTRY( "GetType", CTmsAudioServicesTestClass::GetType ),
        ENTRY( "GetVAD", CTmsAudioServicesTestClass::GetVAD ),
        ENTRY( "ToggleVAD", CTmsAudioServicesTestClass::ToggleVAD ),
        ENTRY( "GetMode", CTmsAudioServicesTestClass::GetMode ),
        ENTRY( "SetMode", CTmsAudioServicesTestClass::SetMode ),
        ENTRY( "GetCNG", CTmsAudioServicesTestClass::GetCNG ),
        ENTRY( "ToggleCNG", CTmsAudioServicesTestClass::ToggleCNG ),
        ENTRY( "GetPLC", CTmsAudioServicesTestClass::GetPLC ),
        ENTRY( "TogglePLC", CTmsAudioServicesTestClass::TogglePLC ),
        ENTRY( "GetBufferType", CTmsAudioServicesTestClass::GetBufferType ),
        ENTRY( "AddClientSrcToDnlStream", CTmsAudioServicesTestClass::AddClientSrcToDnlStream ),
        ENTRY( "AddMicSrcToUplStream", CTmsAudioServicesTestClass::AddMicSrcToUplStream ),
        ENTRY( "AddModemSrcToDnlStream", CTmsAudioServicesTestClass::AddModemSrcToDnlStream ),
        ENTRY( "RemoveClientSrcFromDnlStream", CTmsAudioServicesTestClass::RemoveClientSrcFromDnlStream ),
        ENTRY( "RemoveMicSrcFromUplStream", CTmsAudioServicesTestClass::RemoveMicSrcFromUplStream ),
        ENTRY( "RemoveModemSrcFromDnlStream", CTmsAudioServicesTestClass::RemoveModemSrcFromDnlStream ),
        ENTRY( "AddSourceObserver", CTmsAudioServicesTestClass::AddSourceObserver ),
        ENTRY( "RemoveSourceObserver", CTmsAudioServicesTestClass::RemoveSourceObserver ),
        ENTRY( "AddClientSinkToUplStream", CTmsAudioServicesTestClass::AddClientSinkToUplStream ),
        ENTRY( "AddModemSinkToUplStream", CTmsAudioServicesTestClass::AddModemSinkToUplStream ),
        ENTRY( "AddSpkrSinkToDnlStream", CTmsAudioServicesTestClass::AddSpkrSinkToDnlStream ),
        ENTRY( "RemoveClientSinkFromUplStream", CTmsAudioServicesTestClass::RemoveClientSinkFromUplStream ),
        ENTRY( "RemoveModemSinkFromUplStream", CTmsAudioServicesTestClass::RemoveModemSinkFromUplStream ),
        ENTRY( "RemoveSpkrSinkFromDnlStream", CTmsAudioServicesTestClass::RemoveSpkrSinkFromDnlStream ),
        ENTRY( "AddSinkObserver", CTmsAudioServicesTestClass::AddSinkObserver ),
        ENTRY( "RemoveSinkObserver", CTmsAudioServicesTestClass::RemoveSinkObserver ),
        ENTRY( "GetStreamType", CTmsAudioServicesTestClass::GetStreamType ),
        ENTRY( "GetStreamState", CTmsAudioServicesTestClass::GetStreamState ),
        ENTRY( "SetOutput", CTmsAudioServicesTestClass::SetOutput ),
        ENTRY( "GetOutput", CTmsAudioServicesTestClass::GetOutput ),
        ENTRY( "GetPreviousOutput", CTmsAudioServicesTestClass::GetPreviousOutput ),
        ENTRY( "GetAvailableOutputs", CTmsAudioServicesTestClass::GetAvailableOutputs ),
        ENTRY( "AddGlobalRoutingObserver", CTmsAudioServicesTestClass::AddGlobalRoutingObserver ),
        ENTRY( "DeleteGlobalRoutingObserver", CTmsAudioServicesTestClass::DeleteGlobalRoutingObserver ),
        ENTRY( "Example", CTmsAudioServicesTestClass::ExampleL )
        };

    const TInt count = sizeof(KFunctions) / sizeof(TStifFunctionInfo);
    return RunInternalL(KFunctions, count, aItem);
    }

// -----------------------------------------------------------------------------
// CTmsAudioServicesTestClass::EventName
// Return descriptor with the notification description
// -----------------------------------------------------------------------------
TPtrC CTmsAudioServicesTestClass::EventName(TInt aKey)
    {
    static TText* const badKeyword = (TText*) L"BadKeyword";
    static TText* const keywords[] =
        {
        (TText*)L"EOpenDownlinkComplete",
        (TText*)L"EOpenUplinkComplete",
        (TText*)L"EDownlinkClosed",
        (TText*)L"EUplinkClosed",
        (TText*)L"EOpenCompleteDNL",
        (TText*)L"EOpenCompleteUPL",
        (TText*)L"EStreamPaused",
        (TText*)L"EStreamStarted",
        (TText*)L"EOpenComplete",
        (TText*)L"EPlaybackComplete",
        (TText*)L"EEmptyBuffer",
        (TText*)L"EFillBuffer",
        (TText*)L"EOutputChanged",
        (TText*)L"ESetOutputComplete",
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
// CTmsAudioServicesTestClass::AddExpectedEvent
// Add an event to the expected events' list
// -----------------------------------------------------------------------------
void CTmsAudioServicesTestClass::AddExpectedEvent(TTmsExpectedEvent event,
        TInt ms)
    {
    FTRACE(FPrint(_L("CTmsAudioServicesTestClass::AddExpectedEvent")));
    iExpectedEvents.Append(event);
    TPtrC eventName = EventName(event);
    iLog->Log(_L("Adding expected event:(0x%02x)%S Total=%d"), event,
            &eventName, iExpectedEvents.Count());

    if (iTimeoutController && !iTimeoutController->IsActive())
        {
        if (ms > 0)
            {
            iTimeoutController->Start(TTimeIntervalMicroSeconds(ms * 1000));
            }
        else
            {
            iLog->Log(_L("Timeout with default value (1s)"));
            iTimeoutController->Start(TTimeIntervalMicroSeconds(1000000));
            }
        }
    }

// -----------------------------------------------------------------------------
// CTmsAudioServicesTestClass::RemoveExpectedEvent
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
TBool CTmsAudioServicesTestClass::RemoveExpectedEvent(
        TTmsExpectedEvent aEvent)
    {
    FTRACE(FPrint(_L("CTmsAudioServicesTestClass::RemoveExpectedEvent")));
    iLog->Log(_L("CTmsAudioServicesTestClass::RemoveExpectedEvent"));
    TBool match = EFalse;
    for (TUint i = 0; i < iExpectedEvents.Count(); i++)
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
// CTmsAudioServicesTestClass::RemoveAllExpectedEvents
// Remove the indicated event from the expected events' list
// -----------------------------------------------------------------------------
void CTmsAudioServicesTestClass::RemoveAllExpectedEvents()
    {
    FTRACE(FPrint(_L("CTmsAudioServicesTestClass::RemoveAllExpectedEvents")));
    iLog->Log(_L("Removing all expected events"));
    iExpectedEvents.Reset();
    iOcurredEvents.Reset();
    }

// -----------------------------------------------------------------------------
// CTmsAudioServicesTestClass::ProcessEvent
// -----------------------------------------------------------------------------
void CTmsAudioServicesTestClass::ProcessEvent(TTmsExpectedEvent aEvent,
        TInt aError)
    {
    FTRACE(FPrint(_L("CTmsAudioServicesTestClass::ProcessExpectedEvent")));
    iLog->Log(_L("CTmsAudioServicesTestClass:: ProcessExpectedEvent"));
    TPtrC nameEvent = EventName(aEvent);

    if (aError == KErrNone)
        {
        if (RemoveExpectedEvent(aEvent))
            {
            iLog->Log(_L("Expected Event: (0x%02x)%S has ocurred Total=%d"),
                    aEvent, &nameEvent, iExpectedEvents.Count());
            }
        else
            {
            iLog->Log(_L("Event: (0x%02x)%S has ocurred"), aEvent, &nameEvent);
            return;
            }

        if (iExpectedEvents.Count() == 0)
            {
            Signal();
            iTimeoutController->Cancel();
            }

        }
    else
        {
        iLog->Log(_L("[Error] Event: (0x%02x)%S return with error code=%d"),
                aEvent, &nameEvent, aError);
        if (iExpectedEvents.Count() != 0)
            {
            RemoveExpectedEvent(aEvent);
            }
        iTimeoutController->Cancel();
        Signal(KErrCallbackErrorCode);
        }
    }

// -----------------------------------------------------------------------------
// CTmsAudioServicesTestClass::HandleTimeout
// Review if all the expected events have ocurred once the time is over
// -----------------------------------------------------------------------------

void CTmsAudioServicesTestClass::HandleTimeout(TInt aError)
    {
    FTRACE(FPrint(_L("CTmsAudioServicesTestClass::HandleTimeout")));

    if (aError != KErrNone)
        {
        if (iExpectedEvents.Count() == 0)
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
// CTmsAudioServicesTestClass::SetTimeout
// Create a timer and set a timeout
// When the timeout is reached the test case is marked as failed
// It's used rather than the "timeout" keyword in the configuration file
// because in this way the log continues
// -----------------------------------------------------------------------------
TInt CTmsAudioServicesTestClass::SetTimeout(CStifItemParser& aItem)
    {
    FTRACE (FPrint(_L("CTmsAudioServicesTestClass::SetTimeout")));
    TInt timeout = 0;
    TInt error = aItem.GetNextInt(timeout);
    if (iTimeoutController)
        {
        if (timeout > 0)
            {
            iTimeoutController->Start(TTimeIntervalMicroSeconds(timeout
                    * 1000));
            }
        else
            {
            iTimeoutController->Start(TTimeIntervalMicroSeconds(1000000));
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
TInt CTmsAudioServicesTestClass::SetAllowedPanic(CStifItemParser& aItem)
    {
    FTRACE (FPrint(_L("CTmsAudioServicesTestClass::SetAllowedPanic")));
    TInt error = KErrNone;
    TInt panicCode;
    TPtrC panicType;
    if ((KErrNone == aItem.GetNextString(panicType)) &&
            (KErrNone == aItem.GetNextInt(panicCode)))
        {
        iLog->Log(_L("Allowing panic: %S %d"), &panicType, panicCode);
        //     iTestModuleIf.SetExitReason( CTestModuleIf::EPanic, panicCode );
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
// CTmsAudioServicesTestClass::SetExpectedEvents()
// -----------------------------------------------------------------------------
TInt CTmsAudioServicesTestClass::SetExpectedEvents(CStifItemParser& aItem)
    {
    FTRACE (FPrint(_L("CTmsAudioServicesTestClass::SetExpectedEvents")));
    TInt error = KErrNone;
    TInt event = 0;
    while (KErrNone == aItem.GetNextInt(event))
        {
        AddExpectedEvent(static_cast<TTmsExpectedEvent> (event), 0); // Default timeout value
        }
    return error;
    }

// -----------------------------------------------------------------------------
// CTmsAudioServicesTestClass::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTmsAudioServicesTestClass::ExampleL(CStifItemParser& aItem)
    {
    // Print to UI
    _LIT( KTmsAudioServicesTestClass, "TmsAudioServicesTestClass" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf(0, KTmsAudioServicesTestClass, KExample);
    // Print to log file
    iLog->Log(KExample);

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while (aItem.GetNextString(string) == KErrNone)
        {
        TestModuleIf().Printf(i, KTmsAudioServicesTestClass, KParam, i,
                &string);
        i++;
        }
    return KErrNone;
    }

TInt CTmsAudioServicesTestClass::CreateTmsFactory(CStifItemParser& /*aItem */)
    {
    RDebug::Printf("CTmsAudioServicesTestClass::CreateTmsFactory >> ENTER");
    FTRACE(FPrint(_L("CTmsAudioServicesTestClass::CreateFactory")));
    iLog->Log(_L("CTmsAudioServicesTestClass::CreateFactory"));
    TInt error = KErrNone;

    TMSVer* v = NULL;//not defined yet
    error = TMSFactory::CreateFactory(iFactory, *v);
    RDebug::Printf("[TMS STIF] TMSFactory::CreateFactory Return [%d]", error);
    RDebug::Printf("CTmsAudioServicesTestClass::CreateTmsFactory << EXIT");
    return error;
    }

TInt CTmsAudioServicesTestClass::CreateCall(CStifItemParser& aItem)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::CreateCall"));
    TInt error = KErrNone;
    TInt calltype;
    aItem.GetNextInt(calltype);

    if (iFactory && !iTmsCall)
        {
        switch (calltype)
            {
            case TMS_CALL_CS:
            case TMS_CALL_ECS:
            case TMS_CALL_IP:
                {
                error = iFactory->CreateCall(calltype, iTmsCall, 0);
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::CreateCall, error [%d]"),
                        error);
                }
                break;
            default:
                error = KErrNotSupported;
                break;
            }
        }
    RDebug::Printf("[TMS STIF] TMSFactory::CreateCall Return [%d]", error);
    return error;
    }

TInt CTmsAudioServicesTestClass::CreateFormat(CStifItemParser& aItem)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::CreateFormat"));
    TInt error = KErrNone;
    TMSFormatType format;
    TPtrC StreamType;
    TPtrC formatType;

    //  error = aItem.GetNextInt(formattype);
    error = aItem.GetNextString(formatType);

    if (error == KErrNone)
        {
        error = aItem.GetNextString(StreamType);

        if (iFactory && !iTmsFormat)
            {
            iLog ->Log(
                    _L("CTmsAudioServicesTestClass::CreateFormat - input format"));

            if (formatType == KTagG711)
                {
                format = TMS_FORMAT_G711;
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::CreateFormat - input format TMS_FORMAT_G711"));
                }
            else if (formatType == KTagG729)
                {
                format = TMS_FORMAT_G729;
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::CreateFormat - input format TMS_FO RMAT_G729"));
                }
            else if (formatType == KTagILBC)
                {
                format = TMS_FORMAT_ILBC;
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::CreateFormat - input format TMS_FORMAT_ILBC"));
                }
            else if (formatType == KTagAMRNB)
                {
                format = TMS_FORMAT_AMR;
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::CreateFormat - input format TMS_FORMAT_AMR"));
                }
            else if (formatType == KTagPCM16)
                {
                format = TMS_FORMAT_PCM;
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::CreateFormat - input format TMS_FORMAT_PCM"));
                }
            else
                {
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::CreateFormat failed, format not supported"));
                error = KErrNotSupported;
                return error;
                }

            if (StreamType == KTagDnlink)
                {
                iLog ->Log(
                        _L("CTmsAudioServicesTestClass::CreateFormat - iTmsFormatDn"));
                error = iFactory->CreateFormat(format, iTmsFormatDn);
                iDnLinkCodec = format;
                }
            else if (StreamType == KTagUplink)
                {
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::CreateFormat - iTmsFormatUp"));
                error = iFactory->CreateFormat(format, iTmsFormatUp);
                iUpLinkCodec = format;
                }
            else
                {
                error = iFactory->CreateFormat(format, iTmsFormat);
                }
            }
        else
            {
            error = KErrNotSupported;
            }
        }

    return error;
    }

TInt CTmsAudioServicesTestClass::CreateBuffer(CStifItemParser& aItem)
    {
    FTRACE (FPrint(_L("CTmsAudioServicesTestClass::CreateBuffer")));
    iLog->Log(_L("CTmsAudioServicesTestClass::CreateBuffer"));
    TInt error = KErrNone;
    TInt size = 0;
    TInt buffertype;

    aItem.GetNextInt(buffertype);
    aItem.GetNextInt(size);

    if (iFactory && !iTmsBuffer)
        {
        switch (buffertype)
            {
            case TMS_BUFFER_MEMORY:
                {
                error = iFactory->CreateBuffer(buffertype, size, iTmsBuffer);
                }
                break;
            default:
                error = KErrNotSupported;
                break;
            }
        }

    return error;
    }

TInt CTmsAudioServicesTestClass::CreateEffect(CStifItemParser& aItem)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::CreateEffect"));
    TInt error = KErrNone;
    TInt effecttype;

    aItem.GetNextInt(effecttype);

    if (iFactory)
        {
        switch (effecttype)
            {
            case TMS_EFFECT_VOLUME:
                {
                error = CreateVolumeEffect();
                //      AddVolumeEffectToStream();
                }
                break;
            case TMS_EFFECT_GLOBAL_VOL:
                {
                error = CreateVolumeGlobleEffect();
                //       AddGlobleVolumeEffectToStream();
                }
                break;
            case TMS_EFFECT_GAIN:
                {
                error = CreateGainEffect();
                //        AddGainEffectToStream();
                }
                break;
            case TMS_EFFECT_GLOBAL_GAIN:
                {
                error = CreateGlobleGainEffect();
                //        AddGlobleGainEffectToStream();
                }
                break;
            default:
                error = KErrNotSupported;
                break;
            }
        }
    RDebug::Printf("[TMS STIF] CreateEffect Return [%d]", error);
    return error;
    }

TInt CTmsAudioServicesTestClass::CreateGlobalRouting(CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::CreateGlobalRouting"));
    TInt error = KErrNone;
    if (iFactory)
        {
        error = iFactory->CreateGlobalRouting(iTmsGlobalRouting);
        }
    else
        {
        error = KErrNotReady;
        iLog->Log(
                _L("CTmsAudioServicesTestClass::CreateGlobalRouting - Need create TMSFactory first" ));
        }
    return error;

    }

TInt CTmsAudioServicesTestClass::CreateSource(CStifItemParser& aItem)
    {
    iLog ->Log(_L("CTmsAudioServicesTestClass::CreateSource"));
    TInt error = KErrNone;
    TInt sourcetype;

    aItem.GetNextInt(sourcetype);

    if (iFactory)
        {
        switch (sourcetype)
            {
            case TMS_SOURCE_CLIENT:
                {
                error = iFactory->CreateSource((TMSSourceType) sourcetype,
                        iTmsClientSource);
                }
                break;
            case TMS_SOURCE_MODEM:
                {
                error = iFactory->CreateSource((TMSSourceType) sourcetype,
                        iTmsModemSource);
                }
                break;
            case TMS_SOURCE_MIC:
                {
                error = iFactory->CreateSource((TMSSourceType) sourcetype,
                        iTmsMicSource);
                }
                break;
            default:
                error = KErrNotSupported;
                break;
            }
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::CreateSink(CStifItemParser& aItem)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::CreateSink"));
    TInt error = KErrNone;
    TInt sinktype;

    aItem.GetNextInt(sinktype);

    if (iFactory)
        {
        switch (sinktype)
            {
            case TMS_SINK_CLIENT:
                {
                error = iFactory->CreateSink((TMSSinkType) sinktype,
                        iTmsClientSink);
                }
                break;
            case TMS_SINK_MODEM:
                {
                error = iFactory->CreateSink((TMSSinkType) sinktype,
                        iTmsModemSink);
                }
                break;
            case TMS_SINK_SPEAKER:
                {
                error = iFactory->CreateSink((TMSSinkType) sinktype,
                        iTmsSpkrSink);
                }
                break;
            default:
                error = TMS_RESULT_SINK_TYPE_NOT_SUPPORTED;
                break;
            }
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::GetSourceType(CStifItemParser& aItem)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::GetSourceType"));
    TInt error = KErrNone;
    TInt sourcetype;
    TMSSourceType itSourceType;
    aItem.GetNextInt(sourcetype);

    if (iFactory)
        {
        switch (sourcetype)
            {
            case TMS_SOURCE_CLIENT:
                {
                if (iTmsClientSource)
                    {
                    error = iTmsClientSource->GetType(itSourceType);
                    }
                }
                break;
            case TMS_SOURCE_MODEM:
                {
                if (iTmsModemSource)
                    {
                    error = iTmsModemSource->GetType(itSourceType);
                    }
                }
                break;
            case TMS_SOURCE_MIC:
                {
                if (iTmsMicSource)
                    {
                    error = iTmsMicSource->GetType(itSourceType);
                    }
                }
                break;
            default:
                error = KErrNotSupported;
                break;
            }
        }
    if (error != KErrNone)
        {
        iLog->Log(_L("GetType return error [%d]"), error);
        return error;
        }

    if (itSourceType != sourcetype)
        {
        iLog->Log(
                _L("returned souece type is not same as expected, returned sourcetype :[%d]"),
                itSourceType);
        error = KErrUnexpectedValue;
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::GetSinkType(CStifItemParser& aItem)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::GetSinkType"));
    TInt error = KErrNone;
    TInt sinktype;
    TMSSinkType itSinkType;
    aItem.GetNextInt(sinktype);

    if (iFactory)
        {
        switch (sinktype)
            {
            case TMS_SINK_CLIENT:
                {
                if (iTmsClientSink)
                    error = iTmsClientSink->GetType(itSinkType);
                }
                break;
            case TMS_SINK_MODEM:
                {
                if (iTmsModemSink)
                    error = iTmsModemSink->GetType(itSinkType);
                }
                break;
            case TMS_SINK_SPEAKER:
                {
                if (iTmsSpkrSink)
                    error = iTmsSpkrSink->GetType(itSinkType);
                }
                break;
            default:
                error = TMS_RESULT_SINK_TYPE_NOT_SUPPORTED;
                break;
            }

        if (itSinkType != sinktype)
            {
            iLog->Log(
                    _L("retur ned sink type is not sameas expected, returned sinktype :[%d]"),
                    itSinkType);
            error = KErrUnexpectedValue;
            }
        }

    if (error != KErrNone)
        {
        iLog->Log(_L("GetType return error [%d]"), error);
        return error;
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::GetEffectType(CStifItemParser& aItem)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::GetEffectType"));
    TInt error = KErrNone;

    TInt effecttype;
    TMSEffectType itEffectType;
    aItem.GetNextInt(effecttype);

    if (iFactory)
        {
        switch (effecttype)
            {
            case TMS_EFFECT_VOLUME:
                {
                if (iTmsDnlink && iTmsDnlinkEffect)
                    error = iTmsDnlinkEffect->GetType(itEffectType);
                }
                break;
            case TMS_EFFECT_GLOBAL_VOL:
                {
                if (iGlobalVol)
                    error = iGlobalVol->GetType(itEffectType);
                }
                break;
            case TMS_EFFECT_GAIN:
                {
                if (iTmsUplink && iTmsUplinkEffect)
                    error = iTmsUplinkEffect->GetType(itEffectType);
                }
                break;
            case TMS_EFFECT_GLOBAL_GAIN:
                {
                if (iGlobalGain)
                    error = iGlobalGain->GetType(itEffectType);

                }
                break;
            default:
                error = KErrNotSupported;
                break;
            }
        }

    if (error != KErrNone)
        {
        iLog->Log(_L("GetType return error [%d]"), error);
        return error;
        }

    if (itEffectType != effecttype)
        {
        iLog->Log(
                _L("returned effecttype type is not same as expected, returned effecttype :[%d]"),
                itEffectType);
        error = KErrUnexpectedValue;
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::DeleteSource(CStifItemParser& /*aItem */)
    {
    iLog ->Log(_L("CTmsAudioServicesTestClass::DeleteSource"));
    TInt error = KErrNone;

    if (iFactory)
        {
        if (iTmsClientSource)
            {
            error = iFactory->DeleteSource(iTmsClientSource);
            if (error != KErrNone)
                {
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::DeleteClientSource, failed [%d]"),
                        error);
                return error;
                }
            }
        if (iTmsModemSource)
            {
            error = iFactory->DeleteSource(iTmsModemSource);
            if (error != KErrNone)
                {
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::DeleteModemSource, failed [%d]"),
                        error);
                return error;
                }
            }
        if (iTmsMicSource)
            {
            error = iFactory->DeleteSource(iTmsMicSource);
            if (error != KErrNone)
                {
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::DeleteMicSource, failed [%d]"),
                        error);
                return error;
                }
            }
        }

    return error;
    }

TInt CTmsAudioServicesTestClass::DeleteSink(CStifItemParser& /*aItem */)
    {
    iLog ->Log(_L("CTmsAudioServicesTestClass::DeleteSink"));
    TInt error = KErrNone;

    if (iFactory)
        {
        if (iTmsClientSink)
            {
            error = iFactory->DeleteSink(iTmsClientSink);
            if (error != KErrNone)
                {
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::DeleteClientSource, failed [%d]"),
                        error);
                return error;
                }
            }
        if (iTmsModemSink)
            {
            error = iFactory->DeleteSink(iTmsModemSink);
            if (error != KErrNone)
                {
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::DeleteModemSource, failed [%d]"),
                        error);
                return error;
                }
            }
        if (iTmsSpkrSink)
            {
            error = iFactory->DeleteSink(iTmsSpkrSink);
            if (error != KErrNone)
                {
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::DeleteSpkrSource, failed [%d]"),
                        error);
                return error;
                }
            }
        }

    return error;
    }

TInt CTmsAudioServicesTestClass::CreateDownlinkStream(CStifItemParser& /*aItem*/)
    {
    FTRACE(FPrint(_L("CTmsAudioServicesTestClass::CreateDownlinkStream")));
    iLog->Log(_L("CTmsAudioServicesTestClass::CreateDownlinkStream"));
    TInt error = KErrNone;

    if (iTmsCall)
        {
        error = iTmsCall->CreateStream(TMS_STREAM_DOWNLINK, iTmsDnlink);
        }

    iLog->Log(
            _L("CTmsAudioServicesTestClass::CreateDownlinkStream Error [%d]"),
            error);
    return error;
    }

TInt CTmsAudioServicesTestClass::GetDownlinkVersion(CStifItemParser& /*aItem */)
    {
    FTRACE(FPrint(_L("CTmsAudioServicesTestClass::GetDownlinkVersion")));
    iLog->Log(_L("CTmsAudioServicesTestClass::GetDownlinkVersion"));
    TInt error = KErrNone;
    TVersion ver(0, 0, 0);
    //  error = iTmsDnlink->GetVersion(ver);
    return error;
    }

TInt CTmsAudioServicesTestClass::CreateUplinkStream(CStifItemParser& /*aItem */)
    {
    FTRACE(FPrint(_L("CTmsAudioServicesTestClass::CreateUplinkStream")));
    iLog->Log(_L("CTmsAudioServicesTestClass::CreateUplinkStream"));
    TInt error = KErrNone;
    if (iTmsCall)
        {
        error = iTmsCall->CreateStream(TMS_STREAM_UPLINK, iTmsUplink);
        }
    RDebug::Printf("[TMS STIF] CreateUplinkStream Return [%d]", error);
    return error;
    }

TInt CTmsAudioServicesTestClass::GetUplinkVersion(CStifItemParser& /*aItem */)
    {
    FTRACE(FPrint(_L("CTmsAudioServicesTe stClass::GetUplinkVersion")));
    iLog->Log(_L("CTmsAudioServicesTestClass::GetUplinkVersion"));
    TInt error = KErrNone;
    TVersion ver(0, 0, 0);
    //error = iTmsUplink->GetVersion(ver);
    return error;
    }

TInt CTmsAudioServicesTestClass::GetSupportedFormats(CStifItemParser& aItem)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::GetSupportedFormats"));
    TInt error = KErrNone;
    TPtrC StreamType;
    error = aItem.GetNextString(StreamType);

    if (iFactory)
        {
        if (StreamType == KTagUplink)
            {
            error = iFactory->GetSupportedFormats(TMS_STREAM_UPLINK, iCodec);
            }
        else
            {
            error
                    = iFactory->GetSupportedFormats(TMS_STREAM_DOWNLINK,
                            iCodec);
            }

        if (error != KErrNone)
            {
            iLog->Log(_L("DNL Codecs retrieve error, %d"), error);
            return error;
            }

        if (iCodec.size() > 0)
            {
            iLog->Log(_L("Supported DNL codecs"));
            std::vector<TMSFormat*>::iterator itCodecs = iCodec.begin();
            TMSFormatType fmttype;
            for (; itCodecs < iCodec.end(); itCodecs++)
                {
                (*itCodecs)->GetType(fmttype);
                DisplayFormat(fmttype);
                }

            itCodecs = iCodec.begin();
            for (; itCodecs < iCodec.end(); itCodecs++)
                {
                iCodec.erase(itCodecs);
                //delete *itDnlCodecs;
                iFactory->DeleteFormat(*itCodecs);

                }
            }
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::IsCallTypeSupported(CStifItemParser& aItem)
    {
    iLog ->Log(_L("CTmsAudioServicesTestClass::IsCallTypeSupported"));
    TInt error = KErrNone;
    TInt calltype;
    TBool isSupported;
    aItem.GetNextInt(calltype);

    if (iFactory)
        {
        error = iFactory->IsCallTypeSupported((TMSCallType) calltype,
                isSupported);

        if (isSupported)
            {
            iLog->Log(_L("Call type is supported"));
            }
        else
            {
            iLog->Log(_L("Call type is not supported"));
            }
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::GetType(CStifItemParser& aItem)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::GetType"));
    TInt error = KErrNone;
    TMSFormatType fmttype;

    TPtrC formatType;
    error = aItem.GetNextString(formatType);
    if (error == KErrNone)
        {
        iTmsFormat->GetType(fmttype);
        if ((formatType == KTagG711 && fmttype != TMS_FORMAT_G711)
                || (formatType == KTagG729 && fmttype != TMS_FORMAT_G729)
                || (formatType == KTagILBC && fmttype != TMS_FORMAT_ILBC)
                || (formatType == KTagAMRNB && fmttype != TMS_FORMAT_AMR)
                || (formatType == KTagPCM16 && fmttype != TMS_FORMAT_PCM))
            {
            iLog->Log(
                    _L("CTmsAudioServicesTestClass::GetType failed, Format is same as expected"));
            error = KErrUnexpectedValue;
            }
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::SetDownlinkFormat(CStifItemParser& /*aItem */)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::SetDownlinkFormat"));
    TInt error = KErrNone;

    if (iTmsDnlink)
        {
        if (iTmsFormatDn)
            {
            error = iTmsDnlink->SetFormat(iTmsFormatDn);
            }
        else
            {
            error = iTmsDnlink->SetFormat(iTmsFormat);
            }

        if (error != KErrNone)
            {
            iLog->Log(
                    _L("CTmsAudioServicesTestClass::SetDownlinkFormat - failed, return error = %d" ),
                    error);
            return error;
            }
        }
    else
        {
        error = KErrNotReady;
        iLog->Log(
                _L("CTmsAudioServicesTestClass::SetDownlinkFormat - Need create TMSStream & TMSFormat first" ));
        }

    return error;
    }

TInt CTmsAudioServicesTestClass::ReSetDownlinkFormat(CStifItemParser& /*aItem */)
    {
    //FTRACE(FPrint(_L("CTmsAudioServicesTestClass::SetDownlinkFormat")));
    iLog->Log(_L("CTmsAudioServicesTestClass::ReSetDownlinkFormat"));
    TInt error = KErrNone;

    if (iTmsDnlink && iTmsFormatDn)
        {
        error = iTmsDnlink->ResetFormat(iTmsFormatDn);
        if (error != KErrNone)
            {
            iLog->Log(
                    _L("CTmsAudioServicesTestClass::ReSetDownlinkFormat - failed, return error = %d" ),
                    error);
            return error;
            }
        }
    else
        {
        error = KErrNotReady;
        iLog->Log(
                _L("CTmsAudioServicesTestClass::ReSetDownlinkFormat - Need create TMSStream & TMSFormat first" ));
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::OpenDownlink(CStifItemParser& /*aItem */)
    {
    iLog ->Log(_L("CTmsAudioServicesTestClass::OpenDownlink"));
    TInt error = KErrNone;

    if (iDnLinkStatus == UNINITIALIZED)
        {
        if (iTmsDnlink)
            {
            iLog->Log(
                    _L("CTmsAudioServicesTestClass::OpenDownlink - init Downlink"));
            iTmsDnlink->AddObserver(*this, NULL);
            error = iTmsDnlink->Init();
            }

        if (error != KErrNone)
            {
            iLog->Log(_L("DNL open failed: error = %d"), error);
            }
        else
            {
            AddExpectedEvent(EOpenDownlinkComplete, KMediumTimeout);
            }
        }
    else
        {
        iLog->Log(_L("DNL already open: error = %d"), KErrInUse);
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::SetUplinkFormat(CStifItemParser& /*aItem */)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::SetUplinkFormat"));
    TInt error = KErrNone;

    if (iTmsUplink)
        {
        if (iTmsFormatUp)
            {
            iLog->Log(
                    _L("CTmsAudioServicesTestClass::SetUplinkFormat - iTmsFormatUp"));
            error = iTmsUplink->SetFormat(iTmsFormatUp);
            }
        else
            {
            iLog->Log(
                    _L("CTmsAudioServicesTestClass::SetUplinkFormat - iTmsFormat"));
            error = iTmsUplink->SetFormat(iTmsFormat);
            }

        if (error != KErrNone)
            {
            iLog->Log(
                    _L("CTmsAudioServicesTestClass::SetUplinkFormat - failed, return error = %d" ),
                    error);
            return error;
            }
        }
    else
        {
        error = KErrNotReady;
        iLog->Log(
                _L("CTmsAudioServicesTestClass::SetUplinkFormat - Need create TMSStream & TMSFormat first" ));
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::ReSetUplinkFormat(CStifItemParser& /*aItem */)
    {
    //  FTRACE(FPrint(_L("CTmsAudioServicesTestClass::SetDownlinkFormat")));
    iLog->Log(_L("CTmsAudioServicesTestClass::ReSetUplinkFormat"));
    TInt error = KErrNone;

    if (iTmsUplink && iTmsFormatUp)
        {
        error = iTmsUplink->ResetFormat(iTmsFormatUp);
        if (error != KErrNone)
            {
            iLog->Log(
                    _L("CTmsAudioServicesTestClass::ReSetUplinkFormat - failed, return error = %d" ),
                    error);
            return error;
            }
        }
    else
        {
        error = KErrNotReady;
        iLog->Log(
                _L("CTmsAudioServicesTestClass::ReSetUplinkFormat - Need create TMSStream & TMSFormat first" ));
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::OpenUplink(CStifItemParser& /*aItem */)
    {
    //FTRACE(FPrint(_L("CTmsAudioServicesTestClass::OpenUplink")));
    iLog->Log(_L("CTmsAudioServicesTestClass::OpenUplink"));
    TInt error = KErrNone;

    if (iUpLinkStatus == UNINITIALIZED)
        {
        if (iTmsUplink)
            {
            iLog->Log(
                    _L("CTmsAudioServicesTestClass::OpenDownlink - init Uplink"));
            iTmsUplink->AddObserver(*this, NULL);
            error = iTmsUplink->Init();
            }

        if (error != KErrNone)
            {
            iLog->Log(_L("UPL open error: error = %d"), error);
            }
        else
            {
            AddExpectedEvent(EOpenUplinkComplete, KMediumTimeout);
            }
        }
    else
        {
        iLog->Log(_L("UPL already open: error = %d"), KErrInUse);
        }
    return error;
    }

// ----------------------------------------------------------------------------
// CTmsAudioServicesTestClass::DisplayFormat
//
// ----------------------------------------------------------------------------
//
void CTmsAudioServicesTestClass::DisplayFormat(TMSFormatType aFormat)
    {
    if (aFormat == TMS_FORMAT_AMR)
        {
        iLog->Log(_L("AMR-NB"));
        }
    else if (aFormat == TMS_FORMAT_G711)
        {
        iLog->Log(_L("G.711"));
        }
    else if (aFormat == TMS_FORMAT_G729)
        {
        iLog->Log(_L("G.729"));
        }
    else if (aFormat == TMS_FORMAT_ILBC)
        {
        iLog->Log(_L("iLBC"));
        }
    else if (aFormat == TMS_FORMAT_PCM)
        {
        iLog->Log(_L("PCM-16"));
        }
    }

TInt CTmsAudioServicesTestClass::Close(CStifItemParser& aItem)
    {
    FTRACE(FPrint(_L("CTmsAudioServicesTestClass::Close")));
    iLog->Log(_L("CTmsAudioServicesTestClass::Close"));
    TInt error = KErrNone;

    TPtrC closeType;
    error = aItem.GetNextString(closeType);
    if (error == KErrNone)
        {
        if (closeType == KTagUplink)
            {
            //iRecBufPtr.Set(NULL, 0, 0);
            iRecBufReady = EFalse;
            //iUpLinkCodec = ENULL;
            iTmsUplink->Deinit();
            iUpLinkStatus = UNINITIALIZED;
            iLog->Log(_L("Close Uplink"));
            AddExpectedEvent(EUplinkClosed, KMediumTimeout);
            }
        else if (closeType == KTagDnlink)
            {
            //iPlayBufPtr.Set(NULL, 0, 0);
            iTmsDnlink->Deinit();
            iDnLinkStatus = UNINITIALIZED;
            iPlayBufReady = EFalse;
            // iDnLinkCodec = ENULL;
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

TInt CTmsAudioServicesTestClass::Start(CStifItemParser& aItem)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::Start"));
    TInt error = KErrNone;

    TPtrC startType;
    error = aItem.GetNextString(startType);
    if (error == KErrNone)
        {
        if (startType == KTagUplink)
            {
            if ((iUpLinkStatus == INITIALIZED) || (iUpLinkStatus == PAUSED))
                {
                iTmsUplink->Start();

                if (iUpLinkStatus == INITIALIZED)
                    {
                    AddExpectedEvent(EEmptyBuffer, KMediumTimeout);
                    }
                else
                    {
                    AddExpectedEvent(EStreamStarted, KMediumTimeout);
                    }
                iUpLinkStatus = STARTED;
                iLog->Log(_L("Start Uplink"));
                }
            else
                {
                iLog->Log(_L("UPL not ready"));
                error = KErrNotReady;
                }
            }
        else if (startType == KTagDnlink)
            {
            if ((iDnLinkStatus == INITIALIZED) || (iDnLinkStatus == PAUSED))
                {
                iTmsDnlink->Start();
                if (iDnLinkStatus == INITIALIZED)
                    {
                    AddExpectedEvent(EFillBuffer, KMediumTimeout);
                    }
                else
                    {
                    AddExpectedEvent(EStreamStarted, KMediumTimeout);
                    }
                iDnLinkStatus = STARTED;
                iLog->Log(_L("Start Dnlink"));
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

TInt CTmsAudioServicesTestClass::Pause(CStifItemParser& aItem)
    {
    FTRACE(FPrint(_L("CTmsAudioServicesTestClass::Pause -- ENTER")));
    iLog->Log(_L("CTmsAudioServicesTestClass::Pause --ENTER"));
    TInt error = KErrNone;
    TPtrC startType;
    error = aItem.GetNextString(startType);
    if (error == KErrNone)
        {
        if (startType == KTagUplink)
            {
            if (iUpLinkStatus == STARTED)
                {
                iTmsUplink->Pause();
                iUpLinkStatus = PAUSED;
                iLog->Log(_L("Pause Uplink"));
                AddExpectedEvent(EStreamPaused, KMediumTimeout);
                }
            else
                {
                iLog->Log(_L("UPL not started"));
                error = KErrNotReady;
                }
            }
        else if (startType == KTagDnlink)
            {
            if (iDnLinkStatus == STARTED)
                {
                iTmsDnlink->Pause();
                iDnLinkStatus = PAUSED;
                iLog->Log(_L("Pause Dnlink"));
                AddExpectedEvent(EStreamPaused, KMediumTimeout);
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

    FTRACE(FPrint(_L("CTmsAudioServicesTestClass::Pause -- EXIT")));
    iLog->Log(_L("CTmsAudioServicesTestClass::Pause --EXIT"));
    return error;
    }

TInt CTmsAudioServicesTestClass::Stop(CStifItemParser& aItem)
    {
    FTRACE(FPrint(_L("CTmsAudioServicesTestClass::Stop")));
    iLog->Log(_L("CTmsAudioServicesTestClass::Stop"));
    TInt error = KErrNone;
    TPtrC stopType;
    error = aItem.GetNextString(stopType);
    if (error == KErrNone)
        {
        if (stopType == KTagUplink)
            {
            if ((iUpLinkStatus == STARTED) || (iUpLinkStatus == PAUSED))
                {
                iTmsUplink->Stop();
                iUpLinkStatus = INITIALIZED;
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
            if ((iDnLinkStatus == STARTED) || (iDnLinkStatus == PAUSED))
                {
                iTmsDnlink->Stop();
                iDnLinkStatus = INITIALIZED;
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

TInt CTmsAudioServicesTestClass::Gain(CStifItemParser& aItem)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::Gain"));
    TInt error = KErrNone;
    guint ret;
    TInt effecttype;
    aItem.GetNextInt(effecttype);

    if (effecttype == TMS_EFFECT_GAIN)
        {
        RDebug::Printf("[TMS STIF] GAIN, GetMaxLevel");
        ret = ((TMSGainEffect*) iTmsUplinkEffect)->GetMaxLevel(iMaxGain);
        RDebug::Printf(
                "[TMS STIF] GAIN, GetMaxLevel Level [%d] Ret Err [%d]",
                iMaxGain, ret);
        RDebug::Printf("[TMS STIF] GAIN, SetLevel to [%d]", iMaxGain);
        ret = ((TMSGainEffect*) iTmsUplinkEffect)->SetLevel(iMaxGain);
        RDebug::Printf("[TMS STIF] GAIN, SetLevel ret [%d]", ret);
        iLog->Log(_L("SetMaxGain: %d"), iMaxGain);
        RDebug::Printf("[TMS STIF] GAIN, GetLevel");
        ret = ((TMSGainEffect*) iTmsUplinkEffect)->GetLevel(iGain);
        RDebug::Printf(
                "[TMS STIF] GAIN, Exp Level [%d] Ret Level [%d] Ret Err [%d]",
                iMaxGain, iGain, ret);
        if (iGain != iMaxGain)
            {
            iLog->Log(
                    _L("GetGain doesn't return expected MaxGain!!! returned Gain = %d"),
                    iGain);
            return KErrUnexpectedValue;
            }

        ((TMSGainEffect*) iTmsUplinkEffect)->SetLevel(0);
        iLog->Log(_L("MuteMic"));
        ((TMSGainEffect*) iTmsUplinkEffect)->GetLevel(iGain);

        if (iGain != 0)
            {
            iLog->Log(
                    _L("GetGain does not return expected Mute value!!! returned Gain = %d"),
                    iGain);
            return KErrUnexpectedValue;
            }
        }
    else if (effecttype == TMS_EFFECT_GLOBAL_GAIN)
        {
        RDebug::Printf("[TMS STIF] GLOBAL GAIN, GetMaxLevel");
        ret = ((TMSGlobalGainEffect*) iGlobalGain)->GetMaxLevel(iMaxGain);
        RDebug::Printf(
                "[TMS STIF] GLOBAL GAIN, GetMaxLevel Level [%d] Ret Err [%d]",
                iMaxGain, ret);
        RDebug::Printf("[TMS STIF] GLOBAL GAIN, SetLevel to [%d]", iMaxGain);
        ret = ((TMSGlobalGainEffect*) iGlobalGain)->SetLevel(iMaxGain);
        RDebug::Printf("[TMS STIF] GLOBAL GAIN, SetLevel ret [%d]", ret);
        iLog->Log(_L("SetGlobleMaxGain: %d"), iMaxGain);
        RDebug::Printf("[TMS STIF] GLOBAL GAIN, GetLevel");
        ret = ((TMSGlobalGainEffect*) iGlobalGain)->GetLevel(iGain);
        RDebug::Printf(
                "[TMS STIF] GLOBAL GAIN, Exp Level [%d] Ret Level [%d] Ret Err [%d]",
                iMaxGain, iGain, ret);
        if (iGain != iMaxGain)
            {
            iLog->Log(
                    _L("GetGain doesn't return expected Globle MaxGain!!! returned Gain = %d"),
                    iGain);
            return KErrUnexpectedValue;
            }

        ((TMSGlobalGainEffect*) iGlobalGain)->SetLevel(0);
        iLog->Log(_L("MuteMic"));
        ((TMSGlobalGainEffect*) iGlobalGain)->GetLevel(iGain);

        if (iGain != 0)
            {
            iLog->Log(
                    _L("GetGain does not return expected loble Mute value!!! returned Gain = %d"),
                    iGain);
            return KErrUnexpectedValue;
            }
        }
    else
        {
        error = KErrArgument;
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::Volume(CStifItemParser& aItem)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::Volume"));
    TInt error = KErrNone;
    TInt effecttype;
    guint ret;
    aItem.GetNextInt(effecttype);

    if (effecttype == TMS_EFFECT_VOLUME)
        {
        RDebug::Printf("[TMS STIF] Volume, GetMaxLevel");
        ret = ((TMSVolumeEffect*) iTmsDnlinkEffect)->GetMaxLevel(iMaxVolume);
        RDebug::Printf("[TMS STIF] MaxLevel [%d] Ret Error [%d]", iMaxVolume,
                ret);
        RDebug::Printf("[TMS STIF] Volume, SetLevel to MaxLevel [%d]",
                iMaxVolume);
        ret = ((TMSVolumeEffect*) iTmsDnlinkEffect)->SetLevel(iMaxVolume);
        RDebug::Printf("[TMS STIF] SetLevel Ret Error [%d]", ret);
        iLog->Log(_L("SetMaxVolume: %d"), iMaxVolume);
        RDebug::Printf("[TMS STIF] GetLevel");
        ret = ((TMSVolumeEffect*) iTmsDnlinkEffect)->GetLevel(iVolume);
        RDebug::Printf("[TMS STIF] Level [%d] Ret Error [%d]", iVolume, ret);
        if (iVolume != iMaxVolume)
            {
            iLog->Log(
                    _L("GetVolume doesn't return expected MaxVolume!!! returned Volume = %d"),
                    iVolume);
            return KErrUnexpectedValue;
            }

        RDebug::Printf("[TMS STIF] Volume, SetLevel to 0 ");
        ret = ((TMSVolumeEffect*) iTmsDnlinkEffect)->SetLevel(0);
        RDebug::Printf("[TMS STIF] SetLevel Ret Error [%d]", ret);
        iLog->Log(_L("Mute Volume"));
        ret = ((TMSVolumeEffect*) iTmsDnlinkEffect)->GetLevel(iVolume);
        RDebug::Printf("[TMS STIF] Level [%d] Ret Error [%d]", iVolume, ret);
        if (iVolume != 0)
            {
            iLog->Log(
                    _L("GetVolume does not return expected Mute value!!! returned Volume = %d"),
                    iVolume);
            return KErrUnexpectedValue;
            }
        }
    else if (effecttype == TMS_EFFECT_GLOBAL_VOL)
        {
        RDebug::Printf("[TMS STIF] GLOBAL_VOL, Get Current Level");
        ret = ((TMSGlobalVolEffect*) iGlobalVol)->GetLevel(iVolume);
        RDebug::Printf("[TMS STIF] Current Vol Level [%d] Ret Error [%d]",
                iVolume, ret);
        RDebug::Printf("[TMS STIF] GLOBAL_VOL, GetMaxLevel");
        ret = ((TMSGlobalVolEffect*) iGlobalVol)->GetMaxLevel(iMaxVolume);
        RDebug::Printf("[TMS STIF] MaxLevel [%d] Ret Error [%d]", iMaxVolume,
                ret);
        RDebug::Printf("[TMS STIF] GLOBAL_VOL, SetLevel to [%d]", iMaxVolume);
        ret = ((TMSGlobalVolEffect*) iGlobalVol)->SetLevel(iMaxVolume);
        RDebug::Printf("[TMS STIF] SetLevel Ret Error [%d]", ret);
        iLog->Log(_L("SetMaxGlobleVolume: %d"), iMaxVolume);
        RDebug::Printf("[TMS STIF] GLOBAL_VOL,GetLevel");
        ret = ((TMSGlobalVolEffect*) iGlobalVol)->GetLevel(iVolume);
        RDebug::Printf(
                "[TMS STIF] Expected Level [%d] Ret Level [%d] Ret Error [%d]",
                iMaxVolume, iVolume, ret);
        iLog->Log(_L("GetMaxGlobleVolume: %d"), iVolume);
        if (iVolume != iMaxVolume)
            {
            iLog->Log(
                    _L("GetGlobleVolume doesn't return expected MaxVolume!!! returned Volume = %d"),
                    iVolume);
            return KErrUnexpectedValue;
            }

        ((TMSGlobalVolEffect*) iGlobalVol)->SetLevel(0);
        iLog->Log(_L("Mute Globle Volume"));
        ((TMSGlobalVolEffect*) iGlobalVol)->GetLevel(iVolume);

        if (iVolume != 0)
            {
            iLog->Log(
                    _L("GetGlobleVolume does not return expected Mute value!!! returned Volume = %d"),
                    iVolume);
            return KErrUnexpectedValue;
            }
        }
    else
        {
        error = KErrArgument;
        }

    return error;
    }

TInt CTmsAudioServicesTestClass::GetStreamType(CStifItemParser& aItem)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::GetStreamType"));
    TInt error = KErrNone;
    gint status(KErrNone);
    TPtrC StreamType;
    error = aItem.GetNextString(StreamType);
    if (error == KErrNone)
        {
        if (StreamType == KTagDnlink)
            {
            if (iTmsDnlink)
                {
                status = iTmsDnlink->GetStreamType();
                return status;
                }
            }
        else if (StreamType == KTagUplink)
            {
            if (iTmsUplink)
                {
                status = iTmsUplink->GetStreamType();
                return status;
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

TInt CTmsAudioServicesTestClass::GetStreamState(CStifItemParser& aItem)
    {
    iLog ->Log(_L("CTmsAudioServicesTestClass::GetStreamState"));
    TInt error = KErrNone;
    gint state(KErrNone);
    TPtrC StreamType;
    error = aItem.GetNextString(StreamType);
    if (error == KErrNone)
        {
        if (StreamType == KTagDnlink)
            {
            if (iTmsDnlink)
                {
                state = iTmsDnlink->GetState();
                iLog->Log(_L("Downlink STREAM state:[ %d]"), state);
                return state;
                }
            }
        else if (StreamType == KTagUplink)
            {
            if (iTmsUplink)
                {
                state = iTmsUplink->GetState();
                iLog->Log(_L("Uplink STREAM state:[ %d]"), state);
                return state;
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

TInt CTmsAudioServicesTestClass::AddGlobalRoutingObserver(CStifItemParser& /*aItem */)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::AddGlobalRoutingObserver"));
    TInt error = KErrNone;

    if (iTmsGlobalRouting)
        {
        error = iTmsGlobalRouting->AddObserver(*this, NULL);
        }

    if (error != KErrNone)
        {
        iLog->Log(
                _L("CTmsAudioServicesTestClass::AddGlobalRoutingObserver failed error [%d]"),
                error);
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::DeleteGlobalRoutingObserver(CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAu dioServicesTestClass::DeleteGlobalRoutingObserver"));
    TInt error = KErrNone;

    if (iTmsGlobalRouting)
        {
        error = iTmsGlobalRouting->RemoveObserver(*this);
        }

    if (error != KErrNone)
        {
        iLog->Log(
                _L("CTmsAudioServicesTestClass::DeleteGlobalRoutingObserver failed error [%d]"),
                error);
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::SetOutput(CStifItemParser& aItem)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::SetOutputDevice"));
    TInt error = KErrNone;
    TPtrC OutputType;
    error = aItem.GetNextString(OutputType);
    if (error == KErrNone)
        {
        if (OutputType == KTagNone)
            {
            error = iTmsGlobalRouting->SetOutput(TMS_AUDIO_OUTPUT_NONE);
            iLog->Log(
                    _L("CTmsAudioServicesTestClass::SetOutputDevice - TMS_AUDIO_OUTPUT_NONE, error:[%d] "),
                    error);
            }
        else if (OutputType == KTagPublic)
            {
            error = iTmsGlobalRouting->SetOutput(TMS_AUDIO_OUTPUT_PUBLIC);
            iLog->Log(
                    _L("CTmsAudioServicesTestClass::SetOutputDevice - TMS_AUDIO_OUTPUT_PUBLIC, error:[%d] "),
                    error);
            }
        else if (OutputType == KTagPrivate)
            {
            error = iTmsGlobalRouting->SetOutput(TMS_AUDIO_OUTPUT_PRIVATE);
            iLog->Log(
                    _L ("CTmsAudioServicesTestClass::SetOutputDevice - TMS_AUDIO_OUTPUT_PRIVATE, error:[%d] "),
                    error);
            }
        else if (OutputType == KTagHandset)
            {
            error = iTmsGlobalRouting->SetOutput(TMS_AUDIO_OUTPUT_HANDSET);
            iLog->Log(
                    _L("CTmsAudioServicesTestClass::SetOutputDevice - TMS_AUDIO_OUTPUT_HANDSET, error:[%d] "),
                    error);
            }
        else if (OutputType == KTagLoudspeaker)
            {
            error = iTmsGlobalRouting->SetOutput(TMS_AUDIO_OUTPUT_LOUDSPEAKER);
            iLog->Log(
                    _L("CTmsAudioServicesTestClass::SetOutputDevice - TMS_AUDIO_OUTPUT_LOUDSPEAKER, error:[%d] "),
                    error);
            }
        else if (OutputType == KTagWiredAccessory)
            {
            error = iTmsGlobalRouting->SetOutput(
                    TMS_AUDIO_OUTPUT_WIRED_ACCESSORY);
            iLog->Log(
                    _L("CTmsAudioServicesTestClass::SetOutputDevice - TMS_AUDIO_OUTPUT_WIRED_ACCESSORY, error:[%d] "),
                    error);
            }
        else if (OutputType == KTagAccessory)
            {
            error = iTmsGlobalRouting->SetOutput(TMS_AUDIO_OUTPUT_ACCESSORY);
            iLog->Log(
                    _L("CTmsAudioServicesTestClass::SetOutputDevice - TMS_AUDIO_OUTPUT_ACCESSORY, error:[%d] "),
                    error);
            }
        else if (OutputType == KTagTTY)
            {
            error = iTmsGlobalRouting->SetOutput(TMS_AUDIO_OUTPUT_ETTY);
            iLog->Log(
                    _L("CTmsAudioServicesTestClass::SetOutputDevice - TMS_AUDIO_OUTPUT_ETTY, error:[%d] "),
                    error);
            }
        else
            {
            iLog-> Log(KMsgBadTestParameters);
            error = KErrBadTestParameter;
            return error;
            }

        AddExpectedEvent(ESetOutputComplete, KLongTimeout);
        }

    return error;
    }

TInt CTmsAudioServicesTestClass::GetOutput(CStifItemParser & aItem)
    {
    iLog->Log(_L("CallAudioControl::GetOutput"));
    TInt error = KErrNone;

    if (iTmsGlobalRouting)
        {
        error = iTmsGlobalRouting->GetOutput(iDevice);
        iLog->Log(
                _L("CTmsAudioServicesTestClass::GetOutput, error:[%d] iDevice:[%d]"),
                error, iDevice);
        }

    if (error != KErrNone)
        {
        return error;
        }

    TPtrC OutputType;
    error = aItem.GetNextString(OutputType);
    if (error == KErrNone)
        {
        if (OutputType == KTagNone)
            {
            if (iDevice != TMS_AUDIO_OUTPUT_NONE)
                {
                error = KErrUnexpectedValue;
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::GetOutput - not expected output, error:[%d] "),
                        error);
                }
            }
        else if (OutputType == KTagPublic)
            {
            if (iDevice != TMS_AUDIO_OUTPUT_PUBLIC)
                {
                error = KErrUnexpectedValue;
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::GetOutput - TMS_AUDIO_OUTPUT_PUBLIC, error:[%d] "),
                        error);
                }
            }
        else if (OutputType == KTagPrivate)
            {
            if (iDevice != TMS_AUDIO_OUTPUT_PRIVATE)
                {
                error = KErrUnexpectedValue;
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::GetOutput - TMS_AUDIO_OUTPUT_PRIVATE, error:[%d] "),
                        error);
                }
            }
        else if (OutputType == KTagHandset)
            {
            if (iDevice != TMS_AUDIO_OUTPUT_HANDSET)
                {
                error = KErrUnexpectedValue;
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::GetOutput - not expected output, error:[%d] "),
                        error);
                }
            }
        else if (OutputType == KTagLoudspeaker)
            {
            if (iDevice != TMS_AUDIO_OUTPUT_LOUDSPEAKER)
                {
                error = KErrUnexpectedValue;
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::GetOutput - not expected output, error:[%d] "),
                        error);
                }
            }
        else if (OutputType == KTagWiredAccessory)
            {
            if (iDevice != TMS_AUDIO_OUTPUT_WIRED_ACCESSORY)
                {
                error = KErrUnexpectedValue;
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::GetOutput - not expected output, error:[%d] "),
                        error);
                }
            }
        else if (OutputType == KTagAccessory)
            {
            if (iDevice != TMS_AUDIO_OUTPUT_ACCESSORY)
                {
                error = KErrUnexpectedValue;
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::GetOutput - not expected output, error:[%d] "),
                        error);
                }
            }
        else if (OutputType == KTagTTY)
            {
            if (iDevice != TMS_AUDIO_OUTPUT_ETTY)
                {
                error = KErrUnexpectedValue;
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::GetOutput - not expected output, error:[%d] "),
                        error);
                }
            }
        else
            {
            iLog->Log(KMsgBadTestParameters);
            error = KErrBadTestParameter;
            }
        }

    return error; // GetOutput doesn't expect any specific output value.
    }

TInt CTmsAudioServicesTestClass::GetPreviousOutput(CStifItemParser& aItem)
    {
    iLog ->Log(_L("CallAudioControl::GetPreviousOutput"));
    TInt error = KErrNone;

    if (iTmsGlobalRouting)
        {
        error = iTmsGlobalRouting->GetPreviousOutput(iDevice);
        iLog->Log(
                _L("CTmsAudioServicesTestClass::GetPreviousOutput, error:[%d] "),
                error);
        }

    if (error != KErrNone)
        {
        return error;
        }

    TPtrC OutputType;
    error = aItem.GetNextString(OutputType);
    if (error == KErrNone)
        {
        if (OutputType == KTagNone)
            {
            if (iDevice != TMS_AUDIO_OUTPUT_NONE)
                {
                error = KErrUnexpectedValue;
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::GetPreviousOutput - not expected output, error:[%d] "),
                        error);
                }
            }
        else if (OutputType == KTagPublic)
            {
            if (iDevice != TMS_AUDIO_OUTPUT_PUBLIC)
                {
                error = KErrUnexpectedValue;
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::GetPreviousOutput - TMS_AUDIO_OUTPUT_PUBLIC, error:[%d] "),
                        error);
                }
            }
        else if (OutputType == KTagPrivate)
            {
            if (iDevice != TMS_AUDIO_OUTPUT_PRIVATE)
                {
                error = KErrUnexpectedValue;
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::GetPreviousOutput - TMS_AUDIO_OUTPUT_PRIVATE, error:[%d] "),
                        error);
                }
            }
        else if (OutputType == KTagHandset)
            {
            if (iDevice != TMS_AUDIO_OUTPUT_HANDSET)
                {
                error = KErrUnexpectedValue;
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::GetPreviousOutput - not expected output, error:[%d] "),
                        error);
                }
            }
        else if (OutputType == KTagLoudspeaker)
            {
            if (iDevice != TMS_AUDIO_OUTPUT_LOUDSPEAKER)
                {
                error = KErrUnexpectedValue;
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::GetPreviousOutput - not expected output, error:[%d] "),
                        error);
                }
            }
        else if (OutputType == KTagWiredAccessory)
            {
            if (iDevice != TMS_AUDIO_OUTPUT_WIRED_ACCESSORY)
                {
                error = KErrUnexpectedValue;
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::GetPreviousOutput - not expected output, error:[%d] "),
                        error);
                }
            }
        else if (OutputType == KTagAccessory)
            {
            if (iDevice != TMS_AUDIO_OUTPUT_ACCESSORY)
                {
                error = KErrUnexpectedValue;
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::GetPreviousOutput - not expected output, error:[%d] "),
                        error);
                }
            }
        else if (OutputType == KTagTTY)
            {
            if (iDevice != TMS_AUDIO_OUTPUT_ETTY)
                {
                error = KErrUnexpectedValue;
                iLog->Log(
                        _L("CTmsAudioServicesTestClass::GetPr eviousOutput - not expected output, error:[%d] "),
                        error);
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

TInt CTmsAudioServicesTestClass::GetAvailableOutputs(CStifItemParser& /*aItem */)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::GetAvailableOutputs"));
    TInt error = KErrNone;

    if (iTmsGlobalRouting)
        {
        error = iTmsGlobalRouting ->GetAvailableOutputs(iAvailableoutputs);

        if (error == KErrNone)
            {
            iLog->Log(
                    _L("CTmsAudioServicesTestClass::GetAvailableOutputs, display output"));
            std::vector<guint>::iterator iteroutputs =
                    iAvailableoutputs.begin();

            for (; iteroutputs < iAvailableoutputs.end(); iteroutputs++)
                {
                DisplayDevice(*iteroutputs);
                }
            }
        else
            {
            iLog->Log(
                    _L("CTmsAudioServicesTestClass::GetAvailableOutputs failed, error = %d"),
                    error);
            }
        }
    else
        {
        error = KErrNotReady;
        }

    return error;
    }

TInt CTmsAudioServicesTestClass::GetBitRateList(CStifItemParser& /*aItem*/)
    {
    iLog ->Log(_L("CTmsAudioServicesTestClass::GetBitRateList"));
    TInt error = KErrNone;
    error = iTmsFormatUp -> GetSupportedBitRates(iBitratesVector);

    if (error == KErrNone)
        {
        iLog->Log(
                _L("CTmsAudioServicesTestClass::GetBitRateList - display supported bitRate list"));
        std::vector<guint>::iterator itBitrates = iBitratesVector.begin();
        for (; itBitrates <= iBitratesVector.end(); itBitrates++)
            {
            iLog->Log(
                    _L("CTmsAudioServicesTestClass::GetBitRateList BR: %d"),
                    *itBitrates);
            }
        }
    else
        {
        iLog->Log(
                _L("CTmsAudioServicesTestClass::GetBitRateList failed. error: %d"),
                error);
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::SetBitrates(CStifItemParser& aItem)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::SetBitrate"));
    TInt error = KErrNone;
    TPtrC bitRate;
    error = aItem.GetNextString(bitRate);
    if (error == KErrNone)
        {
        if (bitRate == KTagMin)
            {
            if (iBitratesVector.size() > 0)
                {
                std::vector<guint>::iterator iBitrate =
                        iBitratesVector.begin();
                error = iTmsFormatUp -> SetBitRate(*iBitrate);
                iLog->Log(_L("BR set %d"), *iBitrate);
                }
            }
        else if (bitRate == KTagMax)
            {
            if (iBitratesVector.size() > 0)
                {
                std::vector<guint>::iterator iBitrate = iBitratesVector.end();
                error = iTmsFormatUp -> SetBitRate(*iBitrate);
                iLog->Log(_L("BR set %d"), *iBitrate);
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

TInt CTmsAudioServicesTestClass::GetBitrates(CStifItemParser& aItem)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::GetBitrate"));
    TInt error = KErrNone;
    TPtrC expectedBitRate;
    error = aItem.GetNextString(expectedBitRate);
    if (error == KErrNone)
        {
        if (expectedBitRate == KTagMin)
            {
            if (iBitratesVector.size() > 0)
                {
                std::vector<guint>::iterator currentBitrate =
                        iBitratesVector.begin();
                guint iBitrate;
                error = iTmsFormatUp -> GetBitRate(iBitrate);
                if (iBitrate != *currentBitrate)
                    {
                    iLog->Log(_L("BR get %d"), iBitrate);
                    iLog->Log(_L("BR set %d"), *currentBitrate);
                    error = KErrUnexpectedValue;
                    }
                }
            }
        else if (expectedBitRate == KTagMax)
            {
            if (iBitratesVector.size() > 0)
                {
                guint iBitrate;
                std::vector<guint>::iterator currentBitrate =
                        iBitratesVector.end();
                error = iTmsFormatUp -> GetBitRate(iBitrate);
                if (iBitrate != *currentBitrate)
                    {
                    iLog->Log(_L("BR get %d"), iBitrate);
                    iLog->Log(_L("BR set %d"), *currentBitrate);
                    error = KErrUnexpectedValue;
                    }
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

TInt CTmsAudioServicesTestClass::GetVAD(CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::GetVAD"));
    TInt error = KErrNone;
    TBool aVad;

    switch (iUpLinkCodec)
        {
        case TMS_FORMAT_G711:
            error = ((TMSG711Format*) iTmsFormatUp)->GetVADMode(aVad);
            break;
        case TMS_FORMAT_G729:
            error = ((TMSG729Format*) iTmsFormatUp)->GetVADMode(aVad);
            break;
        case TMS_FORMAT_ILBC:
            error = ((TMSILBCFormat*) iTmsFormatUp)->GetVADMode(aVad);
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

TInt CTmsAudioServicesTestClass::ToggleVAD(CStifItemParser & /*aItem*/)
    {
    iLog ->Log(_L("CTmsAudioServicesTestClass::ToggleVAD"));
    TInt error = KErrNone;
    iVad = (iVad) ? EFalse : ETrue;

    switch (iUpLinkCodec)
        {
        case TMS_FORMAT_G711:
            error = ((TMSG711Format*) iTmsFormatUp)->SetVADMode(iVad);
            break;
        case TMS_FORMAT_G729:
            error = ((TMSG729Format*) iTmsFormatUp)->SetVADMode(iVad);
            break;
        case TMS_FORMAT_ILBC:
            error = ((TMSILBCFormat*) iTmsFormatUp)->SetVADMode(iVad);
            break;
        default:
            break;
        }
    iLog->Log(_L("VAD set: %d"), iVad);
    return error;
    }

TInt CTmsAudioServicesTestClass::GetMode(CStifItemParser& aItem)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::GetMode"));
    TInt error = KErrNone;
    TPtrC linkType;
    error = aItem.GetNextString(linkType);
    if (error == KErrNone)
        {
        if ((linkType == KTagUplink) && (iUpLinkCodec == TMS_FORMAT_G711))
            {
            TMSG711CodecMode aMode;
            ((TMSG711Format*) iTmsFormatUp)->GetMode(aMode);

            if (aMode == TMS_G711_CODEC_MODE_ALAW)
                {
                iLog->Log(_L("UPL Mode: aLaw"));
                }
            else if (aMode == TMS_G711_CODEC_MODE_MULAW)
                {
                iLog->Log(_L("UPL Mode: uLaw"));
                }
            else
                {
                iLog->Log(_L("UPL Mode: unknown"));
                }
            }
        else if ((linkType == KTagUplink)
                && (iUpLinkCodec == TMS_FORMAT_ILBC))
            {
            TMSILBCCodecMode aMode;
            ((TMSILBCFormat*) iTmsFormatUp)->GetMode(aMode);

            if (aMode == TMS_ILBC_CODEC_MODE_20MS_FRAME)
                {
                iLog->Log(_L("UPL Mode: 20ms frame"));
                }
            else if (aMode == TMS_ILBC_CODEC_MODE_30MS_FRAME)
                {
                iLog->Log(_L("UPL Mode: 30ms frame"));
                }
            else
                {
                iLog->Log(_L("UPL Mode: unknown"));
                }
            }
        else if ((linkType == KTagDnlink)
                && (iDnLinkCodec == TMS_FORMAT_G711))
            {
            TMSG711CodecMode aMode;
            ((TMSG711Format*) iTmsFormatDn)->GetMode(aMode);

            if (aMode == TMS_G711_CODEC_MODE_ALAW)
                {
                iLog->Log(_L("DNL Mode: aLaw"));
                }
            else if (aMode == TMS_G711_CODEC_MODE_MULAW)
                {
                iLog->Log(_L("DNL Mode: uLaw"));
                }
            else
                {
                iLog->Log(_L("DNL Mode: unknown"));
                }
            }
        else if ((linkType == KTagDnlink)
                && (iDnLinkCodec == TMS_FORMAT_ILBC))
            {
            TMSILBCCodecMode aMode;
            ((TMSILBCFormat*) iTmsFormatDn)->GetMode(aMode);

            if (aMode == TMS_ILBC_CODEC_MODE_20MS_FRAME)
                {
                iLog->Log(_L("DNL Mode: 20ms frame"));
                }
            else if (aMode == TMS_ILBC_CODEC_MODE_30MS_FRAME)
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

TInt CTmsAudioServicesTestClass::SetMode(CStifItemParser& aItem)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::SetMode"));
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
        if ((linkType == KTagUplink) && (iUpLinkCodec == TMS_FORMAT_G711)
                && (mode == KTagALaw))
            {
            iLog->Log(_L("UPL Mode Set: aLaw"));
            ((TMSG711Format*) iTmsFormatUp)->SetMode(TMS_G711_CODEC_MODE_ALAW);
            }
        else if ((linkType == KTagUplink)
                && (iUpLinkCodec == TMS_FORMAT_G711) && (mode == KTagULaw))
            {
            iLog->Log(_L("UPL Mode Set: uLaw"));
            ((TMSG711Format*) iTmsFormatUp)->SetMode(
                    TMS_G711_CODEC_MODE_MULAW);
            }
        else if ((linkType == KTagUplink)
                && (iUpLinkCodec == TMS_FORMAT_ILBC) && (mode == KTag20ms))
            {
            iLog->Log(_L("UPL Mode Set: 20ms frame"));
            ((TMSILBCFormat*) iTmsFormatUp)->SetMode(
                    TMS_ILBC_CODEC_MODE_20MS_FRAME);
            }
        else if ((linkType == KTagUplink)
                && (iUpLinkCodec == TMS_FORMAT_ILBC) && (mode == KTag30ms))
            {
            iLog->Log(_L("UPL Mode Set: 30ms frame"));
            ((TMSILBCFormat*) iTmsFormatUp)->SetMode(
                    TMS_ILBC_CODEC_MODE_30MS_FRAME);
            }
        else if ((linkType == KTagDnlink)
                && (iDnLinkCodec == TMS_FORMAT_G711) && (mode == KTagALaw))
            {
            iLog->Log(_L("DNL Mode Set: aLaw"));
            ((TMSG711Format*) iTmsFormatDn)->SetMode(TMS_G711_CODEC_MODE_ALAW);
            }
        else if ((linkType == KTagDnlink)
                && (iDnLinkCodec == TMS_FORMAT_G711) && (mode == KTagULaw))
            {
            iLog->Log(_L("DNL Mode Set: uLaw"));
            ((TMSG711Format*) iTmsFormatDn)->SetMode(
                    TMS_G711_CODEC_MODE_MULAW);
            }
        else if ((linkType == KTagDnlink)
                && (iDnLinkCodec == TMS_FORMAT_ILBC) && (mode == KTag20ms))
            {
            iLog->Log(_L("DNL Mode Set: 20ms frame"));
            ((TMSILBCFormat*) iTmsFormatDn)->SetMode(
                    TMS_ILBC_CODEC_MODE_20MS_FRAME);
            }
        else if ((linkType == KTagDnlink)
                && (iDnLinkCodec == TMS_FORMAT_ILBC) && (mode == KTag30ms))
            {
            iLog->Log(_L("DNL Mode Set: 30ms frame"));
            ((TMSILBCFormat*) iTmsFormatDn)->SetMode(
                    TMS_ILBC_CODEC_MODE_30MS_FRAME);
            }
        else
            {
            iLog->Log(KMsgBadTestParameters);
            error = KErrBadTestParameter;
            }
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::GetCNG(CStifItemParser& /*aItem*/)
    {
    FTRACE (FPrint(_L("CTmsAudioServicesTestClass::GetCNG")));
    iLog->Log(_L("CTmsAudioServicesTestClass::GetCNG"));
    TInt error = KErrNone;
    TBool aCng = EFalse;
    switch (iDnLinkCodec)
        {
        case TMS_FORMAT_G711:
            error = ((TMSG711Format*) iTmsFormatDn)->GetCNG(aCng);
            iLog->Log(_L("CNG: %d"), aCng);
            break;
        case TMS_FORMAT_ILBC:
            error = ((TMSILBCFormat*) iTmsFormatDn)->GetCNG(aCng);
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

TInt CTmsAudioServicesTestClass::ToggleCNG(CStifItemParser & /*aItem*/)
    {

    iLog ->Log(_L("CTmsAudioServicesTestClass::ToggleCNG"));
    TInt error = KErrNone;
    iCng = (iCng) ? EFalse : ETrue;
    switch (iDnLinkCodec)
        {
        case TMS_FORMAT_G711:
            ((TMSG711Format*) iTmsFormatDn)->SetCNG(iCng);
            iLog->Log(_L("CNG set: %d"), iCng);
            break;
        case TMS_FORMAT_ILBC:
            ((TMSILBCFormat*) iTmsFormatDn)->SetCNG(iCng);
            iLog->Log(_L("CNG set: %d"), iCng);
            break;
        default:
            break;
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::GetPLC(CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::GetPLC"));
    TInt error = KErrNone;
    TBool aPlc = EFalse;
    if (iDnLinkCodec == TMS_FORMAT_G711)
        {
        ((TMSG711Format*) iTmsFormatDn)->GetPlc(aPlc);
        iLog->Log(_L("PLC: %d"), aPlc);
        }

    if (aPlc != iPlc)
        {
        iLog->Log(_L("returned Cng doesn't match set Plc: Failed"));
        error = KErrUnexpectedValue;
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::TogglePLC(CStifItemParser & /*aItem*/)
    {

    iLog ->Log(_L("CTmsAudioServicesTestClass::TogglePLC"));
    TInt error = KErrNone;
    iPlc = (iPlc) ? EFalse : ETrue;
    if (iDnLinkCodec == TMS_FORMAT_G711)
        {
        error = ((TMSG711Format*) iTmsFormatDn)->SetPlc(iPlc);
        iLog->Log(_L("PLC set: %d"), iPlc);
        }

    return error;
    }

TInt CTmsAudioServicesTestClass::GetBufferType(CStifItemParser& /*aItem*/)
    {
    FTRACE (FPrint(_L("CTmsAudioServicesTestClass::GetBufferType")));
    iLog->Log(_L("CTmsAudioServicesTestClass::GetBufferType"));
    TInt error = KErrNone;
    return error;
    }

// Note: IP ONLY
TInt CTmsAudioServicesTestClass::AddClientSrcToDnlStream(CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::AddClientSrcToDnlStream"));
    TInt error = KErrNone;
    if (iTmsDnlink && iTmsClientSource)
        {
        error = iTmsDnlink->AddSource(iTmsClientSource);
        }
    iLog->Log(
            _L("CTmsAudioServicesTestClass::AddClientSrcToDnlStream Error [%d]"),
            error);
    return error;
    }

// NOTE: CS and IP
TInt CTmsAudioServicesTestClass::AddMicSrcToUplStream(CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::AddMicSrcToUplStream"));
    TInt error = KErrNone;
    if (iTmsUplink && iTmsMicSource)
        {
        error = iTmsUplink->AddSource(iTmsMicSource);
        }
    iLog->Log(
            _L("CTmsAudioServicesTestClass::AddMicSrcToUplStream Error [%d]"),
            error);
    return error;
    }

// Note: CS ONLY
TInt CTmsAudioServicesTestClass::AddModemSrcToDnlStream(CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::AddModemSrcToDnlStream"));
    TInt error = KErrNone;
    if (iTmsDnlink && iTmsModemSource)
        {
        error = iTmsDnlink->AddSource(iTmsModemSource);
        }
    iLog->Log(
            _L("CTmsAudioServicesTestClass::AddModemSrcToDnlStream Error [%d]"),
            error);
    return error;
    }

// Note: IP ONLY
TInt CTmsAudioServicesTestClass::RemoveClientSrcFromDnlStream(
        CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::RemoveClientSrcFromDnlStream"));
    TInt error = KErrNone;
    if (iTmsDnlink && iTmsClientSource)
        {
        error = iTmsDnlink->RemoveSource(iTmsClientSource);
        }
    iLog->Log(
            _L("CTmsAudioServicesTestClass::RemoveClientSrcFromDnlStream Error [%d]"),
            error);
    return error;
    }

// Note: CS and IP
TInt CTmsAudioServicesTestClass::RemoveMicSrcFromUplStream(CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::RemoveMicSrcFromUplStream"));
    TInt error = KErrNone;
    if (iTmsUplink && iTmsMicSource)
        {
        error = iTmsUplink->RemoveSource(iTmsMicSource);
        }
    iLog->Log(
            _L("CTmsAudioServicesTestClass::RemoveMicSrcFromUplStream Error [%d]"),
            error);
    return error;
    }

// Note: CS ONLY
TInt CTmsAudioServicesTestClass::RemoveModemSrcFromDnlStream(CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::RemoveModemSrcFromDnlStream"));
    TInt error = KErrNone;
    if (iTmsDnlink && iTmsModemSource)
        {
        error = iTmsDnlink->RemoveSource(iTmsModemSource);
        }
    iLog->Log(
            _L("CTmsAudioServicesTestClass::RemoveModemSrcFromDnlStream Error [%d]"),
            error);
    return error;
    }

TInt CTmsAudioServicesTestClass::AddSourceObserver(CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::AddSourceObserver"));
    TInt error = KErrNone;
    if (iTmsClientSource)
        {
        error = ((TMSClientSource*) iTmsClientSource)->AddObserver(*this,
                NULL);
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::RemoveSourceObserver(CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::RemoveSourceObserver"));
    TInt error = KErrNone;
    if (iTmsClientSource)
        {
        error = ((TMSClientSource*) iTmsClientSource)->RemoveObserver(*this);
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::AddClientSinkToUplStream(CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::AddClientSinkToUplStream"));
    TInt error = KErrNone;
    if (iTmsUplink && iTmsClientSink)
        {
        //((TMSClientSink*) iTmsSink)->AddObserver(*this, NULL);
        error = iTmsUplink->AddSink(iTmsClientSink);
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::AddModemSinkToUplStream(CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::AddModemSinkToUplStream"));
    TInt error = KErrNone;
    if (iTmsUplink && iTmsModemSink)
        {
        //((TMSClientSink*) iTmsSink)->AddObserver(*this, NULL);
        error = iTmsUplink->AddSink(iTmsModemSink);
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::AddSpkrSinkToDnlStream(CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::AddSpkrSinkToDnlStream"));
    TInt error = KErrNone;
    if (iTmsDnlink && iTmsSpkrSink)
        {
        error = iTmsDnlink->AddSink(iTmsSpkrSink);
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::RemoveClientSinkFromUplStream(
        CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::RemoveClientSinkFromUplStream"));
    TInt error = KErrNone;
    if (iTmsUplink && iTmsClientSink)
        {
        error = iTmsUplink->RemoveSink(iTmsClientSink);
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::RemoveModemSinkFromUplStream(
        CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::RemoveModemSinkFromUplStream"));
    TInt error = KErrNone;
    if (iTmsUplink && iTmsModemSink)
        {
        error = iTmsUplink->RemoveSink(iTmsModemSink);
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::RemoveSpkrSinkFromDnlStream(CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::RemoveSpkrSinkFromDnlStream"));
    TInt error = KErrNone;
    if (iTmsDnlink && iTmsSpkrSink)
        {
        error = iTmsDnlink->RemoveSink(iTmsSpkrSink);
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::AddSinkObserver(CStifItemParser& /*aItem*/)
    {
    iLog ->Log(_L("CTmsAudioServicesTestClass::AddSinkObserver"));
    TInt error = KErrNone;
    if (iTmsUplink && iTmsClientSink)
        {
        error = ((TMSClientSink*) iTmsClientSink)->AddObserver(*this, NULL);
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::RemoveSinkObserver(CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::RemoveSinkObserver"));
    TInt error = KErrNone;
    if (iTmsUplink && iTmsClientSink)
        {
        error = ((TMSClientSink*) iTmsClientSink)->RemoveObserver(*this);
        }
    return error;
    }

gint CTmsAudioServicesTestClass::CreateVolumeEffect()
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::CreateVolumeEffect"));
    gint status(KErrNone);
    if (iFactory && iTmsDnlink && !iTmsDnlinkEffect)
        {
        status = iFactory->CreateEffect(TMS_EFFECT_VOLUME, iTmsDnlinkEffect);
        if (status == KErrNone)
            {
            ((TMSVolumeEffect*) iTmsDnlinkEffect)->AddObserver(*this, NULL);
            }
        }
    RDebug::Printf("[TMS STIF] CreateVolumeEffect Return [%d]", status);
    return status;
    }

gint CTmsAudioServicesTestClass::CreateVolumeGlobleEffect()
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::CreateVolumeGlobleEffect"));
    gint status(KErrNone);
    if (iFactory && !iGlobalVol)
        {
        status = iFactory->CreateEffect(TMS_EFFECT_GLOBAL_VOL, iGlobalVol);

        if (status == KErrNone)
            {
            ((TMSGlobalVolEffect*) iGlobalVol)->AddObserver(*this, NULL);
            iLog->Log(
                    _L("CTmsAudioServicesTestClass::CreateVolumeGlobleEffect - AddObserver"));
            }
        }
    RDebug::Printf("[TMS STIF] CreateVolumeGlobleEffect Return [%d]", status);
    return status;
    }

TInt CTmsAudioServicesTestClass::AddVolumeEffectToStream(CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::AddVolumeEffectToStream"));
    TInt error = KErrNone;
    if (iTmsDnlink && iTmsDnlinkEffect)
        {
        error = iTmsDnlink->AddEffect(iTmsDnlinkEffect);
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::AddGlobleVolumeEffectToStream(
        CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::AddGlobleVolumeEffectToStream"));
    TInt error = KErrNone;
    if (iTmsDnlink && iGlobalVol)
        {
        error = iTmsDnlink->AddEffect(iGlobalVol);
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::RemoveGlobleVolumeEffectToStream(
        CStifItemParser& /*aItem*/)
    {
    iLog->Log(
            _L("CTmsAudioServicesTestClass::RemoveGlobleVolumeEffectToStream"));
    TInt error = KErrNone;
    if (iTmsDnlink && iGlobalVol)
        {
        error = iTmsDnlink->RemoveEffect(iGlobalVol);
        }
    return error;
    }

gint CTmsAudioServicesTestClass::CreateGainEffect()
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::CreateGainEffect"));
    gint status(KErrNone);

    if (iFactory && iTmsUplink && !iTmsUplinkEffect)
        {
        status = iFactory->CreateEffect(TMS_EFFECT_GAIN, iTmsUplinkEffect);
        if (status == KErrNone)
            {
            ((TMSGainEffect*) iTmsUplinkEffect)->AddObserver(*this, NULL);
            }
        }
    RDebug::Printf("[TMS STIF] CreateGainEffect Return [%d]", status);
    return status;
    }

gint CTmsAudioServicesTestClass::CreateGlobleGainEffect()
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::CreateGlobleGainEffect"));
    gint status(KErrNone);
    if (iFactory && !iGlobalGain)
        {
        status = iFactory->CreateEffect(TMS_EFFECT_GLOBAL_GAIN, iGlobalGain);
        if (status == KErrNone)
            {
            ((TMSGlobalGainEffect*) iGlobalGain)->AddObserver(*this, NULL);
            }
        }
    RDebug::Printf("[TMS STIF] CreateGlobleGainEffect Return [%d]", status);
    return status;
    }

TInt CTmsAudioServicesTestClass::AddGainEffectToStream(CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::AddGainEffectToStream"));
    TInt error = KErrNone;
    if (iTmsUplink && iTmsUplinkEffect)
        {
        error = iTmsUplink->AddEffect(iTmsUplinkEffect);
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::AddGlobleGainEffectToStream(CStifItemParser& /*aItem*/)
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::AddGlobleGainEffectToStream"));
    TInt error = KErrNone;
    if (iTmsUplink && iGlobalGain)
        {
        error = iTmsUplink->AddEffect(iGlobalGain);
        }
    return error;
    }

TInt CTmsAudioServicesTestClass::RemoveGlobleGainEffectToStream(
        CStifItemParser& /*aItem*/)
    {
    iLog->Log(
            _L("CTmsAudioServicesTestClass::RemoveGlobleGainEffectToStream"));
    TInt error = KErrNone;
    if (iTmsUplink && iGlobalGain)
        {
        error = iTmsUplink->RemoveEffect(iGlobalGain);
        }
    return error;
    }

// ----------------------------------------------------------------------------
// CTmsTestEngine::DoLoopback
//
// ----------------------------------------------------------------------------
//
void CTmsAudioServicesTestClass::DoLoopback()
    {
    iLog->Log(_L("CTmsAudioServicesTestClass::DoLoopback"));

    if (iPlayBufReady && iRecBufReady)
        {
        iLog->Log(_L("Both uplink and downlink are ready"));
        guint8* srcptr(0);
        guint8* desptr(0);
        guint srcsize(0);
        guint dessize(0);

        iPlayBuf->GetDataPtr(desptr);
        iPlayBuf->GetDataSize(dessize);

        iRecBuf->GetDataPtr(srcptr);
        iRecBuf->GetDataSize(srcsize);

        Mem::Copy(desptr, srcptr, srcsize);

        ((TMSClientSource*) iTmsClientSource)->BufferFilled(*iPlayBuf);
        ((TMSClientSink*) iTmsClientSink)->BufferProcessed(iRecBuf);

        iPlayBufReady = EFalse; // buf filled, ready for FillBuffer
        iRecBufReady = EFalse; // buf consumed, ready for EmptyBuffer
        }

    iLog->Log(_L("CTmsAudioServicesTestClass::DoLoopback END"));
    }

TInt CTmsAudioServicesTestClass::ConfigEncAudDevice(CStifItemParser& /*aItem*/)
    {
    FTRACE(FPrint(_L("CTmsAudioServicesTestClass::ConfigEncAudDevice")));
    iLog->Log(_L("CTmsAudioServicesTestClass::ConfigEncAudDevice"));
    TInt error = KErrNone;
    return error;
    }

// ----------------------------------------------------------------------------
// CTmsAudioServicesTestClass::DisplayDevice
//
// ----------------------------------------------------------------------------
//
void CTmsAudioServicesTestClass::DisplayDevice(TMSAudioOutput device)
    {
    switch (device)
        {
        case TMS_AUDIO_OUTPUT_NONE:
            iLog->Log(_L("none"));
            break;
        case TMS_AUDIO_OUTPUT_PUBLIC:
            iLog->Log(_L("Public"));
            break;
        case TMS_AUDIO_OUTPUT_PRIVATE:
            iLog->Log(_L("Private"));
            break;
        case TMS_AUDIO_OUTPUT_HANDSET:
            iLog->Log(_L("Handset"));
            break;
        case TMS_AUDIO_OUTPUT_LOUDSPEAKER:
            iLog->Log(_L("Loudspeaker"));
            break;
        case TMS_AUDIO_OUTPUT_WIRED_ACCESSORY:
            iLog->Log(_L("Wired accessory"));
            break;
        case TMS_AUDIO_OUTPUT_ACCESSORY:
            iLog->Log(_L("Accessory"));
            break;
        case TMS_AUDIO_OUTPUT_ETTY:
            iLog->Log(_L("TTY"));
            break;
        default:
            break;
        }
    }
