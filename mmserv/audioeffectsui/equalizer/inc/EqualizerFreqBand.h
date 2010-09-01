/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Declares classes related to Equalizer Settings
*
*/




#ifndef EQUALIZERFREQBAND_H
#define EQUALIZERFREQBAND_H

//  INCLUDES
#include <AknDialog.h> 
#include "EqualizerPrivateConstants.h"

// FORWARD DECLARATIONS
class CAudioEqualizerUtility;
class CAudioEqualizer;

// CLASS DECLARATIONS

NONSHARABLE_CLASS(CEqualizerFreqBand)  : public CCoeControl
    {
    public:      // Constructors and destructor
        /**
        * This function is called to create an object of 
        * type CEqualizerFreqBand - Two phase Construction
        * @since 3.0
        * @param aaEqEditingDialog a reference to CEqualizerEditingDialog
        * @param aAudEqUtility a reference to CAudioEqualizerUtility 
        * @param aPresetIndex the index of the Preset being edited
        * @param aIsActivePreset whether the preset is active or not?
        * @param aBandIndex the index of the Frequncy band
        * @param aContainerRect the CEqualizerFreqBandContainer rectangle
        */
        static CEqualizerFreqBand* NewL(CEqualizerEditingDialog* aEqEditingDialog,
         CAudioEqualizerUtility*& aAudEqUtility, 
         const TInt aPresetIndex, const TBool aIsActivePreset, const TInt aBandIndex, 
         TRect& aContainerRect);
        /**
        * This function is called to create an object of 
        * type CEqualizerFreqBand - Two phase Construction
        * @since 3.0
        * @param aaEqEditingDialog a reference to CEqualizerEditingDialog
        * @param aAudEqUtility a reference to CAudioEqualizerUtility 
        * @param aPresetIndex the index of the Preset being edited
        * @param aIsActivePreset whether the preset is active or not?
        * @param aBandIndex the index of the Frequncy band
        * @param aContainerRect the CEqualizerFreqBandContainer rectangle
        */
        static CEqualizerFreqBand* NewLC(CEqualizerEditingDialog* aEqEditingDialog,
         CAudioEqualizerUtility*& aAudEqUtility, 
         const TInt aPresetIndex, const TBool aIsActivePreset, const TInt aBandIndex, 
         TRect& aContainerRect);
        /**
        * Destructor
        * @since 3.0
        */
        virtual ~CEqualizerFreqBand();

    public:     // Functions from base classes 
        /**
        * From CCoeControl Called when user presses a key
        */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
           TEventCode aType);
    
    public:
    
		/**
		* Function to return the Rect of the controls from the 
		* layout [read from the CDL file].
		* TRect aRect :  parent controls rect
		* TInt aVariety : equalizer variety based on the number 
		* of frequnecy bands
		* TInt aComponentNumber : Controls which are enumerated for
		* which the rectangle is required from the layout
		*/
        static TRect ComponentRect(const TRect& aRect, TInt aVariety, 
                                                TInt aComponentNumber);
    
    private://Constructors
        /**
        * This function is called to create an object of 
        * type CEqualizerFreqBand - First phase Constructor
        * @since 3.0
        * @param aaEqEditingDialog a reference to CEqualizerEditingDialog
        * @param aAudEqUtility a reference to CAudioEqualizerUtility 
        * @param aPresetIndex the index of the Preset being edited
        * @param aIsActivePreset whether the preset is active or not?
        * @param aBandIndex the index of the Frequncy band
        * @param aContainerRect the CEqualizerFreqBandContainer rectangle
        */
        CEqualizerFreqBand(CEqualizerEditingDialog* aEqEditingDialog,CAudioEqualizerUtility*& aAudEqUtility, 
         const TInt aPresetIndex, const TBool aIsActivePreset, 
         const TInt aBandIndex,
         TRect& aContainerRect); 

        /*
        * Second Phase Constructor
        */
        void ConstructL();    
        
    private: // New functions 
        /**
        *This function is called to load the bitmaps from the icons file
        */
        void LoadBitmapsL();
        
        /**
        * This member is called by the Equalizer UI to update the new band
        * level in the engine 
        * a new preset. 
        * @ since Series 60 3.0
        * lastIncFlag : Flag which indicates that the slider has reached the
        * the limits
        */
        void UpdateBandLevelInEngineL(TBool lastIncFlag);
    public: // New functions 
       /**
        * This function is used to set new values for CAudioEqualizerUtility* 
        * @since 3.0
        * @param aAudEqUtility New pointer to CAudioEqualizerUtility
        */
        void SetAudioEqualizerL(CAudioEqualizerUtility* aAudEqUtility);

#ifdef RD_SCALABLE_UI_V2
       /**
        * This function is used by the frequency band container to send 
        * pointer events to individual bands 
        * @since 3.2
        * @param aPointerEvent pointer event 
        */
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);
#endif //RD_SCALABLE_UI_V2

    private: // Functions from base classes      
        /**
        * From CCoeControl Called to draw a control
        */
        void Draw(const TRect& aRect) const;
        /**
        * From CCoeControl Called when the control's size is changed
        */
        void SizeChanged();   
        /**
        * From CCoeControl Called when a control's focus is changed
        */
        void FocusChanged(TDrawNow aDrawNow);
            
        /**
        * From CCoeControl
        * This function is called when there is a skin change or
        * change in the layout
        */
        void HandleResourceChange(TInt aType);      
    
    private:   //Data
        //Pointers to hold the equalizer backgroung bitmap and mask
        CFbsBitmap * iBgBitmap;
        CFbsBitmap * iBgBitmapMask;

        //Pointers to hold the equalizer scale bitmap and mask
        CFbsBitmap * iSliderBitmap;
        CFbsBitmap * iSliderBitmapMask;
        
        // Stores the zero position of mover
        TInt iZeroPos;
        // Minimum frequency band decibel level 
        TInt32 iMinBandLevel;
        // Maximum frequency band decibel level 
        TInt32 iMaxBandLevel;        
        // Used to store a pointer to CEqualizerEditingDialog 
        CEqualizerEditingDialog* iEqEditingDialog;
        // Used to store a pointer to CAudioEqualizerUtility 
        CAudioEqualizerUtility* iAudEqUtility;
        // Used to store a pointer to CAudioEqualizer 
        CAudioEqualizer* iAudioEqualizer;
        // The index of the preset in the array returned by
        // CAudioEqualizerUtility::Presets()
        const TInt iPresetIndex;
        // Used to store the name of the preset
        TPresetName iPresetName;
        // Whether the preset displayed is an active preset?
        const TBool iIsActivePreset;
        // Index of the Frequency Band as passed to Engine
        const TInt iBandIndex;
        // Position of the mover
        TInt iMoverPos;
        //Stores the container's rect value
        TRect& iContainerRect;  
        //Stores the freq band Scale's rectangle
        TRect iScaleRect;
        //Stores the slider's rectangle
        TRect iSliderRect;
        //Stores the background rectangle
        TRect iBackgroundRect;
		//Stores the step count of the slider
        TInt32 iStepCount;
        };


#endif //EQUALIZERFREQBAND_H
// End of File
