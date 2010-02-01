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
* Description:    Implementation of CEqualizerPresetsDialog class
*
*/




// INCLUDE FILES   
#include <StringLoader.h>
#include <akntitle.h>  
#include <eikenv.h>
#include <bldvariant.hrh>
#include <aknnotewrappers.h>
#include <bautils.h>
#include <aknenv.h> 
#include <aknlists.h>
#include <data_caging_path_literals.hrh>
#include <eikclbd.h>
#include <AknIconArray.h> 
#include <avkon.hrh>
//LAF and Skin changes
#include <AknsUtils.h>
#include <e32base.h>
#include <AknsConstants.h>
#include <aknlayout.cdl.h>
#include <featmgr.h>
#include <hlplch.h>
#include <AudioEqualizerUtility.h>
#include <sysutil.h>
#include <AknGlobalNote.h>
#include <aknclearer.h>
#include <layoutmetadata.cdl.h>

#include <csxhelp/eq.hlp.hrh>
#include <Equalizer.rsg>  // Contains the resource definition
#include <equalizer.mbg>
#include "EqualizerPrivateConstants.h"
#include "EqualizerPresetsDialog.h" 
#include "EqualizerPresetListArray.h" 
#include "EqualizerEditingDialog.h" 
#include "Equalizer.hrh"

// CONSTANTS
//Used during building a new preset name   
_LIT( KEqrFormat, "%d)" );
_LIT( KEqrZero, "0" );
_LIT( KEqrParenthesis, "(" );  

// Used for checking if the new preset name contains illegal characters
_LIT(KEqrLessThan, "<");
_LIT(KEqrGreaterThan, ">");
_LIT(KEqrQuote, "\"");
_LIT(KEqrBackSlash, "\\");
_LIT(KEqrForwardSlash, "/");
_LIT(KEqrOrSymbol, "|");
_LIT(KEqrColon, ":");
_LIT(KEqrAsterisk, "*");
_LIT(KEqrQuestionMark, "?");
  
// ================= MEMBER FUNCTIONS =======================
 
// -------------------------------------------------------------------
// CEqualizerPresetsDialog* CEqualizerPresetsDialog::NewL()
// Create instance of concrete ECOM interface implementation
// -------------------------------------------------------------------
// 
EXPORT_C CEqualizerPresetsDialog* CEqualizerPresetsDialog::NewL(
       CAudioEqualizerUtility* aAudEqUtility,
       TInt aCurrentActivePreset, 
       MEqualizerPresetActivationObserver& aEqrPresetActObserver)
    {
    CEqualizerPresetsDialog* self = new (ELeave) CEqualizerPresetsDialog(
        aAudEqUtility, aEqrPresetActObserver);
    CleanupStack::PushL(self); 
    self->ConstructL(aCurrentActivePreset);
    CleanupStack::Pop(self); 
    return self;
    }

// -------------------------------------------------------------------
// CEqualizerPresetsDialog::CEqualizerPresetsDialog(
//       CAudioEqualizerUtility*& aAudEqUtility,
//       MEqualizerPresetActivationObserver& aEqrPresetActObserver)
// First Phase Construction
// -------------------------------------------------------------------
// 
CEqualizerPresetsDialog::CEqualizerPresetsDialog(
       CAudioEqualizerUtility* aAudEqUtility,
       MEqualizerPresetActivationObserver& aEqrPresetActObserver):
       
       iAudEqUtility(aAudEqUtility),
       iEqrPresetActObserver(aEqrPresetActObserver)
    {
    }

// -------------------------------------------------------------------
// CEqualizerPresetsDialog::ConstructL()
// Second Phase Constructor
// -------------------------------------------------------------------
// 
void CEqualizerPresetsDialog::ConstructL(const TInt aCurrentActivePreset)
    {
    //Initially set these variable to EFalse
    iContextSensitiveMenu = EFalse;
    iIgnoreKeyEvents = EFalse;
    
    iRepository = CRepository::NewL(KCRUidMusicPlayerSettings);

    //Add the resource file

    // Find which drive this DLL is installed.
    TFileName fileName;
    // Get the full resource file
    TParse fp;
    // Add the resource dir
    fp.Set( KEqualizerResourceFileName, &KDC_APP_RESOURCE_DIR, NULL);
    // Get the filename with full path
    fileName = fp.FullName(); 
    //for localisation of resource file
    BaflUtils::NearestLanguageFile( iEikonEnv->FsSession(), fileName ); 
    iResourceFileOffset = iEikonEnv->AddResourceFileL( fileName );

    //Read text for preset "None" from rss file
    iPresetNoneText = StringLoader::LoadL(R_QTN_EQUALIZER_PRESET_NONE);
    iTitleText = StringLoader::LoadL(R_QTN_EQUALIZER_TITLE_TEXT);

    iItemArray = CEqualizerPresetListArray::NewL(iAudEqUtility, 
                                     aCurrentActivePreset);

    iItemArray->GetCurrentActivePreset(iActivePreset);

    CAknDialog::ConstructL(R_EQUALIZER_CONTAINER_MENUBAR);
    }

// -------------------------------------------------------------------
// CEqualizerPresetsDialog::~CEqualizerPresetsDialog()
// Destructor
// --------------------------------------------------------------------
//  
CEqualizerPresetsDialog::~CEqualizerPresetsDialog()
    {
    if (iResourceFileOffset)
        {
        iEikonEnv->DeleteResourceFile(iResourceFileOffset); 
        }
    delete iRepository;
    if (iAppTitleText)
        {
        // restore musicplayer's title
        CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
        CAknTitlePane* tp=(CAknTitlePane*)sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle));   
        tp->SetTextL(*iAppTitleText);        
	    delete iAppTitleText;
        }
    delete iPresetNoneText;
    delete iTitleText;
    delete iItemArray;
    }

// --------------------------------------------------------------------------
// void CEqualizerPresetsDialog::BuildPresetNameL( TDes& aName) const
// Returns free folder name
// --------------------------------------------------------------------------
//
void CEqualizerPresetsDialog::BuildPresetNameL( TDes& aName) const
    {
    //set name in the query box to "Preset"
    HBufC* defaultText = StringLoader::LoadLC(
        R_QTN_EQ_PRESET_DEFAULT_NAME);    

    TInt n(0);

    // Numbers start from 1
    TBool found( EFalse );
    TBuf<KMaxPresetNameLength> number;

    // The loop is iterated until a free folder name is found.
    FOREVER
        {
        found = EFalse;
        // Constructs a candidate for the name
        TBuf<KMaxPresetNameLength> newName;
        newName.Copy( *defaultText );

        if( n > 0 )
            // First new is without number ( New folder, New folder(01), New folder(02), ...)
            {
            // Creates the current number.
            number.Format( KEqrFormat, n );
            AknTextUtils::LanguageSpecificNumberConversion ( number );

            TInt leadingZeroLength( 0 );
            if( n < 10 )
                {
                leadingZeroLength = KEqrZero().Length();
                }

            // checks the total length of the costructed string
            if( ( number.Length() + newName.Length() + KEqrParenthesis().Length()
                  + leadingZeroLength ) > KMaxPresetNameLength )
                {
                User::Leave( KErrOverflow );
                }

            // Appends the first parenthesis
            newName.Append( KEqrParenthesis );
            if( n < 10 )
                {
                // Appends the leading zero ( always two digits 08, 09, 10, 11,...)
                 TBuf< 16 > zero(KEqrZero);
        		AknTextUtils::DisplayTextLanguageSpecificNumberConversion( zero );
                newName.Append( KEqrZero );
                }
            newName.Append( number );
            }
        found = SearchPresetL(newName);
        
        if( found == KErrNotFound )
            {
            aName = newName;
            CleanupStack::PopAndDestroy( defaultText); 
            return;
            }
        n++;
        }   
    }

// -----------------------------------------------------------------------
// CEqualizerPresetsDialog::GetNewName(TInt aResourceId, TDes& presetName)
//
// This function is called to display the dialog box to get a new name
// during "Raname" or "Create new"
// -----------------------------------------------------------------------
// 
TBool CEqualizerPresetsDialog::GetNewNameL(const TInt aResourceId, TDes& 
aPresetName) const
    {
    CAknTextQueryDialog* dlg;
    TBool ret = EFalse;
    
    while (1)
        {
        dlg = CAknTextQueryDialog::NewL(aPresetName); 
        if (dlg->ExecuteLD(aResourceId))
            {
            // Illegal characters in a preset name are < > \" \\ / | : * ?"

            if ( (aPresetName.FindC( KEqrLessThan ) != KErrNotFound ) ||
                 (aPresetName.FindC( KEqrGreaterThan ) != KErrNotFound ) ||
                 (aPresetName.FindC( KEqrQuote ) != KErrNotFound ) ||
                 (aPresetName.FindC( KEqrBackSlash ) != KErrNotFound ) ||                 
                 (aPresetName.FindC( KEqrForwardSlash ) != KErrNotFound ) ||
                 (aPresetName.FindC( KEqrOrSymbol ) != KErrNotFound ) ||
                 (aPresetName.FindC( KEqrColon ) != KErrNotFound ) ||                 
                 (aPresetName.FindC( KEqrAsterisk ) != KErrNotFound ) ||
                 (aPresetName.FindC( KEqrQuestionMark ) != KErrNotFound )             
               )
                {
                HBufC* text = StringLoader::LoadLC(
                    R_QTN_FLDR_ILLEGAL_CHARACTERS);
                CAknInformationNote* note = new( ELeave ) CAknInformationNote(ETrue);
                note->ExecuteLD( *text );
                CleanupStack::PopAndDestroy(text);
                continue;
                }
            
            TInt error = SearchPresetL(aPresetName);    
            if (error != KErrNotFound)
                {
                // display error
                HBufC* label = StringLoader::LoadLC(
                    R_QTN_FLDR_NAME_ALREADY_USED, aPresetName );
                CAknInformationNote* note = new( ELeave )
                                         CAknInformationNote( ETrue );
                note->ExecuteLD( *label );
                CleanupStack::PopAndDestroy(label);
                continue;
                }
            // New name entered
            ret = ETrue;
            }
        // Break, in case user Cancels the data query or 
        // enters a valid name 
        break;
        }    
    return ret;
    }
 
// ---------------------------------------------------------------------------
// TPresetName CEqualizerPresetsDialog::GetHighlightedPresetInListBox() const
// This function returns the highlighted Preset in ListBox
// ---------------------------------------------------------------------------
//  
TPresetName CEqualizerPresetsDialog::GetHighlightedPresetInListBox() const
    {
    TInt selectedIndex = iListBox->CurrentItemIndex();
    CTextListBoxModel* model = iListBox->Model();
    TPresetName selectedItem(model->ItemText(selectedIndex));
    TPresetName highlightedPreset = 
             selectedItem.Right(selectedItem.Length()- iItemArray->GetPrefixLength());
    return highlightedPreset;  
    }

// ---------------------------------------------------------------------------
//void CEqualizerPresetsDialog::ActivateEqrPresetL(const TInt  aIndex, 
//const TDesC& aPreset)  const
//
//This function is used for the activation of a preset
// ---------------------------------------------------------------------------
//
void CEqualizerPresetsDialog::ActivateEqrPresetL(const TInt aIndex, 
const TDesC& aPreset) 
    {
    TInt err;
    // Store activated preset in central repository    
    SetCurrentPreset(aPreset);
    
    // Set the active preset in the equalizer item array
    iItemArray->SetCurrentActivePreset(aPreset);
    
    // Display Confirmation note                   
    HBufC* activateNoteText = StringLoader::LoadLC(
    R_QTN_EQ_NOTE_INFO_PRESET_ACTIVATED, aPreset);
    CAknConfirmationNote* dialog = new(ELeave)CAknConfirmationNote();
    dialog->ExecuteLD(*activateNoteText);
    CleanupStack::PopAndDestroy(activateNoteText); 
    
    // Inform the engine
    if (aPreset.Compare(*iPresetNoneText) == 0)            
        {
        TRAP(err, iAudEqUtility->DisableEqualizerL());                
        }
    else
        {
        TRAP(err, iAudEqUtility->ApplyPresetL(GetEngineIndexL(aPreset)));
        }  
   
   if(!err)
	   {
	   iListBox->HandleItemAdditionL();
	   iListBox->SetCurrentItemIndexAndDraw(aIndex);
	   // To prevent two tickmarks from appearing sometimes
	   iListBox->DrawNow(); 
	   }
   }

// ---------------------------------------------------------------------------
//void CEqualizerPresetsDialog::RenameEqrPresetL(const TInt  aIndex, 
//const TDesC& aPreset)  const
//
//This function is used for the renaming of a preset
// ---------------------------------------------------------------------------
//
void CEqualizerPresetsDialog::RenameEqrPresetL(const TInt /*aIndex*/, 
const TDesC& aPreset) 
    {
    //Check if this is an active preset
    TPresetName activePresetName = CurrentPreset();
    if (aPreset.Compare(activePresetName) == 0)            
       {
        //Display Confirmation note                 
        HBufC* renameText = StringLoader::LoadLC(
        R_QTN_EQ_NOTE_ERR_PRESET_IN_USE_NO_RENAME);               
        CAknErrorNote* dialog = new(ELeave)CAknErrorNote();         
        dialog->ExecuteLD(*renameText);
        CleanupStack::PopAndDestroy(renameText);
        } 
    else
        {
        TPresetName presetName;
        presetName =  aPreset;
        TBool nameEnteredIsFine = 
        GetNewNameL(R_DIALOG_TEXT_RENAME_ITEM_NAME, presetName); 
          
        if (nameEnteredIsFine)
            {
            TInt engineIndex = GetEngineIndexL(aPreset);
            //Update Engine
            iAudEqUtility->GetPresetL(engineIndex);
            CAudioEqualizer& audioEqualizer = iAudEqUtility->Equalizer();
                                           
            iAudEqUtility->ModifyPresetL(engineIndex,presetName,
             audioEqualizer);
              
            //Update listbox
            // Sort the array of user-defined presets
            iItemArray->UpdateSortedArrayL(); 
            iListBox->SetCurrentItemIndex(SearchPresetL(presetName));
            iListBox->DrawNow();
            }
        }
   }

// ---------------------------------------------------------------------------
// void CEqualizerPresetsDialog::EditEqrPresetL(const TInt  aIndex, 
// const TDesC& aPreset)  const
//
// This function is used for the editing of a preset
// ---------------------------------------------------------------------------
//
void CEqualizerPresetsDialog::EditEqrPresetL(const TInt  /*aIndex*/, 
const TDesC& aPreset) 
    {
    //Set application name in the title pane to preset name.
    CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
    CAknTitlePane* tp=(CAknTitlePane*)sp->ControlL(TUid::Uid(
    EEikStatusPaneUidTitle));
    tp->SetTextL(aPreset);
    TInt engineIndex = GetEngineIndexL(aPreset);
    //Update Engine
    TBool isActivePreset = (aPreset==CurrentPreset());
    
    DisplaySettingsL(iAudEqUtility, engineIndex, isActivePreset);
    }

// ---------------------------------------------------------------------------
// void CEqualizerPresetsDialog::CreateNewEqrPresetL(TInt&  aIndex, 
// TPresetName& aPreset) const
//
// This function is used for the creation of a new preset.
// @param aIndex The index of the preset in the ListBox. This is set
// to a the index in the ListBox if preset creation is successful. 
// If the creation is unscuccessful, then the parameter is not changed.
// @param aPreset The name of the preset created
// ---------------------------------------------------------------------------
//
void CEqualizerPresetsDialog::CreateNewEqrPresetL(TInt&  aIndex, 
TPresetName& aPreset) const
    {
    RFs fs; 
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL<RFs>( fs );
       
    if (SysUtil::FFSSpaceBelowCriticalLevelL(&fs,0))
        {
        ShowDiskFullNoteL(); // Phone memory full, show note
        }
    else
        {
        TPresetName presetName;
        BuildPresetNameL(presetName);
 
        TBool nameEnteredIsFine = GetNewNameL(R_DIALOG_TEXT_NEW_ITEM_NAME,
         presetName);

        if (nameEnteredIsFine)
            {
            //Update Engine
            CAudioEqualizer& audioEqualizer = iAudEqUtility->Equalizer();
            iAudEqUtility->CreatePresetL(presetName, audioEqualizer);
        
            //Set the level of all bands to 0.
            TInt numFreqBand = audioEqualizer.NumberOfBands();
            for (TInt i = 0; i < numFreqBand; i++)
                {
                audioEqualizer.SetBandLevelL(i+1, 0);
                }

            iItemArray->UpdateSortedArrayL();            
            //Update AudioEqualizerUtility object, setting the level of all
            // bands to 0 
            iAudEqUtility->ModifyPresetL(GetEngineIndexL(presetName),
                                 presetName, audioEqualizer);
        
            //Set the focus to the new preset 
            iListBox->HandleItemAdditionL();
            iListBox->SetCurrentItemIndex(SearchPresetL(presetName));                 
            iListBox->DrawNow();
    
            aPreset = presetName;
            aIndex = iItemArray->MdcaCount()-1;  
	          }
        }
        CleanupStack::PopAndDestroy();
    }
    
// ---------------------------------------------------------------------------
//void CEqualizerPresetsDialog::DeleteEqrPresetL(const TInt  aIndex, 
//const TDesC& aPreset)  const
//
//This function is used for the deletion of a preset
// ---------------------------------------------------------------------------
//
void CEqualizerPresetsDialog::DeleteEqrPresetL(const TInt aIndex,
const TDesC& aPreset)
    {
    //Check if this is an active preset
    TPresetName activePresetName = CurrentPreset();
    if (aPreset.Compare(activePresetName) == 0)     
        {
        //Display error note                   
        HBufC* renameText =
         StringLoader::LoadLC(R_QTN_EQ_NOTE_ERR_PRESET_IN_USE);
        CAknErrorNote* dialog = new(ELeave)CAknErrorNote();
        dialog->ExecuteLD(*renameText);
        CleanupStack::PopAndDestroy(renameText);
        } 
    else
        {
        //Delete?\n%U" §qtn.query.common.conf.delete?
        HBufC* text = 
        StringLoader::LoadLC( R_QTN_QUERY_COMMON_CONF_DELETE, 
        aPreset);

        // Show confirmation note
        CAknQueryDialog* dlg = CAknQueryDialog::NewL();
        TInt result( 
        dlg->ExecuteLD( R_EQUALIZER_CONFIRMATION_QUERY, *text ) );
        CleanupStack::PopAndDestroy(text); // text

        if ( result )
            {
            //Update Engine
            iAudEqUtility->DeletePresetL(GetEngineIndexL(aPreset));
            //Remove from listbox
            iItemArray->UpdateSortedArrayL();
            iListBox->HandleItemRemovalL();
            // This is to avoid the gap observed when the last entry present
            // in the Preset List is deleted
            iListBox->SetCurrentItemIndexAndDraw(0);
            //If we are at the last preset set focus to the next preset
            // else the previous preset
            if (aIndex == iItemArray->MdcaCount())
                {
                iListBox->SetCurrentItemIndexAndDraw(aIndex - 1);
                }
            else
                {
                iListBox->SetCurrentItemIndexAndDraw(aIndex);
                }
            iListBox->DrawNow();
            }
        }
    }

// -------------------------------------------------------------------
// TInt CEqualizerPresetsDialog::GetEngineIndexL
// (const TPresetName& aPresetName)
//
// This function is called to find the index of a preset in the list of
// presets returned by CAudioEqualizerUtility::Presets()
// --------------------------------------------------------------------
// 
TInt CEqualizerPresetsDialog::GetEngineIndexL(const TPresetName& aPresetName) const  
    {
    TInt engineIndex = KErrNotFound;
    TInt count;
        
    TArray<TEfAudioEqualizerUtilityPreset> presets = iAudEqUtility->Presets();
    TInt numPredefinedPresets = iAudEqUtility->NumberOfPreDefinedPresets();
    TInt numPresets = presets.Count();
    for (count = 0; count < numPresets; count++)
        {
        TPresetName selectedItem = iItemArray->MdcaPoint(count+1);
        TPresetName presetName = selectedItem.Right(
                            selectedItem.Length() - iItemArray->GetPrefixLength());

        
        if (((count < numPredefinedPresets) && (presetName == aPresetName))
            || (presets[count].iPresetName == aPresetName))
            {
            engineIndex = count;
            }
        }
   if (engineIndex == KErrNotFound)
        {
        // Leave if this function is called for preset "None" or a preset
        // which is there in the ListBox but not in the Engine.
        User::Leave(KErrArgument);
        }
        
    return engineIndex;
    }
    
// -----------------------------------------------------------------------
// CEqualizerPresetsDialog::DisplaySettingsL
// (CAudioEqualizerUtility *aAudEqUtility, TInt aIndex,
// const TBool aIsActivePreset)) const
//
// This function is used to display the current equalizer settings
// by creating a new dialog. The dialog can also be used for editing, etc.
// of the settings.
// ------------------------------------------------------------------------
//  
void CEqualizerPresetsDialog::DisplaySettingsL(CAudioEqualizerUtility*
 aAudEqUtility, const TInt aIndex, const TBool aIsActivePreset)
    {
    TRect rect = Rect();
    
    TInt item;
    TPresetName preset = GetHighlightedPresetInListBox(); 
    iItemArray->GetPresetKeyFromLocalisedName(item, preset);
    iEditingDialog = CEqualizerEditingDialog::NewL(aAudEqUtility, 
      aIndex, rect, aIsActivePreset,iEqrPresetActObserver, item);
    iEditingDialog->ExecuteLD(); 
    iEditingDialog = NULL;    
    }  

// --------------------------------------------------------------
// CEqualizerPresetsDialog::CurrentPreset()
// This function is used to read the currently active preset from
// the central repository
// --------------------------------------------------------------
//    
TPresetName CEqualizerPresetsDialog::CurrentPreset() const
    {
    TInt res = 0;
    TPresetName preset;
    User::LeaveIfError(iRepository->Get(KMPlayerEqPresetId, res));
    iItemArray->GetLocalisedPresetNameFromKey(res,preset );
    return preset;   
    }

// ----------------------------------------------------------------
// CEqualizerPresetsDialog::SetCurrentPreset(const TPtrC& aPreset)
// This function is used to store the currently active preset into
// the central repository
// ----------------------------------------------------------------
//     
void CEqualizerPresetsDialog::SetCurrentPreset(const TPresetName& aPreset)
    {
    TInt activePresetKey;
    iActivePreset = aPreset;
    iItemArray->GetPresetKeyFromLocalisedName(activePresetKey, aPreset);
    iEqrPresetActObserver.HandlePresetActivation(activePresetKey);
    }

// ----------------------------------------------------------------------------
// CEqualizerPresetsDialog::SearchPresetL(const TPresetName& aPreset) const
// This function is used to search for a preset name in the ListBox.
// It return the index of the preset, if found and KErrNotFound if not found.
// ----------------------------------------------------------------------------
//       
TInt CEqualizerPresetsDialog::SearchPresetL(const TPresetName& aPreset) const
    {
    TInt ret = KErrNotFound;
    iItemArray->UpdateSortedArrayL();

    for (TInt i=0; i<iItemArray->MdcaCount(); i++)
        {
        TPresetName selectedItem = iItemArray->MdcaPoint(i);
        TPresetName presetName = selectedItem.Right(selectedItem.Length() - iItemArray->GetPrefixLength());

        if (aPreset == presetName)      
            {
            ret = i;
            break;
            }
        }
    return ret;
    }

// ----------------------------------------------------------------------------
// CEqualizerPresetsDialog::SetAudioEqualizerL(CAudioEqualizerUtility* 
// aAudEqUtility)
// This function is used to set a new CAudioEqualizerUtility* 
// ----------------------------------------------------------------------------
//    
EXPORT_C void CEqualizerPresetsDialog::SetAudioEqualizerL(
CAudioEqualizerUtility* aAudEqUtility)
    {
    iAudEqUtility=aAudEqUtility;
    if (iItemArray)
        {
        iItemArray->SetAudioEqualizer(iAudEqUtility);
        }	

    if (iEditingDialog)
        {
        iEditingDialog->SetAudioEqualizerL(iAudEqUtility);
        }

        
    if (iAudEqUtility)
        {
        iIgnoreKeyEvents = EFalse;
        }
    else
        {
        iIgnoreKeyEvents = ETrue; 
        }
    }
    
// ----------------------------------------------------------------------
// CEqualizerPresets::GetListBoxIconsL()
// This function is used to load the lisitbox icons into an array
// ----------------------------------------------------------------------
//
CArrayPtr<CGulIcon>* CEqualizerPresetsDialog::GetListBoxIconsL() const
    {
    // Get the icon array and deleted if already exists. Memory leak error.
    CArrayPtr<CGulIcon>* previcons = iListBox->ItemDrawer()->ColumnData()->IconArray();
    if( previcons )
    	{
    	previcons->ResetAndDestroy();
    	delete previcons;
    	previcons = NULL;
    	}
    
    // CGulIcon class packages two bitmaps: icon image and its mask
    // CAknIconArray inherits from CArrayPtrFlat
    CArrayPtr<CGulIcon>* iconList = new (ELeave) 
                                 CAknIconArray(KEqualizerArrayGranularity);
    CleanupStack::PushL(iconList);
    
    // Find which drive this DLL is installed.
    TFileName fileName;
    // Get the full resource file
    TParse fp;
    // Add the resource dir
    fp.Set( KIconsFilename, &KDC_APP_BITMAP_DIR, NULL);

    // Get the filename with full path
    fileName = fp.FullName(); 
     
    MAknsSkinInstance* skinInstance;
    CFbsBitmap* newIconBmp = NULL;
    CFbsBitmap* newIconBmpMask = NULL;
    CGulIcon* newIcon = NULL;
    
    skinInstance = AknsUtils::SkinInstance();
    
    //creating blank icon
    AknsUtils::CreateIconLC(skinInstance,KAknsIIDQgnPropEmpty,
                           newIconBmp,newIconBmpMask,fileName,
                           EMbmEqualizerEmpty13x13,
                           EMbmEqualizerEmpty13x13_mask);
    
    newIcon = CGulIcon::NewL(newIconBmp,newIconBmpMask);
    
    CleanupStack::Pop(2);
    CleanupStack::PushL(newIcon);
    iconList->AppendL(newIcon);
    
    CleanupStack::Pop();
    
    //creating TICK mark icon
    AknsUtils::CreateIconLC(skinInstance,KAknsIIDQgnPropPslnActive,
                           newIconBmp,newIconBmpMask,fileName,
                           EMbmEqualizerQgn_prop_psln_active,
                           EMbmEqualizerQgn_prop_psln_active_mask);


    newIcon = CGulIcon::NewL(newIconBmp,newIconBmpMask);
    
    CleanupStack::Pop(2);
    CleanupStack::PushL(newIcon);
    iconList->AppendL(newIcon);

    CleanupStack::Pop();
    
    CleanupStack::Pop(iconList);

    newIconBmp = NULL;
    newIconBmpMask = NULL;
    newIcon = NULL;

    return iconList;
    }    
    
// -------------------------------------------------------------------
// CEqualizerPresetsDialog::ExecuteLD() 
//
// This function is called to create a dialog
// to display the preset list view of the equalizer
// --------------------------------------------------------------------
//  
EXPORT_C TInt CEqualizerPresetsDialog::ExecuteLD()
    {
    TInt ret = CAknDialog::ExecuteLD(R_EQUALIZER_DIALOG);
    return ret;
    }

// ---------------------------------------------------------------------------
// void CEqualizerPresetsDialog::PreLayoutDynInitL(void)
// This function is called just before the dialog is displayed
// ---------------------------------------------------------------------------
// 
void CEqualizerPresetsDialog::PreLayoutDynInitL(void)
    {
     
    CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
    CAknTitlePane* tp=(CAknTitlePane*)sp->ControlL(TUid::Uid(
    EEikStatusPaneUidTitle));
    // Store the Application's title text
    iAppTitleText =  tp->Text()->AllocL(); 
       
    // Set preset name in the title pane
    tp->SetTextL(*iTitleText);

    iPreviousStatusPaneLayout = sp->CurrentLayoutResId();
    
    if ( sp )
        {
        if ( R_AVKON_STATUS_PANE_LAYOUT_USUAL != iPreviousStatusPaneLayout )
            {
            sp->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL );
            }
        }
    iListBox= (CAknSingleGraphicStyleListBox*)Control(ESelectionListControl);
    iListBox->ItemDrawer()->ColumnData()->SetIconArray(GetListBoxIconsL());
    
    // Set item array to listbox model
    iListBox->Model()->SetItemTextArray(iItemArray);
    // Item array is not owned by ListBox so it has to be freed. 
    iListBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);        
    
    //
    // Create a scrollbar that will be displayed automatically
    // if all the items cannot be displayed on screen
    //
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
    CEikScrollBarFrame::EOff,CEikScrollBarFrame::EAuto);
    // Set focus to the currently active preset
    
#ifdef RD_SCALABLE_UI_V2
    iListBox->SetListBoxObserver(this);
#endif //RD_SCALABLE_UI_V2
    TPresetName currentPreset = CurrentPreset();
    
    TInt cur = SearchPresetL(currentPreset);
    if (cur == KErrNotFound) //Some preset should always be active
        {
        User::Leave(KErrNotSupported);
        }  
    iListBox->SetCurrentItemIndexAndDraw(cur);     
    
    }

// -------------------------------------------------------------------
// CEqualizerPresetsDialog::OkToExitL(TInt aButtonId)
//
// This function is called when the user presses the selection key
// for displaying context sesitive menu
// --------------------------------------------------------------------
// 
TBool CEqualizerPresetsDialog::OkToExitL(TInt aButtonId)
    {
    TBool ret = EFalse;
    if (aButtonId == EEikBidOk)
        {
        iContextSensitiveMenu = ETrue;
        DisplayMenuL();
        }
    else if (aButtonId == EAknSoftkeyBack) 
        {
        CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
        CAknTitlePane* tp=(CAknTitlePane*)sp->ControlL(TUid::Uid(
          EEikStatusPaneUidTitle));
    
        if (iAppTitleText)
            {
             //set application name in the title pane
            tp->SetTextL(*iAppTitleText);        
            }
        ret = CAknDialog::OkToExitL( aButtonId ); 
        }
     else if (aButtonId == EAknSoftkeySelect) 
        {
        const TInt index = iListBox->CurrentItemIndex();
        TPresetName preset = GetHighlightedPresetInListBox();
        ActivateEqrPresetL(index, preset);
        }
    else  
        {
        ret = CAknDialog::OkToExitL( aButtonId );        
        } 

    if ( ret )
        {
        // Restore landscape layout
        CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
        if( Layout_Meta_Data::IsLandscapeOrientation() )
            {
            if ( sp && KErrNone != iPreviousStatusPaneLayout )
                {
                if ( sp->CurrentLayoutResId() != R_AVKON_STATUS_PANE_LAYOUT_USUAL )
                    {
                    sp->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL );
                    }
                }
            }
        else
            {
            CAknLocalScreenClearer* clearer = CAknLocalScreenClearer::NewLC( ETrue );
            sp->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL_FLAT );
            CleanupStack::PopAndDestroy( clearer );
            }
        }
    return ret;
    } 

// -------------------------------------------------------------------
// void CEqualizerPresetsDialog::SizeChanged()
//
// This function is called by the framework whenever there is a change
// in the size of the control
// -------------------------------------------------------------------
//
void CEqualizerPresetsDialog::SizeChanged()
    {
    TRect rect = Rect();
    //Getting the layout for the list box
    AknLayoutUtils::LayoutControl(iListBox, rect, AknLayout::list_gen_pane(1));
    CAknDialog::SizeChanged();
    }

// -------------------------------------------------------------------
// void CEqualizerPresetsDialog::HandleForegroundEventL( TBool aForeground)
//
// Handles changes to the application 
// when it switches to or from the foreground.
// -------------------------------------------------------------------
//     
void CEqualizerPresetsDialog::HandleForegroundEventL( TBool aForeground)
    {
    if (aForeground)
        {
        CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
        CAknTitlePane* tp=(CAknTitlePane*)sp->ControlL(TUid::Uid(
        EEikStatusPaneUidTitle));
        tp->SetTextL(*iTitleText);
        } 
    }
    
// -------------------------------------------------------------------
// void CEqualizerPresetsDialog::FocusChanged(TDrawNow aDrawNow)
//
// Responds to a change in focus.
// This is called whenever the control gains or loses focus
// -------------------------------------------------------------------
//   
void CEqualizerPresetsDialog::FocusChanged(TDrawNow /*aDrawNow*/)
	{
	iItemArray->UpdateSortedArrayL(); 
	TPresetName currentPreset = CurrentPreset();
    iItemArray->SetCurrentActivePreset(currentPreset);

    if(iListBox)
    {
        iListBox->HandleItemAdditionL();
	}
	}
	
// -------------------------------------------------------------------
// void CEqualizerFreqBand::HandleResourceChangeL(TInt aType)
//
// This function is called by the framework whenever there is a 
// change in skin or layout
// -------------------------------------------------------------------
//
void CEqualizerPresetsDialog::HandleResourceChangeL(TInt aType)
    {
    if (aType == KAknsMessageSkinChange)
        {
        //Creating the icons for the new Skin
        iListBox->ItemDrawer()->ColumnData()->SetIconArray(GetListBoxIconsL());
        iListBox->HandleResourceChange(aType);    
        }

     else if (aType == KEikDynamicLayoutVariantSwitch)
        {                
	    TRect rect;
	    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);  	    
	    SetRect(rect);
        }            
       
    }

void CEqualizerPresetsDialog::HandleResourceChange(TInt aType)
	{
	CAknDialog::HandleResourceChange( aType );
	HandleResourceChangeL(aType);
	}

// ---------------------------------------------------------
// CEqualizerPresetsDialog::OfferKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode aType)
// This function is used to handle key events
// ---------------------------------------------------------
//
TKeyResponse CEqualizerPresetsDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    { 
    if (iIgnoreKeyEvents)
        {
        return EKeyWasNotConsumed;
        }
    if (aType == EEventKey)
        {
        if (aKeyEvent.iCode == EKeyBackspace)
            {
            // Find the preset highlighted
            const TInt listBoxIndex = iListBox->CurrentItemIndex();
              
            // pre-defined presets cannot be deleted.
            // preset "None" is not stored in engine.
            if (listBoxIndex > (TInt) 
               (iAudEqUtility->NumberOfPreDefinedPresets()))
                {
                TPresetName preset = GetHighlightedPresetInListBox(); 
                DeleteEqrPresetL(listBoxIndex, preset);
                }
            return EKeyWasConsumed;
            }
        }
    return CAknDialog::OfferKeyEventL( aKeyEvent, aType ); 
    }
    
// ---------------------------------------------------------------------------
// CEqualizerPresetsDialog::DynInitMenuPaneL(
//                              TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ---------------------------------------------------------------------------
//
void CEqualizerPresetsDialog::DynInitMenuPaneL(
                        TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    TPresetName selectedItem = GetHighlightedPresetInListBox(); 
     
    TInt selectedIndex = iListBox->CurrentItemIndex();     
    TInt numberOfPreDefinedPresets = 
    iAudEqUtility->NumberOfPreDefinedPresets();      
    
    if ( aResourceId == R_EQUALIZER_CONTAINER_MENU)
        {
        //Is the menubar invoked from Options menu or selection key?
        if (iContextSensitiveMenu == EFalse)
            {
            // The menubar is invoked from Options menu
	        iMenuBar->SetMenuType(CEikMenuBar::EMenuOptions);
            // The selected preset is "None"
            if (selectedItem.Compare(*iPresetNoneText) == 0)
                {
                aMenuPane->SetItemDimmed(EEqualizerCmdRename, ETrue);
                aMenuPane->SetItemDimmed(EEqualizerCmdDelete, ETrue);
                aMenuPane->SetItemDimmed(EEqualizerCmdEdit, ETrue);
                }
            // The selected preset is a pre defined preset other than "None"
            if (selectedIndex <= numberOfPreDefinedPresets)
                {
                aMenuPane->SetItemDimmed(EEqualizerCmdRename, ETrue);
                aMenuPane->SetItemDimmed(EEqualizerCmdDelete, ETrue);
                }
            // Help should be displayed only if the feature is supported
            // according to Feature Manager
            if (!FeatureManager::FeatureSupported(KFeatureIdHelp))
                {
                aMenuPane->SetItemDimmed(EEqualizerCmdHelp, ETrue);
                }
            }
        else
            {
            // The menubar is invoked from selection key
	        iMenuBar->SetMenuType(CEikMenuBar::EMenuContext);
            // These Options are always hidden
            aMenuPane->SetItemDimmed(EEqualizerCmdDelete, ETrue);
            aMenuPane->SetItemDimmed(EEqualizerCmdRename, ETrue);
            aMenuPane->SetItemDimmed(EEqualizerCmdCreateNew, ETrue);
            aMenuPane->SetItemDimmed(EEqualizerCmdHelp, ETrue);
            aMenuPane->SetItemDimmed(EAknCmdExit, ETrue);
            
            // The selected preset is "None"
            if (selectedItem.Compare(*iPresetNoneText) == 0)            
                {
                aMenuPane->SetItemDimmed(EEqualizerCmdEdit, ETrue);
                }
            iContextSensitiveMenu = EFalse; 
            }
        }
   }

// -------------------------------------------------------------------
// CEqualizerPresetsDialog::ProcessCommandL(TInt  aCommandId ) 
//
// This function is called to process the menubar command in a dialog
// --------------------------------------------------------------------
// 
void CEqualizerPresetsDialog::ProcessCommandL(TInt  aCommandId )
    {
    // Need to do this in all cases or menu does not disappear after
    // a selection is made
    CAknDialog::ProcessCommandL(aCommandId);
    
    // Find the preset highlighted
    const TInt index = iListBox->CurrentItemIndex();
    TPresetName preset = GetHighlightedPresetInListBox(); 
       
    switch(aCommandId)
        {
        case EAknCmdExit :
        case EEikCmdExit :
            {
            iAvkonAppUi->HandleCommandL( aCommandId );
            break;
            }    
        case EEqualizerCmdEdit:
            {
            EditEqrPresetL(index, preset);
            break;
            }
        case EEqualizerCmdActivate:
            {
            ActivateEqrPresetL(index, preset);
            break;
            }
        case EEqualizerCmdCreateNew:
            {
            TInt presetIndex = -1;
            TPresetName presetName; 

            CreateNewEqrPresetL(presetIndex, presetName);
         
            if (presetIndex != -1)
                {
                // A valid preset name has been entered
                EditEqrPresetL(presetIndex, presetName);
                }   
            break;   
            }
 
        case EEqualizerCmdDelete:
            { 
            DeleteEqrPresetL(index, preset); 
            break;
            }

        case EEqualizerCmdRename:
            { 
            RenameEqrPresetL(index, preset);
            break;
            }

        case EEqualizerCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL
                ( iEikonEnv->WsSession(),
                 CEikonEnv::Static()->AppUi()->AppHelpContextL() );
            break;
            }

        default:
            {
            break;
            }
        }
    }
     
// -------------------------------------------------------------------
// void CEqualizerPresetsDialog::GetHelpContext( 
//            TCoeHelpContext& aContext ) const
// Get help context for the control
// --------------------------------------------------------------------
//         
void CEqualizerPresetsDialog::GetHelpContext( 
            TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidEqualizer;
    aContext.iContext = KEQ_HLP_LIST_VIEW;
    }

// -------------------------------------------------------------------
// void CEqualizerPresetsDialog::ShowDiskFullNoteL() 
// This function is used to display a note to the user if           
// memory is below critical level when creating a new preset.
// --------------------------------------------------------------------
//  
void CEqualizerPresetsDialog::ShowDiskFullNoteL() const
    {
    HBufC* message = NULL;
    message = StringLoader::LoadLC(R_QTN_MEMLO_NOT_ENOUGH_MEMORY);
       
    CAknGlobalNote* note = CAknGlobalNote::NewLC();
    note->SetSoftkeys(R_AVKON_SOFTKEYS_OK_EMPTY);
    note->ShowNoteL(/*status,*/ EAknGlobalErrorNote, *message);
    
    CleanupStack::PopAndDestroy(note); // note, message
    CleanupStack::PopAndDestroy(message); // note, message
    }

#ifdef RD_SCALABLE_UI_V2
// -------------------------------------------------------------------
// void CEqualizerPresetsDialog::HandleListBoxEventL 
// This function callback from MEikListBoxObserver when a listbox item           
// is double clicked using a pen input .
// --------------------------------------------------------------------
//  
void CEqualizerPresetsDialog::HandleListBoxEventL(
    CEikListBox* /*aListBox*/,
    TListBoxEvent aEventType )
    {
    switch ( aEventType )
        {
#ifdef SINGLE_CLICK_INCLUDED
        case EEventItemSingleClicked:
#endif
        case EEventItemDoubleClicked:
            {
            const TInt index = iListBox->CurrentItemIndex();
            TPresetName preset = GetHighlightedPresetInListBox();
            ActivateEqrPresetL(index, preset);
            break;
            }
        default:
            break;
        }
    }
#endif //RD_SCALABLE_UI_V2
// End of File 

