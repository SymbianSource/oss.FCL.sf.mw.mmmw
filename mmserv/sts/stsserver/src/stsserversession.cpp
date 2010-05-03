/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * This method provides the generic Symbian specific message handling
 * for the server session.  The actual STS specific processing occurs
 * in the STS class.
 */

#include "stsserversession.h"
#include "stsserver.h"
#include "sts.h"

// CStsServerSession IMPLEMENTATION
CStsServerSession::CStsServerSession(CStsServer& aServer, CSts& aSts) :
    iServer(aServer), iSts(aSts)
    {
    }

CStsServerSession::~CStsServerSession()
    {
    iServer.DropSession(this);
    }

void CStsServerSession::CreateL()
    {
    iServer.AddSession(this);
    }

void CStsServerSession::ServiceL(const RMessage2& aMessage)
    {
    switch (aMessage.Function())
        {
        case StsMsg_PlayTone:
            DoPlayToneL(aMessage);
            break;
        case StsMsg_StopTone:
            DoStopToneL(aMessage);
            break;
        default:
            break;
        }
    }

void CStsServerSession::ServiceError(const RMessage2& aMessage, TInt aError)
    {
    CSession2::ServiceError(aMessage, aError);
    }

void CStsServerSession::DoPlayToneL(const RMessage2& aMessage)
    {
    CSystemToneService::TToneType toneType =
            (CSystemToneService::TToneType) aMessage.Int0();
    unsigned int context = 0;
    iSts.PlayTone(toneType, context);
    TPckg<unsigned int> contextPckg(context);
    TRAPD(err,aMessage.WriteL(1,contextPckg));
    aMessage.Complete(err);
    }

void CStsServerSession::DoStopToneL(const RMessage2& aMessage)
    {
    unsigned int context = aMessage.Int0();
    aMessage.Complete(KErrNone);
    iSts.StopTone(context);
    }
