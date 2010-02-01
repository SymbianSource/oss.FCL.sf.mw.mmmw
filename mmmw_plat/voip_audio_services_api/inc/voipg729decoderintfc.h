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
* Description:  Defines APIs specific to G.729 decoder.
*
*/


#ifndef __VOIPG729DECFORMATINTFC_H
#define __VOIPG729DECFORMATINTFC_H

#include <voipaudiocommon.h>
#include <voipformatintfc.h>


// FORWARD DECLARATIONS
class CVoIPG729DecoderIntfcImpl;


/**
 *  CVoIPG729DecoderIntfc class
 *
 *  G.729 decoder configuration class
 *
 *  @lib VoIPAudioIntfc.dll
 *  @since S60 v5.0
 */
class CVoIPG729DecoderIntfc : public CVoIPFormatIntfc
    {
public:
    /**
     * Destructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    IMPORT_C virtual ~CVoIPG729DecoderIntfc();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt BadLsfNextBuffer();

protected:
    /**
     * Constructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    CVoIPG729DecoderIntfc();

    /**
     * 2-nd phase constructor
     *
     * @since S60 v5.0
     * @param CVoIPG729DecoderIntfcImpl*
     * @return void
     */
    void ConstructL(CVoIPG729DecoderIntfcImpl* aFormatIntfcImpl);

protected:
    CVoIPG729DecoderIntfcImpl* iFormatIntfcImpl;

    };


#endif //__VOIPG729DECFORMATINTFC_H

// End of file
