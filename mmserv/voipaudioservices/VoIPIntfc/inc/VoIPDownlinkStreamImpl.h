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


#ifndef __VOIPDOWNLINKSTREAMIMPL_H
#define __VOIPDOWNLINKSTREAMIMPL_H


// INCLUDES
#include <voipdownlinkstream.h>
#include "VoIPAudioSession.h"
#include "VoIPDtmfTonePlayerImpl.h"
#include "VoIPQueueHandler.h"


/**
 *  CVoIPAudioDownlinkStreamImpl class
 *
 *  ?more_complete_description
 *
 *  @lib VoIPAudioIntfc.dll
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CVoIPAudioDownlinkStreamImpl) :
                    public CBase,
                    public CVoIPAudioDownlinkStream,
                    public MQueueHandlerObserver
    {
    enum TQueueType
        {
        EMsgBufQueue,
        EMsgComQueue
        };

public:

    /**
     * 2-phased constructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    static CVoIPAudioDownlinkStreamImpl* NewL(
                    const TMMFPrioritySettings aPriority);

    /**
     * Destructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    virtual ~CVoIPAudioDownlinkStreamImpl();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param MVoIPDownlinkObserver&
     * @return TInt
     */
    TInt Open(MVoIPDownlinkObserver& aObserver);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TVersion&
     * @return TInt
     */
    TInt GetVersion(TVersion& aVersion);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TInt&
     * @return TInt
     */
    TInt GetMaxVolume(TInt& aVolume);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TInt
     * @return TInt
     */
    TInt SetVolume(TInt aVolume);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TInt&
     * @return TInt
     */
    TInt GetVolume(TInt& aVolume);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param const TMMFPrioritySettings
     * @param RArray<TVoIPCodecFormat>&
     * @return void
     */
    void GetSupportedFormatsL(const TMMFPrioritySettings aPriority,
                              RArray<TVoIPCodecFormat>& aFormats);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TVoIPCodecFormat
     * @param CVoIPFormatIntfc&
     * @return void
     */
    void SetFormatL(TVoIPCodecFormat aFormat, CVoIPFormatIntfc*& aIntfc);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TVoIPCodecFormat
     */
    TVoIPCodecFormat GetFormat();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TVoIPOutputDevice
     * @return TInt
     */
    TInt SetAudioDevice(const TVoIPOutputDevice aDevice);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TVoIPOutputDevice&
     * @return TInt
     */
    TInt GetAudioDevice(TVoIPOutputDevice& aDevice);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    TInt Start();

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
     * @param CVoIPDataBuffer*
     * @return TInt
     */
    TInt BufferFilled(CVoIPDataBuffer* aBuffer);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param CVoIPJitterBufferIntfc*&
     * @return void
     */
    void CreateJitterBufferIntfcL(CVoIPJitterBufferIntfc*& aJBIntfc);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return RVoIPAudioSession*
     */
    RVoIPAudioSession* GetSession();


// From MQueueHandlerObserver

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param CVoIPDataBuffer*
     * @return void
     */
    virtual void FillBuffer(CVoIPDataBuffer* aBuffer);

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
     * @return TInt
     */
    CVoIPAudioDownlinkStreamImpl();

    /**
     * Symbian constructor
     *
     * @since S60 v5.0
     * @param none
     * @return void
     */
    void ConstructL(const TMMFPrioritySettings aPriority);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TDesC&
     * @param TInt
     * @param TQueueType
     * @return TInt
     */
    TInt CreateQueue(const TDesC& aQueueName,
                     const TInt aNumSlots,
                     const TQueueType aQueueType);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return void
     */
    void ReceiveMsgQHandlerEventsL();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return void
     */
    void ReceiveMsgQComHandlerEventsL();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TVoIPCodecFormat
     * @return TFourCC
     */
    TUint32 CodecFourCC(TVoIPCodecFormat aFormat);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TUint32
     * @return TVoIPCodecFormat
     */
    TVoIPCodecFormat ConvertFourCC(TUint32 aCodec);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TVoIPCodecFormat
     * @return TBool
     */
    TBool IsCodecSupportedL(TVoIPCodecFormat aFormat);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TVoIPCodecFormat
     * @return TBool
     */
    TBool FindFormat(TVoIPCodecFormat aFormat);

private:
    friend class CDTMFTonePlayerImpl;

private:
// Client side VoIP server session
    RVoIPAudioSession         iVoIPAudioSession;

    MVoIPDownlinkObserver*    iObserver;

// Message queues
    RMsgQueue<TVoIPMsgBuf>    iMsgQueue;
    RMsgQueue<TVoIPMsgBuf>    iMsgComQueue;

// Message queue handlers
    CQueueHandler*            iMsgQHandler;
    CQueueHandler*            iMsgQComHandler;

    TMMFPrioritySettings      iPriority;
    TPtr8                     iBufferPtr;
    TInt                      iBufferLen;
    TInt                      iG711FrameSize;

    TVoIPCodecFormat          iFormat;
    RArray<TVoIPCodecFormat>* iCodecFormats;

    };

#endif //__VOIPDOWNLINKSTREAMIMPL_H

// End of file
