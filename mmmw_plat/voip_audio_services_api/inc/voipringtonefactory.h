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
* Description:  Defines factory object for creating ring tone player.
*
*/


#ifndef __RINGTONEFACTORY_H
#define __RINGTONEFACTORY_H


// FORWARD DECLARATIONS
class CRingTonePlayer;
class CRingToneFactoryImpl;


/**
 *  CRingToneFactory class
 *
 *  ?more_complete_description
 *
 *  @lib VoIPAudioIntfc.dll
 *  @since S60 v5.0
 */
class CRingToneFactory
    {
public:

    /**
     * CreateFactory
     *
     * @since S60 v5.0
     * @param CRingToneFactory*&
     * @return TInt
     */
    IMPORT_C static TInt CreateFactory(CRingToneFactory*& aFactory);

    /**
     * Destructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    IMPORT_C ~CRingToneFactory();

    /**
     * CreateRingTonePlayer
     *
     * @since S60 v5.0
     * @param CRingTonePlayer*&
     * @return TInt
     */
    IMPORT_C TInt CreateRingTonePlayer(CRingTonePlayer*& aFactory);

private:

    /**
     * Constructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    CRingToneFactory();

    /**
     * Symbian constructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    void ConstructL();

private:

    CRingToneFactoryImpl* iRingToneFactoryImpl;

    };

#endif //__RINGTONEFACTORY_H

// End of file
