/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class provides an interface for controlling the presets
*                for an FM tuner.
*
*
*/

 

#ifndef C_RADIOFMPRESETUTILITYBODY_H
#define C_RADIOFMPRESETUTILITYBODY_H

#include <RadioPresetUtility.h>
#include <RadioFmPresetUtility.h>
#include "RadioFmPresetRadioSettings.h"
#include "RadioFmPresetCentralRepositoryHandler.h"
#include <e32base.h> 

const TInt KDefaultRadioFrequency = 87500000;
const TInt KMaxNumberOfPresetStations = 100;
const TUint KDefaultRadioPTY = 0;
const TUint KDefaultRadioPI = 0;

/**
 *  This class provides an interface for controlling the preset for an FM tuner.
 *  The maximum number of presets supported by this utility is pre-defined and the
 *  client must first query it.
 *
 *  @lib RadioPresetUtility.lib
 *  @since S60 3.2
 */
class CRadioFmPresetUtility::CBody : public CBase,
                              public MRadioPreset,
                              public MRadioFmPreset
    {
public:

    /**
     * Factory function to create a new instance of the FM tuner.
     *
     * @since S60 3.2
     * @param aObserver The observer object
     * @return A new FM tuner utility object.
     */
    static CBody* NewL( MRadioPresetObserver& aObserver );

    ~CBody();
    
	// from base class MRadioFmPreset

     /**
      * From MRadioFmPresetUtility
      * Set the preset with specified frequency and name. Previous content in the specified index
      * will be overwritten with the new information.
      *
      * The maximum length for the preset name is 32 characters. Names longer than 32 characters
      * will be truncated.
      *
      * @since S60 3.2
      * @param aIndex A preset index.
      * @param aName Preset name.
      * @param aFrequency Frequency in Hz.
      */
     void SetPresetL( TInt aIndex, const TFmPresetName& aName, TInt aFrequency );

     /**
      * From MRadioFmPresetUtility
      * Set the preset with specified URL. Previous content in the specified index
      * will be overwritten with the new information.
      *
      * The maximum length for the preset URL is 64 characters. URLs longer than 32 characters
      * will be truncated.
      *
      * @since S60 5.2
      * @param aIndex A preset index.
      * @param aUrl Preset URL.
      */     
     void SetPresetUrlL( TInt aIndex, const TFmPresetUrl& aUrl );
 
     /**
      * From MRadioFmPresetUtility
      * Set the preset with specified PTY. Previous content in the specified index
      * will be overwritten with the new information.
      *
      * @since S60 5.2
      * @param aIndex A preset index.
      * @param aPty Preset PTY (programme type).
      */
     void SetPresetPtyL( TInt aIndex, TUint aPty );

     /**
      * From MRadioFmPresetUtility
      * Set the preset with specified PI code. Previous content in the specified index
      * will be overwritten with the new information.
      *
      * @since S60 5.2
      * @param aIndex A preset index.
      * @param aPICode Preset PI (programme identification).
      */
     void SetPresetPICodeL( TInt aIndex, TUint aPICode );

     /**
      * From MRadioFmPresetUtility
      * Set the preset with specified favourite information. Previous content in the specified index
      * will be overwritten with the new information.
      *
      * @since S60 5.2
      * @param aIndex A preset index.
      * @param aFavourite Preset favourite info.
      */     
     void SetPresetFavouriteInfoL( TInt aIndex, TBool aFavourite );

     /**
      * From MRadioFmPresetUtility
      * Set the preset with specified user rename information. Previous content in the specified index
      * will be overwritten with the new information.
      *
      * @since S60 5.2
      * @param aIndex A preset index.
      * @param aRename Preset user rename info.
      */     
     void SetPresetUserRenameInfoL( TInt aIndex, TBool aRename );

     /**
      * From MRadioFmPreset
      * Get the preset name and frequency at the specified index.
      * Leaves with KErrNotFound if invalid index is specified.
      *
      * Preset name is maximum of 32 characters. Client must allocate buffer big
      * enough to hold the 32 characters. If not, characters will be truncated to fit the
      * buffer.
      *
      * @since S60 3.2
      * @param aIndex A preset index.
      * @param aFrequency On return, contains the preset name.
      * @param Frequency On return, contains frequency in Hz.
      */
     void GetPresetL( TInt aIndex, TFmPresetName& aName, TInt& aFrequency );

     /**
      * From MRadioFmPreset      
      * Get the preset name at the specified index.
      * Leaves with KErrNotFound if invalid index is specified.
      *
      * Preset name is maximum of 32 characters. Client must allocate buffer big
      * enough to hold the 32 characters. If not, characters will be truncated to fit the
      * buffer.
      *
      * @since S60 5.1
      * @param aIndex A preset index.
      * @param aName On return, contains the preset name.
      */
     void GetPresetNameL( TInt aIndex, TFmPresetName& aName );

     /**
      * From MRadioFmPresetUtility      
      * Get the preset URL at the specified index.
      * Leaves with KErrNotFound if invalid index is specified.
      *
      * Preset URL is maximum of 64 characters. Client must allocate buffer big
      * enough to hold the 64 characters. If not, characters will be truncated to fit the
      * buffer.
      *
      * @since S60 5.2
      * @param aIndex A preset index.
      * @param aUrl On return, contains the preset name.
      */
     void GetPresetUrlL( TInt aIndex, TFmPresetUrl& aUrl );

     /**
      * From MRadioFmPresetUtility
      * Get the preset PTY (programme type) at the specified index.
      * Leaves with KErrNotFound if invalid index is specified.
      *
      * @since S60 5.2
      * @param aIndex A preset index.
      * @param aPty On return, contains the preset PTY.
      */
     void GetPresetPtyL( TInt aIndex, TUint& aPty );
     
     /**
      * From MRadioFmPresetUtility
      * Get the preset PI code (programme identification) at the specified index.
      * Leaves with KErrNotFound if invalid index is specified.
      *
      * @since S60 5.2
      * @param aIndex A preset index.
      * @param aPICode On return, contains the preset programme identification.
      */
     void GetPresetPICodeL( TInt aIndex, TUint& aPICode );
     
     /**
      * From MRadioFmPresetUtility
      * Get the preset favourite info at the specified index.
      * Leaves with KErrNotFound if invalid index is specified.
      *
      * @since S60 5.2
      * @param aIndex A preset index.
      * @param aFavourite On return, contains the preset favourite info.
      */
     void GetPresetFavouriteInfoL( TInt aIndex, TBool& aFavourite );

     /**
      * From MRadioFmPresetUtility
      * Get the preset user rename info at the specified index.
      * Leaves with KErrNotFound if invalid index is specified.
      *
      * @since S60 5.2
      * @param aIndex A preset index.
      * @param aRename On return, contains the preset rename info.
      */     
     void GetPresetUserRenameInfoL( TInt aIndex, TBool& aRename );
      
     /**
      * Get the preset frequency at the specified index.
      * Leaves with KErrNotFound if invalid index is specified.
      *
      * @since S60 5.1
      * @param aIndex A preset index.
      * @param aFrequency On return, contains the preset name.
      */
     void GetPresetFrequencyL( TInt aIndex, TInt& aFrequency );
	       
// from base class MRadioPreset

    /**
     * From MRadioPreset
     * Get the maximum number of presets supported by this utility.
     * The numbering of presets starts from 1 and the largest preset number equals the value
     * returned from this function.
     *
     * @since S60 3.2
     * @param aMaxNumOfPresets On return contains the maximum number of presets.
     * @return A standard system error code.
     */
     TInt GetMaxNumberOfPresets( TInt& aMaxNumOfPresets ) const;

    /**
     * From MRadioPreset
     * Get the number of presets already set by the client (i.e. contains data).
     *
     * @since S60 3.2
     * @param aNumOfPresets On return contains the current number of presets.
     * @return A standard system error code.
     */
     TInt GetNumberOfPresets( TInt& aNumOfPresets ) const;

    /**
     * From MRadioPreset
     * Get the first preset index that has been set.
     * Leaves with KErrNone if no preset exists.
     *
     * @since S60 3.2
     * @param aIndex On return contains the index of the first preset.
     */
     void GetFirstPresetL( TInt& aIndex ) const;

    /**
     * From MRadioPreset
     * Get the next preset index that has been set, given the current index.
     * Leaves with KErrNone if no other preset exists.
     *
     * @since S60 3.2
     * @param aIndex Current preset index.
     * @param aNextIndex On return contains the index of next preset.
     */
     void GetNextPresetL( TInt aIndex, TInt& aNextIndex ) const;

    /**
     * From MRadioPreset
     * Reset the specified preset.
     * Has not effect if specified index contains no preset.
     *
     * NOTE: Specifying aIndex == 0, will reset all presets.
     *
     * @since S60 3.2
     * @param aIndex A preset index.
     */
     void DeletePresetL( TInt aIndex );

private:
    CBody();
    void ConstructL();
    
    
private:

	MRadioPresetObserver* 	iRadioPresetClient;
	TRadioSettings*         iRadioSettings;
	CCentralRepositoryHandler* iCentralRepositoryHandler;

    };

#endif // C_RADIOFMPRESETUTILITY_H
