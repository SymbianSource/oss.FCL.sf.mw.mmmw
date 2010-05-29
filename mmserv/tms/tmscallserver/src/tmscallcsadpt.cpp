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

#include <tms.h>
#include <S60FourCC.h>
#include <AudioPreference.h>
#include <TelephonyAudioRouting.h>
#include "tmscsuplink.h"
#include "tmscsdownlink.h"
#include "tmscallcsadpt.h"
#include "tmsutility.h"
#include "tmsshared.h"
#include "tmsclientserver.h"
#include "tmstarsettings.h"
#include "tmsdtmftoneplayer.h"
#include "tmsdtmfprovider.h"
#include "tmsdtmfnotifier.h"

using namespace TMS;

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::NewL
// Symbian constructor.
// -----------------------------------------------------------------------------
//
TMSCallCSAdpt* TMSCallCSAdpt::NewL()
    {
    TMSCallCSAdpt* self = new (ELeave) TMSCallCSAdpt();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::ConstructL
// 2-nd phase constructor.
// -----------------------------------------------------------------------------
//
void TMSCallCSAdpt::ConstructL()
    {
    TRACE_PRN_FN_ENT;
    iCSDownlink = NULL;
    iCSUplink = NULL;
    iRouting = NULL;
    iTarSettings = NULL;
    iDTMFDnlinkPlayer = NULL;
    iDTMFUplinkPlayer = NULL;
    iDTMFNotifier = NULL;
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::TMSCallCSAdpt
//
// -----------------------------------------------------------------------------
//
TMSCallCSAdpt::TMSCallCSAdpt()
    {
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::~TMSCallCSAdpt
//
// -----------------------------------------------------------------------------
//
TMSCallCSAdpt::~TMSCallCSAdpt()
    {
    TRACE_PRN_FN_ENT;

    delete iRouting;
    delete iTarSettings;
    delete iDTMFUplinkPlayer;
    delete iDTMFDnlinkPlayer;
    delete iDTMFNotifier;
    delete iCSUplink;
    delete iCSDownlink;

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
// TMSCallCSAdpt::PostConstruct
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::PostConstruct()
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
// TMSCallCSAdpt::CreateStream
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::CreateStream(const TMSCallType /*callType*/,
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
// TMSCallCSAdpt::InitStream
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::InitStream(const TMSCallType /*callType*/,
        const TMSStreamType strmType, const gint strmId,
        const TMSFormatType /*frmtType*/, const gint retrytime,
        const RMessage2& message)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);

    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            {
            status = TMS_RESULT_DOES_NOT_EXIST;
            if (strmId == iUplinkStreamId)
                {
                // Open message queue handling client-server communication
                if (iMsgQueueUp.Handle() <= 0)
                    {
                    // Third argument in TMSCallProxy::InitStream
                    status = iMsgQueueUp.Open(message, 1);
                    }
                if (status == TMS_RESULT_SUCCESS)
                    {
                    status = InitUplink(retrytime);
                    }
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            {
            status = TMS_RESULT_DOES_NOT_EXIST;
            if (strmId == iDnlinkStreamId)
                {
                // Open message queue handling client-server communication
                if (iMsgQueueDn.Handle() <= 0)
                    {
                    // Third argument in TMSCallProxy::InitStream
                    status = iMsgQueueDn.Open(message, 1);
                    }
                if (status == TMS_RESULT_SUCCESS)
                    {
                    status = InitDownlink(retrytime);
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
// TMSCallCSAdpt::InitUplink
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::InitUplink(const gint retrytime)
    {
    gint status(TMS_RESULT_SUCCESS);

    if (!iCSUplink)
        {
        TRAP(status, iCSUplink = TMSCSUplink::NewL(*this, retrytime));
        }
    if (!iDTMFUplinkPlayer && status == TMS_RESULT_SUCCESS)
        {
        TRAP(status, iDTMFUplinkPlayer = TMSDTMFProvider::NewL());
        iDTMFUplinkPlayer->AddObserver(*this);
        }
    if (!iDTMFNotifier && status == TMS_RESULT_SUCCESS)
        {
        TRAP(status, iDTMFNotifier = TMSDtmfNotifier::NewL());
        }
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::InitDownlink
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::InitDownlink(const gint retrytime)
    {
    gint status(TMS_RESULT_SUCCESS);

    if (!iCSDownlink)
        {
        TRAP(status, iCSDownlink = TMSCSDownlink::NewL(*this, retrytime));
        }
    if (!iRouting && status == TMS_RESULT_SUCCESS)
        {
        TRAP(status, iRouting = CTelephonyAudioRouting::NewL(*this));
        }
    if (!iTarSettings && status == TMS_RESULT_SUCCESS)
        {
        TRAP(status, iTarSettings = TMSTarSettings::NewL());
        }
    if (!iDTMFDnlinkPlayer && status == TMS_RESULT_SUCCESS)
        {
        TRAP(status, iDTMFDnlinkPlayer = TMSAudioDtmfTonePlayer::NewL(*this,
                KAudioDTMFString, KAudioPriorityDTMFString));
        }
    if (!iDTMFNotifier && status == TMS_RESULT_SUCCESS)
        {
        TRAP(status, iDTMFNotifier = TMSDtmfNotifier::NewL());
        }
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::StartStream
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::StartStream(const TMSCallType /*callType*/,
        const TMSStreamType strmType, const gint strmId, const gint retrytime)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            {
            if (iCSUplink && strmId == iUplinkStreamId)
                {
                iCSUplink->Activate(retrytime);
                status = TMS_RESULT_SUCCESS;
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            {
            if (iCSDownlink && strmId == iDnlinkStreamId)
                {
                iCSDownlink->Activate(retrytime);
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
// TMSCallCSAdpt::PauseStream
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::PauseStream(const TMSCallType /*callType*/,
        const TMSStreamType /*strmType*/, const gint /*strmId*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::StopStream
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::StopStream(const TMSCallType /*callType*/,
        const TMSStreamType strmType, const gint strmId)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);

    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            {
            if (iCSUplink && strmId == iUplinkStreamId)
                {
                iCSUplink->Deactivate();
                status = TMS_RESULT_SUCCESS;
                NotifyClient(iUplinkStreamId, ECmdUplinkInitComplete, status);
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            {
            if (iCSDownlink && strmId == iDnlinkStreamId)
                {
                iCSDownlink->Deactivate();
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
// TMSCallCSAdpt::DeinitStream
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::DeinitStream(const TMSCallType /*callType*/,
        const TMSStreamType strmType, const gint strmId)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);

    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            {
            if (iCSUplink && strmId == iUplinkStreamId)
                {
                iCSUplink->Deactivate();
                iUplinkInitialized = FALSE;
                status = TMS_RESULT_SUCCESS;
                NotifyClient(iUplinkStreamId, ECmdUplinkDeInitComplete, status);
                }
            break;
        case TMS_STREAM_DOWNLINK:
            if (iCSDownlink && strmId == iDnlinkStreamId)
                {
                iCSDownlink->Deactivate();
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
// TMSCallCSAdpt::DeleteStream
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::DeleteStream(const TMSCallType /*callType*/,
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
// TMSCallCSAdpt::DataXferBufferEmptied
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::DataXferBufferEmptied(const TMSCallType /*callType*/,
        const TMSStreamType /*strmType*/, const gint /*strmId*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::DataXferBufferFilled
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::DataXferBufferFilled(const TMSCallType /*callType*/,
        const TMSStreamType /*strmType*/, const gint /*strmId*/,
        const guint /*datasize*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::GetDataXferBufferHndl
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::GetDataXferBufferHndl(const TMSCallType /*callType*/,
        const TMSStreamType /*strmType*/, const gint /*strmId*/,
        const guint32 /*key*/, RChunk& /*chunk*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::GetMaxVolume
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::GetMaxVolume(guint& volume)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iCSDownlink && iDnlinkInitialized)
        {
        volume = iCSDownlink->MaxVolume();
        status = TMS_RESULT_SUCCESS;
        TRACE_PRN_N1(_L("TMS->TMSCallCSAdpt: GetMaxVolume [%d]"), volume);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::SetVolume
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::SetVolume(const guint volume)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iCSDownlink && iDnlinkInitialized)
        {
        iCSDownlink->SetVolume(volume);
        status = TMS_RESULT_SUCCESS;
        NotifyClient(iDnlinkStreamId, ECmdSetVolume, status);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::GetVolume
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::GetVolume(guint& volume)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iCSDownlink && iDnlinkInitialized)
        {
        volume = iCSDownlink->Volume();
        status = TMS_RESULT_SUCCESS;
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::GetMaxGain
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::GetMaxGain(guint& gain)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iCSUplink && iUplinkInitialized)
        {
        gain = iCSUplink->MaxGain();
        status = TMS_RESULT_SUCCESS;
        TRACE_PRN_N1(_L("TMS->TMSCallCSAdpt::GetMaxGain [%d]"), gain);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::SetGain
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::SetGain(const guint gain)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iCSUplink && iUplinkInitialized)
        {
        iCSUplink->SetGain(gain);
        status = TMS_RESULT_SUCCESS;
        NotifyClient(iUplinkStreamId, ECmdSetGain, status);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::GetGain
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::GetGain(guint& gain)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iCSUplink && iUplinkInitialized)
        {
        gain = iCSUplink->Gain();
        status = TMS_RESULT_SUCCESS;
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::GetGlobalMaxVolume
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::GetGlobalMaxVolume(guint& volume)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iCSDownlink && iDnlinkInitialized)
        {
        volume = iCSDownlink->MaxVolume();
        status = TMS_RESULT_SUCCESS;
        TRACE_PRN_N1(_L("TMS->TMSCallCSAdpt::GetGlobalMaxVolume [%d]"), volume);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::SetGlobalVolume
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::SetGlobalVolume(const guint volume)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    iGlobalVol = volume;
    if (iCSDownlink && iDnlinkInitialized)
        {
        iCSDownlink->SetVolume(volume);
        status = TMS_RESULT_SUCCESS;
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::GetGlobalVolume
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::GetGlobalVolume(guint& volume)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iCSDownlink && iDnlinkInitialized)
        {
        volume = iCSDownlink->Volume();
        status = TMS_RESULT_SUCCESS;
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::GetMaxGain
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::GetGlobalMaxGain(guint& gain)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iCSUplink && iUplinkInitialized)
        {
        gain = iCSUplink->MaxGain();
        status = TMS_RESULT_SUCCESS;
        TRACE_PRN_N1(_L("TMS->TMSCallCSAdpt::GetGlobalMaxGain [%d]"), gain);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::SetGain
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::SetGlobalGain(const guint gain)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    iGlobalGain = gain;
    if (iCSUplink && iUplinkInitialized)
        {
        iCSUplink->SetGain(gain);
        status = TMS_RESULT_SUCCESS;
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::GetGlobalGain
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::GetGlobalGain(guint& gain)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_INVALID_STATE);
    if (iCSUplink && iUplinkInitialized)
        {
        gain = iCSUplink->Gain();
        status = TMS_RESULT_SUCCESS;
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::GetCodecMode
// No codec format in CS call
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::GetCodecMode(const TMSFormatType /*fmttype*/,
        const TMSStreamType /*strmtype*/, gint& /*mode*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::SetCodecMode
// No codec format in CS call
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::SetCodecMode(const TMSFormatType /*fmttype*/,
        const TMSStreamType /*strmtype*/, const gint /*mode*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::GetSupportedBitRatesCount
// No codec format in CS call
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::GetSupportedBitRatesCount(guint& /*count*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::GetSupportedBitRates
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::GetSupportedBitRates(CBufFlat*& /*brbuffer*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::GetBitRate
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::GetBitRate(guint& /*bitrate*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::SetBitRate
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::SetBitRate(const guint /*bitrate*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::GetVAD
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::GetVAD(const TMSFormatType /*fmttype*/, gboolean& /*vad*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::SetVAD
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::SetVAD(const TMSFormatType /*fmttype*/,
        const gboolean /*vad*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::GetCNG
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::GetCNG(const TMSFormatType /*fmttype*/, gboolean& /*cng*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::SetCNG
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::SetCNG(const TMSFormatType /*fmttype*/,
        const gboolean /*cng*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::GetPlc
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::GetPlc(const TMSFormatType /*fmttype*/, gboolean& /*plc*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::SetPlc
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::SetPlc(const TMSFormatType /*fmttype*/,
        const gboolean /*plc*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::SetOutput
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::SetOutput(const TMSAudioOutput output)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);

    if (iRouting)
        {
        CTelephonyAudioRouting::TAudioOutput taroutput = TOTAROUTPUT(output);
        TRAP(status, iRouting->SetOutputL(taroutput));
        if (status == KErrArgument)
            {
            status = TMS_RESULT_INVALID_ARGUMENT;
            }
        else
            {
            status = TMS_RESULT_SUCCESS;
            }
        }

    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::GetOutput
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::GetOutput(TMSAudioOutput& output)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);

    if (iRouting)
        {
        CTelephonyAudioRouting::TAudioOutput taroutput;
        taroutput = iRouting->Output();
        status = TMS_RESULT_SUCCESS;
        output = TOTMSOUTPUT(taroutput);
        }

    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::GetPreviousOutput
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::GetPreviousOutput(TMSAudioOutput& output)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);

    if (iRouting)
        {
        CTelephonyAudioRouting::TAudioOutput taroutput;
        taroutput = iRouting->PreviousOutput();
        status = TMS_RESULT_SUCCESS;
        output = TOTMSOUTPUT(taroutput);
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::GetAvailableOutputsL
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::GetAvailableOutputsL(gint& count, CBufFlat*& outputsbuf)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    TMSAudioOutput tmsoutput;

    if (iRouting)
        {
        RBufWriteStream stream;
        stream.Open(*outputsbuf);
        CleanupClosePushL(stream);

        TArray<CTelephonyAudioRouting::TAudioOutput>
                availableOutputs = iRouting->AvailableOutputs();

        guint numOfItems = availableOutputs.Count();
        count = numOfItems;
        for (guint i = 0; i < numOfItems; i++)
            {
            tmsoutput = TOTMSOUTPUT(availableOutputs[i]);
            stream.WriteUint32L(tmsoutput);
            }

        CleanupStack::PopAndDestroy(&stream);
        status = TMS_RESULT_SUCCESS;
        }

    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::StartDTMF
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::StartDTMF(const TMSStreamType strmtype, TDes& dtmfstring)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_STREAM_TYPE_NOT_SUPPORTED);
    TmsMsgBufPckg dtmfpckg;
    dtmfpckg().iStatus = status;
    dtmfpckg().iRequest = ECmdDTMFTonePlayFinished;

    if (strmtype == TMS_STREAM_DOWNLINK && iDnlinkInitialized)
        {
        if (iDTMFDnlinkPlayer)
            {
            iDTMFDnlinkPlayer->PlayDtmfTone(dtmfstring);
            status = TMS_RESULT_SUCCESS;
            }
        dtmfpckg().iStatus = TMS_RESULT_SUCCESS;
        dtmfpckg().iRequest = ECmdDTMFToneDnlPlayStarted;
        }
    else if (strmtype == TMS_STREAM_UPLINK && iUplinkInitialized)
        {
        //use etel for uplink
        if (iDTMFUplinkPlayer)
            {
            status = iDTMFUplinkPlayer->SendDtmfToneString(dtmfstring);
            status = TMS_RESULT_SUCCESS;
            }
        dtmfpckg().iStatus = TMSUtility::EtelToTMSResult(status);
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
// TMSCallCSAdpt::StopDTMF
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::StopDTMF(const TMSStreamType streamtype)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);

    if (streamtype == TMS_STREAM_DOWNLINK && iDTMFDnlinkPlayer)
        {
        iDTMFDnlinkPlayer->Cancel();
        }
    else if (streamtype == TMS_STREAM_UPLINK && iDTMFUplinkPlayer)
        {
        status = iDTMFUplinkPlayer->StopDtmfTone();
        status = TMSUtility::EtelToTMSResult(status);
        }

    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::ContinueDTMF
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::ContinueDTMF(const gboolean sending)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);

    if (iDTMFUplinkPlayer)
        {
        status = iDTMFUplinkPlayer->ContinueDtmfStringSending(sending);
        status = TMSUtility::EtelToTMSResult(status);
        }

    TRACE_PRN_FN_EXT;
    return status;
    }

//From TMSDTMFTonePlayerObserver
// -----------------------------------------------------------------------------
// TMSCallCSAdpt::DTMFInitCompleted
//
// -----------------------------------------------------------------------------
//
void TMSCallCSAdpt::DTMFInitCompleted(gint /*status*/)
    {
    TRACE_PRN_FN_ENT;
    // TODO: process error
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::DTMFToneFinished
//
// -----------------------------------------------------------------------------
//
void TMSCallCSAdpt::DTMFToneFinished(gint status)
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_IF_ERR(status);
    TmsMsgBufPckg dtmfpckg;

    // KErrUnderflow indicates end of DTMF playback.
    if (status == KErrUnderflow || status == KErrInUse)
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
// TMSCallCSAdpt::HandleDTMFEvent
//
// -----------------------------------------------------------------------------
//
void TMSCallCSAdpt::HandleDTMFEvent(
        const TMSDTMFObserver::TCCPDtmfEvent event, const gint status,
        const TChar /*tone*/)
    {
    TRACE_PRN_FN_ENT;
    TmsMsgBufPckg dtmfpckg;

    TRACE_PRN_N1(_L("**TMS TMSCallCSAdpt::HandleDTMFEvent error:%d"), status);

    dtmfpckg().iStatus = TMSUtility::EtelToTMSResult(status);

    switch (event)
        {
        case ECCPDtmfUnknown:               //Unknown
            break;
        case ECCPDtmfManualStart:           //DTMF sending started manually
        case ECCPDtmfSequenceStart:         //Automatic DTMF sending initialized
            dtmfpckg().iRequest = ECmdDTMFToneUplPlayStarted;
            break;
        case ECCPDtmfManualStop:            //DTMF sending stopped manually
        case ECCPDtmfManualAbort:           //DTMF sending aborted manually
        case ECCPDtmfSequenceStop:          //Automatic DTMF sending stopped
        case ECCPDtmfSequenceAbort:         //Automatic DTMF sending aborted
        case ECCPDtmfStopInDtmfString:      //There was stop mark in DTMF string
        case ECCPDtmfStringSendingCompleted: //DTMF sending success
            dtmfpckg().iRequest = ECmdDTMFTonePlayFinished;
            break;
        default:
            break;
        }

    if (iDTMFNotifier)
        {
        iDTMFNotifier->SetDtmf(dtmfpckg, TRUE);
        }
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::DownlinkInitCompleted
// From TMSCSDevSoundObserver
// -----------------------------------------------------------------------------
//
void TMSCallCSAdpt::DownlinkInitCompleted(gint status)
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
// TMSCallCSAdpt::UplinkInitCompleted
// From TMSCSDevSoundObserver
// -----------------------------------------------------------------------------
//
void TMSCallCSAdpt::UplinkInitCompleted(gint status)
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
// TMSCallCSAdpt::UplinkActivationFailed
// From TMSCSDevSoundObserver
// -----------------------------------------------------------------------------
//
void TMSCallCSAdpt::UplinkActivationCompleted(gint status)
    {
    TRACE_PRN_FN_ENT;
    NotifyClient(iUplinkStreamId, ECmdUplinkStarted, status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::DownlinkActivationFailed
// From TMSCSDevSoundObserver
// -----------------------------------------------------------------------------
//
void TMSCallCSAdpt::DownlinkActivationCompleted(gint status)
    {
    TRACE_PRN_FN_ENT;
    NotifyClient(iDnlinkStreamId, ECmdDownlinkStarted, status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::AvailableOutputsChanged
//
// -----------------------------------------------------------------------------
//
void TMSCallCSAdpt::AvailableOutputsChanged(
        CTelephonyAudioRouting& /*aTelephonyAudioRouting*/)
    {
    TRACE_PRN_FN_ENT;
    TRoutingMsgBufPckg pckg;
    pckg().iEvent = TMS_EVENT_ROUTING_AVAIL_OUTPUTS_CHANGED;
    iTarSettings->SetTar(pckg, ETrue);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::OutputChanged
//
// -----------------------------------------------------------------------------
//
void TMSCallCSAdpt::OutputChanged(
        CTelephonyAudioRouting& aTelephonyAudioRouting)
    {
    TRACE_PRN_FN_ENT;
    TRoutingMsgBufPckg pckg;
    pckg().iEvent = TMS_EVENT_ROUTING_OUTPUT_CHANGED;
    pckg().iOutput = TOTMSOUTPUT(aTelephonyAudioRouting.Output());
    iTarSettings->SetTar(pckg, ETrue);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::SetOutputComplete
//
// -----------------------------------------------------------------------------
//
void TMSCallCSAdpt::SetOutputComplete(
        CTelephonyAudioRouting& aTelephonyAudioRouting, gint /*status*/)
    {
    TRACE_PRN_FN_ENT;
    TRoutingMsgBufPckg pckg;
    pckg().iEvent = TMS_EVENT_ROUTING_SET_OUTPUT_COMPLETE;
    pckg().iOutput = TOTMSOUTPUT(aTelephonyAudioRouting.Output());
    if (iTarSettings)
        {
        iTarSettings->SetTar(pckg, ETrue);
        }
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::NotifyClient
//
// -----------------------------------------------------------------------------
//
void TMSCallCSAdpt::NotifyClient(const gint strmId, const gint command,
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
