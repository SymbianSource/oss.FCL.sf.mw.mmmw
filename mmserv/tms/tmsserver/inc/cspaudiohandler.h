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
 * Description:  Declarations for class TMSCenRepAudioHandler
 *
 */

#ifndef CSPAUDIOHANDLER_H
#define CSPAUDIOHANDLER_H

#include <sounddevice.h>

#include "mcspcenrepobserver.h"
#include "tmsserver.h"

namespace TMS {

class TMSCSPPubSubListener;
class TMSCenRepListener;
class TMSCSPAudioStreams;

/**
 * Handles call adding from calls not done by the plugin.
 *
 */
class TMSCenRepAudioHandler : public MCSPCenRepObserver
    {
public:
    //Constructors and descructor

    /**
     * Two-phased constructing for the monitor.
     *
     * @param aObserver the observer for getting notification
     * @param aLine the line to monitor
     * @param aLineId line identifier
     */
    static TMSCenRepAudioHandler* NewL(TMSServer* aServer);

    /**
     * C++ default destructor
     */
    virtual ~TMSCenRepAudioHandler();

    void SetLoudSpeakerVol(TInt vol);
    void SetEarPieceVol(TInt vol);

    // from base class MCSPPubSubObserver
    /**
     * Handler for changed event.
     * @param aUid uid of setting
     * @param aKey id of setting
     * @param aStatus status of completed AO operation
     */
    //virtual void HandleNotifyPSL( const TUid aUid, const TInt& aKey,
    //    const TRequestStatus& aStatus );

    // from base class MCSPCenRepObserver
    /**
     * Handler for changed event.
     * @param aUid uid of setting
     * @param aVal value
     */
    virtual void TMSCenRepAudioHandler::HandleNotifyCenRepL(const TUid aUid,
            const TUint32 aKey, TInt aVal);

    // from base class MDevSoundObserver
protected:
    //From DevSound


protected:
    // From CActive
    /**
     * From CActive
     * RunL
     */
    void RunL();

    /**
     * From CActive
     * Catches errors if RunL leaves
     * @param aError error code
     * @return error code
     */
    TInt RunError(TInt aError);

    /**
     * From CActive
     * Cancels the monitor
     */
    void DoCancel();

private:
    /**
     * C++ default constructor
     * @param aObserver the observer for status change (incoming call)
     * @param aLine the line associated with the call
     * @param aLineId line identifier
     */
    TMSCenRepAudioHandler(TMSServer* aServer);

    /**
     * Constructs the monitor in the second phase.
     */
    void ConstructL();

private:
    // data

    /**
     * Mute listening from Publish&Subscribe.
     */
    TMSCSPPubSubListener* iMuteListener;

    /**
     * Incall loudspeaker listening from Central Repository.
     */
    TMSCenRepListener* iIncallLoudspeakerVolumeListener;

    /**
     * Incall ear volume listening from Central Repository.
     */
    TMSCenRepListener* iIncallEarVolumeListener;

    /**
     * Audio streams handler.
     */
    //CSPAudioStreams* iAudioStreams;

    /**
     * Call count
     */
    TInt iCallCount;
    TMSServer* iTMSSer;
    };

} //namespace TMS

#endif // CSPAUDIOHANDLER_H
