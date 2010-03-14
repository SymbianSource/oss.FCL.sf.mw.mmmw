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

#ifndef CALLCSADPT_H
#define CALLCSADPT_H

#include <TelephonyAudioRouting.h>
#include <MTelephonyAudioRoutingObserver.h>
#include <e32msgqueue.h>
#include "tmsclientserver.h"
#include "calladpt.h"
#include "mcspdevsoundobserver.h"

namespace TMS {

// FORWARD DECLARATIONS
class TMSCSUplink;
class TMSCSDownlink;
class TMSTarSettings;

/*
 * CallCSAdapt class
 */
class TMSCallCSAdpt : public TMSCallAdpt,
                      public TMSCSPDevSoundObserver,
                      public MTelephonyAudioRoutingObserver
    {
public:
    TMSCallCSAdpt();
    virtual ~TMSCallCSAdpt();
    virtual gint PostConstruct();

    virtual gint CreateStream(TMSCallType callType, TMSStreamType strmType,
            gint& outStrmId);
    virtual gint InitStreamL(TMSCallType callType, TMSStreamType strmType,
            gint strmId, TMSFormatType frmtType, const RMessage2& aMessage);
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

    virtual gint SetOutput(TMSAudioOutput output);
    virtual gint GetOutput(TMSAudioOutput& output);
    virtual gint GetPreviousOutput(TMSAudioOutput& output);
    virtual gint GetAvailableOutputsL(gint& count, CBufFlat*& outputsbuffer);

    void NotifyClient(const gint strmId, const gint aCommand,
            const gint aStatus = KErrNone, const gint64 aInt64 = TInt64(0));

    //From TMSCSPDevSoundObserver
    void DownlinkInitCompleted(TInt status);
    void UplinkInitCompleted(TInt status);
    void UplinkActivatedSuccessfully();
    void DownlinkActivatedSuccessfully();
    void UplinkActivationFailed();
    void DownlinkActivationFailed();

protected:
    void AvailableOutputsChanged(
            CTelephonyAudioRouting& aTelephonyAudioRouting);
    void OutputChanged(CTelephonyAudioRouting& aTelephonyAudioRouting);
    void SetOutputComplete(CTelephonyAudioRouting& aTelephonyAudioRouting,
            gint aError);
    void GetSupportedBitRatesL(CBufFlat*& brbuffer);

private:
    gint iNextStreamId;

    TMSCSUplink* iCSUplink;
    TMSCSDownlink* iCSDownlink;
    CTelephonyAudioRouting* iRouting;
    TMSTarSettings* iTarSettings;
    TMSStreamType iStrmtype;

    RMsgQueue<TmsMsgBuf> iMsgQueueUp;
    RMsgQueue<TmsMsgBuf> iMsgQueueDn;
    TmsMsgBuf iMsgBuffer;

    gboolean iUplinkInitialized;
    gint iUplinkStreamId;
    gboolean iDnlinkInitialized;
    gint iDnlinkStreamId;
    };

} //namespace TMS

#endif // CALLCSADPT_H

// End of file
