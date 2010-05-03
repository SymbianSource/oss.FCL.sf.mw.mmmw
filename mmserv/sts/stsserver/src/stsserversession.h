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
 * This file defines the interface for the STS server side session.
 */

#ifndef STSSERVERSESSION_H_
#define STSSERVERSESSION_H_

#include <e32base.h>
#include <e32msgqueue.h>

#include "stsclientservercommon.h"

class CSts;
class CStsServer;

class CStsServerSession : public CSession2
    {
public:

    CStsServerSession(CStsServer& aServer, CSts& aSts);

    virtual ~CStsServerSession();

    // inherited from CSession2
    void CreateL();
    void ServiceL(const RMessage2& aMessage);
    void ServiceError(const RMessage2& aMessage, TInt aError);

private:

    void DoPlayToneL(const RMessage2& aMessage);
    void DoStopToneL(const RMessage2& aMessage);

    CStsServer& iServer;
    CSts& iSts;

    };

#endif // STSSERVERSESSION_H_
