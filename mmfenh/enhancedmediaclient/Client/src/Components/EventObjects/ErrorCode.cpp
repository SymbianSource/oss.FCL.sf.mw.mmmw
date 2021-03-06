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
* Description:  Implementation of ErrorCode class.
*
*/


#include "ErrorCode.h"

using namespace multimedia;

CErrorCode::CErrorCode(
    TInt aReason):
    iReason(aReason)
    {
    // No impl
    }

CErrorCode::~CErrorCode()
    {
    }

TAny* CErrorCode::GetInterface()
    {
    MErrorCode* interface = this;
    return interface;
    }

TInt CErrorCode::GetErrorCode()
    {
    return iReason;
    }

// End of file
