/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  VOIP Audio Services
*
*/


#ifndef __RINGTONEFACTORYIMPL_H
#define __RINGTONEFACTORYIMPL_H


// FORWARD DECLARATIONS
class CRingTonePlayer;


/**
 *  CRingToneFactoryImpl class
 *
 *  ?more_complete_description
 *
 *  @lib VoIPAudioIntfc.dll
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CRingToneFactoryImpl) : public CBase
    {
public:

    /**
     * 2-phase constructor
     *
     * @since S60 v5.0
     * @param none
     * @return CRingToneFactoryImpl* -
     */
    static CRingToneFactoryImpl* NewL();

    /**
     * Destructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    virtual ~CRingToneFactoryImpl();

    /**
     *
     *
     * @since S60 v5.0
     * @param CRingTonePlayer*&
     * @return TInt
     */
    TInt CreateRingTonePlayer(CRingTonePlayer*& aRingTonePlayer);

private:

    /**
     * Constructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    CRingToneFactoryImpl();

    /**
     * Symbian constructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    void ConstructL();

private:

    };

#endif //__RINGTONEFACTORYIMPL_H

// End of file
