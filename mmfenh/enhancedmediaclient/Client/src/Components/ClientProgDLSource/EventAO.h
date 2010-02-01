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
* Description:  header of EventAO class.
*
*/


#ifndef C_CEVENTAO_H
#define C_CEVENTAO_H

//  INCLUDES
#include <e32base.h>

namespace multimedia
    {
    
    // Observer
    /**
     *  Observer that monitors the Source State changes
     *  @lib EnhancedMediaClient.lib
     *  @since S60 v3.2
     */    
    class MSourceStateObserver
        {
        public:
            virtual void SourceStateChanged() = 0;
        };
    
    /**
     *  Active Object that monitors the Source State Change
     *  Event from the Source Side
     *  @lib EnhancedMediaClient.lib
     *  @since S60 v3.2
     */    
   class CEventAO : public CActive
        {
        public:  // Constructors and destructor
            /**
             * Function to Create the Object.
             * @param aObserver Observer to callback the Event.
             */
            static CEventAO* NewL(MSourceStateObserver& aObserver);
            /**
            * Destructor.
            */
            virtual ~CEventAO();
            
            /**
             * From CActive.
             * Indicates that this active object has issued a
             * request and that it is now outstanding 
             *
             * @since S60 v3.2
             */            
            void SetActive();

            /**
             * From CEventAO.
             * Returns the Error Stoted on Completion of the Request
             *
             * @since S60 v3.2
             */            
            TInt Error();
            
        protected:
            // From CActive
            void RunL();
            void DoCancel();
            TInt RunError( TInt aError );
            
        private:
            CEventAO( MSourceStateObserver& aObserver );
            void ConstructL();
            
        private:
            /**
            * Observer stored to Callback
            */
            MSourceStateObserver& iObserver;
            /**
            * Error Stored upon Request Completion
            */
            TInt iError;
        };
    
    } // namespace multimedia

#endif // C_CEVENTAO_H

// End of File
