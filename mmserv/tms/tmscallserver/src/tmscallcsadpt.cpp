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

using namespace TMS;

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::TMSCallCSAdpt
//
// -----------------------------------------------------------------------------
//
TMSCallCSAdpt::TMSCallCSAdpt()
    {
    TRACE_PRN_FN_ENT;

    iCSDownlink = NULL;
    iCSUplink = NULL;
    iRouting = NULL;
    iTarSettings = NULL;

    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::~TMSCallCSAdpt
//
// -----------------------------------------------------------------------------
//
TMSCallCSAdpt::~TMSCallCSAdpt()
    {
    TRACE_PRN_FN_ENT;
    delete iCSDownlink;
    delete iCSUplink;
    delete iRouting;
    delete iTarSettings;

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
gint TMSCallCSAdpt::CreateStream(TMSCallType /*callType*/,
        TMSStreamType strmType, gint& outStrmId)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);
    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            status = TMS_RESULT_ALREADY_EXIST;
            if (!iUplinkInitialized)
                {
                iUplinkInitialized = TRUE;
                iUplinkStreamId = iNextStreamId;
                outStrmId = iUplinkStreamId;
                iNextStreamId++;
                iUplinkInitialized = TRUE;
                status = TMS_RESULT_SUCCESS;
                }
            break;
        case TMS_STREAM_DOWNLINK:
            status = TMS_RESULT_ALREADY_EXIST;
            if (!iDnlinkInitialized)
                {
                iDnlinkInitialized = TRUE;
                iDnlinkStreamId = iNextStreamId;
                outStrmId = iDnlinkStreamId;
                iNextStreamId++;
                iDnlinkInitialized = TRUE;
                status = TMS_RESULT_SUCCESS;
                }
            break;
        default:
            status = TMS_RESULT_STREAM_TYPE_NOT_SUPPORTED;
            break;
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::InitStream
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::InitStream(TMSCallType /*callType*/, TMSStreamType strmType,
        gint strmId, TMSFormatType /*frmtType*/, const RMessage2& message)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);

    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            {
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
                    TRAP(status, iCSUplink = TMSCSUplink::NewL(*this));
                    }
                iStrmtype = TMS_STREAM_UPLINK;
                }
            else
                {
                status = TMS_RESULT_DOES_NOT_EXIST;
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            {
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
                    TRAP(status, iCSDownlink = TMSCSDownlink::NewL(*this));
                    if (status == TMS_RESULT_SUCCESS)
                        {
                        TRAP(status, iRouting =
                                CTelephonyAudioRouting::NewL(*this));
                        if (status == TMS_RESULT_SUCCESS)
                            {
                            TRAP(status, iTarSettings = TMSTarSettings::NewL());
                            }
                        }
                    }
                iStrmtype = TMS_STREAM_DOWNLINK;
                }
            else
                {
                status = TMS_RESULT_DOES_NOT_EXIST;
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
// TMSCallCSAdpt::StartStream
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::StartStream(TMSCallType /*callType*/,
        TMSStreamType strmType, gint strmId)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);
    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            if (iCSUplink && strmId == iUplinkStreamId)
                {
                iCSUplink->Activate();
                }
            break;
        case TMS_STREAM_DOWNLINK:
            if (iCSDownlink && strmId == iDnlinkStreamId)
                {
                iCSDownlink->Activate();
                }
            break;
        default:
            status = TMS_RESULT_STREAM_TYPE_NOT_SUPPORTED;
            break;
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::PauseStream
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::PauseStream(TMSCallType /*callType*/,
        TMSStreamType /*strmType*/, gint /*strmId*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::StopStream
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::StopStream(TMSCallType /*callType*/, TMSStreamType strmType,
        gint strmId)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);
    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            if (iCSUplink && strmId == iUplinkStreamId)
                {
                iCSUplink->Deactivate();
                NotifyClient(iUplinkStreamId, ECmdUplinkInitComplete,
                        status, 0);
                }
            break;
        case TMS_STREAM_DOWNLINK:
            if (iCSDownlink && strmId == iDnlinkStreamId)
                {
                iCSDownlink->Deactivate();
                NotifyClient(iDnlinkStreamId, ECmdDownlinkInitComplete,
                        status, 0);
                }
            break;
        default:
            status = TMS_RESULT_STREAM_TYPE_NOT_SUPPORTED;
            break;
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::DeinitStream
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::DeinitStream(TMSCallType /*callType*/,
        TMSStreamType strmType, gint strmId)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);
    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            if (iCSUplink && strmId == iUplinkStreamId)
                {
                iCSUplink->Deactivate();
                iUplinkInitialized = FALSE;
                NotifyClient(iUplinkStreamId, ECmdUplinkDeInitComplete,
                        status, 0);
                }
            break;
        case TMS_STREAM_DOWNLINK:
            if (iCSDownlink && strmId == iDnlinkStreamId)
                {
                iCSDownlink->Deactivate();
                iDnlinkInitialized = FALSE;
                NotifyClient(iDnlinkStreamId, ECmdDownlinkDeInitComplete,
                        status, 0);
                }
            break;
        default:
            status = TMS_RESULT_STREAM_TYPE_NOT_SUPPORTED;
            break;
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::DeleteStream
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::DeleteStream(TMSCallType /*callType*/,
        TMSStreamType strmType, gint strmId)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);
    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            if (strmId == iUplinkStreamId)
                {
                iUplinkStreamId = -1;
                iUplinkInitialized = FALSE;
                }
            break;
        case TMS_STREAM_DOWNLINK:
            if (strmId == iDnlinkStreamId)
                {
                iDnlinkStreamId = -1;
                iDnlinkInitialized = FALSE;
                }
            break;
        default:
            status = TMS_RESULT_STREAM_TYPE_NOT_SUPPORTED;
            break;
        }
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::DataXferBufferEmptied
//
// -----------------------------------------------------------------------------
//
gint TMSCallCSAdpt::DataXferBufferEmptied(TMSCallType /*callType*/,
        TMSStreamType /*strmType*/, gint /*strmId*/)
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
gint TMSCallCSAdpt::DataXferBufferFilled(TMSCallType /*callType*/,
        TMSStreamType /*strmType*/, gint /*strmId*/, guint /*datasize*/)
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
    gint status(TMS_RESULT_ILLEGAL_OPERATION);
    if (iCSDownlink && iDnlinkInitialized)
        {
        volume = iCSDownlink->MaxVolume();
        status = TMS_RESULT_SUCCESS;
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
    gint status(TMS_RESULT_ILLEGAL_OPERATION);
    if (iCSDownlink && iDnlinkInitialized)
        {
        iCSDownlink->SetVolume(volume);
        status = TMS_RESULT_SUCCESS;
        NotifyClient(iDnlinkStreamId, ECmdSetVolume, status, 0);
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
    gint status(TMS_RESULT_ILLEGAL_OPERATION);
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
    gint status(TMS_RESULT_ILLEGAL_OPERATION);
    if (iCSUplink && iUplinkInitialized)
        {
        gain = iCSUplink->MaxGain();
        status = TMS_RESULT_SUCCESS;
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
    gint status(TMS_RESULT_ILLEGAL_OPERATION);
    if (iUplinkInitialized)
        {
        iCSUplink->SetGain(gain);
        status = TMS_RESULT_SUCCESS;
        NotifyClient(iUplinkInitialized, ECmdSetGain, status, 0);
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
    gint status(TMS_RESULT_ILLEGAL_OPERATION);
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
    gint status(TMS_RESULT_ILLEGAL_OPERATION);
    if (iCSDownlink && iDnlinkInitialized)
        {
        volume = iCSDownlink->MaxVolume();
        status = TMS_RESULT_SUCCESS;
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
    gint status(TMS_RESULT_SUCCESS);

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
    gint status(TMS_RESULT_ILLEGAL_OPERATION);
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
    gint status(TMS_RESULT_ILLEGAL_OPERATION);
    if (iCSUplink && iUplinkInitialized)
        {
        gain = iCSUplink->MaxGain();
        status = TMS_RESULT_SUCCESS;
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
    gint status(TMS_RESULT_SUCCESS);

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
    gint status(TMS_RESULT_ILLEGAL_OPERATION);
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
//
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
//
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
//
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
gint TMSCallCSAdpt::GetSupportedBitRates(CBufFlat*& brbuffer)
    {
    TRACE_PRN_FN_ENT;
    TRAPD(status, GetSupportedBitRatesL(brbuffer));
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::GetSupportedBitRatesL
//
// GetSupportedBitRates implementation which can leave.
// -----------------------------------------------------------------------------
//
void TMSCallCSAdpt::GetSupportedBitRatesL(CBufFlat*& /*brbuffer*/)
    {
    User::Leave(TMS_RESULT_FEATURE_NOT_SUPPORTED);
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
gint TMSCallCSAdpt::SetVAD(const TMSFormatType /*fmttype*/, const gboolean /*vad*/)
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
gint TMSCallCSAdpt::SetOutput(TMSAudioOutput output)
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
gint TMSCallCSAdpt::GetAvailableOutputsL(gint& count, CBufFlat*& outputsbuffer)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_UNINITIALIZED_OBJECT);
    TMSAudioOutput tmsoutput;

    if (iRouting)
        {
        RBufWriteStream stream;
        stream.Open(*outputsbuffer);
        CleanupClosePushL(stream);

        TArray<CTelephonyAudioRouting::TAudioOutput> availableOutputs =
                iRouting->AvailableOutputs();

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


// From TMSCSPDevSoundObserver

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::DownlinkInitCompleted
//
// -----------------------------------------------------------------------------
//
void TMSCallCSAdpt::DownlinkInitCompleted(gint status)
    {
    TRACE_PRN_FN_ENT;
    NotifyClient(iDnlinkStreamId, ECmdDownlinkInitComplete, status, 0);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::UplinkInitCompleted
//
// -----------------------------------------------------------------------------
//
void TMSCallCSAdpt::UplinkInitCompleted(gint status)
    {
    TRACE_PRN_FN_ENT;
    NotifyClient(iUplinkStreamId, ECmdUplinkInitComplete, status, 0);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::UplinkActivatedSuccessfully
//
// -----------------------------------------------------------------------------
//
void TMSCallCSAdpt::UplinkActivatedSuccessfully()
    {
    TRACE_PRN_FN_ENT;
    NotifyClient(iUplinkStreamId, ECmdUplinkStarted, KErrNone, 0);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::DownlinkActivatedSuccessfully
//
// -----------------------------------------------------------------------------
//
void TMSCallCSAdpt::DownlinkActivatedSuccessfully()
    {
    TRACE_PRN_FN_ENT;
    NotifyClient(iDnlinkStreamId, ECmdDownlinkStarted, KErrNone, 0);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::UplinkActivationFailed
//
// -----------------------------------------------------------------------------
//
void TMSCallCSAdpt::UplinkActivationFailed()
    {
    TRACE_PRN_FN_ENT;
    NotifyClient(iUplinkStreamId, ECmdUplinkStarted, TMS_RESULT_FATAL_ERROR, 0);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallCSAdpt::DownlinkActivationFailed
//
// -----------------------------------------------------------------------------
//
void TMSCallCSAdpt::DownlinkActivationFailed()
    {
    TRACE_PRN_FN_ENT;
    NotifyClient(iDnlinkStreamId, ECmdDownlinkStarted,
            TMS_RESULT_FATAL_ERROR, 0);
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
        CTelephonyAudioRouting& aTelephonyAudioRouting, gint /*aError*/)
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
