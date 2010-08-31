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

#ifndef CMMFRADIOBACKENDENGINE_H
#define CMMFRADIOBACKENDENGINE_H

#ifdef __cplusplus

#include <e32base.h>
#include <RadioUtility.h>
#include <RadioFmTunerUtility.h>
#include <RadioPlayerUtility.h>
#include <RadioPresetUtility.h>
#include <RadioFmPresetUtility.h>
#include <FMRadioTunerControl.h>
#include "xaadaptationmmf.h"

NONSHARABLE_CLASS(CMMFRadioBackendEngine) : public CBase,
                     public MRadioPlayerObserver,
                     public MRadioFmTunerObserver,
                     public MRadioPresetObserver          
    {
public:

    static CMMFRadioBackendEngine* Instance();
    void DeleteInstance();
    ~CMMFRadioBackendEngine();
    
		void SetFrequency(TInt aFreq);		   
		TInt GetFrequency(TInt& aFreq);
		TInt GetSignalStrength(TInt& aFreq);		
		void StationSeek(XAboolean aUpwards);
		void CancelSetFrequency();	
		void CancelStationSeek();	
		void SetFreqRange(TFmRadioFrequencyRange aRange);	
		TInt GetFreqRange(TFmRadioFrequencyRange& aRange);
		TInt GetFreqRangeProperties(TFmRadioFrequencyRange& aRange, TInt& aMinFreq, TInt& aMaxFreq);			
		TInt GetMaxVolume(TInt& aMaxVol);									
		TInt SetVolume(TInt aVol);		
		TInt GetVolume(TInt& aVol);
		TInt SetMute(XAboolean aMute);	
		TInt GetForcedMonoReception(XAuint32& aForcedMono);
		TInt ForceMonoReception(XAuint32 aForcedMono);
		void PlayRadio();		
		void StopRadio();		
        XAresult SetRadioAdaptContext(void * adaptcontext);  	
		XAresult SetPlayerAdaptContext(void * adaptcontext);    					
						
    /**
     * From MRadioPlayerObserver.
     * Called when Radio state changed.
     *
     * @since S60 3.2
     * @param aState Radio player state
     * @param aError A standard system error code, only used when aState is ERadioPlayerIdle
     */
    void MrpoStateChange( TPlayerState aState, TInt aError );

    /**
     * From MRadioPlayerObserver.
     * Called when volume changes. This may be caused by other applications.
     *
     * @since S60 3.2
     * @param aVolume Current volume.
     */
    void MrpoVolumeChange( TInt aVolume );

    /**
     * From MRadioPlayerObserver.
     * Called when mute setting changes. This may be caused by other applications.
     *
     * @since S60 3.2
     * @param aMute ETrue indicates audio is muted.
     */
    void MrpoMuteChange( TBool aMute );

    /**
     * From MRadioPlayerObserver.
     * Called when mute setting changes. This may be caused by other applications.
     *
     * Called when balance setting changes. This may be caused by other applications.
     *
     * @since S60 3.2
     * @param aLeftPercentage
     *        Left speaker volume percentage. This can be any value from zero to 100.
     *        Zero value means left speaker is muted.
     * @param aRightPercentage
     *        Right speaker volume percentage. This can be any value from zero to 100.
     *        Zero value means right speaker is muted.
     */
    void MrpoBalanceChange( TInt aLeftPercentage, TInt aRightPercentage );
        
    /**
     * From MRadioFmTunerObserver.
     * Called when Request for tuner control completes.
     *
     * @since S60 3.2
     * @param aError A standard system error code or FM tuner error (TFmRadioTunerError).
     */
    void MrftoRequestTunerControlComplete( TInt aError );

    /**
     * From MRadioFmTunerObserver.
     * Set frequency range complete event. This event is asynchronous and is received after
     * a call to CRadioFmTunerUtility::SetFrequencyRange.
     *
     * @since S60 3.2
     * @param aError A standard system error code or FM tuner error (TFmRadioTunerError).
     */
    void MrftoSetFrequencyRangeComplete( TInt aError );

    /**
     * From MRadioFmTunerObserver.
     * Set frequency complete event. This event is asynchronous and is received after a call to
     * CRadioFmTunerUtility::SetFrequency.
     *
     * @since S60 3.2
     * @param aError A standard system error code or FM tuner error (TFmRadioTunerError).
     */
    void MrftoSetFrequencyComplete( TInt aError );

    /**
     * From MRadioFmTunerObserver.
     * Station seek complete event. This event is asynchronous and is received after a call to
     * CRadioFmTunerUtility::StationSeek.
     *
     * @since S60 3.2
     * @param aError A standard system error code or FM tuner error (TFmRadioTunerError).
     * @param aFrequency The frequency(Hz) of the radio station that was found.
     */
    void MrftoStationSeekComplete( TInt aError, TInt aFrequency );

    /**
     * From MRadioFmTunerObserver.
     * Called when FM Transmitter status changes (if one is present in the device). Tuner receiver
     * is forced to be turned off due to hardware conflicts when FM transmitter is activated.
     *
     * @since S60 3.2
     * @param aActive ETrue if FM transmitter is active; EFalse otherwise.
     */
    void MrftoFmTransmitterStatusChange( TBool aActive );

    /**
     * From MRadioFmTunerObserver.
     * Called when antenna status changes.
     *
     * @since S60 3.2
     * @param aAttached ETrue if antenna is attached; EFalse otherwise.
     */
    void MrftoAntennaStatusChange(TBool aAttached );

    /**
     * From MRadioFmTunerObserver.
     * Called when offline mode status changes.
     * @since S60 3.2
     *
     * @param aOfflineMode ETrue if offline mode is enabled; EFalse otherwise.
     */
    void MrftoOfflineModeStatusChange(TBool aOfflineMode );

    /**
     * From MRadioFmTunerObserver.
     * Called when the frequency range changes. This may be caused by other applications.
     *
     * @since S60 3.2
     * @param aBand New frequency range.
     */
    void MrftoFrequencyRangeChange(TFmRadioFrequencyRange aBand );

    /**
     * From MRadioFmTunerObserver.
     * Called when the tuned frequency changes. This may be caused by other
     * applications or RDS if AF/TA is enabled.
     *
     * @since S60 3.2
     * @param aNewFrequency The new tuned frequency(Hz).
     */
    void MrftoFrequencyChange( TInt aNewFrequency );

    /**
     * From MRadioFmTunerObserver.
     * Called when the forced mono status change. This may be caused by other applications.
     *
     * @since S60 3.2
     * @param aForcedMono ETrue if forced mono mode is enabled; EFalse otherwise.
     */
    void MrftoForcedMonoChange( TBool aForcedMono );

    /**
     * From MRadioFmTunerObserver.
     * Called when the squelch (muting the frequencies without broadcast) status change.
     * This may be caused by other applications.
     *
     * @since S60 3.2
     * @param aSquelch ETrue if squelch is enabled; EFalse otherwise.
     */
    void MrftoSquelchChange( TBool aSquelch );

    /**
     * From MRadioPresetObserver.
     * Called when a preset changes.
     *
     * NOTE: EPresetDeleted with aIndex == 0, indicates that all presets have been deleted.
     *
     * @since S60 3.2
     * @param aChange Change event type
     * @param aIndex Index to the preset that has changed. Zero means all presets.
     */
    void MrpeoPresetChanged( TPresetChangeEvent aChange, TInt aIndex ); 
       
private:
    static CMMFRadioBackendEngine* s_instance;
    CMMFRadioBackendEngine();
    void ConstructL();
	XAresult TranslateError(TInt error);    
	XAresult SetForceMonoFlag();		 

private:      
    CRadioUtility* 				iRadioUtility; 
    CRadioFmTunerUtility* 	    iFmTunerUtility;
    CRadioPlayerUtility* 		iRadioPlayerUtility;
    TBool						iForceStereo;
    void* 						iRadioAdaptContext;
    void* 						iPlayerAdaptContext;    
    TFmRadioFrequencyRange 		iDefaultFreqRange;
    XAuint32 					iDefaultFrequency; 
    TInt 						iDefaultMinFreq;
    TInt 						iDefaultMaxFreq; 
       
    };
    
#else  /* __cplusplus */

extern void*     	cmmfradiobackendengine_init(void);
extern void      	cmmfradiobackendengine_delete(void* context);
extern void      	set_frequency(void* context, XAuint32 freq);
extern void  			cancel_set_frequency(void* context);
extern void 			station_seek(void* context, XAboolean upwards);    
extern void 			cancel_station_seek(void* context);        
extern XAresult  	get_frequency(void* context, XAuint32* freq);
extern XAresult 	get_signal_strength(void* context, XAuint32* signalStrength);
extern void 		 	set_freq_range(void* context, XAuint8 range);
extern XAresult  	get_freq_range(void* context, XAuint8* range);   
extern XAresult  	get_freq_range_properties(void* context, XAuint8 range, XAuint32* aMinFreq, XAuint32* aMaxFreq);
extern XAresult  	set_stereo_mode(void* context, XAuint32 mode);
extern XAresult  	get_stereo_mode(void* context, XAuint32* mode);
extern XAresult  	mmf_set_radio_adapt_context(void * context, void * adaptcontext);
extern XAresult  	mmf_set_player_adapt_context(void * context, void * adaptcontext);
extern void         stop_radio(void * context);
extern void         play_radio(void* context);
extern XAresult     set_volume(void* context, XAuint32 vol);
extern XAresult     get_volume(void* context, XAuint32* vol);
extern XAresult     mmf_set_player_adapt_context(void * context, void * adaptcontext);
extern XAresult     set_mute(void* context, XAboolean mute);
#endif /* __cplusplus */

#endif /* CMMFRADIOBACKENDENGINE_H */
