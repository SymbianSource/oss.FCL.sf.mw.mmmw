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

#include <AudioPreference.h>
#include "tmsutility.h"
#include "tmsclientserver.h"
#include "tmsshared.h"
#include "tmsipcallstream.h"
#include "tmscallipadpt.h"

using namespace TMS;

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::NewL
// Symbian constructor.
// -----------------------------------------------------------------------------
//
TMSCallIPAdpt* TMSCallIPAdpt::NewL()
    {
    TMSCallIPAdpt* self = new (ELeave) TMSCallIPAdpt();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::ConstructL
// 2-nd phase constructor.
// -----------------------------------------------------------------------------
//
void TMSCallIPAdpt::ConstructL()
    {
    TRACE_PRN_FN_ENT;
    iIPUplink = NULL;
    iIPDownlink = NULL;
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::TMSCallIPAdpt
//
// -----------------------------------------------------------------------------
//
TMSCallIPAdpt::TMSCallIPAdpt()
    {
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::~TMSCallIPAdpt
//
// -----------------------------------------------------------------------------
//
TMSCallIPAdpt::~TMSCallIPAdpt()
    {
    TRACE_PRN_FN_ENT;

    iCodecs.Reset();
    iCodecs.Close();
    iArrBitrates.Reset();

    delete iIPUplink;
    delete iIPDownlink;

    if (iMsgQueueUp.Handle() > 0)
        {
        iMsgQueueUp.Close();
        }
    if (iMsgQueueDn.Handle() > 0)
        {
        iMsgQueueDn.Close();
        }

    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::PostConstruct
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::PostConstruct()
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);
    iNextStreamId = 1;
    iUplinkInitialized = FALSE;
    iDnlinkInitialized = FALSE;

    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::CreateStream
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::CreateStream(TMSCallType /*callType*/,
        TMSStreamType strmType, gint& outStrmId)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);
    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            {
            status = TMS_RESULT_ALREADY_EXIST;
            if (!iUplinkInitialized)
                {
                iUplinkInitialized = TRUE;
                iUplinkStreamId = iNextStreamId;
                outStrmId = iUplinkStreamId;
                iNextStreamId++;
                status = TMS_RESULT_SUCCESS;
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            {
            status = TMS_RESULT_ALREADY_EXIST;
            if (!iDnlinkInitialized)
                {
                iDnlinkInitialized = TRUE;
                iDnlinkStreamId = iNextStreamId;
                outStrmId = iDnlinkStreamId;
                iNextStreamId++;
                status = TMS_RESULT_SUCCESS;
                }
            break;
            }
        default:
            {
            status = TMS_RESULT_STREAM_TYPE_NOT_SUPPORTED;
            break;
            }
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::InitStream
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::InitStream(TMSCallType /*callType*/, TMSStreamType strmType,
        gint strmId, TMSFormatType frmtType, const RMessage2& message)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);

    guint32 fourCC = TOFOURCC(frmtType);
    if (fourCC == NULL)
        {
        return TMS_RESULT_INVALID_ARGUMENT;
        }

    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            {
            status = TMS_RESULT_DOES_NOT_EXIST;
            if (strmId == iUplinkStreamId)
                {
                SetFormat(iUplinkStreamId, fourCC);
                status = OpenUplinkL(message);
                NotifyClient(iUplinkStreamId, ECmdUplinkInitComplete, status);
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            {
            status = TMS_RESULT_DOES_NOT_EXIST;
            if (strmId == iDnlinkStreamId)
                {
                SetFormat(iDnlinkStreamId, fourCC);
                status = OpenDownlinkL(message);
                NotifyClient(iDnlinkStreamId, ECmdDownlinkInitComplete, status);
                }
            break;
            }
        default:
            {
            status = TMS_RESULT_STREAM_TYPE_NOT_SUPPORTED;
            break;
            }
        }

    TRACE_PRN_IF_ERR(status);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::StartStream
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::StartStream(TMSCallType /*callType*/,
        TMSStreamType strmType, gint strmId)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            {
            if (strmId == iUplinkStreamId && iIPUplink)
                {
                iIPUplink->Start();
                status = TMS_RESULT_SUCCESS;
                NotifyClient(iUplinkStreamId, ECmdUplinkStarted, status);
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            {
            if (strmId == iDnlinkStreamId && iIPDownlink)
                {
                iIPDownlink->Start();
                status = TMS_RESULT_SUCCESS;
                NotifyClient(iDnlinkStreamId, ECmdDownlinkStarted, status);
                }
            break;
            }
        default:
            {
            status = TMS_RESULT_STREAM_TYPE_NOT_SUPPORTED;
            break;
            }
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::PauseStream
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::PauseStream(TMSCallType /*callType*/,
        TMSStreamType strmType, gint strmId)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            {
            if (strmId == iUplinkStreamId && iIPUplink)
                {
                iIPUplink->Stop();
                status = TMS_RESULT_SUCCESS;
                NotifyClient(iUplinkStreamId, ECmdUplinkPaused, status);
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            {
            if (strmId == iDnlinkStreamId && iIPDownlink)
                {
                iIPDownlink->Stop();
                status = TMS_RESULT_SUCCESS;
                NotifyClient(iDnlinkStreamId, ECmdDownlinkPaused, status);
                }
            break;
            }
        default:
            {
            status = TMS_RESULT_STREAM_TYPE_NOT_SUPPORTED;
            break;
            }
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::StopStream
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::StopStream(TMSCallType /*callType*/, TMSStreamType strmType,
        gint strmId)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);

    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            {
            if (strmId == iUplinkStreamId && iIPUplink)
                {
                iIPUplink->Stop();
                status = TMS_RESULT_SUCCESS;
                NotifyClient(iUplinkStreamId, ECmdUplinkInitComplete, status);
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            {
            if (strmId == iDnlinkStreamId && iIPDownlink)
                {
                iIPDownlink->Stop();
                status = TMS_RESULT_SUCCESS;
                NotifyClient(iDnlinkStreamId, ECmdDownlinkInitComplete, status);
                }
            break;
            }
        default:
            {
            status = TMS_RESULT_STREAM_TYPE_NOT_SUPPORTED;
            break;
            }
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::DeinitStream
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::DeinitStream(TMSCallType /*callType*/,
        TMSStreamType strmType, gint strmId)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);

    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            {
            if (strmId == iUplinkStreamId && iIPUplink)
                {
                iIPUplink->Stop();
                //iUplinkStreamId = -1;
                iUplinkInitialized = FALSE;
                status = TMS_RESULT_SUCCESS;
                NotifyClient(iUplinkStreamId, ECmdDownlinkDeInitComplete,
                        status);
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            {
            if (strmId == iDnlinkStreamId && iIPDownlink)
                {
                iIPDownlink->Stop();
                //iDnlinkStreamId = -1;
                iDnlinkInitialized = FALSE;
                status = TMS_RESULT_SUCCESS;
                NotifyClient(iDnlinkStreamId, ECmdDownlinkDeInitComplete,
                        status);
                }
            break;
            }
        default:
            {
            status = TMS_RESULT_STREAM_TYPE_NOT_SUPPORTED;
            break;
            }
        }

    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::DeleteStream
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::DeleteStream(TMSCallType /*callType*/,
        TMSStreamType strmType, gint strmId)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);
    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            {
            // This is additional error checking
            if (strmId == iUplinkStreamId)
                {
                iUplinkStreamId = -1;
                iUplinkInitialized = FALSE;
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            {
            // This is additional error checking
            if (strmId == iDnlinkStreamId)
                {
                iDnlinkStreamId = -1;
                iDnlinkInitialized = FALSE;
                }
            break;
            }
        default:
            {
            status = TMS_RESULT_STREAM_TYPE_NOT_SUPPORTED;
            break;
            }
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::DataXferBufferEmptied
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::DataXferBufferEmptied(TMSCallType /*callType*/,
        TMSStreamType strmType, gint strmId)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);
    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            {
            if (strmId == iUplinkStreamId)
                {
                TRAP(status, BufferEmptiedL());
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            status = TMS_RESULT_ILLEGAL_OPERATION;
            break;
        default:
            status = TMS_RESULT_STREAM_TYPE_NOT_SUPPORTED;
            break;
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::DataXferBufferFilled
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::DataXferBufferFilled(TMSCallType /*callType*/,
        TMSStreamType strmType, gint strmId, guint datasize)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);
    switch (strmType)
        {
        case TMS_STREAM_DOWNLINK:
            if (strmId == iDnlinkStreamId)
                {
                TRAP(status, BufferFilledL(datasize));
                }
            break;
        case TMS_STREAM_UPLINK:
            status = TMS_RESULT_ILLEGAL_OPERATION;
            break;
        default:
            status = TMS_RESULT_STREAM_TYPE_NOT_SUPPORTED;
            break;
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetDataXferBufferHndl
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetDataXferBufferHndl(const TMSCallType /*callType*/,
        const TMSStreamType strmType, const gint strmId, const guint32 key,
        RChunk& chunk)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);
    switch (strmType)
        {
        case TMS_STREAM_DOWNLINK:
            {
            if (strmId == iDnlinkStreamId)
                {
                TRAP(status, GetDataXferChunkHndl(strmType, key, chunk));
                }
            break;
            }
        case TMS_STREAM_UPLINK:
            {
            if (strmId == iUplinkStreamId)
                {
                TRAP(status, GetDataXferChunkHndl(strmType, key, chunk));
                }
            break;
            }
        default:
            status = TMS_RESULT_STREAM_TYPE_NOT_SUPPORTED;
            break;
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetMaxVolume
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetMaxVolume(guint& volume)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);

    if (iDnlinkInitialized && iIPDownlink)
        {
        status = iIPDownlink->GetMaxVolume(volume);
        iMaxVolume = volume;
        TRACE_PRN_N1(_L("TMS->TMSCallIPAdpt: GetMaxVolume [%d]"), iMaxVolume);
        }

    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::SetVolume
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::SetVolume(const guint volume)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iDnlinkInitialized && iIPDownlink)
        {
        status = iIPDownlink->SetVolume(volume);
        NotifyClient(iDnlinkStreamId, ECmdSetVolume, status);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetVolume
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetVolume(guint& volume)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iDnlinkInitialized && iIPDownlink)
        {
        status = iIPDownlink->GetVolume(volume);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetMaxGain
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetMaxGain(guint& gain)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iUplinkInitialized && iIPUplink)
        {
        status = iIPUplink->GetMaxGain(gain);
        iMaxGain = gain;
        TRACE_PRN_N1(_L("TMS->TMSCallIPAdpt::GetMaxGain [%d]"), iMaxGain);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::SetGain
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::SetGain(const guint gain)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iUplinkInitialized && iIPUplink)
        {
        status = iIPUplink->SetGain(gain);
        NotifyClient(iUplinkStreamId, ECmdSetGain, status);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetGain
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetGain(guint& gain)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iUplinkInitialized && iIPUplink)
        {
        status = iIPUplink->GetGain(gain);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetGlobalMaxVolume
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetGlobalMaxVolume(guint& volume)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iDnlinkInitialized && iIPDownlink)
        {
        status = iIPDownlink->GetMaxVolume(volume);
        iMaxVolume = volume;
        TRACE_PRN_N1(_L("TMS->TMSCallIPAdpt: GetMaxVolume [%d]"), iMaxVolume);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::SetGlobalVolume
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::SetGlobalVolume(const guint volume)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iDnlinkInitialized && iIPDownlink)
        {
        status = iIPDownlink->SetVolume(volume);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetGlobalVolume
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetGlobalVolume(guint& volume)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iDnlinkInitialized && iIPDownlink)
        {
        status = iIPDownlink->GetVolume(volume);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetGlobalMaxGain
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetGlobalMaxGain(guint& gain)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iUplinkInitialized && iIPUplink)
        {
        status = iIPUplink->GetMaxGain(gain);
        iMaxGain = gain;
        TRACE_PRN_N1(_L("TMS->TMSCallIPAdpt::GetMaxGain [%d]"), iMaxGain);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::SetGlobalGain
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::SetGlobalGain(const guint gain)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iUplinkInitialized && iIPUplink)
        {
        status = iIPUplink->SetGain(gain);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetGlobalGain
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetGlobalGain(guint& gain)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iUplinkInitialized && iIPUplink)
        {
        status = iIPUplink->GetGain(gain);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetCodecMode
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetCodecMode(const TMSFormatType fmttype,
        const TMSStreamType strmtype, gint& mode)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);
    switch (fmttype)
        {
        case TMS_FORMAT_ILBC:
            status = GetIlbcCodecMode(mode, strmtype);
            break;
        case TMS_FORMAT_G711:
            status = GetG711CodecMode(mode, strmtype);
            break;
        default:
            status = TMS_RESULT_FORMAT_TYPE_NOT_SUPPORTED;
            break;
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::SetCodecMode
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::SetCodecMode(const TMSFormatType fmttype,
        const TMSStreamType strmtype, const gint mode)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);
    switch (fmttype)
        {
        case TMS_FORMAT_ILBC:
            status = SetIlbcCodecMode(mode, strmtype);
            break;
        case TMS_FORMAT_G711:
            status = SetG711CodecMode(mode, strmtype);
            break;
        default:
            status = TMS_RESULT_FORMAT_TYPE_NOT_SUPPORTED;
            break;
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetSupportedBitRatesCount
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetSupportedBitRatesCount(guint& count)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iIPUplink)
        {
        status = iIPUplink->GetSupportedBitrates(iArrBitrates);
        count = iArrBitrates.Count();
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetSupportedBitRates
//
// Bitrates are already returned from the codec as a result of call to
// GetSupportedBitratesCount(). Just pack them into a descriptor and return
// back to the client.
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetSupportedBitRates(CBufFlat*& brbuffer)
    {
    TRAPD(status, GetSupportedBitRatesL(brbuffer));
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetSupportedBitRatesL
//
// GetSupportedBitRates implementation which can leave.
// -----------------------------------------------------------------------------
//
void TMSCallIPAdpt::GetSupportedBitRatesL(CBufFlat*& brbuffer)
    {
    TRACE_PRN_FN_ENT;
    RBufWriteStream stream;
    stream.Open(*brbuffer);
    CleanupClosePushL(stream);
    guint numOfItems = iArrBitrates.Count();

    for (guint i = 0; i < numOfItems; i++)
        {
        stream.WriteUint32L(iArrBitrates[i]);
        //TRACE_PRN_N1(_L("TMS->TMSCallIPAdpt: BR: [%d]"), iArrBitrates[i]);
        }

    CleanupStack::PopAndDestroy(&stream);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetBitRate
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetBitRate(guint& bitrate)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iIPUplink)
        {
        status = iIPUplink->GetBitrate(bitrate);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::SetBitRate
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::SetBitRate(const guint bitrate)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iIPUplink)
        {
        status = iIPUplink->SetBitrate(bitrate);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetVAD
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetVAD(const TMSFormatType fmttype, gboolean& vad)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iIPUplink)
        {
        status = iIPUplink->GetVad(fmttype, vad);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::SetVAD
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::SetVAD(const TMSFormatType fmttype, const gboolean vad)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iIPUplink)
        {
        status = iIPUplink->SetVad(fmttype, vad);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetCNG
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetCNG(const TMSFormatType fmttype, gboolean& cng)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iIPDownlink)
        {
        status = iIPDownlink->GetCng(fmttype, cng);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::SetCNG
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::SetCNG(const TMSFormatType fmttype, const gboolean cng)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iIPDownlink)
        {
        status = iIPDownlink->SetCng(fmttype, cng);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetPlc
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetPlc(const TMSFormatType fmttype, gboolean& plc)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iIPDownlink)
        {
        status = iIPDownlink->GetPlc(fmttype, plc);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::SetPlc
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::SetPlc(const TMSFormatType fmttype, const gboolean plc)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iIPDownlink)
        {
        status = iIPDownlink->SetPlc(fmttype, plc);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::OpenDownlinkL
// Method for player initialization.
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::OpenDownlinkL(const RMessage2& message)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);

    // Clients must have MultimediaDD capability to use this priority/pref.
    // TODO: Also, TMS will monitor for emergency call and if detected it
    //       will deny access to audio resources.
    iPriority.iPref = KAudioPrefVoipAudioDownlink;
    iPriority.iPriority = KAudioPriorityVoipAudioDownlink;

    if (!iIPDownlink)
        {
        iIPDownlink = TMSIPDownlink::NewL(iDnFourCC, iPriority);
        }

    if (iIPDownlink)
        {
        // Open message queue for handling server notifications to the client
        if (iMsgQueueDn.Handle() <= 0)
            {
            // Second argument in TMSCallProxy::InitStream
            status = iMsgQueueDn.Open(message, 1);
            }

        if (status == TMS_RESULT_SUCCESS)
            {
            // For transfer data buffer processing
            iIPDownlink->SetMsgQueue(iMsgQueueDn);
            }
        }

    TRACE_PRN_IF_ERR(status);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::OpenUplinkL
// Method for recorder initialization.
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::OpenUplinkL(const RMessage2& message)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);

    // Ensure clients have MultimediaDD capability to use this priority/pref
    iPriority.iPref = KAudioPrefVoipAudioUplink;
    iPriority.iPriority = KAudioPriorityVoipAudioUplink;

    if (!iIPUplink)
        {
        iIPUplink = TMSIPUplink::NewL(iUpFourCC, iPriority);
        }

    if (iIPUplink)
        {
        // Open message queue for handling server notifications to the client
        if (iMsgQueueUp.Handle() <= 0)
            {
            // Second argument in TMSCallProxy::InitStream
            status = iMsgQueueUp.Open(message, 1);
            }

        if (status == TMS_RESULT_SUCCESS)
            {
            // For transfer data buffer processing
            iIPUplink->SetMsgQueue(iMsgQueueUp);
            }
        }

    TRACE_PRN_IF_ERR(status);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::SetFormat
//
// -----------------------------------------------------------------------------
//
void TMSCallIPAdpt::SetFormat(const gint strmId, const guint32 aFormat)
    {
    if (strmId == iUplinkStreamId)
        {
        iUpFourCC = aFormat;
        }
    else if (strmId == iDnlinkStreamId)
        {
        iDnFourCC = aFormat;
        }
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::BufferFilled
//
// -----------------------------------------------------------------------------
//
void TMSCallIPAdpt::BufferFilledL(guint dataSize)
    {
    if (iIPDownlink)
        {
        iIPDownlink->BufferFilled(dataSize);
        }
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::BufferEmptied
//
// -----------------------------------------------------------------------------
//
void TMSCallIPAdpt::BufferEmptiedL()
    {
    if (iIPUplink)
        {
        iIPUplink->BufferEmptied();
        }
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetDataXferChunkHndl
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetDataXferChunkHndl(const TMSStreamType strmType,
        const guint32 key, RChunk& chunk)
    {
    TRACE_PRN_FN_ENT;

    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    switch (strmType)
        {
        case TMS_STREAM_DOWNLINK:
            {
            if (iIPDownlink)
                {
                status = iIPDownlink->GetDataXferChunkHndl(key, chunk);
                }
            break;
            }
        case TMS_STREAM_UPLINK:
            {
            if (iIPUplink)
                {
                status = iIPUplink->GetDataXferChunkHndl(key, chunk);
                }
            break;
            }
        default:
            status = TMS_RESULT_STREAM_TYPE_NOT_SUPPORTED;
            break;
        }

    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::SetIlbcCodecMode
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::SetIlbcCodecMode(const gint mode,
        const TMSStreamType strmtype)
    {
    gint status(TMS_RESULT_INVALID_ARGUMENT);

    if (strmtype == TMS_STREAM_DOWNLINK)
        {
        if (iDnlinkInitialized && iIPDownlink)
            {
            status = iIPDownlink->SetIlbcCodecMode(mode);
            }
        }
    else if (strmtype == TMS_STREAM_UPLINK)
        {
        if (iUplinkInitialized && iIPUplink)
            {
            status = iIPUplink->SetIlbcCodecMode(mode);
            }
        }

    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetIlbcCodecMode
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetIlbcCodecMode(gint& mode, const TMSStreamType strmtype)
    {
    gint status(TMS_RESULT_INVALID_ARGUMENT);

    if (strmtype == TMS_STREAM_DOWNLINK)
        {
        if (iDnlinkInitialized && iIPDownlink)
            {
            status = iIPDownlink->GetIlbcCodecMode(mode);
            }
        }
    else if (strmtype == TMS_STREAM_UPLINK)
        {
        if (iUplinkInitialized && iIPUplink)
            {
            status = iIPUplink->GetIlbcCodecMode(mode);
            }
        }

    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::SetG711CodecMode
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::SetG711CodecMode(const gint mode,
        const TMSStreamType strmtype)
    {
    gint status(TMS_RESULT_INVALID_ARGUMENT);

    if (strmtype == TMS_STREAM_DOWNLINK)
        {
        if (iDnlinkInitialized && iIPDownlink)
            {
            status = iIPDownlink->SetG711CodecMode(mode);
            }
        }
    else if (strmtype == TMS_STREAM_UPLINK)
        {
        if (iUplinkInitialized && iIPUplink)
            {
            status = iIPUplink->SetG711CodecMode(mode);
            }
        }

    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetG711CodecMode
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetG711CodecMode(gint& mode, const TMSStreamType strmtype)
    {
    gint status(TMS_RESULT_INVALID_ARGUMENT);

    if (strmtype == TMS_STREAM_DOWNLINK)
        {
        if (iDnlinkInitialized && iIPDownlink)
            {
            status = iIPDownlink->GetG711CodecMode(mode);
            }
        }
    else if (strmtype == TMS_STREAM_UPLINK)
        {
        if (iUplinkInitialized && iIPUplink)
            {
            status = iIPUplink->GetG711CodecMode(mode);
            }
        }

    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::FrameModeRequiredForEC
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::FrameModeRqrdForEC(gboolean& frmodereq)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (iIPDownlink)
        {
        status = iIPDownlink->FrameModeRqrdForEC(frmodereq);
        }
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::SetFrameMode
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::SetFrameMode(const gboolean frmode)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (iIPDownlink)
        {
        status = iIPDownlink->SetFrameMode(frmode);
        }
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetFrameMode
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetFrameMode(gboolean& frmode)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (iIPDownlink)
        {
        status = iIPDownlink->GetFrameMode(frmode);
        }
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::ConcealErrorForNextBuffer
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::ConcealErrorForNextBuffer()
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (iIPDownlink)
        {
        status = iIPDownlink->ConcealErrorForNextBuffer();
        }
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::BadLsfNextBuffer
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::BadLsfNextBuffer()
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (iIPDownlink)
        {
        status = iIPDownlink->BadLsfNextBuffer();
        }
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::SetOutput
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::SetOutput(TMSAudioOutput output)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (iDnlinkInitialized && iIPDownlink)
        {
        TRAP(status, iIPDownlink->SetAudioDeviceL(output));
        }
    return status;
    }
// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetOutput
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetOutput(TMSAudioOutput& output)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (iDnlinkInitialized && iIPDownlink)
        {
        TRAP(status, iIPDownlink->GetAudioDeviceL(output));
        }
    return status;
    }
// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetPreviousOutput
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetPreviousOutput(TMSAudioOutput& /*output*/)
    {
    return TMS_RESULT_FEATURE_NOT_SUPPORTED;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetAvailableOutputsL
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetAvailableOutputsL(gint& /*count*/,
        CBufFlat*& /*outputsbuffer*/)
    {
    return TMS_RESULT_FEATURE_NOT_SUPPORTED;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::NotifyClient
// -----------------------------------------------------------------------------
//
void TMSCallIPAdpt::NotifyClient(const gint strmId, const gint aCommand,
        const gint aStatus, const gint64 /*aInt64*/)
    {
    iMsgBuffer.iRequest = aCommand;
    iMsgBuffer.iStatus = aStatus;

    if (strmId == iUplinkStreamId)
        {
        iMsgQueueUp.Send(iMsgBuffer);
        }
    else if (strmId == iDnlinkStreamId)
        {
        iMsgQueueDn.Send(iMsgBuffer);
        }
    }

// End of file
