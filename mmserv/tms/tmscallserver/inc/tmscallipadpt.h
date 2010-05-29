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
 * Description: Telephony Multimedia Service
 *
 */

#ifndef CALLIPADAPT_H
#define CALLIPADAPT_H

// INCLUDES
#include <e32msgqueue.h>
#include <mmcccodecinformation.h>
#include "tmsclientserver.h"
#include "tmsshared.h"
#include "tmscalladpt.h"
#include "tmsipcallstream.h"
#include "tmsdtmftoneplayer.h"
#include "tmsdtmfnotifier.h"

namespace TMS {

// FORWARD DECLARATIONS
class TMSIPDownlink;
class TMSIPUplink;

/*
 * TMSCallIPAdpt class
 */
class TMSCallIPAdpt : public TMSCallAdpt,
                      public TMSIPDevSoundObserver,
                      public TMSDTMFTonePlayerObserver
    {
public:
    static TMSCallIPAdpt* NewL();
    virtual ~TMSCallIPAdpt();
    virtual gint PostConstruct();

    // From TMSStream
    virtual gint CreateStream(const TMSCallType callType,
            const TMSStreamType strmType, gint& outStrmId);
    virtual gint InitStream(const TMSCallType callType,
            const TMSStreamType strmType, const gint strmId,
            const TMSFormatType frmtType, const gint retryTime,
            const RMessage2& message);
    virtual gint StartStream(const TMSCallType callType,
            const TMSStreamType strmType, const gint strmId,
            const gint retrytime);
    virtual gint PauseStream(const TMSCallType callType,
            const TMSStreamType strmType, const gint strmId);
    virtual gint StopStream(const TMSCallType callType,
            const TMSStreamType strmType, const gint strmId);
    virtual gint DeinitStream(const TMSCallType callType,
            const TMSStreamType strmType, const gint strmId);
    virtual gint DeleteStream(const TMSCallType callType,
            const TMSStreamType strmType, const gint strmId);
    virtual gint DataXferBufferEmptied(const TMSCallType callType,
            const TMSStreamType strmType, const gint strmId);
    virtual gint DataXferBufferFilled(const TMSCallType callType,
            const TMSStreamType strmType, const gint strmId,
            const guint datasize);
    virtual gint GetDataXferBufferHndl(const TMSCallType callType,
            const TMSStreamType strmType, const gint strmId,
            const guint32 key, RChunk& chunk);

    // From TMS effects
    virtual gint GetMaxVolume(guint& volume);
    virtual gint SetVolume(const guint volume);
    virtual gint GetVolume(guint& volume);
    virtual gint GetMaxGain(guint& gain);
    virtual gint SetGain(const guint gain);
    virtual gint GetGain(guint& gain);
    virtual gint GetGlobalMaxVolume(guint& volume);
    virtual gint SetGlobalVolume(const guint volume);
    virtual gint GetGlobalVolume(guint& volume);
    virtual gint GetGlobalMaxGain(guint& gain);
    virtual gint SetGlobalGain(const guint gain);
    virtual gint GetGlobalGain(guint& gain);

    // From TMS formats
    virtual gint GetCodecMode(const TMSFormatType fmttype,
            const TMSStreamType strmtype, gint& mode);
    virtual gint SetCodecMode(const TMSFormatType fmttype,
            const TMSStreamType strmtype, const gint mode);
    virtual gint GetSupportedBitRatesCount(guint& count);
    virtual gint GetSupportedBitRates(CBufFlat*& brbuffer);
    virtual gint GetBitRate(guint& bitrate);
    virtual gint SetBitRate(const guint bitrate);
    virtual gint GetVAD(const TMSFormatType fmttype, gboolean& vad);
    virtual gint SetVAD(const TMSFormatType fmttype, const gboolean vad);
    virtual gint GetCNG(const TMSFormatType fmttype, gboolean& cng);
    virtual gint SetCNG(const TMSFormatType fmttype, const gboolean cng);
    virtual gint GetPlc(const TMSFormatType fmttype, gboolean& plc);
    virtual gint SetPlc(const TMSFormatType fmttype, const gboolean plc);

    // From TMS audio routing
    virtual gint SetOutput(const TMSAudioOutput output);
    virtual gint GetOutput(TMSAudioOutput& output);
    virtual gint GetPreviousOutput(TMSAudioOutput& output);
    virtual gint GetAvailableOutputsL(gint& count, CBufFlat*& outputsbuf);

	// From TMSDTMF
    virtual gint StartDTMF(const TMSStreamType streamtype, TDes& dtmfstr);
    virtual gint StopDTMF(const TMSStreamType streamtype);
    virtual gint ContinueDTMF(const gboolean sending);

    // From TMS codec formats
    gint SetIlbcCodecMode(const gint mode, const TMSStreamType strmtype);
    gint GetIlbcCodecMode(gint& mode, const TMSStreamType strmtype);
    gint SetG711CodecMode(const gint mode, const TMSStreamType strmtype);
    gint GetG711CodecMode(gint& mode, const TMSStreamType strmtype);
    gint FrameModeRqrdForEC(gboolean& frmodereq);
    gint SetFrameMode(const gboolean frmode);
    gint GetFrameMode(gboolean& frmode);
    gint ConcealErrorForNextBuffer();
    gint BadLsfNextBuffer();

    void SetFormat(const gint strmId, const guint32 aFormat);

    void BufferFilledL(guint dataSize);
    void BufferEmptiedL();
    gint GetDataXferChunkHndl(const TMSStreamType strmType,
            const guint32 key, RChunk& chunk);

    //From TMSIPDevSoundObserver
    void DownlinkInitCompleted(gint status);
    void UplinkInitCompleted(gint status);
    void DownlinkStarted(gint status);
    void UplinkStarted(gint status);

    //From TMSDTMFTonePlayerObserver
    void DTMFInitCompleted(gint status);
    void DTMFToneFinished(gint status);

private:
    TMSCallIPAdpt();
    void ConstructL();

    gint OpenDownlink(const RMessage2& message, const gint retrytime);
    gint OpenUplink(const RMessage2& message, const gint retrytime);

    gint InitDTMF(TMSStreamType strmtype);
    void GetSupportedBitRatesL(CBufFlat*& brbuffer);
    void NotifyClient(const gint strmId, const gint command,
            const gint status = KErrNone, const gint64 int64 = TInt64(0));

private:
    gint iNextStreamId;
    TMSIPUplink* iIPUplink;
    TMSIPDownlink* iIPDownlink;
    TMSAudioDtmfTonePlayer* iDTMFDnlinkPlayer;
    TMSDtmfNotifier* iDTMFNotifier;
    TMSAudioDtmfTonePlayer* iDTMFUplinkPlayer;
    // Message queues for communication and data transfer back to the client
    RMsgQueue<TmsMsgBuf> iMsgQueueUp;
    RMsgQueue<TmsMsgBuf> iMsgQueueDn;
    TmsMsgBuf iMsgBuffer;

    gboolean iUplinkInitialized;
    gint iUplinkStreamId;
    gboolean iDnlinkInitialized;
    gint iDnlinkStreamId;

    TMMFPrioritySettings iPriority;
    guint32 iUpFourCC;
    guint32 iDnFourCC;
    gint iMaxVolume;
    gint iMaxGain;
    RArray<guint> iArrBitrates;
    RArray<TFourCC> iCodecs;
    gint iCodecsCount;
    };

} //namespace TMS

#endif //CALLIPADPT_H

// End of file
