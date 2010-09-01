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
* Description:  Progressive Download Utility  Progressive Download Utility
*
*/



#include <ProgressiveDownloadUtility.h>
#include "PdProperties.h"
/**
Concrete implementation of the CProgressDownloadUtility API.
@see CProgressDownloadUtility
*/

EXPORT_C CProgressiveDownloadUtility* CProgressiveDownloadUtility::NewL()
	{
    CProgressiveDownloadUtility* self = new(ELeave) CProgressiveDownloadUtility();
	CleanupStack::PushL(self);

	self->iProperties = CPdProperties::NewL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CProgressiveDownloadUtility::~CProgressiveDownloadUtility()
	{
	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadUtility::~CProgressiveDownloadUtility"));
    #endif
    delete iProperties;
	}

void CProgressiveDownloadUtility::ConstructL()
	{

	}

CProgressiveDownloadUtility::CProgressiveDownloadUtility()
	{

	}

EXPORT_C MAudioPdPlayUtility* CProgressiveDownloadUtility::OpenL(const TDesC& aFileName,MAudioPdPlayUtilityCallback& aCallback)
	{
	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadUtility::OpenL by filename"));
    #endif
	return STATIC_CAST(MAudioPdPlayUtility*,iProperties->OpenL(aFileName,aCallback));

	}

EXPORT_C MAudioPdPlayUtility* CProgressiveDownloadUtility::OpenL(const RFile& aFileHandle,MAudioPdPlayUtilityCallback& aCallback)
	{
	#if _DEBUG
	  RDebug::Print(_L("CProgressiveDownloadUtility::OpenL by filehandle"));
    #endif
	return STATIC_CAST(MAudioPdPlayUtility*,iProperties->OpenL(aFileHandle,aCallback));
	}

// ENd of File
