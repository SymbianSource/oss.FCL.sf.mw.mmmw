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
* Description: 
*
*/

#ifndef XAAUDIOENCODERITFADAPTATION_H
#define XAAUDIOENCODERITFADAPTATION_H

#include "XAAdaptationContextBase.h"
#include "XADebug.h"

/* FUNCTIONS */
XAresult XAAudioEncoderItfAdapt_SetEncoderSettings(XAAdaptationBaseCtx *ctx,
                                                   XAAudioEncoderSettings *pSettings );
XAresult XAAudioEncoderItfAdapt_GetEncoderSettings(XAAdaptationBaseCtx *ctx,
                                                   XAAudioEncoderSettings *pSettings );

#endif /* XAAUDIOENCODERITFADAPTATION_H */
