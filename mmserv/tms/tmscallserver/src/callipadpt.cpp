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
#include "ipcallstream.h"
#include "callipadpt.h"

using namespace TMS;

// -----------------------------------------------------------------------------
// CallIPAdpt::NewL
// Symbian constructor.
// -----------------------------------------------------------------------------
//
CallIPAdpt* CallIPAdpt::NewL()
    {
    CallIPAdpt* self = new (ELeave) CallIPAdpt();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::ConstructL
// 2-nd phase constructor.
// -----------------------------------------------------------------------------
//
void CallIPAdpt::ConstructL()
    {
    TRACE_PRN_FN_ENT;
    iVoIPUplink = NULL;
    iVoIPDownlink = NULL;
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::CallIPAdpt
//
// -----------------------------------------------------------------------------
//
CallIPAdpt::CallIPAdpt()
    {
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::~CallIPAdpt
//
// -----------------------------------------------------------------------------
//
CallIPAdpt::~CallIPAdpt()
    {
    TRACE_PRN_FN_ENT;

    iCodecs.Reset();
    iCodecs.Close();
    iArrBitrates.Reset();

    delete iVoIPUplink;
    delete iVoIPDownlink;

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
// CallIPAdpt::PostConstruct
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::PostConstruct()
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
// CallIPAdpt::CreateStream
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::CreateStream(TMSCallType /*callType*/,
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
// CallIPAdpt::InitStream
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::InitStreamL(TMSCallType /*callType*/,
        TMSStreamType strmType, gint strmId, TMSFormatType frmtType,
        const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);

    TUint32 fourCC = TOFOURCC(frmtType);
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
                status = OpenUplinkL(aMessage);
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
                status = OpenDownlinkL(aMessage);
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
// CallIPAdpt::StartStream
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::StartStream(TMSCallType /*callType*/, TMSStreamType strmType,
        gint strmId)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            {
            if (strmId == iUplinkStreamId && iVoIPUplink)
                {
                iVoIPUplink->Start();
                status = TMS_RESULT_SUCCESS;
                NotifyClient(iUplinkStreamId, ECmdUplinkStarted, status);
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            {
            if (strmId == iDnlinkStreamId && iVoIPDownlink)
                {
                iVoIPDownlink->Start();
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
// CallIPAdpt::PauseStream
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::PauseStream(TMSCallType /*callType*/, TMSStreamType strmType,
        gint strmId)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            {
            if (strmId == iUplinkStreamId && iVoIPUplink)
                {
                iVoIPUplink->Stop();
                status = TMS_RESULT_SUCCESS;
                NotifyClient(iUplinkStreamId, ECmdUplinkPaused, status);
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            {
            if (strmId == iDnlinkStreamId && iVoIPDownlink)
                {
                iVoIPDownlink->Stop();
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
// CallIPAdpt::StopStream
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::StopStream(TMSCallType /*callType*/, TMSStreamType strmType,
        gint strmId)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);

    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            {
            if (strmId == iUplinkStreamId && iVoIPUplink)
                {
                iVoIPUplink->Stop();
                status = TMS_RESULT_SUCCESS;
                NotifyClient(iUplinkStreamId, ECmdUplinkInitComplete, status);
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            {
            if (strmId == iDnlinkStreamId && iVoIPDownlink)
                {
                iVoIPDownlink->Stop();
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
// CallIPAdpt::DeinitStream
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::DeinitStream(TMSCallType /*callType*/, TMSStreamType strmType,
        gint strmId)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);

    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            {
            if (strmId == iUplinkStreamId && iVoIPUplink)
                {
                iVoIPUplink->Stop();
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
            if (strmId == iDnlinkStreamId && iVoIPDownlink)
                {
                iVoIPDownlink->Stop();
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
// CallIPAdpt::DeleteStream
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::DeleteStream(TMSCallType /*callType*/,
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
// CallIPAdpt::DataXferBufferEmptied
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::DataXferBufferEmptied(TMSCallType /*callType*/,
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
// CallIPAdpt::DataXferBufferFilled
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::DataXferBufferFilled(TMSCallType /*callType*/,
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
// CallIPAdpt::GetDataXferBufferHndl
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetDataXferBufferHndl(const TMSCallType /*callType*/,
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
// CallIPAdpt::GetMaxVolume
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetMaxVolume(guint& volume)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);

    if (iDnlinkInitialized && iVoIPDownlink)
        {
        status = iVoIPDownlink->GetMaxVolume(volume);
        iMaxVolume = volume;
        TRACE_PRN_N1(_L("TMS->CallIPAdpt: GetMaxVolume [%d]"), iMaxVolume);
        }

    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::SetVolume
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::SetVolume(const guint volume)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iDnlinkInitialized && iVoIPDownlink)
        {
        status = iVoIPDownlink->SetVolume(volume);
        NotifyClient(iDnlinkStreamId, ECmdSetVolume, status);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::GetVolume
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetVolume(guint& volume)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iDnlinkInitialized && iVoIPDownlink)
        {
        status = iVoIPDownlink->GetVolume(volume);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::GetMaxGain
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetMaxGain(guint& gain)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iUplinkInitialized && iVoIPUplink)
        {
        status = iVoIPUplink->GetMaxGain(gain);
        iMaxGain = gain;
        TRACE_PRN_N1(_L("TMS->CallIPAdpt::GetMaxGain [%d]"), iMaxGain);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::SetGain
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::SetGain(const guint gain)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iUplinkInitialized && iVoIPUplink)
        {
        status = iVoIPUplink->SetGain(gain);
        NotifyClient(iUplinkStreamId, ECmdSetGain, status);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::GetGain
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetGain(guint& gain)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iUplinkInitialized && iVoIPUplink)
        {
        status = iVoIPUplink->GetGain(gain);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::GetGlobalMaxVolume
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetGlobalMaxVolume(guint& volume)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iDnlinkInitialized && iVoIPDownlink)
        {
        status = iVoIPDownlink->GetMaxVolume(volume);
        iMaxVolume = volume;
        TRACE_PRN_N1(_L("TMS->CallIPAdpt: GetMaxVolume [%d]"), iMaxVolume);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::SetGlobalVolume
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::SetGlobalVolume(const guint volume)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iDnlinkInitialized && iVoIPDownlink)
        {
        status = iVoIPDownlink->SetVolume(volume);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::GetGlobalVolume
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetGlobalVolume(guint& volume)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iDnlinkInitialized && iVoIPDownlink)
        {
        status = iVoIPDownlink->GetVolume(volume);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::GetGlobalMaxGain
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetGlobalMaxGain(guint& gain)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iUplinkInitialized && iVoIPUplink)
        {
        status = iVoIPUplink->GetMaxGain(gain);
        iMaxGain = gain;
        TRACE_PRN_N1(_L("TMS->CallIPAdpt::GetMaxGain [%d]"), iMaxGain);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::SetGlobalGain
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::SetGlobalGain(const guint gain)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iUplinkInitialized && iVoIPUplink)
        {
        status = iVoIPUplink->SetGain(gain);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::GetGlobalGain
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetGlobalGain(guint& gain)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iUplinkInitialized && iVoIPUplink)
        {
        status = iVoIPUplink->GetGain(gain);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::GetCodecMode
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetCodecMode(const TMSFormatType fmttype,
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
// CallIPAdpt::SetCodecMode
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::SetCodecMode(const TMSFormatType fmttype,
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
// CallIPAdpt::GetSupportedBitRatesCount
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetSupportedBitRatesCount(guint& count)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iVoIPUplink)
        {
        status = iVoIPUplink->GetSupportedBitrates(iArrBitrates);
        count = iArrBitrates.Count();
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::GetSupportedBitRates
//
// Bitrates are already returned from the codec as a result of call to
// GetSupportedBitratesCount(). Just pack them into a descriptor and return
// back to the client.
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetSupportedBitRates(CBufFlat*& brbuffer)
    {
    TRAPD(status, GetSupportedBitRatesL(brbuffer));
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::GetSupportedBitRatesL
//
// GetSupportedBitRates implementation which can leave.
// -----------------------------------------------------------------------------
//
void CallIPAdpt::GetSupportedBitRatesL(CBufFlat*& brbuffer)
    {
    TRACE_PRN_FN_ENT;
    RBufWriteStream stream;
    stream.Open(*brbuffer);
    CleanupClosePushL(stream);
    guint numOfItems = iArrBitrates.Count();

    for (guint i = 0; i < numOfItems; i++)
        {
        stream.WriteUint32L(iArrBitrates[i]);
        //TRACE_PRN_N1(_L("TMS->CallIPAdpt: BR: [%d]"), iArrBitrates[i]);
        }

    CleanupStack::PopAndDestroy(&stream);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::GetBitRate
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetBitRate(guint& bitrate)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iVoIPUplink)
        {
        status = iVoIPUplink->GetBitrate(bitrate);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::SetBitRate
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::SetBitRate(const guint bitrate)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iVoIPUplink)
        {
        status = iVoIPUplink->SetBitrate(bitrate);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::GetVAD
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetVAD(const TMSFormatType fmttype, gboolean& vad)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iVoIPUplink)
        {
        status = iVoIPUplink->GetVad(fmttype, vad);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::SetVAD
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::SetVAD(const TMSFormatType fmttype, const gboolean vad)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iVoIPUplink)
        {
        status = iVoIPUplink->SetVad(fmttype, vad);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::GetCNG
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetCNG(const TMSFormatType fmttype, gboolean& cng)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iVoIPDownlink)
        {
        status = iVoIPDownlink->GetCng(fmttype, cng);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::SetCNG
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::SetCNG(const TMSFormatType fmttype, const gboolean cng)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iVoIPDownlink)
        {
        status = iVoIPDownlink->SetCng(fmttype, cng);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::GetPlc
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetPlc(const TMSFormatType fmttype, gboolean& plc)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iVoIPDownlink)
        {
        status = iVoIPDownlink->GetPlc(fmttype, plc);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::SetPlc
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::SetPlc(const TMSFormatType fmttype, const gboolean plc)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iVoIPDownlink)
        {
        status = iVoIPDownlink->SetPlc(fmttype, plc);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::OpenDownlinkL
// Method for player initialization.
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::OpenDownlinkL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);

    // Ensure clients have MultimediaDD capability to use this priority/pref
    // TODO: TMS must monitor for emergency call now
    iPriority.iPref = TMdaPriorityPreference(KAudioPrefVoipAudioDownlink);
    iPriority.iPriority = TMdaPriority(KAudioPriorityVoipAudioDownlink);

    if (!iVoIPDownlink)
        {
        iVoIPDownlink = TMSVoIPDownlink::NewL(iDnFourCC, iPriority);
        }

    if (iVoIPDownlink)
        {
        // Open message queue for handling server notifications to the client
        if (iMsgQueueDn.Handle() <= 0)
            {
            // Second argument in TMSCallProxy::InitStream
            status = iMsgQueueDn.Open(aMessage, 1);
            }

        if (status == TMS_RESULT_SUCCESS)
            {
            // For transfer data buffer processing
            iVoIPDownlink->SetMsgQueue(iMsgQueueDn);
            }
        }

    TRACE_PRN_IF_ERR(status);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::OpenUplinkL
// Method for recorder initialization.
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::OpenUplinkL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);

    // Ensure clients have MultimediaDD capability to use this priority/pref
    iPriority.iPref = TMdaPriorityPreference(KAudioPrefVoipAudioUplink);
    iPriority.iPriority = TMdaPriority(KAudioPriorityVoipAudioUplink);

    if (!iVoIPUplink)
        {
        iVoIPUplink = TMSVoIPUplink::NewL(iUpFourCC, iPriority);
        }

    if (iVoIPUplink)
        {
        // Open message queue for handling server notifications to the client
        if (iMsgQueueUp.Handle() <= 0)
            {
            // Second argument in TMSCallProxy::InitStream
            status = iMsgQueueUp.Open(aMessage, 1);
            }

        if (status == TMS_RESULT_SUCCESS)
            {
            // For transfer data buffer processing
            iVoIPUplink->SetMsgQueue(iMsgQueueUp);
            }
        }

    TRACE_PRN_IF_ERR(status);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::SetFormat
//
// -----------------------------------------------------------------------------
//
void CallIPAdpt::SetFormat(const gint strmId, const TUint32 aFormat)
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
// CallIPAdpt::BufferFilled
//
// -----------------------------------------------------------------------------
//
void CallIPAdpt::BufferFilledL(TUint dataSize)
    {
    if (iVoIPDownlink)
        {
        iVoIPDownlink->BufferFilled(dataSize);
        }
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::BufferEmptied
//
// -----------------------------------------------------------------------------
//
void CallIPAdpt::BufferEmptiedL()
    {
    if (iVoIPUplink)
        {
        iVoIPUplink->BufferEmptied();
        }
    }

// ----------------------------------------------------------------------------
// CallIPAdpt::GetDataXferChunkHndl
//
// ----------------------------------------------------------------------------
//
gint CallIPAdpt::GetDataXferChunkHndl(const TMSStreamType strmType,
        const TUint32 key, RChunk& chunk)
    {
    TRACE_PRN_FN_ENT;

    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    switch (strmType)
        {
        case TMS_STREAM_DOWNLINK:
            {
            if (iVoIPDownlink)
                {
                status = iVoIPDownlink->GetDataXferChunkHndl(key, chunk);
                }
            break;
            }
        case TMS_STREAM_UPLINK:
            {
            if (iVoIPUplink)
                {
                status = iVoIPUplink->GetDataXferChunkHndl(key, chunk);
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
// CallIPAdpt::SetIlbcCodecMode
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::SetIlbcCodecMode(const gint mode, const TMSStreamType strmtype)
    {
    gint status(TMS_RESULT_INVALID_ARGUMENT);

    if (strmtype == TMS_STREAM_DOWNLINK)
        {
        if (iDnlinkInitialized && iVoIPDownlink)
            {
            status = iVoIPDownlink->SetIlbcCodecMode(mode);
            }
        }
    else if (strmtype == TMS_STREAM_UPLINK)
        {
        if (iUplinkInitialized && iVoIPUplink)
            {
            status = iVoIPUplink->SetIlbcCodecMode(mode);
            }
        }

    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::GetIlbcCodecMode
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetIlbcCodecMode(gint& mode, const TMSStreamType strmtype)
    {
    gint status(TMS_RESULT_INVALID_ARGUMENT);

    if (strmtype == TMS_STREAM_DOWNLINK)
        {
        if (iDnlinkInitialized && iVoIPDownlink)
            {
            status = iVoIPDownlink->GetIlbcCodecMode(mode);
            }
        }
    else if (strmtype == TMS_STREAM_UPLINK)
        {
        if (iUplinkInitialized && iVoIPUplink)
            {
            status = iVoIPUplink->GetIlbcCodecMode(mode);
            }
        }

    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::SetG711CodecMode
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::SetG711CodecMode(const gint mode, const TMSStreamType strmtype)
    {
    gint status(TMS_RESULT_INVALID_ARGUMENT);

    if (strmtype == TMS_STREAM_DOWNLINK)
        {
        if (iDnlinkInitialized && iVoIPDownlink)
            {
            status = iVoIPDownlink->SetG711CodecMode(mode);
            }
        }
    else if (strmtype == TMS_STREAM_UPLINK)
        {
        if (iUplinkInitialized && iVoIPUplink)
            {
            status = iVoIPUplink->SetG711CodecMode(mode);
            }
        }

    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::GetG711CodecMode
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetG711CodecMode(gint& mode, const TMSStreamType strmtype)
    {
    gint status(TMS_RESULT_INVALID_ARGUMENT);

    if (strmtype == TMS_STREAM_DOWNLINK)
        {
        if (iDnlinkInitialized && iVoIPDownlink)
            {
            status = iVoIPDownlink->GetG711CodecMode(mode);
            }
        }
    else if (strmtype == TMS_STREAM_UPLINK)
        {
        if (iUplinkInitialized && iVoIPUplink)
            {
            status = iVoIPUplink->GetG711CodecMode(mode);
            }
        }

    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::FrameModeRequiredForEC
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::FrameModeRqrdForEC(gboolean& frmodereq)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (iVoIPDownlink)
        {
        status = iVoIPDownlink->FrameModeRqrdForEC(frmodereq);
        }
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::SetFrameMode
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::SetFrameMode(const gboolean frmode)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (iVoIPDownlink)
        {
        status = iVoIPDownlink->SetFrameMode(frmode);
        }
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::GetFrameMode
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetFrameMode(gboolean& frmode)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (iVoIPDownlink)
        {
        status = iVoIPDownlink->GetFrameMode(frmode);
        }
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::ConcealErrorForNextBuffer
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::ConcealErrorForNextBuffer()
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (iVoIPDownlink)
        {
        status = iVoIPDownlink->ConcealErrorForNextBuffer();
        }
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::BadLsfNextBuffer
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::BadLsfNextBuffer()
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (iVoIPDownlink)
        {
        status = iVoIPDownlink->BadLsfNextBuffer();
        }
    return status;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::SetOutput
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::SetOutput(TMSAudioOutput output)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (iDnlinkInitialized && iVoIPDownlink)
        {
        TRAP(status, iVoIPDownlink->SetAudioDeviceL(output));
        }
    return status;
    }
// -----------------------------------------------------------------------------
// CallIPAdpt::GetOutput
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetOutput(TMSAudioOutput& output)
    {
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (iDnlinkInitialized && iVoIPDownlink)
        {
        TRAP(status, iVoIPDownlink->GetAudioDeviceL(output));
        }
    return status;
    }
// -----------------------------------------------------------------------------
// CallIPAdpt::GetPreviousOutput
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetPreviousOutput(TMSAudioOutput& /*output*/)
    {
    return TMS_RESULT_FEATURE_NOT_SUPPORTED;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::GetAvailableOutputsL
//
// -----------------------------------------------------------------------------
//
gint CallIPAdpt::GetAvailableOutputsL(gint& /*count*/, CBufFlat*& /*outputsbuffer*/)
    {
    return TMS_RESULT_FEATURE_NOT_SUPPORTED;
    }

// -----------------------------------------------------------------------------
// CallIPAdpt::NotifyClient
// -----------------------------------------------------------------------------
//
void CallIPAdpt::NotifyClient(const gint strmId, const TInt aCommand,
        const TInt aStatus, const TInt64 /*aInt64*/)
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
