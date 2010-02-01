/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface definition for the ProgressiveDownloadUtility functions.
*
*/


#ifndef __MAUDIOPDPLAYUTILITY_H__
#define __MAUDIOPDPLAYUTILITY_H__

#include <e32std.h>
#include <e32base.h>
#include "MProgressiveDownloadUtility.h"
#include "MAudioPlayUtility.h"




class MAudioPdPlayUtilityCallback: public MProgressiveDownloadUtilityCallback, 
                                   public MMdaAudioPlayerCallback
	{
	public:
		virtual void HandleDownloadEventL(TUint aTransactionID,                 
                                          TBrCtlDownloadEvent aEvent,
                                          TUint aValue)=0;
		
	};

class MAudioPdPlayUtility:public MProgessiveDownloadUtility, 
                          public MAudioPlayUtility
	{
	public:
		virtual ~MAudioPdPlayUtility(){};
		
	};



#endif
