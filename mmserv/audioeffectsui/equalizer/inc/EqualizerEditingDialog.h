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




#ifndef EQUALIZEREDITINGDIALOG_H
#define EQUALIZEREDITINGDIALOG_H

//  INCLUDES
#include <AknDialog.h> 
#include "EqualizerPrivateConstants.h"

#include "EqualizerPresetsDialog.h"
// FORWARD DECLARATIONS
class CAudioEqualizerUtility;
class CEqualizerFreqBandContainer;

// CLASS DECLARATIONS
NONSHARABLE_CLASS(CEqualizerEditingDialog) : public CAknDialog
    {
    public:      // Constructors and destructor
    
        /**
        * This function is called to create an object of 
        * type CEqualizerPresetsDialog - Two phase Construction
        * @since 3.0
        * @param aAudEqUtility a pointer to CAudioEqualizerUtility 
        * @param aPresetIndex the index into the preset array retrieved
        * with the method CAudioEqualizerUtility::Presets()
        * @param aRect a reference to the view rectangle
        * returned by CAudioEqualizerUtility::Presets()         
        * @param aIsPresetActive whether the preset is active or not? 
        */
        static CEqualizerEditingDialog* NewL(
        CAudioEqualizerUtility* aAudEqUtility,const TInt aPresetIndex, 
        TRect& aRect, const TBool aIsPresetActive,
        MEqualizerPresetActivationObserver& aObs,
        TInt aPresetKey);
        /**
        * Destructor
        * @since 3.0
        */
       virtual ~CEqualizerEditingDialog(); 
    
    public:      // New functions  
        /**
        * This function is used to execute the Dialog.
        * @since 3.0
        * @returns return value of CAknDialog::ExecuteLD()   
        */
        TInt ExecuteLD();
        /**
        * This function is used to read the positions of the mover for the 
        * frequency bands from the engine.
        * @since 3.0
        * @param aReset Whether the preset is being reset
        */
        void SetMoverPositionsL(TBool aReset);   
        
        /**
        * This function is used to set new values for CAudioEqualizerUtility* 
        * @since 3.0
        * @param aAudEqUtility New pointer to CAudioEqualizerUtility
        */
        void SetAudioEqualizerL(CAudioEqualizerUtility* aAudEqUtility);    

        /**
        * This function is used to return current mover position for each frequence band
        * @since 3.0
        * @param aBandIndex Frequence band index for which mover position is required
        * @returns current mover position
        */
        TInt GetMoverPosition(TInt aBandIndex);   
 
    private:
        /**
        * First Phase Constructor
        * @since 3.0
        * @param aAudEqUtility a pointer to CAudioEqualizerUtility 
        * @param aPresetIndex the index into the preset array retrieved
        * with the method CAudioEqualizerUtility::Presets()
        * @param aRect a reference to the view rectangle
        * returned by CAudioEqualizerUtility::Presets()         
        * @param aIsPresetActive whether the preset is active or not? 
        */
        CEqualizerEditingDialog(CAudioEqualizerUtility* aAudEqUtility,
        const TInt aPresetIndex, TRect& aRect, 
        const TBool aIsPresetActive,
        MEqualizerPresetActivationObserver& aObs,
        TInt aPresetKey);
         
        /**
        * Second Phase Constructor
        * @since 3.0
        */
        void ConstructL();
       
    private: // Functions from base classes  
        /**
        * From CAknDialog Called when a key event happens.
        */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, 
            TEventCode aType ); 
        /**
        * From CAknDialog Called just before the menu pane is displayed
        */
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
        /**
        * From CAknDialog Called just before the menu pane is displayed
        */
        void ProcessCommandL(TInt  aCommandId); 
        /**
        * From CAknDialog Called when OK key is pressed
        */
        TBool OkToExitL(TInt aButtonId);
        /**
        * From CAknDialog Called when a custom control is to be created
        */
        SEikControlInfo CreateCustomControlL  (  TInt  aControlType   );

        /**
        * From CAknDialog
        * This function will respond to any change in the controls size.
        */
        void SizeChanged();
        /**
        * Get help context for the control.
        * @param aContext The context that is filled in.
        * @return None.
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

		/**
		* This function is called to retrieve the rectangle
		* of the freq band control, scale and slider from the layout
		*/
      	void GetLayoutRect();

    private:
        // Used to store a pointer to CAudioEqualizerUtility 
        CAudioEqualizerUtility* iAudEqUtility;
        // Used to store a pointer to CEqualizerFreqBandContainer 
        CEqualizerFreqBandContainer* iFreqBandContainer;
        // Used to store the preset name
        TPresetName iPresetName;
        // The index of the preset in the array returned by
        // CAudioEqualizerUtility::Presets()
        const TInt iPresetIndex;
        // The rectangle in which the Dialog is displayed
        TRect& iRect;
        // Whether the preset displayed is an active preset?
        const TBool iIsActivePreset;
        // Used to store the offset of the resource file
        TInt iResourceFileOffset;
        // Stores the positions of the movers in the various bands.
        CArrayFix<TInt>* iMover;
        // Number of Frequency bands 
        TUint iNumberOfFreqBands;   
        // Ignore Key events between track changes
        TBool iIgnoreKeyEvents;   
		// Array to store the rects of the freq band
        RArray<TRect> iFreqBandRect;
        // Array to store the rects of the freq band scale
        RArray<TRect> iScaleRect;
        // Array to store the rects of the background
        RArray<TRect> iBackgroundRect;
        // Array to store the rects of the slider
        RArray<TRect> iSliderRect;
		// Used to determine if context sensitive menu is ON or not when pressing MSK
        TBool iContextSensitiveMenu;
       
        // Observer for changing presets
        MEqualizerPresetActivationObserver& iObserver;
        TInt iPresetKey;
  };

#endif     //EQUALIZEREDITINGDIALOG_H

// End of File
