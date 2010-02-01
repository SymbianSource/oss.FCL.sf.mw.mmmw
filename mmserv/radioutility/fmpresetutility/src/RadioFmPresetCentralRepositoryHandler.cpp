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
* Description:  Contains implementation of the CCentralRepositoryHandler class. 
*
*/


//  INCLUDE FILES
#include "RadioFmPresetCentralRepositoryHandler.h"
#include "RadioFmPresetRadioSettings.h" 
#include <centralrepository.h>
#include "RadioFmPresetCRKeys.h"
#include "RadioFmPresetUtilityBody.h"		
#include "debug.h"
#include <s32mem.h>


// ============= STATIC FUNCTIONS ==============

static TInt ChannelIndexL( const TInt aChannelIndex )
    {
    if ( ( KMaxNumberOfPresetStations <= aChannelIndex ) ||
            ( 0 >  aChannelIndex ) )
        {
        User::Leave( KErrArgument );
        }
    return KFmRadioPresetChannel1 +
        ( KNbrOfParametersPerChannelSequence * (aChannelIndex) );
    }

// ----------------------------------------------------
// CalculateNameIndexL
// Calculates key index for channel's name parameter for.
// Returns: key index of channel name parameter
// ----------------------------------------------------
// 
static TInt CalculateNameIndexL( TInt aChannelIndex )
    {
    return (ChannelIndexL( aChannelIndex ) + KChannelNameOffset);
    }

// ----------------------------------------------------
// CalculateFrequencyIndexL
// Calculates key index for channel's frequency parameter.
// Returns: key index of channel frequency parameter
// ----------------------------------------------------
// 
static TInt CalculateFrequencyIndexL( TInt aChannelIndex )
    {
    return (ChannelIndexL( aChannelIndex ) + KFrequencyOffset);
    }

// ----------------------------------------------------
// CalculateUrlIndexL
// Calculates key index for channel's URL parameter.
// Returns: key index of channel URL parameter
// ----------------------------------------------------
// 
static TInt CalculateUrlIndexL( TInt aChannelIndex )
    {
    return (ChannelIndexL( aChannelIndex ) + KUrlOffset);
    }

// ----------------------------------------------------
// CalculateRestOfParametersIndexL
// Calculates key index for channel's other parameters.
// Returns: key index of channel other parameters
// ----------------------------------------------------
// 
static TInt CalculateRestOfParametersIndexL( TInt aChannelIndex )
    {
    return (ChannelIndexL( aChannelIndex ) + KRestOfParametersOffset);
    }

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------
// CCentralRepositoryHandler::CCentralRepositoryHandler
// Default class constructor.
// ----------------------------------------------------
//
CCentralRepositoryHandler::CCentralRepositoryHandler(
    TRadioSettings& aRadioSettings)  // a reference to aRadioSettings
    :iRadioSettings(&aRadioSettings)
    {
    FTRACE(FPrint(_L("calling CCentralRepositoryHandler::NewL()")));
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::ConstructL
// Second phase class constructor.
// ----------------------------------------------------
//
void CCentralRepositoryHandler::ConstructL()
    {
    RDebug::Print(_L("entering CCentralRepositoryHandler::ConstructL()"));
    TRAPD (err, iCentralRepository = CRepository::NewL(KCRUidFmPresetUtility));
    RDebug::Print(_L("err[%d]"), err);
    RDebug::Print(_L("calling CCentralRepositoryHandler::RetrieveInitialSettings()"));
    RetrieveInitialSettings();
    RDebug::Print(_L("exiting CCentralRepositoryHandler::ConstructL()"));
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::NewL
// Two-phased class constructor.
// ----------------------------------------------------
//
CCentralRepositoryHandler* CCentralRepositoryHandler::NewL(
    TRadioSettings& aRadioSettings)  // a reference to aRadioSettings
    {
    RDebug::Print(_L("entering CCentralRepositoryHandler::NewL()"));    	
    CCentralRepositoryHandler *self = new(ELeave) CCentralRepositoryHandler(aRadioSettings);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::~CCentralRepositoryHandler
// Destructor of CCentralRepositoryHandler class.
// ----------------------------------------------------
//
CCentralRepositoryHandler::~CCentralRepositoryHandler()
    {
    iRadioSettings = NULL;
	  if(iCentralRepository)
	     {
	     delete iCentralRepository;
	     }
    }


// ----------------------------------------------------
// CCentralRepositoryHandler::RetrieveInitialSettings
// Retrieves persistent settings from shared data.
// Returns: None
// ----------------------------------------------------
//
void CCentralRepositoryHandler::RetrieveInitialSettings()
    {
    
    

    }

// ----------------------------------------------------
// CCentralRepositoryHandler::RetrievePresetFrequency
// Retrieve the preset frequency from shared data
// Returns: None
// ----------------------------------------------------
//
TInt CCentralRepositoryHandler::RetrievePresetFrequencyL(
    TInt aIndex) const  // the preset index
    {
    TInt tempFrequency( KErrNotFound );
    // Read frequency from its own key in the key sequence
    iCentralRepository->Get( CalculateFrequencyIndexL( aIndex ), tempFrequency );
    return tempFrequency;
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::RetrievePresetName
// Retrieve the preset name from shared data
// Returns: None
// ----------------------------------------------------
//
TFmPresetName CCentralRepositoryHandler::RetrievePresetNameL(
    TInt aIndex) const  // the preset channel index
    {
    // Read name from its own key in the key sequence
    TInt nameIndex( CalculateNameIndexL( aIndex ) );
    TFmPresetName tempChannelName;
    iCentralRepository->Get(nameIndex, tempChannelName );
    return tempChannelName;
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::RetrievePresetUrl
// Retrieve the preset Url from shared data
// Returns: None
// ----------------------------------------------------
//
TFmPresetUrl CCentralRepositoryHandler::RetrievePresetUrlL(
    TInt aIndex) const  // the preset channel index
    {    
    // Read url from its own key in the key sequence
    TInt urlIndex( CalculateUrlIndexL( aIndex ) );
    TFmPresetUrl tempUrlName;
    iCentralRepository->Get(urlIndex, tempUrlName );   
    return tempUrlName;     //ownership moves to the caller
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::RetrievePresetPTYL
// Retrieve the preset PTY from shared data
// Returns: tempPTY: the channel PTY
// ----------------------------------------------------
//
TUint CCentralRepositoryHandler::RetrievePresetPTYL(
    TInt aIndex) const  // the preset index
    {
    TUint tempPTY = KDefaultRadioPTY;
    HBufC8* buf = HBufC8::NewLC( sizeof(TPresetChannel) );
    TPtr8 bufPtr = buf->Des();
    TInt tempChannelIndex( CalculateRestOfParametersIndexL( aIndex ) );
    TInt errCode = iCentralRepository->Get( tempChannelIndex, bufPtr );
    if(KErrNone == errCode)
        {
        if (bufPtr.Length() != 0)
            {
            tempPTY = ConvertStringToChannelPTYL(bufPtr);
            }
        } 
    CleanupStack::PopAndDestroy(buf);
    return tempPTY;
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::RetrievePresetPIL
// Retrieve the preset PI from shared data
// Returns: tempPI: the channel PI
// ----------------------------------------------------
//
TUint CCentralRepositoryHandler::RetrievePresetPIL(
    TInt aIndex) const  // the preset index
    {
    TUint tempPI = KDefaultRadioPI;
    HBufC8* buf = HBufC8::NewLC( sizeof(TPresetChannel) );
    TPtr8 bufPtr = buf->Des();
    TInt tempChannelIndex( CalculateRestOfParametersIndexL( aIndex ) );
    TInt errCode = iCentralRepository->Get( tempChannelIndex, bufPtr );
    if(KErrNone == errCode)
        {
        if (bufPtr.Length() != 0)
            {
            tempPI = ConvertStringToChannelPIL(bufPtr);
            }
        } 
    CleanupStack::PopAndDestroy(buf);
    return tempPI;
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::RetrievePresetFavouriteL
// Retrieve the preset favourite information from shared data
// Returns: tempFav: the channel local station flag
// ----------------------------------------------------
//
TBool CCentralRepositoryHandler::RetrievePresetFavouriteL(
    TInt aIndex) const  // the preset index
    {
    TBool tempFav = EFalse;
    HBufC8* buf = HBufC8::NewLC(sizeof(TPresetChannel));
    TPtr8 bufPtr = buf->Des();
    TInt tempChannelIndex( CalculateRestOfParametersIndexL( aIndex ) );
    TInt errCode = iCentralRepository->Get(tempChannelIndex, bufPtr);
    if(KErrNone == errCode)
    {
    if (bufPtr.Length() != 0)
        {
        tempFav = ConvertStringToChannelFavouriteL(bufPtr);
        }
    } 
    CleanupStack::PopAndDestroy(buf);
    return tempFav;
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::RetrievePresetUserRenameL
// Retrieve the preset user rename information from shared data
// Returns: tempRen: the channel user rename flag
// ----------------------------------------------------
//
TBool CCentralRepositoryHandler::RetrievePresetUserRenameL(
    TInt aIndex) const  // the preset index
    {
    TBool tempRen = EFalse;
    HBufC8* buf = HBufC8::NewLC(sizeof(TPresetChannel));
    TPtr8 bufPtr = buf->Des();
    TInt tempChannelIndex( CalculateRestOfParametersIndexL( aIndex ) );
    TInt errCode = iCentralRepository->Get(tempChannelIndex, bufPtr);
    if(KErrNone == errCode)
        {
        if (bufPtr.Length() != 0)
            {
            tempRen = ConvertStringToChannelUserRenameL(bufPtr);
            }
        } 
    CleanupStack::PopAndDestroy(buf);
    return tempRen;
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::SavePresetFrequency
// Save channel preset info back to shared data
// Returns: None
// ----------------------------------------------------
//
void CCentralRepositoryHandler::SavePresetFrequencyL(
    TInt aIndex,   // preset channel index
    TInt aFrequency)  //new frequency
    {    
    TInt frequencyIndex( CalculateFrequencyIndexL( aIndex ) );
    User::LeaveIfError( iCentralRepository->Set(frequencyIndex, aFrequency ) );
    if ( 0 >= aFrequency )
        {
        // aFrequency values 0, -1(Or <=0) means that it is better to destroy
        // rest of parameters and also URL
        iCentralRepository->Delete( CalculateRestOfParametersIndexL( aIndex ) );
        iCentralRepository->Delete( CalculateUrlIndexL( aIndex ) );
        }
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::SavePresetName
// Save channel preset name back to shared data
// Returns: None
// ----------------------------------------------------
//
void CCentralRepositoryHandler::SavePresetNameL(
    TInt aIndex,  //the preset channel index
    const TFmPresetName& aStationName )  //preset channel name
    {
    TInt nameIndex( CalculateNameIndexL(aIndex) );
    TPtrC namePtr( aStationName );
    User::LeaveIfError( iCentralRepository->Set( nameIndex, namePtr) );
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::SavePresetUrlL
// Save channel preset url back to shared data
// Returns: None
// ----------------------------------------------------
//
void CCentralRepositoryHandler::SavePresetUrlL(
    TInt aIndex,  //the preset channel index
    const TFmPresetUrl& aStationUrl )  //preset Url
    {
    TInt urlIndex( CalculateUrlIndexL(aIndex) );
    TPtrC urlPtr( aStationUrl );
    User::LeaveIfError( iCentralRepository->Set( urlIndex, urlPtr ) );
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::SavePresetPIL
// Save channel PI code back to shared data
// Returns: None
// ----------------------------------------------------
//
void CCentralRepositoryHandler::SavePresetPIL(
    TInt aIndex,   // preset channel index
    TUint aPI)     // new programme identification code
    {    
    TPresetChannel presetChannel;
    TInt restOfParametersIndex( CalculateRestOfParametersIndexL( aIndex ) );
    HBufC8* buf = HBufC8::NewLC(sizeof(TPresetChannel));
    TPtr8 bufPtr = buf->Des();
    UpdateChannelObjectWithPIL(aIndex, presetChannel, aPI); 
    ExternalizeToStreamL(presetChannel, bufPtr); 
    User::LeaveIfError( iCentralRepository->Set(restOfParametersIndex, bufPtr ) );
    CleanupStack::PopAndDestroy(buf);
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::SavePresetPTYL
// Save channel PTY code back to shared data
// Returns: None
// ----------------------------------------------------
//
void CCentralRepositoryHandler::SavePresetPTYL(
    TInt aIndex,   // preset channel index
    TUint aPTY)    // new programme identification code
    {
    TPresetChannel presetChannel;
    TInt restOfParametersIndex( CalculateRestOfParametersIndexL( aIndex ) );
    HBufC8* buf = HBufC8::NewLC(sizeof(TPresetChannel));
    TPtr8 bufPtr = buf->Des();
    UpdateChannelObjectWithPTYL(aIndex, presetChannel, aPTY); 
    ExternalizeToStreamL(presetChannel, bufPtr); 
    User::LeaveIfError( iCentralRepository->Set(restOfParametersIndex, bufPtr ) );
    CleanupStack::PopAndDestroy(buf);
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::SavePresetFavouriteInfoL
// Save channel favourite station info back to shared data
// Returns: None
// ----------------------------------------------------
//
void CCentralRepositoryHandler::SavePresetFavouriteInfoL(
    TInt aIndex,   // preset channel index
    TBool aFav)    // new programme favourite status
    {
    TPresetChannel presetChannel ;
    TInt restOfParametersIndex( CalculateRestOfParametersIndexL( aIndex ) );
    HBufC8* buf = HBufC8::NewLC(sizeof(TPresetChannel));
    TPtr8 bufPtr = buf->Des();
    UpdateChannelObjectWithFavouriteInfoL(aIndex, presetChannel, aFav); 
    ExternalizeToStreamL(presetChannel, bufPtr); 
    User::LeaveIfError( iCentralRepository->Set(restOfParametersIndex, bufPtr ) );
    CleanupStack::PopAndDestroy(buf);
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::SavePresetUserRenameInfoL
// Save channel favourite station info back to shared data
// Returns: None
// ----------------------------------------------------
//
void CCentralRepositoryHandler::SavePresetUserRenameInfoL(
    TInt aIndex,   // preset channel index
    TBool aUR)     // new programme user rename status
    {
    TPresetChannel presetChannel;
    TInt restOfParametersIndex( CalculateRestOfParametersIndexL( aIndex ) );
    HBufC8* buf = HBufC8::NewLC(sizeof(TPresetChannel));
    TPtr8 bufPtr = buf->Des();
    UpdateChannelObjectWithUserRenameInfoL(aIndex, presetChannel, aUR); 
    ExternalizeToStreamL(presetChannel, bufPtr); 
    User::LeaveIfError( iCentralRepository->Set(restOfParametersIndex, bufPtr ) );
    CleanupStack::PopAndDestroy(buf);
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::SaveFmPresetSettings
// Save all FmPreset related persistent info back to shared data
// Returns: None
// ----------------------------------------------------
//
void CCentralRepositoryHandler::SaveFmPresetSettings()
    {
 /* DEPRECATED. Not used anymore    
    iCentralRepository->Set( KFmRadioFrequency, iRadioSettings->Frequency() );
    iCentralRepository->Set( KFmRadioChannel, iRadioSettings->CurrentPreset() );
    */
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::ConvertStringToChannelPTYL
// Get the channel programme type code from the externalized stream
// Returns: tempPTY: the channel PTY
//          ?value_n: ?description
//                    ?description
// ----------------------------------------------------
//
TUint CCentralRepositoryHandler::ConvertStringToChannelPTYL(
    const TDesC8& aString) const  // a reference to the raw string
    {
RDebug::Print (_L ("Entering CCentralRepositoryHandler::ConvertStringToChannelPTYL"));        
    TUint tempPTY = KDefaultRadioPTY;
    TPresetChannel tempPresetChannel;
    RDesReadStream inStream;
    inStream.Open(aString);
    CleanupClosePushL(inStream);
    tempPresetChannel.InternalizeL(inStream);
    CleanupStack::PopAndDestroy(&inStream);
    tempPTY = tempPresetChannel.ChannelPty();
RDebug::Print (_L ("Exiting CCentralRepositoryHandler::ConvertStringToChannelPTYL"));             
    return tempPTY;
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::ConvertStringToChannelPIL
// Get the channel programme identification code from the externalized stream
// Returns: tempPI: the channel PI
//          ?value_n: ?description
//                    ?description
// ----------------------------------------------------
//
TUint CCentralRepositoryHandler::ConvertStringToChannelPIL(
    const TDesC8& aString) const  // a reference to the raw string
    {
RDebug::Print (_L ("Entering CCentralRepositoryHandler::ConvertStringToChannelPIL"));        
    TUint tempPI = KDefaultRadioPI;
    TPresetChannel tempPresetChannel;
    RDesReadStream inStream;
    inStream.Open(aString);
    CleanupClosePushL(inStream);
    tempPresetChannel.InternalizeL(inStream);
    CleanupStack::PopAndDestroy(&inStream);
    tempPI = tempPresetChannel.ChannelPI();
RDebug::Print (_L ("Exiting CCentralRepositoryHandler::ConvertStringToChannelPIL"));             
    return tempPI;
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::ConvertStringToChannelFavouriteL
// Get the channel favourite flag from the externalized stream
// Returns: tempFav: the channel favourite flag
//          ?value_n: ?description
//                    ?description
// ----------------------------------------------------
//
TBool CCentralRepositoryHandler::ConvertStringToChannelFavouriteL(
    const TDesC8& aString) const  // a reference to the raw string
    {
RDebug::Print (_L ("Entering CCentralRepositoryHandler::ConvertStringToChannelFavouriteL"));        
    TBool tempFav = EFalse;
    TPresetChannel tempPresetChannel;
    RDesReadStream inStream;
    inStream.Open(aString);
    CleanupClosePushL(inStream);
    tempPresetChannel.InternalizeL(inStream);
    CleanupStack::PopAndDestroy(&inStream);
    tempFav = tempPresetChannel.ChannelFavouriteInfo();
RDebug::Print (_L ("Exiting CCentralRepositoryHandler::ConvertStringToChannelFavouriteL"));             
    return tempFav;
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::ConvertStringToChannelUserRenameL
// Get the channel user rename flag from the externalized stream
// Returns: tempRen: the channel favourite flag
//          ?value_n: ?description
//                    ?description
// ----------------------------------------------------
//
TBool CCentralRepositoryHandler::ConvertStringToChannelUserRenameL(
    const TDesC8& aString) const  // a reference to the raw string
    {
RDebug::Print (_L ("Entering CCentralRepositoryHandler::ConvertStringToChannelUserRenameL"));        
    TBool tempRen = EFalse;
    TPresetChannel tempPresetChannel;
    RDesReadStream inStream;
    inStream.Open(aString);
    CleanupClosePushL(inStream);
    tempPresetChannel.InternalizeL(inStream);
    CleanupStack::PopAndDestroy(&inStream);
    tempRen = tempPresetChannel.ChannelUserRenameInfo();
RDebug::Print (_L ("Exiting CCentralRepositoryHandler::ConvertStringToChannelUserRenameL"));             
    return tempRen;
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::UpdateChannelObjectWithPIL
// Update the current preset channel's PI code 
// Returns: none
// ----------------------------------------------------
//
void CCentralRepositoryHandler::UpdateChannelObjectWithPIL (
    TInt aIndex,   //the channel index
    TPresetChannel& aPresetChannel, // reference to channel object
    TUint aPI) // new frequency
    {
    //Initialize the channel object with correct information.
    GetPresetL( aIndex, aPresetChannel );    
    aPresetChannel.SetChannelPI(aPI);
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::UpdateChannelObjectWithPTYL
// Update the current preset channel's PTY code 
// Returns: none
// ----------------------------------------------------
//
void CCentralRepositoryHandler::UpdateChannelObjectWithPTYL (
    TInt aIndex,   //the channel index
    TPresetChannel& aPresetChannel, // reference to channel object
    TUint aPTY) // new PTY
    {
    //Initialize the channel object with correct information.
    GetPresetL( aIndex, aPresetChannel );    
    aPresetChannel.SetChannelPty(aPTY);
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::UpdateChannelObjectWithFavouriteInfoL
// Update the current preset channel's favourite status 
// Returns: none
// ----------------------------------------------------
//
void CCentralRepositoryHandler::UpdateChannelObjectWithFavouriteInfoL (
    TInt aIndex,   //the channel index
    TPresetChannel& aPresetChannel, // reference to channel object
    TBool aFav) // new favourite info
    {
    GetPresetL(aIndex, aPresetChannel);
    aPresetChannel.SetChannelFavouriteInfo(aFav);
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::UpdateChannelObjectWithUserRenameInfoL
// Update the current preset channel's user rename status 
// Returns: none
// ----------------------------------------------------
//
void CCentralRepositoryHandler::UpdateChannelObjectWithUserRenameInfoL (
    TInt aIndex,   //the channel index
    TPresetChannel& aPresetChannel, // reference to channel object
    TBool aUR) // new user rename info
    {
    //Initialize the channel object with correct information.
    GetPresetL( aIndex, aPresetChannel );    
    aPresetChannel.SetChannelUserRenameInfo(aUR);
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::DeletePresetL
// Delete the current preset
// Returns: none
// ----------------------------------------------------
//   
void CCentralRepositoryHandler::DeletePresetL(TInt aIndex)
	{
    
    if ( 0 > aIndex )
    	{ //Delete all presets
    	for(TInt i = 0;i<KMaxNumberOfPresetStations;i++)
    		{
            iCentralRepository->Delete( CalculateNameIndexL( i ) );    
            iCentralRepository->Delete( CalculateFrequencyIndexL( i ) );   
            iCentralRepository->Delete( CalculateUrlIndexL( i ) );
            iCentralRepository->Delete( CalculateRestOfParametersIndexL( i ) );
    		}
    	}
    else
    	{
        iCentralRepository->Delete( CalculateNameIndexL( aIndex ) );    
        iCentralRepository->Delete( CalculateFrequencyIndexL( aIndex ) );   
        iCentralRepository->Delete( CalculateUrlIndexL( aIndex ) );
        iCentralRepository->Delete( CalculateRestOfParametersIndexL( aIndex ) );
    	}		
	}
    
// ----------------------------------------------------
// CCentralRepositoryHandler::GetNumberOfPresets
// Get the current number of presets
// Returns: number of valid presets in cenrep  
// ----------------------------------------------------
//  
void CCentralRepositoryHandler::GetNumberOfPresetsL(TInt& aNum)
	{
    TInt count = 0;
    const TInt KMinAcceptableRadioFrequency = 76000000;
    const TInt KMaxAcceptableRadioFrequency = 108000000;
    TInt err( KErrNone );
    TInt frequencyIndex( 0 );
    TInt frequency( 0 );
    
    for(TInt index = 0;index < KMaxNumberOfPresetStations;index++)
    	{
        frequencyIndex = CalculateFrequencyIndexL( index );
        err = iCentralRepository->Get( frequencyIndex, frequency );
    	if(KErrNone == err )
        	{
            //Check validity of preset entries
            if ( ( KMinAcceptableRadioFrequency <= frequency ) && ( KMaxAcceptableRadioFrequency > frequency ) )
                {
                count++;
                if(count == 1)
                    {
                    iFirstPreset = index;
                    }
                }
        	} 
    	}
    aNum = count;
	}

// ----------------------------------------------------
// CCentralRepositoryHandler::GetFirstPreset
// Gets the first preset
// Returns: none
// ----------------------------------------------------
// 
void CCentralRepositoryHandler::GetFirstPresetL(TInt& aNum)
    {
    TInt count   = 0;
    iFirstPreset = 0;

    GetNumberOfPresetsL(count);     //this updates iFirstPreset

	if(count)
		{
		aNum = iFirstPreset;
		}
	}

// ----------------------------------------------------
// CCentralRepositoryHandler::GetNextPreset
// Get the next preset index that has been set, 
// given the current index
// Returns: none
// ----------------------------------------------------
// 
void CCentralRepositoryHandler::GetNextPresetL(TInt aIndex,TInt& aNextIndex)
    {
    TInt next = 0;
    TInt err( KErrNone );
    TInt frequencyIndex( 0 );
    TInt frequency( 0 );
    //start search on next one
    aIndex = aIndex + 1;
    for(TInt index = aIndex ;index < KMaxNumberOfPresetStations;index++)
    	{
        frequencyIndex = CalculateFrequencyIndexL( index );
        err = iCentralRepository->Get( frequencyIndex, frequency );
    	if( KErrNone == err && frequency != KErrNotFound )
        	{
            next = index;
            break;
        	} 
    	}
    aNextIndex = next;	
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::ExternalizeToStream
// Externalize the string in the aBuffer
// Returns: none
// ----------------------------------------------------
//
void CCentralRepositoryHandler::ExternalizeToStreamL(
    const TPresetChannel& aPresetChannel, // reference to channel object
    TDes8& aBuffer)  // buffer for externalize stream
    {
    RDesWriteStream outStream(aBuffer);
    CleanupClosePushL(outStream);
    aPresetChannel.ExternalizeL(outStream);
    outStream.CommitL();
    CleanupStack::PopAndDestroy(&outStream);    
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::InternalizeFromStream
// Internalize a preset from string
// Returns: none
// ----------------------------------------------------
//
void CCentralRepositoryHandler::InternalizeFromStreamL(
    TPresetChannel& aPresetChannel, // reference to channel object
    TDes8& aBuffer)  // buffer for externalize stream
    {
    RDesReadStream inStream(aBuffer);
    CleanupClosePushL(inStream);
    aPresetChannel.InternalizeL(inStream);
    CleanupStack::PopAndDestroy(&inStream);    
    }

// ----------------------------------------------------
// CCentralRepositoryHandler::GetPreset
// Returns: KErrNone if no errors, otherwise system errors.
// ----------------------------------------------------
//
TInt CCentralRepositoryHandler::GetPresetL(TInt aIndex, TPresetChannel& aPreset)
    {
    TInt ret = KErrNotFound;
    // Read name from its own key in the key sequence
    TInt nameIndex( CalculateNameIndexL( aIndex ) );
    TFmPresetName tempChannelName;
    HBufC* nameBuf = HBufC::NewLC( tempChannelName.MaxSize() );
    TPtr nameBufPtr = nameBuf->Des();
    ret = iCentralRepository->Get(nameIndex, nameBufPtr );
    if( KErrNone == ret )
        {
        if (nameBufPtr.Length() != 0)
            {
            tempChannelName.Copy( nameBufPtr );
            }
        aPreset.SetChannelName( tempChannelName );
        }
    else
        {
        User::Leave( ret );
        }
    CleanupStack::PopAndDestroy(nameBuf);                       

    // Read frequency from its own key in the key sequence
    TInt frequency( 0 );
    User::LeaveIfError( iCentralRepository->Get( CalculateFrequencyIndexL( aIndex ), frequency ) );
    aPreset.SaveChannelFrequency( frequency );

    // Read url from its own key in the key sequence
    TInt urlIndex( CalculateUrlIndexL( aIndex ) );
    TFmPresetUrl tempUrlName;
    HBufC* urlBuf = HBufC::NewLC( tempUrlName.MaxSize() );
    TPtr urlBufPtr = urlBuf->Des();
    ret = iCentralRepository->Get(urlIndex, urlBufPtr );
    if ( KErrNone == ret )
        {        
        if (urlBufPtr.Length() != 0)
            {
            tempUrlName.Copy( urlBufPtr );
            }
        aPreset.SetChannelUrl( tempUrlName );
        }
    else if ( KErrNotFound == ret )
        {
        // This is an optional parameter.
        }
    else
        {
        User::Leave( ret );
        }
    CleanupStack::PopAndDestroy(urlBuf);
                      

    HBufC8* buf = HBufC8::NewLC( sizeof(TPresetChannel) );
    TPtr8 bufPtr = buf->Des();
    TInt restOfParametersIndex( CalculateRestOfParametersIndexL( aIndex ) );
    if(KErrNone == iCentralRepository->Get(restOfParametersIndex, bufPtr ))
        {
        if (bufPtr.Length() != 0)
            {
            TRAPD(err, InternalizeFromStreamL( aPreset, bufPtr );)
            ret = err;
            }
        }
    CleanupStack::PopAndDestroy(buf);                       
    return ret;
    }
    
// end of file
