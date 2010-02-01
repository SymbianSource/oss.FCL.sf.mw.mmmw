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
* Description:  Defines APIs specific to G.711 decoder.
*
*/


#ifndef __VOIPG711DECFORMATINTFC_H
#define __VOIPG711DECFORMATINTFC_H

#include <voipaudiocommon.h>
#include <voipformatintfc.h>


// FORWARD DECLARATIONS
class CVoIPG711DecoderIntfcImpl;


/**
 *  CVoIPG711DecoderIntfc class
 *
 *  G.711 decoder configuration class
 *
 *  @lib VoIPAudioIntfc.dll
 *  @since S60 v5.0
 */
class CVoIPG711DecoderIntfc : public CVoIPFormatIntfc
    {
public:
    /**
     * Destructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    IMPORT_C virtual ~CVoIPG711DecoderIntfc();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TG711CodecMode
     * @return TInt
     */
    IMPORT_C TInt SetMode(TG711CodecMode aMode);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TG711CodecMode&
     * @return TInt
     */
    IMPORT_C TInt GetMode(TG711CodecMode& aMode);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TBool
     * @return TInt
     */
    IMPORT_C TInt SetCNG(TBool aCng);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TBool&
     * @return TInt
     */
    IMPORT_C TInt GetCNG(TBool& aCng);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TBool
     * @return TInt
     */
    IMPORT_C TInt SetPLC(TBool aPlc);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TBool&
     * @return TInt
     */
    IMPORT_C TInt GetPLC(TBool& aPlc);

protected:
    /**
     * Constructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    CVoIPG711DecoderIntfc();

    /**
     * 2-nd phase constructor
     *
     * @since S60 v5.0
     * @param CVoIPG711DecoderIntfcImpl*
     * @return void
     */
    void ConstructL(CVoIPG711DecoderIntfcImpl* aFormatIntfcImpl);

protected:
    CVoIPG711DecoderIntfcImpl* iFormatIntfcImpl;

    };


#endif //__VOIPG711DECFORMATINTFC_H

// End of file
