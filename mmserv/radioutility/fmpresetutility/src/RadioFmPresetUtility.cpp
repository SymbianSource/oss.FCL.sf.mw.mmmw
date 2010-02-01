/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains implementation of the CRadioFmPresetUtility class,
*             	which controlls the presets for an FM tuner.
*
*/

 
#include <RadioPresetUtility.h>
#include <RadioFmPresetUtility.h>
#include "RadioFmPresetUtilityBody.h"

EXPORT_C CRadioFmPresetUtility* CRadioFmPresetUtility::NewL( MRadioPresetObserver& aObserver )
	{
	CRadioFmPresetUtility* self = new(ELeave) CRadioFmPresetUtility(aObserver);
	CleanupStack::PushL(self);
	self->iBody = CRadioFmPresetUtility::CBody::NewL( aObserver );
	CleanupStack::Pop(self);
	return self;
	}

// Destructor
EXPORT_C CRadioFmPresetUtility::~CRadioFmPresetUtility()
	{
		delete iBody;
	}

CRadioFmPresetUtility::CRadioFmPresetUtility(MRadioPresetObserver& /*aObserver*/)
	{
	}

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::SetPresetL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRadioFmPresetUtility::SetPresetL( TInt aIndex, const TFmPresetName& aName, TInt aFrequency )
	{	
	iBody->SetPresetL(aIndex,aName,aFrequency);
	}

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::SetPresetUrlL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRadioFmPresetUtility::SetPresetUrlL( TInt aIndex, const TFmPresetUrl& aUrl )
    {
    iBody->SetPresetUrlL(aIndex, aUrl);
    }

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::SetPresetPtyL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRadioFmPresetUtility::SetPresetPtyL( TInt aIndex, TUint aPty )
    {
    iBody->SetPresetPtyL(aIndex, aPty);
    }

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::SetPresetPICodeL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRadioFmPresetUtility::SetPresetPICodeL( TInt aIndex, TUint aPICode )
    {
    iBody->SetPresetPICodeL(aIndex, aPICode);
    }
// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::SetPresetFavouriteInfoL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRadioFmPresetUtility::SetPresetFavouriteInfoL( TInt aIndex, TBool aFavourite )
    {
    iBody->SetPresetFavouriteInfoL(aIndex, aFavourite);
    }

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::SetPresetUserRenameInfoL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRadioFmPresetUtility::SetPresetUserRenameInfoL( TInt aIndex, TBool aRename )
    {
    iBody->SetPresetUserRenameInfoL(aIndex, aRename);
    }

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::GetPresetL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C void CRadioFmPresetUtility::GetPresetL( TInt aIndex, TFmPresetName& aName, TInt& aFrequency )
	{
    iBody->GetPresetL(aIndex,aName,aFrequency);
	}

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::GetPresetNameL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C void CRadioFmPresetUtility::GetPresetNameL( TInt aIndex, TFmPresetName& aName)
	{
    iBody->GetPresetNameL(aIndex,aName);
	}

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::GetPresetUrlL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C void CRadioFmPresetUtility::GetPresetUrlL( TInt aIndex, TFmPresetUrl& aUrl)
	{
    iBody->GetPresetUrlL(aIndex,aUrl);
	}

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::GetPresetPtyL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
EXPORT_C void CRadioFmPresetUtility::GetPresetPtyL( TInt aIndex, TUint& aPty)
    {
    iBody->GetPresetPtyL(aIndex,aPty);
    }

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::GetPresetPICodeL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
EXPORT_C void CRadioFmPresetUtility::GetPresetPICodeL( TInt aIndex, TUint& aPICode)
    {
    iBody->GetPresetPICodeL(aIndex,aPICode);
    }


// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::GetPresetFavouriteInfoL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
EXPORT_C void CRadioFmPresetUtility::GetPresetFavouriteInfoL( TInt aIndex, TBool& aFavourite)
    {
    iBody->GetPresetFavouriteInfoL(aIndex,aFavourite);
    }

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::GetPresetUserRenameInfoL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
EXPORT_C void CRadioFmPresetUtility::GetPresetUserRenameInfoL( TInt aIndex, TBool& aRename)
    {
    iBody->GetPresetUserRenameInfoL(aIndex,aRename);
    }

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::GetPresetFrequencyL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C void CRadioFmPresetUtility::GetPresetFrequencyL( TInt aIndex, TInt& aFrequency)
	{
    iBody->GetPresetFrequencyL(aIndex,aFrequency);
	}

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::GetMaxNumberOfPresets
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRadioFmPresetUtility::GetMaxNumberOfPresets( TInt& aMaxNumOfPresets ) const
	{
	return iBody->GetMaxNumberOfPresets(aMaxNumOfPresets);
	}

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::GetNumberOfPresets
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C TInt CRadioFmPresetUtility::GetNumberOfPresets( TInt& aNumOfPresets ) const
	{
	return iBody->GetNumberOfPresets(aNumOfPresets);
	}

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::GetFirstPresetL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRadioFmPresetUtility::GetFirstPresetL( TInt& aIndex) const
	{
    iBody->GetFirstPresetL(aIndex);
	}

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::GetNextPresetL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CRadioFmPresetUtility::GetNextPresetL( TInt aIndex, TInt& aNextIndex ) const
	{
    iBody->GetNextPresetL(aIndex,aNextIndex);
	}

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::DeletePresetL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
EXPORT_C void CRadioFmPresetUtility::DeletePresetL( TInt aIndex )
	{
    iBody->DeletePresetL(aIndex);
	}
// End of File
