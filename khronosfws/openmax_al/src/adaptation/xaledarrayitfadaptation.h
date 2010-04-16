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

#ifndef XALEDARRAYITFADAPTATION_H_
#define XALEDARRAYITFADAPTATION_H_

#include "XAAdaptationContextBase.h"
#include "XADebug.h"



/* FUNCTIONS */
XAresult XALEDArrayItfAdapt_ActivateLEDArray ( XAAdaptationBaseCtx *bCtx, XAuint32 lightMask );
XAresult XALEDArrayItfAdapt_SetColor ( XAAdaptationBaseCtx *bCtx, XAuint8 index, const XAHSL * pColor );
#endif /* XALEDARRAYITFADAPTATION_H_ */
