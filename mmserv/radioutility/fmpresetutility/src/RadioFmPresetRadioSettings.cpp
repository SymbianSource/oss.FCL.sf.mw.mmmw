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
* Description:  Contains implementation of the TRadioSettings class, which holds 
*                       the RadioFmPresetUtility settings.
*
*/


//  INCLUDES
#include <RadioFmPresetUtility.h>
#include <RadioPresetUtility.h>
#include "RadioFmPresetUtilityBody.h"
#include "RadioFmPresetRadioSettings.h"


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------
// TRadioSettings::TRadioSettings
// Default class constructor.
// ----------------------------------------------------
//
TRadioSettings::TRadioSettings():
                    iCurrentPreset(0),
                    iFrequency(KDefaultRadioFrequency),
                    iRadioOn(EFalse)
    {
    }

// ----------------------------------------------------
// TRadioSettings::CurrentPreset
// retrieve currently selected channel
// Returns: iCurrentPreset: current preset index number
// ----------------------------------------------------
//
TInt TRadioSettings::CurrentPreset() const
    {
    return iCurrentPreset;
    }

// ----------------------------------------------------
// TRadioSettings::Frequency
// retrieve current frequency
// Returns: iFrequency: current frequency
// ----------------------------------------------------
//
TInt TRadioSettings::Frequency() const
    {
    return iFrequency;
    }

// ----------------------------------------------------
// TRadioSettings::IsRadioOn
// retrieve current radio on state (on/off)
// Returns: iRadioOn: a flag indicating radio on/off
// ----------------------------------------------------
//
TBool TRadioSettings::IsRadioOn() const
    {
    return iRadioOn;
    }

// ----------------------------------------------------
// TRadioSettings::SetCurrentPreset
// set the current preset
// Returns: none
// ----------------------------------------------------
//
void TRadioSettings::SetCurrentPreset(
    const TInt aChannel) //new channel to be set
    {
    iCurrentPreset = aChannel;
    }

// ----------------------------------------------------
// TRadioSettings::SetFrequency
// set the current frequency
// Returns: none
// ----------------------------------------------------
//
void TRadioSettings::SetFrequency(
    const TInt aFrequency) // new frequency
    {
    iFrequency = aFrequency;
    }

// ----------------------------------------------------
// TRadioSettings::SetRadioOn
// set radio state to "on"
// Returns: none
// ----------------------------------------------------
//
void TRadioSettings::SetRadioOn()
    {
    iRadioOn = ETrue;
    }

// ----------------------------------------------------
// TRadioSettings::SetRadioOff
// set radio state to "off"
// Returns: none
// ----------------------------------------------------
//
void TRadioSettings::SetRadioOff()
    {
    iRadioOn = EFalse;
    }



// end of file

