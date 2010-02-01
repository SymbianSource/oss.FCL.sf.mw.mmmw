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


#ifndef __VOIPDTMFTONEPLAYERIMPL_H
#define __VOIPDTMFTONEPLAYERIMPL_H

// INCLUDES
#include <voipdtmftoneplayer.h>
#include "VoIPQueueHandler.h"

// CONSTANTS

// FORWARD DECLARATIONS
class CVoIPAudioUplinkStream;
class CVoIPAudioDownlinkStream;
class CVoIPDataBuffer;
class RVoIPAudioSession;


/**
 *  CDTMFTonePlayerImpl class
 *
 *  DTMF Tone Player class
 *
 *  @lib VoIPAudioIntfc.dll
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CDTMFTonePlayerImpl) : public CBase,
                                         public CDTMFTonePlayer,
                                         public MQueueHandlerObserver
    {
    enum TStreamType
        {
        EDownlink,
        EUplink
        };

public:

    /**
     * 2-phase constructor
     *
     * @since S60 v5.0
     * @param none
     * @return CDTMFTonePlayerImpl* -
     */
    static CDTMFTonePlayerImpl* NewL();

    /**
     * Destructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    virtual ~CDTMFTonePlayerImpl();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    TInt Open(MDTMFToneObserver& aObserver);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TPtr -
     * @return TInt -
     */
    TInt Play(const TPtr aTones);

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
     * @return void
     */
    void Close();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param CVoIPAudioUplinkStream& -
     * @return TInt
     */
    TInt AddVoIPAudioUplinkStream(CVoIPAudioUplinkStream& aStream,
                                  RVoIPAudioSession* aSession);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param CVoIPAudioUplinkStream&
     * @return TInt
     */
    TInt RemoveVoIPAudioUplinkStream(CVoIPAudioUplinkStream& aStream);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param CVoIPAudioDownlinkStream& -
     * @return TInt
     */
    TInt AddVoIPAudioDownlinkStream(CVoIPAudioDownlinkStream& aStream,
                                    RVoIPAudioSession* aSession);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param CVoIPAudioDownlinkStream& -
     * @return TInt
     */
    TInt RemoveVoIPAudioDownlinkStream(CVoIPAudioDownlinkStream& aStream);

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
    CDTMFTonePlayerImpl();

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

    MDTMFToneObserver*     iObserver;
    RVoIPAudioSession*     iVoIPAudioSession;

    TStreamType            iStreamType;

    };

#endif //__VOIPDTMFTONEPLAYERIMPL_H


// End of file
