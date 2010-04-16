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
 * Description:  Defines APIs specific to iLBC decoder.
 *
 */

#ifndef __VOIPILBCDECFORMATINTFC_H
#define __VOIPILBCDECFORMATINTFC_H

#include <voipaudiocommon.h>
#include <voipformatintfc.h>

// FORWARD DECLARATIONS
class CVoIPILBCDecoderIntfcImpl;

/**
 *  CVoIPILBCDecoderIntfc class
 *
 *  iLBC decoder configuration class
 *
 *  @lib VoIPAudioIntfc.dll
 *
 */
class CVoIPILBCDecoderIntfc : public CVoIPFormatIntfc
    {
public:
    /**
     * Destructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    IMPORT_C virtual ~CVoIPILBCDecoderIntfc();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TILBCCodecMode
     * @return TInt
     */
    IMPORT_C TInt SetMode(TILBCCodecMode aMode);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TILBCCodecMode&
     * @return TInt
     */
    IMPORT_C TInt GetMode(TILBCCodecMode& aMode);

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

protected:
    /**
     * Constructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    CVoIPILBCDecoderIntfc();

    /**
     * 2-nd phase constructor
     *
     * @since S60 v5.0
     * @param CVoIPILBCDecoderIntfcImpl*
     * @return void
     */
    void ConstructL(CVoIPILBCDecoderIntfcImpl* aFormatIntfcImpl);

protected:
    CVoIPILBCDecoderIntfcImpl* iFormatIntfcImpl;

    };

#endif //__VOIPILBCDECFORMATINTFC_H

// End of file
