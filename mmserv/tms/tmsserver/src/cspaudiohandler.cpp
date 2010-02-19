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
 * Description:  Implements the class TMSCenRepAudioHandler
 *
 */

#include <e32base.h>
#include "tmstelephonycenrep.h"

#ifdef _USE_TELEPHONY_CENREP_
#include <telmicmutestatuspskeys.h>
#include <telincallvolcntrlcrkeys.h>
#else
const TUid KCRUidInCallVolume = {0x102828B1};
const TUint32 KTelIncallEarVolume  = 0x00000001;
const TUint32 KTelIncallLoudspeakerVolume  = 0x00000002;
#endif

#include "cspaudiohandler.h"
#include "cspcenreplistener.h"
#include "tmsutility.h"

using namespace TMS;
// ---------------------------------------------------------------------------
// TMSCenRepAudioHandler::NewL.
// ---------------------------------------------------------------------------
//
TMSCenRepAudioHandler* TMSCenRepAudioHandler::NewL(TMSServer* aServer)
    {
    TRACE_PRN_FN_ENT;
    TMSCenRepAudioHandler* self = new (ELeave) TMSCenRepAudioHandler(aServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    TRACE_PRN_FN_EXT;
    return self;
    }

// ---------------------------------------------------------------------------
// Destructs the object by canceling first ongoing monitoring.
// ---------------------------------------------------------------------------
//
TMSCenRepAudioHandler::~TMSCenRepAudioHandler()
    {
    TRACE_PRN_FN_ENT;
    delete iIncallLoudspeakerVolumeListener;
    delete iIncallEarVolumeListener;
    TRACE_PRN_FN_EXT;
    }

// ---------------------------------------------------------------------------
// TMSCenRepAudioHandler::SetLoudSpeakerVol
// ---------------------------------------------------------------------------
//
void TMSCenRepAudioHandler::SetLoudSpeakerVol(TInt vol)
    {
    if (iIncallLoudspeakerVolumeListener)
        {
        iIncallLoudspeakerVolumeListener->Set(vol);
        }
    }

// ---------------------------------------------------------------------------
// TMSCenRepAudioHandler::SetEarPieceVol
// ---------------------------------------------------------------------------
//
void TMSCenRepAudioHandler::SetEarPieceVol(TInt vol)
    {
    if (iIncallEarVolumeListener)
        {
        iIncallEarVolumeListener->Set(vol);
        }
    }

// ---------------------------------------------------------------------------
// From MCSPCenRepObserver
// TMSCenRepAudioHandler::HandleNotifyCenRepL
// ---------------------------------------------------------------------------
//
void TMSCenRepAudioHandler::HandleNotifyCenRepL(const TUid /*aUid*/,
        const TUint32 aKey, TInt aVal)
    {
    TRACE_PRN_FN_ENT;
    if (aKey == KTelIncallLoudspeakerVolume)
        {
        iTMSSer->SetLevel(NULL,FALSE, aVal);
        }
    else if (aKey == KTelIncallEarVolume)
        {
        iTMSSer->SetLevel(NULL,FALSE, aVal);
        }
    TRACE_PRN_FN_EXT;
    }

// ---------------------------------------------------------------------------
// Constructs the monitor.
// ---------------------------------------------------------------------------
//
TMSCenRepAudioHandler::TMSCenRepAudioHandler(TMSServer* aServer) :
    iTMSSer(aServer)
    {
    TRACE_PRN_FN_ENT;
    iCallCount = 0; // Active calls count
    TRACE_PRN_FN_EXT;
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void TMSCenRepAudioHandler::ConstructL()
    {
    TRACE_PRN_FN_ENT;

    iIncallLoudspeakerVolumeListener = TMSCenRepListener::NewL(
            KCRUidInCallVolume, KTelIncallLoudspeakerVolume, this);

    iIncallEarVolumeListener = TMSCenRepListener::NewL(KCRUidInCallVolume,
            KTelIncallEarVolume, this);

    // Initialize audio volumes
    TInt volEar;
    TInt volLoud;

    TInt volGetRes = iIncallEarVolumeListener->Get(volEar);
    volGetRes = iIncallLoudspeakerVolumeListener->Get(volLoud);

    TRACE_PRN_FN_EXT;
    }

// End of file
