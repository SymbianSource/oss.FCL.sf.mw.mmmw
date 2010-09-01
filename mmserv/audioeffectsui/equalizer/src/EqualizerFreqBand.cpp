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
* Description:    Defines functionality related to Equalizer Frequency band
*
*/



// INCLUDE FILES

#include <eikdef.h>
#include <e32math.h>
#include <AknUtils.h>
#include <AknsDrawUtils.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh> 
//LAF and Skin changes
#include <AknIconUtils.h>
#include <AknsUtils.h>
#include <AknsConstants.h>
#include <AknsDrawUtils.h>
#include <aknlayout.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <AknLayout2ScalableDef.h>
#include <AknLayout2Def.h>
#include <AudioEqualizerUtility.h>
#if defined(RD_TACTILE_FEEDBACK)
#include <touchfeedback.h>
#endif

#include <equalizer.mbg>
#include "EqualizerPrivateConstants.h"
#include "EqualizerEditingDialog.h"
#include "EqualizerFreqBand.h"
#include "Equalizer.hrh"

// ============================ MEMBER FUNCTIONS =============================

//============================================================================
// -----> CEqualizerFreqBand (implementation)
//============================================================================

// ----------------------------------------------------------------------------
// CEqualizerFreqBand* CEqualizerFreqBand::NewL(CEqualizerEditingDialog* aEqEditingDialog,
//    CAudioEqualizerUtility*& aAudEqUtility, CAudioEqualizer& aAudEqualizer,
//     const TInt aPresetIndex, const TBool aIsActivePreset,
//    const TInt aBandIndex, TInt& aMoverPos, TRect& aRect)
//
// Two Phase construction
// ----------------------------------------------------------------------------
//
CEqualizerFreqBand* CEqualizerFreqBand::NewL(CEqualizerEditingDialog* aEqEditingDialog,
    CAudioEqualizerUtility*& aAudEqUtility, const TInt aPresetIndex,
    const TBool aIsActivePreset, const TInt aBandIndex, 
    TRect& aRect)
    {
    CEqualizerFreqBand* self = CEqualizerFreqBand::NewLC(aEqEditingDialog, aAudEqUtility,
     aPresetIndex, aIsActivePreset, aBandIndex, 
     aRect);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CEqualizerFreqBand* CEqualizerFreqBand::NewLC(CEqualizerEditingDialog* aEqEditingDialog,
// CAudioEqualizerUtility*& aAudEqUtility, const TInt aPresetIndex,
// const TBool aIsActivePreset,   const TInt aBandIndex, TInt& aMoverPos, 
// TRect& aRect)
// Two Phase construction
// ----------------------------------------------------------------------------
//
CEqualizerFreqBand* CEqualizerFreqBand::NewLC(CEqualizerEditingDialog* aEqEditingDialog,
    CAudioEqualizerUtility*& aAudEqUtility, const TInt aPresetIndex,
    const TBool aIsActivePreset, const TInt aBandIndex, 
    TRect& aRect)
    {
    CEqualizerFreqBand* self = new (ELeave) CEqualizerFreqBand(aEqEditingDialog, aAudEqUtility,
    aPresetIndex, aIsActivePreset, aBandIndex, 
    aRect);
    CleanupStack::PushL(self); 
    self->ConstructL();
    return self;
    }    
// ----------------------------------------------------------------------------
//CEqualizerFreqBand::CEqualizerFreqBand(CEqualizerEditingDialog* aEqEditingDialog,
//    CAudioEqualizerUtility*& aAudEqUtility, 
//     const TPresetName& aPresetName, const TBool aIsActivePreset,
//    const TInt aBandIndex, TInt& aMoverPos, TRect& aRect) 
//
// First phase constructor
// ----------------------------------------------------------------------------
CEqualizerFreqBand::CEqualizerFreqBand(CEqualizerEditingDialog* aEqEditingDialog, 
    CAudioEqualizerUtility*& aAudEqUtility, const TInt aPresetIndex, 
    const TBool aIsActivePreset,
    const TInt aBandIndex,  TRect& aContainerRect) 
    : iEqEditingDialog(aEqEditingDialog), iAudEqUtility(aAudEqUtility), iPresetIndex(aPresetIndex), 
    iIsActivePreset(aIsActivePreset), iBandIndex(aBandIndex), 
    iContainerRect(aContainerRect)
    {
    }

// -------------------------------------------------------------------
// void CEqualizerFreqBand::ConstructL() 
//
// Second phase constructor
// --------------------------------------------------------------------
//
void CEqualizerFreqBand::ConstructL() 
    {
    LoadBitmapsL();
    iPresetName = iAudEqUtility->GetPresetL(iPresetIndex);
    iAudioEqualizer = &iAudEqUtility->Equalizer();
    iAudioEqualizer->DbLevelLimits(iMinBandLevel, iMaxBandLevel);
    }

// -------------------------------------------------------------------
// CEqualizerFreqBand::~CEqualizerFreqBand()
//
// Destructor
// --------------------------------------------------------------------
//    
CEqualizerFreqBand::~CEqualizerFreqBand()
    { 
    delete iBgBitmap;
    delete iBgBitmapMask;
    delete iSliderBitmap;
    delete iSliderBitmapMask;

#if defined(RD_TACTILE_FEEDBACK)
   MTouchFeedback* feedback = MTouchFeedback::Instance();
   if (feedback)
       {
   	   feedback->RemoveFeedbackForControl(this);
       }
#endif
    }

// -------------------------------------------------------------------
// void CEqualizerFreqBand::Draw(const TRect& aRect) const
//
// Function to draw the control
// --------------------------------------------------------------------
//
void CEqualizerFreqBand::Draw(const TRect& /*aRect*/) const
    {
    CWindowGc& gc=SystemGc();
    MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    AknsDrawUtils::Background( skinInstance, cc, this, gc, iBackgroundRect );
    TRect innerRect = iBackgroundRect;
    innerRect.Shrink( TSize( 5, 5) );
	if (IsFocused())
		{
   			
   		AknsDrawUtils::DrawFrame( skinInstance, gc, iBackgroundRect, 
   						innerRect,KAknsIIDQsnFrGrid, KAknsIIDQsnFrGridCenter);		
		}
    					
	gc.BitBltMasked(iBackgroundRect.iTl,iBgBitmap, TRect(TPoint(), 
					iBackgroundRect.Size()), iBgBitmapMask, ETrue);
	
	TInt halfSliderSize = iSliderRect.Height() / 2;
	TInt sliderRectYCoord = iSliderRect.iTl.iY;
	TInt yCoordOffset = ((iMoverPos - sliderRectYCoord) - halfSliderSize);
	TRect tempSliderRect = iSliderRect;
	tempSliderRect.Move(0,yCoordOffset);
	
    gc.BitBltMasked(tempSliderRect.iTl,iSliderBitmap,TRect(TPoint(), 
    						tempSliderRect.Size()),iSliderBitmapMask,ETrue);
	}
// -------------------------------------------------------------------
// void CEqualizerFreqBand::SizeChanged()
//
// This function is called by the framework whenever the control size 
// is changed
// --------------------------------------------------------------------
void CEqualizerFreqBand::SizeChanged()
    {
	TInt variety = EEqualizerVarietyOne;

    CAudioEqualizer& audioEqualizer = iAudEqUtility->Equalizer();
    TUint numberOfFreqBands = audioEqualizer.NumberOfBands();
        
	if(numberOfFreqBands == KEqualizerFiveBands)
	    {
	    variety = EEqualizerVarietyTwo;
	    }
   
	iScaleRect = ComponentRect(Rect(),
									variety,EEqualizerScale);
	iBackgroundRect = ComponentRect(iScaleRect,
								EEqualizerVarietyNotUsed,EEqualizerBackground);
 	iSliderRect = ComponentRect(iScaleRect,
 									EEqualizerVarietyNotUsed,EEqualizerSlider);

#if defined(RD_TACTILE_FEEDBACK)
	MTouchFeedback* feedback = MTouchFeedback::Instance();
	if (feedback)
		{
		feedback->SetFeedbackArea(this, 0, iBackgroundRect,
			ETouchFeedbackBasic, ETouchEventStylusDown);
		}
#endif

	AknIconUtils::SetSize(iBgBitmap,iBackgroundRect.Size(), EAspectRatioNotPreserved);
	AknIconUtils::SetSize(iBgBitmapMask,iBackgroundRect.Size(), EAspectRatioNotPreserved);
	
	AknIconUtils::SetSize(iSliderBitmap,iSliderRect.Size(), EAspectRatioNotPreserved);
    AknIconUtils::SetSize(iSliderBitmapMask,iSliderRect.Size(), EAspectRatioNotPreserved);
    
    TInt scaleHeight = iBackgroundRect.Height();
    TInt zeroCoord = iBackgroundRect.iTl.iY + (scaleHeight / 2);
    iZeroPos = zeroCoord;  
    iMoverPos = iEqEditingDialog->GetMoverPosition(iBandIndex);
    
    TInt sliderInc = iBackgroundRect.Height() / KNumberOfSteps;
    TReal res,frac;
    frac = (TReal)(iMoverPos - iZeroPos) / sliderInc;
    Math::Round(res,frac,0);
    Math::Int(iStepCount,res);
    iStepCount += KNumberOfSteps / 2;
    if(iStepCount > KNumberOfSteps)
    	{
    	iStepCount = KNumberOfSteps;
    	}
    if(iStepCount < 0)
    	{
    	iStepCount = 0;
    	}

    }

// -------------------------------------------------------------------
// void CEqualizerFreqBand::FocusChanged(TDrawNow aDrawNow)
//
// This function is called by the framework whenever the control's 
// focus is changed
// --------------------------------------------------------------------
//
void CEqualizerFreqBand::FocusChanged(TDrawNow aDrawNow)
    {
    if (aDrawNow)
        {
        DrawNow();        
        }
    }
    
// -------------------------------------------------------------------
// void CEqualizerFreqBand::HandleResourceChange(TInt aType)
//
// This function is called by the framework whenever there is a 
// change in skin
// -------------------------------------------------------------------
void CEqualizerFreqBand::HandleResourceChange(TInt aType)
    {
    if (aType == KAknsMessageSkinChange)
        {
        TInt tempMoverPos = iMoverPos;
        LoadBitmapsL();
        SizeChanged();
        iMoverPos = tempMoverPos;
        DrawNow();
        }
    }

// -------------------------------------------------------------------
// void CEqualizerFreqBand::LoadBitmapsL()
// This function is called to load the bitmaps from the icons file
// --------------------------------------------------------------------
//     
void CEqualizerFreqBand::LoadBitmapsL()
    {
    // Find which drive this DLL is installed.
    TFileName fileName;
    // Get the full resource file
    TParse fp;
    // Add the resource dir
    fp.Set( KIconsFilename, &KDC_APP_BITMAP_DIR, NULL);
    // Get the filename with full path
    fileName = fp.FullName(); 

    MAknsSkinInstance * skinInstance;
    skinInstance = AknsUtils::SkinInstance();
    
    //Destroying previously created background icons
    if (iBgBitmap)
    	{
    	delete iBgBitmap;
    	iBgBitmap = NULL;
    	}
    	
    if (iBgBitmapMask)
    	{
    	delete iBgBitmapMask;
    	iBgBitmapMask = NULL;
    	}
    
    //Destroying previously created slider icons
    if (iSliderBitmap)
    	{
    	delete iSliderBitmap;
    	iSliderBitmap = NULL;
    	}
    
    if (iSliderBitmapMask)
    	{
    	delete iSliderBitmapMask;
    	iSliderBitmapMask = NULL;
    	}
    
    //Creating the background icon
    AknsUtils::CreateIconL(skinInstance,KAknsIIDQgnIndiMupEqBg,
                           iBgBitmap,iBgBitmapMask,fileName,
                           EMbmEqualizerQgn_indi_mup2_eq_bg,
                           EMbmEqualizerQgn_indi_mup2_eq_bg_mask);
    
    //Creating the slider icon
    AknsUtils::CreateIconL(skinInstance,KAknsIIDQgnIndiMupEqSlider,
                           iSliderBitmap,iSliderBitmapMask,fileName,
                           EMbmEqualizerQgn_indi_mup_eq_slider,
                           EMbmEqualizerQgn_indi_mup_eq_slider_mask);
    }       

// -------------------------------------------------------------------
// void CEqualizerFreqBand::UpdateBandLevelInEngineL()
// This function is called to update the engine with the new band level
// --------------------------------------------------------------------
// 
void CEqualizerFreqBand::UpdateBandLevelInEngineL(TBool lastIncFlag)
    {
    //Update engine
	TInt sliderInc = iBackgroundRect.Height() / KNumberOfSteps;
    TInt diffUIBandLevel = KNumberOfSteps / 2;
    TInt bandLevel;
    TReal frac;
    TInt sliderNoOfPix;
    TInt actualNoOfPix;
    TInt pixDiff;
    TReal fracDiff;
        
    if (iMoverPos > iZeroPos)
        {
        frac = (TReal)(iMoverPos - iZeroPos) / ((TReal)diffUIBandLevel * sliderInc);
		if(lastIncFlag)
			{
			sliderNoOfPix = diffUIBandLevel * sliderInc;
			actualNoOfPix = iMoverPos - iZeroPos;
			if(sliderNoOfPix > actualNoOfPix)
				{
				pixDiff = sliderNoOfPix - actualNoOfPix;
				fracDiff = (TReal)pixDiff / ((TReal)diffUIBandLevel * sliderInc);
				frac += fracDiff;				
				}
			else
				{
				pixDiff = actualNoOfPix - sliderNoOfPix;
				fracDiff = (TReal)pixDiff / ((TReal)diffUIBandLevel * sliderInc);
				frac -= fracDiff;				
				}
			}
		if (frac > 1)
			{
			frac = 1;
			}
		else
		if (frac < -1)
			{
			frac = -1;
			}
        bandLevel=(TInt)(frac * iMinBandLevel);        
        }
    else if (iMoverPos < iZeroPos)
        {
        frac = (TReal)(iZeroPos - iMoverPos) / ((TReal)diffUIBandLevel * sliderInc);
        if(lastIncFlag)						
			{
			sliderNoOfPix = diffUIBandLevel * sliderInc;
			actualNoOfPix = iZeroPos - iMoverPos;
			if(sliderNoOfPix > actualNoOfPix)
				{
				pixDiff = sliderNoOfPix - actualNoOfPix;
				fracDiff = (TReal)pixDiff / ((TReal)diffUIBandLevel * sliderInc);
				frac += fracDiff;				
				}
			else
				{
				pixDiff = actualNoOfPix - sliderNoOfPix;
				fracDiff = (TReal)pixDiff / ((TReal)diffUIBandLevel * sliderInc);
				frac -= fracDiff;				
				}
			}
		if (frac > 1)
			{
			frac = 1;
			}
		else
		if (frac < -1)
			{
			frac = -1;
			}
        bandLevel=(TInt)(frac * iMaxBandLevel);   
        }
    else
        {
        bandLevel = 0;
        }   
       
//RDebug::Printf("Todd1 ==> bnad index=%d, Level set = %d", iBandIndex+1, bandLevel);
    iAudioEqualizer->SetBandLevelL(iBandIndex+1, bandLevel);     

    if (iIsActivePreset) 
        {
        TInt err;
        TRAP(err, iAudioEqualizer->EnableL());	
        }
   }

// -------------------------------------------------------------------
// TKeyResponse CEqualizerFreqBand::OfferKeyEventL
// (const TKeyEvent& aKeyEvent,TEventCode aType)
// This function is called to handle key events
// --------------------------------------------------------------------
//
TKeyResponse CEqualizerFreqBand::OfferKeyEventL(const TKeyEvent& aKeyEvent,
TEventCode aType)
    {
    TKeyResponse ret = EKeyWasNotConsumed;
    
    TInt halfSliderSize = iSliderRect.Height() / 2;
    TInt sliderInc = iBackgroundRect.Height() / KNumberOfSteps;
	TInt nextInc;
	TBool lastIncFlag = 0;
	
    if (aType==EEventKey && (aKeyEvent.iScanCode==EStdKeyDownArrow ))
        {
        if (iMoverPos < (iBackgroundRect.iBr.iY - halfSliderSize))
         	{
            nextInc = sliderInc;
            iStepCount += 1;
            if(((iMoverPos + nextInc) <= 
            				(iBackgroundRect.iBr.iY - halfSliderSize))
            				&& (iStepCount != KNumberOfSteps))
            	{
            	TInt offset = Abs(iMoverPos - iZeroPos) % sliderInc;
            	if(offset==0)
            		iMoverPos += nextInc ;
            	else
            		{
            		//iStepCount--;
            		if(iMoverPos>iZeroPos)	
            	   		iMoverPos+= sliderInc - offset;
            	
            		if(iMoverPos<iZeroPos)
            	    	iMoverPos+= sliderInc + offset;
            		}
            	}
		    else
		    	{
		    	nextInc = (iBackgroundRect.iBr.iY - halfSliderSize)
		    								 - iMoverPos;
		    	iMoverPos += nextInc;
		    	
		    	if(iStepCount > KNumberOfSteps )
		    	    iStepCount = KNumberOfSteps;
		    	}
            if(((iMoverPos) == (iBackgroundRect.iBr.iY - halfSliderSize)) ||
            			(iStepCount == KNumberOfSteps))
            	{
            	lastIncFlag = 1;
            	}
            //Update engine
            UpdateBandLevelInEngineL(lastIncFlag);
            }
        DrawNow();
        ret = EKeyWasConsumed;
        }
    else if (aType==EEventKey && (aKeyEvent.iScanCode==EStdKeyUpArrow))
        {
        if (iMoverPos > (iBackgroundRect.iTl.iY+halfSliderSize))
            {
            nextInc = sliderInc;
            iStepCount -= 1;
            if(((iMoverPos - nextInc) >= (iBackgroundRect.iTl.iY + halfSliderSize))
            			&& (iStepCount != 0))
            	{
            	TInt offset = Abs(iMoverPos - iZeroPos) % sliderInc;
            	if(offset==0)
               		iMoverPos -= nextInc;
            	else
            		{
            		//iStepCount++; 
            		if(iMoverPos>iZeroPos)	
            	   		iMoverPos-= nextInc + offset;
            	
            		if(iMoverPos<iZeroPos)
            	   		iMoverPos-= sliderInc - offset;
            		}
            	}
            else
            	{
            	nextInc = iMoverPos - (iBackgroundRect.iTl.iY + halfSliderSize);
            	iMoverPos -= nextInc;
            	
            	if(iStepCount < 0 )
		    	    iStepCount = 0;
            	}
            if(((iMoverPos) == (iBackgroundRect.iTl.iY + halfSliderSize)) ||
            			(iStepCount == 0))
            	{
            	lastIncFlag = 1;
            	}
            //Update engine
            UpdateBandLevelInEngineL(lastIncFlag);
            }
        DrawNow();
        ret = EKeyWasConsumed;
        }
    else
        {
        ret = EKeyWasNotConsumed;
        }
    return ret;
    }

// ----------------------------------------------------------------------------
// TRect CEqualizerFreqBand::ComponentRect(const TRect& aRect, TInt aVariety, 
//                                                    TInt aComponentNumber)
// This function is called to return the rect of a control after reading the 
// layout information from the CDL file. It returns the rect for the control 
// based on the Enumerated member sent as a parameter
// ----------------------------------------------------------------------------
//
TRect CEqualizerFreqBand::ComponentRect(const TRect& aRect, TInt aVariety, 
                                                    TInt aComponentNumber)
    {
    TAknWindowComponentLayout lLayout;
    
    switch(aComponentNumber)
        {
        case EEqualizerFreqBand1:    lLayout = AknLayoutScalable_Apps::
                                            mup_equalizer_pane(aVariety);
                                    break;
                                    
        case EEqualizerFreqBand2:    lLayout = AknLayoutScalable_Apps::
                                            mup_equalizer_pane_cp1(aVariety);
                                    break;
                                    
        case EEqualizerFreqBand3:    lLayout = AknLayoutScalable_Apps::
                                            mup_equalizer_pane_cp2(aVariety);
                                    break;
                                    
        case EEqualizerFreqBand4:    lLayout = AknLayoutScalable_Apps::
                                            mup_equalizer_pane_cp3(aVariety);
                                    break;
                                    
        case EEqualizerFreqBand5:    lLayout = AknLayoutScalable_Apps::
                                            mup_equalizer_pane_cp4(aVariety);
                                    break;

        case EEqualizerFreqBand6:    lLayout = AknLayoutScalable_Apps::
                                            mup_equalizer_pane_cp5(1);
                                    break;

        case EEqualizerFreqBand7:    lLayout = AknLayoutScalable_Apps::
                                            mup_equalizer_pane_cp6(1);
                                    break;

        case EEqualizerFreqBand8:    lLayout = AknLayoutScalable_Apps::
                                            mup_equalizer_pane_cp7(1);
                                    break;
                                    
        case EEqualizerMainPane:    lLayout = AknLayoutScalable_Apps::
                                            main_mup_eq_pane();
                                    break;

        case EEqualizerScale:         lLayout = AknLayoutScalable_Apps::
                                            mup_scale_pane(aVariety);
                                    break;

        case EEqualizerBackground:    lLayout = AknLayoutScalable_Apps::
                                            mup_scale_pane_g1();
                                    break;

        case EEqualizerSlider:        lLayout = AknLayoutScalable_Apps::
                                            mup_scale_pane_g2();
                                    break;

        default:                    break;
        }
    
    TAknLayoutRect lLayoutRect;
    lLayoutRect.LayoutRect(aRect,lLayout.LayoutLine());
    
    TRect rect = lLayoutRect.Rect();
    return (rect);
    }

// ----------------------------------------------------------------------------
// CEqualizerFreqBand::SetAudioEqualizerL(CAudioEqualizerUtility* 
// aAudEqUtility)
// This function is used to set a new CAudioEqualizerUtility* 
// ----------------------------------------------------------------------------
//    
void CEqualizerFreqBand::SetAudioEqualizerL(
CAudioEqualizerUtility* aAudEqUtility)
	{
	
	// Save the user's settings
	iAudEqUtility->ModifyPresetL(iPresetIndex, iPresetName, *iAudioEqualizer);
	
	// Set new value for CAudioEqualizerUtility
	__ASSERT_DEBUG((iAudEqUtility != NULL), User::Invariant());
	iAudEqUtility = aAudEqUtility;
	iPresetName = iAudEqUtility->GetPresetL(iPresetIndex);
    iAudioEqualizer = &iAudEqUtility->Equalizer();
	}

#ifdef RD_SCALABLE_UI_V2
// ----------------------------------------------------------------------------
// CEqualizerFreqBand::HandlePointerEventL(const TPointerEvent&  
// aPointerEvent)
// This function is used handle pen input events 
// ----------------------------------------------------------------------------
//    
void CEqualizerFreqBand::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
    TInt pos;
	TInt halfSliderSize = iSliderRect.Height() / 2;
    TInt sliderInc = iBackgroundRect.Height() / KNumberOfSteps;
    TInt offset;
	TBool lastIncFlag = 0;
#if defined(RD_TACTILE_FEEDBACK)
	MTouchFeedback* feedback = NULL;
#endif

	if ( aPointerEvent.iPosition.iY < iBackgroundRect.iTl.iY )
		{
		pos = iBackgroundRect.iTl.iY;
		}
	else if ( aPointerEvent.iPosition.iY > iBackgroundRect.iBr.iY )
		{
		pos = iBackgroundRect.iBr.iY;
		}
	else
		{
		pos = aPointerEvent.iPosition.iY;
		}

	if (AknLayoutUtils::PenEnabled())
		{
		switch (aPointerEvent.iType)
            {
            case TPointerEvent::EButton1Down:
                break;

            case TPointerEvent::EButton1Up:
            	offset = Abs(pos- iZeroPos) % sliderInc;

            	if(pos<= iZeroPos)
	            	{
	               	if(offset!=0)
            			{
	            		if(offset <= (sliderInc/2))
	            	   		pos+= offset;
	            		else
	            	    	pos+= offset - sliderInc;
	            		}
	            	}
	            else
	            	{
	            	if(offset!=0)
		            	{
		            	if(offset <= (sliderInc/2))
	            	   		pos-= offset;
	            	    else
	            	   		pos+= sliderInc - offset;
		            	}
	            	}

	            iStepCount = (pos - iBackgroundRect.iTl.iY)/sliderInc;

	            if(iStepCount == 0)
		            {
		            lastIncFlag = 1;
		            pos = iBackgroundRect.iTl.iY + halfSliderSize;
		            }

	            if(iStepCount >= KNumberOfSteps)
		            {
		            lastIncFlag = 1;
		            pos = iBackgroundRect.iBr.iY - halfSliderSize;
		            }

                iMoverPos = pos;
	            UpdateBandLevelInEngineL(lastIncFlag);
	            DrawNow();
                break;

            case TPointerEvent::EDrag:
#if defined(RD_TACTILE_FEEDBACK)
            	feedback = MTouchFeedback::Instance();
				if (feedback)
				    {
					feedback->InstantFeedback(ETouchFeedbackSensitive);
	                }
#endif
                if(pos< (iBackgroundRect.iTl.iY + halfSliderSize))
                    {
                	pos = iBackgroundRect.iTl.iY + halfSliderSize;
                	lastIncFlag = 1;
                    }

                if(pos> (iBackgroundRect.iBr.iY - halfSliderSize))
                    {
                	pos=iBackgroundRect.iBr.iY - halfSliderSize;
                	lastIncFlag = 1;
                    }

                iMoverPos = pos;
	            UpdateBandLevelInEngineL(lastIncFlag);
	            DrawNow();
                break;
            default:
//RDebug::Printf("Todd2 %d=", aPointerEvent.iType);
                break;
            }
		}
	}
#endif //RD_SCALABLE_UI_V2
// End of File
