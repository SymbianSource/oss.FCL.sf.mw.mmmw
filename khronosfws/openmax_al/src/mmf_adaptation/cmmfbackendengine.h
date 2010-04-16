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

#ifndef XA_CMMFBackendEngine_H
#define XA_CMMFBackendEngine_H

#define ENABLE_GRAPHICS_SURFACE_INTEGRATION

#include "openmaxalwrapper.h"

#ifdef __cplusplus

#include <e32base.h>
#include <MdaAudioSamplePlayer.h>  
#include <f32file.h>
#include <videoplayer.h>
#include <videoplayer2.h>
#include <e32std.h>
#include <mmf/common/mmferrors.h>
#include <mmf/common/Mmfbase.h>
#include <MMFMetadataEventConfigConstants.h>
#ifdef ENABLE_GRAPHICS_SURFACE_INTEGRATION
#include <graphics/surface.h> //TSurfaceId
#include <mmf/common/mmfvideo.h> //TVideoAspectRatio
#include <mediaclientvideodisplay.h>
#include <surfaceeventhandler.h>
#include <mdaaudiosampleeditor.h>
#include "sfmrlocaltimer.h"
#endif

// MetaDataWrite Custom Command UID
const TUid KUidMetaDataWriteCustomCommand = {0x10207af9};

#define MAX_EXTENSION_SIZE 10

NONSHARABLE_CLASS(CMMFBackendEngine) : public CBase,
                          public MVideoPlayerUtilityObserver,
                          public MVideoLoadingObserver,
						              public MMdaAudioPlayerCallback,
						              public MMdaObjectStateChangeObserver
    {
public:      
      enum TFuncInUse
          {
          EPlay = 0,
          ERecord
          };
      
	enum TPlayerInUse
	{
		EVideoPlayerUtility = 0,
		EAudioPlayerUtility,
	};
	
	enum TPlayerState
	{
	  EPlayerClosed,
		EPlayerPaused,
		EPlayerPlaying,
		EPlayerError
	};
	
	enum TRecorderState
	{
	  ERecorderNotReady = 0,
	  ERecorderOpen,
	  ERecorderRecording
	};
	      
public:

    static CMMFBackendEngine* NewL();
    ~CMMFBackendEngine();
    

    void InitVideoPlayerUtilityL();
    TInt SetFileName(char* uri,XAuint32 format,TFuncInUse func);
    void ResumePlayback();
    void PausePlayback();    
    void StopPlayback();
    void SetWindowHandle(void* display_info);
    void CreateAndConfigureWindowL();
    void CloseAndDestroy();
    void SetNativeDisplayInformation(void* display_info);
    TInt SetRecorderState(TRecorderState state, XAboolean stopCalled);
    TInt GetRecordPosition(XAuint64* position);
    TInt SetPositionUpdatePerioed(XAmillisecond period);
    TInt SetAdaptContext(void* adaptcontext);
    TInt GetEncoderId(XAuint32* encoderId);
    TInt GetDestinationBitRate(XAuint32* bitrate);
    TInt GetDestinationSampleRate(XAmilliHertz* samplerate);
    TInt GetDestinationChannels(XAuint32* channels);
    TInt SetDestinationBitRate(XAuint32* bitrate);
    TInt SetDestinationSampleRate(XAmilliHertz* samplerate);
    TInt SetDestinationChannels(XAuint32* channels);
private:

		//From MVidePlayerUtilityObserver
		void MvpuoOpenComplete(TInt aError);
		void MvpuoFrameReady(CFbsBitmap& aFrame,TInt aError);
		void MvpuoPlayComplete(TInt aError);
		void MvpuoEvent(const TMMFEvent& aEvent);
		void MvpuoPrepareComplete(TInt aError);

public:

		//MMdaAudioPlayerCallback
		void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
		void MapcPlayComplete(TInt aError);
		
    // from MMdaObjectStateChangeObserver
    void MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt /*aErrorcCode*/);
		
    
private:

		// From MRebufferCallback
		void MvloLoadingStarted();
		void MvloLoadingComplete();		

private:
    //audio player methods
    void AudioOpenFile();
    void AudioClose();
    void AudioStop();
    void AudioPause();
    
    //video player methods
    
private:
    CMMFBackendEngine();
    void ConstructL();
    TInt DetermineAPIToUse(char *uri, TFuncInUse aFunc);
  
    
    //RThread                   m_ThreadHandle; 
    CVideoPlayerUtility2*		  iVideoPlayer;
		CBase*		                iBaseVideoPlayer;
		CBase*                    iBaseAudioPlayer; 
		CBase*                    iBaseAudioRecorder;
		CMdaAudioPlayerUtility*	  iAudioPlayer;  
		CMdaAudioRecorderUtility* iAudioRecorder;
		TFileName					        iFileName;
		TInt						iAPIBeingUsed;		
		TPlayerState              iPlayerState;
		TInt                      iPreviousRecordState;
		TInt                      iCurrentRecordState;
		TInt                      iErrorCode;
		TRecorderState            iRecordState;
		LocalTimer*               iPositionUpdateTimer;
		TUint64                   iTimerDelay;
		void*                     iAdaptContext;
		XAuint32                  iFileFormat;
		TInt                      iPauseSupportMP4;
		CActiveSchedulerWait*     iActiveSchedulerWait;
#ifdef ENABLE_GRAPHICS_SURFACE_INTEGRATION
    CMediaClientVideoDisplay* iMediaClientVideoDisplay;
    TRect m_clipRect;
    TRect m_videoExtent;    
    TRect m_cropRegion;
    TVideoAspectRatio m_pixelAspectRatio;
    RWindow m_Window;
    RWindow* m_pWindow;
    TRect m_cropRect;
    TSurfaceId m_surfaceId;
    TVideoRotation m_rotation;
    TReal32 m_scaleWidth;
    TReal32 m_scaleHeight;
    TInt m_horizPos;
    TInt m_vertPos; 
    TInt m_displayId;   
    TBool m_bWindowReferencePassed;
    CWsScreenDevice* m_pScr;
    RWsSession m_ws;
    RWsSession* m_pWs;

#endif /*ENABLE_GRAPHICS_SURFACE_INTEGRATION*/    
    };
#else  /* __cplusplus */


extern int mmf_backend_engine_init(void** engine);

extern int mmf_set_recorder_uri(void* context, char* uri, XAuint32 format);

extern void mmf_set_player_uri(void* context, char* uri, XAuint32 format);

extern void mmf_pause_playback(void* context);

extern void mmf_resume_playback(void* context);

extern void mmf_set_window_handle(void* context, void* display_infos);

extern void mmf_stop_playback(void* context);

extern void mmf_close(void* context);
    
extern void mmf_setup_native_display(void* context, void* display_info);

extern int mmf_set_adapt_context(void* context, void* adaptcontext);

extern int mmf_start_recording(void* context);

extern int mmf_stop_recording(void* context, XAboolean stopCalled);

extern int mmf_get_record_position(void* context, XAuint64* position);

extern int mmf_set_record_position_update_period(void* context, XAmillisecond msec);

extern int mmf_get_encoder_id(void* context, XAuint32* encoderId);
extern int mmf_get_destination_channels(void* context, XAuint32* channelsIn);
extern int mmf_get_destination_samplerate(void* context, XAmilliHertz* sampleRate);
extern int mmf_get_destination_bitrate(void* context, XAuint32* bitRate);
extern int mmf_set_destination_channels(void* context, XAuint32* channelsIn);
extern int mmf_set_destination_samplerate(void* context, XAmilliHertz* sampleRate);
extern int mmf_set_destination_bitrate(void* context, XAuint32* bitRate);

#endif /* __cplusplus */

#endif /* XA_CMMFBackendEngine_H */
