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

#ifndef XADEVICEVOLUMEITFADAPTATION_H_
#define XADEVICEVOLUMEITFADAPTATION_H_

#include "XAAdaptationContextBase.h"
#include "XADebug.h"


/* FUNCTIONS */
XAresult XADeviceVolumeItfAdapt_SetVolume( XAAdaptationBaseCtx *bCtx, XAuint32 deviceID, XAint32 volume);
XAresult XADeviceVolumeItfAdapt_IsDeviceIDSupported(XAAdaptationBaseCtx *bCtx, XAuint32 deviceID, XAboolean *isSupported);
#endif /* XADEVICEVOLUMEITFADAPTATION_H_ */
