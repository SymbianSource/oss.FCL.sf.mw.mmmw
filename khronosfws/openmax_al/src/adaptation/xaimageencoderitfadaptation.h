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

#ifndef XAIMAGEENCODERITFADAPTATION_H
#define XAIMAGEENCODERITFADAPTATION_H

#include "XAAdaptationContextBase.h"

/* FUNCTIONS */
XAresult XAImageEncoderItfAdapt_SetImageSettings(XAAdaptationBaseCtx *bCtx,
                                                 const XAImageSettings *pSettings);
XAresult XAImageEncoderItfAdapt_GetImageSettings(XAAdaptationBaseCtx *bCtx,
                                                 XAImageSettings *pSettings);
#endif /* XAIMAGEENCODERITFADAPTATION_H */
