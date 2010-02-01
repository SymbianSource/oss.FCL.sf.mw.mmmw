/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is the main interface to the RadioServer. It implements
*				 the client-side session.
*
*/



// INCLUDE FILES
#include 	<e32math.h>
#include    "RadioSession.h"
#include    "RadioClientServer.h"
#include	"RadioRequest.h"
#include	"RadioEventHandler.h"
#include    "RadioDebug.h"

// CONSTANTS
// Index to list of asynchronous requests that generates response to MRadioObserver.
const TInt KReqInternal				 = 0;
const TInt KReqRequestTunerControl   = 1;
const TInt KReqSetFrequencyRange     = 2;
const TInt KReqSetFrequency          = 3;
const TInt KReqStationSeek           = 4;
const TInt KReqPlay					 = 5;
const TInt KReqStationSeekByPTY      = 6;
const TInt KReqStationSeekByTA       = 7;
const TInt KReqStationSeekByTP       = 8;

// Max number of retries to start the server
const TInt KRadioStartRetry	= 2;
// Minimum of one for each asynchronous message + one to allow cancel
const TInt KRadioMessageSlots = 11;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// StartServer
// Static function to start the server process thread.
// Start the server process/thread which lives in an EPOCEXE object.
// Returns: TInt: KErrNone (0) if no error
// -----------------------------------------------------------------------------
//
static TInt StartServer()
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] StartServer()"));

    const TUidType serverUid(KNullUid, KNullUid, KRadioServerUid3);

	// We just create a new server process. Simultaneous launching of two such
	// processes should be detected when the second one attempts to create
	// the server object, failing with KErrAlreadyExists.
    RProcess server;
    TInt r = server.Create(KRadioServerImg, KNullDesC, serverUid);

    if ( r != KErrNone )
	    {
		RADIO_RDEBUG_INT(_L("[RADIO-SESS] server.Create() failed [%d]"), r);
        return r;
	    }
	TRequestStatus stat;
	server.Rendezvous(stat);
	if ( stat != KRequestPending )
		{
		server.Kill(0);		// abort startup
		}
	else
		{
		server.Resume();	// logon OK - start the server
		}
	User::WaitForRequest(stat);		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	r = (server.ExitType() == EExitPanic) ? KErrGeneral : stat.Int();
	server.Close();
	return r;
	}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RRadioSession::RRadioSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RRadioSession::RRadioSession()
	:	iConnected(EFalse),
		iDestinationPckg(TMMFMessageDestination(KRadioServerInterfaceUid, KRadioServerObjectHandle)),
		iObserver(NULL),
		iPrimaryClient(EFalse),
		iRdsNotify(EFalse)
    {
    }

// -----------------------------------------------------------------------------
// RRadioSession::Connect
// Create a client-side session. Start the server if not started already.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::Connect(
	MRadioObserver& aObserver,
	TBool aPrimaryClient )
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] Connect() entry"));
	// check if already connected
	if ( iConnected )
		{
		RADIO_RDEBUG(_L("[RADIO-SESS] Connect(): already connected"));
		return KErrAlreadyExists;
		}

   	TInt retry = KRadioStartRetry;
   	TInt err = KErrGeneral;
   	TInt numMessageSlots = KRadioMessageSlots;
   	for (;;)
   		{
   		// Try to create a new session with the server
   		err = CreateSession(KRadioServerName, Version(), numMessageSlots);
   		if ( (err != KErrNotFound) && (err != KErrServerTerminated) )
   			{
   			break;	// Connected to existing server - ok
			}
   		if ( --retry == 0 )
   			{
			break; 	// Failed.
			}
		// Server not running, try to start it.
   		err = StartServer();
   		if ( (err != KErrNone) && (err != KErrAlreadyExists) )
   			{
			break; 	// Server not launched - propagate error
			}
   		}

	if ( err != KErrNone )
		{
		RADIO_RDEBUG_INT(_L("[RADIO-SESS] Connect(): Unable to start server [%d]"), err);
		return err;
		}

	// Create active object request handlers and add it to scheduler
	TRAP(err, StartRequestHandlersL(aObserver));
	if ( err == KErrNone )
		{
		// Create active object event handlers and add it to scheduler
		TRAP(err, StartEventHandlersL(aObserver));
		if ( err == KErrNone )
			{
			iConnected = ETrue;
			iObserver = &aObserver;
			if ( aPrimaryClient )
				{
				iPrimaryClient = ETrue;
				err = SendReceive(ERadioServAddPrimaryClient, TIpcArgs(&iDestinationPckg));
				}
			}
		else
			{
			iRequests.ResetAndDestroy();
			iEventHandlers.ResetAndDestroy();
			}
		}
	else
		{
		iRequests.ResetAndDestroy();
		}

	RADIO_RDEBUG_INT(_L("[RADIO-SESS] Connect() exit err=[%d]"), err);
	return err;
	}

// -----------------------------------------------------------------------------
// RRadioSession::Version
// -----------------------------------------------------------------------------
//
EXPORT_C TVersion RRadioSession::Version() const
	{
	return(TVersion(KRadioServerVersionMajor, KRadioServerVersionMinor, KRadioServerVersionBuild));
	}

// -----------------------------------------------------------------------------
// RRadioSession::Close
// -----------------------------------------------------------------------------
//
EXPORT_C void RRadioSession::Close()
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] Close() entry"));
	iRequests.ResetAndDestroy();
	iRequests.Close();

	iEventHandlers.ResetAndDestroy();
	iEventHandlers.Close();

	iRdsEventHandlers.ResetAndDestroy();
	iRdsEventHandlers.Close();

	if ( iConnected )
		{
		iConnected = EFalse;
		if ( iPrimaryClient )
			{
			SendReceive(ERadioServRemovePrimaryClient, TIpcArgs(&iDestinationPckg));
			}
		RSessionBase::Close();
		}
	RADIO_RDEBUG(_L("[RADIO-SESS] Close() exit"));
	}

//********** TunerUtility control begins

// -----------------------------------------------------------------------------
// RRadioSession::RequestTunerControl
// -----------------------------------------------------------------------------
//
EXPORT_C void RRadioSession::RequestTunerControl(
	TRsTuner aTuner )
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] RequestTunerControl()"));
	if ( iConnected )
		{
		if ( iRequests[KReqRequestTunerControl]->IsActive() )
			{
			iRequests[KReqRequestTunerControl]->Cancel();
			}
		SendReceive(ERadioServRequestTunerControl,
					TIpcArgs(&iDestinationPckg, aTuner),
					iRequests[KReqRequestTunerControl]->iStatus);
		iRequests[KReqRequestTunerControl]->SetActive();
		}
	else
		{
		iRequests[KReqInternal]->CompleteRequest(ERadioServRequestTunerControl, KErrDisconnected);
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetTunerCapabilities
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::GetTunerCapabilities(
	TRsTunerCapabilities& aCaps ) const
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] GetTunerCapabilities()"));
	if ( iConnected )
		{
		TPckg<TRsTunerCapabilities> caps(aCaps);
		return SendReceive(ERadioServGetTunerCapabilities,
						   TIpcArgs(&iDestinationPckg, &caps));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::EnableTunerInOfflineMode
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::EnableTunerInOfflineMode(
	TBool aEnable )
	{
	RADIO_RDEBUG_INT(_L("[RADIO-SESS] EnableTunerInOfflineMode() [%d]"), aEnable);
	if ( iConnected )
		{
		return SendReceive(ERadioServEnableTunerInOfflineMode,
    					   TIpcArgs(&iDestinationPckg, aEnable));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::SetFrequencyRange
// -----------------------------------------------------------------------------
//
EXPORT_C void RRadioSession::SetFrequencyRange(
	TRsFrequencyRange aRange )
	{
	RADIO_RDEBUG_INT(_L("[RADIO-SESS] SetFrequencyRange() [%d]"), aRange);
	if ( iConnected )
		{
		if ( iRequests[KReqSetFrequencyRange]->IsActive() )
			{
			iRequests[KReqSetFrequencyRange]->Cancel();
			}
		SendReceive(ERadioServSetFrequencyRange,
					TIpcArgs(&iDestinationPckg, aRange),
					iRequests[KReqSetFrequencyRange]->iStatus);
		iRequests[KReqSetFrequencyRange]->SetActive();
		}
	else
		{
		iRequests[KReqInternal]->CompleteRequest(ERadioServSetFrequencyRange, KErrDisconnected);
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::CancelSetFrequencyRange
// -----------------------------------------------------------------------------
//
EXPORT_C void RRadioSession::CancelSetFrequencyRange()
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] CancelSetFrequencyRange()"));
	if ( iConnected && iRequests[KReqSetFrequencyRange]->IsActive() )
		{
		iRequests[KReqSetFrequencyRange]->Cancel();
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetFrequencyRange
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::GetFrequencyRange(
	TRsFrequencyRange& aRange,
	TInt& aMinFreq,
	TInt& aMaxFreq) const
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] GetFrequencyRange()"));
	if ( iConnected )
		{
		TPckg<TRsFrequencyRange> range(aRange);
		TPckg<TInt> min(aMinFreq);
		TPckg<TInt> max(aMaxFreq);
		return SendReceive(ERadioServGetFrequencyRange,
						   TIpcArgs(&iDestinationPckg, &range, &min, &max));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::SetFrequency
// -----------------------------------------------------------------------------
//
EXPORT_C void RRadioSession::SetFrequency(
	TInt aFrequency )
	{
	RADIO_RDEBUG_INT(_L("[RADIO-SESS] SetFrequency() [%d]"), aFrequency);
	if ( iConnected )
		{
		if ( iRequests[KReqSetFrequency]->IsActive() )
			{
			iRequests[KReqSetFrequency]->Cancel();
			}
		SendReceive(ERadioServSetFrequency,
					TIpcArgs(&iDestinationPckg, aFrequency),
					iRequests[KReqSetFrequency]->iStatus);
		iRequests[KReqSetFrequency]->SetActive();
		}
	else
		{
		iRequests[KReqInternal]->CompleteRequest(ERadioServSetFrequency, KErrDisconnected);
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::CancelSetFrequency
// -----------------------------------------------------------------------------
//
EXPORT_C void RRadioSession::CancelSetFrequency()
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] CancelSetFrequency()"));
	if ( iConnected && iRequests[KReqSetFrequency]->IsActive() )
		{
		iRequests[KReqSetFrequency]->Cancel();
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetFrequency
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::GetFrequency(
	TInt& aFrequency ) const
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] GetFrequency()"));
	if ( iConnected )
		{
		TPckg<TInt> frequency(aFrequency);
		return SendReceive(ERadioServGetFrequency,
						   TIpcArgs(&iDestinationPckg, &frequency));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::StationSeek
// -----------------------------------------------------------------------------
EXPORT_C void RRadioSession::StationSeek(
	TBool aUpwards )
	{
	RADIO_RDEBUG_INT(_L("[RADIO-SESS] StationSeek() [%d]"), aUpwards);
	if ( iConnected )
		{
		if ( iRequests[KReqStationSeek]->IsActive() )
			{
			iRequests[KReqStationSeek]->Cancel();
			}
		SendReceive(ERadioServStationSeek,
					TIpcArgs(&iDestinationPckg, aUpwards, &(iRequests[KReqStationSeek]->iIntPckg)),
					iRequests[KReqStationSeek]->iStatus);
		iRequests[KReqStationSeek]->SetActive();
		}
	else
		{
		iRequests[KReqInternal]->CompleteRequest(ERadioServStationSeek, KErrDisconnected);
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::CancelStationSeek
// -----------------------------------------------------------------------------
EXPORT_C void RRadioSession::CancelStationSeek()
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] CancelStationSeek()"));
	if ( iConnected && iRequests[KReqStationSeek]->IsActive() )
		{
		iRequests[KReqStationSeek]->Cancel();
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetSignalStrength
// -----------------------------------------------------------------------------
EXPORT_C TInt RRadioSession::GetSignalStrength(
	TInt& aSignalStrength ) const
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] GetSignalStrength()"));
	if ( iConnected )
		{
		TPckg<TInt> strength(aSignalStrength);
		return SendReceive(ERadioServGetSignalStrength,
						   TIpcArgs(&iDestinationPckg, &strength));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetMaxSignalStrength
// -----------------------------------------------------------------------------
EXPORT_C TInt RRadioSession::GetMaxSignalStrength(
	TInt& aMaxSignalStrength ) const
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] GetMaxSignalStrength()"));
	if ( iConnected )
		{
		TPckg<TInt> maxStrength(aMaxSignalStrength);
		return SendReceive(ERadioServGetMaxSignalStrength,
						   TIpcArgs(&iDestinationPckg, &maxStrength));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetStereoMode
// -----------------------------------------------------------------------------
EXPORT_C TInt RRadioSession::GetStereoMode(
	TBool& aStereo ) const
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] GetStereoMode()"));
	if ( iConnected )
		{
		TPckg<TBool> stereo(aStereo);
		return SendReceive(ERadioServGetStereoMode,
						   TIpcArgs(&iDestinationPckg, &stereo));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::ForceMonoReception
// -----------------------------------------------------------------------------
EXPORT_C TInt RRadioSession::ForceMonoReception(
	TBool aForcedMono )
	{
	RADIO_RDEBUG_INT(_L("[RADIO-SESS] ForceMonoReception() [%d]"), aForcedMono);
	if ( iConnected )
		{
		return SendReceive(ERadioServForceMonoReception,
						   TIpcArgs(&iDestinationPckg, aForcedMono));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetForceMonoReception
// -----------------------------------------------------------------------------
EXPORT_C TInt RRadioSession::GetForceMonoReception(
	TBool& aForcedMono ) const
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] GetForceMonoReception()"));
	if ( iConnected )
		{
		TPckg<TBool> mono(aForcedMono);
		return SendReceive(ERadioServGetForceMonoReception,
						   TIpcArgs(&iDestinationPckg, &mono));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::SetSquelch
// -----------------------------------------------------------------------------
EXPORT_C TInt RRadioSession::SetSquelch(
	TBool aEnabled )
	{
	RADIO_RDEBUG_INT(_L("[RADIO-SESS] SetSquelch() [%d]"), aEnabled);
	if ( iConnected )
		{
		return SendReceive(ERadioServSetSquelch,
						   TIpcArgs(&iDestinationPckg, aEnabled));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetSquelch
// -----------------------------------------------------------------------------
EXPORT_C TInt RRadioSession::GetSquelch(
	TBool& aSquelch ) const
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] GetSquelch()"));
	if ( iConnected )
		{
		TPckg<TBool> squelch(aSquelch);
		return SendReceive(ERadioServGetSquelch,
						   TIpcArgs(&iDestinationPckg, &squelch));
		}
	else
		{
		return KErrDisconnected;
		}
	}

//********** PlayerUtility control begins

// -----------------------------------------------------------------------------
// RRadioSession::PlayerState
// -----------------------------------------------------------------------------
EXPORT_C TInt RRadioSession::PlayerState(
	TRsPlayerState& aState ) const
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] GetPlayerState()"));
	if ( iConnected )
		{
		TPckg<TRsPlayerState> state(aState);
		return SendReceive(ERadioServGetPlayerState,
						   TIpcArgs(&iDestinationPckg, &state));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::Play
// -----------------------------------------------------------------------------
EXPORT_C void RRadioSession::Play()
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] Play()"));
	if ( iConnected )
		{
		if ( iRequests[KReqPlay]->IsActive() )
			{
			// Already active; ignore.
			return;
			}
		SendReceive(ERadioServPlay,
					TIpcArgs(&iDestinationPckg),
					iRequests[KReqPlay]->iStatus);
		iRequests[KReqPlay]->SetActive();
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::Stop
// -----------------------------------------------------------------------------
EXPORT_C void RRadioSession::Stop(
	TBool aIfOnlyPrimaryClient )
	{
	RADIO_RDEBUG_INT(_L("[RADIO-SESS] Stop() [%d]"), aIfOnlyPrimaryClient);
	if ( iConnected )
		{
		TInt err = Send(ERadioServStop,
						TIpcArgs(&iDestinationPckg, aIfOnlyPrimaryClient));
		if (err != KErrNone)
			{
			RADIO_RDEBUG_INT(_L("[RADIO-SESS] Stop() - err[%d]"), err);
			}
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetMaxVolume
// -----------------------------------------------------------------------------
EXPORT_C TInt RRadioSession::GetMaxVolume(
    TInt& aMaxVolume ) const
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] GetMaxVolume()"));
	if ( iConnected )
		{
		TPckg<TInt> maxVolume(aMaxVolume);
		return SendReceive(ERadioServGetMaxVolume,
						   TIpcArgs(&iDestinationPckg, &maxVolume));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::SetVolume
// -----------------------------------------------------------------------------
EXPORT_C TInt RRadioSession::SetVolume(
	TInt aVolume )
	{
	RADIO_RDEBUG_INT(_L("[RADIO-SESS] SetVolume() [%d]"), aVolume);
	if ( iConnected )
		{
		return SendReceive(ERadioServSetVolume,
						   TIpcArgs(&iDestinationPckg, aVolume));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetVolume
// -----------------------------------------------------------------------------
EXPORT_C TInt RRadioSession::GetVolume(
	TInt& aVolume ) const
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] GetVolume()"));
	if ( iConnected )
		{
		TPckg<TInt> volume(aVolume);
		return SendReceive(ERadioServGetVolume,
						   TIpcArgs(&iDestinationPckg, &volume));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::SetVolumeRamp
// -----------------------------------------------------------------------------
EXPORT_C TInt RRadioSession::SetVolumeRamp(
	const TTimeIntervalMicroSeconds& aRampInterval )
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] SetVolumeRamp()"));
	if ( iConnected )
		{
		TPckgBuf<TTimeIntervalMicroSeconds> interval(aRampInterval);
		return SendReceive(ERadioServSetVolumeRamp,
						   TIpcArgs(&iDestinationPckg, &interval));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::Mute
// -----------------------------------------------------------------------------
EXPORT_C TInt RRadioSession::Mute(
	TBool aMute )
	{
	RADIO_RDEBUG_INT(_L("[RADIO-SESS] Mute() [%d]"), aMute);
	if ( iConnected )
		{
		return SendReceive(ERadioServSetMute,
						   TIpcArgs(&iDestinationPckg, aMute));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetMuteStatus
// -----------------------------------------------------------------------------
EXPORT_C TInt RRadioSession::GetMuteStatus(
	TBool& aMute ) const
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] Mute()"));
	if ( iConnected )
		{
		TPckg<TBool> mute(aMute);
		return SendReceive(ERadioServGetMuteStatus,
						   TIpcArgs(&iDestinationPckg, &mute));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::SetBalance
// -----------------------------------------------------------------------------
EXPORT_C TInt RRadioSession::SetBalance(
	TInt aLeftPercentage,
	TInt aRightPercentage )
	{
	RADIO_RDEBUG_INT2(_L("[RADIO-SESS] SetBalance() left=[%d] right=[%d]"), aLeftPercentage, aRightPercentage);
	if ( iConnected )
		{
		return SendReceive(ERadioServSetBalance,
						   TIpcArgs(&iDestinationPckg, aLeftPercentage, aRightPercentage));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetBalance
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::GetBalance(
	TInt& aLeftPercentage,
	TInt& aRightPercentage ) const
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] GetBalance()"));
	if ( iConnected )
		{
		TPckg<TInt> left(aLeftPercentage);
		TPckg<TInt> right(aRightPercentage);
		return SendReceive(ERadioServGetBalance,
						   TIpcArgs(&iDestinationPckg, &left, &right));
		}
	else
		{
		return KErrDisconnected;
		}
	}

//********** RDSUtility control begins

// -----------------------------------------------------------------------------
// RRadioSession::GetRdsCapabilities
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::GetRdsCapabilities(
	TRsRdsCapabilities& aCaps ) const
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] GetRdsCapabilities()"));
	if ( iConnected )
		{
		TPckg<TRsRdsCapabilities> caps(aCaps);
		return SendReceive(ERadioServGetRdsCapabilities,
						   TIpcArgs(&iDestinationPckg, &caps));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetRdsSignalStatus
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::GetRdsSignalStatus(
	TBool& aRdsSignal ) const
    {
    RADIO_RDEBUG(_L("[RADIO-SESS] GetRdsSignalStatus()"));
	if ( iConnected )
		{
		TPckg<TBool> signal(aRdsSignal);
		return SendReceive(ERadioServGetRdsSignalStatus,
						   TIpcArgs(&iDestinationPckg, &signal));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::NotifyRdsDataChange
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::NotifyRdsDataChange(
	TRsRdsData aRdsData )
	{
	RADIO_RDEBUG_INT(_L("[RADIO-SESS] NotifyRdsDataChange() [%d]"), aRdsData.iRdsFunctions);
	if ( !iConnected )
		{
		return KErrDisconnected;
		}

	if ( !iObserver )
		{
		return KErrNotReady;
		}

	TRAPD(err, StartRdsEventHandlersL(aRdsData.iRdsFunctions));
	if ( err != KErrNone )
		{
		RADIO_RDEBUG_INT(_L("[RADIO-SESS] NotifyRdsDataChange() err=[%d]"), err);
		return err;
		}
	err = SendReceive(ERadioServNotifyRdsDataChange,
					  TIpcArgs(&iDestinationPckg));
	if ( err == KErrNone )
		{
		iRdsNotify = ETrue;
		}
	return err;
    }

// -----------------------------------------------------------------------------
// RRadioSession::CancelNotifyRdsDataChange
// -----------------------------------------------------------------------------
//
EXPORT_C void RRadioSession::CancelNotifyRdsDataChange()
    {
    RADIO_RDEBUG(_L("[RADIO-SESS] CancelNotifyRdsDataChange()"));
	if ( iConnected && iRdsNotify )
		{
		iRdsEventHandlers.ResetAndDestroy();
		SendReceive(ERadioServCancelNotifyRdsDataChange,
					TIpcArgs(&iDestinationPckg));
		}
    }

// -----------------------------------------------------------------------------
// RRadioSession::NotifyRadioTextPlusChange
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::NotifyRadioTextPlusChange(
	RArray<TInt>& /*aRtPlusClasses*/ )
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] NotifyRadioTextPlusChange()"));
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// RRadioSession::CancelNotifyRadioTextPlusChange
// -----------------------------------------------------------------------------
//
EXPORT_C void RRadioSession::CancelNotifyRadioTextPlusChange()
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] CancelNotifyRadioTextPlusChange()"));
	// NotifyRadioTextPlusChange is not supported
	}

// -----------------------------------------------------------------------------
// RRadioSession::SetAutomaticSwitching
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::SetAutomaticSwitching(
	TBool aAuto )
	{
	RADIO_RDEBUG_INT(_L("[RADIO-SESS] SetAutomaticSwitching() [%d]"), aAuto);
	if ( iConnected )
		{
		return SendReceive(ERadioServSetAutomaticSwitching,
						   TIpcArgs(&iDestinationPckg, aAuto));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetAutomaticSwitching
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::GetAutomaticSwitching(
	TBool& aAuto )
	{
    RADIO_RDEBUG(_L("[RADIO-SESS] GetAutomaticSwitching()"));
	if ( iConnected )
		{
		TPckg<TBool> autoSwitching(aAuto);
		return SendReceive(ERadioServGetAutomaticSwitching,
						   TIpcArgs(&iDestinationPckg, &autoSwitching));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::CancelAFSearch
// -----------------------------------------------------------------------------
//
EXPORT_C void RRadioSession::CancelAFSearch()
	{
   	RADIO_RDEBUG(_L("[RADIO-SESS] CancelAFSearch()"));
	if ( iConnected )
		{
		SendReceive(ERadioServCancelAFSearch, TIpcArgs(&iDestinationPckg));
		}
	}

 // -----------------------------------------------------------------------------
// RRadioSession::SetAutomaticTrafficAnnouncement
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::SetAutomaticTrafficAnnouncement(
	TBool /*aAuto*/ )
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] SetAutomaticTrafficAnnouncement()"));
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetAutomaticTrafficAnnouncement
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::GetAutomaticTrafficAnnouncement(
	TBool& /*aAuto*/ )
	{
    RADIO_RDEBUG(_L("[RADIO-SESS] GetAutomaticTrafficAnnouncement()"));
	return KErrNotSupported;
   	}

// -----------------------------------------------------------------------------
// RRadioSession::StationSeekByPTY
// -----------------------------------------------------------------------------
//
EXPORT_C void RRadioSession::StationSeekByPTY(
	TRsRdsProgrammeType aPty,
	TBool aSeekUp )
   	{
   	RADIO_RDEBUG_INT(_L("[RADIO-SESS] StationSeekByPTY() pty=[%d]"), aPty);
	if ( iConnected )
		{
		if ( iRequests[KReqStationSeekByPTY]->IsActive() )
			{
			iRequests[KReqStationSeekByPTY]->Cancel();
			}
		SendReceive(ERadioServStationSeekByPTY,
					TIpcArgs(&iDestinationPckg, aPty, aSeekUp, &(iRequests[KReqStationSeekByPTY]->iIntPckg)),
					iRequests[KReqStationSeekByPTY]->iStatus);
		iRequests[KReqStationSeekByPTY]->SetActive();
		}
	else
		{
		iRequests[KReqInternal]->CompleteRequest(ERadioServStationSeekByPTY, KErrDisconnected);
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::StationSeekByTA
// -----------------------------------------------------------------------------
//
EXPORT_C void RRadioSession::StationSeekByTA(
	TBool aSeekUp )
   	{
    RADIO_RDEBUG(_L("[RADIO-SESS] StationSeekByTA()"));
	if ( iConnected )
		{
		if ( iRequests[KReqStationSeekByTA]->IsActive() )
			{
			iRequests[KReqStationSeekByTA]->Cancel();
			}
		SendReceive(ERadioServStationSeekByTA,
					TIpcArgs(&iDestinationPckg, aSeekUp, &(iRequests[KReqStationSeekByTA]->iIntPckg)),
					iRequests[KReqStationSeekByTA]->iStatus);
		iRequests[KReqStationSeekByTA]->SetActive();
		}
	else
		{
		iRequests[KReqInternal]->CompleteRequest(ERadioServStationSeekByTA, KErrDisconnected);
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::StationSeekByTP
// -----------------------------------------------------------------------------
//
EXPORT_C void RRadioSession::StationSeekByTP(
	TBool aSeekUp )
   	{
    RADIO_RDEBUG(_L("[RADIO-SESS] StationSeekByTP()"));
	if ( iConnected )
		{
		if ( iRequests[KReqStationSeekByTP]->IsActive() )
			{
			iRequests[KReqStationSeekByTP]->Cancel();
			}
		SendReceive(ERadioServStationSeekByTP,
					TIpcArgs(&iDestinationPckg, aSeekUp, &(iRequests[KReqStationSeekByTP]->iIntPckg)),
					iRequests[KReqStationSeekByTP]->iStatus);
		iRequests[KReqStationSeekByTP]->SetActive();
		}
	else
		{
		iRequests[KReqInternal]->CompleteRequest(ERadioServStationSeekByTP, KErrDisconnected);
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::CancelRdsStationSeek
// -----------------------------------------------------------------------------
//
EXPORT_C void RRadioSession::CancelRdsStationSeek()
  	{
   	RADIO_RDEBUG(_L("[RADIO-SESS] CancelRdsStationSeek()"));
	if ( iConnected )
		{
		if ( iRequests[KReqStationSeekByPTY]->IsActive() )
			{
			iRequests[KReqStationSeekByPTY]->Cancel();
			}
		if ( iRequests[KReqStationSeekByTA]->IsActive() )
			{
			iRequests[KReqStationSeekByTA]->Cancel();
			}
		if ( iRequests[KReqStationSeekByTP]->IsActive() )
			{
			iRequests[KReqStationSeekByTP]->Cancel();
			}
		}
  	}

// -----------------------------------------------------------------------------
// RRadioSession::GetFreqByPTY
// -----------------------------------------------------------------------------
//
EXPORT_C void RRadioSession::GetFreqByPTY(
	TRsRdsProgrammeType /*aPty*/ )
	{
  	RADIO_RDEBUG(_L("[RADIO-SESS] GetFreqByPTY()"));
	iRequests[KReqInternal]->CompleteRequest(ERadioServGetFreqByPTY, KErrNotSupported);
  	}

// -----------------------------------------------------------------------------
// RRadioSession::CancelGetFreqByPTY
// -----------------------------------------------------------------------------
//
EXPORT_C void RRadioSession::CancelGetFreqByPTY()
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] CancelGetFreqByPTY()"));
	// GetFreqByPTY is not supported
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetFreqByTA
// -----------------------------------------------------------------------------
//
EXPORT_C void RRadioSession::GetFreqByTA()
	{
    RADIO_RDEBUG(_L("[RADIO-SESS] GetFreqByTA()"));
	iRequests[KReqInternal]->CompleteRequest(ERadioServGetFreqByTA, KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// RRadioSession::CancelGetFreqByTA
// -----------------------------------------------------------------------------
//
EXPORT_C void RRadioSession::CancelGetFreqByTA()
    {
    RADIO_RDEBUG(_L("[RADIO-SESS] CancelGetFreqByTA()"));
    // GetFreqByTA is not supported
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetPSByPTY
// -----------------------------------------------------------------------------
//
EXPORT_C void RRadioSession::GetPSByPTY(
	TRsRdsProgrammeType /*aPty*/ )
    {
    RADIO_RDEBUG(_L("[RADIO-SESS] GetPSByPTY()"));
	iRequests[KReqInternal]->CompleteRequest(ERadioServGetPSByPTY, KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// RRadioSession::CancelGetPSByPTY
// -----------------------------------------------------------------------------
//
EXPORT_C void RRadioSession::CancelGetPSByPTY()
    {
    RADIO_RDEBUG(_L("[RADIO-SESS] CancelGetPSByPTY()"));
    // GetPSByPTY is not supported
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetPSByTA
// -----------------------------------------------------------------------------
//
EXPORT_C void RRadioSession::GetPSByTA()
    {
    RADIO_RDEBUG(_L("[RADIO-SESS] GetPSByTA()"));
	iRequests[KReqInternal]->CompleteRequest(ERadioServGetPSByTA, KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// RRadioSession::CancelGetPSByTA
// -----------------------------------------------------------------------------
//
EXPORT_C void RRadioSession::CancelGetPSByTA()
    {
    RADIO_RDEBUG(_L("[RADIO-SESS] CancelGetPSByTA()"));
    // GetPSByTA is not supported
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetProgrammeIdentification
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::GetProgrammeIdentification(
	TInt& aPi )
    {
 	RADIO_RDEBUG(_L("[RADIO-SESS] GetProgrammeIdentification()"));
	if ( iConnected )
		{
		TPckg<TInt> pi(aPi);
		return SendReceive(ERadioServGetProgrammeIdentification,
						   TIpcArgs(&iDestinationPckg, &pi));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetProgrammeType
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::GetProgrammeType(
	TRsRdsProgrammeType& aPty )
    {
  	RADIO_RDEBUG(_L("[RADIO-SESS] GetProgrammeType()"));
	if ( iConnected )
		{
		TPckg<TRsRdsProgrammeType> pty(aPty);
		return SendReceive(ERadioServGetProgrammeType,
						   TIpcArgs(&iDestinationPckg, &pty));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetProgrammeService
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::GetProgrammeService(
	TRsRdsPSName& aPs )
    {
 	RADIO_RDEBUG(_L("[RADIO-SESS] GetProgrammeService()"));
	if ( iConnected )
		{
		TPckg<TRsRdsPSName> ps(aPs);
		return SendReceive(ERadioServGetProgrammeService,
						   TIpcArgs(&iDestinationPckg, &ps));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetRadioText
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::GetRadioText(
	TRsRdsRadioText& aRt )
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] GetRadioText()"));
	if ( iConnected )
		{
		TPckg<TRsRdsRadioText> rt(aRt);
		return SendReceive(ERadioServGetRadioText,
						   TIpcArgs(&iDestinationPckg, &rt));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetRadioTextPlus
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::GetRadioTextPlus(
	TRsRdsRTplusClass /*aRtPlusClass*/,
	TRsRdsRadioText& /*aRtPlusData*/ )
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] GetRadioText()"));
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetClockTime
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::GetClockTime(
	TDateTime& aCt )
	{
    RADIO_RDEBUG(_L("[RADIO-SESS] GetClockTime()"));
	if ( iConnected )
		{
		TPckg<TDateTime> ct(aCt);
		return SendReceive(ERadioServGetClockTime,
						   TIpcArgs(&iDestinationPckg, &ct));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetTrafficAnnouncementStatus
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::GetTrafficAnnouncementStatus(
	TBool& aTaStatus )
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] GetTrafficAnnouncementStatus()"));
	if ( iConnected )
		{
		TPckg<TBool> taStatus(aTaStatus);
		return SendReceive(ERadioServGetTrafficAnnouncementStatus,
						   TIpcArgs(&iDestinationPckg, &taStatus));
 		}
 	else
 		{
 		return KErrDisconnected;
 		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::GetTrafficProgrammeStatus
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RRadioSession::GetTrafficProgrammeStatus(
	TBool& aTpStatus )
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] GetTrafficProgrammeStatus()"));
	if ( iConnected )
		{
		TPckg<TBool> tpStatus(aTpStatus);
		return SendReceive(ERadioServGetTrafficProgrammeStatus,
						   TIpcArgs(&iDestinationPckg, &tpStatus));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::CustomCommandSync
// -----------------------------------------------------------------------------
EXPORT_C TInt RRadioSession::CustomCommandSync(
	const TMMFMessageDestinationPckg& aDestination,
	TInt aFunction,
	const TDesC8& aDataTo1,
	const TDesC8& aDataTo2 )
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] CustomCommandSync NoReturn()"));
	if ( iConnected )
		{
		return SendReceive(aFunction, TIpcArgs(&aDestination, &aDataTo1, &aDataTo2));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::CustomCommandSync
// -----------------------------------------------------------------------------
EXPORT_C TInt RRadioSession::CustomCommandSync(
	const TMMFMessageDestinationPckg& aDestination,
	TInt aFunction,
	const TDesC8& aDataTo1,
	const TDesC8& aDataTo2,
	TDes8& aDataFrom )
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] CustomCommandSync Return()"));
	if ( iConnected )
		{
		return SendReceive(aFunction, TIpcArgs(&aDestination, &aDataTo1, &aDataTo2, &aDataFrom));
		}
	else
		{
		return KErrDisconnected;
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::CustomCommandAsync
// -----------------------------------------------------------------------------
EXPORT_C void RRadioSession::CustomCommandAsync(
	const TMMFMessageDestinationPckg& aDestination,
	TInt aFunction,
	const TDesC8& aDataTo1,
	const TDesC8& aDataTo2,
	TRequestStatus& aStatus )
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] CustomCommandAsync NoReturn()"));
	if ( iConnected )
		{
		SendReceive(aFunction, TIpcArgs(&aDestination, &aDataTo1, &aDataTo2), aStatus);
		}
	else
		{
		TRequestStatus* stat = &aStatus;
    	User::RequestComplete(stat, KErrDisconnected);
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::CustomCommandAsync
// -----------------------------------------------------------------------------
EXPORT_C void RRadioSession::CustomCommandAsync(
	const TMMFMessageDestinationPckg& aDestination,
	TInt aFunction,
	const TDesC8& aDataTo1,
	const TDesC8& aDataTo2,
	TDes8& aDataFrom,
	TRequestStatus& aStatus )
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] CustomCommandAsync Return()"));
	if ( iConnected )
		{
		SendReceive(aFunction, TIpcArgs(&aDestination, &aDataTo1, &aDataTo2, &aDataFrom), aStatus);
		}
	else
		{
		TRequestStatus* stat = &aStatus;
    	User::RequestComplete(stat, KErrDisconnected);
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::CancelRequest
// -----------------------------------------------------------------------------
//
void RRadioSession::CancelRequest(
	TInt aRequest )
	{
	RADIO_RDEBUG_INT(_L("[RADIO-SESS] CancelRequest(): [%d]"), aRequest);
	if ( iConnected )
		{
		SendReceive(ERadioServCancel, TIpcArgs(&iDestinationPckg, aRequest));
		}
	}

// -----------------------------------------------------------------------------
// RRadioSession::StartRequestHandlersL
// Create asynchronous request handlers.
// ** NOTE: The order in which each handler is appended to the iRequests array is critical.
// Access to each request is done with constants declared at the beginning of this file.
// -----------------------------------------------------------------------------
//
void RRadioSession::StartRequestHandlersL(
	MRadioObserver& aObserver )
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] StartRequestHandlersL()"));

	CRadioRequest* req = NULL;

	req = CRadioRequest::NewLC(*this, aObserver, ERadioServNone);
	User::LeaveIfError( iRequests.Append(req) );
	CleanupStack::Pop( req );

	req = CRadioRequest::NewLC(*this, aObserver, ERadioServRequestTunerControl);
	User::LeaveIfError( iRequests.Append(req) );
	CleanupStack::Pop( req );

	req = CRadioRequest::NewLC(*this, aObserver, ERadioServSetFrequencyRange);
	User::LeaveIfError( iRequests.Append(req) );
	CleanupStack::Pop( req );

	req = CRadioRequest::NewLC(*this, aObserver, ERadioServSetFrequency);
	User::LeaveIfError( iRequests.Append(req) );
	CleanupStack::Pop( req );

	req = CRadioRequest::NewLC(*this, aObserver, ERadioServStationSeek);
	User::LeaveIfError( iRequests.Append(req) );
	CleanupStack::Pop( req );

	req = CRadioRequest::NewLC(*this, aObserver, ERadioServPlay);
	User::LeaveIfError( iRequests.Append(req) );
	CleanupStack::Pop( req );

	req = CRadioRequest::NewLC(*this, aObserver, ERadioServStationSeekByPTY);
	User::LeaveIfError( iRequests.Append(req) );
	CleanupStack::Pop( req );

	req = CRadioRequest::NewLC(*this, aObserver, ERadioServStationSeekByTA);
	User::LeaveIfError( iRequests.Append(req) );
	CleanupStack::Pop( req );

	req = CRadioRequest::NewLC(*this, aObserver, ERadioServStationSeekByTP);
	User::LeaveIfError( iRequests.Append(req) );
	CleanupStack::Pop( req );
	}

// -----------------------------------------------------------------------------
// RRadioSession::StartEventHandlersL
// -----------------------------------------------------------------------------
//
void RRadioSession::StartEventHandlersL(
	MRadioObserver& aEventObserver )
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] StartEventHandlersL()"));

	CRadioEventHandler* handler = NULL;

	handler = CRadioEventHandler::NewLC(aEventObserver, *this, ERadioServPsPlayerState);
	User::LeaveIfError( iEventHandlers.Append(handler) );
	CleanupStack::Pop( handler );

	handler = CRadioEventHandler::NewLC(aEventObserver, *this, ERadioServPsAntennaStatus);
	User::LeaveIfError( iEventHandlers.Append(handler) );
	CleanupStack::Pop( handler );

	handler = CRadioEventHandler::NewLC(aEventObserver, *this, ERadioServPsOfflineMode );
	User::LeaveIfError( iEventHandlers.Append(handler) );
	CleanupStack::Pop( handler );

	handler = CRadioEventHandler::NewLC(aEventObserver, *this, ERadioServPsTransmitterStatus );
	User::LeaveIfError( iEventHandlers.Append(handler) );
	CleanupStack::Pop( handler );

	handler = CRadioEventHandler::NewLC(aEventObserver, *this, ERadioServPsFrequency);
	User::LeaveIfError( iEventHandlers.Append(handler) );
	CleanupStack::Pop( handler );

    handler = CRadioEventHandler::NewLC(aEventObserver, *this, ERadioServPsFrequencyRange);
	User::LeaveIfError( iEventHandlers.Append(handler) );
	CleanupStack::Pop( handler );

	handler = CRadioEventHandler::NewLC(aEventObserver, *this, ERadioServPsForceMonoReception);
	User::LeaveIfError( iEventHandlers.Append(handler) );
	CleanupStack::Pop( handler );

	handler = CRadioEventHandler::NewLC(aEventObserver, *this, ERadioServPsSquelch);
	User::LeaveIfError( iEventHandlers.Append(handler) );
	CleanupStack::Pop( handler );

	handler = CRadioEventHandler::NewLC(aEventObserver, *this, ERadioServPsVolume);
	User::LeaveIfError( iEventHandlers.Append(handler) );
	CleanupStack::Pop( handler );

	handler = CRadioEventHandler::NewLC(aEventObserver, *this, ERadioServPsMuteStatus);
	User::LeaveIfError( iEventHandlers.Append(handler) );
	CleanupStack::Pop( handler );

	handler = CRadioEventHandler::NewLC(aEventObserver, *this, ERadioServPsBalance);
	User::LeaveIfError( iEventHandlers.Append(handler) );
	CleanupStack::Pop( handler );
	}

// -----------------------------------------------------------------------------
// RRadioSession::StartRdsEventHandlersL
// -----------------------------------------------------------------------------
//
void RRadioSession::StartRdsEventHandlersL(
	TUint32 aRdsFunction )
	{
	RADIO_RDEBUG(_L("[RADIO-SESS] StartRdsEventHandlersL()"));

	iRdsEventHandlers.ResetAndDestroy();
	CRadioEventHandler* handler = NULL;

	// If RDS notification started always return the latest RdsSignalStatus
	handler = CRadioEventHandler::NewLC(*iObserver, *this, ERadioServPsRdsSignalStatus);
	User::LeaveIfError( iRdsEventHandlers.Append(handler) );
	CleanupStack::Pop( handler );

	handler = CRadioEventHandler::NewLC(*iObserver, *this, ERadioServPsAutoSwitchStatus);
	User::LeaveIfError( iRdsEventHandlers.Append(handler) );
	CleanupStack::Pop( handler );

    if (aRdsFunction & TRsRdsData::ERsRdsProgrammeIdentification)
    	{
    	handler = CRadioEventHandler::NewLC(*iObserver, *this, ERadioServPsProgrammeIdentification);
		User::LeaveIfError( iRdsEventHandlers.Append(handler) );
		CleanupStack::Pop( handler );
    	}

    if (aRdsFunction & TRsRdsData::ERsRdsProgrammeType)
    	{
    	handler = CRadioEventHandler::NewLC(*iObserver, *this,ERadioServPsProgrammeType );
		User::LeaveIfError( iRdsEventHandlers.Append(handler) );
		CleanupStack::Pop( handler );
    	}

	if (aRdsFunction & TRsRdsData::ERsRdsProgrammeService)
		{
		handler = CRadioEventHandler::NewLC(*iObserver, *this, ERadioServPsProgrammeService);
		User::LeaveIfError( iRdsEventHandlers.Append(handler) );
		CleanupStack::Pop( handler );
		}

	if(aRdsFunction & TRsRdsData::ERsRdsRadioText)
		{
		handler = CRadioEventHandler::NewLC(*iObserver, *this, ERadioServPsRadioText);
		User::LeaveIfError( iRdsEventHandlers.Append(handler) );
		CleanupStack::Pop( handler );
		}

  if(aRdsFunction & TRsRdsData::ERsRdsRadioTextPlus)
    {
    handler = CRadioEventHandler::NewLC(*iObserver, *this, ERadioServPsRadioTextPlusObjects);
    User::LeaveIfError( iRdsEventHandlers.Append(handler) );
    CleanupStack::Pop( handler );
    }

	if (aRdsFunction & TRsRdsData::ERsRdsClockTime)
		{
	    handler = CRadioEventHandler::NewLC(*iObserver, *this, ERadioServPsClockTime);
		User::LeaveIfError( iRdsEventHandlers.Append(handler) );
		CleanupStack::Pop( handler );
		}

	if (aRdsFunction & TRsRdsData::ERsRdsTrafficAnnouncement)
		{
		handler = CRadioEventHandler::NewLC(*iObserver, *this, ERadioServPsTrafficAnnouncementStatus);
		User::LeaveIfError( iRdsEventHandlers.Append(handler) );
		CleanupStack::Pop( handler );
		}

	if (aRdsFunction & TRsRdsData::ERsRdsAlternateFrequency)
		{
	    handler = CRadioEventHandler::NewLC(*iObserver, *this, ERadioServPsAfSearchBegin);
	    User::LeaveIfError( iRdsEventHandlers.Append(handler) );
		CleanupStack::Pop( handler );

	    handler = CRadioEventHandler::NewLC(*iObserver, *this, ERadioServPsAfSearchEnd);
	    User::LeaveIfError( iRdsEventHandlers.Append(handler) );
		CleanupStack::Pop( handler );
		}
	}

//  End of File
