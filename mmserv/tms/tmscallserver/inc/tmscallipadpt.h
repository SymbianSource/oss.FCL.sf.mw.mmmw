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
#include "tmscalladpt.h"

namespace TMS {

// FORWARD DECLARATIONS
class TMSIPDownlink;
class TMSIPUplink;

// TMSCallIPAdpt class
class TMSCallIPAdpt : public TMSCallAdpt
    {
public:
    // Constractor
    static TMSCallIPAdpt* NewL();
    virtual ~TMSCallIPAdpt();
    virtual gint PostConstruct();

    // From TMSStream
    virtual gint CreateStream(TMSCallType callType, TMSStreamType strmType,
            gint& outStrmId);
    virtual gint InitStream(TMSCallType callType, TMSStreamType strmType,
            gint strmId, TMSFormatType frmtType, const RMessage2& message);
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

    // From TMS audio output
    virtual gint SetOutput(TMSAudioOutput output);
    virtual gint GetOutput(TMSAudioOutput& output);
    virtual gint GetPreviousOutput(TMSAudioOutput& output);
    virtual gint GetAvailableOutputsL(gint& count, CBufFlat*& outputsbuffer);

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

    gint OpenDownlinkL(const RMessage2& message);
    gint OpenUplinkL(const RMessage2& message);
    void SetFormat(const gint strmId, const guint32 aFormat);

    void BufferFilledL(guint dataSize);
    void BufferEmptiedL();
    gint GetDataXferChunkHndl(const TMSStreamType strmType,
            const guint32 key, RChunk& chunk);

private:
    void ConstructL();
    TMSCallIPAdpt();

    void NotifyClient(const gint strmId, const gint aCommand,
            const gint aStatus = KErrNone, const gint64 aInt64 = gint64(0));
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
    guint32 iUpFourCC;
    guint32 iDnFourCC;
    gint iMaxVolume;
    gint iMaxGain;
    RArray<guint> iArrBitrates;
    RArray<TFourCC> iCodecs;
    gint iCodecsCount;

    };

} //namespace TMS

#endif //__CALLIPADAPT_H

// End of file
