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
* Description:    Declaration of EqualizerPresetsDialog class
*
*/




#ifndef EQUALIZERPRESETSDIALOG_H
#define EQUALIZERPRESETSDIALOG_H

// INCLUDES
#include <AknDialog.h> 
#include <aknlists.h>
#include <EqualizerConstants.h>
#include <AudioEqualizerUtility.h>

// CLASS DECLARATION
class CEqualizerPresetListArray;
class CEqualizerEditingDialog;

/**
* This class is used to notify the client application about the activation
* of a new preset.
*  
*  @lib Equalizer.lib
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS(MEqualizerPresetActivationObserver)
    {
    public: // New functions
        /**
        * This member is called by the Equalizer UI when the user activates 
        * a new preset. 
        * @ since Series 60 3.0
        * @ aActivatedPreset It is KEqualizerPresetNone if the preset "None"
        * is active. Otherwise, it is the value of 
        * TEfAudioEqualizerUtilityPreset::iPresetNameKey for the entry
        * whose TEfAudioEqualizerUtilityPreset::iPresetName matches the
        * activated preset name, in the array retrieved using 
        * CAudioEqualizerUtility::Presets().
        */
        virtual void HandlePresetActivation (TInt aActivatedPreset) = 0;
   };
    
/**
*  This class is used to display the preset list view of the Equalizer.
*  
*  @lib Equalizer.lib
*  @since Series 60 3.0
*/
class CEqualizerPresetsDialog : public CAknDialog
#ifdef RD_SCALABLE_UI_V2
                              , public MEikListBoxObserver
#endif //RD_SCALABLE_UI_V2
    {
    public:     //Constructors and Destructors
        /**
        * This function is called to create an object of 
        * type CEqualizerPresetsDialog - Two phase Construction
        * @since 3.0
        * @param aAudEqUtility a reference to CAudioEqualizerUtility 
        * @param aCurrentActivePreset It is KEqualizerPresetNone if the preset
        * "None" is active. Otherwise, it is the value of 
        * TEfAudioEqualizerUtilityPreset::iPresetNameKey for the entry
        * whose TEfAudioEqualizerUtilityPreset::iPresetName matches the
        * activated preset name, in the array retrieved using 
        * CAudioEqualizerUtility::Presets().
        * @param aEqualizerPresetActivationObserver a reference to an object 
        * of type MEqualizerPresetActivationObserver         
        */
        IMPORT_C static CEqualizerPresetsDialog* NewL(
           CAudioEqualizerUtility* aAudEqUtility,
           TInt aCurrentActivePreset, 
           MEqualizerPresetActivationObserver& aEqrPresetActObserver);

        /**
        * Destructor
        * @since 3.0
        */
        virtual ~CEqualizerPresetsDialog();
    private: //New functions
        /**
        * Second Phase Constructor
        * @since 3.0
        * @param aCurrentActivePreset the index into the preset array retrieved
        * with the method CAudioEqualizerUtility::Presets()if a preset other 
        * than "None" is active. It is KEqualizerPresetNone if the preset 
        * "None" is active.
        */
        void ConstructL(const TInt aCurrentActivePreset);
        /**
        * This function is called to create an object of 
        * type CEqualizerPresetsDialog - First Phase Constructor
        * @since 3.0
        * @param aAudEqUtility a reference to CAudioEqualizerUtility 
        * @param aEqualizerPresetActivationObserver a reference to an object 
        * of type MEqualizerPresetActivationObserver         
        */
        CEqualizerPresetsDialog (
           CAudioEqualizerUtility* aAudEqUtility,
           MEqualizerPresetActivationObserver& aEqrPresetActObserver);
       
        /**
        * This function is called to get a free preset name
        * @since 3.0
        * @param aName Used to reurn a free preset name
        */
        void BuildPresetNameL( TDes& aName) const;
        
        /**
        * This function returns the highlighted Preset in ListBox
        * @since 3.0
        * @return      Highlighted Preset e.g. "None"
        */
        TPresetName GetHighlightedPresetInListBox() const;
        /**
        * This function is called to find the index of a preset in the list
        * of presets returned by CAudioEqualizerUtility::Presets()
        * @since 3.0
        * @param aPreset The preset focussed in ListBox
        * @return index of the highlighted preset in the ListBox in the 
        * list of presets returned by CAudioEqualizerUtility::Presets() 
        */
        TInt GetEngineIndexL(const TPresetName& aPreset) const; 
        /**
        * This function is used for the activation of a preset
        * @since 3.0
        * @param aIndex The index of the preset in the ListBox
        * @param aPresetName The name of the preset
        */
        void ActivateEqrPresetL(const TInt aIndex, const TDesC& aPreset);
        /**
        * This function is used for the editing of a preset
        * @since 3.0
        * @param aIndex The index of the preset in the ListBox
        * @param aPresetName The name of the preset
        */
        void EditEqrPresetL(const TInt aIndex, const TDesC& aPreset);
        /**
        * This function is used for the creation of a new preset
        * @since 3.0
        * @param aIndex The index of the preset in the ListBox. This is set
        * to a the index in the ListBox if preset creation is successful. 
        * If the creation is unscuccessful, then the parameter is not changed.
        * @param aPreset The name of the preset created
        */
        void CreateNewEqrPresetL(TInt& aIndex, TPresetName& aPreset) const;
 
        /**
        * This function is used for the renaming of a preset
        * @since 3.0
        * @param aIndex The index of the preset in the ListBox
        * @param aPresetName The name of the preset
        */
        void RenameEqrPresetL(const TInt aIndex, const TDesC& aPreset); 

        /**
        * This function is used for the deletion of a preset
        * @since 3.0
        * @param aIndex The index of the preset in the ListBox
        * @param aPresetName The name of the preset
        */
        void DeleteEqrPresetL(const TInt aIndex, const TDesC& aPreset);
        
        /**
        * This function is used to display a dialog to query the user
        * for a new name. It is used while creating a new preset or 
        * renaming an existing preset.
        * @since 3.0
        * @param aResourceId The resource id to use for the dialog
        * @param aPresetName 
        * @return       ETrue if the user did not cancel the dialog box 
        *               EFalse otherwise 
        */
        TBool GetNewNameL(const TInt aResourceId, TDes& aPresetName) const; 
 
        /**
        * This function is used to load the ListBox icons in an array
        * @since 3.0
        * @returns   array of ListBox icons 
        */
        CArrayPtr<CGulIcon>* GetListBoxIconsL() const;
        
        /**
        * This function is used to inform the client application about the 
        * currently active preset 
        * engine 
        * @since 3.0
        */
        void SetCurrentPreset(const TPresetName& aPreset);

        /**
        * This function is used to rturn the currently active preset 
        * engine 
        * @since 3.0
        */        
        TPresetName CurrentPreset() const;

        /**
        * This function is used to search for a preset name in the ListBox
        * @since 3.0
        * @param aPreset The preset name to search for 
        * @return It returns the index of the preset, if found and 
        *  KErrNotFound if not found.
        */
        TInt SearchPresetL(const TPresetName& aPreset) const;

        /**
        * This function is used to display a dialog to the user for 
        * editing a particular preset.
        * @since 3.0
        * @param aAudEqUtility pointer to CAudioEqualizerUtility
        * @param aIndex the index of the preset in the array of presets 
        * @param aIsActive whether the preset is active or not? 
        * returned by CAudioEqualizerUtility::Presets() 
        */
        void DisplaySettingsL(CAudioEqualizerUtility* aAudEqUtility, 
        const TInt aIndex, const TBool aIsActive);
        
        /**
        * This function is used to display a note to the user if 
        * memory is below critical level when creating a new preset.
        * @since 3.1
        */
        void ShowDiskFullNoteL() const;
    
    public: 
        /**
        * This function is used to execute the Dialog.
        * @since 3.0
        * @returns return value of CAknDialog::ExecuteLD()   
        */
        IMPORT_C TInt ExecuteLD();
        
        /**
        * This function is used to set new values for CAudioEqualizerUtility* 
        * @since 3.0
        * @param aAudEqUtility New pointer to CAudioEqualizerUtility
        */
        IMPORT_C void SetAudioEqualizerL(CAudioEqualizerUtility* aAudEqUtility);

    private: //Functions from base classes
    
        /**
        * From CAknDialog
        * This function responds to the size changes to sets the 
        * size and position of the 
        * contents of this control.
        */
        void SizeChanged();
    
        /**
        * From CAknDialog
        * Handles changes to the application 
        * when it switches to or from the foreground.
        */
        void HandleForegroundEventL( TBool aForeground);
        
        /**
        * From CAknDialog
        * Responds to a change in focus.
        * This is called whenever the control gains or loses focus
        */
        void FocusChanged(TDrawNow aDrawNow);
        
        /**
        * From CCoeControl
        * This function is called when there is a Skin change event
        * or layout change event
        */     
        void HandleResourceChangeL(TInt aType);
        
        /**
        * This function is called when there is a Skin change event
        * or layout change event, it calls HandleResourceChangeL()
        */     
        void HandleResourceChange(TInt aType);
        
        /**
        * From CAknDialog Called when a key event happens.
        */
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, 
            TEventCode aType ); 
        /**
        * From CAknDialog Called just before the dialog is displayed.
        */
        void PreLayoutDynInitL(void); 
        /**
        * From CAknDialog Called to process a user's menu command.
        */
        void ProcessCommandL(TInt  aCommandId); 
        /**
        * From CAknDialog Called just before the menu pane is displayed
        */
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
        /**
        * From CAknDialog Called when OK key is pressed
        */
        TBool OkToExitL(TInt aButtonId); 
                       
        /**
        * Get help context for the control.
        * @param aContext The context that is filled in.
        * @return None.
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

#ifdef RD_SCALABLE_UI_V2
        /**
        * From MEikListBoxObserver
        * Handles listbox events.
        * @param aListBox Listbox where the event occurred.
        * @param aEventType Event type.
        *
        */
        void HandleListBoxEventL( CEikListBox* aListBox,
                                  TListBoxEvent aEventType );
#endif //RD_SCALABLE_UI_V2

    private:
        // Used to store whether the "OK" key has been pressed or not
        // and accordingly invoke appropriate code in DynInitMenuPaneL().
        // It is set to ETrue in OkToExitL() when the key pressed is "OK"
        // and set to EFalse in DynInitMenuPaneL() after the "Context
        // Sensitive" menu handling has been done.
        TBool iContextSensitiveMenu;
        // Used to store the offset of the resource file
        TInt iResourceFileOffset;
        // The ListBox control
        CAknSingleGraphicStyleListBox* iListBox;
        // Name of currently active preset
        TPresetName iActivePreset;                
        // Application title text
        HBufC* iAppTitleText; 
        // Preset "None" Text
        HBufC* iPresetNoneText;
        // Equalizer's title text
        HBufC* iTitleText;    
        // Used to store a pointer to CAudioEqualizerUtility
        CAudioEqualizerUtility* iAudEqUtility;
        // Used to store a reference to the observer class
        MEqualizerPresetActivationObserver& iEqrPresetActObserver;
        // ListBox Item Array
        CEqualizerPresetListArray* iItemArray;
        // Ignore Key events between track changes
        TBool iIgnoreKeyEvents;
        //Editing dialog
        CEqualizerEditingDialog *iEditingDialog;
        CRepository* iRepository;
		// Status pane layout before launching this dialog
		TInt iPreviousStatusPaneLayout;
    };

#endif  // EQUALIZERPRESETSDIALOG_H

// End of File
