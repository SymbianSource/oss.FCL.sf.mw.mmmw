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
/*
Media Helper Class for handling surface events

Shy Ward
*/

#include "cmmfbackendengine.h"
#include <string.h>
#include <ctype.h>
#include <uri8.h>

extern "C" {
#include "xaadaptationmmf.h"
}


#define ENABLE_GRAPHICS_SURFACE_INTEGRATION_NOT_NOW

CMMFBackendEngine* CMMFBackendEngine::NewL()
    {
    CMMFBackendEngine* self = new (ELeave) CMMFBackendEngine();
    self->ConstructL();
    return self;
    }

CMMFBackendEngine::~CMMFBackendEngine()
    {
    CloseAndDestroy();
    }
    
CMMFBackendEngine::CMMFBackendEngine()                  
    {
    iVideoPlayer = NULL;
    m_bWindowReferencePassed = FALSE;
    iRecordState = ERecorderNotReady;
    iPositionUpdateTimer = NULL;
    }


void CMMFBackendEngine::InitVideoPlayerUtilityL()
    {
	iBaseVideoPlayer = CVideoPlayerUtility2::NewL(*this,EMdaPriorityNormal,EMdaPriorityPreferenceTimeAndQuality);
	iVideoPlayer = (CVideoPlayerUtility2*)iBaseVideoPlayer;
	iVideoPlayer->RegisterForVideoLoadingNotification(*this);   
    }

void CMMFBackendEngine::CreateAndConfigureWindowL()
    {
#ifdef ENABLE_GRAPHICS_SURFACE_INTEGRATION_NOT_NOW
    // create window for attaching the surface as its background
    //RWsSession ws;
    //TInt err2(KErrNone);
    TInt err2 = m_ws.Connect();
    m_pScr = new(ELeave) CWsScreenDevice(m_ws);
    err2 = m_pScr->Construct();
    CWindowGc* gc = NULL;
    err2 = m_pScr->CreateContext(gc);
    RWindowGroup grp(m_ws);
    err2 = grp.Construct(0xf00f00);
    const TSize KWinSize(320, 240);
    m_pWindow = new (ELeave) RWindow(m_ws);
    err2 = m_pWindow->Construct(grp, 0xfeefee);
    m_pWindow->SetExtent(TPoint(), KWinSize);
    m_pWindow->SetBackgroundColor(TRgb(255,0,0,128));
    //win.SetBackgroundColor(TRgb(0,0,0,0));
    m_pWindow->Activate();
    m_pWindow->Invalidate();
    m_pWindow->BeginRedraw();
    gc->Activate(*m_pWindow);
    m_pWindow->EndRedraw();
    m_ws.Flush();

    //Create MediaClientUitlity for NGA Surfaces
    TInt displayId = m_pScr->GetScreenNumber();

    // Add the display window
    m_cropRegion = TRect(m_pWindow->Size());
    m_clipRect = TRect(m_pWindow->Size());
    m_videoExtent = TRect(m_pWindow->Size());
    m_rotation = EVideoRotationNone;
            
#endif // ENABLE_GRAPHICS_SURFACE_INTEGRATION_NOT_NOW     
    
    }

void CMMFBackendEngine::ConstructL()
    {
	//iBaseAudioPlayer = CMdaAudioPlayerUtility::NewL(*this);
	//iAudioPlayer = (CMdaAudioPlayerUtility*)iBaseAudioPlayer;
	iAPIBeingUsed = EAudioPlayerUtility;	
	//InitVideoPlayerUtilityL();
	iPlayerState = EPlayerClosed;
	iBaseAudioRecorder = CMdaAudioRecorderUtility::NewL(*this);
	iAudioRecorder = (CMdaAudioRecorderUtility*)iBaseAudioRecorder;
	iActiveSchedulerWait = new (ELeave) CActiveSchedulerWait;
    } 

TInt CMMFBackendEngine::SetFileName(char* uri, XAuint32 format, TFuncInUse func)
    {
    TInt err(KErrNone);
    _LIT8(KFileSlash,"file:///");
    TInt fileslashlen = KFileSlash().Length();
    if(func == ERecord)
        {
        if(iRecordState == ERecorderNotReady)
            {
            iFileFormat = format;
            iAPIBeingUsed = DetermineAPIToUse(uri, ERecord);
            TPtr8 fileuri(((TUint8*)uri), strlen(uri) ,strlen(uri));
            TPtr8 filepath = fileuri.RightTPtr(strlen(uri)-fileslashlen);
            if(iAPIBeingUsed == EAudioPlayerUtility)
                {
                TInt pos = filepath.LocateReverse(':');
                if(pos != KErrNotFound)
                    {
                    fileuri.Delete(fileslashlen+pos,1);
                    }
                
                TUriParser8 localfileUri;
                TInt ret = localfileUri.Parse(fileuri);
                HBufC* file = NULL;
                TRAP(err,file = localfileUri.GetFileNameL());  
                TPtr des = file->Des();
                iFileName = des;
                delete file;
                file = NULL;
                TRAP(err, iAudioRecorder->OpenFileL(iFileName));
                if(!iActiveSchedulerWait->IsStarted())
                    {
                    iActiveSchedulerWait->Start();
                    }
                }
           }
        }
    else
        {
        if(iPlayerState == EPlayerClosed)
            {
            iFileFormat = format;
            iAPIBeingUsed = DetermineAPIToUse(uri,EPlay);
            TPtr8 fileuri((TUint8*)uri, strlen(uri),strlen(uri));
            
            if(iAPIBeingUsed == EAudioPlayerUtility)
                {
                TInt pos = fileuri.LocateReverse(':');
                fileuri.Delete(pos,1);
                
                TUriParser8 localfileUri;
                TInt ret = localfileUri.Parse(fileuri);
                HBufC* file = NULL;
                TRAP(err,file = localfileUri.GetFileNameL());  
                TPtr des = file->Des();
                iFileName = des;
                delete file;
                file = NULL;
                }
            else
                {
                HBufC* f(NULL);
                TRAP(err, f = HBufC::NewL(strlen(uri)));
                TPtr des = f->Des();
                des.Copy(fileuri);
                iFileName = des;
                delete f;
                f = NULL;
                }
           }
        }
    return err;
    }

TInt CMMFBackendEngine::DetermineAPIToUse(char* uri, TFuncInUse aFunc)
    {
    char* tempExtensionPtr = NULL;
    char extension[MAX_EXTENSION_SIZE] = { 0 };
    
    tempExtensionPtr = strchr(uri, (int)'.');
    strcpy(extension, tempExtensionPtr);
        
    for(unsigned int i=0;i<sizeof(extension);i++)
        {
        extension[i] = tolower(extension[i]);
        }
    
    if(aFunc == ERecord)
        {
        return EAudioPlayerUtility; 
        }
    else
        {
        if(!strcmp(extension, ".mp3") || (!strcmp(extension, ".amr")) || (!strcmp(extension, ".aac")) || (!strcmp(extension, ".mid")) ||
                  (!strcmp(extension, ".awb")))
            {
            return EAudioPlayerUtility; 
            }
          else 
            {
            return EVideoPlayerUtility;
            }
        }
    }

void CMMFBackendEngine::SetWindowHandle(void* display_info)
    {
    XADataLocator_NativeDisplay* nativeDisplay;
    XADataSink* videoSink = (XADataSink*)display_info;
    
    nativeDisplay = (XADataLocator_NativeDisplay*) (videoSink->pLocator);
    
    m_pWindow = ((RWindow*)(nativeDisplay->hWindow));
    m_pWs =     ((RWsSession*)(nativeDisplay->hDisplay));
    
    m_bWindowReferencePassed = TRUE;
    
    }

//From MVidePlayerUtilityObserver
void CMMFBackendEngine::MvpuoOpenComplete(TInt aError)
    {
    if(aError == KErrNone)
        {
        //Issue the command to Play the file
        TRAPD (error, iVideoPlayer->Prepare());
        if (error == KErrNone)
            {
                //prepare success
            }
        else
            {
                //prepare failed
            }
        }
    else
        {
        //could not open file
        }
    }



void CMMFBackendEngine::MvpuoFrameReady(CFbsBitmap& /*aFrame*/,TInt /*aError*/)
    {
    
    }



void CMMFBackendEngine::MvpuoPlayComplete(TInt aError)
    {
    
        
    if(aError == KErrNone)
        {
            //playback complete
        }
    else
        {
            //playback complete with error
        }
    }



void CMMFBackendEngine::MvpuoEvent(class TMMFEvent const & event)
    {
    RDebug::Print(_L("CMMFBackendEngine::MvpuoEvent (0x%x %d)\n"),
  		          event.iEventType, event.iErrorCode);

    if (event.iEventType == KMMFEventCategoryVideoPlayerGeneralError &&
        event.iErrorCode == KErrHardwareNotAvailable)
        {
        //audio device taken
        //handle this case
        }

	else if (event.iEventType == KMMFEventCategoryVideoPlayerGeneralError &&
			 event.iErrorCode == KErrMMPartialPlayback)
        {
        //Partial Playback
        } 
    
    else if (event.iEventType == KMMFRefreshMetaData)
        {
       //entry = iVideoPlayer->MetaDataEntryL( event.iErrorCode );
        }
    else
        {
    
        }
    
}



void CMMFBackendEngine::MvpuoPrepareComplete(TInt aError)
    {
    TInt err(KErrNone);
    TAutoScaleType autoScale = EAutoScaleBestFit;
	if(aError == KErrNone)
	    {
        if(m_pWindow == NULL)
            {
            //no RWindow was given to use so I'll create one and handle it
            //CreateAndConfigureWindowL();
            }
        else
            {
            TRAP(err, iVideoPlayer->AddDisplayWindowL(*m_pWs, *m_pScr, *m_pWindow));
            TRAP(err, iVideoPlayer->SetAutoScaleL(*m_pWindow, autoScale));
            }

        TReal beginSecs = 1;
        TReal intervalSecs = 5;
        TTimeIntervalMicroSeconds  interval(1000000*intervalSecs);
        TTimeIntervalMicroSeconds  beginning(1000000*beginSecs);
        
        iVideoPlayer->Play();
        iPlayerState = EPlayerPlaying;
	    }
    }

// From MRebufferCallback
void CMMFBackendEngine::MvloLoadingStarted()
    {
    }




void CMMFBackendEngine::MvloLoadingComplete()
    {
    //file playing
    }

//MMdaAudioPlayerCallback
void CMMFBackendEngine::MapcInitComplete(TInt aError,
										  const TTimeIntervalMicroSeconds& /* aDuration */)
    {
    if (aError == KErrNone)
        {
        iAudioPlayer->Play();
        iPlayerState = EPlayerPlaying;
        }
    else
        {
        //init failed
        }
    }


void CMMFBackendEngine::MapcPlayComplete(TInt /* aError */)
    {
    //audio playback complete
    }

// from MMdaObjectStateChangeObserver
void CMMFBackendEngine::MoscoStateChangeEvent(CBase* /*aObject*/, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode)
    {
    iPreviousRecordState = aPreviousState;
    iCurrentRecordState = aCurrentState;
    iErrorCode = aErrorCode;
    RDebug::Print(_L("CMMFBackendEngine::MoscoStateChangeEvent 1 Error[%d]"),aErrorCode);  
    if(iCurrentRecordState == CMdaAudioClipUtility::EOpen)  //EOpen
        {
        //outputfile is open and ready for recording
        iRecordState = CMMFBackendEngine::ERecorderOpen;
        if(iErrorCode == KErrNone)
            {
            if (iPreviousRecordState == CMdaAudioClipUtility::ENotReady)
                {
                RDebug::Print(_L("CMMFBackendEngine::MoscoStateChangeEvent 2"));    
                TMMFMessageDestination destination(KUidMetaDataWriteCustomCommand);
                TMMFMessageDestinationPckg pckg = TMMFMessageDestinationPckg(destination);
                TInt ret = iAudioRecorder->RecordControllerCustomCommandSync(pckg, 0, KNullDesC8, KNullDesC8);
                RDebug::Print(_L("CMMFBackendEngine::MoscoStateChangeEvent 3 [%d]"),ret);    
                if(ret != KErrNone && iFileFormat == XA_CONTAINERTYPE_MP4)
                    {
                    iPauseSupportMP4 = FALSE;
                    }
                if(iActiveSchedulerWait->IsStarted())
                    {
                    iActiveSchedulerWait->AsyncStop();
                    }
                }
            }
        else
            {
            XAAdaptEventMMF event = {XA_RECORDITFEVENTS, XA_OBJECT_EVENT_RESOURCES_LOST, 0, NULL};
            XAAdaptationBaseMMF_SendAdaptEvents((XAAdaptationBaseMMFCtx*)iAdaptContext, &event );
            }
        }
    else if(iCurrentRecordState == CMdaAudioClipUtility::ERecording)  //ERecording
        {
        iRecordState = CMMFBackendEngine::ERecorderRecording;
        iPositionUpdateTimer->Start(iTimerDelay);
        XAAdaptEventMMF event = {XA_RECORDITFEVENTS, XA_RECORDEVENT_HEADMOVING, 0, NULL};
        XAAdaptationBaseMMF_SendAdaptEvents((XAAdaptationBaseMMFCtx*)iAdaptContext, &event );
        }
    else   //ENotReady
        {
        //outputfile is not open
        iRecordState = CMMFBackendEngine::ERecorderNotReady;   
        }
    }

TInt CMMFBackendEngine::SetRecorderState(TRecorderState state, XAboolean stopCalled)
    {
    TInt err(KErrNone);
    switch(state)
        {
        case ERecorderNotReady:
            iAudioRecorder->Close();
            iRecordState = ERecorderNotReady;
            break;
        case ERecorderOpen:
            if(iFileFormat == XA_CONTAINERTYPE_MP4 && !iPauseSupportMP4 && !stopCalled)
                {
                err = KErrNotSupported;
                return err;
                }            
            iPositionUpdateTimer->Stop();
            iAudioRecorder->Stop(); 
            iRecordState = ERecorderOpen;
            break;
        case ERecorderRecording:
            TRAP(err, iAudioRecorder->RecordL());
            break;
        }
    return err;
    }

void CMMFBackendEngine::AudioOpenFile()
    {
	TRAPD(err, iAudioPlayer->OpenFileL(iFileName));
	if(err)
	    {
        
	    }
    }

void CMMFBackendEngine::AudioStop()
    {
	iAudioPlayer->Stop();
    }

void CMMFBackendEngine::AudioPause()
    {
	iAudioPlayer->Pause();
    }

void CMMFBackendEngine::AudioClose()
    {
	iAudioPlayer->Close();
    }

void CMMFBackendEngine::ResumePlayback()
    {
    if(iPlayerState == EPlayerClosed)
        {
        //prepare playback
        if(iAPIBeingUsed == EAudioPlayerUtility)
            {
            AudioOpenFile();
            }
        else
            {
            TRAPD (error, iVideoPlayer->OpenFileL(iFileName));
            if(error != KErrNone)
                {
                
                }
            }
        iPlayerState = EPlayerPaused;    
        }
    else if (iPlayerState == EPlayerPaused)
        {
        if(iAPIBeingUsed == EAudioPlayerUtility)
            {
            iAudioPlayer->Play();  
            }
        else
            {
            iVideoPlayer->Play();
            }
        iPlayerState = EPlayerPlaying;
        }
    }

void CMMFBackendEngine::PausePlayback()
    {
    if(iAPIBeingUsed == EAudioPlayerUtility)
        {
        AudioPause();
        }
    else
        {
        TRAPD(err, iVideoPlayer->PauseL());
        if(err != KErrNone)
            {
            
            }
        } 
    iPlayerState = EPlayerPaused;
    }

void CMMFBackendEngine::StopPlayback()
    {
    if(iAPIBeingUsed == EAudioPlayerUtility)
        {
        AudioStop();
        }
    if(iAPIBeingUsed == EVideoPlayerUtility)
        {
        iVideoPlayer->Stop();
        }
    iPlayerState = EPlayerClosed;   
    }

void CMMFBackendEngine::CloseAndDestroy()
    {
    if (iBaseVideoPlayer && iVideoPlayer)
        {
        iVideoPlayer->Close();
        delete iBaseVideoPlayer;
        iVideoPlayer = NULL;
        iBaseVideoPlayer = NULL;
        }
    
    if (iBaseAudioPlayer && iAudioPlayer)
        {
        iAudioPlayer->Close();
        delete iBaseAudioPlayer;
        iAudioPlayer = NULL;
        iBaseAudioPlayer = NULL;
        }
    
    if (iBaseAudioRecorder)
        {
        iAudioRecorder->Close();
        delete iBaseAudioRecorder;
        iAudioRecorder = NULL;
        iBaseAudioRecorder = NULL;
        }
    
    if(iPositionUpdateTimer)
        {
        delete iPositionUpdateTimer;
        iPositionUpdateTimer = NULL;
        }

    if(iActiveSchedulerWait)
        {
        if(iActiveSchedulerWait->IsStarted())
            {
            iActiveSchedulerWait->AsyncStop();
            }
        delete iActiveSchedulerWait;
        iActiveSchedulerWait = NULL;
        } 
    }

void CMMFBackendEngine::SetNativeDisplayInformation(void* display_info)
    {
	//display_info is of type XADataSink
	//display_info.pLocator is of type XADataLocator_NativeDisplay
	m_bWindowReferencePassed = TRUE;
	XADataLocator_NativeDisplay* nativeDisplay;
	XADataSink* videoSink = (XADataSink*)display_info;
	
	nativeDisplay = (XADataLocator_NativeDisplay*) (videoSink->pLocator);
	//TODO: scrDevice is not been passed Properly
    // Add the display window
    m_cropRegion = TRect(((RWindow*)(nativeDisplay->hWindow))->Size());
    m_videoExtent = TRect(((RWindow*)(nativeDisplay->hWindow))->Size());
    m_cropRect = TRect(((RWindow*)(nativeDisplay->hWindow))->Size());
    m_clipRect = TRect(((RWindow*)(nativeDisplay->hWindow))->Size());
    m_cropRegion = TRect(((RWindow*)(nativeDisplay->hWindow))->Size());
    m_pWindow = ((RWindow*)(nativeDisplay->hWindow));
    m_pWs = ((RWsSession*)(nativeDisplay->hDisplay));
    
    TRAPD(err, m_pScr = new(ELeave) CWsScreenDevice(*m_pWs));
    if(err == KErrNone)
        {
        m_pScr->Construct();
        }
    }

TInt CMMFBackendEngine::GetRecordPosition(XAuint64* position)
    {
    TInt err(KErrNone);
    *position = iAudioRecorder->Position().Int64();
    return err;
    }

TInt CMMFBackendEngine::SetPositionUpdatePerioed(XAmillisecond period)
    {
    TInt err(KErrNone);
    iTimerDelay = period;
    return err;
    }

TInt CMMFBackendEngine::SetAdaptContext(void* adaptcontext)
    {
    TInt err(KErrNone);
    iAdaptContext = adaptcontext;
    TRAP(err, iPositionUpdateTimer = new (ELeave) LocalTimer(this, iAdaptContext));
    if(!err)
        {
        iPositionUpdateTimer->PostInit();
        }
    return err;
    }

TInt CMMFBackendEngine::GetEncoderId(XAuint32* encoderId)
    {
    TInt err(KErrNone);
    if(iRecordState == CMMFBackendEngine::ERecorderOpen)
        {
        TFourCC dest;
        TRAP(err, dest = iAudioRecorder->DestinationDataTypeL());
        if(err == KErrNone)
            {
            *encoderId = dest.FourCC();
            }
        }
    return err;
    }

TInt CMMFBackendEngine::GetDestinationBitRate(XAuint32* bitrate)
    {
    TInt err(KErrNone);
    TUint br(0);
    if(iRecordState == CMMFBackendEngine::ERecorderOpen)
        {
        TRAP(err, br = iAudioRecorder->DestinationBitRateL());
        if(err == KErrNone)
            {
            *bitrate = br;
            }
        }
    return err;
    }

TInt CMMFBackendEngine::GetDestinationSampleRate(XAmilliHertz* samplerate)
    {
    TInt err(KErrNone);
    TUint sr(0);
    if(iRecordState == CMMFBackendEngine::ERecorderOpen)
        {
        TRAP(err, sr = iAudioRecorder->DestinationSampleRateL());
        if(err == KErrNone)
            {
            *samplerate = sr;
            }
        }
    return err;
    }

TInt CMMFBackendEngine::GetDestinationChannels(XAuint32* channels)
    {
    TInt err(KErrNone);
    TUint ch(0);
    if(iRecordState == CMMFBackendEngine::ERecorderOpen)
        {
        TRAP(err,ch = iAudioRecorder->DestinationNumberOfChannelsL());
        if(err == KErrNone)
            {
            *channels = ch;
            }
        }
    return err;
    }

TInt CMMFBackendEngine::SetDestinationBitRate(XAuint32* bitrate)
    {
    TInt err(KErrNone);
    if(iRecordState == CMMFBackendEngine::ERecorderOpen)
        {
        TRAP(err, iAudioRecorder->SetDestinationBitRateL(*bitrate));
        if(err != KErrNone)
            {
            return XA_RESULT_PARAMETER_INVALID;
            }
        }
    return err;
    }

TInt CMMFBackendEngine::SetDestinationSampleRate(XAmilliHertz* samplerate)
    {
    TInt err(KErrNone);
    if(iRecordState == CMMFBackendEngine::ERecorderOpen)
        {
        TRAP(err, iAudioRecorder->SetDestinationSampleRateL(*samplerate));
        if(err != KErrNone)
            {
            return XA_RESULT_PARAMETER_INVALID;
            }
        }
    return err;
    }

TInt CMMFBackendEngine::SetDestinationChannels(XAuint32* channels)
    {
    TInt err(KErrNone);
    if(iRecordState == CMMFBackendEngine::ERecorderOpen)
        {
        TRAP(err,iAudioRecorder->SetDestinationNumberOfChannelsL(*channels));
        if(err != KErrNone)
            {
            return XA_RESULT_PARAMETER_INVALID;
            }
        }
    return err;
    }

extern "C" {

    int mmf_backend_engine_init(void** engine)
    { 
        TRAPD(err, *engine = CMMFBackendEngine::NewL());
        return err;
        /*int err = CreateEngine(engine);
        return err;*/
    }
        
    int mmf_set_recorder_uri(void* context, char* uri, XAuint32 format)
    {
        return ((CMMFBackendEngine*)(context))->SetFileName(uri,format,CMMFBackendEngine::ERecord);
    }

    void mmf_set_player_uri(void* context, char* uri, XAuint32 format)
    {
        ((CMMFBackendEngine*)(context))->SetFileName(uri,format,CMMFBackendEngine::EPlay);
    }

    int mmf_set_adapt_context(void* context, void* adaptcontext)
    {
        return ((CMMFBackendEngine*)(context))->SetAdaptContext(adaptcontext);
    }

    void mmf_pause_playback(void* context)
    {
        ((CMMFBackendEngine*)(context))->PausePlayback();      
    }

    void mmf_resume_playback(void* context)
    {
        ((CMMFBackendEngine*)(context))->ResumePlayback();       
    }
    void mmf_set_window_handle(void* context, void* display_info)
    {
        ((CMMFBackendEngine*)(context))->SetWindowHandle(display_info);
    }
    
    void mmf_stop_playback(void* context)
    {
        ((CMMFBackendEngine*)(context))->StopPlayback(); 
    }
    void mmf_close(void* context)
    {
        delete ((CMMFBackendEngine*)context);
    }
    void mmf_setup_native_display(void* context, void* display_info)
    {
        ((CMMFBackendEngine*)(context))->SetNativeDisplayInformation(display_info); 
    }
    
    int mmf_start_recording(void* context)
    {
        return ((CMMFBackendEngine*)(context))->SetRecorderState(CMMFBackendEngine::ERecorderRecording,FALSE); 
    }
    
    int mmf_stop_recording(void* context, XAboolean stopCalled )
    {
        return ((CMMFBackendEngine*)(context))->SetRecorderState(CMMFBackendEngine::ERecorderOpen, stopCalled); 
    }
        
    int mmf_get_record_position(void* context, XAuint64* position)
    {
        return ((CMMFBackendEngine*)(context))->GetRecordPosition(position);
    }
    
    int mmf_set_record_position_update_period(void* context, XAmillisecond msec)
    {
        return ((CMMFBackendEngine*)(context))->SetPositionUpdatePerioed(msec);
    }    
    
    int mmf_get_encoder_id(void* context, XAuint32* encoderId)
    {
        return ((CMMFBackendEngine*)(context))->GetEncoderId(encoderId);
    }
    
    int mmf_get_destination_channels(void* context, XAuint32* channelsIn)
    {
        return ((CMMFBackendEngine*)(context))->GetDestinationChannels(channelsIn);
    }
    
    int mmf_get_destination_samplerate(void* context, XAmilliHertz* sampleRate)
    {
        return ((CMMFBackendEngine*)(context))->GetDestinationSampleRate(sampleRate);
    }

    int mmf_get_destination_bitrate(void* context, XAuint32* bitRate)
    {
        return ((CMMFBackendEngine*)(context))->GetDestinationBitRate(bitRate);
    }

    int mmf_set_destination_channels(void* context, XAuint32* channelsIn)
    {
        return ((CMMFBackendEngine*)(context))->SetDestinationChannels(channelsIn);
    }
    
    int mmf_set_destination_samplerate(void* context, XAmilliHertz* sampleRate)
    {
        return ((CMMFBackendEngine*)(context))->SetDestinationSampleRate(sampleRate);
    }

    int mmf_set_destination_bitrate(void* context, XAuint32* bitRate)
    {
        return ((CMMFBackendEngine*)(context))->SetDestinationBitRate(bitRate);
    }
    
}
