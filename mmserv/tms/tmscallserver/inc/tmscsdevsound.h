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

#ifndef TMSCSDEVSOUND_H
#define TMSCSDEVSOUND_H

#include <sounddevice.h>
#include <tms.h>

namespace TMS {

// FORWARD DECLARATIONS
class TMSCSDevSoundObserver;

/**
 *  Wrapper for CMMFDevSound
 *
 */
NONSHARABLE_CLASS(TMSCSDevSound) : public CBase,
                                   public MDevSoundObserver
    {
public:

    /**
     * Destructor.
     */
    virtual ~TMSCSDevSound();

    /**
     * Activates the DevSound stream.
     * Stream activation status is indicated by TMSCSDevSoundObserver callback
     * methods. If the stream is already active or being activated, call to
     * this will result in no action.
     */
    virtual void Activate();

    /**
     * Deactivates an active DevSound stream.
     */
    virtual void Deactivate();

    /*
     * Returns DevSound instance associated with the stream.
     */
    CMMFDevSound& DevSound();

    /**
     * From MDevSoundObserver
     * Indicates DevSound initialization status.
     */
    void InitializeComplete(TInt aError);

    /**
     * From MDevSoundObserver
     * Empty implementation
     */
    void BufferToBeFilled(CMMFBuffer* /*aBuffer*/) {}

    /**
     * From MDevSoundObserver
     * Empty implementation
     */
    void PlayError(TInt /*aError*/) {}

    /**
     * From MDevSoundObserver
     * Empty implementation
     */
    void ToneFinished(TInt /*aError*/) {}

    /**
     * From MDevSoundObserver
     * Empty implementation
     */
    void BufferToBeEmptied(CMMFBuffer* /*aBuffer*/) {}

    /**
     * From MDevSoundObserver
     * Empty implementation
     */
    void RecordError(TInt /*aError*/) {}

    /**
     * From MDevSoundObserver
     * Empty implementation
     */
    void ConvertError(TInt /*aError*/) {}

    /**
     * From MDevSoundObserver
     * Empty implementation
     */
    void DeviceMessage(TUid /*aMessageType*/, const TDesC8& /*aMsg*/) {}

protected:

    TMSCSDevSound(TMSCSDevSoundObserver& observer);
    void ConstructL(const TMSStreamType strmtype);
    void InitializeL();
    void NotifyEvent(gint error);

private:

    /**
     * Derived class has to activate the stream here.
     */
    virtual void DoActivateL() = 0;

protected:

    /**
     * Indication of DevSound activated state. TRUE == stream activated.
     * Derived class has to update this.
     */
    gboolean iActive;

    /**
     * Indication of an ongoing DevSound activation.
     * Derived class has to update this.
     */
    gboolean iActivationOngoing;

    /**
     * Devsound instance associated with the stream.
     */
    CMMFDevSound* iDevSound;

    /**
     * An observer subscribing to TMSCSDevSound notifications.
     */
    TMSCSDevSoundObserver& iObserver;

    TMSStreamType iStreamType;
    gint iPreference;
    gint iPriority;
    TMMFState iMode;
    };

} //namespace TMS

#endif //TMSCSDEVSOUND_H
