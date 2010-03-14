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
 * Description:  Defines APIs specific to G.711 encoder.
 *
 */

#ifndef __VOIPG711ENCFORMATINTFC_H
#define __VOIPG711ENCFORMATINTFC_H

#include <voipaudiocommon.h>
#include <voipformatintfc.h>

// FORWARD DECLARATIONS
class CVoIPG711EncoderIntfcImpl;

/**
 *  CVoIPG711EncoderIntfc class
 *
 *  G.711 encoder configuration class
 *
 *  @lib VoIPAudioIntfc.dll
 *
 */
class CVoIPG711EncoderIntfc : public CVoIPFormatIntfc
    {
public:
    /**
     * Destructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    IMPORT_C virtual ~CVoIPG711EncoderIntfc();

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

protected:
    /**
     * Constructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    CVoIPG711EncoderIntfc();

    /**
     * 2-nd phase constructor
     *
     * @since S60 v5.0
     * @param CVoIPG711EncoderIntfcImpl*
     * @return void
     */
    void ConstructL(CVoIPG711EncoderIntfcImpl* aFormatIntfcImpl);

protected:
    CVoIPG711EncoderIntfcImpl* iFormatIntfcImpl;

    };

#endif //__VOIPG711ENCFORMATINTFC_H

// End of file
