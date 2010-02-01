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
#include "tarsettings.h"
#include "tmsclientserver.h"
#include "tmsutility.h"

using namespace TMS;

// CONSTANTS
_LIT_SECURITY_POLICY_PASS(KTMSServerReadPolicy);
_LIT_SECURITY_POLICY_C1(KTMSServerWritePolicy, ECapabilityWriteUserData);

// -----------------------------------------------------------------------------
// TarSettings::TarSettings
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TarSettings::TarSettings()
    {
    }

// -----------------------------------------------------------------------------
// TarSettings::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void TarSettings::ConstructL()
    {
    TRACE_PRN_FN_ENT;
    TInt err = RProperty::Define(ERoutingPs, RProperty::EByteArray,
            KTMSServerReadPolicy, KTMSServerWritePolicy);
    TRACE_PRN_N1(_L("TarSetting::ConstructL err:%d"),err);

    if (err != KErrAlreadyExists)
        {
        User::LeaveIfError(err);
        }
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TarSettings::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
TarSettings* TarSettings::NewL()
    {
    TarSettings* self = new (ELeave) TarSettings();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
TarSettings::~TarSettings()
    {
    TRACE_PRN_FN_ENT;
    TInt err = RProperty::Delete(KTMSPropertyCategory, ERoutingPs);
    TRACE_PRN_N1(_L("TarSetting::~TarSettings err:%d"),err);
    TRACE_PRN_FN_ENT;
    }

// -----------------------------------------------------------------------------
// TarSettings::SetVolume
// -----------------------------------------------------------------------------
//
void TarSettings::SetTar(TRoutingMsgBufPckg rountingpckg, TBool aPublish)
    {
    TRACE_PRN_FN_ENT;
    //    TInt err(KErrNone);
    if (aPublish)
        {
        /*err =*/RProperty::Set(KTMSPropertyCategory, ERoutingPs, rountingpckg);
        }

    //TRACE_PRN_N1(_L("TarSetting::SetTar err:%d"),err);
    TRACE_PRN_FN_EXT;
    }

//  End of File
