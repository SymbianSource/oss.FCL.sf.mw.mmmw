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
* Description:    Declares class for frequency band container
*
*/




#ifndef EQUALIZERFREQBANDCONTAINER_H
#define EQUALIZERFREQBANDCONTAINER_H

//  INCLUDES
#include <AknDialog.h> 
#include "EqualizerFreqBand.h"
#include "EqualizerPrivateConstants.h"

// FORWARD DECLARATIONS
class CAudioEqualizerUtility;
class CAudioEqualizer;
class CAknsBasicBackgroundControlContext;

// CLASS DECLARATIONS

//////////////////////////////////////////////////////////////////////////////
//
// -----> CEqualizerFreqBandContainer(definition)
//
//////////////////////////////////////////////////////////////////////////////
NONSHARABLE_CLASS(CEqualizerFreqBandContainer)  : public CCoeControl
    {
    public:
        /**
          * This function is called to create an object of 
          * type CEqualizerFreqBandContainer - First phase Constructor
          * @since 3.0
          * @param aaEqEditingDialog a reference to CEqualizerEditingDialog
          * @param aAudEqUtility a pointer to CAudioEqualizerUtility 
          * @param aPresetIndex the index of the Preset being edited
          * @param aIsActivePreset whether the preset is active or not?
          * @param aRect the CEqualizerFreqBandContainer rectangle
          */
        CEqualizerFreqBandContainer(
        CEqualizerEditingDialog* aEqEditingDialog, 
        CAudioEqualizerUtility* aAudEqUtility, 
        const TInt aPresetIndex, const TBool aIsActivePreset, TRect& aRect,
        MEqualizerPresetActivationObserver& aObserver, TInt aPresetKey,
        TPresetName& aPresetName ); 
        /**
         * Destructor
         * @since 3.0
         */
        virtual ~CEqualizerFreqBandContainer();
  
    public:     
        /**
         * Handles key events 
         * @param aKeyEvent key event.
         * @param aType key code
         * @return TKeyResponse
         */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType); 
  
 
   private:  //New Functions
        void ConstructL(TRect& aRect);
        /**
         * This function is used to swap the focus from one control to other
         * @since 3.0
         * @param aPrevControl Previous control
         * @param aNewControl New Control
         */
        void SwapFocus(CCoeControl* aPrevControl, CCoeControl* aNewControl) ;

        /**
         * This function is used to construct the frequency value labels
         * @since 3.2
         */
        void ConstructLabelsL();
   
   public: // New functions     
       /**
        * This function is used to set new values for CAudioEqualizerUtility* 
        * @since 3.0
        * @param aAudEqUtility New pointer to CAudioEqualizerUtility
        */
        void SetAudioEqualizerL(CAudioEqualizerUtility* aAudEqUtility);
        
    private:  //Functions from base classes    
        /**
         * This function is called by the framework whenever a custom control
         * is constructed inside the Dialog
         */
        void ConstructFromResourceL(TResourceReader& aReader);
        /**
         * Virtual, defined by CCoeControl; replaces the default implementation
         * provided by CCoeControl.
         */        
        void         Draw(const TRect& /*aRect*/) const;

        /**
         * Virtual, defined by CCoeControl; replaces the default implementation
         * provided by CCoeControl. 
         */
        TInt         CountComponentControls() const;

        /** Virtual, defined by CCoeControl; replaces the default implementation
         * provided by CCoeControl.
         */
        CCoeControl* ComponentControl(TInt aIndex) const;
        /**
         *Virtual, defined by CCoeControl; empty implementation provided by
         * CCoeControl; full implementation provided by this class
         */
        void         SizeChanged();
        /*
         * From CCoeControl Called when a control's focus is changed
         */
        void FocusChanged(TDrawNow aDrawNow);

        /*
		* From CCoeControl
		* @see CCoeControl
		*/
		TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
		
#ifdef RD_SCALABLE_UI_V2
        /*
		* From CCoeControl
		* @see CCoeControl
		*/
		void HandlePointerEventL(const TPointerEvent& aPointerEvent);
#endif //RD_SCALABLE_UI_V2

    private:
        // Data members defined and used by this class.
        //Array of frequency band pointers  
        CArrayPtr<CEqualizerFreqBand>* iFreqBand;
        // Stores the currently focussed frequency band
        CCoeControl*    iFocussedFreqBand;
        // Stores the Index of the currently focussed frequency band
        TUint           iIndexOfFocussedFreqBand;
        // Used to store a pointer to CEqualizerEditingDialog 
        CEqualizerEditingDialog* iEqEditingDialog;
        // Used to store a pointer to CAudioEqualizerUtility 
        CAudioEqualizerUtility* iAudEqUtility;
        // Used to store the preset index
        const TInt iPresetIndex;
        // Whether the preset displayed is an active preset?
        const TBool iIsActivePreset;  
        // Rectangle in which to display the control 
        TRect& iRect;
        // Number of Frequency bands 
        TUint iNumberOfFreqBands;
        // For skin in the background
        CAknsBasicBackgroundControlContext* iBackGround;
        //Array of frequence band labels
        CArrayPtr<CEikLabel>* iFreqLabels;
        
        // Observer for changing equalizer presets
        MEqualizerPresetActivationObserver& iObserver;
        TInt iPresetKey;

        // Used to indicate a band is selected in touch
        TBool iBandSelected;

        // Used to store the preset name
        TPresetName iPresetName;
    
    };

#endif     //EQUALIZERFREQBANDCONTAINER_H

// End of File
