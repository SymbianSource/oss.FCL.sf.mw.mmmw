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
#include "csuplink.h"
#include "csdownlink.h"
#include "callcsadpt.h"
#include "tmsutility.h"
#include "tmsshared.h"
#include "tmsclientserver.h"
#include "tarsettings.h"

using namespace TMS;

// -----------------------------------------------------------------------------
// CallCSAdpt::CallCSAdpt
//
// -----------------------------------------------------------------------------
//
CallCSAdpt::CallCSAdpt()
    {
    TRACE_PRN_FN_ENT;

    iCSDownlink = NULL;
    iCSUplink = NULL;
    iRouting = NULL;

    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::~CallCSAdpt
//
// -----------------------------------------------------------------------------
//
CallCSAdpt::~CallCSAdpt()
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
// CallCSAdpt::PostConstruct
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::PostConstruct()
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
// CallCSAdpt::CreateStream
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::CreateStream(TMSCallType /*callType*/,
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
// CallCSAdpt::InitStream
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::InitStreamL(TMSCallType /*callType*/,
        TMSStreamType strmType, gint strmId, TMSFormatType /*frmtType*/,
        const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);

    switch (strmType)
        {
        case TMS_STREAM_UPLINK:
            if (strmId == iUplinkStreamId)
                {
                // Open message queue handling client-server communication
                if (iMsgQueueUp.Handle() <= 0)
                    {
                    // Third argument in TMSCallProxy::InitStream
                    status = iMsgQueueUp.Open(aMessage, 1);
                    }
                if (status == TMS_RESULT_SUCCESS)
                    {
                    TRAP(status, iCSUplink = CSUplink::NewL(*this));
                    }
                iStrmtype = TMS_STREAM_UPLINK;
                }
            else
                {
                status = TMS_RESULT_DOES_NOT_EXIST;
                }
            break;
        case TMS_STREAM_DOWNLINK:
            if (strmId == iDnlinkStreamId)
                {
                // Open message queue handling client-server communication
                if (iMsgQueueDn.Handle() <= 0)
                    {
                    // Third argument in TMSCallProxy::InitStream
                    status = iMsgQueueDn.Open(aMessage, 1);
                    }
                if (status == TMS_RESULT_SUCCESS)
                    {
                    TRAP(status, iCSDownlink = CSDownlink::NewL(*this));
                    if (status == TMS_RESULT_SUCCESS)
                        {
                        TRAP(status, iRouting =
                             CTelephonyAudioRouting::NewL(*this));
                        if (status == TMS_RESULT_SUCCESS)
                            {
                            iTarSettings = TarSettings::NewL();
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
        default:
            status = TMS_RESULT_STREAM_TYPE_NOT_SUPPORTED;
            break;
        }

    TRACE_PRN_IF_ERR(status);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::StartStream
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::StartStream(TMSCallType /*callType*/,
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
// CallCSAdpt::PauseStream
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::PauseStream(TMSCallType /*callType*/,
        TMSStreamType /*strmType*/, gint /*strmId*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::StopStream
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::StopStream(TMSCallType /*callType*/, TMSStreamType strmType,
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
// CallCSAdpt::DeinitStream
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::DeinitStream(TMSCallType /*callType*/,
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
// CallCSAdpt::DeleteStream
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::DeleteStream(TMSCallType /*callType*/,
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
// CallCSAdpt::DataXferBufferEmptied
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::DataXferBufferEmptied(TMSCallType /*callType*/,
        TMSStreamType /*strmType*/, gint /*strmId*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::DataXferBufferFilled
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::DataXferBufferFilled(TMSCallType /*callType*/,
        TMSStreamType /*strmType*/, gint /*strmId*/, guint /*datasize*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::GetDataXferBufferHndl
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::GetDataXferBufferHndl(const TMSCallType /*callType*/,
        const TMSStreamType /*strmType*/, const gint /*strmId*/,
        const guint32 /*key*/, RChunk& /*chunk*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::GetMaxVolume
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::GetMaxVolume(guint& volume)
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
// CallCSAdpt::SetVolume
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::SetVolume(const guint volume)
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
// CallCSAdpt::GetVolume
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::GetVolume(guint& volume)
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
// CallCSAdpt::GetMaxGain
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::GetMaxGain(guint& gain)
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
// CallCSAdpt::SetGain
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::SetGain(const guint gain)
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
// CallCSAdpt::GetGain
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::GetGain(guint& gain)
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
// CallCSAdpt::GetGlobalMaxVolume
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::GetGlobalMaxVolume(guint& volume)
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
// CallCSAdpt::SetGlobalVolume
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::SetGlobalVolume(const guint volume)
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
// CallCSAdpt::GetGlobalVolume
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::GetGlobalVolume(guint& volume)
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
// CallCSAdpt::GetMaxGain
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::GetGlobalMaxGain(guint& gain)
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
// CallCSAdpt::SetGain
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::SetGlobalGain(const guint gain)
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
// CallCSAdpt::GetGlobalGain
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::GetGlobalGain(guint& gain)
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
// CallCSAdpt::GetCodecMode
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::GetCodecMode(const TMSFormatType /*fmttype*/,
        const TMSStreamType /*strmtype*/, gint& /*mode*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::SetCodecMode
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::SetCodecMode(const TMSFormatType /*fmttype*/,
        const TMSStreamType /*strmtype*/, const gint /*mode*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::GetSupportedBitRatesCount
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::GetSupportedBitRatesCount(guint& /*count*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::GetSupportedBitRates
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::GetSupportedBitRates(CBufFlat*& brbuffer)
    {
    TRACE_PRN_FN_ENT;
    TRAPD(status, GetSupportedBitRatesL(brbuffer));
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::GetSupportedBitRatesL
//
// GetSupportedBitRates implementation which can leave.
// -----------------------------------------------------------------------------
//
void CallCSAdpt::GetSupportedBitRatesL(CBufFlat*& /*brbuffer*/)
    {
    User::Leave(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::GetBitRate
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::GetBitRate(guint& /*bitrate*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::SetBitRate
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::SetBitRate(const guint /*bitrate*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::GetVAD
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::GetVAD(const TMSFormatType /*fmttype*/, gboolean& /*vad*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::SetVAD
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::SetVAD(const TMSFormatType /*fmttype*/, const gboolean /*vad*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::GetCNG
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::GetCNG(const TMSFormatType /*fmttype*/, gboolean& /*cng*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::SetCNG
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::SetCNG(const TMSFormatType /*fmttype*/, const gboolean /*cng*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::GetPlc
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::GetPlc(const TMSFormatType /*fmttype*/, gboolean& /*plc*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::SetPlc
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::SetPlc(const TMSFormatType /*fmttype*/, const gboolean /*plc*/)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_FEATURE_NOT_SUPPORTED);
    TRACE_PRN_FN_EXT;
    return status;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::SetOutput
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::SetOutput(TMSAudioOutput output)
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
// CallCSAdpt::GetOutput
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::GetOutput(TMSAudioOutput& output)
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
// CallCSAdpt::GetPreviousOutput
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::GetPreviousOutput(TMSAudioOutput& output)
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
// CallCSAdpt::GetAvailableOutputsL
//
// -----------------------------------------------------------------------------
//
gint CallCSAdpt::GetAvailableOutputsL(gint& count, CBufFlat*& outputsbuffer)
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
            //TRACE_PRN_N1(_L("TMS->CallIPAdpt: outputs: [%d]"), availableOutputs[i]);
            }

        CleanupStack::PopAndDestroy(&stream);
        status = TMS_RESULT_SUCCESS;
        }

    TRACE_PRN_FN_EXT;
    return status;
    }


// From MCSPDevSoundObserver

// -----------------------------------------------------------------------------
// CallCSAdpt::DownlinkInitCompleted
//
// -----------------------------------------------------------------------------
//
void CallCSAdpt::DownlinkInitCompleted(TInt status)
    {
    TRACE_PRN_FN_ENT;
    NotifyClient(iDnlinkStreamId, ECmdDownlinkInitComplete, status, 0);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::UplinkInitCompleted
//
// -----------------------------------------------------------------------------
//
void CallCSAdpt::UplinkInitCompleted(TInt status)
    {
    TRACE_PRN_FN_ENT;
    NotifyClient(iUplinkStreamId, ECmdUplinkInitComplete, status, 0);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::UplinkActivatedSuccessfully
//
// -----------------------------------------------------------------------------
//
void CallCSAdpt::UplinkActivatedSuccessfully()
    {
    TRACE_PRN_FN_ENT;
    NotifyClient(iUplinkStreamId, ECmdUplinkStarted, KErrNone, 0);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::DownlinkActivatedSuccessfully
//
// -----------------------------------------------------------------------------
//
void CallCSAdpt::DownlinkActivatedSuccessfully()
    {
    TRACE_PRN_FN_ENT;
    NotifyClient(iDnlinkStreamId, ECmdDownlinkStarted, KErrNone, 0);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::UplinkActivationFailed
//
// -----------------------------------------------------------------------------
//
void CallCSAdpt::UplinkActivationFailed()
    {
    TRACE_PRN_FN_ENT;
    NotifyClient(iUplinkStreamId, ECmdUplinkStarted, TMS_RESULT_FATAL_ERROR, 0);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::DownlinkActivationFailed
//
// -----------------------------------------------------------------------------
//
void CallCSAdpt::DownlinkActivationFailed()
    {
    TRACE_PRN_FN_ENT;
    NotifyClient(iDnlinkStreamId, ECmdDownlinkStarted,
            TMS_RESULT_FATAL_ERROR, 0);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::AvailableOutputsChanged
//
// -----------------------------------------------------------------------------
//
void CallCSAdpt::AvailableOutputsChanged(
        CTelephonyAudioRouting& /*aTelephonyAudioRouting*/)
    {
    TRACE_PRN_FN_ENT;
    TRoutingMsgBufPckg pckg;
    pckg().iEvent = TMS_EVENT_ROUTING_AVAIL_OUTPUTS_CHANGED;
    iTarSettings->SetTar(pckg, ETrue);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::OutputChanged
//
// -----------------------------------------------------------------------------
//
void CallCSAdpt::OutputChanged(CTelephonyAudioRouting& aTelephonyAudioRouting)
    {
    TRACE_PRN_FN_ENT;
    TRoutingMsgBufPckg pckg;
    pckg().iEvent = TMS_EVENT_ROUTING_OUTPUT_CHANGED;
    pckg().iOutput = TOTMSOUTPUT(aTelephonyAudioRouting.Output());
    iTarSettings->SetTar(pckg, ETrue);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::SetOutputComplete
//
// -----------------------------------------------------------------------------
//
void CallCSAdpt::SetOutputComplete(
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
    //TRACE_PRN_IF_ERR(aError);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CallCSAdpt::NotifyClient
//
// -----------------------------------------------------------------------------
//
void CallCSAdpt::NotifyClient(const gint strmId, const gint aCommand,
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
