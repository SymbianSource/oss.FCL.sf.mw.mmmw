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
* Description:
*
*/

#include "cmmfradiobackendengine.h"

extern "C" {
#include "xaradioitfadaptation.h"
#include "xanokiavolumeextitfadaptationmmf.h"
#include "xanokialinearvolumeitfadaptationmmf.h"
#include "xaplayitfadaptationmmf.h"
}

CMMFRadioBackendEngine *CMMFRadioBackendEngine::s_instance = 0;

CMMFRadioBackendEngine* CMMFRadioBackendEngine::Instance()
{
	if (!s_instance)
	{	
		s_instance = new CMMFRadioBackendEngine();
		if (s_instance)
		{
			s_instance->ConstructL();
		}
	}
	return s_instance;
}

void CMMFRadioBackendEngine::DeleteInstance()
{
	if (iFmTunerUtility)
  {
  	iFmTunerUtility->Close();
  	iFmTunerUtility = NULL;
  }	
  if (iRadioPlayerUtility)
  {
  	iRadioPlayerUtility->Close();
  	iRadioPlayerUtility = NULL;
  }
  
  if (iRadioUtility)
  {
		delete iRadioUtility;
		iRadioUtility = NULL;
	}
		
}

CMMFRadioBackendEngine::~CMMFRadioBackendEngine()
{
}
    
CMMFRadioBackendEngine::CMMFRadioBackendEngine()
{
	
}

void CMMFRadioBackendEngine::ConstructL()
{		
 	iRadioUtility = CRadioUtility::NewL( ETrue );
 	if (iRadioUtility)
 	{		
 		iFmTunerUtility = &iRadioUtility->RadioFmTunerUtilityL( *this );
 	}
 	if (iFmTunerUtility)
 	{
 		iFmTunerUtility->EnableTunerInOfflineMode( ETrue );
 		iFmTunerUtility->RequestTunerControl();  
 	} 
  if (iRadioUtility)
  {  
 		iRadioPlayerUtility = &iRadioUtility->RadioPlayerUtilityL( *this ); 
 	}   
 	 
//	SetForceMonoFlag();
	iDefaultFrequency = RADIO_DEFAULT_FREQ;
	iDefaultFreqRange = (TFmRadioFrequencyRange)RADIO_DEFAULT_FREQ_RANGE;  
	iDefaultMinFreq = RADIO_DEFAULT_MIN_FREQ;
	iDefaultMaxFreq = RADIO_DEFAULT_MAX_FREQ;
	 	 
}
void CMMFRadioBackendEngine::StationSeek(XAboolean aUpwards)
{
	if (iFmTunerUtility)
 	{
  	iFmTunerUtility->StationSeek(aUpwards);
 	}	
}

void CMMFRadioBackendEngine::SetFrequency(TInt aFreq)
{
  DEBUG_API_A1("CMMFRadioBackendEngine::SetFrequency: %d", aFreq);	
	if (iFmTunerUtility)
 	{
  	iFmTunerUtility->SetFrequency(aFreq);
 	}	
}

TInt CMMFRadioBackendEngine::GetFrequency(TInt& aFreq)
{
  DEBUG_API("CMMFRadioBackendEngine::GetFrequency");	
	TInt ret = KErrNotFound;
	if (iFmTunerUtility)
	{
  	ret = iFmTunerUtility->GetFrequency(aFreq);
  	if (ret != KErrNone)
  	{
  		aFreq = iDefaultFrequency;
  		ret = KErrNone;
  	}
 	}	
  DEBUG_API_A1("CMMFRadioBackendEngine::GetFrequency RET: %d", ret);	 
//  RDebug::Print(_L("CMMFRadioBackendEngine::GetFrequency RET: %d"), ret);	 
 	return ret; 	
}

TInt CMMFRadioBackendEngine::GetSignalStrength(TInt& aSignalStrength)
{
	TInt ret = KErrNotFound;
 	if (iFmTunerUtility)
	{
  	ret = iFmTunerUtility->GetSignalStrength(aSignalStrength);
 	}	
  DEBUG_API_A1("CMMFRadioBackendEngine::GetSignalStrength RET: %d", ret);	
//  RDebug::Print(_L("CMMFRadioBackendEngine::GetSignalStrength RET: %d"), ret);	    	
	return ret;
}

void CMMFRadioBackendEngine::CancelSetFrequency()
{
	if (iFmTunerUtility)
 	{
 		iFmTunerUtility->CancelSetFrequency();
 	}	
}

void CMMFRadioBackendEngine::CancelStationSeek()
{
	if (iFmTunerUtility)
 	{
 		iFmTunerUtility->CancelStationSeek();
 	}	
}

void CMMFRadioBackendEngine::SetFreqRange(TFmRadioFrequencyRange aRange)
{
	if (iFmTunerUtility)
 	{
 		iFmTunerUtility->SetFrequencyRange(aRange);
 	}	
}

TInt CMMFRadioBackendEngine::GetFreqRange(TFmRadioFrequencyRange& aRange)
{
	TInt ret = KErrNotFound;
	TInt minFreq = 0;
	TInt maxFreq = 0;
		
 	if (iFmTunerUtility)
 	{
 		ret = iFmTunerUtility->GetFrequencyRange((TFmRadioFrequencyRange&)aRange, (TInt&)minFreq, (TInt&)maxFreq);
 		if (ret != KErrNone)
 		{
 			aRange = iDefaultFreqRange;
 			minFreq = iDefaultMinFreq;
 			maxFreq = iDefaultMaxFreq;
 			ret = KErrNone;
 		}
 	}	
  DEBUG_API_A1("CMMFRadioBackendEngine::GetFreqRange RET: %d", ret);	 
//  RDebug::Print(_L("CMMFRadioBackendEngine::GetFreqRange RET: %d"), ret);	   	
  return ret;
}

TInt CMMFRadioBackendEngine::GetFreqRangeProperties(TFmRadioFrequencyRange& aRange, TInt& aMinFreq, TInt& aMaxFreq)
{
	TInt ret = KErrNotFound;
		
	if (iFmTunerUtility)
 	{
  		ret = iFmTunerUtility->GetFrequencyRange((TFmRadioFrequencyRange&)aRange, (TInt&)aMinFreq, (TInt&)aMaxFreq);
  		if (ret != KErrNone)
  		{
  			aRange = iDefaultFreqRange;
  			aMinFreq = iDefaultMinFreq;
  			aMaxFreq = iDefaultMaxFreq;
  			ret = KErrNone;
  		}
 	}	
  DEBUG_API_A1("CMMFRadioBackendEngine::GetFreqRangeProperties RET: %d", ret);	 	
//  RDebug::Print(_L("CMMFRadioBackendEngine::GetFreqRangeProperties RET: %d"), ret);	   
  return ret;
}
TInt CMMFRadioBackendEngine::GetMaxVolume(TInt& aMaxVol)
{
	TInt ret = KErrNotFound;

	if (iRadioPlayerUtility)
	{
		ret = iRadioPlayerUtility->GetMaxVolume(aMaxVol);
	}		 
  return ret;
}

TInt CMMFRadioBackendEngine::SetVolume(TInt aVol)
{
	TInt ret = KErrNotFound; 

	if (iRadioPlayerUtility)
	{
		ret = iRadioPlayerUtility->SetVolume(aVol);
	}		 
  return ret;
}

TInt CMMFRadioBackendEngine::SetMute(XAboolean aMute)
{
	TInt ret = KErrNotFound;

	if (iRadioPlayerUtility)
	{
		ret = iRadioPlayerUtility->Mute(aMute);
	}		
  return ret;
}

TInt CMMFRadioBackendEngine::GetVolume(TInt& aVol)
{
	TInt ret = KErrNotFound;

	if (iRadioPlayerUtility)
	{
		ret = iRadioPlayerUtility->GetVolume(aVol);
	}		
  return ret;
}

TInt CMMFRadioBackendEngine::GetForcedMonoReception(XAuint32& aForcedMono) 
{
	TInt ret = KErrNotFound;
	TBool forceMono;

	if (iFmTunerUtility)
	{
		ret = iFmTunerUtility->GetForcedMonoReception(forceMono);
		if (ret == KErrNotReady)
		{
			aForcedMono = XA_STEREOMODE_STEREO; // Radio Utility Default value
			return KErrNone;
		}
	}		 
		
	
	if (forceMono)
	{
		aForcedMono = XA_STEREOMODE_MONO;
	}
	else
	{
		if (iForceStereo)			
			aForcedMono = XA_STEREOMODE_STEREO;		
		else	
			aForcedMono = XA_STEREOMODE_AUTO;					
	}
  return ret;
}

void CMMFRadioBackendEngine::PlayRadio()
{
	if (iRadioPlayerUtility)
	{
		iRadioPlayerUtility->Play();
	}		
}

void CMMFRadioBackendEngine::StopRadio()
{
	if (iRadioPlayerUtility)
	{
		iRadioPlayerUtility->Stop();
	}		
}

TInt CMMFRadioBackendEngine::ForceMonoReception(XAuint32 aForcedMono)
{
	TInt ret = KErrNotFound;
	TBool currentMode;
	
	ret = GetForcedMonoReception((XAuint32&)currentMode);
	if (ret != XA_RESULT_SUCCESS)
		return ret;
		
	if (iFmTunerUtility)
	{
		if (aForcedMono == XA_STEREOMODE_MONO)
		{
			iForceStereo = EFalse;
			ret = iFmTunerUtility->ForceMonoReception(ETrue);
		}
		else if (aForcedMono == XA_STEREOMODE_STEREO)
		{
			iForceStereo = ETrue;						
			if (currentMode == XA_STEREOMODE_AUTO) // Transition from Auto to Stereo doesn't require Utility call:
			{
				MrftoForcedMonoChange(ETrue);
			}	
			else
			{	
				ret = iFmTunerUtility->ForceMonoReception(EFalse);
			}
		}	
		else // (aForcedMono == XA_STEREOMODE_AUTO)
		{
			iForceStereo = EFalse;
			if (currentMode == XA_STEREOMODE_STEREO) // Transition from Stereo to Auto doesn't require Utility call:	
			{
				MrftoForcedMonoChange(EFalse);
			}	
			else
			{										
				ret = iFmTunerUtility->ForceMonoReception(EFalse);
			}
		}							
	}	
  DEBUG_API_A1("CMMFRadioBackendEngine::ForceMonoReception RET: %d", ret);	
 // RDebug::Print(_L("CMMFRadioBackendEngine::ForceMonoReception RET: %d"), ret);	   		
  return ret;
}

XAresult CMMFRadioBackendEngine::SetForceMonoFlag() 
{
	TInt ret = KErrNotFound;
	TBool forceMono;

	if (iFmTunerUtility)
	{
		ret = iFmTunerUtility->GetForcedMonoReception(forceMono);
		if (ret == KErrNotReady)
		{
			// For Radio Utility Default value = XA_STEREOMODE_STEREO
			iForceStereo = ETrue;
			return KErrNone;
		}
				
	}		 
		
	if (forceMono == XA_STEREOMODE_MONO)
	{
		iForceStereo = EFalse;
	}
	else
	{
		iForceStereo = ETrue;			
	}
  return ret;
}
XAresult CMMFRadioBackendEngine::SetRadioAdaptContext(void * adaptcontext)
{
	iRadioAdaptContext = adaptcontext;
	return XA_RESULT_SUCCESS;
} 

XAresult CMMFRadioBackendEngine::SetPlayerAdaptContext(void * adaptcontext)
{
	iPlayerAdaptContext = adaptcontext;
	return XA_RESULT_SUCCESS;
} 

XAresult TranslateError(TInt error)
    {
    XAresult status(XA_RESULT_SUCCESS);
    switch(error)
        {
        case KErrNone:
            status = XA_RESULT_SUCCESS;
            break;
        // to do: investigate and add other possible errors:
            
       /* case XA_RESULT_PRECONDITIONS_VIOLATED:
 
            break;
        case XA_RESULT_PARAMETER_INVALID:

            break;
        case XA_RESULT_MEMORY_FAILURE:

            break;
        case XA_RESULT_RESOURCE_ERROR:

            break;
        case XA_RESULT_RESOURCE_LOST:
 
            break;
        case XA_RESULT_IO_ERROR:

            break;
        case XA_RESULT_BUFFER_INSUFFICIENT:

            break;
        case XA_RESULT_CONTENT_CORRUPTED:

            break;
        case XA_RESULT_CONTENT_UNSUPPORTED:
 
            break;
        case XA_RESULT_CONTENT_NOT_FOUND:

            break;
        case XA_RESULT_PERMISSION_DENIED:

            break;
        case XA_RESULT_FEATURE_UNSUPPORTED:

            break;
        case XA_RESULT_INTERNAL_ERROR:

            break;
        case XA_RESULT_UNKNOWN_ERROR:

            break;
        case XA_RESULT_OPERATION_ABORTED:

            break;
        case XA_RESULT_CONTROL_LOST:

            break;
            */
        default:
        	break;
        } // end switch
    return status;
    }
// -----------------------------------------------------------------------------
// CMMFRadioBackendEngine::MrpeoPresetChanged
// Observer for Presets
// -----------------------------------------------------------------------------
void CMMFRadioBackendEngine::MrpeoPresetChanged(
    TPresetChangeEvent /*aChange*/,
    TInt /*aIndex*/ )
{

}

// ----------------------------------------------------
// CMMFRadioBackendEngine::MrftoSquelchChange
//
// ----------------------------------------------------
//
void CMMFRadioBackendEngine::MrftoSquelchChange(
    TBool /*aSquelch*/ )
{
	
}    

// ----------------------------------------------------
// CMMFRadioBackendEngine::MrftoForcedMonoChanged
// Called when a client enables/disabled forced mono reception
// ----------------------------------------------------
//
void CMMFRadioBackendEngine::MrftoForcedMonoChange(
    TBool aForcedMono )
{
  DEBUG_API_A1("CMMFRadioBackendEngine::MrftoForcedMonoChange: aForcedMono =  %d", aForcedMono);	
//  RDebug::Print(_L("CMMFRadioBackendEngine::MrftoForcedMonoChange: aForcedMono = %d"), aForcedMono);	   	
	XARadioItfAdapt_StereoStatusChange((XAAdaptationBaseCtx*)iRadioAdaptContext, aForcedMono);    
}  
    
 // ----------------------------------------------------
// CMMFRadioBackendEngine::MrftoFrequencyChange
// Called when the tuned frequency changes
// ----------------------------------------------------
//
void CMMFRadioBackendEngine::MrftoFrequencyChange(
    TInt aNewFrequency)
{
 	DEBUG_API_A1("CMMFRadioBackendEngine::MrftoFrequencyChange: aNewFrequency =  %d", aNewFrequency);	
 // RDebug::Print(_L("CMMFRadioBackendEngine::MrftoFrequencyChange aNewFrequency = %d"), aNewFrequency);	  			
	XARadioItfAdapt_FrequencyChange((XAAdaptationBaseCtx*)iRadioAdaptContext, aNewFrequency);    
}  
    
// ----------------------------------------------------
// CMMFRadioBackendEngine::MrftoFrequencyRangeChange
//
// ----------------------------------------------------
//
void CMMFRadioBackendEngine::MrftoFrequencyRangeChange(
    TFmRadioFrequencyRange aNewRange )
{
 	DEBUG_API_A1("CMMFRadioBackendEngine::MrftoFrequencyRangeChange: aNewRange =  %d", aNewRange);		
//  RDebug::Print(_L("CMMFRadioBackendEngine::MrftoFrequencyRangeChange: aNewRange = %d"), aNewRange);	  		
	XARadioItfAdapt_FrequencyRangeChange((XAAdaptationBaseCtx*)iRadioAdaptContext, aNewRange);        
} 
 
// ----------------------------------------------------
// CMMFRadioBackendEngine::MrftoOfflineModeStatusChange
// Called when offline mode status changes
// ----------------------------------------------------
//
void CMMFRadioBackendEngine::MrftoOfflineModeStatusChange(
    TBool /*aOfflineMode*/ )
{
 
}  
    
// ----------------------------------------------------
// CMMFRadioBackendEngine::MrftoAntennaStatusChange
// Called when antenna status changes.
// ----------------------------------------------------
//
void CMMFRadioBackendEngine::MrftoAntennaStatusChange(
    TBool /*aAttached*/ )
{

}

// ----------------------------------------------------
// CMMFRadioBackendEngine::MrftoFmTransmitterStatusChange
// The FM tuner is turned off when FM transmitter is on.
// ----------------------------------------------------
//
void CMMFRadioBackendEngine::MrftoFmTransmitterStatusChange(
    TBool /*aActive*/ )
{
 
}
  
// ----------------------------------------------------
// CMMFRadioBackendEngine::MrftoStationSeekComplete
//
// ----------------------------------------------------
//
void CMMFRadioBackendEngine::MrftoStationSeekComplete(
    TInt aError,
    TInt aFrequency )
{
	XARadioItfAdapt_SeekComplete((XAAdaptationBaseCtx*)iRadioAdaptContext, aError, aFrequency);    
}      	
    
// ----------------------------------------------------
// CMMFRadioBackendEngine::MrftoSetFrequencyComplete
// ----------------------------------------------------
//
void CMMFRadioBackendEngine::MrftoSetFrequencyComplete(
    TInt aError )
{
 	DEBUG_API_A1("CMMFRadioBackendEngine::MrftoSetFrequencyComplete: aError =  %d", aError);		
//  RDebug::Print(_L("CMMFRadioBackendEngine::MrftoFrrequencyComplete: aError = %d"), aError);	  		
	TInt freq = 0;
  if (!aError) 
 	{
 		iFmTunerUtility->GetFrequency(freq);
 	}
 	DEBUG_API_A1("CMMFRadioBackendEngine::MrftoSetFrequencyComplete: new freq is: %d", freq);	 
//  RDebug::Print(_L("CMMFRadioBackendEngine::MrftoSetFrequencyComplete: new freq is: %d"), freq);	  		
	XARadioItfAdapt_SetFrequencyComplete((XAAdaptationBaseCtx*)iRadioAdaptContext, aError, freq);    
}  
    
// ----------------------------------------------------
// CMMFRadioBackendEngine::MrftoSetFrequencyRangeComplete
//
// ----------------------------------------------------
//
void CMMFRadioBackendEngine::MrftoSetFrequencyRangeComplete(
    TInt aError )
{
 	DEBUG_API_A1("CMMFRadioBackendEngine::MrftoSetFrequencyRangeComplete: aError =  %d", aError);		
//  RDebug::Print(_L("CMMFRadioBackendEngine::MrftoSetFrequencyRangeComplete: aError = %d"), aError);	  	
	XARadioItfAdapt_SetFrequencyRangeComplete((XAAdaptationBaseCtx*)iRadioAdaptContext, aError);       	  
}

// ----------------------------------------------------
// CMMFRadioBackendEngine::MrftoRequestTunerControlComplete
// ----------------------------------------------------
//
void CMMFRadioBackendEngine::MrftoRequestTunerControlComplete(
    TInt aError)
{
//  RDebug::Print(_L("CMMFRadioBackendEngine::MrftoRequestTunerControlComplete: aError = %d"), aError);	  	  			 
} 
    
// ----------------------------------------------------
// CMMFRadioBackendEngine::MrpoBalanceChange
//
// ----------------------------------------------------
//
void CMMFRadioBackendEngine::MrpoBalanceChange(
    TInt /*aLeftPercentage*/,
    TInt /*aRightPercentage*/ )
{
	
} 
    
// ----------------------------------------------------
// CMMFRadioBackendEngine::MrpoMuteChange
//
// ----------------------------------------------------
//
void CMMFRadioBackendEngine::MrpoMuteChange(
    TBool aMute )
{
	XANokiaVolumeExtItfAdapt_MuteChange((XAAdaptationBaseCtx*)iPlayerAdaptContext, aMute);   
}  
    
// ----------------------------------------------------
// CMMFRadioBackendEngine::MrpoVolumeChange
//
// ----------------------------------------------------
//
void CMMFRadioBackendEngine::MrpoVolumeChange(
    TInt aVolume )
{
	XANokiaLinearVolumeItfAdapt_VolumeChange((XAAdaptationBaseCtx*)iPlayerAdaptContext, aVolume);      	
}  
    
// ----------------------------------------------------
// CMMFRadioBackendEngine::MrpoStateChange This is Tuner State
// Called when the state of the tuner changes
// ----------------------------------------------------
//
void CMMFRadioBackendEngine::MrpoStateChange(
    TPlayerState aState,
    TInt aError )
{	    	
	if ( aError )
 	{
 		DEBUG_API_A1("CMMFRadioBackendEngine::MrpoStateChange: aError =  %d", aError);
 // 	RDebug::Print(_L("CMMFRadioBackendEngine::MrpoStateChange: aError = %d"), aError);	  	 			     	
 	}

 	DEBUG_API_A1("CMMFRadioBackendEngine::MrpoStateChange: new state =  %d", aState);	
//  RDebug::Print(_L("CMMFRadioBackendEngine::MrpoStateChange: new state = %d"), aState);	  	 	   
 	      
	if ( aState == ERadioPlayerPlaying )
 	{
  	XAPlayItfAdaptMMF_StateChange((XAAdaptationBaseCtx*)iPlayerAdaptContext, ETrue);  
	}  
 	else  // ERadioPlayerIdle
 	{
  	XAPlayItfAdaptMMF_StateChange((XAAdaptationBaseCtx*)iPlayerAdaptContext, EFalse);  
 	}        	
}                           
               
extern "C" {

	void* cmmfradiobackendengine_init()
	{ 
		return CMMFRadioBackendEngine::Instance();
	}
    
	void cmmfradiobackendengine_delete(void* context)
	{ 
		((CMMFRadioBackendEngine*)(context))->DeleteInstance();
	}    
                 
	void  set_frequency(void* context, XAuint32 freq)
	{
		((CMMFRadioBackendEngine*)(context))->SetFrequency((TInt)freq); 			
	}
    
	XAresult  get_frequency(void* context, XAuint32* freq)
	{
		return ((CMMFRadioBackendEngine*)(context))->GetFrequency((TInt&)*freq); 			
	}  
    
	XAresult  get_signal_strength(void* context, XAuint32* signalStrength)
	{
		return ((CMMFRadioBackendEngine*)(context))->GetSignalStrength((TInt&)*signalStrength); 			
	}      
    
	void station_seek(void* context, XAboolean upwards)      
	{
		((CMMFRadioBackendEngine*)(context))->StationSeek(upwards); 			
	}
		
	void cancel_set_frequency(void* context)      
	{
		((CMMFRadioBackendEngine*)(context))->CancelSetFrequency(); 			
	}		
		
	void cancel_station_seek(void* context)      
	{
		((CMMFRadioBackendEngine*)(context))->CancelStationSeek(); 			
	}		
		
	void set_freq_range(void* context, XAuint8 range)      
	{
		((CMMFRadioBackendEngine*)(context))->SetFreqRange((TFmRadioFrequencyRange)range); 			
	}	
		
	XAresult  get_freq_range(void* context, XAuint8* range)
	{
		return ((CMMFRadioBackendEngine*)(context))->GetFreqRange((TFmRadioFrequencyRange&)*range); 			
	}
    
	XAresult  get_freq_range_properties(void* context, XAuint8 range, XAuint32* aMinFreq, XAuint32* aMaxFreq)
	{
		return ((CMMFRadioBackendEngine*)(context))->GetFreqRangeProperties((TFmRadioFrequencyRange&)range, (TInt&) *aMinFreq, (TInt&) *aMaxFreq); 		
	}   
    
	XAresult  get_max_volume(void* context, XAmillibel* maxVol)
	{
		return  ((CMMFRadioBackendEngine*)(context))->GetMaxVolume((TInt&)*maxVol); 
	}   

	XAresult  set_volume(void* context, XAuint32 vol)
	{
		return ((CMMFRadioBackendEngine*)(context))->SetVolume((TInt)vol); 			
	} 
    
	XAresult  get_volume(void* context, XAuint32* vol)
	{
		return ((CMMFRadioBackendEngine*)(context))->GetVolume((TInt&)*vol); 		
	}  
    
	XAresult  set_mute(void* context, XAboolean mute)
	{
		return ((CMMFRadioBackendEngine*)(context))->SetMute(mute); 			
	} 
    
	XAresult  get_stereo_mode(void* context, XAuint32* mode)
	{
		return ((CMMFRadioBackendEngine*)(context))->GetForcedMonoReception(*mode); 			
	}     
    
	XAresult  set_stereo_mode(void* context, XAuint32 mode)
	{
		return ((CMMFRadioBackendEngine*)(context))->ForceMonoReception(mode); 		
	}  
     
	void play_radio(void* context)
	{
		((CMMFRadioBackendEngine*)(context))->PlayRadio(); 			
	} 
    
	void stop_radio(void* context)
	{
		((CMMFRadioBackendEngine*)(context))->StopRadio(); 	
	}        
    
	XAresult mmf_set_radio_adapt_context(void * context, void * adaptcontext)
	{
		return ((CMMFRadioBackendEngine*)(context))->SetRadioAdaptContext(adaptcontext);
	}   
	
	XAresult mmf_set_player_adapt_context(void * context, void * adaptcontext)
	{
		return ((CMMFRadioBackendEngine*)(context))->SetPlayerAdaptContext(adaptcontext);
	}   	                       										
}
