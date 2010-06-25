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
    iIPDownlink = NULL;
    iIPUplink = NULL;
    iDTMFDnlinkPlayer = NULL;
    iDTMFUplinkPlayer = NULL;
    iDTMFNotifier = NULL;
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
    delete iDTMFUplinkPlayer;
    delete iDTMFDnlinkPlayer;
    delete iDTMFNotifier;
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
gint TMSCallIPAdpt::CreateStream(const TMSCallType /*callType*/,
        const TMSStreamType strmType, gint& outStrmId)
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
                iUplinkStreamId = iNextStreamId;
                outStrmId = iUplinkStreamId;
                iNextStreamId++;
                //iUplinkInitialized = TRUE; //not initialized yet!
                status = TMS_RESULT_SUCCESS;
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            {
            status = TMS_RESULT_ALREADY_EXIST;
            if (!iDnlinkInitialized)
                {
                iDnlinkStreamId = iNextStreamId;
                outStrmId = iDnlinkStreamId;
                iNextStreamId++;
                //iDnlinkInitialized = TRUE; //not initialized yet!
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
gint TMSCallIPAdpt::InitStream(const TMSCallType /*callType*/,
        const TMSStreamType strmType, const gint strmId,
        const TMSFormatType frmtType, const gint retrytime,
        const RMessage2& message)
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
                status = OpenUplink(message, retrytime);
                if (status == TMS_RESULT_SUCCESS)
                    {
                    status = InitDTMF(TMS_STREAM_UPLINK);
                    }
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            {
            status = TMS_RESULT_DOES_NOT_EXIST;
            if (strmId == iDnlinkStreamId)
                {
                SetFormat(iDnlinkStreamId, fourCC);
                status = OpenDownlink(message, retrytime);
                if (status == TMS_RESULT_SUCCESS)
                    {
                    status = InitDTMF(TMS_STREAM_DOWNLINK);
                    }
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
gint TMSCallIPAdpt::StartStream(const TMSCallType /*callType*/,
        const TMSStreamType strmType, const gint strmId, const gint retrytime)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            {
            if (iIPUplink && strmId == iUplinkStreamId)
                {
                iIPUplink->Start(retrytime);
                status = TMS_RESULT_SUCCESS;
                NotifyClient(iUplinkStreamId, ECmdUplinkStarted, status);
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            {
            if (iIPDownlink && strmId == iDnlinkStreamId)
                {
                iIPDownlink->Start(retrytime);
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
gint TMSCallIPAdpt::PauseStream(const TMSCallType /*callType*/,
        const TMSStreamType strmType, const gint strmId)
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
gint TMSCallIPAdpt::StopStream(const TMSCallType /*callType*/,
        const TMSStreamType strmType, const gint strmId)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);

    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            {
            if (iIPUplink && strmId == iUplinkStreamId)
                {
                iIPUplink->Stop();
                status = TMS_RESULT_SUCCESS;
                NotifyClient(iUplinkStreamId, ECmdUplinkInitComplete, status);
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            {
            if (iIPDownlink && strmId == iDnlinkStreamId)
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
gint TMSCallIPAdpt::DeinitStream(const TMSCallType /*callType*/,
        const TMSStreamType strmType, const gint strmId)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);

    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            {
            if (iIPUplink && strmId == iUplinkStreamId)
                {
                iIPUplink->Stop();
                //iUplinkStreamId = -1;
                iUplinkInitialized = FALSE;
                status = TMS_RESULT_SUCCESS;
                NotifyClient(iUplinkStreamId, ECmdUplinkDeInitComplete, status);
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            {
            if (iIPDownlink && strmId == iDnlinkStreamId)
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
gint TMSCallIPAdpt::DeleteStream(const TMSCallType /*callType*/,
        const TMSStreamType strmType, const gint strmId)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);
    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            {
            if (strmId == iUplinkStreamId)
                {
                iUplinkStreamId = -1;
                iUplinkInitialized = FALSE;
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            {
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
gint TMSCallIPAdpt::DataXferBufferEmptied(const TMSCallType /*callType*/,
        const TMSStreamType strmType, const gint strmId)
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
gint TMSCallIPAdpt::DataXferBufferFilled(const TMSCallType /*callType*/,
        const TMSStreamType strmType, const gint strmId, const guint datasize)
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
        TRACE_PRN_N1(_L("TMS->TMSCallIPAdpt::GetGlobalMaxVolume [%d]"), volume);
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
    //iGlobalVol = volume;
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
        TRACE_PRN_N1(_L("TMS->TMSCallIPAdpt::GetGlobalMaxGain [%d]"), gain);
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
    //iGlobalGain = gain;
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
// TMSCallIPAdpt::OpenDownlink
// Method for player initialization.
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::OpenDownlink(const RMessage2& message, const gint retrytime)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);

    // Clients must have MultimediaDD capability to use this priority/pref.
    // TODO: Also, TMS will monitor for emergency call and if detected it
    //       will deny access to audio resources.

    /* Clarify with adaptation team which prio/pref values should be used.
     * 1) KAudioPrefUnknownVoipAudioDownlink      -3rd party VoIP?
     *    KAudioPriorityUnknownVoipAudioDownlink  -3rd party VoIP?
     * 2) KAudioPrefVoipAudioDownlink             -NOK native VoIP?
     *    KAudioPriorityVoipAudioDownlink         -NOK native VoIP?
     */
    iPriority.iPref = KAudioPrefVoipAudioDownlink;
    iPriority.iPriority = KAudioPriorityVoipAudioDownlink;

    delete iIPDownlink;
    iIPDownlink = NULL;
    TRAP(status, iIPDownlink = TMSIPDownlink::NewL(*this, iDnFourCC, iPriority,
            retrytime));

    if (iIPDownlink && status == TMS_RESULT_SUCCESS)
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
// TMSCallIPAdpt::OpenUplink
// Method for recorder initialization.
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::OpenUplink(const RMessage2& message, const gint retrytime)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);

    // Clients must have MultimediaDD capability to use this priority/pref

    /* Clarify with adaptation team which prio/pref values should be used.
     * 1) KAudioPrefUnknownVoipAudioUplink      -3rd party VoIP?
     *    KAudioPriorityUnknownVoipAudioUplink  -3rd party VoIP?
     * 2) KAudioPrefVoipAudioUplink             -NOK native VoIP?
     *    KAudioPriorityVoipAudioUplink         -NOK native VoIP?
     */
    iPriority.iPref = KAudioPrefVoipAudioUplink;
    iPriority.iPriority = KAudioPriorityVoipAudioUplink;

    delete iIPUplink;
    iIPUplink = NULL;
    TRAP(status, iIPUplink = TMSIPUplink::NewL(*this, iUpFourCC, iPriority,
            retrytime));

    if (iIPUplink && status == TMS_RESULT_SUCCESS)
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
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (iDnlinkInitialized && iIPDownlink)
        {
        TRAP(status, iIPDownlink->SetAudioDeviceL(output));
        }
    TRACE_PRN_FN_EXT;
    return status;
    }
// -----------------------------------------------------------------------------
// TMSCallIPAdpt::GetOutput
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::GetOutput(TMSAudioOutput& output)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (iDnlinkInitialized && iIPDownlink)
        {
        TRAP(status, iIPDownlink->GetAudioDeviceL(output));
        }
    TRACE_PRN_FN_EXT;
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
        CBufFlat*& /*outputsbuf*/)
    {
    return TMS_RESULT_FEATURE_NOT_SUPPORTED;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::InitDTMF
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::InitDTMF(TMSStreamType strmtype)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_STREAM_TYPE_NOT_SUPPORTED);

    if (strmtype == TMS_STREAM_DOWNLINK)
        {
        delete iDTMFDnlinkPlayer;
        iDTMFDnlinkPlayer = NULL;
        TRAP(status, iDTMFDnlinkPlayer = TMSAudioDtmfTonePlayer::NewL(*this,
                KAudioDTMFString, KAudioPriorityDTMFString));
         }
    else if (strmtype == TMS_STREAM_UPLINK)
        {
        delete iDTMFUplinkPlayer;
        iDTMFUplinkPlayer = NULL;
        TRAP(status, iDTMFUplinkPlayer = TMSAudioDtmfTonePlayer::NewL(*this,
                KAudioDTMFString, KAudioPriorityDTMFString));
        }

    if (!iDTMFNotifier && status == TMS_RESULT_SUCCESS)
         {
         TRAP(status, iDTMFNotifier = TMSDtmfNotifier::NewL());
         }

    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::StartDTMF
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::StartDTMF(TMSStreamType strmtype, TDes& dtmfstring)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_STREAM_TYPE_NOT_SUPPORTED);
    TmsMsgBufPckg dtmfpckg;
    dtmfpckg().iStatus = status;
    dtmfpckg().iRequest = ECmdDTMFTonePlayFinished;

    if (strmtype == TMS_STREAM_DOWNLINK)
        {
        status = TMS_RESULT_UNINITIALIZED_OBJECT;
        if (iDTMFDnlinkPlayer)
            {
            iDTMFDnlinkPlayer->PlayDtmfTone(dtmfstring);
            status = TMS_RESULT_SUCCESS;
            }
        dtmfpckg().iStatus = status;
        dtmfpckg().iRequest = ECmdDTMFToneDnlPlayStarted;
        }
    else if (strmtype == TMS_STREAM_UPLINK)
        {
        status = TMS_RESULT_UNINITIALIZED_OBJECT;
        if (iDTMFUplinkPlayer)
            {
            iDTMFUplinkPlayer->PlayDtmfTone(dtmfstring);
            status = TMS_RESULT_SUCCESS;
            }
        dtmfpckg().iStatus = status;
        dtmfpckg().iRequest = ECmdDTMFToneUplPlayStarted;
        }

    if (iDTMFNotifier)
        {
        iDTMFNotifier->SetDtmf(dtmfpckg);
        }

    TRACE_PRN_IF_ERR(status);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::StopDTMF
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::StopDTMF(TMSStreamType streamtype)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);

    if (streamtype == TMS_STREAM_DOWNLINK && iDTMFDnlinkPlayer)
        {
        iDTMFDnlinkPlayer->Cancel();
        }
    else if (streamtype == TMS_STREAM_UPLINK && iDTMFUplinkPlayer)
        {
        //status = iDTMFUplinkPlayer->StopDtmfTone();
        iDTMFUplinkPlayer->Cancel();
        }

    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::ContinueDTMF
//
// -----------------------------------------------------------------------------
//
gint TMSCallIPAdpt::ContinueDTMF(const gboolean /*sending*/)
    {
    return TMS_RESULT_FEATURE_NOT_SUPPORTED;
    }

//From DTMFTonePlayerObserver
// -----------------------------------------------------------------------------
// TMSCallIPAdpt::DTMFInitCompleted
//
// -----------------------------------------------------------------------------
//
void TMSCallIPAdpt::DTMFInitCompleted(gint /*status*/)
    {
    TRACE_PRN_FN_ENT;
    // TODO: process error
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::DTMFToneFinished
//
// -----------------------------------------------------------------------------
//
void TMSCallIPAdpt::DTMFToneFinished(gint status)
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_IF_ERR(status);
    TmsMsgBufPckg dtmfpckg;

    // KErrUnderflow indicates end of DTMF playback.
    if (status == KErrUnderflow /*|| status == KErrInUse*/)
        {
        status = TMS_RESULT_SUCCESS;
        }
    dtmfpckg().iStatus = TMSUtility::TMSResult(status);
    dtmfpckg().iRequest = ECmdDTMFTonePlayFinished;
    if (iDTMFNotifier)
        {
        iDTMFNotifier->SetDtmf(dtmfpckg);
        }
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::DownlinkInitCompleted
// From TMSIPDevSoundObserver
// -----------------------------------------------------------------------------
//
void TMSCallIPAdpt::DownlinkInitCompleted(gint status)
    {
    TRACE_PRN_FN_ENT;
    if (status == TMS_RESULT_SUCCESS)
        {
        iDnlinkInitialized = TRUE;
        }
    NotifyClient(iDnlinkStreamId, ECmdDownlinkInitComplete, status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::UplinkInitCompleted
// From TMSIPDevSoundObserver
// -----------------------------------------------------------------------------
//
void TMSCallIPAdpt::UplinkInitCompleted(gint status)
    {
    TRACE_PRN_FN_ENT;
    if (status == TMS_RESULT_SUCCESS)
        {
        iUplinkInitialized = TRUE;
        }
    NotifyClient(iUplinkStreamId, ECmdUplinkInitComplete, status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::UplinkStarted
// From TMSIPDevSoundObserver
// -----------------------------------------------------------------------------
//
void TMSCallIPAdpt::UplinkStarted(gint status)
    {
    TRACE_PRN_FN_ENT;
    NotifyClient(iUplinkStreamId, ECmdUplinkStarted, status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::DownlinkStarted
// From TMSIPDevSoundObserver
// -----------------------------------------------------------------------------
//
void TMSCallIPAdpt::DownlinkStarted(gint status)
    {
    TRACE_PRN_FN_ENT;
    NotifyClient(iDnlinkStreamId, ECmdDownlinkStarted, status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallIPAdpt::NotifyClient
// -----------------------------------------------------------------------------
//
void TMSCallIPAdpt::NotifyClient(const gint strmId, const gint command,
        const gint status, const gint64 /*int64*/)
    {
    iMsgBuffer.iRequest = command;
    iMsgBuffer.iStatus = status;

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
