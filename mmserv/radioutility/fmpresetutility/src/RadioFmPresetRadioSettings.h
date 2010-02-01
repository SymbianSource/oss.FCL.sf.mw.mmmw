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


#ifndef RADIOFMPRESETRADIOSETTINGS_H
#define RADIOFMPRESETRADIOSETTINGS_H


// CLASS DECLARATION
class CRadioFmPresetUtility;


/**
*  TRadioSettings class.
*  Maintain the radio setting data.
* 
*/
class TRadioSettings
    {
    public:
        /**  
        * default constructor.
        */
       TRadioSettings();
   
        /**
        * return current preset number.
        * @since Series 60 2.7
        * @return current preset number
        */
       TInt CurrentPreset() const; 
        /**
        * return current frequency.
        * @since Series 60 2.7
        * @return current frequency
        */
       TInt Frequency() const; 
        /**
        * check if radio is on.
        * @since Series 60 2.7
        * @return true/false
        */
       TBool IsRadioOn() const;
        /**
        * Set current preset number.
        * @since Series 60 2.7
        * @return current preset number
        */
       void SetCurrentPreset(const TInt aChannel);
        /**
        * set current frequency .
        * @since Series 60 2.7
        * @param aFrequency the frequency to be set
        * @return none
        */
       void SetFrequency(const TInt aFrequency);
        /**
        * set radio on.
        * @since Series 60 2.7
        * @return none
        */
       void SetRadioOn();
        /**
        * set radio off.
        * @since Series 60 2.7
        * @return none
        */
       void SetRadioOff();
 
    private: 
        TInt iCurrentPreset; 
        /// current frequency
        TInt iFrequency; 
        //  radio on/off flag
        TBool iRadioOn;
    };

#endif  // FMPRESETRADIOSETTINGS_H

// End of File
