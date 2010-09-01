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
* Description:  This file contains implementation of 
                    ProfileSettingsMonitorServerImpl.
*
*/


#include "ProfileSettingsMonitorServerImpl.h"
#include <AudioClientsListPSKeys.h>
#include <AudioClientsListPSData.h>
#include <MediaStreamPropertiesPSKeys.h>

#ifdef _DEBUG
#define DEB_TRACE0(str)             RDebug::Print(str)
#define DEB_TRACE1(str, val1)       RDebug::Print(str, val1)
#else
#define DEB_TRACE0(str)
#define DEB_TRACE1(str, val1)
#endif // _DEBUG


static _LIT_SECURITY_POLICY_PASS(KClientStateReadPolicy);
static _LIT_SECURITY_POLICY_S0(KMMFAudioServerSIDPolicy, KUidMmfAudioServerDllUnicodeDefine);


CProfileSettingsMonitorServerImpl::CProfileSettingsMonitorServerImpl()
	{
	}

CProfileSettingsMonitorServerImpl::~CProfileSettingsMonitorServerImpl()
	{
	iWarningTonesProperty.Close();
	RProperty::Delete(KGASPSUidGlobalAudioSettings, KGASWarningTones);
	
	iMessagingTonesProperty.Close();
	RProperty::Delete(KGASPSUidGlobalAudioSettings, KGASMessageTones);
	
	iSilentProfileProperty.Close();
	RProperty::Delete(KGASPSUidGlobalAudioSettings, KGASSilentProfile);
	
	iVibraProperty.Close();
	RProperty::Delete(KGASPSUidGlobalAudioSettings, KGASVibra);
	
	// Release profile engine resources
	if ( iEngine )
	    {
	    iEngine->Release();
	    }
	delete iEngine;
	delete iNotifyHandler;
	}

CProfileSettingsMonitorServerImpl* CProfileSettingsMonitorServerImpl::NewL()
	{
	CProfileSettingsMonitorServerImpl* s = new(ELeave) CProfileSettingsMonitorServerImpl();
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

void CProfileSettingsMonitorServerImpl::ConstructL()
	{
    iEngine = ProEngFactory::NewEngineL();
    iNotifyHandler = ProEngFactory::NewNotifyHandlerL();    
    User::LeaveIfError( iNotifyHandler->RequestProfileActivationNotificationsL( *this ));    
    User::LeaveIfError( iNotifyHandler->RequestActiveProfileNotificationsL( *this ));
    
    // Initialize iSilentProfile
    iActiveProfileId = iEngine->ActiveProfileId();
    if(iActiveProfileId == 1) 
        iSilentProfile = ETrue;
    else
        iSilentProfile = EFalse;
    
    // Initialize iWarningTones, iMessagingTones and iVibra
    MProEngProfile* currentProfile = iEngine->ActiveProfileL();
    //CleanupStack::PushL(currentProfile);
    MProEngToneSettings& settings = currentProfile->ToneSettings();
    iWarningTones = settings.WarningAndGameTones();
    iMessagingTones = !( currentProfile->IsSilent() );
    iVibra = settings.VibratingAlert();
    //CleanupStack::PopAndDestroy(currentProfile);
    
    // Get PublicSilence from CenRep
    TInt featureBitmask( 0 );     
    CRepository* profileCenRep = CRepository::NewL( KCRUidProfilesLV );
    CleanupStack::PushL(profileCenRep);
    User::LeaveIfError( profileCenRep->Get( KProfilesLVFlags, featureBitmask ) );
    if  ( featureBitmask & KProEngFeatureIdForceSilentMode )
        iPublicSilence = ETrue;
    else
        iPublicSilence = EFalse;
    CleanupStack::PopAndDestroy(profileCenRep);
    
    TSecurityPolicy read = TSecurityPolicy(ECapabilityWriteDeviceData);
    TSecurityPolicy write = TSecurityPolicy(ECapabilityMultimediaDD);

    // Define and Initialize Warning Tones PnS key
    User::LeaveIfError(RProperty::Define(
                                    KGASPSUidGlobalAudioSettings,
                                    KGASWarningTones,
                                    RProperty::EInt,
                                    read,
                                    write));	    
    User::LeaveIfError(iWarningTonesProperty.Attach(
                                                KGASPSUidGlobalAudioSettings,
                                                KGASWarningTones));
    User::LeaveIfError(iWarningTonesProperty.Set(
                                                KGASPSUidGlobalAudioSettings,
                                                KGASWarningTones,
                                                iWarningTones));
    
    // Define and Initialize Messaging Tones PnS key
    User::LeaveIfError(RProperty::Define(
                                    KGASPSUidGlobalAudioSettings,
                                    KGASMessageTones,
                                    RProperty::EInt,
                                    read,
                                    write));
    User::LeaveIfError(iMessagingTonesProperty.Attach(
                                                KGASPSUidGlobalAudioSettings,
                                                KGASMessageTones));
    User::LeaveIfError(iMessagingTonesProperty.Set(
                                                KGASPSUidGlobalAudioSettings,
                                                KGASMessageTones,
                                                iMessagingTones));
    
    // Define and Initialize Silent Profile PnS key
     User::LeaveIfError(RProperty::Define(
                                    KGASPSUidGlobalAudioSettings,
                                    KGASSilentProfile,
                                    RProperty::EInt,
                                    read,
                                    write));
    User::LeaveIfError(iSilentProfileProperty.Attach(
                                                KGASPSUidGlobalAudioSettings,
                                                KGASSilentProfile));
    User::LeaveIfError(iSilentProfileProperty.Set(
                                                KGASPSUidGlobalAudioSettings,
                                                KGASSilentProfile,
                                                iSilentProfile));
    
    // Define and Initialize Vibra PnS key
    User::LeaveIfError(RProperty::Define(
                                    KGASPSUidGlobalAudioSettings,
                                    KGASVibra,
                                    RProperty::EInt,
                                    read,
                                    write));
    User::LeaveIfError(iVibraProperty.Attach(
                                        KGASPSUidGlobalAudioSettings,
                                        KGASVibra));
    User::LeaveIfError(iVibraProperty.Set(
                                        KGASPSUidGlobalAudioSettings,
                                        KGASVibra,
                                        iVibra));
                        
    // Define and Initialize Public Silence PnS key
    User::LeaveIfError(RProperty::Define(
                                    KGASPSUidGlobalAudioSettings,
                                    KGASPublicSilence,
                                    RProperty::EInt,
                                    read,
                                    write));
    User::LeaveIfError(iPublicSilenceProperty.Attach(
                                                KGASPSUidGlobalAudioSettings,
                                                KGASPublicSilence));
    User::LeaveIfError(iPublicSilenceProperty.Set(
                                                KGASPSUidGlobalAudioSettings,
                                                KGASPublicSilence,
                                                iPublicSilence));

    //Stream Gain Control
    TInt status = RProperty::Define(
                                KPSUidMediaStreamProperties,
                                KMediaStreamCurrentGain,
                                RProperty::EInt,
                                read,
                                write);
    if ( status != KErrAlreadyExists )
	    User::LeaveIfError(status);

    // Audio Clients List
    InitializeAudioClientsListPSKeysL();
    }

void CProfileSettingsMonitorServerImpl::InitializeAudioClientsListPSKeysL()
    {
    DEB_TRACE0(_L("CProfileSettingsMonitorServerImpl::InitializeAudioClientsListPSKeysL:Enter"));
	// define KAudioPolicyApplicationAudioStatePlaying property to be a byte array
	TInt status = RProperty::Define(KPSUidMMFAudioServer,
	                                KAudioPolicyApplicationAudioStatePlaying,
	                                RProperty::EByteArray,
		                            KClientStateReadPolicy,
		                            KMMFAudioServerSIDPolicy,
		                            sizeof(struct TAudioPolicyProcessIdListStruct));
	DEB_TRACE1(_L("Define KAudioPolicyApplicationAudioStatePlaying returns %d"), status);
	if (status != KErrAlreadyExists)
	    {
	    User::LeaveIfError(status);
	    }

	// define KAudioPolicyApplicationAudioStateRecording property to be a byte array
	status = RProperty::Define(KPSUidMMFAudioServer,
	                           KAudioPolicyApplicationAudioStateRecording,
	                           RProperty::EByteArray,
	                           KClientStateReadPolicy,
	                           KMMFAudioServerSIDPolicy,
	                           sizeof(struct TAudioPolicyProcessIdListStruct));
	DEB_TRACE1(_L("Define KAudioPolicyApplicationAudioStateRecording returns %d"), status);
	if (status != KErrAlreadyExists)
	    {
	    User::LeaveIfError(status);
	    }

	// define KAudioPolicyApplicationAudioStatePaused property to be a byte array
	status = RProperty::Define(KPSUidMMFAudioServer,
	                           KAudioPolicyApplicationAudioStatePaused,
	                           RProperty::EByteArray,
	                           KClientStateReadPolicy,
	                           KMMFAudioServerSIDPolicy,
	                           sizeof(struct TAudioPolicyProcessIdListStruct));
	DEB_TRACE1(_L("Define KAudioPolicyApplicationAudioStatePaused returns %d"), status);
	if (status != KErrAlreadyExists)
    	{
    	User::LeaveIfError(status);
    	}

	// define KAudioPolicyForceMutedClients property to be a byte array
	status = RProperty::Define(KPSUidMMFAudioServer,
	                           KAudioPolicyForceMutedClients,
	                           RProperty::EByteArray,
	                           KClientStateReadPolicy,
	                           KMMFAudioServerSIDPolicy,
	                           sizeof(struct TAudioPolicyProcessIdListStruct));
	DEB_TRACE1(_L("Define KAudioPolicyForceMutedClients returns %d"), status);
	if (status != KErrAlreadyExists)
    	{
    	User::LeaveIfError(status);
    	}

	// define KAudioPolicyAudioClients property to be a byte array
	status = RProperty::Define(KPSUidMMFAudioServer,
	                           KAudioPolicyAudioClients,
	                           RProperty::EByteArray,
	                           KClientStateReadPolicy,
	                           KMMFAudioServerSIDPolicy,
	                           sizeof(struct TAudioClientListStruct));
	DEB_TRACE1(_L("Define KAudioPolicyAudioClients returns %d"), status);
	if (status != KErrAlreadyExists)
    	{
    	User::LeaveIfError(status);
    	}
    DEB_TRACE0(_L("CProfileSettingsMonitorServerImpl::InitializeAudioClientsListPSKeysL:Exit"));
    }

void CProfileSettingsMonitorServerImpl::HandleProfileActivatedL( TInt aProfileId )
    {
    DEB_TRACE1(_L("CProfileSettingsMonitorServerImpl::HandleProfileActivatedL[%d]"), aProfileId);
    
    if ( iActiveProfileId != aProfileId )
        {
        MProEngProfile* currentProfile = iEngine->ActiveProfileL();
        CleanupReleasePushL(*currentProfile);
        
        TBool silentProfile;
        iActiveProfileId = aProfileId;
        //iActiveProfileId = iEngine->ActiveProfileId();
        
        if(iActiveProfileId == 1) // Silent profile is always '1'
            silentProfile = ETrue;
        else
            silentProfile = EFalse;
        
        if(silentProfile != iSilentProfile)
            {
            iSilentProfile = silentProfile;
            User::LeaveIfError(iSilentProfileProperty.Set(
                                                        KGASPSUidGlobalAudioSettings,
                                                        KGASSilentProfile,
                                                        iSilentProfile));
            }
            
        MProEngToneSettings& settings = currentProfile->ToneSettings();
        
        if(iWarningTones != settings.WarningAndGameTones())
            {
            iWarningTones = settings.WarningAndGameTones();
            User::LeaveIfError(iWarningTonesProperty.Set(
                                                        KGASPSUidGlobalAudioSettings,
                                                        KGASWarningTones,
                                                        iWarningTones));
            }
        
        if(iMessagingTones != (!( currentProfile->IsSilent() )))
            {
            iMessagingTones = !( currentProfile->IsSilent() );
            User::LeaveIfError(iMessagingTonesProperty.Set(
                                                        KGASPSUidGlobalAudioSettings,
                                                        KGASMessageTones,
                                                        iMessagingTones));
            }
        if(iVibra != settings.VibratingAlert())
            {
            iVibra = settings.VibratingAlert();
            User::LeaveIfError(iVibraProperty.Set(
                                                KGASPSUidGlobalAudioSettings,
                                                KGASVibra,
                                                iVibra));
            }
        CleanupStack::PopAndDestroy(currentProfile);
        }
#ifdef _DEBUG
    else
        {
        RDebug::Print(_L("CProfileSettingsMonitorServerImpl::HandleProfileActivatedL[%d]- \
            Activated is same as current profile"), aProfileId);
        }
#endif // _DEBUG    
    }

void CProfileSettingsMonitorServerImpl::HandleProfileActivationNotificationError( TInt /*aError*/ )
    {
    DEB_TRACE0(_L("CMyProfileNameArrayObserver::HandleProfileActivationNotificationError"));
	}

void CProfileSettingsMonitorServerImpl::HandleActiveProfileModifiedL()
    {
    DEB_TRACE0(_L("CProfileSettingsMonitorServerImpl::HandleActiveProfileModifiedL"));
    
    iActiveProfileId = iEngine->ActiveProfileId();
    MProEngProfile* currentProfile = iEngine->ActiveProfileL();
    CleanupReleasePushL(*currentProfile);
    
    MProEngToneSettings& settings = currentProfile->ToneSettings();
    if(iWarningTones != settings.WarningAndGameTones())
        {
        iWarningTones = settings.WarningAndGameTones();
        User::LeaveIfError(iWarningTonesProperty.Set(
                                                    KGASPSUidGlobalAudioSettings,
                                                    KGASWarningTones,
                                                    iWarningTones));
        }
    if(iMessagingTones != (!( currentProfile->IsSilent() )))
        {
        iMessagingTones = !( currentProfile->IsSilent() );
        User::LeaveIfError(iMessagingTonesProperty.Set(
                                                    KGASPSUidGlobalAudioSettings,
                                                    KGASMessageTones,
                                                    iMessagingTones));
        }

    if(iVibra != settings.VibratingAlert())
        {
        iVibra = settings.VibratingAlert();
        User::LeaveIfError(iVibraProperty.Set(
                                            KGASPSUidGlobalAudioSettings,
                                            KGASVibra,
                                            iVibra));
        }
    CleanupStack::PopAndDestroy(currentProfile);
    }

void CProfileSettingsMonitorServerImpl::HandleActiveProfileNotificationError( TInt /*aError*/ )
    {
    DEB_TRACE0(_L("CMyProfileNameArrayObserver::HandleProfileActivationNotificationError"));
	}

// End of file
	
