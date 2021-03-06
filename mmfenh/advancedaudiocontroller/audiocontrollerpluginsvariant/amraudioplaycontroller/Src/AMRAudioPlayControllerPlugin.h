/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Play controller for AMR.
*
*/


#ifndef AMRAUDIOPLAYCONTROLLERPLUGIN_H
#define AMRAUDIOPLAYCONTROLLERPLUGIN_H

// INCLUDES
#include "AdvancedAudioPlayController.h"

// FORWARD DECLARATION
class CAMRAudioPlayControllerDecoderBuilder;

// CLASS DECLARATION

/**
*  CAMRAudioPlayControllerPlugin
*
*  @lib VBRAudioPlayController.lib
*  @since 2.1
*/
class CAMRAudioPlayControllerPlugin : public CAdvancedAudioPlayController
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CAMRAudioPlayControllerPlugin* NewL();

        /**
        * Destructor
        */
        virtual ~CAMRAudioPlayControllerPlugin();

    private: // Functions from base classes

        /**
		* From CAdvancedAudioPlayController
        * Format specific initialization for Data Source
        * @since 3.0
        * @return void
        */
        void DoAddDataSourceL();

        /**
		* From CAdvancedAudioPlayController
        * Format specific initialization for Data Sink
        * @since 3.0
        * @return void
        */
        void DoAddDataSinkL();

         /**
 		 * From CAdvancedAudioPlayController
         * Format specific initialization for Data Sink
		 *
         * @since 3.0
         * @return void
         */
        void DoReadHeaderL(CMMFDataBuffer* aBuffer);

        /**
		* From CAdvancedAudioPlayController
        * Instantiates the codec.
        * @since 5.0
        * @return CAdvancedAudioDecoder*
        */
        CAdvancedAudioDecoder* BuildDecoderL();

	private:
        /**
        * C++ default constructor.
        */
        CAMRAudioPlayControllerPlugin();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // Data

    	// Builder for AMR decoder object
		CAMRAudioPlayControllerDecoderBuilder* iDecoderBuilder;
    };

#endif	// AMRAUDIOPLAYCONTROLLERPLUGIN_H

// End of File
