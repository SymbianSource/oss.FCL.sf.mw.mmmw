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

#ifndef TAREVENTHANDLER_H
#define TAREVENTHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <e32property.h>
#include "tmsserver.h"

namespace TMS {

// CLASS DECLARATION
class CTarEventHandler : public CActive
    {
public:
    /**
     * Two-phased constructor.
     */
    static CTarEventHandler* NewL(TMSServer* aServer);

    /**
     * Destructor.
     */
    virtual ~CTarEventHandler();

private:
    /**
     * From CActive
     * Cancel outstanding request
     */
    void DoCancel();

    /**
     * From CActive
     * Implementation of CActive::RunL.
     * Called when server request has completed.
     **/
    void RunL();

private:

    /**
     * C++ default constructor.
     */
    CTarEventHandler(TMSServer* aServer);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:
    TMSServer* iTMSSer;
    RProperty iProperty;
    };

} //namespace TMS

#endif  // TAREVENTHANDLER_H

// End of File
