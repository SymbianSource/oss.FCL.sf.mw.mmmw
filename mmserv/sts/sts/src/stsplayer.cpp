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
 * This file provides the implementation for creating and deleting a
 * an MMF based player for playing and stopping a tone playback.
 */
#include "stsplayer.h"
#include <AudioPreference.h>
#include <MProEngEngine.h>
#include <MProEngProfile.h>
#include <MProEngTones.h>
#include <MProEngToneSettings.h>
#include <ProEngFactory.h>
#include <systemtoneservice.h>


_LIT(KDefaultFile,"z:\\data\\sounds\\digital\\clock.aac");





/*static*/CStsPlayer* CStsPlayer::CreateTonePlayer(
        MStsPlayerObserver& aObserver, CSystemToneService::TToneType aTone,
        unsigned int aContext)
    {
    CStsPlayer* self = 0;
     
    self = new CStsPlayer(aObserver, KDefaultFile, 0, aContext);
    
    if( self != 0)
    	{
    		TRAPD(err,self->LoadActiveProfileSettingsL());
             
    		if( err == KErrNone )
       		self->SetToneSettings(aTone);
    		else 
        {
          delete self;
          self = 0;
         }
        	        
    		if (self != 0)
         {
            bool successful = self->Init();
            if (!successful)
            {
               delete self;
               self = 0;
            }
          }
        }
    return self;
    }

/*static*/CStsPlayer* CStsPlayer::CreateAlarmPlayer(
        MStsPlayerObserver& aObserver, CSystemToneService::TAlarmType aAlarm,
        unsigned int aContext)
    {
    CStsPlayer* self = 0;
   	
   	self = new CStsPlayer(aObserver, KDefaultFile, 10, aContext);
   	
   	if( self != 0)
    	{		
   			TRAPD(err,self->LoadActiveProfileSettingsL());
   			
   			if( err == KErrNone )
   	    	self->SetAlarmSettings(aAlarm); 
   			else 
   	  	 {
   	    	 delete self;
   	     	 self = 0;
   	   		}           
   
    	if (self != 0)
      	  {
        	  bool successful = self->Init();
          	if (!successful)
            {
             	  delete self;
               	self = 0;
            }	
        	}
        }
    return self;
    }

void CStsPlayer::LoadActiveProfileSettingsL()
	{
		if(!iEngine)
		   	{
		   		iEngine =  ProEngFactory::NewEngineL();
        	}
		
        if (iEngine)
            iProfile = iEngine->ActiveProfileL();
	    
	    if (iProfile)
	        {
             MProEngToneSettings& settings  = iProfile->ToneSettings();
             iWarningToneEnabled = settings.WarningAndGameTones();
	        }
     
     
     
     
  }

void CStsPlayer::SetToneSettings(CSystemToneService::TToneType aTone)
	{
		
    if(iProfile)
    
    {
    	MProEngTones&         tones = iProfile->ProfileTones();
		
			switch (aTone)
				{
				case CSystemToneService::EEmailAlert:
				    iFileName.Set(tones.EmailAlertTone());
				    iAudioPreference = KAudioPrefNewSMS;
				    iAudioPriority = KAudioPriorityRecvMsg ;
				    break;
				case CSystemToneService::ESmsAlert:
				    iFileName.Set(tones.MessageAlertTone());
				    iAudioPreference = KAudioPrefNewSMS;
				    iAudioPriority = KAudioPriorityRecvMsg;
				    break; 
				case CSystemToneService::EMmsAlert:
				    iFileName.Set(tones.MessageAlertTone());
				    iAudioPreference = KAudioPrefNewSMS;
				    iAudioPriority = KAudioPriorityRecvMsg;
				    break;
				case CSystemToneService::EChatAlert:
				    iFileName.Set(tones.MessageAlertTone());
				    iAudioPreference = KAudioPrefNewSMS ;
				    iAudioPriority = KAudioPriorityRecvMsg;
				    break; 
				case CSystemToneService::EWarningBeep:
				    iAudioPreference = KAudioPrefWarning;
				    iAudioPriority =  KAudioPriorityWarningTone ;
				    if (iWarningToneEnabled)
				        iFileName.Set(KDefaultFile);
				    else
				        iVolume = 0;
				        
				    break;
				default:
					iAudioPreference = KAudioPrefDefaultTone;
				    iAudioPriority = KAudioPriorityWarningTone ;
				    break;
				}
			}
			
		else
		{			 
				 iAudioPreference = KAudioPrefDefaultTone;
				 iAudioPriority = KAudioPriorityWarningTone ;
		}
		
	}	
	
	
void CStsPlayer::SetAlarmSettings(CSystemToneService::TAlarmType aAlarm)
	{
		if(iProfile)
    
   		 {
    		MProEngTones&         tones = iProfile->ProfileTones();
		 
		 	switch (aAlarm)
        	{
        	case CSystemToneService::EIncomingCall:
            	iFileName.Set(tones.RingingTone1());
            	iAudioPreference = KAudioPrefIncomingCall ;
				iAudioPriority = KAudioPriorityRingingTone ;
            	break;
       		case CSystemToneService::EIncomingCallLine2:
            	iFileName.Set(tones.RingingTone2());
            	iAudioPreference = KAudioPrefIncomingCall;
				iAudioPriority = KAudioPriorityRingingTone ;
            	break;  
            case CSystemToneService::EIncomingDataCall:
            	iFileName.Set(tones.VideoCallRingingTone());
            	iAudioPreference = KAudioPrefIncomingDataCall;
		    	iAudioPriority = KAudioPriorityRealOnePlayer;
            	break;
        	default:
        		iAudioPreference = KAudioPrefAlarmClock  ;
				iAudioPriority = KAudioPriorityAlarm;
            	break;
       		}
      	}
      else
		{			 
				 iAudioPreference = KAudioPrefDefaultTone;
				 iAudioPriority = KAudioPriorityWarningTone ;
		}  
	}	




CStsPlayer::CStsPlayer(MStsPlayerObserver& aObserver, const TDesC& aFileName,
        int aRepeatNumberOfTimes, unsigned int aContext) :
    iObserver(aObserver), iPlayer(0), iFileName(aFileName),
            iRepeatNumberOfTimes(aRepeatNumberOfTimes), iContext(aContext)
    {
    	iEngine =  ProEngFactory::NewEngineL();
    }

bool CStsPlayer::Init()
    {

    
        TRAPD(result, iPlayer = CMdaAudioPlayerUtility::NewL(*this));
        return result == KErrNone;
    
    }


CStsPlayer::~CStsPlayer()
    {
    	if (iProfile)
        {
        iProfile->Release();
        }
    	if (iEngine)
        {
        iEngine->Release();
        }
        
    delete iPlayer; 
    
    }

void CStsPlayer::Play()
    {
    // Play the tone
    TRAPD(err, iPlayer->OpenFileL(iFileName));

    // If there is an error, indicate that the playback is complete. 
    if (err)
        {
        //TODO: Add trace here
        iObserver.PlayComplete(iContext);
        }
    }

void CStsPlayer::Stop()
    {
    iPlayer->Stop();
    }

void CStsPlayer::MapcInitComplete(TInt aError,
        const TTimeIntervalMicroSeconds& /*aDuration*/)
    {
    if (aError == KErrNone)
        {
        TTimeIntervalMicroSeconds delay = 0;
        // Set PriorityPref
        iPlayer->SetPriority(iAudioPriority,iAudioPreference);
        iPlayer->SetRepeats(iRepeatNumberOfTimes, delay);
        iPlayer->Play();
        }
    else
        {
        //TODO: add trace
        // Since there is an error, indicate that the playback is complete
        RDebug::Print(_L("InitCompleteReturned"), aError);
        iObserver.PlayComplete(iContext);
        }
    }

void CStsPlayer::MapcPlayComplete(TInt aError)
    {
    if (aError != KErrNone)
        {
        //TODO: add trace
        }
    iObserver.PlayComplete(iContext);
    }
