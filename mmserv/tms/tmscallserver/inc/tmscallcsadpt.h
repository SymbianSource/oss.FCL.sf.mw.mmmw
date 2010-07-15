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

// INCLUDES
#include <TelephonyAudioRouting.h>
#include <MTelephonyAudioRoutingObserver.h>
#include <e32msgqueue.h>
#include "tmsclientserver.h"
#include "tmscalladpt.h"
#include "tmscsdevsoundobserver.h"

namespace TMS {

// FORWARD DECLARATIONS
class TMSCSUplink;
class TMSCSDownlink;
class TMSTarSettings;

/*
 * TMSCallCSAdpt class
 */
class TMSCallCSAdpt : public TMSCallAdpt,
                      public TMSCSDevSoundObserver,
                      public MTelephonyAudioRoutingObserver
    {
public:
	static TMSCallCSAdpt* NewL();
    virtual ~TMSCallCSAdpt();
    virtual gint PostConstruct();

    // From TMSStream
    virtual gint CreateStream(const TMSCallType callType,
            const TMSStreamType strmType, gint& outStrmId);
    virtual gint InitStream(const TMSCallType callType,
            const TMSStreamType strmType, const gint strmId,
            const TMSFormatType frmtType, const RMessage2& message);
    virtual gint StartStream(const TMSCallType callType,
            const TMSStreamType strmType, const gint strmId);
    virtual gint PauseStream(const TMSCallType callType,
            const TMSStreamType strmType, const gint strmId);
    virtual gint StopStream(const TMSCallType callType,
            const TMSStreamType strmType, const gint strmId);
    virtual gint DeinitStream(const TMSCallType callType,
            const TMSStreamType strmType, const gint strmId);
    virtual gint DeleteStream(const TMSCallType callType,
            const TMSStreamType strmType, const gint strmId);
    virtual gint DataXferBufferEmptied(const TMSCallType callType,
            const TMSStreamType strmType, const gint strmId);
    virtual gint DataXferBufferFilled(const TMSCallType callType,
            const TMSStreamType strmType, const gint strmId,
            const guint datasize);
    virtual gint GetDataXferBufferHndl(const TMSCallType callType,
            const TMSStreamType strmType, const gint strmId,
            const guint32 key, RChunk& chunk);

    // From TMS effects
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

    // From TMS formats
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

    // From TMS audio routing
    virtual gint SetOutput(const TMSAudioOutput output);
    virtual gint GetOutput(TMSAudioOutput& output);
    virtual gint GetPreviousOutput(TMSAudioOutput& output);
    virtual gint GetAvailableOutputsL(gint& count, CBufFlat*& outputsbuf);

    //From TMSCSDevSoundObserver
    void DownlinkInitCompleted(gint status);
    void UplinkInitCompleted(gint status);
    void DownlinkActivationCompleted(gint status);
    void UplinkActivationCompleted(gint status);

private:
    TMSCallCSAdpt();
    void ConstructL();

    gint InitUplink();
    gint InitDownlink();
    void AvailableOutputsChanged(
            CTelephonyAudioRouting& aTelephonyAudioRouting);
    void OutputChanged(CTelephonyAudioRouting& aTelephonyAudioRouting);
    void SetOutputComplete(CTelephonyAudioRouting& aTelephonyAudioRouting,
            gint status);
    void GetSupportedBitRatesL(CBufFlat*& brbuffer);
    void NotifyClient(const gint strmId, const gint command,
            const gint status = KErrNone, const gint64 int64 = TInt64(0));

private:
    gint iNextStreamId;
    TMSCSUplink* iCSUplink;
    TMSCSDownlink* iCSDownlink;
    CTelephonyAudioRouting* iRouting;
    TMSTarSettings* iTarSettings;

    // Message queues for communication and data transfer back to the client
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
