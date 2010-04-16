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

#ifndef MCSPDEVSOUNDOBSERVER_H
#define MCSPDEVSOUNDOBSERVER_H

namespace TMS {

/**
 *  Observer interface for knowing when audio streams have been
 *  successfully ramped up.
 *
 */
class TMSCSPDevSoundObserver
    {
public:
    virtual void DownlinkInitCompleted(TInt x) = 0;
    virtual void UplinkInitCompleted(TInt x) = 0;

    /**
     * Notification that Mic has been activated successfully
     *
     */
    virtual void UplinkActivatedSuccessfully() = 0;

    /**
     * Notification that speaker has been activated successfully
     *
     */
    virtual void DownlinkActivatedSuccessfully() = 0;

    /**
     * Notification that Mic initialisation failed.
     *
     */
    virtual void UplinkActivationFailed() = 0;

    /**
     * Notification that Speaker activation failed.
     *
     */
    virtual void DownlinkActivationFailed() = 0;
    };

} //namespace TMS

#endif // MCSPDEVSOUNDOBSERVER_H
