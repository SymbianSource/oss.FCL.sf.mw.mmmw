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
* Description:    Defines functionality related to Equalizer Frequency Band 
*                Container
*
*/



// INCLUDE FILES

#include <AknUtils.h>
#include <AknsDrawUtils.h>
#include <bautils.h>
#include <AknsBasicBackgroundControlContext.h> 
#include <data_caging_path_literals.hrh>

//LAF and Skin changes
#include <AknIconUtils.h>
#include <AknsUtils.h>
#include <AknsConstants.h>
#include <aknlayout.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <AknLayout2ScalableDef.h>
#include <AknLayout2Def.h>
#include <AudioEqualizerUtility.h>
#include <eiklabel.h>
#include <StringLoader.h>
#include <Equalizer.rsg>
#include <layoutmetadata.cdl.h>
#include <e32math.h>

#include "EqualizerPrivateConstants.h"
#include "EqualizerEditingDialog.h"
#include "EqualizerFreqBand.h"
#include "EqualizerFreqBandContainer.h"
#include "Equalizer.hrh"

_LIT(KLabelFormatInt,"%d");
_LIT(KLabelFormatFloat,"%.1f");

// ============================ MEMBER FUNCTIONS =============================

//////////////////////////////////////////////////////////////////////////////
//
// -----> CEqualizerFreqBandContainer (implementation)
//
//////////////////////////////////////////////////////////////////////////////

// -------------------------------------------------------------------
// CEqualizerFreqBandContainer::CEqualizerFreqBandContainer
//(CAudioEqualizerUtility* aAudEqUtility,  TInt aPresetIndex,
// TRect& aRect)
//
// First phase constructor
// --------------------------------------------------------------------
// 
CEqualizerFreqBandContainer::CEqualizerFreqBandContainer(
	CEqualizerEditingDialog* aEqEditingDialog,
    CAudioEqualizerUtility* aAudEqUtility, TInt aPresetIndex, 
    const TBool aIsActivePreset, TRect& aRect,
    MEqualizerPresetActivationObserver& aObserver,
    TInt aPresetKey, TPresetName& aPresetName )
     :iEqEditingDialog(aEqEditingDialog), iAudEqUtility(aAudEqUtility), iPresetIndex(aPresetIndex), 
     iIsActivePreset(aIsActivePreset), iRect(aRect), iObserver( aObserver ), iPresetKey( aPresetKey ),
     iBandSelected(EFalse), iPresetName( aPresetName )
    {
    }

// -------------------------------------------------------------------
// CEqualizerFreqBandContainer::~CEqualizerFreqBandContainer()
//
// Destructor
// --------------------------------------------------------------------
//     
CEqualizerFreqBandContainer::~CEqualizerFreqBandContainer()
    {
    delete iBackGround; 
    if (iFreqBand)
        {
        iFreqBand->ResetAndDestroy(); //delete items
        delete (iFreqBand);
        }
    if (iFreqLabels)
        {
        iFreqLabels->ResetAndDestroy(); //delete items
        delete (iFreqLabels);
        }
    }

// -------------------------------------------------------------------
// void CEqualizerFreqBandContainer::ConstructL(TRect& aRect)
//
// Because CEqualizerFreqBandContainer is a compound control, it needs a
// ConstructL() for when it's created outside a dialog, and a
// ConstructFromResourceL() for when it's created inside a dialog.
// --------------------------------------------------------------------
//     
void CEqualizerFreqBandContainer::ConstructL(TRect& aRect)
    {
    iBackGround = CAknsBasicBackgroundControlContext::NewL
    	( KAknsIIDQsnBgAreaMain, Rect(), EFalse );
    iAudEqUtility->GetPresetL(iPresetIndex);
    CAudioEqualizer& audioEqualizer = iAudEqUtility->Equalizer();
    iNumberOfFreqBands = audioEqualizer.NumberOfBands();
    iFreqBand = new(ELeave) CArrayPtrFlat<CEqualizerFreqBand>
                 (KEqualizerArrayGranularity);
    iFreqLabels = new(ELeave) CArrayPtrFlat<CEikLabel>
                 (KEqualizerArrayGranularity);
    
    for (int i=0; i<iNumberOfFreqBands; i++)
        {
        // Create the CEqualizerFreqBands. Their size and position is 
        // set in CEqualizerFreqBandContainer::SizeChangedL().
        
        CEqualizerFreqBand* band = CEqualizerFreqBand::NewL(iEqEditingDialog, iAudEqUtility, 
         iPresetIndex, iIsActivePreset, i, aRect);
         
        CleanupStack::PushL(band);
        iFreqBand->AppendL(band);
        CleanupStack::Pop(band);
        (*iFreqBand)[i]->SetContainerWindowL(*this);
        }

    (*iFreqBand)[0]->SetFocus(ETrue);
    iIndexOfFocussedFreqBand = 0;
        
#ifdef RD_SCALABLE_UI_V2
    EnableDragEvents();
#endif //RD_SCALABLE_UI_V2
    ConstructLabelsL();
    // Set the bounding rectangle of this control (this will result in 
    // a call to SizeChangedL(). The component controls must be 
    // created before calling this, because SizeChangedL() sets their
    // sizes.
    SetRect(aRect);
    ActivateL();
    }

// ----------------------------------------------------------------------
// void CEqualizerFreqBandContainer::ConstructFromResourceL
// (TResourceReader& /*aReader*/)
//
// This function is used when the CEqualizerFreqBandContainer is created
// inside a dialog.
// ----------------------------------------------------------------------
// 
void CEqualizerFreqBandContainer::ConstructFromResourceL(TResourceReader& 
/*aReader*/)
    {
    ConstructL(iRect);
    }

// -------------------------------------------------------------------
// TInt CEqualizerFreqBandContainer::CountComponentControls() const
//
// Returns the number of controls in a compound control.
// -------------------------------------------------------------------
// 
TInt CEqualizerFreqBandContainer::CountComponentControls() const
    {
    return iNumberOfFreqBands + iFreqLabels->Count();
    }

// -------------------------------------------------------------------
// CCoeControl* CEqualizerFreqBandContainer::ComponentControl
// (TInt aIndex) const
//
// Returns the control given by the index in a compind control.
// -------------------------------------------------------------------
//
CCoeControl* CEqualizerFreqBandContainer::ComponentControl(TInt aIndex) const
    {
    if(aIndex<iNumberOfFreqBands)
    return (*iFreqBand)[aIndex];
    else 
    return (*iFreqLabels)[aIndex - iNumberOfFreqBands];
    }

// ---------------------------------------------------------------------
// void CEqualizerFreqBandContainer::SizeChanged()
//
// This function gets called whenever one of the size-setting functions 
// is called. As this is a compound control, this function calculates 
// and sets the size and position for its components, based on its own 
// size.
// ---------------------------------------------------------------------
//
void CEqualizerFreqBandContainer::SizeChanged()
    {
    TInt equalizerVariety = EEqualizerVarietyOne; //By defualt use variety one
    
    TRect myRect = Rect();
    TRect rect = CEqualizerFreqBand::ComponentRect(iRect,
                                EEqualizerVarietyNotUsed,EEqualizerMainPane);
    iRect = rect;
    /**
     *Equalizer variety based on the number of bands
     * Number of bands = 8 ==> Variety = 0
     * Number of bands = 5 ==> Variety = 1
     */
    if(iNumberOfFreqBands == KEqualizerEightBands)
        {
        equalizerVariety = EEqualizerVarietyOne;
        }
        
    if(iNumberOfFreqBands == KEqualizerFiveBands)
        {
        equalizerVariety = EEqualizerVarietyTwo;
        }
        
    //Setting the position and size for the frequency bands from the layout 
    //obtained
    for(TInt count = 0;count < iNumberOfFreqBands; count++)
        {
        TRect freqBandRect = CEqualizerFreqBand::ComponentRect(iRect,
                                                     equalizerVariety+2,count);
        (*iFreqBand)[count]->SetRect(freqBandRect);
        }
        
    TInt variety=0;
    if(Layout_Meta_Data::IsLandscapeOrientation())
    	variety=4;
    TRect temp;
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EApplicationWindow, temp );
    TAknLayoutRect mainPaneLayout;
    mainPaneLayout.LayoutRect( temp,AknLayoutScalable_Apps::main_pane(variety).LayoutLine() );
    myRect = mainPaneLayout.Rect();
    myRect.iBr.iY = myRect.iBr.iY - myRect.iTl.iY;
    myRect.iTl.iY = 0;
    
    TAknTextLineLayout tLayout;
    
    tLayout = AknLayoutScalable_Apps::main_mup_eq_pane_t9(equalizerVariety);
   	AknLayoutUtils::LayoutLabel((*iFreqLabels)[0], myRect, tLayout);
	
	tLayout = AknLayoutScalable_Apps::main_mup_eq_pane_t10(equalizerVariety);
	AknLayoutUtils::LayoutLabel((*iFreqLabels)[1], myRect, tLayout);
	
	tLayout = AknLayoutScalable_Apps::main_mup_eq_pane_t1(equalizerVariety);
	AknLayoutUtils::LayoutLabel((*iFreqLabels)[2], iRect, tLayout);
	
	tLayout = AknLayoutScalable_Apps::main_mup_eq_pane_t2(equalizerVariety);
	AknLayoutUtils::LayoutLabel((*iFreqLabels)[3], iRect, tLayout);
	
	tLayout = AknLayoutScalable_Apps::main_mup_eq_pane_t3(equalizerVariety);
	AknLayoutUtils::LayoutLabel((*iFreqLabels)[4], iRect, tLayout);
	
	tLayout = AknLayoutScalable_Apps::main_mup_eq_pane_t4(equalizerVariety);
	AknLayoutUtils::LayoutLabel((*iFreqLabels)[5], iRect, tLayout);
	
	tLayout = AknLayoutScalable_Apps::main_mup_eq_pane_t5(equalizerVariety);
	AknLayoutUtils::LayoutLabel((*iFreqLabels)[6], iRect, tLayout);
	
	if(equalizerVariety == 0)
		{
		tLayout = AknLayoutScalable_Apps::main_mup_eq_pane_t6(equalizerVariety);
		AknLayoutUtils::LayoutLabel((*iFreqLabels)[7], iRect, tLayout);
		
		tLayout = AknLayoutScalable_Apps::main_mup_eq_pane_t7(equalizerVariety);
		AknLayoutUtils::LayoutLabel((*iFreqLabels)[8], iRect, tLayout);
		
		tLayout = AknLayoutScalable_Apps::main_mup_eq_pane_t8(equalizerVariety);
		AknLayoutUtils::LayoutLabel((*iFreqLabels)[9], iRect, tLayout);
		}
	
	if (iBackGround)
	    {
	    iBackGround->SetRect(iRect);
	    }
    TRgb color;
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
        
    for(TInt i=0; i<iFreqLabels->Count(); i++)
        {
        CCoeControl* label( ComponentControl( i+iNumberOfFreqBands ));
        TInt error = AknsUtils::GetCachedColor( skin, color,
                                                KAknsIIDQsnTextColors,
                                                EAknsCIQsnTextColorsCG6 );
        if ( !error )
           	AknLayoutUtils::OverrideControlColorL( *label,
                                                  EColorLabelText,
                                                  color);
        }
    }

// -------------------------------------------------------------------
// void CEqualizerFreqBandContainer::Draw(const TRect& aRect) const
//
// This function gets called to draw a control
// -------------------------------------------------------------------
//         
void CEqualizerFreqBandContainer::Draw(const TRect& aRect) const
    {
    CWindowGc& gc=SystemGc();
    gc.Clear(iRect);
    // Drawing skin
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    if (iBackGround)
        {
        AknsDrawUtils::Background(skin, iBackGround, this, gc, iRect);
        }
    
    for(TInt i=0; i<iFreqLabels->Count(); i++)
    	{
    	if( (*iFreqLabels)[i] )
    		{
    		(*iFreqLabels)[i]->Draw(aRect);
    		}
        }
    }

// ---------------------------------------------------------------------
// void CEqualizerFreqBandContainer::FocusChanged(TDrawNow aDrawNow)
//
// This function gets called whenever the application calls SetFocus().
// It redraws the CEqualizerFreqBandContainer, so that they are updated
// to show which one now has focus.
// ---------------------------------------------------------------------
//
void CEqualizerFreqBandContainer::FocusChanged(TDrawNow aDrawNow)
    {
    if (IsFocused())
        {
        (*iFreqBand)[0]->SetFocus(ETrue, EDrawNow);
        iFocussedFreqBand = (*iFreqBand)[0];
        iIndexOfFocussedFreqBand = 0;
        }
    else
        {
        if (iFocussedFreqBand && iFocussedFreqBand->IsFocused())
            {
            iFocussedFreqBand->SetFocus(EFalse, EDrawNow);             
            }
        }
    if (aDrawNow)
        {
        DrawNow();        
        } 
    }

// ----------------------------------------------------------------------
// void CEqualizerFreqBandContainer::SwapFocus(CCoeControl* aPrevControl,
//  CCoeControl* aNewControl)
//
// This function is called from OfferKeyEventL() whenever the user 
// presses left/right arrow keys to change to the previous/next band
// ----------------------------------------------------------------------
//
void CEqualizerFreqBandContainer::SwapFocus(CCoeControl* aPrevControl, 
CCoeControl* aNewControl)
    {
    aPrevControl->SetFocus(EFalse, EDrawNow);
    aNewControl->SetFocus(ETrue, EDrawNow);
    iFocussedFreqBand =  aNewControl;
    }

// -------------------------------------------------------------------------
// CEqualizerFreqBandContainer::OfferKeyEventL 
// Function for handling key events.
// -------------------------------------------------------------------------
//
TKeyResponse CEqualizerFreqBandContainer::OfferKeyEventL(const TKeyEvent& 
aKeyEvent,TEventCode aType)
    {
    TKeyResponse ret = EKeyWasNotConsumed;
    TInt indexOfLastFocussedBand = -1;
    if( aType == EEventKey )
        {
          // Use the arrow keys to move focus between the two CEqualizerFreqBands.
        switch (aKeyEvent.iCode)
            {
            case EKeyUpArrow:
            case EKeyDownArrow:
                {
                ret = (*iFreqBand)[iIndexOfFocussedFreqBand]->OfferKeyEventL(
                aKeyEvent, aType);
                
                // In MPX the band dialog presets need to be saved 
                // and called back to the observer
                //
                if( iIsActivePreset )
                    {
                    CAudioEqualizer& audioEqualizer = iAudEqUtility->Equalizer();
                    iAudEqUtility->ModifyPresetL(iPresetIndex, iPresetName, audioEqualizer);
                    
                    iObserver.HandlePresetActivation( iPresetKey );
                    }
                }
                break;
            
            case EKeyRightArrow:
                if (AknLayoutUtils::LayoutMirrored())
                    {
                    // For Series60 Arabic Hebrew LAF                
                    // Bugfix for Id: TCON-6HKGAB            
                    indexOfLastFocussedBand = iIndexOfFocussedFreqBand;
                    iIndexOfFocussedFreqBand = 
                    (indexOfLastFocussedBand+iNumberOfFreqBands-1)
                                    %iNumberOfFreqBands;
                    SwapFocus((*iFreqBand)[indexOfLastFocussedBand], 
                    (*iFreqBand)[iIndexOfFocussedFreqBand]);
                    ret = EKeyWasConsumed;                     
                    }
                else
                    {
                    indexOfLastFocussedBand = iIndexOfFocussedFreqBand;
                    iIndexOfFocussedFreqBand = 
                    (indexOfLastFocussedBand+1)%iNumberOfFreqBands;
                    SwapFocus((*iFreqBand)[indexOfLastFocussedBand], 
                    (*iFreqBand)[iIndexOfFocussedFreqBand]);
                    ret = EKeyWasConsumed;                   
                    }
                break;
            case EKeyLeftArrow:
                if (AknLayoutUtils::LayoutMirrored())
                    {
                    // For Series60 Arabic Hebrew LAF                
                    // Bugfix for Id: TCON-6HKGAB
                    indexOfLastFocussedBand = iIndexOfFocussedFreqBand;
                    iIndexOfFocussedFreqBand = (indexOfLastFocussedBand + 1)% 
                                                iNumberOfFreqBands;
                    SwapFocus((*iFreqBand)[indexOfLastFocussedBand], 
                    (*iFreqBand)[iIndexOfFocussedFreqBand]);
                    ret = EKeyWasConsumed;                     
                    }
                else
                    {
                    indexOfLastFocussedBand = iIndexOfFocussedFreqBand;
                    iIndexOfFocussedFreqBand = (indexOfLastFocussedBand + 
                    iNumberOfFreqBands -1)% iNumberOfFreqBands;
                    SwapFocus((*iFreqBand)[indexOfLastFocussedBand], 
                    (*iFreqBand)[iIndexOfFocussedFreqBand]);
                    ret = EKeyWasConsumed;                   
                    }
                break;

            default:
                break;
            }         
        }

    // If the CEqualizerFreqBandContainer didn't use the key event, it
    // must return EKeyWasNotConsumed,
    // so that the key event is passed to other controls on the stack.
    return ret;
    }

// ----------------------------------------------------------------------------
// CEqualizerFreqBandContainer::SetAudioEqualizerL(CAudioEqualizerUtility* 
// aAudEqUtility)
// This function is used to set a new CAudioEqualizerUtility* 
// ----------------------------------------------------------------------------
//    
void CEqualizerFreqBandContainer::SetAudioEqualizerL(
CAudioEqualizerUtility* aAudEqUtility)
	{
	iAudEqUtility=aAudEqUtility;
	for (int i=0; i<iNumberOfFreqBands; i++)
	    {
	    (*iFreqBand)[i]->SetAudioEqualizerL(aAudEqUtility);
	    }
	}

// ---------------------------------------------------------
// CEqualizerFreqBandContainer::MopSupplyObject
// Pass skin information if need.
// (other items were commented in a header).
// ---------------------------------------------------------
//
TTypeUid::Ptr CEqualizerFreqBandContainer::MopSupplyObject(TTypeUid aId)
	{
	if ( aId.iUid == MAknsControlContext::ETypeId )
		{
		// Return the control context for the childs
		return MAknsControlContext::SupplyMopObject( aId, iBackGround );
		}
	return CCoeControl::MopSupplyObject( aId );
	}
	
// ---------------------------------------------------------
// CEqualizerFreqBandContainer::ConstructLabelsL
// Contruct the frequency value labels.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CEqualizerFreqBandContainer::ConstructLabelsL()
	{
	
	CAudioEqualizer& audioEqualizer = iAudEqUtility->Equalizer();
	CEikLabel* tempLabel;
	TBuf<15> tempStr;
	TReal x,y;
	
	tempLabel = new (ELeave) CEikLabel;
	tempLabel->SetContainerWindowL( *this );
  HBufC* hzText = StringLoader::LoadLC(R_QTN_EQ_PRESET_FREQUENCY_HZ);
	tempLabel->SetTextL(hzText->Des());
	tempLabel->SetBrushStyle(CGraphicsContext::ENullBrush);
	CleanupStack::PushL(tempLabel);
	iFreqLabels->AppendL(tempLabel); 
	CleanupStack::Pop(tempLabel);
	CleanupStack::PopAndDestroy(); // hzText
	
	tempLabel = new (ELeave) CEikLabel;
	tempLabel->SetContainerWindowL( *this );
  HBufC* mhzText = StringLoader::LoadLC(R_QTN_EQ_PRESET_FREQUENCY_KHZ);
	tempLabel->SetTextL(mhzText->Des());
	tempLabel->SetBrushStyle(CGraphicsContext::ENullBrush);
	CleanupStack::PushL(tempLabel);
	iFreqLabels->AppendL(tempLabel); 
	CleanupStack::Pop(tempLabel);
	CleanupStack::PopAndDestroy(); // MhzText

	tempLabel = new (ELeave) CEikLabel;
	tempLabel->SetContainerWindowL( *this );
	tempStr.Format(KLabelFormatInt,audioEqualizer.CenterFrequency(1));
	AknTextUtils::LanguageSpecificNumberConversion( tempStr );
	tempLabel->SetTextL(tempStr);
	tempLabel->SetBrushStyle(CGraphicsContext::ENullBrush);
	CleanupStack::PushL(tempLabel);
	iFreqLabels->AppendL(tempLabel); 
	CleanupStack::Pop(tempLabel);
	
	tempLabel = new (ELeave) CEikLabel;
	tempLabel->SetContainerWindowL( *this );
    tempStr.Format(KLabelFormatInt, audioEqualizer.CenterFrequency(2));
    AknTextUtils::LanguageSpecificNumberConversion( tempStr );
	tempLabel->SetTextL(tempStr);
	tempLabel->SetBrushStyle(CGraphicsContext::ENullBrush);
	CleanupStack::PushL(tempLabel);
	iFreqLabels->AppendL(tempLabel); 
	CleanupStack::Pop(tempLabel);
	
	tempLabel = new (ELeave) CEikLabel;
	tempLabel->SetContainerWindowL( *this );
    tempStr.Format(KLabelFormatInt, audioEqualizer.CenterFrequency(3));
    AknTextUtils::LanguageSpecificNumberConversion( tempStr );
	tempLabel->SetTextL(tempStr);
	tempLabel->SetBrushStyle(CGraphicsContext::ENullBrush);
	CleanupStack::PushL(tempLabel);
	iFreqLabels->AppendL(tempLabel); 
	CleanupStack::Pop(tempLabel);
	
	Math::Round(y,(TReal)audioEqualizer.CenterFrequency(4)/1000,1);
	tempLabel = new (ELeave) CEikLabel;
	tempLabel->SetContainerWindowL( *this );
	Math::Mod(x,y*10,10);
	if(x==0)
	    tempStr.Format(KLabelFormatInt,(TInt)y);
	else
	    tempStr.Format(KLabelFormatFloat,y);
	AknTextUtils::LanguageSpecificNumberConversion( tempStr );
	tempLabel->SetTextL(tempStr);
	tempLabel->SetBrushStyle(CGraphicsContext::ENullBrush);
	CleanupStack::PushL(tempLabel);
	iFreqLabels->AppendL(tempLabel); 
	CleanupStack::Pop(tempLabel);
	
	Math::Round(y,(TReal)audioEqualizer.CenterFrequency(5)/1000,1);
	tempLabel = new (ELeave) CEikLabel;
	tempLabel->SetContainerWindowL( *this );
	Math::Mod(x,y*10,10);
	if(x==0)
	    tempStr.Format(KLabelFormatInt,(TInt)y);
	else
	    tempStr.Format(KLabelFormatFloat,y);
	AknTextUtils::LanguageSpecificNumberConversion( tempStr );
	tempLabel->SetTextL(tempStr);
	tempLabel->SetBrushStyle(CGraphicsContext::ENullBrush);
	CleanupStack::PushL(tempLabel);
	iFreqLabels->AppendL(tempLabel); 
	CleanupStack::Pop(tempLabel);
	
	if(iNumberOfFreqBands ==KEqualizerEightBands)
		{
		Math::Round(y,(TReal)audioEqualizer.CenterFrequency(6)/1000,1);
		tempLabel = new (ELeave) CEikLabel;
		tempLabel->SetContainerWindowL( *this );
		Math::Mod(x,y*10,10);
		if(x==0)
	        tempStr.Format(KLabelFormatInt,(TInt)y);
	    else
	        tempStr.Format(KLabelFormatFloat,y);
	    AknTextUtils::LanguageSpecificNumberConversion( tempStr );
		tempLabel->SetTextL(tempStr);
		tempLabel->SetBrushStyle(CGraphicsContext::ENullBrush);
		CleanupStack::PushL(tempLabel);
		iFreqLabels->AppendL(tempLabel); 
		CleanupStack::Pop(tempLabel);
		
		Math::Round(y,(TReal)audioEqualizer.CenterFrequency(7)/1000,1);
		tempLabel = new (ELeave) CEikLabel;
		tempLabel->SetContainerWindowL( *this );
		Math::Mod(x,y*10,10);
		if(x==0)
	        tempStr.Format(KLabelFormatInt,(TInt)y);
	    else
	        tempStr.Format(KLabelFormatFloat,y);
	    AknTextUtils::LanguageSpecificNumberConversion(tempStr);
		tempLabel->SetTextL(tempStr);
		tempLabel->SetBrushStyle(CGraphicsContext::ENullBrush);
		CleanupStack::PushL(tempLabel);
		iFreqLabels->AppendL(tempLabel); 
		CleanupStack::Pop(tempLabel);
		
		Math::Round(y,(TReal)audioEqualizer.CenterFrequency(8)/1000,1);
		tempLabel = new (ELeave) CEikLabel;
		tempLabel->SetContainerWindowL( *this );
		Math::Mod(x,y*10,10);
		if(x==0)
	        tempStr.Format(KLabelFormatInt,(TInt)y);
    	else
	        tempStr.Format(KLabelFormatFloat,y);
    	AknTextUtils::LanguageSpecificNumberConversion(tempStr);
		tempLabel->SetTextL(tempStr);
		tempLabel->SetBrushStyle(CGraphicsContext::ENullBrush);
		CleanupStack::PushL(tempLabel);
		iFreqLabels->AppendL(tempLabel); 
		CleanupStack::Pop(tempLabel);
		}
		
	TRgb color;
  MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    
  for(TInt i=0; i<iFreqLabels->Count(); i++)
   	{
   	CCoeControl* label( ComponentControl( i+iNumberOfFreqBands ));
   	TInt error = AknsUtils::GetCachedColor( skin, color,
                                            KAknsIIDQsnTextColors,
                                            EAknsCIQsnTextColorsCG6 );
   	if ( !error )
       	AknLayoutUtils::OverrideControlColorL( *label,
                                              EColorLabelText,
                                              color);
   	}
	
	}
	
#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------
// CEqualizerFreqBandContainer::HandlePointerEventL
// This function is used to handle pen input events.
// ---------------------------------------------------------
//
void CEqualizerFreqBandContainer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    if (AknLayoutUtils::PenEnabled())
        {
        if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
            {
	        for ( TInt i = 0; i < iNumberOfFreqBands; i++ )
	            {
	            TRect x = (*iFreqBand)[i]->Rect();
	            if (x.Contains(aPointerEvent.iPosition))
	                {
	                if ( iIndexOfFocussedFreqBand != i )
	                    {
	                    TInt indexOfLastFocussedBand = iIndexOfFocussedFreqBand;
	                    iIndexOfFocussedFreqBand = i;
	                    SwapFocus((*iFreqBand)[indexOfLastFocussedBand],
	                        (*iFreqBand)[iIndexOfFocussedFreqBand]);
	                    }
	                iBandSelected = ETrue;
	                break;
	                }
				}
			}

		if ( iBandSelected )
		    {
			(*iFreqBand)[iIndexOfFocussedFreqBand]->HandlePointerEventL(aPointerEvent);
		    }

		// In MPX the band dialog presets need to be saved
		// and called back to the observer
		//
		if ( iBandSelected &&
            ( (aPointerEvent.iType == TPointerEvent::EButton1Up) ||
		      (aPointerEvent.iType == TPointerEvent::EDrag) ) )
			{
			if ( iIsActivePreset )
                {
				CAudioEqualizer& audioEqualizer = iAudEqUtility->Equalizer();
				iAudEqUtility->ModifyPresetL(iPresetIndex, iPresetName, audioEqualizer);
				iObserver.HandlePresetActivation( iPresetKey );
				}
			if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
                {
                iBandSelected = EFalse;
			    }
			}
	    }
    }
#endif //RD_SCALABLE_UI_V2

// End of File

