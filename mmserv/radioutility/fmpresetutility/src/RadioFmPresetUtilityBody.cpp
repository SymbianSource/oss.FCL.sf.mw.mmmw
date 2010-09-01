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
* Description:  Contains implementation of the CRadioFmPresetUtility::CBody
*             	class, which controlls the presets for an FM tuner. 
*
*/



#include "RadioFmPresetUtilityBody.h"
#include "debug.h"
#include "RadioFmPresetRadioSettings.h"
#include "RadioFmPresetCentralRepositoryHandler.h"
#include "RadioFmPresetCRKeys.h"
#include <e32property.h>



CRadioFmPresetUtility::CBody* CRadioFmPresetUtility::CBody::NewL( MRadioPresetObserver& aObserver )
	{
	CRadioFmPresetUtility::CBody* s = new(ELeave) CRadioFmPresetUtility::CBody();
	s->iRadioPresetClient = &aObserver;
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop(s);
	return s;
	}


// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::CBody
// -----------------------------------------------------------------------------
//
CRadioFmPresetUtility::CBody::CBody()
    {
    }	
// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::~CBody
// -----------------------------------------------------------------------------
//
CRadioFmPresetUtility::CBody::~CBody()
    {
    delete iRadioSettings;
    delete iCentralRepositoryHandler;
    }
	
// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::ConstructL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CRadioFmPresetUtility::CBody::ConstructL()
	{  
    iRadioSettings = new(ELeave) TRadioSettings;
    iCentralRepositoryHandler = CCentralRepositoryHandler::NewL(*iRadioSettings);
	}

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::SetPresetL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioFmPresetUtility::CBody::SetPresetL( TInt aIndex, const TFmPresetName& aName, TInt aFrequency )
	{
	iCentralRepositoryHandler->SavePresetFrequencyL(aIndex,aFrequency);
	iCentralRepositoryHandler->SavePresetNameL(aIndex,aName);	
	iRadioPresetClient->MrpeoPresetChanged(MRadioPresetObserver::EPresetAdded,aIndex);
	}

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::SetPresetUrlL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioFmPresetUtility::CBody::SetPresetUrlL( TInt aIndex, const TFmPresetUrl& aUrl )
	{
	iCentralRepositoryHandler->SavePresetUrlL(aIndex, aUrl);
	iRadioPresetClient->MrpeoPresetChanged(MRadioPresetObserver::EPresetUpdated,aIndex);
	}

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::SetPresetPtyL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioFmPresetUtility::CBody::SetPresetPtyL( TInt aIndex, TUint aPty )
    {
    iCentralRepositoryHandler->SavePresetPTYL(aIndex, aPty);
    iRadioPresetClient->MrpeoPresetChanged(MRadioPresetObserver::EPresetUpdated,aIndex);
    }

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::SetPresetPICodeL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioFmPresetUtility::CBody::SetPresetPICodeL( TInt aIndex, TUint aPICode )
    {
    iCentralRepositoryHandler->SavePresetPIL(aIndex, aPICode);
    iRadioPresetClient->MrpeoPresetChanged(MRadioPresetObserver::EPresetUpdated,aIndex);
    }

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::SetPresetFavouriteInfoL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioFmPresetUtility::CBody::SetPresetFavouriteInfoL( TInt aIndex, TBool aFavourite )
    {
    iCentralRepositoryHandler->SavePresetFavouriteInfoL(aIndex, aFavourite);
    iRadioPresetClient->MrpeoPresetChanged(MRadioPresetObserver::EPresetUpdated,aIndex);
    }

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::SetPresetUserRenameInfoL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioFmPresetUtility::CBody::SetPresetUserRenameInfoL( TInt aIndex, TBool aRename )
    {
    iCentralRepositoryHandler->SavePresetUserRenameInfoL(aIndex, aRename);
    iRadioPresetClient->MrpeoPresetChanged(MRadioPresetObserver::EPresetUpdated,aIndex);
    }

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::GetPresetL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
void CRadioFmPresetUtility::CBody::GetPresetL( TInt aIndex, TFmPresetName& aName, TInt& aFrequency )
    {	
    aFrequency = iCentralRepositoryHandler->RetrievePresetFrequencyL(aIndex);
    aName      = iCentralRepositoryHandler->RetrievePresetNameL(aIndex);
    }

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::GetPresetNameL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
void CRadioFmPresetUtility::CBody::GetPresetNameL( TInt aIndex, TFmPresetName& aName )
	{
   	aName = iCentralRepositoryHandler->RetrievePresetNameL(aIndex);
	}

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::GetPresetUrlL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioFmPresetUtility::CBody::GetPresetUrlL( TInt aIndex, TFmPresetUrl& aUrl )
    {
    aUrl = iCentralRepositoryHandler->RetrievePresetUrlL(aIndex);
    }

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::GetPresetPtyL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
void CRadioFmPresetUtility::CBody::GetPresetPtyL( TInt aIndex, TUint& aPty )
    {
    aPty = iCentralRepositoryHandler->RetrievePresetPTYL(aIndex);
    }

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::GetPresetPICodeL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
void CRadioFmPresetUtility::CBody::GetPresetPICodeL( TInt aIndex, TUint& aPICode )
    {
    aPICode = iCentralRepositoryHandler->RetrievePresetPIL(aIndex);
    }

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::RetrievePresetFavouriteL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
void CRadioFmPresetUtility::CBody::GetPresetFavouriteInfoL( TInt aIndex, TBool& aFavourite )
    {
    aFavourite = iCentralRepositoryHandler->RetrievePresetFavouriteL(aIndex);
    }

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::GetPresetUserRenameInfoL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
void CRadioFmPresetUtility::CBody::GetPresetUserRenameInfoL( TInt aIndex, TBool& aRename )
    {
    aRename = iCentralRepositoryHandler->RetrievePresetUserRenameL(aIndex);
    }

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::GetPresetFrequencyL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
void CRadioFmPresetUtility::CBody::GetPresetFrequencyL( TInt aIndex, TInt& aFrequency )
	{
    aFrequency = iCentralRepositoryHandler->RetrievePresetFrequencyL(aIndex);
	}

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::GetMaxNumberOfPresets
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRadioFmPresetUtility::CBody::GetMaxNumberOfPresets( TInt& aMaxNumOfPresets ) const
	{
	aMaxNumOfPresets = KMaxNumberOfPresetStations;
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::GetNumberOfPresets
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
TInt CRadioFmPresetUtility::CBody::GetNumberOfPresets( TInt& aNumOfPresets ) const
	{	 
	TInt count = 0;
	TInt error = KErrNone;
	
	TRAP_IGNORE(iCentralRepositoryHandler->GetNumberOfPresetsL(count);)
    
    aNumOfPresets = count;
	return error;
	}

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::GetFirstPresetL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioFmPresetUtility::CBody::GetFirstPresetL( TInt& aIndex ) const
	{
    TInt firstPresetsIndex = 0;
    iCentralRepositoryHandler->GetFirstPresetL(firstPresetsIndex);
    aIndex = firstPresetsIndex;    
	}

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::GetNextPresetL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CRadioFmPresetUtility::CBody::GetNextPresetL( TInt aIndex, TInt& aNextIndex ) const
	{
     
    iCentralRepositoryHandler->GetNextPresetL(aIndex,aNextIndex); 
    
	}

// -----------------------------------------------------------------------------
// CRadioFmPresetUtility::CBody::DeletePresetL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//	
void CRadioFmPresetUtility::CBody::DeletePresetL( TInt aIndex )
	{
    iCentralRepositoryHandler->DeletePresetL(aIndex);
    
    iRadioPresetClient->MrpeoPresetChanged(MRadioPresetObserver::EPresetDeleted,aIndex);
    
	}

// End of File
