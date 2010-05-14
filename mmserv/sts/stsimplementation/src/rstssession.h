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
 * This file defines the interface to the client side session of the
 * STS Server.
 */

#ifndef RSTSSESSION_H_
#define RSTSSESSION_H_

#include <e32msgqueue.h>
#include <systemtoneservice.h>

#include "stsclientservercommon.h"

class RStsSession : public RSessionBase
    {
public:

    TInt Connect();

    void Close();

    TInt SendPlayTone(CSystemToneService::TToneType aTone);

    TInt SendPlayAlarm(CSystemToneService::TAlarmType aAlarm,
            unsigned int& aAlarmContext, MStsPlayAlarmObserver& aObserver);

    TInt SendStopAlarm(unsigned int aAlarmContext);

private:

    static TInt CallBackThreadMain(TAny* aSession);
    void RunThreadL();
    
    TInt StartMsgQueue();
    TInt StartServer();
    TInt StartThread();

    RThread iThread;
    RMsgQueue<TStsCallBack> iMsgQueue;
    };

#endif // RSTSSESSION_H_
