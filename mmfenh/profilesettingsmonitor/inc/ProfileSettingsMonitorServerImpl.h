/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains definitions of ProfileSettingsMonitorServerImpl.
*
*/


#ifndef __PROFILESETTINGSMONITORSERVERIMPL_H__
#define __PROFILESETTINGSMONITORSERVERIMPL_H__

#include <e32base.h>
#include <e32std.h>
#include <e32debug.h>
#include <Profile.hrh>
#include <ProEngFactory.h>
#include <MProEngEngine.h>
#include <MProEngProfile.h>
#include <MProEngActiveProfileObserver.h>
#include <MProEngProfileActivationObserver.h>
#include <MProEngNotifyHandler.h>
#include <MProEngToneSettings.h>
#include <e32property.h>
#include <GlobalAudioSettingsPSKeys.h>
#include <centralrepository.h>
#include <ProfileEngineDomainCRKeys.h>
#include <ProfilesVariant.hrh>
#include <e32capability.h>
class CProfileSettingsMonitorServerImpl : 
public CBase, 
public MProEngProfileActivationObserver, 
public MProEngActiveProfileObserver
	{
public:
	static CProfileSettingsMonitorServerImpl* NewL(); 
	~CProfileSettingsMonitorServerImpl();
	
	//MProEngProfileActivationObserver
	void HandleProfileActivatedL( TInt aProfileId );
    void HandleProfileActivationNotificationError(
                 TInt aError );
                 
    //MProEngActiveProfileObserver
    void HandleActiveProfileModifiedL();
    void HandleActiveProfileNotificationError(
                TInt aError );

private:
	CProfileSettingsMonitorServerImpl();
	void ConstructL();
	
	// Initializes publish and subscribe keys used for
	// audio clients list implementation
	void InitializeAudioClientsListPSKeysL();

private:
    MProEngEngine* iEngine;
    MProEngNotifyHandler* iNotifyHandler;
    TInt iActiveProfileId;
    TBool iWarningTones;
    TBool iMessagingTones;
    TBool iSilentProfile;
    TBool iVibra;
    TBool iPublicSilence;
    RProperty iWarningTonesProperty;
    RProperty iMessagingTonesProperty;
    RProperty iSilentProfileProperty;
    RProperty iVibraProperty;
    RProperty iPublicSilenceProperty;
	};

#endif

// End of file
