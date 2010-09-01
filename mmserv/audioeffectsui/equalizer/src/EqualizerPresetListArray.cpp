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
* Description:    Item text array for Equalizer Preset Listbox.
*
*/




// INCLUDE FILES
#include    <coemain.h>
#include    <aknconsts.h>
#include    <AknsUtils.h>
#include    <AknsConstants.h>
#include    <eikenv.h>
#include    <data_caging_path_literals.hrh>
#include    <StringLoader.h>
#include    <barsread.h>

#include    <Equalizer.rsg>  // Contains the resource definition
#include    "EqualizerPrivateConstants.h"
#include    "EqualizerPresetListArray.h"

// CONSTANTS

const TInt KEqrActiveIcon = 1;
const TInt KEqrNonActiveIcon = 0;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEqualizerPresetListArray::CEqualizerPresetListArray
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEqualizerPresetListArray::CEqualizerPresetListArray (
 CAudioEqualizerUtility*  aAudioEqUtil): 
                               iAudEqUtility(aAudioEqUtil)
    {
    }

// -----------------------------------------------------------------------------
// CEqualizerPresetListArray::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEqualizerPresetListArray::ConstructL(TInt aActivePresetKey)
    {
    //Read text for preset "None" from rss file
    iPresetNoneText = StringLoader::LoadL(R_QTN_EQUALIZER_PRESET_NONE);

    // iItem length cannot be defined dynamically as MdcaPoint is const function
    iItem = HBufC::NewL(KListBoxEntryMaxLength);
    iSortedPresetArray =  new (ELeave) CDesCArrayFlat
                                    (KEqualizerArrayGranularity);  
    UpdateSortedArrayL();
    
    HBufC* tempText = NULL;
    iPreDefinedPresetArray =  new (ELeave) CDesCArrayFlat
                                    (KEqualizerArrayGranularity);
    if (iAudEqUtility) 
        {
        // construct the array of available languages
        TResourceReader reader;
        CEikonEnv::Static()->CreateResourceReaderLC( reader, 
                                    R_EQUALIZER_PREDEFINED_PRESETS );
        CDesCArray* presetStringArray; 
        presetStringArray = reader.ReadDesCArrayL();
        CleanupStack::PopAndDestroy(); // resource reader
        CleanupStack::PushL( presetStringArray );
        
        TArray<TEfAudioEqualizerUtilityPreset> presets =
                                                iAudEqUtility->Presets();
        TInt numPreDefinedPresets =iAudEqUtility->NumberOfPreDefinedPresets();                                        
        for (TInt presetIndex = 0; 
             presetIndex < numPreDefinedPresets; presetIndex++) 
            {
            TArray<TEfAudioEqualizerUtilityPreset> presets =
                                                    iAudEqUtility->Presets();
            TInt numPreDefinedPresets = 
                                iAudEqUtility->NumberOfPreDefinedPresets();                                        
            for (TInt presetIndex = 0; 
                 presetIndex < numPreDefinedPresets; presetIndex++) 
                {
                if ((tempText = SearchForPredefinedPresetL(*presetStringArray, 
                            presets[presetIndex].iPresetName)) == NULL)
                    {
                    // If control reaches here then there is a predefined preset in 
                    // the cenrep whose name is not localised
                    // In case no match is found display the English text
                    tempText = presets[presetIndex].iPresetName.AllocL();
                    }
                CleanupStack::PushL(tempText);
                iPreDefinedPresetArray->AppendL(*tempText);
                CleanupStack::PopAndDestroy(tempText);
                }
			GetLocalisedPresetNameFromKey(aActivePresetKey, iActivePreset);
            }
        CleanupStack::PopAndDestroy(presetStringArray);    
        }
    }
    


// -----------------------------------------------------------------------------
// CEqualizerPresetListArray::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CEqualizerPresetListArray* CEqualizerPresetListArray::NewL(
                               CAudioEqualizerUtility* aAudioEqUtil,
                               TInt aActivePresetKey)
    {
    CEqualizerPresetListArray* self = new(ELeave) CEqualizerPresetListArray
                                           (aAudioEqUtil);                                  
    CleanupStack::PushL(self);
    self->ConstructL(aActivePresetKey);
    CleanupStack::Pop(self); 

    return self;
    }
    
// ---------------------------------------------------------------------------
// CEqualizerPresetListArray::~CEqualizerPresetListArray
// Destructor.
// ---------------------------------------------------------------------------
//
CEqualizerPresetListArray::~CEqualizerPresetListArray()
    {
    delete iPresetNoneText;
    delete iItem;
    if (iSortedPresetArray)
        {
        iSortedPresetArray->Reset();
        delete iSortedPresetArray;       
        }
    if (iPreDefinedPresetArray)
        {
        iPreDefinedPresetArray->Reset();
        delete iPreDefinedPresetArray;         
        }
    }

// ---------------------------------------------------------------------------
// CEqualizerPresetListArray::MdcaCount() const
// Returns the number of descriptor elements in a descriptor array.
// ---------------------------------------------------------------------------
//
TInt CEqualizerPresetListArray::MdcaCount() const
    {
    TInt ret = 0;
    // Check if iAudEqUtility is NULL, if it is NULL then the ListBox
    // is empty.
    if (iAudEqUtility) 
        {
        TArray<TEfAudioEqualizerUtilityPreset> presets =
                                                iAudEqUtility->Presets();
        ret = presets.Count() + 1; //Add the preset "None"
        }
    
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CEqualizerPresetListArray::MdcaPoint
// Indexes into a descriptor array.
// -----------------------------------------------------------------------------
//
TPtrC CEqualizerPresetListArray::MdcaPoint(TInt aIndex) const
    {
    TPtrC item;
    TArray<TEfAudioEqualizerUtilityPreset> presets = iAudEqUtility->Presets();
    
    if (aIndex <= iAudEqUtility->NumberOfPreDefinedPresets())
        { // The predefined presets do not need to be sorted
        if (aIndex == 0)
            {
            CreateListBoxIconText(*iPresetNoneText);
            }
        else
            {
            CreateListBoxIconText((*iPreDefinedPresetArray)[aIndex-1]);
            }
        }
    else
        { // Sort the user defined presets
        CreateListBoxIconText((*iSortedPresetArray)[aIndex -
                         iAudEqUtility->NumberOfPreDefinedPresets()-1]);       
        }
    
    item.Set(*iItem);
    return item;
    
    }
    
// ---------------------------------------------------------------------------
// CEqualizerPresetListArray::UpdateSortedArrayL() 
// Updates the sorted array with the changed user-defined presets
// ---------------------------------------------------------------------------
//    
void CEqualizerPresetListArray::UpdateSortedArrayL()
    {
    TInt presetIndex;
    TInt numPreDefinedPresets = iAudEqUtility->NumberOfPreDefinedPresets();
    TArray<TEfAudioEqualizerUtilityPreset> presets = iAudEqUtility->Presets();
    iSortedPresetArray->Reset();
    for (presetIndex = numPreDefinedPresets; presetIndex < presets.Count(); 
                                                                presetIndex++) 
        {
        TPresetName preset = presets[presetIndex].iPresetName;
        iSortedPresetArray->InsertIsqL(presets[presetIndex].iPresetName, 
                                                                ECmpCollated);
        }
    }

// ---------------------------------------------------------------------------
// CEqualizerPresetListArray::GetLocalisedPresetNameFromKey() 
// Returns the localised name for the preset given the PresetNameKey
// ---------------------------------------------------------------------------
//    
void CEqualizerPresetListArray::GetLocalisedPresetNameFromKey(TInt aPresetNameKey, 
											TPresetName& aPresetName)
    {
    TInt count;

  	//If a junk aCurrentActivePreset is passed to us we activate preset "None"
	aPresetName = *iPresetNoneText;

	if (aPresetNameKey == KEqualizerPresetNone)
		{
		aPresetName = *iPresetNoneText;
		}
	else
		{
		TArray<TEfAudioEqualizerUtilityPreset> presets = iAudEqUtility->Presets();
		TInt numPredefinedPresets = iAudEqUtility->NumberOfPreDefinedPresets();
	    TInt numPresets = presets.Count();
		for (count = 0; count < numPresets; count++)
		    {
			if (presets[count].iPresetNameKey == aPresetNameKey)
				{
				if (count < numPredefinedPresets)
				    {
					TPtrC selectedItem = MdcaPoint(count+1);
                    TInt begin = selectedItem.FindC(KEqrTab);
                    TPtrC localisedPreset = selectedItem.Right
                       (selectedItem.Length() -begin-1);            
                    aPresetName = localisedPreset;
					}
				else
					{
					aPresetName = presets[count].iPresetName;
					}
			    }
			}
		}	
    } 

// ---------------------------------------------------------------------------
// CEqualizerPresetListArray::GetPresetKeyFromLocalisedName() 
// Returns the PresetNameKey given the localised name for the preset
// ---------------------------------------------------------------------------
//    
void CEqualizerPresetListArray::GetPresetKeyFromLocalisedName(TInt& 
            aPresetNameKey, const TPresetName& aPresetName)
    {
    TInt count;

	if (aPresetName == *iPresetNoneText)
		{
		aPresetNameKey = KEqualizerPresetNone;
		}
	else
		{
		TArray<TEfAudioEqualizerUtilityPreset> presets = iAudEqUtility->Presets();
		TInt numPredefinedPresets = iAudEqUtility->NumberOfPreDefinedPresets();
	    TInt numPresets = presets.Count();
		for (count = 0; count < numPresets; count++)
		    {
		    TPtrC selectedItem = MdcaPoint(count+1);
            TInt begin = selectedItem.FindC(KEqrTab);
            TPtrC localisedPreset = selectedItem.Right(selectedItem.Length()
                                        -begin-1);            

			if (((count < numPredefinedPresets) && (localisedPreset == 
			    aPresetName)) || (presets[count].iPresetName == aPresetName))
				{
				aPresetNameKey = presets[count].iPresetNameKey; 
				break;
				}
			}
		}	
    } 

// ---------------------------------------------------------------------------
// HBufC* CEqualizerPresetListArray::SearchForPredefinedPresetL
// (const CDesCArray& aPresetArray, const TPresetName& aCenrepPreset)
// Searches for a localised name for a preset found in the CenRep
// ---------------------------------------------------------------------------
//    
HBufC* CEqualizerPresetListArray::SearchForPredefinedPresetL(const CDesCArray& 
    aPresetArray, const TPresetName& aCenrepPreset)
    {
    HBufC* retLocalisedPreset = NULL;
    for (TInt index =0; index<aPresetArray.Count(); index++)
        {
        TPtrC resourceFileText(aPresetArray[index]);
        TInt begin = resourceFileText.FindC(KEqrTab);
        TPtrC preset(resourceFileText.Left(begin));
        TPtrC localisedPreset(resourceFileText.Right(resourceFileText.Length()
                                -begin-1));
        if (aCenrepPreset == preset)   
            {
            retLocalisedPreset = localisedPreset.AllocL();
            break;
            }
        }
    return retLocalisedPreset;
    }
    
// ---------------------------------------------------------------------------
// CEqualizerPresetListArray::CreateListBoxIconText(const 
//  TPresetName& aPresetName) const 
// Creates the ListBox text including the icon information. e.g "0\tNone"
// ---------------------------------------------------------------------------
//    
void CEqualizerPresetListArray::CreateListBoxIconText(const 
  TPtrC& aPresetName) const
    {
    TPtr ptr = iItem->Des();
    ptr.Zero();
    if (aPresetName == iActivePreset) 
        {
        ptr.AppendNumUC(KEqrActiveIcon);
        }
    else
        {
        ptr.AppendNumUC(KEqrNonActiveIcon);
        }
    ptr.Append(KEqrTab); 
    ptr.Append(aPresetName);
    }
    
// -----------------------------------------------------------------------------
// void CEqualizerPresetListArray::SetCurrentActivePreset(const TDesC&
//                                       aActivePreset)
// Used to set the currently active preset.
// -----------------------------------------------------------------------------
//
void CEqualizerPresetListArray::SetCurrentActivePreset(const TDesC& 
                                       aActivePreset)
    {
    iActivePreset = aActivePreset;
    }

// -----------------------------------------------------------------------------
// TPtrC CEqualizerPresetListArray::GetCurrentActivePreset()
// Used to set the currently active preset.
// -----------------------------------------------------------------------------
//
void CEqualizerPresetListArray::GetCurrentActivePreset(TPresetName& aActivePreset)
    {
    aActivePreset = iActivePreset;
    }    
    
// ----------------------------------------------------------------------------
// CEqualizerPresetListArray::SetAudioEqualizer(CAudioEqualizerUtility* 
// aAudEqUtility)
// This function is used to set a new CAudioEqualizerUtility* 
// ----------------------------------------------------------------------------
//    
void CEqualizerPresetListArray::SetAudioEqualizer(
CAudioEqualizerUtility* aAudEqUtility)
	{
	iAudEqUtility=aAudEqUtility;
	}
	
// ----------------------------------------------------------------------------
// CEqualizerPresetListArray::GetPrefixLength()
// This function is used to get the length of prefix (eg "0\t")
// before the preset name. 
// ----------------------------------------------------------------------------
//    
TInt CEqualizerPresetListArray::GetPrefixLength()
	{
	TPtr ptr = iItem->Des();
    ptr.Zero();
    ptr.AppendNumUC(KEqrActiveIcon);
    ptr.Append(KEqrTab); 
    return ptr.Length();
	}
	    
//  End of File  
