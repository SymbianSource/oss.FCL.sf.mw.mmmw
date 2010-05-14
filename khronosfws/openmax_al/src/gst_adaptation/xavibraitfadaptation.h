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

#ifndef XAVibraItfADAPTATION_H_
#define XAVibraItfADAPTATION_H_

#include "xaadaptationgst.h"

/* FUNCTIONS */
XAresult XAVibraItfAdapt_Vibrate( XAAdaptationGstCtx *bCtx, XAboolean vibrate );
XAresult XAVibraItfAdapt_SetFrequency ( XAAdaptationGstCtx *bCtx,  XAmilliHertz frequency );
XAresult XAVibraItfAdapt_SetIntensity ( XAAdaptationGstCtx *bCtx,  XApermille intensity );
#endif /* XAVIBRAITFADAPTATION_H_ */
