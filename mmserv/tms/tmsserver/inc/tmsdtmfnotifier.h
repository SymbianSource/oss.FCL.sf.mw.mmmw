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

#ifndef DTMFNOTIFIER_H
#define DTMFNOTIFIER_H

//  INCLUDES
#include <e32property.h>
#include <sounddevice.h>
#include "tmsclientserver.h"

namespace TMS {

// CLASS DECLARATION
class TMSDtmfNotifier
    {
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static TMSDtmfNotifier* NewL();

    /**
     * Destructor.
     */
    virtual ~TMSDtmfNotifier();

public:
    // New functions

    void SetDtmf(TmsMsgBufPckg dtmfpckg, gboolean publish = TRUE);

private:

    /**
     * C++ default constructor.
     */
    TMSDtmfNotifier();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:
    gint iVolume;
    };

} //namespace TMS

#endif // DTMFNOTIFIER_H

// End of File
