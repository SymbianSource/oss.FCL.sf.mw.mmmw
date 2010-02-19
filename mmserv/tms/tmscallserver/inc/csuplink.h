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

#ifndef CS_UPLINK_H
#define CS_UPLINK_H

#include <glib.h>
#include "cspdevsound.h"

namespace TMS {

/**
 *  Provides uplink functionality.
 *
 */
NONSHARABLE_CLASS(TMSCSUplink) : public TMSCSPDevSound
    {
public:

    static TMSCSUplink* NewL(TMSCSPDevSoundObserver& aObserver);

    virtual ~TMSCSUplink();

    /**
     * Determines mic mute state.
     *
     * @return ETrue - mic is muted, EFalse - mic is not muted.
     */
    TBool IsMuted();

    /**
     * Set mic muted.
     */
    void SetMuted();

    /**
     * Set mic unmuted.
     */
    void SetUnmuted();

    /**
     * Set mic gain.
     */
    void SetGain(gint aGain);

    /**
     * Get mic muted.
     */
    gint Gain();

    /**
     * Get max gain.
     */
    gint MaxGain();

    // from base class MDevSoundObserver

    /**
     * From MDevSoundObserver
     * Notification from Devsound that stream (mic) is
     * activated successfully.
     */
    void BufferToBeEmptied(CMMFBuffer* aBuffer);

    /**
     * From MDevSoundObserver
     * Notification from devsound that downstream(mic) activation
     * feiled.
     */
    void RecordError(TInt aError);

private:

    // from base class MCSPDevSound

    /**
     * From MCSPDevSound.
     * Tries to activate the mic stream.
     */
    void DoActivateL();

protected:

    TMSCSUplink(TMSCSPDevSoundObserver& aObserver);
    void ConstructL();
    };

} //namespace TMS

#endif // CSUplink_H
