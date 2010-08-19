/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the S60 ProgDL custom command msg codes.
*
*/


#ifndef PROGDLSOURCECUSTOMCOMMANDS_H
#define PROGDLSOURCECUSTOMCOMMANDS_H

enum TProgDLSourceCC
    {
        EDownloadRate = 1,
        ESetDownloadSize,
        ESetActualSize,
        EGetSourceState,
        ESetDownloadState,
        EGetBitRate,
        EGetPercentageBuffered,
        ESetUpdateFileName,
        ESetFileMoving
    };

#endif // PROGDLSOURCECUSTOMCOMMANDS_H

//  End of File
