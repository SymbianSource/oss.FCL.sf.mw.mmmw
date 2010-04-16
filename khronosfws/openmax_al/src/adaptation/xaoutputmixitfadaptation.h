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

#ifndef OUTPUTMIXITFADAPTATION_H_
#define OUTPUTMIXITFADAPTATION_H_

#include "XAAdaptationContextBase.h"
#include "XADebug.h"

XAresult XAOutputMixItfAdapt_GetDestinationOutputDeviceIDs( XAAdaptationBaseCtx *bCtx, XAint32 * pNumDevices, XAuint32 * pDeviceIDs );
XAresult XAOutputMixItfAdapt_ReRoute( XAAdaptationBaseCtx *bCtx, XAint32 numOutputDevices, XAuint32 * pOutputDeviceIDs);

#endif /* OUTPUTMIXITFADAPTATION_H_ */
