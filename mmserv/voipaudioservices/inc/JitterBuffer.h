/*
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Jitter buffer for audio frames.
 *  Interface   : VoIPJitterBuffer.lib
 *
 */

#ifndef __VOIPJITTERBUFFER_H
#define __VOIPJITTERBUFFER_H

// DATA TYPES

// FORWARD DECLARATIONS
class CMccCnGenerator;
class CVoIPJitterBufferImpl;
class TVoIPJBConfig;
class MJitterBufferObserver;

// CLASS DECLARATION

/**
 *  Jitter buffer.
 *
 *  @lib VoIPJitterBuffer.lib
 *  @since Series 60 5.0
 */
class CVoIPJitterBuffer : public CActive
    {
private:
    /**
     * Jitter buffer states
     */
    enum TJBState
        {
        EJBufStopped,
        EJBufPlaying,
        EJBufPaused
        };

    /**
     * Transition states during JB playback
     */
    enum TJBTransitionState
        {
        EWait,
        //    ENeedData,
        EEmptyData
        };

public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CVoIPJitterBuffer* NewL(MJitterBufferObserver* aObserver);

    /**
     * Destructor.
     */
    virtual ~CVoIPJitterBuffer();

public:
    // Methods

    /**
     * SetupL
     * Sets codec information and initializes JB.
     * @since Series 60 5.0
     * @param TFourCC - codec's fourcc
     * @param TVoIPJBConfig& - JB configuraion parameters
     * @return void
     */
    IMPORT_C void SetupL(const TFourCC aCodec, const TVoIPJBConfig& aJBConfig);

    /**
     * ResetBuffer
     * Resets JB to its original state with no data.
     * @since Series 60 5.0
     * @param TBool - ETrue == tone is to be played
     * @return void
     */
    IMPORT_C void ResetBuffer(TBool aPlayTone = ETrue);

    /**
     * Delay Down
     *
     * @since Series 60 5.0
     * @param none
     * @return void
     */
    IMPORT_C void DelayDown();

    /**
     * Delay Up
     *
     * @since Series 60 5.0
     * @param none
     * @return void
     */
    IMPORT_C void DelayUp();

    /**
     * Play
     *
     * @since Series 60 5.0
     * @param none
     * @return void
     */
    IMPORT_C void Play();

    /**
     * Pause
     *
     * @since Series 60 5.0
     * @param none
     * @return void
     */
    IMPORT_C void Pause();

    /**
     * Stop
     *
     * @since Series 60 5.0
     * @param none
     * @return void
     */
    IMPORT_C void Stop();

    /**
     * FillBuffer
     * Retrieves next frame from JB queue.
     * @since Series 60 5.0
     * @param CMMFBuffer* - buffer to be filled with data from the JB
     * @return TInt
     */
    IMPORT_C TInt FillBuffer(CMMFBuffer* aBuffer);

    /**
     * EmptyBufferL
     * Queues received frame in JB.
     * @since Series 60 5.0
     * @param CMMFBuffer* - buffer to be queued in the JB
     * @return TInt
     */
    IMPORT_C TInt EmptyBuffer(CMMFBuffer* aBuffer);

private:

    /**
     * C++ default constructor.
     */
    CVoIPJitterBuffer(MJitterBufferObserver* aObserver);

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

    /**
     * from CActive
     */
    void DoCancel();

    /**
     * from CActive
     */
    void RunL();

    /**
     * from CActive
     */
    TInt RunError(TInt aError);

private:
    // Internal helper methods

    /**
     * Play current frame from JB.
     * @since Series 60 5.0
     * @param none
     * @return void
     */
    void PlayBuffer();

    /**
     * TransitionState
     * @since Series 60 5.0
     * @param TJBTransitionState
     * @param TUint32
     * @return void
     */
    void TransitionState(TJBTransitionState aTransitionState,
            TUint32 aStateChangeDelay = 0);

    /**
     * Calculates the time interval at which data rate should be slowed down
     * when subsequent EmptyBufferL calls are coming too fast.
     * @since Series 60 5.0
     * @param none
     * @return Suggested delay in microseconds.
     */
    TUint32 DetermineEmptyBufferDelay();

private:
    // Data

    // Jitter buffer Implementation
    CVoIPJitterBufferImpl* iJitterBufferImpl;

    // Pointer to databuffer
    CMMFBuffer* iPlayBuffer;

    // Jitterbuffer state
    TJBState iState;

    // JB Transition State
    TJBTransitionState iTransitionState;

    MJitterBufferObserver* iObserver;
    TFourCC iCodec;

    RTimer iTimer;
    TTime iTmCurrentEmptyBuffer;
    TTime iTmPreviousEmptyBuffer;

    TUint32 iSampleInterval;
    TUint32 iEmptyBufferDelayThreshold;

    // Current request size of the buffer
    TInt iRequestSize;

    };

#endif  //__VOIPJITTERBUFFER_H

// End of file
