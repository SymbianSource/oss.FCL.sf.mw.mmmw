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
* Description:  Defines APIs to communicate with Central Repository
*
*/


#ifndef RADIOFMPRESETCENTRALREPOSITORYHANDLER_H
#define RADIOFMPRESETCENTRALREPOSITORYHANDLER_H

//  INCLUDES
#include <RadioFmPresetUtility.h>
#include <RadioPresetUtility.h>
#include "RadioFmPresetChannel.h"

// CONSTANTS
const TInt KBufferLength = 20;
const TInt KStreamBufferSize = 100;

// Forward declare implementation class
class TRadioSettings;
class CRepository;

// CLASS DECLARATION

/**
*  it is a wrapper of Central Repository, responsible for the operations that need to access 
*  the central respositorhy.
*
*  @lib FmPresetUtility.lib
*  @since Series 60 3.0_version
*/
class CCentralRepositoryHandler : public CBase
    {
    public:
      	/**
      	* Two-phased class constructor.
      	* @param aRadioSettings  pointer to the CRadioSettings class
      	*/
        static CCentralRepositoryHandler* NewL(TRadioSettings& aRadioSettings);
      	/**
      	* Destructor of CCentralRepositoryHandler class.
      	*/
        virtual ~CCentralRepositoryHandler();
    public:
      	/**
      	* Retrieve the preset frequency from Central Repository
        * @since Series 60 3.0
      	* @param aIndex  the index of which preset to retrieve
      	* @return the frequency stored in Central Repository
      	*/
        TInt RetrievePresetFrequencyL(TInt aIndex) const;

        /**
      	* Save channel preset info back to Central Repository
        * @since Series 60 3.0
      	* @param aIndex  the index of which preset to save
      	* @param aFrequency  the frequency to be saved
      	*/
        void SavePresetFrequencyL(TInt aIndex, TInt aFrequency);

        /**
      	* Retrieve the preset name from Central Repository
        * @since Series 60 3.0
      	* @param aIndex  the index of which preset to retrieve
      	* @return the name of the preset stored in Central Repository
      	*/
        TFmPresetName RetrievePresetNameL(TInt aIndex) const;

        /**
        * Retrieve the preset URL from Central Repository
        * @since Series 60 5.2
        * @param aIndex  the index of which preset to retrieve
        * @return the URL of the preset stored in Central Repository
        */
        TFmPresetUrl RetrievePresetUrlL(TInt aIndex) const;

        /**
        * Retrieve the preset PTY (programme type) from Central Repository
        * @since Series 60 5.2
        * @param aIndex  the index of which preset to retrieve
        * @return the PTY of the preset stored in Central Repository
        */
        TUint RetrievePresetPTYL(TInt aIndex) const;

        /**
        * Retrieve the preset PI code (programme identification) from Central Repository
        * @since Series 60 5.2
        * @param aIndex  the index of which preset to retrieve
        * @return the PI code of the preset stored in Central Repository
        */        
        TUint RetrievePresetPIL(TInt aIndex) const;

        /**
        * Retrieve the preset favourite information from Central Repository
        * @since Series 60 5.2
        * @param aIndex  the index of which preset to retrieve
        * @return the favourite info of the preset stored in Central Repository
        */
        TBool RetrievePresetFavouriteL(TInt aIndex) const;
        
        /**
        * Retrieve the preset user rename information from Central Repository
        * @since Series 60 5.2
        * @param aIndex  the index of which preset to retrieve
        * @return the user rename info of the preset stored in Central Repository
        */        
        TBool RetrievePresetUserRenameL(TInt aIndex) const;
        
        /**
      	* Save channel preset name back to Central Repository
        * @since Series 60 3.0
      	* @param aIndex the index of which preset to save
      	* @param aStationName the preset name to be saved
      	* @return none
      	*/
        void SavePresetNameL(TInt aIndex, const TFmPresetName& aStationName );

        /**
        * Save channel URL name back to Central Repository
        * @since Series 60 5.2
        * @param aIndex the index of which preset to save
        * @param aStationUrl the preset URL to be saved
        * @return none
        */
        void SavePresetUrlL(TInt aIndex, const TFmPresetUrl& aStationUrl);
        
        /**
        * Save channel PI code back to Central Repository
        * @since Series 60 5.2
        * @param aIndex the index of which preset to save
        * @param aPI the preset programme identification to be saved
        * @return none
        */
        void SavePresetPIL(TInt aIndex, TUint aPI);

        /**
        * Save channel PTY (genre) back to Central Repository
        * @since Series 60 5.2
        * @param aIndex the index of which preset to save
        * @param aPTY the preset programme type to be saved
        * @return none
        */
        void SavePresetPTYL(TInt aIndex, TUint aPTY);

        /**
        * Save channel favourite info back to Central Repository
        * @since Series 60 5.2
        * @param aIndex the index of which preset to save
        * @param aFav the preset favourite info to be saved
        * @return none
        */
        void SavePresetFavouriteInfoL(TInt aIndex, TBool aFav);

        /**
        * Save channel user rename info back to Central Repository
        * @since Series 60 5.2
        * @param aIndex the index of which preset to save
        * @param aUR the preset user rename info to be saved
        * @return none
        */
        void SavePresetUserRenameInfoL(TInt aIndex, TBool aUR);
        
        /**
      	* Save all FmPreset related persistent info back to Central Repository
        * @since Series 60 3.0
      	* @param aIndex - the index of which preset to save
      	* @return none
      	*/
        void SaveFmPresetSettings();

        /**
        * Delete the specified preset.
        * Specifying aIndex == 0, will delete all presets.
        * Has no effect if specified index contains no preset.
        */
        void DeletePresetL(TInt aIndex);
        
        /**
        * Get the current number of presets.
      	* @param aNum - the number of presets.
      	* @return none
        */
        void GetNumberOfPresetsL(TInt& aNum);
        
        /**
        * Get the index of first preset.
      	* @param aIndex - the first preset index.
      	* @return none
        */        
        void GetFirstPresetL(TInt& aIndex);

        /**
        * Get the index of the next preset.
      	* @param aIndex -  the first preset index.
      	* @param aNextIndex - the next preset index.
      	* @return none
        */           
        void GetNextPresetL(TInt aIndex,TInt& aNextIndex);
        
              
    private:
      	/**
      	* Default class constructor.
      	* @param aFmPreset - pointer to the CRadioFmPresetUtility instance (used for callbacks)
      	* @param aRadioSettings - pointer to the CRadioSettings class
      	*/
        CCentralRepositoryHandler(TRadioSettings& aRadioSettings);
       	/**
     	* Second phase class constructor.
     	*/
        void ConstructL();
     	/**
     	* Retrieves persistent settings from Central Repository.
     	*/
        void RetrieveInitialSettings();

        /**
        * Get the channel PTY from the externalized stream.
        * @param aString - the raw data received from Central Repository
        */        
        TUint ConvertStringToChannelPTYL(const TDesC8& aString) const;
        
        /**
        * Get the channel PI code from the externalized stream.
        * @param aString - the raw data received from Central Repository
        */
        TUint ConvertStringToChannelPIL(const TDesC8& aString) const;

        /**
        * Get the channel favourite info from the externalized stream.
        * @param aString - the raw data received from Central Repository
        */
        TBool ConvertStringToChannelFavouriteL(const TDesC8& aString) const;
        
        /**
        * Get the channel user rename info from the externalized stream.
        * @param aString - the raw data received from Central Repository
        */
        TBool ConvertStringToChannelUserRenameL(const TDesC8& aString) const;
        
        /**
        * Update the current preset channel's PI code (programme identification).
        * @param aIndex - the index of which preset's channel name to be updated
        * @param aPresetChannel - reference to the CRadioSettings class
        * @param aPI - the new channel PI code to be updated to the preset channel
        */ 
        void UpdateChannelObjectWithPIL (TInt aIndex, TPresetChannel& aPresetChannel, TUint aPI);
        
        /**
        * Update the current preset channel's PTY (programme type).
        * @param aIndex - the index of which preset's channel name to be updated
        * @param aPresetChannel - reference to the CRadioSettings class
        * @param aPTY - the new channel PTY (genre) to be updated to the preset channel
        */ 
        void UpdateChannelObjectWithPTYL (TInt aIndex, TPresetChannel& aPresetChannel, TUint aPTY);
        
        /**
        * Update the current preset channel's favourite information.
        * @param aIndex - the index of which preset's channel name to be updated
        * @param aPresetChannel - reference to the CRadioSettings class
        * @param aFav - the new channel favourite info to be updated to the preset channel
        */
        void UpdateChannelObjectWithFavouriteInfoL (TInt aIndex, TPresetChannel& aPresetChannel, TBool aFav);

        /**
        * Update the current preset channel's user rename information.
        * @param aIndex - the index of which preset's channel name to be updated
        * @param aPresetChannel - reference to the CRadioSettings class
        * @param aUR - the new channel user rename info to be updated to the preset channel
        */
        void UpdateChannelObjectWithUserRenameInfoL (TInt aIndex, TPresetChannel& aPresetChannel, TBool aUR);
        /**
       	* Externalize the string in the aBuffer.
       	* @param aPresetChannel - reference to the RadioSettings class
       	* @param aBuffer - the buffer to be externalized
       	*/     
       	void ExternalizeToStreamL(const TPresetChannel& aPresetChannel, TDes8& aBuffer);
        /**
        * Internalize TPresetChannel from the aBuffer.
        * @param aPresetChannel - reference to the RadioSettings class
        * @param aBuffer - the buffer where to internalize from
        */            	
       	void InternalizeFromStreamL(TPresetChannel& aPresetChannel, TDes8& aBuffer);       	

       	/**
       	 * Gets stored information of given index and internalizes the stream to aPreset
       	 * @param aIndex the index of the stored preset to be read
       	 * @param aPreset a reference to TPresetChannel where to internalize from
       	 * @return KErrNone if no errors
       	 */
       	TInt GetPresetL(TInt aIndex, TPresetChannel& aPreset);
       	
        // Handle to Central Repository server
        CRepository* iCentralRepository; 
        // Reference to RadioSetting object     
        TRadioSettings* iRadioSettings;
        
        TInt iFirstPreset;
    };

#endif  // RADIOFMPRESETCENTRALREPOSITORYHANDLER_H

// end of file
