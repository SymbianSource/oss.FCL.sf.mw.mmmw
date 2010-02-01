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
* Description:  Defines APIs common to all supported codecs.
*
*/


#ifndef __VOIPJITTERBUFFERINTFC_H
#define __VOIPJITTERBUFFERINTFC_H

//  INCLUDES
#include <e32base.h>
#include <voipaudiocommon.h>


// FORWARD DECLARATIONS
class CVoIPJitterBufferIntfc;
class CVoIPJitterBufferIntfcImpl;


/**
 *  TVoIPJBConfig
 *  ?description
 *  @since S60 v5.0
 */
class TVoIPJBConfig
    {
public:
    // Jitter latency
    TUint iJitterLatency;

    // Frame time
    TUint8 iSampleInterval;

    //Jitter Buffer buffer queue size
    TUint iJBBufferLength;

    //Jitter Buffer threshold
    TUint iJBThreshold;

    //Jitter Buffer InactivityTimeOut
    TUint iJBInactivityTimeOut;

    //Jitter Buffer tone TimeOut
    TUint iJBPlayToneTimeout;

    //Jitter Buffer tone frequecy
    TUint iJBPlayToneFrequency;

    //Jitter Buffer tone duration
    TUint iJBPlayToneDuration;
    };


/**
 *  MVoIPJitterBufferObserver class
 *
 *  description
 *
 *  @lib VoIPAudioIntfc.dll
 *  @since S60 v5.0
 */
class MVoIPJitterBufferObserver
    {
public:
    static const TUint KJBReserved1 = 1; //TBD
    static const TUint KJBReserved2 = 2; //TBD

public:
    /**
     * ?description
     *
     * @since S60 v5.0
     * @param const CVoIPJitterBufferIntfc&
     * @param TInt - event type
     * @return void
     */
    virtual void Event(const CVoIPJitterBufferIntfc& aSrc,
                       TInt aEventType) = 0;
    };


/**
 *  CVoIPFormatIntfc class
 *
 *  Base class for codec format configuration
 *
 *  @lib VoIPAudioIntfc.dll
 *  @since S60 v5.0
 */
class CVoIPJitterBufferIntfc
    {
public:
    /**
     * Destructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    IMPORT_C virtual ~CVoIPJitterBufferIntfc();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param MVoIPFormatObserver&
     * @return TInt
     */
    IMPORT_C TInt SetObserver(MVoIPJitterBufferObserver& aObserver);


    /**
     * ConfigureJitterBuffer
     * @since S60 v5.0
     * @param TVoIPJBConfig&
     * @return void
     */
    IMPORT_C TInt ConfigureJitterBuffer(const TVoIPJBConfig& aJbConfig);

    /**
     * ResetJitterbuffer
     * @since S60 v5.0
     * @param aPlayTone ETrue, if tone should be played
     * @return void
     */
    IMPORT_C TInt ResetJitterBuffer(TBool aPlayTone = ETrue);

    /**
     * DelayDown
     * @since S60 v5.0
     * @param none
     * @return void
     */
    IMPORT_C TInt DelayDown();

    /**
     * DelayUp
     * @since S60 v5.0
     * @param none
     * @return void
     */
    IMPORT_C TInt DelayUp();

protected:

    /**
     * Constructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
      CVoIPJitterBufferIntfc();

    /**
     * 2-nd phase constructor
     *
     * @since S60 v5.0
     * @param CVoIPJitterBufferIntfcImpl*
     * @return void
     */
    void ConstructL(CVoIPJitterBufferIntfcImpl* aVoIPJitterBufferIntfcImpl);

protected:
    CVoIPJitterBufferIntfcImpl* iVoIPJitterBufferIntfcImpl;

    };

#endif  //__VOIPJITTERBUFFERINTFC_H


// End of File
