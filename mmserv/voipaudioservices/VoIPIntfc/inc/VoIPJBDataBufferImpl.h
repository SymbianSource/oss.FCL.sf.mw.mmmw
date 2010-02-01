/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  VOIP Audio Services  Defines jitter buffer type for VoIP data streaming.
*
*/


#ifndef __VOIPJBDATABUFFERIMPL_H
#define __VOIPJBDATABUFFERIMPL_H

#include "VoIPDataBufferImpl.h"


/**
 *  CVoIPJBDataBufferImpl
 *  ?description
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CVoIPJBDataBufferImpl) : public CVoIPDataBufferImpl
    {
public:
    /**
     * 2-phased constructor
     *
     * @since S60 v5.0
     * @param TInt
     * @return none
     */
    static CVoIPJBDataBufferImpl* NewL(TInt aBufferLen);

    /**
     * Destructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    virtual ~CVoIPJBDataBufferImpl();

    /**
     * SetBufferSequence
     *
     * @since S60 v5.0
     * @param const TUint
     * @return none
     */

    void SetBufferSequence(const TUint aBufferSeq);

    /**
     * GetBufferSequence
     *
     * @since S60 v5.0
     * @param TUint&
     * @return none
     */
    void GetBufferSequence(TUint& aBufferSeq);

protected:
    /**
     * Constructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    CVoIPJBDataBufferImpl();

    /**
     * Symbian constructor
     *
     * @since S60 v5.0
     * @param TInt
     * @return void
     */
    void ConstructL(TInt aBufferLen);

private:

    TUint iBufferSequence;
    };

#endif //__VOIPJBDATABUFFERIMPL_H


//End of file
