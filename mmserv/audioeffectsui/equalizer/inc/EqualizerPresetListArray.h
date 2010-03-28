/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Item text array for Equalizer listbox.
*
*/





#ifndef CEQUALIZERPRESETLISTARRAY_H
#define CEQUALIZERPRESETLISTARRAY_H

//  INCLUDES
#include "EqualizerPrivateConstants.h"
#include <AudioEqualizerUtility.h>

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
*  Item text array for playlist listbox. 
*   Creates item text descriptors according to playlist data stored to playlist
*   model.
*
*  @lib Equalizer.dll
*  @since 3.0
*/
NONSHARABLE_CLASS(CEqualizerPresetListArray) : public CBase,
                              public MDesCArray
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @since 3.0
         * @param aAudEqUtility a pointer to CAudioEqualizerUtility 
         * @param aActivePreset Currently active preset
         */
        static CEqualizerPresetListArray* NewL(
                   CAudioEqualizerUtility*  aAudioEqUtil,
                   TInt aActivePresetKey);
        
        /**
        * Destructor.
        */
        virtual ~CEqualizerPresetListArray();
    
    public: //New Functions
        /**
         * Used to set the currently active preset
         * @since 3.0
         * @param aActivePreset Currently active preset
         */
        void SetCurrentActivePreset(const TPtrC& aActivePreset);
        
		/** 
         * Returns the currently actibe preset
         */        
		void GetCurrentActivePreset(TPresetName& aActivePreset);
		/** 
         * Updates the sorted array with the changed user-defined presets
         */
        void UpdateSortedArrayL();
        
        /**
        * Returns the PresetNameKey given the localised name for the preset
        */
		void GetPresetKeyFromLocalisedName(TInt& aPresetNameKey, 
											const TPresetName& aPresetName);
        /**
        * Returns the localised name for the preset given the PresetNameKey
        */
		void GetLocalisedPresetNameFromKey(TInt aPresetNameKey, 
											 TPresetName& aPresetName);
											 
       /**
        * This function is used to set new values for currently active preset 
        * @since 3.0
        * @param aActivePreset Active preset
        */
        void SetCurrentActivePreset(const TDesC& aActivePreset);
       
       /**
        * This function is used to set new values for CAudioEqualizerUtility* 
        * @since 3.0
        * @param aAudEqUtility New pointer to CAudioEqualizerUtility
        */
        void SetAudioEqualizer(CAudioEqualizerUtility* aAudEqUtility);    
        
        /**
        * This function is used to get the length of prefix (eg "0\t")
        * before the preset name. 
        * @since 3.0
        * @returns Length of the prefix
        */
        TInt GetPrefixLength();
        
    public: // Functions from base classes

        /**
        * From MDesCArray, returns the number of descriptor elements
        * in a descriptor
        */
        TInt MdcaCount() const;
        
        /**
        * From MDesCArray, indexes into a descriptor array.
        */
        TPtrC MdcaPoint(TInt aIndex) const;
        
   
    private: // Constructors

        /**
         * First-phase constructor.
         * @since 3.0
         * @param aAudioEqUtil a pointer to CAudioEqualizerUtility 
         */
        CEqualizerPresetListArray(CAudioEqualizerUtility*  aAudioEqUtil);
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(TInt aActivePresetKey);
        
        /** 
        * Creates the ListBox text including the icon information. 
        * e.g "0\tNone"
        */
        void CreateListBoxIconText(const TPtrC& aPresetName) const;

        /**
        * Searches for a localised name for a preset found in the CenRep
        */
        HBufC* SearchForPredefinedPresetL(const CDesCArray& 
            aPresetArray, const TPresetName& aCenrepPreset);
    private:    // Data
        HBufC* iItem;
        // Pointer to CAudioEqualizerUtility
        CAudioEqualizerUtility* iAudEqUtility;
        // This preset array contains alphabetically sorted user-defined
        // presets
        CDesCArray* iSortedPresetArray;
        // This preset array contains localised predefined presets
        CDesCArray* iPreDefinedPresetArray;    
        //Preset "None"
        HBufC* iPresetNoneText;
        //Current active preset
        TPresetName iActivePreset;
    };

#endif      // CEQUALIZERPRESETLISTARRAY_H
            
// End of File
