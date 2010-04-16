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

#include "tmsutility.h"
#include "tmsutility.h"
#include "calladpt.h"
#include "tmscallserver.h"
#include "tmscallclisrv.h"
#include "tmscallsession.h"

using namespace TMS;

const guint KArrayExpandSize = 8;

// -----------------------------------------------------------------------------
// TMSCallSession::TMSCallSession
// -----------------------------------------------------------------------------
//
TMSCallSession::TMSCallSession(TMSCallServer &aServer) :
    iTMSCallServer(aServer)
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::~TMSCallSession
// The destructor
// -----------------------------------------------------------------------------
//
TMSCallSession::~TMSCallSession()
    {
    TRACE_PRN_FN_ENT;
    delete iCallAdpt;
    iTMSCallServer.DropSession();
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::NewL
// Symbian constructor.
// -----------------------------------------------------------------------------
//
TMSCallSession* TMSCallSession::NewL(TMSCallServer &aServer)
    {
    TRACE_PRN_FN_ENT;
    TMSCallSession* self = new (ELeave) TMSCallSession(aServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    TRACE_PRN_FN_EXT;
    return self;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::ConstructL
// 2-nd phase constructor.
// -----------------------------------------------------------------------------
//
void TMSCallSession::ConstructL()
    {
    TRACE_PRN_FN_ENT;
    iTMSCallServer.AddSession(); //add new session
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::ServiceL
// Service request handler.
// -----------------------------------------------------------------------------
//
void TMSCallSession::ServiceL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    TRAPD(status, HandleMessageL(aMessage));
    if (status != TMS_RESULT_SUCCESS)
        {
        aMessage.Complete(status);
        }
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleMessageL
// Service request handler.
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleMessageL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    switch (aMessage.Function())
        {
        case TMS_CREATE_CALL:
            HandleCreateCallL(aMessage);
            break;
        case TMS_CREATE_STREAM:
            HandleCreateStreamL(aMessage);
            break;
        case TMS_INIT_STREAM:
            HandleInitStreamL(aMessage);
            break;
        case TMS_START_STREAM:
            HandleStartStreamL(aMessage);
            break;
        case TMS_PAUSE_STREAM:
            HandlePauseStreamL(aMessage);
            break;
        case TMS_STOP_STREAM:
            HandleStopStreamL(aMessage);
            break;
        case TMS_DEINIT_STREAM:
            HandleDeinitStreamL(aMessage);
            break;
        case TMS_DELETE_STREAM:
            HandleDeleteStreamL(aMessage);
            break;
        case TMS_DELETE_CALL:
            HandleDeleteCallL(aMessage);
            break;
        case TMS_DATA_XFER_BUFFER_EMPTIED:
            HandleDataXferBufferEmptiedCallL(aMessage);
            break;
        case TMS_DATA_XFER_BUFFER_FILLED:
            HandleDataXferBufferFilledCallL(aMessage);
            break;
        case TMS_DATA_XFER_BUFFER_GET_HNDL:
            HandleDataXferBufferGetHndlCallL(aMessage);
            break;
        case TMS_EFFECT_VOLUME_GETMAX:
            HandleEffectVolumeGetMaxVolL(aMessage);
            break;
        case TMS_EFFECT_VOLUME_SET:
            HandleEffectVolumeSetVolL(aMessage);
            break;
        case TMS_EFFECT_VOLUME_GET:
            HandleEffectVolumeGetVolL(aMessage);
            break;
        case TMS_EFFECT_GAIN_GETMAX:
            HandleEffectVolumeGetMaxGainL(aMessage);
            break;
        case TMS_EFFECT_GAIN_SET:
            HandleEffectVolumeSetGainL(aMessage);
            break;
        case TMS_EFFECT_GAIN_GET:
            HandleEffectVolumeGetGainL(aMessage);
            break;
        case TMS_EFFECT_GLOBAL_VOL_SET:
            HandleGlobalEffectVolumeSetVolL(aMessage);
            break;
        case TMS_EFFECT_GLOBAL_VOL_GET:
            HandleGlobalEffectVolumeGetVolL(aMessage);
            break;
        case TMS_EFFECT_GLOBAL_VOL_GETMAX:
            HandleGlobalEffectVolumeGetMaxVolL(aMessage);
            break;
        case TMS_EFFECT_GLOBAL_GAIN_SET:
            HandleGlobalEffectVolumeSetGainL(aMessage);
            break;
        case TMS_EFFECT_GLOBAL_GAIN_GET:
            HandleGlobalEffectVolumeGetGainL(aMessage);
            break;
        case TMS_EFFECT_GLOBAL_GAIN_GETMAX:
            HandleGlobalEffectVolumeGetMaxGainL(aMessage);
            break;
        case TMS_FORMAT_SETCODECMODE:
            HandleFormatSetCodecModeL(aMessage);
            break;
        case TMS_FORMAT_GETCODECMODE:
            HandleFormatGetCodecModeL(aMessage);
            break;
        case TMS_FORMAT_GETSUPPORTEDBITRATESCOUNT:
            HandleFormatGetSupportedBitRatesCountL(aMessage);
            break;
        case TMS_FORMAT_GETSUPPORTEDBITRATES:
            HandleFormatGetSupportedBitRatesL(aMessage);
            break;
        case TMS_FORMAT_SETBITRATE:
            HandleFormatSetBitRateL(aMessage);
            break;
        case TMS_FORMAT_GETBITRATE:
            HandleFormatGetBitRateL(aMessage);
            break;
        case TMS_FORMAT_SETVAD:
            HandleFormatSetVADL(aMessage);
            break;
        case TMS_FORMAT_GETVAD:
            HandleFormatGetVADL(aMessage);
            break;
        case TMS_FORMAT_SETCNG:
            HandleFormatSetCNGL(aMessage);
            break;
        case TMS_FORMAT_GETCNG:
            HandleFormatGetCNGL(aMessage);
            break;
        case TMS_FORMAT_SETPLC:
            HandleFormatSetPlcL(aMessage);
            break;
        case TMS_FORMAT_GETPLC:
            HandleFormatGetPlcL(aMessage);
            break;
        case TMS_ROUTING_OUTPUT_SET:
            HandleRoutingSetOutputL(aMessage);
            break;
        case TMS_ROUTING_OUTPUT_GET:
            HandleRoutingGetOutputL(aMessage);
            break;
        case TMS_ROUTING_PREVIOUSOUTPUT_GET:
            HandleRoutingGetPreviousOutputL(aMessage);
            break;
        case TMS_ROUTING_AVAILABLE_OUTPUT_GET:
            HandleRoutingGetAvailableOutputsL(aMessage);
            break;
        case TMS_DTMF_START:
            HandleDTMFStart(aMessage);
            break;
        case TMS_DTMF_STOP:
            HandleDTMFStop(aMessage);
            break;
        case TMS_DTMF_CONTINUE:
            HandleDTMFContinue(aMessage);
            break;
        default:
            User::Leave(TMS_RESULT_ILLEGAL_OPERATION);
            break;
        }
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleCreateCallL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleCreateCallL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_ALREADY_EXIST);
    if (!iCallAdpt)
        {
        // Get Call type
        gint callType = aMessage.Int0();
        status = TMSCallAdpt::CreateCallL(callType, iCallAdpt);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleCreateStreamL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleCreateStreamL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        TMSCliSrvStreamCreateDataStructBufPckg pckg;
        aMessage.ReadL(0, pckg);
        gint outStrmId;
        status = iCallAdpt->CreateStream(pckg().CallType, pckg().StreamType,
                outStrmId);
        if (status == TMS_RESULT_SUCCESS)
            {
            TPckgBuf<gint> outPckg(outStrmId);
            aMessage.WriteL(1, outPckg);
            }
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleInitStreamL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleInitStreamL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        TMSCliSrvStreamInitDataStructBufPckg pckg;
        aMessage.ReadL(0, pckg);
        status = iCallAdpt->InitStreamL(pckg().CallType, pckg().StreamType,
                pckg().StreamId, pckg().FormatType, aMessage);

        switch (pckg().StreamType)
            {
            case TMS_STREAM_DOWNLINK:
                {
                iCallAdpt->SetGlobalVolume(iGlobalVol);
                }
                break;
            case TMS_STREAM_UPLINK:
                {
                iCallAdpt->SetGlobalGain(iGlobalGain);
                }
                break;
            default:
                break;
            }
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleStartStreamL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleStartStreamL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        TMSCliSrvStreamOpDataStructBufPckg pckg;
        aMessage.ReadL(0, pckg);
        status = iCallAdpt->StartStream(pckg().CallType, pckg().StreamType,
                pckg().StreamId);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandlePauseStreamL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandlePauseStreamL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        TMSCliSrvStreamOpDataStructBufPckg pckg;
        aMessage.ReadL(0, pckg);
        status = iCallAdpt->PauseStream(pckg().CallType, pckg().StreamType,
                pckg().StreamId);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleStopStreamL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleStopStreamL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        TMSCliSrvStreamOpDataStructBufPckg pckg;
        aMessage.ReadL(0, pckg);
        status = iCallAdpt->StopStream(pckg().CallType, pckg().StreamType,
                pckg().StreamId);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleDeinitStreamL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleDeinitStreamL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        TMSCliSrvStreamOpDataStructBufPckg pckg;
        aMessage.ReadL(0, pckg);
        status = iCallAdpt->DeinitStream(pckg().CallType, pckg().StreamType,
                pckg().StreamId);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleDeleteStreamL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleDeleteStreamL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        TMSCliSrvStreamOpDataStructBufPckg pckg;
        aMessage.ReadL(0, pckg);
        status = iCallAdpt->DeleteStream(pckg().CallType, pckg().StreamType,
                pckg().StreamId);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleDeleteCallL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleDeleteCallL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    delete iCallAdpt;
    iCallAdpt = NULL;
    aMessage.Complete(TMS_RESULT_SUCCESS);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleDataXferBufferEmptiedCallL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleDataXferBufferEmptiedCallL(
        const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        TMSCliSrvDataXferDataStructBufPckg pckg;
        aMessage.ReadL(0, pckg);
        status = iCallAdpt->DataXferBufferEmptied(pckg().CallType,
                pckg().StreamType, pckg().StreamId);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleDataXferBufferFilledCallL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleDataXferBufferFilledCallL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        TMSCliSrvDataXferDataStructBufPckg pckg;
        aMessage.ReadL(0, pckg);
        status = iCallAdpt->DataXferBufferFilled(pckg().CallType,
                pckg().StreamType, pckg().StreamId, pckg().DataSize);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleDataXferBufferGetHndlCallL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleDataXferBufferGetHndlCallL(
        const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    RChunk chunk;

    if (iCallAdpt)
        {
        TMSCliSrvDataXferChunkHndlDataStructBufPckg pckg;
        aMessage.ReadL(0, pckg);
        status = iCallAdpt->GetDataXferBufferHndl(pckg().CallType,
                pckg().StreamType, pckg().StreamId, pckg().Key, chunk);
        }

    if (status == TMS_RESULT_SUCCESS && chunk.Handle() > 0)
        {
        aMessage.Complete(chunk);
        }
    else
        {
        // TODO: make sure error code is negative or zero
        aMessage.Complete(0);
        }

    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleEffectVolumeGetMaxVolL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleEffectVolumeGetMaxVolL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        guint vol;
        status = iCallAdpt->GetMaxVolume(vol);
        if (status == TMS_RESULT_SUCCESS)
            {
            TPckgBuf<guint> pckg(vol);
            aMessage.WriteL(0, pckg);
            }
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleEffectVolumeSetVolL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleEffectVolumeSetVolL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        guint vol;
        vol = aMessage.Int0();
        status = iCallAdpt->SetVolume(vol);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleEffectVolumeGetVolL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleEffectVolumeGetVolL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        guint vol;
        status = iCallAdpt->GetVolume(vol);
        if (status == TMS_RESULT_SUCCESS)
            {
            TPckgBuf<guint> pckg(vol);
            aMessage.WriteL(0, pckg);
            }
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleEffectVolumeGetMaxGainL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleEffectVolumeGetMaxGainL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        guint gain;
        status = iCallAdpt->GetMaxGain(gain);
        if (status == TMS_RESULT_SUCCESS)
            {
            TPckgBuf<guint> pckg(gain);
            aMessage.WriteL(0, pckg);
            }
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleEffectVolumeSetGainL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleEffectVolumeSetGainL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        guint gain;
        gain = aMessage.Int0();
        status = iCallAdpt->SetGain(gain);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleEffectVolumeGetGainL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleEffectVolumeGetGainL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        guint gain;
        status = iCallAdpt->GetGain(gain);
        if (status == TMS_RESULT_SUCCESS)
            {
            TPckgBuf<guint> pckg(gain);
            aMessage.WriteL(0, pckg);
            }
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleGlobalEffectVolumeGetMaxVolL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleGlobalEffectVolumeGetMaxVolL(
        const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        guint vol(0);
        status = iCallAdpt->GetGlobalMaxVolume(vol);
        if (status == TMS_RESULT_SUCCESS)
            {
            TPckgBuf<guint> pckg(vol);
            aMessage.WriteL(0, pckg);
            }
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleGlobalEffectVolumeSetVolL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleGlobalEffectVolumeSetVolL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);
    iGlobalVol = aMessage.Int0();
    if (iCallAdpt)
        {
        guint vol(0);
        vol = aMessage.Int0();
        status = iCallAdpt->SetGlobalVolume(vol);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleGlobalEffectVolumeGetVolL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleGlobalEffectVolumeGetVolL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        guint vol(0);
        status = iCallAdpt->GetGlobalVolume(vol);
        if (status == TMS_RESULT_SUCCESS)
            {
            TPckgBuf<guint> pckg(vol);
            aMessage.WriteL(0, pckg);
            }
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleGlobalEffectVolumeGetMaxGainL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleGlobalEffectVolumeGetMaxGainL(
        const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        guint gain;
        status = iCallAdpt->GetGlobalMaxGain(gain);
        if (status == TMS_RESULT_SUCCESS)
            {
            TPckgBuf<guint> pckg(gain);
            aMessage.WriteL(0, pckg);
            }
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleGlobalEffectVolumeSetGainL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleGlobalEffectVolumeSetGainL(
        const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_SUCCESS);
    iGlobalGain = aMessage.Int0();
    if (iCallAdpt)
        {
        guint gain;
        gain = aMessage.Int0();
        status = iCallAdpt->SetGlobalGain(gain);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleGlobalEffectVolumeGetGainL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleGlobalEffectVolumeGetGainL(
        const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        guint gain;
        status = iCallAdpt->GetGlobalGain(gain);
        if (status == TMS_RESULT_SUCCESS)
            {
            TPckgBuf<guint> pckg(gain);
            aMessage.WriteL(0, pckg);
            }
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleFormatGetCodecModeL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleFormatGetCodecModeL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);

    if (iCallAdpt)
        {
        gint mode;
        TMSFormatType fmttype;
        TMSStreamType strmtype;
        fmttype = aMessage.Int0();
        strmtype = aMessage.Int1();
        status = iCallAdpt->GetCodecMode(fmttype, strmtype, mode);
        TPckgBuf<gint> pckg(mode);
        aMessage.WriteL(2, pckg);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleFormatSetCodecModeL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleFormatSetCodecModeL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        gint mode;
        TMSFormatType fmttype;
        TMSStreamType strmtype;
        fmttype = aMessage.Int0();
        strmtype = aMessage.Int1();
        mode = aMessage.Int2();
        status = iCallAdpt->SetCodecMode(fmttype, strmtype, mode);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleFormatGetSupportedBitRatesCountL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleFormatGetSupportedBitRatesCountL(
        const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        guint count;
        status = iCallAdpt->GetSupportedBitRatesCount(count);
        TPckgBuf<guint> pckg(count);
        aMessage.WriteL(0, pckg);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleFormatGetSupportedBitRatesL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleFormatGetSupportedBitRatesL(
        const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        CBufFlat* brbuf = CBufFlat::NewL(KArrayExpandSize);
        CleanupStack::PushL(brbuf);
        status = iCallAdpt->GetSupportedBitRates(brbuf);
        aMessage.WriteL(0, brbuf->Ptr(0));
        CleanupStack::PopAndDestroy(brbuf);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleFormatGetBitRateL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleFormatGetBitRateL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        guint bitrate;
        status = iCallAdpt->GetBitRate(bitrate);
        TPckgBuf<guint> pckg(bitrate);
        aMessage.WriteL(0, pckg);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleFormatSetBitRateL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleFormatSetBitRateL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        guint bitrate;
        bitrate = aMessage.Int0();
        status = iCallAdpt->SetBitRate(bitrate);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleFormatGetVADL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleFormatGetVADL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        gboolean vad;
        TMSFormatType fmttype;
        fmttype = (TMSFormatType) aMessage.Int0();
        status = iCallAdpt->GetVAD(fmttype, vad);
        TPckgBuf<gboolean> pckg(vad);
        aMessage.WriteL(1, pckg);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleFormatSetVADL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleFormatSetVADL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        gboolean vad;
        TMSFormatType fmttype;
        fmttype = (TMSFormatType) aMessage.Int0();
        vad = (gboolean) aMessage.Int1();
        status = iCallAdpt->SetVAD(fmttype, vad);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleFormatGetCNGL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleFormatGetCNGL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        gboolean cng;
        TMSFormatType fmttype;
        fmttype = (TMSFormatType) aMessage.Int0();
        status = iCallAdpt->GetCNG(fmttype, cng);
        TPckgBuf<gboolean> pckg(cng);
        aMessage.WriteL(1, pckg);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleFormatSetCNGL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleFormatSetCNGL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        gboolean cng;
        TMSFormatType fmttype;
        fmttype = (TMSFormatType) aMessage.Int0();
        cng = (gboolean) aMessage.Int1();
        status = iCallAdpt->SetCNG(fmttype, cng);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleFormatGetPlcL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleFormatGetPlcL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        gboolean plc;
        TMSFormatType fmttype;
        fmttype = (TMSFormatType) aMessage.Int0();
        status = iCallAdpt->GetPlc(fmttype, plc);
        TPckgBuf<gboolean> pckg(plc);
        aMessage.WriteL(1, pckg);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleFormatSetPlcL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleFormatSetPlcL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        gboolean plc;
        TMSFormatType fmttype;
        fmttype = (TMSFormatType) aMessage.Int0();
        plc = (gboolean) aMessage.Int1();
        status = iCallAdpt->SetPlc(fmttype, plc);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSCallSession::HandleRoutingSetOutputL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleRoutingSetOutputL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        TMSAudioOutput outputtype;
        outputtype = (TMSAudioOutput) aMessage.Int0();
        status = iCallAdpt->SetOutput(outputtype);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;

    }
// -----------------------------------------------------------------------------
// TMSCallSession::HandleRoutingGetOutputL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleRoutingGetOutputL(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    TMSAudioOutput outputtype;
    if (iCallAdpt)
        {
        status = iCallAdpt->GetOutput(outputtype);
        }
    TPckgBuf<gint> p(outputtype);
    aMessage.Write(0, p);
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }
// -----------------------------------------------------------------------------
// TMSCallSession::HandleRoutingGetPreviousOutputL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleRoutingGetPreviousOutputL(
        const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    TMSAudioOutput outputtype;
    if (iCallAdpt)
        {
        status = iCallAdpt->GetPreviousOutput(outputtype);
        }
    TPckgBuf<gint> p(outputtype);
    aMessage.Write(0, p);
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }
// -----------------------------------------------------------------------------
// TMSCallSession::HandleRoutingGetAvailableOutputsL
//
// -----------------------------------------------------------------------------
//
void TMSCallSession::HandleRoutingGetAvailableOutputsL(
        const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    TPckgBuf<gint> countpkg;
    gint count;
    if (iCallAdpt)
        {
        CBufFlat* outputbuf = CBufFlat::NewL(10);
        CleanupStack::PushL(outputbuf);
        status = iCallAdpt->GetAvailableOutputsL(count, outputbuf);

        countpkg() = count;
        aMessage.WriteL(0, countpkg);
        aMessage.WriteL(1, outputbuf->Ptr(0));
        CleanupStack::PopAndDestroy(outputbuf);
        }
    aMessage.Complete(status);
    }

void TMSCallSession::HandleDTMFStart(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);

    if (iCallAdpt)
        {
        TMSStreamType strmtype;
        gint len = 0;
        strmtype = (TMSStreamType) aMessage.Int0();
        len = aMessage.GetDesLength(1);
        HBufC* tone(NULL);
        if (len > 0)
            {
            delete tone;
            tone = NULL;
            TRAP(status,tone = HBufC::NewL(len));
            if (status == KErrNone)
                {
                TPtr ptr = tone->Des();
                status = aMessage.Read(1, ptr);
                TRACE_PRN_N(ptr);
                if (status == KErrNone)
                    {
                    status = iCallAdpt->StartDTMF(strmtype, ptr);
                    }
                }
            delete tone;
            tone = NULL;
            }
        }

    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

void TMSCallSession::HandleDTMFStop(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        TMSStreamType streamtype;
        streamtype = (TMSStreamType) aMessage.Int0();
        status = iCallAdpt->StopDTMF(streamtype);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

void TMSCallSession::HandleDTMFContinue(const RMessage2& aMessage)
    {
    TRACE_PRN_FN_ENT;
    gint status(TMS_RESULT_DOES_NOT_EXIST);
    if (iCallAdpt)
        {
        TBool continuesending;
        continuesending = (TBool) aMessage.Int0();
        status = iCallAdpt->ContinueDTMF(continuesending);
        }
    aMessage.Complete(status);
    TRACE_PRN_FN_EXT;
    }

// End of file
