/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:
 * This file defines the interface to the client side session of the
 * STS Server.
 */

#ifndef RSTSSESSION_H_
#define RSTSSESSION_H_

#include <e32std.h>
#include <systemtoneservice.h>

class RStsSession : public RSessionBase
    {
public:

    TInt Connect();

    void Close();

    TInt SendPlayTone(CSystemToneService::TToneType aToneType,
            unsigned int& aPlayToneContext);

    TInt SendStopTone(unsigned int aPlayToneContext);

private:

    TInt StartServer();

    };

#endif // RSTSSESSION_H_
