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
* Description:    This file contains declarations for private constants for 
*                Equalizer
*
*/



#ifndef EQUALIZERPRIVATECONSTANTS_H
#define EQUALIZERPRIVATECONSTANTS_H

//  INCLUDES

#include <e32std.h>
#include <bldvariant.hrh>
#include "EqualizerConstants.h"

// CONSTANTS

// Used as the maximum length of a Preset name
const TInt KMaxPresetNameLength = 32;
// Uid of Equalizer
const TUid KUidEqualizer = { 0x101FD659 };
// Used to set the array granularity for dynamic arrays
const TUint KEqualizerArrayGranularity = 4;
// Equalizer rsc file name
_LIT( KEqualizerResourceFileName, "Z:Equalizer.rsc" );
// Equalizer mif file name
_LIT(KIconsFilename, "Z:Equalizer.mbm");
// Used if the number of bands returned from engine is 8
const TInt KEqualizerEightBands = 8;
// Used if the number of bands returned from engine is 5
const TInt KEqualizerFiveBands = 5;
// Number of slider movements
const TInt KNumberOfSteps = 20;

// for old s60 musicplayer
//const TUid KCRUidMusicPlayerSettings = {0x101F880C};
//const TUint32 KMPlayerEqPresetId       = 0x00000009;
const TUid KCRUidMusicPlayerSettings = {0x101FFCDC};
const TUint32 KMPlayerEqPresetId = 0x00000001;

// MACROS
//Used in model text
_LIT(KEqrTab, "\t");

// DATA TYPES

#endif      // EQUALIZERPRIVATECONSTANTS_H

// End of File
