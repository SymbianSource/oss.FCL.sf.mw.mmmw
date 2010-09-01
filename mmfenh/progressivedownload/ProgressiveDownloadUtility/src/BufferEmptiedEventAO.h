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
* Description:  Progressive Download Utility  Definition of the BufTypeSupEventAO class.
*
*/


#ifndef BUFFEREMPTIEDEVENTAO_H
#define BUFFEREMPTIEDEVENTAO_H

//  INCLUDES
#include <e32base.h>

namespace multimedia
    {

    class MDataBuffer;
    // Observer
    class MBufferEmptiedObserver
        {
        public:
            virtual void BufferEmptiedBySource() = 0;
        };

        /**
        *  Class to encapsulate a queue item.
    */
    class CBufferEmptiedEventAO : public CActive
        {
        public:  // Constructors and destructor
            static CBufferEmptiedEventAO* NewL(MBufferEmptiedObserver&);
            virtual ~CBufferEmptiedEventAO();

            void SetActive();
            TInt Error();

            void SetBuffer(MDataBuffer* aBuffer);
            MDataBuffer* GetBuffer();

        protected:
            // From CActive
            void RunL();
            void DoCancel();
            TInt RunError( TInt aError );

        private:
            CBufferEmptiedEventAO( MBufferEmptiedObserver& aObserver );
            void ConstructL();

        private:
            MBufferEmptiedObserver& iObserver;
            MDataBuffer* iBuffer;
            TInt iError;
        };
    }
#endif // BUFTYPESUPEVENTAO_H

// End of File
