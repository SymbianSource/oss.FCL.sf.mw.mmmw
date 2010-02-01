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
* Description:  VOIP Audio Services  Defines buffer type for VoIP data streaming.
*
*/


#ifndef __VOIPDATABUFFERIMPL_H
#define __VOIPDATABUFFERIMPL_H

#include <e32base.h>
#include <voipdatabuffer.h>


/**
 *  CVoIPDataBuferImpl
 *  ?description
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CVoIPDataBufferImpl) : public CBase,
                                         public CVoIPJBDataBuffer
    {
public:
    /**
     * 2-phased constructor
     *
     * @since S60 v5.0
     * @param TInt
     * @return none
     */
    static CVoIPDataBufferImpl* NewL(TInt aBufferLen);

    /**
     * Destructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    virtual ~CVoIPDataBufferImpl();

    /**
     * GetPayloadPtr
     *
     * @since S60 v5.0
     * @param TPtr8&
     * @return none
     */
    void GetPayloadPtr(TPtr8& aPayloadPtr);

    /**
     * SetPayloadPtr
     *
     * @since S60 v5.0
     * @param TPtr8
     * @return none
     */
    void SetPayloadPtr(TPtr8 aPayloadPtr);

    /**
     * GetPayloadPtr
     *
     * @since S60 v5.0
     * @param CVoIPDataBuffer::TVoIPBufferType&
     * @return none
     */
    void GetBufferType(CVoIPDataBuffer::TVoIPBufferType& aType);

protected:
    /**
     * Constructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    CVoIPDataBufferImpl();

    /**
     * Symbian constructor
     *
     * @since S60 v5.0
     * @param TInt
     * @return void
     */
    void ConstructL(TInt aBufferLen);

protected:
    CVoIPDataBuffer::TVoIPBufferType iType;
    TPtr8   iPayloadPtr;
    HBufC8* iBuf;
    TInt    iBufferLen;
    };

#endif //__VOIPDATABUFFERIMPL_H


//End of file
