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
 * Description: Telephony Multimedia Service
 *
 */

// INCLUDE FILES
#include "tmsdtmfnotifier.h"
#include "tmsclientserver.h"
#include "tmsutility.h"

using namespace TMS;

// CONSTANTS
_LIT_SECURITY_POLICY_PASS(KTMSServerReadPolicy);
_LIT_SECURITY_POLICY_C1(KTMSServerWritePolicy, ECapabilityWriteUserData);

// -----------------------------------------------------------------------------
// TMSDtmfNotifier::TMSDtmfNotifier
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TMSDtmfNotifier::TMSDtmfNotifier()
    {
    }

// -----------------------------------------------------------------------------
// TMSDtmfNotifier::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void TMSDtmfNotifier::ConstructL()
    {
    TRACE_PRN_FN_ENT;
    TInt err = RProperty::Define(EDtmfPs, RProperty::EByteArray,
            KTMSServerReadPolicy, KTMSServerWritePolicy);
    TRACE_PRN_N1(_L("DtmfSetting::ConstructL err:%d"),err);

    if (err != KErrAlreadyExists)
        {
        User::LeaveIfError(err);
        }
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSDtmfNotifier::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
TMSDtmfNotifier* TMSDtmfNotifier::NewL()
    {
    TMSDtmfNotifier* self = new (ELeave) TMSDtmfNotifier();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
TMSDtmfNotifier::~TMSDtmfNotifier()
    {
    TRACE_PRN_FN_ENT;
    TInt err = RProperty::Delete(KTMSPropertyCategory, EDtmfPs);
    TRACE_PRN_N1(_L("DtmfSetting::~TMSDtmfNotifier err:%d"),err);
    TRACE_PRN_FN_ENT;
    }

// -----------------------------------------------------------------------------
// TMSDtmfNotifier::SetVolume
// -----------------------------------------------------------------------------
//
void TMSDtmfNotifier::SetDtmf(TmsMsgBufPckg dtmfpckg, TBool aPublish)
    {
    TRACE_PRN_FN_ENT;
    //TInt err(KErrNone);
    if (aPublish)
        {
        /*err =*/RProperty::Set(KTMSPropertyCategory, EDtmfPs, dtmfpckg);
        }

    //TRACE_PRN_N1(_L("DtmfSetting::SetDtmf err:%d"),err);
    TRACE_PRN_FN_EXT;
    }

//  End of File
