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

#ifndef DTMFTONEPLAYEROBSERVER_H
#define DTMFTONEPLAYEROBSERVER_H

namespace TMS {

/**
 *  Observer interface for knowing when dtmf tone player has been initialized.
 */
class TMSDTMFTonePlayerObserver
    {
public:
    virtual void DTMFInitCompleted(TInt error) = 0;
    virtual void DTMFToneFinished(TInt error) = 0;
    };
} //namespace TMS

#endif // DTMFTONEPLAYEROBSERVER_H

