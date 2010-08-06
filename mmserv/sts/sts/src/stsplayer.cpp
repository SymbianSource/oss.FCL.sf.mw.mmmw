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
#include <systemtoneservice.h>



/*static*/CStsPlayer* CStsPlayer::CreateTonePlayer(
        MStsPlayerObserver& aObserver,CSystemToneService::TToneType aTone,
       	unsigned int aContext, const TDesC& aFileName, TInt aVolume,
       	TUint aAudioPreference, TUint  aAudioPriority)
    {
    CStsPlayer* self = 0;
    
    self = new CStsPlayer(aObserver, aFileName, 0, aContext, aVolume, aAudioPreference, aAudioPriority);
    
           	        
    		if (self != 0)
         {
            bool successful = self->Init();
            if (!successful)
            {
               delete self;
               self = 0;
            }
          }
        
    return self;
    }

/*static*/CStsPlayer* CStsPlayer::CreateAlarmPlayer(
        MStsPlayerObserver& aObserver, CSystemToneService::TAlarmType aAlarm,
        unsigned int aContext, const TDesC& aFileName, TInt aVolume,
       	TUint aAudioPreference, TUint  aAudioPriority)
    {
    CStsPlayer* self = 0;
   
   	
   	self = new CStsPlayer(aObserver, aFileName, 10, aContext, aVolume, aAudioPreference, aAudioPriority);
   	
     
    	if (self != 0)
      	  {
        	  bool successful = self->Init();
          	if (!successful)
            {
             	  delete self;
               	self = 0;
            }	
        	}
        
    return self;
    }





CStsPlayer::CStsPlayer(MStsPlayerObserver& aObserver, const TDesC& aFileName,
        int aRepeatNumberOfTimes, unsigned int aContext, TInt aVolume, TUint aAudioPreference, TUint aAudioPriority ) :
    iObserver(aObserver), iPlayer(0), iFileName(aFileName),
            iRepeatNumberOfTimes(aRepeatNumberOfTimes), iContext(aContext), iVolume(aVolume),
            iAudioPreference(aAudioPreference), iAudioPriority(aAudioPriority)
    {
    	
    }

bool CStsPlayer::Init()
    {

    
        TRAPD(result, iPlayer = CMdaAudioPlayerUtility::NewL(*this));
        return result == KErrNone;
    
    }


CStsPlayer::~CStsPlayer()
    {
    	
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
