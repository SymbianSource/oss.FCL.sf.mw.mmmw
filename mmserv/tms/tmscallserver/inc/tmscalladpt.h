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

#ifndef CALLADPT_H
#define CALLADPT_H

#include <e32base.h>
#include <tms.h>

namespace TMS {

// TMSCallAdpt class
class TMSCallAdpt
    {
public:
    static gint CreateCallL(gint callType, TMSCallAdpt*& callAdpt);
    virtual ~TMSCallAdpt();

    // From TMSStream
    virtual gint CreateStream(TMSCallType callType,
            TMSStreamType strmType, gint& outStrmId) = 0;
    virtual gint InitStream(TMSCallType callType, TMSStreamType strmType,
            gint strmId, TMSFormatType frmtType, const RMessage2& aMessage) = 0;
    virtual gint StartStream(TMSCallType callType,
            TMSStreamType strmType, gint strmId) = 0;
    virtual gint PauseStream(TMSCallType callType,
            TMSStreamType strmType, gint strmId) = 0;
    virtual gint StopStream(TMSCallType callType, TMSStreamType strmType,
            gint strmId) = 0;
    virtual gint DeinitStream(TMSCallType callType,
            TMSStreamType strmType, gint strmId) = 0;
    virtual gint DeleteStream(TMSCallType callType,
            TMSStreamType strmType, gint strmId) = 0;
    virtual gint DataXferBufferEmptied(TMSCallType callType,
            TMSStreamType strmType, gint strmId) = 0;
    virtual gint DataXferBufferFilled(TMSCallType callType,
            TMSStreamType strmType, gint strmId, guint datasize) = 0;
    virtual gint GetDataXferBufferHndl(const TMSCallType callType,
            const TMSStreamType strmType, const gint strmId,
            const guint32 key, RChunk& chunk) = 0;

    // From TMS effects
    virtual gint GetMaxVolume(guint& volume) = 0;
    virtual gint SetVolume(const guint volume) = 0;
    virtual gint GetVolume(guint& volume) = 0;
    virtual gint GetMaxGain(guint& gain) = 0;
    virtual gint SetGain(const guint gain) = 0;
    virtual gint GetGain(guint& gain) = 0;
    virtual gint GetGlobalMaxVolume(guint& volume) = 0;
    virtual gint SetGlobalVolume(const guint volume) = 0;
    virtual gint GetGlobalVolume(guint& volume) = 0;
    virtual gint GetGlobalMaxGain(guint& gain) = 0;
    virtual gint SetGlobalGain(const guint gain) = 0;
    virtual gint GetGlobalGain(guint& gain) = 0;

    // From TMS formats
    virtual gint GetCodecMode(const TMSFormatType fmttype,
            const TMSStreamType strmtype, gint& mode) = 0;
    virtual gint SetCodecMode(const TMSFormatType fmttype,
            const TMSStreamType strmtype, const gint mode) = 0;
    virtual gint GetSupportedBitRatesCount(guint& count) = 0;
    virtual gint GetSupportedBitRates(CBufFlat*& brbuffer) = 0;
    virtual gint GetBitRate(guint& bitrate) = 0;
    virtual gint SetBitRate(const guint bitrate) = 0;
    virtual gint GetVAD(const TMSFormatType fmttype, gboolean& vad) = 0;
    virtual gint SetVAD(const TMSFormatType fmttype, const gboolean vad) = 0;
    virtual gint GetCNG(const TMSFormatType fmttype, gboolean& cng) = 0;
    virtual gint SetCNG(const TMSFormatType fmttype, const gboolean cng) = 0;
    virtual gint GetPlc(const TMSFormatType fmttype, gboolean& plc) = 0;
    virtual gint SetPlc(const TMSFormatType fmttype, const gboolean plc) = 0;

    // From TMS routing
    virtual gint SetOutput(TMSAudioOutput output) = 0;
    virtual gint GetOutput(TMSAudioOutput& output) = 0;
    virtual gint GetPreviousOutput(TMSAudioOutput& output) = 0;
    virtual gint GetAvailableOutputsL(gint& count,
            CBufFlat*& outputsbuffer) = 0;

protected:
    TMSCallAdpt();
    virtual gint PostConstruct();

protected:
    gint iGlobalVol;
    gint iGlobalGain;
    };

} //namespace TMS

#endif // CALLADPT_H

// End of file
