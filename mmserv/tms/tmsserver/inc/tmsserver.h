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

#ifndef __TMSSERVER_H
#define __TMSSERVER_H

#include <e32base.h>
#include <mmcccodecinformation.h>
#include <tms.h>
#include "tmsshared.h"
#include "tmsclientserver.h"

namespace TMS {

// FORWARD DECLARATIONS
class TMSServerShutDown;
class TMSStartAndMonitorTMSCallThread;
class TMSCallProxyLocal;
class TMSGlobalEffectsSettings;
class TMSTarEventHandler;
class TMSCSPCenRepListener;
class TMSCenRepAudioHandler;

// -----------------------------------------------------------------------------
// TMSServer class
// -----------------------------------------------------------------------------
//
class TMSServer : public CServer2
    {
public:
    static TMSServer* NewL();
    ~TMSServer();

    void AddSession();
    void DropSession();
    TInt SessionCount();

    void SetDnLinkSession(TBool aSession);
    void SetUpLinkSession(TBool aSession);

    TBool HasDnLinkSession() const;
    TBool HasUpLinkSession() const;

    void GetNewTMSCallSessionHandleL(RHandleBase& aHandle);
    TInt SetOutput(CSession2* sid, TInt output);
    TInt GetOutput(const RMessage2& aMessage);
    TInt GetPreviousOutput(const RMessage2& aMessage);
    TInt GetAvailableOutputs(const RMessage2& aMessage);

    TInt GetMaxLevel(const RMessage2& aMessage);
    TInt GetLevel(const RMessage2& aMessage);
    TInt SetLevel(CSession2* sid, TBool tmsclient, TInt level);

    TInt GetMaxGain(const RMessage2& aMessage);
    TInt GetGain(const RMessage2& aMessage);
    TInt SetGain(CSession2* sid, TInt level);
    TInt GetSupportedCodecs(const TMSStreamType strmType,
            RArray<TFourCC>*& aCodecs);

    TInt NotifyTarClients(TRoutingMsgBufPckg routingpckg);

    void StartRoutingNotifierL();
    void CancelRoutingNotifier();
    void StartCenRepHandlerL();
    void CancelCenRepHandler();

private:
    TMSServer();
    void ConstructL();
    CSession2* NewSessionL(const TVersion& aVersion,
            const RMessage2& aMessage) const;

    TInt StartTMSCallServer(TMSCallProxyLocal& aHandle);
    void GetAvailableOutputsL(const RMessage2& aMessage);
    TInt SendMessageToCallServ(TInt func, TInt value);
    TInt SendMessageToCallServ(TInt func, TIpcArgs args);

private:
    TInt iSession;
    TMSServerShutDown *iShutdownTimer;

    TBool iDnlinkSession;
    TBool iUplinkSession;

    mutable RPointerArray<TMSStartAndMonitorTMSCallThread> iTMSCallServList;
    TMSGlobalEffectsSettings* iEffectSettings;
    TMSTarEventHandler* iTarHandler;
    TMSCenRepAudioHandler* iAudioCenRepHandler;
    TMSAudioOutput iCurrentRouting;

    RArray<TFourCC> iDnlCodecs;
    RArray<TFourCC> iUplCodecs;
    };

// -----------------------------------------------------------------------------
// TMSStartAndMonitorTMSCallThread class
// -----------------------------------------------------------------------------
//
class TMSCallProxyLocal : public RSessionBase
    {
public:
    TInt Open(RServer2& aTMSCallServerHandle);
    TInt SendToCallServer(TInt aFunc, TUint value);
    TInt SendToCallServer(TInt aFunc, TIpcArgs args);
    TInt ReceiveFromCallServer(TInt aFunc, TUint& value);
    TInt ReceiveFromCallServer(TInt aFunc, TIpcArgs args);
    };

// -----------------------------------------------------------------------------
// TMSStartAndMonitorTMSCallThread class
// -----------------------------------------------------------------------------
//
class TMSStartAndMonitorTMSCallThread : public CActive
    {
public:
    static TMSStartAndMonitorTMSCallThread* NewL(TMSServer* aServer);
    ~TMSStartAndMonitorTMSCallThread();
    TInt StartTMSCallServer(TMSCallProxyLocal& aHandle);

private:
    // Construct
    TMSStartAndMonitorTMSCallThread(TMSServer* aServer);
    void ConstructL();

    // From CActive
    void RunL();
    void DoCancel();

public:
    RServer2 iCallSrvrHndl;
    TMSCallProxyLocal iTMSCallProxyLocal;

private:
    TMSServer* iTMSServer;
    RThread iServerThread;
    };

} //namespace TMS

#endif //__TMSSERVER_H

// End of file
