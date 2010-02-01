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
#include <AudioOutput.h>
#include <mmcccodecinformation.h>
#include <IlbcDecoderIntfc.h>
#include <G711DecoderIntfc.h>
#include <G729DecoderIntfc.h>
#include <ErrorConcealmentIntfc.h>
#include "tmsutility.h"
#include "ipcallstream.h"

using namespace TMS;

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::TMSVoIPDownlink
// Standard Constructor
// -----------------------------------------------------------------------------
//
TMSVoIPDownlink::TMSVoIPDownlink()
    {
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::~TMSVoIPDownlink
// Standard Constructor
// -----------------------------------------------------------------------------
//
TMSVoIPDownlink::~TMSVoIPDownlink()
    {
    TRACE_PRN_FN_ENT;

    Stop();

    delete iAudioOutput;
    delete iErrConcealmentIntfc;
    delete iG711DecoderIntfc;
    delete iG729DecoderIntfc;
    delete iIlbcDecoderIntfc;

    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::NewL
// Symbian two-phase constructor
// -----------------------------------------------------------------------------
//
TMSVoIPDownlink* TMSVoIPDownlink::NewL(const guint32 codecID,
        const TMMFPrioritySettings priority)
    {
    TMSVoIPDownlink* self = new (ELeave) TMSVoIPDownlink();
    CleanupStack::PushL(self);
    self->ConstructL(codecID, priority);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::ConstructL
// Part two of Symbian two phase construction
// -----------------------------------------------------------------------------
//
void TMSVoIPDownlink::ConstructL(const guint32 codecID,
        const TMMFPrioritySettings priority)
    {
    TRACE_PRN_FN_ENT;

    iCodecID = codecID;
    iPriority = priority;

    // Client must set these before querying!
    iG711DecodeMode = TMS_G711_CODEC_MODE_ALAW;
    iILBCDecodeMode = TMS_ILBC_CODEC_MODE_20MS_FRAME;

    TRAPD(err, InitDevSoundL(EMMFStatePlaying, priority));
    if (err != TMS_RESULT_SUCCESS)
        {
        User::Leave(err);
        }

    iMaxBufLen = ConfigureMedia(iCodecID);

    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::Start
//
// -----------------------------------------------------------------------------
//
void TMSVoIPDownlink::Start()
    {
    TRACE_PRN_FN_ENT;

    gint err = TMS_RESULT_ILLEGAL_OPERATION;

    if (iStatus == EReady && iDevSound)
        {
        TRAP(err, iDevSound->PlayInitL());
        TRACE_PRN_IF_ERR(err);

#ifdef _DEBUG
        iSamplesPlayedCount = 0;
#endif
        if (err != TMS_RESULT_SUCCESS)
            {
            iStatus = EReady;
            }
        }

    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::Stop
//
// -----------------------------------------------------------------------------
//
void TMSVoIPDownlink::Stop()
    {
    TRACE_PRN_FN_ENT;

    if (iStatus == EStreaming && iDevSound)
        {
        iDevSound->Stop();
        iStatus = EReady;
        }

    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::BufferToBeFilled
// A reference to the buffer delivered from the DevSound is stored locally
// for later use. It will be filled with the data passed from the client
// when it calls BufferFilled.
//
// -----------------------------------------------------------------------------
//
void TMSVoIPDownlink::BufferToBeFilled(CMMFBuffer* aBuffer)
    {
    // Store pointer to the received buffer
    iDevSoundBufPtr = static_cast<CMMFDataBuffer*> (aBuffer);
    iBufLen = iDevSoundBufPtr->RequestSize();
    TRACE_PRN_N1(_L("TMS->DNL->BTBF: LEN[%d]"), iBufLen);

#ifndef __WINSCW__
    //TODO: revisit this!
    // The first AMR buffer returns 1 for no data frame.
    /*if (iCodecID == KMccFourCCIdAMRNB)
     {
     iBufLen = iMaxBufLen;
     }*/
#endif //__WINSCW__
    // Create or adjust the chunk
    gint err = DoChunk(iBufLen, iMsgBuffer);

    if (err != TMS_RESULT_SUCCESS)
        {
        Stop();
        iMsgBuffer.iStatus = err;
        }
    else
        {
        // Notify client there is buffer ready to be filled
        iMsgBuffer.iStatus = err;
        iMsgBuffer.iInt = iBufLen;
        iStatus = EStreaming;
        // If chunk is opened, we will expect a call from the client to
        // get chunk handle. When we get a call to copy chunk handle,
        // check these variables and see if they match. This is not
        // completely secure, but will provide some level of security
        if (iMsgBuffer.iBool == TRUE)
            {
            iWriteDataXferHndlToClient = TRUE;
            iKey = iMsgBuffer.iUint32;
            }
        }

    iMsgBuffer.iRequest = ECmdFillBuffer;
    err = iMsgQueue.Send(iMsgBuffer);

    TRACE_PRN_IF_ERR(err);
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::BufferFilled
//
// -----------------------------------------------------------------------------
//
void TMSVoIPDownlink::BufferFilled(const guint buflen)
    {
    TRACE_PRN_N1(_L("TMS->DNL->BF: LEN[%d]"), buflen);

    // Copy data over from chunk
    TPtr8 dataPtr(iChunk.Base(), buflen, iMaxBufLen);
    //    RDebug::RawPrint(dataPtr);

    if (iStatus == EStreaming && iDevSound && iDevSoundBufPtr)
        {
        // Fill D/S buffer and send it for playback
        iDevSoundBufPtr->Data() = dataPtr;
        TRAPD(ignore, iDevSoundBufPtr->SetRequestSizeL(buflen));
        if (ignore) //makes compiler happy
            {
            //try playing anyway
            iDevSound->PlayData();
            }
        else
            {
            iDevSound->PlayData();
            }
        }
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::SetCodecCi
//
// -----------------------------------------------------------------------------
//
gint TMSVoIPDownlink::SetCodecCi()
    {
    TRAPD(err, SetCodecCiL());
    return err;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::SetCodecCiL
//
// -----------------------------------------------------------------------------
//
void TMSVoIPDownlink::SetCodecCiL()
    {
    TRACE_PRN_FN_ENT;

    switch (iCodecID)
        {
        case KMccFourCCIdG711:
            {
            if (!iG711DecoderIntfc)
                {
                iG711DecoderIntfc = CG711DecoderIntfc::NewL(*iDevSound);
                }
            break;
            }
        case KMccFourCCIdG729:
            {
            if (!iG729DecoderIntfc)
                {
                iG729DecoderIntfc = CG729DecoderIntfc::NewL(*iDevSound);
                }
            break;
            }
        case KMccFourCCIdILBC:
            {
            if (!iIlbcDecoderIntfc)
                {
                iIlbcDecoderIntfc = CIlbcDecoderIntfc::NewL(*iDevSound);
                }
            break;
            }
        case KMccFourCCIdAMRNB:
        case KMMFFourCCCodePCM16:
            {
            break;
            }
        default:
            {
            User::Leave(TMS_RESULT_INVALID_ARGUMENT);
            break;
            }
        }

    if (!iErrConcealmentIntfc && iCodecID != KMMFFourCCCodePCM16)
        {
        iErrConcealmentIntfc = CErrorConcealmentIntfc::NewL(*iDevSound);
        }

    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::SetVolume
//
// -----------------------------------------------------------------------------
//
gint TMSVoIPDownlink::SetVolume(const guint volume)
    {
    gint ret(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (iDevSound)
        {
        iDevSound->SetVolume(volume);
        ret = TMS_RESULT_SUCCESS;
        }
    TRACE_PRN_N1(_L("TMS->DNL: SetVolume [%d]"), volume);
    return ret;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::GetVolume
//
// -----------------------------------------------------------------------------
//
gint TMSVoIPDownlink::GetVolume(guint& volume)
    {
    gint ret(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (iDevSound)
        {
        volume = iDevSound->Volume();
        ret = TMS_RESULT_SUCCESS;
        }
    TRACE_PRN_N1(_L("TMS->DNL: GetVolume [%d]"), volume);
    return ret;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::GetMaxVolume
//
// -----------------------------------------------------------------------------
//
gint TMSVoIPDownlink::GetMaxVolume(guint& volume)
    {
    gint ret(TMS_RESULT_UNINITIALIZED_OBJECT);
    if (iDevSound)
        {
        volume = iDevSound->MaxVolume();
        ret = TMS_RESULT_SUCCESS;
        }
    TRACE_PRN_N1(_L("TMS->DNL: MaxVolume [%d]"), volume);
    return ret;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::GetDataXferChunkHndl
//
// -----------------------------------------------------------------------------
//
gint TMSVoIPDownlink::GetDataXferChunkHndl(const TUint32 key, RChunk& chunk)
    {
    gint status = TMS_RESULT_DOES_NOT_EXIST;

    if (iChunk.Handle())
        {
        if (iWriteDataXferHndlToClient && (iKey == key))
            {
            chunk = iChunk;
            iWriteDataXferHndlToClient = FALSE;
            iKey = 0;
            status = TMS_RESULT_SUCCESS;
            }
        else
            {
            status = TMS_RESULT_ILLEGAL_OPERATION;
            }
        }

    TRACE_PRN_IF_ERR(status);
    return status;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::SetIlbcCodecMode
//
// -----------------------------------------------------------------------------
//
gint TMSVoIPDownlink::SetIlbcCodecMode(gint mode)
    {
    gint err = TMS_RESULT_DOES_NOT_EXIST;

    if (iStatus == EReady)
        {
        iILBCDecodeMode = mode;

        if (iIlbcDecoderIntfc && iCodecID == KMccFourCCIdILBC)
            {
            if (mode == TMS_ILBC_CODEC_MODE_20MS_FRAME)
                {
                err = iIlbcDecoderIntfc->SetDecoderMode(
                        CIlbcDecoderIntfc::E20msFrame);
                TRACE_PRN_N(_L("TMS->DNL: iLBC Mode Set [20ms]"));
                }
            else if (mode == TMS_ILBC_CODEC_MODE_30MS_FRAME)
                {
                err = iIlbcDecoderIntfc->SetDecoderMode(
                        CIlbcDecoderIntfc::E30msFrame);
                TRACE_PRN_N(_L("TMS->DNL: iLBC Mode Set [30ms]"));
                }
            }
        }

    TRACE_PRN_IF_ERR(err);
    return err;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::GetIlbcCodecMode
//
// -----------------------------------------------------------------------------
//
gint TMSVoIPDownlink::GetIlbcCodecMode(gint& mode)
    {
    // not available through CIs -> return cashed value
    mode = iILBCDecodeMode;
    TRACE_PRN_N1(_L("TMS->DNL: GetIlbcCodecMode [%d]"), mode);
    return TMS_RESULT_SUCCESS;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::SetG711CodecMode
//
// -----------------------------------------------------------------------------
//
gint TMSVoIPDownlink::SetG711CodecMode(gint mode)
    {
    gint err = TMS_RESULT_DOES_NOT_EXIST;

    if (iStatus == EReady)
        {
        iG711DecodeMode = mode;

        if (iG711DecoderIntfc && iCodecID == KMccFourCCIdG711)
            {
            if (mode == TMS_G711_CODEC_MODE_ALAW)
                {
                err = iG711DecoderIntfc->SetDecoderMode(
                        CG711DecoderIntfc::EDecALaw);
                TRACE_PRN_N(_L("TMS->DNL: G711 Mode Set [ALaw]"));
                }
            else if (mode == TMS_G711_CODEC_MODE_MULAW)
                {
                err = iG711DecoderIntfc->SetDecoderMode(
                        CG711DecoderIntfc::EDecULaw);
                TRACE_PRN_N(_L("TMS->DNL: G711 Mode Set [uLaw]"));
                }
            }
        }

    TRACE_PRN_IF_ERR(err);
    return err;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::GetG711CodecMode
//
// -----------------------------------------------------------------------------
//
gint TMSVoIPDownlink::GetG711CodecMode(gint& mode)
    {
    // not available through CIs -> return cached value
    mode = iG711DecodeMode;
    TRACE_PRN_N1(_L("TMS->DNL: GetG711CodecMode [%d]"), mode);
    return TMS_RESULT_SUCCESS;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::FrameModeRqrdForEC
//
// -----------------------------------------------------------------------------
//
gint TMSVoIPDownlink::FrameModeRqrdForEC(gboolean& frmodereq)
    {
    gint err = TMS_RESULT_DOES_NOT_EXIST;

    if (iStatus == EReady)
        {
        if (iErrConcealmentIntfc)
            {
            err = iErrConcealmentIntfc->FrameModeRqrdForEC(frmodereq);
            TRACE_PRN_N1(_L("TMS->DNL: FrameModeRqrdForEC [%d]"), frmodereq);
            }
        }

    TRACE_PRN_IF_ERR(err);
    return err;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::SetFrameMode
//
// -----------------------------------------------------------------------------
//
gint TMSVoIPDownlink::SetFrameMode(const gboolean frmode)
    {
    gint err = TMS_RESULT_DOES_NOT_EXIST;

    if (iStatus == EReady)
        {
        iFrameMode = frmode;

        if (iErrConcealmentIntfc)
            {
            err = iErrConcealmentIntfc->SetFrameMode(frmode);
            TRACE_PRN_N1(_L("TMS->DNL: SetFrameMode [%d]"), frmode);
            }
        }

    TRACE_PRN_IF_ERR(err);
    return err;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::GetFrameMode
//
// -----------------------------------------------------------------------------
//
gint TMSVoIPDownlink::GetFrameMode(gboolean& frmode)
    {
    gint err = TMS_RESULT_DOES_NOT_EXIST;

    if (iErrConcealmentIntfc)
        {
        // not available through CIs -> return cached value
        frmode = iFrameMode;
        TRACE_PRN_N1(_L("TMS->DNL: GetFrameMode [%d]"), frmode);
        err = TMS_RESULT_SUCCESS;
        }

    TRACE_PRN_IF_ERR(err);
    return err;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::ConcealErrorForNextBuffer
//
// -----------------------------------------------------------------------------
//
gint TMSVoIPDownlink::ConcealErrorForNextBuffer()
    {
    gint err = TMS_RESULT_DOES_NOT_EXIST;

    if (iErrConcealmentIntfc)
        {
        err = iErrConcealmentIntfc->ConcealErrorForNextBuffer();
        TRACE_PRN_N(_L("TMS->DNL: ConcealErrorForNextBuffer"));
        }

    TRACE_PRN_IF_ERR(err);
    return err;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::SetCng
//
// -----------------------------------------------------------------------------
//
gint TMSVoIPDownlink::SetCng(const TMSFormatType fmttype, const gboolean cng)
    {
    gint err = TMS_RESULT_DOES_NOT_EXIST;

    if (iStatus == EReady)
        {
        if (fmttype == TMS_FORMAT_G711 && iG711DecoderIntfc)
            {
            err = iG711DecoderIntfc->SetCng(cng);
            TRACE_PRN_N1(_L("TMS->DNL: SetCng [%d]"), cng);
            }
        else if (fmttype == TMS_FORMAT_ILBC && iIlbcDecoderIntfc)
            {
            err = iIlbcDecoderIntfc->SetCng(cng);
            TRACE_PRN_N1(_L("TMS->DNL: SetCng [%d]"), cng);
            }
        }

    TRACE_PRN_IF_ERR(err);
    return err;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::GetCng
//
// -----------------------------------------------------------------------------
//
gint TMSVoIPDownlink::GetCng(const TMSFormatType fmttype, gboolean& cng)
    {
    gint err = TMS_RESULT_DOES_NOT_EXIST;

    if (iStatus == EReady)
        {
        if (fmttype == TMS_FORMAT_G711 && iG711DecoderIntfc)
            {
            err = iG711DecoderIntfc->GetCng(cng);
            TRACE_PRN_N1(_L("TMS->DNL: GetCng [%d]"), cng);
            }
        else if (fmttype == TMS_FORMAT_ILBC && iIlbcDecoderIntfc)
            {
            err = iIlbcDecoderIntfc->GetCng(cng);
            TRACE_PRN_N1(_L("TMS->DNL: GetCng [%d]"), cng);
            }
        }

    TRACE_PRN_IF_ERR(err);
    return err;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::SetPlc
//
// -----------------------------------------------------------------------------
//
gint TMSVoIPDownlink::SetPlc(const TMSFormatType fmttype, const gboolean plc)
    {
    gint err = TMS_RESULT_DOES_NOT_EXIST;

    if (iStatus == EReady)
        {
        if (fmttype == TMS_FORMAT_G711 && iG711DecoderIntfc)
            {
            iPlc = plc;
            err = iG711DecoderIntfc->SetPlc(iPlc);
            TRACE_PRN_N1(_L("TMS->DNL: SetPlc [%d]"), plc);
            }
        }

    TRACE_PRN_IF_ERR(err);
    return err;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::GetPlc
//
// -----------------------------------------------------------------------------
//
gint TMSVoIPDownlink::GetPlc(const TMSFormatType fmttype, gboolean& plc)
    {
    gint err = TMS_RESULT_DOES_NOT_EXIST;

    if (fmttype == TMS_FORMAT_G711 && iG711DecoderIntfc)
        {
        // not available through CIs -> return cached value
        plc = iPlc;
        err = TMS_RESULT_SUCCESS;
        TRACE_PRN_N1(_L("TMS->DNL: GetPlc [%d]"), plc);
        }

    TRACE_PRN_IF_ERR(err);
    return err;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::BadLsfNextBuffer
//
// -----------------------------------------------------------------------------
//
gint TMSVoIPDownlink::BadLsfNextBuffer()
    {
    gint err = TMS_RESULT_DOES_NOT_EXIST;

    if (iStatus == EStreaming)
        {
        if (iCodecID == KMccFourCCIdG729 && iG729DecoderIntfc)
            {
            err = iG729DecoderIntfc->BadLsfNextBuffer();
            TRACE_PRN_N(_L("TMS->DNL: BadLsfNextBuffer"));
            }
        }

    TRACE_PRN_IF_ERR(err);
    return err;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::SetAudioDeviceL
//
// -----------------------------------------------------------------------------
void TMSVoIPDownlink::SetAudioDeviceL(TMSAudioOutput output)
    {
    TRACE_PRN_FN_ENT;

    if (!iAudioOutput)
        {
        iAudioOutput = CAudioOutput::NewL(*iDevSound);
        }

    if (iAudioOutput)
        {
        // ENoPreference=0, EAll=1, ENoOutput=2, EPrivate=3, EPublic=4
        CAudioOutput::TAudioOutputPreference outputDev;

        switch (output)
            {
            case TMS_AUDIO_OUTPUT_PRIVATE:
            case TMS_AUDIO_OUTPUT_HANDSET:
                outputDev = CAudioOutput::EPrivate;
                break;
            case TMS_AUDIO_OUTPUT_PUBLIC:
            case TMS_AUDIO_OUTPUT_LOUDSPEAKER:
                outputDev = CAudioOutput::EPublic;
                break;
            default: // Use default device routing
                outputDev = CAudioOutput::ENoPreference;
                break;
            }

        iAudioOutput->SetAudioOutputL(outputDev);
        TRACE_PRN_N1(_L("TMS->DNL: SetAudioDeviceL [%d]"), outputDev);
        }

    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::GetAudioDeviceL
//
// -----------------------------------------------------------------------------
//
void TMSVoIPDownlink::GetAudioDeviceL(TMSAudioOutput& output)
    {
    TRACE_PRN_FN_ENT;

    if (!iAudioOutput)
        {
        iAudioOutput = CAudioOutput::NewL(*iDevSound);
        }

    if (iAudioOutput)
        {
        CAudioOutput::TAudioOutputPreference outputDev =
                iAudioOutput->AudioOutput();

        switch (outputDev)
            {
            case CAudioOutput::ENoPreference:
            case CAudioOutput::EAll:
            case CAudioOutput::ENoOutput:
            case CAudioOutput::EPrivate:
                output = TMS_AUDIO_OUTPUT_PRIVATE;
                break;
            case CAudioOutput::EPublic:
                output = TMS_AUDIO_OUTPUT_PUBLIC;
                break;
            default:
                output = TMS_AUDIO_OUTPUT_PRIVATE;
                break;
            }
        }

    TRACE_PRN_FN_EXT;
    }

// CALLBACKS

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::InitializeComplete
// A callback from the DevSound indicating completion of the initialization.
// It will send config data to the D/S and configure the encoder via CI.
// If everything goes well, the state of the thread is set EReady.
// The initialization completion message is sent back to the main thread.
// -----------------------------------------------------------------------------
//
void TMSVoIPDownlink::InitializeComplete(TInt aError)
    {
    TRACE_PRN_FN_ENT;

    gint err = aError;

    if (err == TMS_RESULT_SUCCESS && iDevSound)
        {
        TMMFCapabilities conf;
        conf = iDevSound->Config();
        conf.iRate = EMMFSampleRate8000Hz;
        conf.iChannels = EMMFMono;
        TRAP(err, iDevSound->SetConfigL(conf));
        if (err == TMS_RESULT_SUCCESS)
            {
            // We are ready to stream even in case of later CI setting failure
            iStatus = EReady;
            iMaxVolume = iDevSound->MaxVolume();
            }

        // Init Custom Interface API to the decoder
        err = SetCodecCi();
        if (err != TMS_RESULT_SUCCESS)
            {
            // DEBUG only
            // Can ignore error - although decoder is not fully configured but
            // it can still run in the default mode.
            TRACE_PRN_IF_ERR(err);
            }
        }

    // TODO: Notify client

    TRACE_PRN_IF_ERR(err);
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// TMSVoIPDownlink::PlayError
// From MDevSoundObserver
// Record error is send to client over comm channel.
// The state of recorder is rolled back to EReady.
// -----------------------------------------------------------------------------
//
void TMSVoIPDownlink::PlayError(TInt /*aError*/)
    {
    //TRACE_PRN_IF_ERR(aError);

#ifdef _DEBUG
    iSamplesPlayedCount = 0;
#endif
    iStatus = EReady;

    // TODO: Notify client
    }

// End of file
