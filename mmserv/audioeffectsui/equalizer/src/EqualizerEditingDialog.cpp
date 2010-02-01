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
* Description:    Defines functionality related to Equalizer Settings Dialog
*
*/



// INCLUDE FILES
 
#include <bautils.h>
#include <aknenv.h>      
#include <avkon.hrh>
#include <e32cmn.h>
#include <data_caging_path_literals.hrh>
//LAF and Skin changes 
#include <aknlayout.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <AknLayout2ScalableDef.h>
#include <AknLayout2Def.h>
#include <AknUtils.h>
#include <bldvariant.hrh>
#include <featmgr.h>
#include <AudioEqualizerUtility.h>
#include <hlplch.h>
#include <StringLoader.h>
#include <akntitle.h> 
#include <aknnotewrappers.h>

#include <Equalizer.rsg>
#include "EqualizerPrivateConstants.h"
#include <csxhelp/eq.hlp.hrh>
#include "EqualizerEditingDialog.h"
#include "EqualizerFreqBandContainer.h"       
#include "Equalizer.hrh"

// ============================ MEMBER FUNCTIONS =============================

// -------------------------------------------------------------------
// CEqualizerEditingDialog* CEqualizerEditingDialog::NewL(
// CAudioEqualizerUtility& aAudEqUtility, 
// const TInt aPresetIndex, TRect& aRect, const TBool aIsActivePreset)
// Create instance of concrete ECOM interface implementation
// -------------------------------------------------------------------
// 
CEqualizerEditingDialog* CEqualizerEditingDialog::NewL(
CAudioEqualizerUtility* aAudEqUtility, const TInt aPresetIndex,
 TRect& aRect, const TBool aIsActivePreset,
 MEqualizerPresetActivationObserver& aObserver,
 TInt aPresetKey)
    {
    CEqualizerEditingDialog* self = new (ELeave) CEqualizerEditingDialog(
        aAudEqUtility, aPresetIndex, aRect, aIsActivePreset, aObserver,
        aPresetKey);
    CleanupStack::PushL(self); 
    self->ConstructL();
    CleanupStack::Pop(self); 
    return self;
    }

// -------------------------------------------------------------------
// CEqualizerEditingDialog::ConstructL()
// Second Phase Constructor
// -------------------------------------------------------------------
// 
void CEqualizerEditingDialog::ConstructL()
    {
    //Initially set these variable to EFalse
    iIgnoreKeyEvents = EFalse;
    iContextSensitiveMenu = EFalse;

    iPresetName = iAudEqUtility->GetPresetL(iPresetIndex);
    CAudioEqualizer& audioEqualizer = iAudEqUtility->Equalizer();
    iNumberOfFreqBands = audioEqualizer.NumberOfBands();
    iMover=new(ELeave)CArrayFixSeg<TInt>(KEqualizerArrayGranularity);

    iFreqBandRect = RArray<TRect>(iNumberOfFreqBands);
    iScaleRect = RArray<TRect>(iNumberOfFreqBands);
    iBackgroundRect = RArray<TRect>(iNumberOfFreqBands);
    iSliderRect = RArray<TRect>(iNumberOfFreqBands);
    GetLayoutRect();

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
    CAknDialog::ConstructL(R_EQUALIZER_SETTINGS_MENUBAR);
    }

// ---------------------------------------------------------------------
// CEqualizerEditingDialog::CEqualizerEditingDialog
// (CAudioEqualizerUtility* aAudEqUtility, TInt aPresetIndex, 
// TRect& aRect, const TBool aIsActivePreset)
// First phase constructor
// ---------------------------------------------------------------------
// 
CEqualizerEditingDialog::CEqualizerEditingDialog(CAudioEqualizerUtility*
aAudEqUtility, const TInt aPresetIndex, TRect& aRect, 
const TBool aIsActivePreset, MEqualizerPresetActivationObserver& aObserver,
TInt aPresetKey ): 
iAudEqUtility(aAudEqUtility), 
iPresetIndex(aPresetIndex), iRect(aRect), 
iIsActivePreset(aIsActivePreset),
iObserver( aObserver ), iPresetKey( aPresetKey )
    {
    }

// --------------------------------------------------------------------
// CEqualizerEditingDialog::~CEqualizerEditingDialog()
// Destructor
// --------------------------------------------------------------------
// 
CEqualizerEditingDialog::~CEqualizerEditingDialog()
    {
    if (iResourceFileOffset)
        {
        iEikonEnv->DeleteResourceFile(iResourceFileOffset);      
        }
    
    if (iMover)
        {
        iMover->Reset();
        delete iMover;
        }
     
    iFreqBandRect.Close();
    iScaleRect.Close();
    iBackgroundRect.Close();
    iSliderRect.Close();
    }

// --------------------------------------------------------------------
// SEikControlInfo CEqualizerEditingDialog::
// CreateCustomControlL  (  TInt  aControlType   )
// This function is called by the framework to create a custom control
// inside the dialog
// --------------------------------------------------------------------
// 
SEikControlInfo CEqualizerEditingDialog::CreateCustomControlL  (  TInt 
 aControlType   ) 
    {
    SEikControlInfo controlInfo;
    controlInfo.iControl = NULL;
    controlInfo.iTrailerTextId = 0;
    controlInfo.iFlags = 0;

    SetMoverPositionsL(EFalse);      
   
    switch ( aControlType)
        {
        case EEqualizerCustomCtrlDlgCtCustomControl:
            controlInfo.iControl = new(ELeave) CEqualizerFreqBandContainer
            (this, iAudEqUtility, iPresetIndex, iIsActivePreset, 
             iRect, iObserver, iPresetKey, iPresetName); 
             
            iFreqBandContainer = (CEqualizerFreqBandContainer*)
                                        controlInfo.iControl; 
            break;
        default:
            break;
        }
    
    return controlInfo;
    }

// -------------------------------------------------------------------
// void CEqualizerEditingDialog::SizeChanged()
//
// This function is called by the framework whenever the control size 
// is changed
// --------------------------------------------------------------------
//
void CEqualizerEditingDialog::SizeChanged()
    {
    CAknDialog::SizeChanged();    
    iRect = Rect();
	GetLayoutRect();
    CAudioEqualizer& audioEqualizer = iAudEqUtility->Equalizer();
    iAudEqUtility->ModifyPresetL(iPresetIndex, iPresetName, audioEqualizer); 
	iMover->Reset();
	SetMoverPositionsL(EFalse);
    }

// -------------------------------------------------------------------
// void CEqualizerEditingDialog::GetLayoutRect()
//
// This function is called to retrive the freqband rect, scale rect
// slider rect from the layout
// --------------------------------------------------------------------
//
void CEqualizerEditingDialog::GetLayoutRect()
	{
	iFreqBandRect.Reset();
	iScaleRect.Reset();
	iBackgroundRect.Reset();
	iSliderRect.Reset();
	
	TInt variety = EEqualizerVarietyOne;
        
	if(iNumberOfFreqBands == KEqualizerFiveBands)
		variety = EEqualizerVarietyTwo;
	
	for(TInt count = 0; count < iNumberOfFreqBands; count++)
		{
		iFreqBandRect.Append(CEqualizerFreqBand::ComponentRect(
			iRect,variety+2,count));
		iScaleRect.Append(CEqualizerFreqBand::ComponentRect(
			iFreqBandRect[count],variety,EEqualizerScale));
		iBackgroundRect.Append(CEqualizerFreqBand::ComponentRect(
			iScaleRect[count],EEqualizerVarietyNotUsed,EEqualizerBackground));
		iSliderRect.Append(CEqualizerFreqBand::ComponentRect(
			iScaleRect[count],EEqualizerVarietyNotUsed,EEqualizerSlider));
		}
	}

// --------------------------------------------------------------------
// void CEqualizerEditingDialog::SetMoverPositionsL(TBool aReset)
// This function is used to read the positions of the mover for the 
// various bands from the engine.
// --------------------------------------------------------------------
// 
void CEqualizerEditingDialog::SetMoverPositionsL(TBool aReset)
    {
    //Calulate change in db level
    TInt32 minBandLevel;
    TInt32 maxBandLevel;
    iAudEqUtility->GetPresetL(iPresetIndex);
    CAudioEqualizer& audioEqualizer = iAudEqUtility->Equalizer();

    audioEqualizer.DbLevelLimits(minBandLevel, maxBandLevel);

    //The minimum band level should always be negative and the
    // maximum band level should always be positive.
    if (( minBandLevel > 0 ) || (maxBandLevel < 0))
        {
        User::Leave(KErrNotSupported);
        }

    for (TInt i =0; i<iNumberOfFreqBands; i++)
        {
	    TInt scaleHeight = iBackgroundRect[i].Height();
	    TInt scaleZeroPos = (iBackgroundRect[i].iTl.iY) + (scaleHeight / 2);

		TInt halfSliderSize = iSliderRect[i].Height() / 2;
		TInt sliderInc = iBackgroundRect[i].Height() / KNumberOfSteps;
		TInt diffUIBandLevel = KNumberOfSteps / 2;

        TInt curBandlevel = audioEqualizer.BandLevel(i+1);
        
        
        __ASSERT_DEBUG((curBandlevel >= minBandLevel) ||
                         (curBandlevel <= maxBandLevel), User::Invariant());
        if ((curBandlevel < minBandLevel) || (curBandlevel > maxBandLevel))
            {
            curBandlevel = 0; //Band level is invalid, use zero instead.
            }
        
        if (curBandlevel > 0)
            {
            TReal frac = (TReal)curBandlevel / maxBandLevel;
            TInt moverPos = (TInt) (scaleZeroPos - (frac * 
            								diffUIBandLevel * sliderInc));
            if((moverPos < (iBackgroundRect[i].iTl.iY + halfSliderSize)) ||
            			((scaleZeroPos - moverPos) >= (diffUIBandLevel * sliderInc)))
            	{
            	moverPos = iBackgroundRect[i].iTl.iY + halfSliderSize;
            	}

            if (aReset)
                {
                (*iMover)[i] = moverPos;    
                }
            else
                {
                iMover->AppendL(moverPos);    
                }
            }
        else if (curBandlevel < 0)
            {
            TReal frac = (TReal)curBandlevel / minBandLevel;
            TInt moverPos = (TInt) (scaleZeroPos + (frac * 
            								diffUIBandLevel * sliderInc));
            if((moverPos > ((iBackgroundRect[i].iBr.iY ) - halfSliderSize)) ||
            		((moverPos - scaleZeroPos) >= (diffUIBandLevel * sliderInc)))
            	{
            	moverPos = (iBackgroundRect[i].iBr.iY ) - halfSliderSize;
            	}

            if (aReset)
                {
                (*iMover)[i] = moverPos;    
                }
            else
                {
                iMover->AppendL(moverPos);    
                }
            }
        else
            {
            if (aReset)
                {
                (*iMover)[i] = scaleZeroPos;    
                }
            else
                {
                iMover->AppendL(scaleZeroPos);    
                }
            }   
        }
    }

// ---------------------------------------------------------
// CEqualizerEditingDialog::OfferKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode aType)
// This function is used to handle key events
// ---------------------------------------------------------
//
TKeyResponse CEqualizerEditingDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    { 
    if (iIgnoreKeyEvents)
        {
        return EKeyWasNotConsumed;
        }
    else
        {
        return CAknDialog::OfferKeyEventL( aKeyEvent, aType );        
        }
    }

// -------------------------------------------------------------------
// void CEqualizerEditingDialog::ProcessCommandL(TInt aCommandId)
// This function is called by the framework to handle the user's
// menubar command options
// --------------------------------------------------------------------
// 
void CEqualizerEditingDialog::ProcessCommandL(TInt aCommandId)
    {
    // Need to do this in all cases or menu does not disappear after
    // a selection is made
    CAknDialog::ProcessCommandL(aCommandId);
    
    switch(aCommandId)
        {
        case EAknCmdExit :
        case EEikCmdExit :
            {
            CAudioEqualizer& audioEqualizer = iAudEqUtility->Equalizer();
			iAudEqUtility->ModifyPresetL(iPresetIndex, iPresetName, audioEqualizer);
            iAvkonAppUi->HandleCommandL( aCommandId );
            break;
            }

        case EEqualizerCmdHelp: 
            {
            HlpLauncher::LaunchHelpApplicationL
                ( iEikonEnv->WsSession(),
                 CEikonEnv::Static()->AppUi()->AppHelpContextL() );
            break;
            }

        case EEqualizerCmdReset:
            {
            iAudEqUtility->GetPresetL(iPresetIndex);
            CAudioEqualizer& audioEqualizer = iAudEqUtility->Equalizer();
 
            if (iPresetIndex < iAudEqUtility->NumberOfPreDefinedPresets())
                 {
                 //Predefined preset
                 iAudEqUtility->ResetPresetL(iPresetIndex);     
                 SetMoverPositionsL(ETrue);
                 }
             else
                 {
                 //User defined preset, Reset manually
                 for (TInt i =0; i<iNumberOfFreqBands; i++)
                     {
					 TInt scaleHeight = iBackgroundRect[i].Height();
				     TInt scaleZeroPos = (iBackgroundRect[i].iTl.iY) 
				     									+ (scaleHeight / 2);
				     (*iMover)[i] = scaleZeroPos;
                     audioEqualizer.SetBandLevelL(i+1, 0);
                     }
                 iAudEqUtility->ModifyPresetL(iPresetIndex, iPresetName, 
                   audioEqualizer);    
                 }

             if (iIsActivePreset)
                 {
                 TRAP_IGNORE(iAudEqUtility->ApplyPresetL(iPresetIndex));
                 }
             
             // In MPX, we need to callback to observer when a preset is changed
             if( iIsActivePreset )
                 {
                 iObserver.HandlePresetActivation( iPresetKey );
                 }
    		 iFreqBandContainer->SetRect(iRect);
             DrawNow();
             break;
             }
              
        default:
            {
            break;
            }
        }
    }

// -------------------------------------------------------------------
// void CEqualizerEditingDialog::GetHelpContext( 
//            TCoeHelpContext& aContext ) const
// Get help context for the control
// --------------------------------------------------------------------
//         
void CEqualizerEditingDialog::GetHelpContext( 
            TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KUidEqualizer;
    aContext.iContext = KEQ_HLP_EDITING_VIEW;
    }

// ---------------------------------------------------------------------------
// CEqualizerEditingDialog::DynInitMenuPaneL(
//                              TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ---------------------------------------------------------------------------
//
void CEqualizerEditingDialog::DynInitMenuPaneL(
                        TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    if ( aResourceId == R_EQUALIZER_SETTINGS_MENU)
        {
        iMenuBar->SetMenuType(CEikMenuBar::EMenuOptions);
        // Help should be displayed only if the feature is supported
        // according to Feature Manager
        if (!FeatureManager::FeatureSupported(KFeatureIdHelp))
            {
            aMenuPane->SetItemDimmed(EEqualizerCmdHelp, ETrue);
            }
            
        if(iContextSensitiveMenu)
        	{
        	iMenuBar->SetMenuType(CEikMenuBar::EMenuContext);
          	aMenuPane->SetItemDimmed(EAknCmdExit, ETrue);
          	aMenuPane->SetItemDimmed(EEqualizerCmdHelp, ETrue);
			iContextSensitiveMenu = EFalse;
        	}
        }
    }

// ----------------------------------------------------------------------------
// CEqualizerEditingDialog::SetAudioEqualizerL(CAudioEqualizerUtility* 
// aAudEqUtility)
// This function is used to set a new CAudioEqualizerUtility* 
// ----------------------------------------------------------------------------
//    
void CEqualizerEditingDialog::SetAudioEqualizerL(
CAudioEqualizerUtility* aAudEqUtility)
	{
	iAudEqUtility=aAudEqUtility;
	if (iAudEqUtility)
	    {
	    iIgnoreKeyEvents = EFalse;
	    iFreqBandContainer->SetAudioEqualizerL(aAudEqUtility);
	    }
	else
	    {
	    iIgnoreKeyEvents = ETrue; 
	    }
	}
	
// -------------------------------------------------------------------
// TInt CEqualizerEditingDialog::ExecuteLD()
// This function is called to execute and delete the dialog
// --------------------------------------------------------------------
// 
TInt CEqualizerEditingDialog::ExecuteLD()
    {
    return CAknDialog::ExecuteLD(R_EQUALIZER_EDITING_DIALOG);
    }                    

// -------------------------------------------------------------------
// TBool CEqualizerEditingDialog::OkToExitL(TInt aButtonId)
// This function is called when the OK key is pressed
// --------------------------------------------------------------------
// 
TBool CEqualizerEditingDialog::OkToExitL(TInt aButtonId)
    {
    TBool ret = EFalse;
    if (aButtonId == EEikBidOk)
        {
        ret = EFalse;
        }
    else if (aButtonId == EAknSoftkeyBack) 
        {
        CAudioEqualizer& audioEqualizer = iAudEqUtility->Equalizer();
        iAudEqUtility->ModifyPresetL(iPresetIndex, iPresetName, audioEqualizer);
        
        HBufC* iTitleText = StringLoader::LoadLC(R_QTN_EQUALIZER_TITLE_TEXT);
        CEikStatusPane* sp=iEikonEnv->AppUiFactory()->StatusPane();
        CAknTitlePane* tp=(CAknTitlePane*)sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle));
        tp->SetTextL(*iTitleText);
        CleanupStack::PopAndDestroy(iTitleText);
    
    	ret = CAknDialog::OkToExitL( aButtonId ); 
        }        
    else if(aButtonId == EAknSoftkeyContextOptions)
        {
        iContextSensitiveMenu = ETrue;
    	 DisplayMenuL();
        }
    else 
        {
        ret = CAknDialog::OkToExitL( aButtonId );        
        }   
    return ret;
    } 
    
// -------------------------------------------------------------------
// TInt CEqualizerEditingDialog::GetMoverPosition(TInt aBandIndex)
// This function returns the current mover position for each frequency band
// --------------------------------------------------------------------
// 
TInt CEqualizerEditingDialog::GetMoverPosition(TInt aBandIndex)
{
	return (*iMover)[aBandIndex];
}

// End of File
