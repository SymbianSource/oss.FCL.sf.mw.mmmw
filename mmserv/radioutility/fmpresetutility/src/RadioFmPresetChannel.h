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
* Description:  Defines radio channel's structure.
*
*/

#ifndef RADIOFMPRESETCHANNEL_H
#define RADIOFMPRESETCHANNEL_H

#include <babitflags.h>

// Forward declare implementation class.
class CRadioFmPresetUtility;
class RWriteStream;
class RReadStream;

namespace MiscInfo
{
    enum DataIds
    {
        Local = 0,
        Favourite = 1,
        UserRename = 2
    };
}

// CLASS DECLARATION
/**
*  define preset channel structure.
*
*  @lib FmPresetUtility.lib
*  @since Series 60 3.0
*/
class TPresetChannel
    {
    public:
        /**
        * default constructor.
        */	
        TPresetChannel();
        /**
        * Get channel Name.
        * @since Series 60 3.0
        * @return channel name
        */
       TFmPresetName GetChannelName() const;
        /**
        * Get channel frequency.
        * @since Series 60 3.0
        * @return channel frequency
        */
        TInt GetChannelFrequency() const;
        /**
        * set channel name.
        * @since Series 60 3.0
        * @param aStationName new channel name
        * @return none
        */    
        void SetChannelName(const TFmPresetName& aStationName );
        /**
        * Set channel url.
        * @since Series 60 5.2
        * @param aStationUrl new channel Url
        * @return none
        */
        void SetChannelUrl(const TFmPresetUrl& aStationUrl );
        /**
        * Get channel url.
        * @since Series 60 5.2
        * @return channel url
        */        
        TFmPresetUrl ChannelUrl() const;
        /**
        * Set channel programme type.
        * @since Series 60 5.2
        * @param aPTY new channel PTY
        * @return none
        */        
        void SetChannelPty(const TUint aPTY);
        /**
        * Get channel programme type.
        * @since Series 60 5.2
        * @return channel PTY
        */      
		TUint ChannelPty() const;
        /**
        * Set channel programme identification code.
        * @since Series 60 5.2
        * @param aPI - the new channel PI-code
        * @return none
        */
        void SetChannelPI(const TUint aPI);
        /**
        * Get channel programme identification code.
        * @since Series 60 5.2
        * @return channel PI-code
        */      
        TUint ChannelPI() const;        
        /**
        * Set or clear channel favourite info.
        * @since Series 60 5.2
        * @param aFavourite - the new channel favourite info
        * @return none
        */
        void SetChannelFavouriteInfo(const TBool aFavourite);
        /**
        * Get channel favourite info.
        * @since Series 60 5.2
        * @return channel favourite info
        */      
        TBool ChannelFavouriteInfo() const;        
        
        /**
        * Set or clear channel user rename info.
        * @since Series 60 5.2
        * @param aUserRename - the new channel favourite info
        * @return none
        */
        void SetChannelUserRenameInfo(const TBool aUserRename);
        /**
        * Get channel user rename info.
        * @since Series 60 5.2
        * @return channel user rename
        */      
        TBool ChannelUserRenameInfo() const;
        /**
        * Save channel frequency.
        * @since Series 60 3.0
        * @param aFrequency - the new channel frequency
        * @return none
        */
        void SaveChannelFrequency(const TInt aFrequency );	
        /**
        * Serialize the data to stream.
        * @since Series 60 3.0
        * @param aStream - used for write stream
        * @return none
        */
        void ExternalizeL(RWriteStream & aStream ) const;
        /**
        * Deserialize the data out of stream.
        * @since Series 60 3.0
    	* @param aStream  used for read stream	
        * @return none
        */
        void InternalizeL(RReadStream & aStream );
      
    private:
        //the preset channel name
		TFmPresetName iPresetChannelName;
		//the preset channel url
		TFmPresetUrl iPresetChannelUrl;
		//the preset channel frequency
		TInt iPresetChannelFrequency;
		//the preset channel Program Type
		TUint iPresetChannelPTY;
		//the preset channel Programme Identification code
		TUint iPresetChannelPI;
        //the preset channel local, favourite and user renamed info
		TBitFlags32 iPresetChannelMisc;
    };

#endif  // RADIOFMPRESETCHANNEL_H

// End of File
