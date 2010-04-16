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

#ifndef XAVIDEOENCODERITFADAPTATION_H
#define XAVIDEOENCODERITFADAPTATION_H

#include "XAAdaptationContextBase.h"


/* FUNCTIONS */
XAresult XAVideoEncoderItfAdapt_SetVideoSettings(XAAdaptationBaseCtx *bCtx,
                                                 XAVideoSettings *pSettings  );
XAresult XAVideoEncoderItfAdapt_GetVideoSettings( XAAdaptationBaseCtx *bCtx,
                                                  XAVideoSettings *pSettings );
#endif /* XAVIDEOENCODERITFADAPTATION_H */
