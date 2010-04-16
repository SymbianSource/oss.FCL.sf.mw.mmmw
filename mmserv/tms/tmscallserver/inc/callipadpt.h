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

#ifndef __CALLIPADAPT_H
#define __CALLIPADAPT_H

// INCLUDES
#include <e32msgqueue.h>
#include <mmcccodecinformation.h>
#include "tmsclientserver.h"
#include "tmsshared.h"
#include "calladpt.h"
#include "cpeaudiodtmftoneplayer.h"
#include "dtmfnotifier.h"

namespace TMS {

// FORWARD DECLARATIONS
class TMSIPDownlink;
class TMSIPUplink;

// TMSCallIPAdpt class
class TMSCallIPAdpt : public TMSCallAdpt,
                      public TMSDTMFTonePlayerObserver
    {
public:
    // Constractor
    static TMSCallIPAdpt* NewL();

    virtual ~TMSCallIPAdpt();
    virtual gint PostConstruct();

    virtual gint CreateStream(TMSCallType callType, TMSStreamType strmType,
            gint& outStrmId);
    virtual gint InitStreamL(TMSCallType callType, TMSStreamType strmType,
            gint strmId, TMSFormatType frmtType, const RMessage2& aMessage);
    virtual gint StartStream(TMSCallType callType, TMSStreamType strmType,
            gint strmId);
    virtual gint PauseStream(TMSCallType callType, TMSStreamType strmType,
            gint strmId);
    virtual gint StopStream(TMSCallType callType, TMSStreamType strmType,
            gint strmId);
    virtual gint DeinitStream(TMSCallType callType, TMSStreamType strmType,
            gint strmId);
    virtual gint DeleteStream(TMSCallType callType, TMSStreamType strmType,
            gint strmId);
    virtual gint DataXferBufferEmptied(TMSCallType callType,
            TMSStreamType strmType, gint strmId);
    virtual gint DataXferBufferFilled(TMSCallType callType,
            TMSStreamType strmType, gint strmId, guint datasize);
    virtual gint GetDataXferBufferHndl(const TMSCallType callType,
            const TMSStreamType strmType, const gint strmId,
            const guint32 key, RChunk& chunk);

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

    virtual gint SetOutput(TMSAudioOutput output);
    virtual gint GetOutput(TMSAudioOutput& output);
    virtual gint GetPreviousOutput(TMSAudioOutput& output);
    virtual gint GetAvailableOutputsL(TInt& count, CBufFlat*& outputsbuffer);

    virtual gint StartDTMF(TMSStreamType streamtype, TDes& dtmfstring);
    virtual gint StopDTMF(TMSStreamType streamtype);
    virtual gint ContinueDTMF(TBool continuesending);

    gint SetIlbcCodecMode(const gint mode, const TMSStreamType strmtype);
    gint GetIlbcCodecMode(gint& mode, const TMSStreamType strmtype);
    gint SetG711CodecMode(const gint mode, const TMSStreamType strmtype);
    gint GetG711CodecMode(gint& mode, const TMSStreamType strmtype);
    gint FrameModeRqrdForEC(gboolean& frmodereq);
    gint SetFrameMode(const gboolean frmode);
    gint GetFrameMode(gboolean& frmode);
    gint ConcealErrorForNextBuffer();
    gint BadLsfNextBuffer();

    gint OpenDownlinkL(const RMessage2& aMessage);
    gint OpenUplinkL(const RMessage2& aMessage);
    void SetFormat(const gint strmId, const TUint32 aFormat);

    void BufferFilledL(TUint dataSize);
    void BufferEmptiedL();
    gint GetDataXferChunkHndl(const TMSStreamType strmType,
            const TUint32 key, RChunk& chunk);

    //From DTMFTonePlayerObserver
     void DTMFInitCompleted(TInt error);
     void DTMFToneFinished(TInt error);

private:
    void ConstructL();
    TMSCallIPAdpt();

    void NotifyClient(const gint strmId, const TInt aCommand,
            const TInt aStatus = KErrNone, const TInt64 aInt64 = TInt64(0));
    //void DetermineG711FrameRateL(); //G711 10/20ms
    void GetSupportedBitRatesL(CBufFlat*& brbuffer);

private:
    gint iNextStreamId;
    gboolean iUplinkInitialized;
    gint iUplinkStreamId;
    gboolean iDnlinkInitialized;
    gint iDnlinkStreamId;

    TMSIPDownlink* iIPDownlink;
    TMSIPUplink* iIPUplink;

    // Message queues for communication and data transfer back to the client
    RMsgQueue<TmsMsgBuf> iMsgQueueUp;
    RMsgQueue<TmsMsgBuf> iMsgQueueDn;

    TmsMsgBuf iMsgBuffer;
    TMMFPrioritySettings iPriority;
    TUint32 iUpFourCC;
    TUint32 iDnFourCC;
    TInt iMaxVolume;
    TInt iMaxGain;
    RArray<TUint> iArrBitrates;
    RArray<TFourCC> iCodecs;
    TInt iCodecsCount;

    TMSAudioDtmfTonePlayer* iDTMFDnlinkPlayer;
    TMSAudioDtmfTonePlayer* iDTMFUplinkPlayer;
    TMSDtmfNotifier* iDTMFNotifier;
    };

} //namespace TMS

#endif //__CALLIPADAPT_H

// End of file
