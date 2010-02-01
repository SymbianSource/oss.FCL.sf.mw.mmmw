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


#ifndef __VOIPUPLINKSTREAMIMPL_H
#define __VOIPUPLINKSTREAMIMPL_H

// INCLUDES
#include <voipformatintfc.h>
#include <voipuplinkstream.h>
#include "VoIPAudioSession.h"
#include "VoIPQueueHandler.h"


/**
 *  CVoIPAudioUplinkStreamImpl class
 *
 *  ?more_complete_description
 *
 *  @lib VoIPAudioIntfc.dll
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CVoIPAudioUplinkStreamImpl) : public CBase,
                                                public CVoIPAudioUplinkStream,
                                                public MQueueHandlerObserver,
                                                public MVoIPFormatObserver
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
     * @return CVoIPAudioUplinkStreamImpl*
     */
    static CVoIPAudioUplinkStreamImpl* NewL(
                const TMMFPrioritySettings aPriority);

    /**
     * Destructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    virtual ~CVoIPAudioUplinkStreamImpl();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param MVoIPUplinkObserver&
     * @return TInt
     */
    TInt Open(MVoIPUplinkObserver& aObserver);

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
    TInt GetMaxGain(TInt& aGain);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TInt
     * @return TInt
     */
    TInt SetGain(TInt aGain);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TInt&
     * @return TInt
     */
    TInt GetGain(TInt& aGain);

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
    TInt BufferEmptied(CVoIPDataBuffer* aBuffer);

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
    virtual void FillBuffer(CVoIPDataBuffer* /*aBuffer*/) {};

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param CVoIPDataBuffer*
     * @return void
     */
    virtual void EmptyBuffer(CVoIPDataBuffer* aBuffer);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param aEventType
     * @param TInt
     * @return void
     */
    virtual void Event(TInt aEventType, TInt aError);

// From MVoIPFormatObserver

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param CVoIPFormatIntfc&
     * @param TInt
     * @return void
     */
    virtual void Event(const CVoIPFormatIntfc& aSrc, TInt aEventType);

private:

    /**
     * Constructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    CVoIPAudioUplinkStreamImpl();

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

// Client side VoIP server session
    RVoIPAudioSession         iVoIPAudioSession;

    MVoIPUplinkObserver*      iObserver;

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
    TBool                     iIsClosing;

    TVoIPCodecFormat          iFormat;
    RArray<TVoIPCodecFormat>* iCodecFormats;

    };

#endif //__VOIPUPLINKSTREAMIMPL_H

// End of file
