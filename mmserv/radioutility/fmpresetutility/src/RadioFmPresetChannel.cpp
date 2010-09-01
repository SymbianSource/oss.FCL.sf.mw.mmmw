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
* Description:   Contains implementation of the TPresetChannel class, which holds 
*                       the FmRadioUtility settings.
*
*/


//  INCLUDES
#include <s32mem.h>
#include <RadioFmPresetUtility.h>
#include "RadioFmPresetUtilityBody.h"
#include <RadioPresetUtility.h>
#include "RadioFmPresetChannel.h"
#include "debug.h"


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------
// TPresetChannel::TPresetChannel
// Default class constructor.
// Returns: None
// ----------------------------------------------------
//
TPresetChannel::TPresetChannel():
	iPresetChannelFrequency( KDefaultRadioFrequency ),
	iPresetChannelPTY(0),
	iPresetChannelPI(0)
    {
    }

// ----------------------------------------------------
// TPresetChannel::GetChannelFrequency
// retrieve current frequency
// Returns: iPresetChannelFrequency: the preset channel frequency
// ----------------------------------------------------
//
TInt TPresetChannel::GetChannelFrequency() const
    {
    return iPresetChannelFrequency;
    }

// ----------------------------------------------------
// TPresetChannel::GetChannelName
// set the current preset
// Returns: iPresetChannelName: preset channel name
// ----------------------------------------------------
//
TFmPresetName TPresetChannel::GetChannelName() const
    {
    return iPresetChannelName;
    }

// ----------------------------------------------------
// TPresetChannel::SetChannelName
// set the current preset
// Returns: None
// ----------------------------------------------------
//
void TPresetChannel::SetChannelName(
    const TFmPresetName& aStationName )  //new channel name to be set
    {
    iPresetChannelName = aStationName;
    }

// ----------------------------------------------------
// TPresetChannel::SetChannelName
// set the current preset
// Returns: None
// ----------------------------------------------------
//
void TPresetChannel::SetChannelUrl(
    const TFmPresetUrl& aStationUrl )  //new channel name to be set
    {
    iPresetChannelUrl = aStationUrl;
    }

// ----------------------------------------------------
// TPresetChannel::GetChannelUrl
// Get the current preset's Url
// Returns: iPresetChannelUrl: preset channel Url
// ----------------------------------------------------
//
TFmPresetUrl TPresetChannel::ChannelUrl() const
    {
    return iPresetChannelUrl;
    }

// ----------------------------------------------------
// TPresetChannel::SetChannelPty
// Set the current preset's PTY
// Returns: none
// ----------------------------------------------------
//
void TPresetChannel::SetChannelPty(const TUint aPTY)
    {
    iPresetChannelPTY = aPTY;
    }

// ----------------------------------------------------
// TPresetChannel::GetChannelPty
// Get the current preset's PTY
// Returns: iPresetChannelPTY: preset channel PTY
// ----------------------------------------------------
//
TUint TPresetChannel::ChannelPty() const
    {
    return iPresetChannelPTY;
    }

// ----------------------------------------------------
// TPresetChannel::SetChannelPI
// Set the current preset's PI-code
// Returns: none
// ----------------------------------------------------
//
void TPresetChannel::SetChannelPI(const TUint aPI)
    {
    iPresetChannelPI = aPI;
    }

// ----------------------------------------------------
// TPresetChannel::GetChannelPI
// Get the current preset's PI-code
// Returns: iPresetChannelPI: preset channel PI
// ----------------------------------------------------
//
TUint TPresetChannel::ChannelPI() const
    {
    return iPresetChannelPI;
    }

// ----------------------------------------------------
// ----------------------------------------------------
// TPresetChannel::SetChannelFavouriteInfo
// Set the current preset's favourite status bit
// Returns: none
// ----------------------------------------------------
//
void TPresetChannel::SetChannelFavouriteInfo(const TBool aFavourite)
    {
    iPresetChannelMisc.Assign( MiscInfo::Favourite, aFavourite );
    }

// ----------------------------------------------------
// TPresetChannel::GetChannelFavouriteInfo
// Get the current preset's favourite status bit
// Returns: iPresetChannelMisc: favourite status bit
// ----------------------------------------------------
//
TBool TPresetChannel::ChannelFavouriteInfo() const
    {
    TBool ret = EFalse;
    if (iPresetChannelMisc[ MiscInfo::Favourite ])
        {
        ret = ETrue;
        }
    return ret;
    }

// ----------------------------------------------------
// TPresetChannel::SetChannelUserRenameInfo
// Set the current preset's user rename status bit
// Returns: none
// ----------------------------------------------------
//
void TPresetChannel::SetChannelUserRenameInfo(const TBool aUserRename)
    {
    iPresetChannelMisc.Assign( MiscInfo::UserRename, aUserRename );
    }

// ----------------------------------------------------
// TPresetChannel::GetChannelUserRenameInfo
// Get the current preset's user rename status bit
// Returns: iPresetChannelMisc: user rename status bit
// ----------------------------------------------------
//
TBool TPresetChannel::ChannelUserRenameInfo() const
    {
    TBool ret = EFalse;
    if (iPresetChannelMisc[ MiscInfo::UserRename ])
        {
        ret = ETrue;
        }
    return ret;
    }

// ----------------------------------------------------
// TPresetChannel::SaveChannelFrequency
// Set the current preset frequency
// Returns: None
// ----------------------------------------------------
//
void TPresetChannel::SaveChannelFrequency(
    const TInt aFrequency )  //new preset channel frequency
    {
    iPresetChannelFrequency = aFrequency;
    }

// ----------------------------------------------------
// TPresetChannel::ExternalizeL
// Externalize data from buffer to stream
// Returns: None
// ----------------------------------------------------
//
void TPresetChannel::ExternalizeL(
    RWriteStream & aStream ) const  // a reference to a RDesWriteStream
    {
	aStream.WriteUint32L(iPresetChannelPTY);
	aStream.WriteUint32L(iPresetChannelPI);
	aStream.WriteUint32L(iPresetChannelMisc.Value());
    }

// ----------------------------------------------------
// TPresetChannel::InternalizeL
// Returns: None
// Interalize the stream
// ----------------------------------------------------
//
void TPresetChannel::InternalizeL(
    RReadStream & aStream )  // a reference to a RDesReadStream
    {
    iPresetChannelPTY = aStream.ReadInt32L();
    iPresetChannelPI = aStream.ReadInt32L();
    iPresetChannelMisc = aStream.ReadInt32L();    
    }

//  End of File  
