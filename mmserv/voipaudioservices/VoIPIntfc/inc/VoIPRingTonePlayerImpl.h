/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  VOIP Audio Services
*
*/


#ifndef __RINGTONEPLAYERIMPL_H
#define __RINGTONEPLAYERIMPL_H

#include <voipringtoneplayer.h>
#include "VoIPAudioSession.h"
#include "VoIPQueueHandler.h"


// FORWARD DECLARATIONS
class RFile;
class RVoIPAudioSession;
class CVoIPDataBuffer;

// CONSTANTS


/**
 *  CRingTonePlayerImpl class
 *
 *  Ring Tone Player class
 *
 *  @lib VoIPAudioIntfc.dll
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CRingTonePlayerImpl) : public CBase,
                                         public CRingTonePlayer,
                                         public MQueueHandlerObserver
    {
public:

    /**
     * 2-phase constructor
     *
     * @since S60 v5.0
     * @param none
     * @return CRingTonePlayerImpl* -
     */
    static CRingTonePlayerImpl* NewL();

    /**
     * Destructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    virtual ~CRingTonePlayerImpl();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param MRingToneObserver&
     * @return TInt
     */
    void OpenL(MRingToneObserver& aObserver);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param MRingToneObserver&
     * @param TPtr
     * @return TInt
     */
    void OpenL(MRingToneObserver& aObserver, TPtr aFileName);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param MRingToneObserver&
     * @param RFile&
     * @return TInt
     */
    void OpenL(MRingToneObserver& aObserver, RFile& aFileHandle);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param MRingToneObserver&
     * @param TPtr8
     * @param TPtr8
     * @return TInt
     */
    void OpenL(MRingToneObserver& aObserver, TPtr8 aDesTone, TPtr8 aMimeType);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    TInt Play();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    TInt Pause();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    TInt Resume();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    TInt Stop();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    void Close();

// From MQueueHandlerObserver

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TPtr8
     * @return void
     */
    virtual void FillBuffer(CVoIPDataBuffer* /*aBuffer*/) {}

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TPtr8
     * @return void
     */
    virtual void EmptyBuffer(CVoIPDataBuffer* /*aBuffer*/) {}

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TInt
     * @param TInt
     * @return void
     */
    virtual void Event(TInt aEventType, TInt aError);

private:

    /**
     * Constructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    CRingTonePlayerImpl();

    /**
     * Symbian constructor
     *
     * @since S60 v5.0
     * @param none
     * @return void
     */
    void ConstructL();

    /**
     * @since S60 v5.0
     * @param none
     * @return void
     */
    void StartMsgQueueL();

    /**
     * @since S60 v5.0
     * @param none
     * @return void
     */
    void ReceiveMsgQComHandlerEventsL();

private:

// Communication queue
    RMsgQueue<TVoIPMsgBuf> iMsgComQueue;
    CQueueHandler*         iMsgQComHandler;

    MRingToneObserver*     iObserver;
    RVoIPAudioSession      iVoIPAudioSession;

    };

#endif //__RINGTONEPLAYERIMPL_H

// End of file
