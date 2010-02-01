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
* Description:  Definition of central repository keys for Fm Preset Utility.
*
*/


#ifndef RADIOFMPRESETCRKEYS_H
#define RADIOFMPRESETCRKEYS_H

// FM Preset Utility UID
const TUid KCRUidFmPresetUtility = {0x10207B87}; 

// The first preset channel's location in central repository
const TUint32 KFmRadioBookKeepingLocation = 0x00000001;
const TUint32 KFmRadioPresetChannel1 = 0x00001000;
const TUint32 KNbrOfParametersPerChannelSequence = 0x10;
const TUint32 KChannelNameOffset = 0x0;
const TUint32 KFrequencyOffset = 0x1;
const TUint32 KUrlOffset = 0x2;
const TUint32 KRestOfParametersOffset = 0xF;

#endif      // RADIOFMPRESETCRKEYS_H

// End of file


