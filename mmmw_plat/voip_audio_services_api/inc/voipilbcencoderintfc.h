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
* Description:  Defines APIs specific to iLBC encoder.
*
*/


#ifndef __VOIPILBCENCFORMATINTFC_H
#define __VOIPILBCENCFORMATINTFC_H

#include <voipaudiocommon.h>
#include <voipformatintfc.h>


// FORWARD DECLARATIONS
class CVoIPILBCEncoderIntfcImpl;


/**
 *  CVoIPILBCEncoderIntfc class
 *
 *  iLBC encoder configuration class
 *
 *  @lib VoIPAudioIntfc.dll
 *  @since S60 v5.0
 */
class CVoIPILBCEncoderIntfc : public CVoIPFormatIntfc
    {
public:
    /**
     * Destructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    IMPORT_C virtual ~CVoIPILBCEncoderIntfc();

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

protected:
    /**
     * Constructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    CVoIPILBCEncoderIntfc();

    /**
     * 2-nd phase constructor
     *
     * @since S60 v5.0
     * @param CVoIPILBCEncoderIntfcImpl*
     * @return void
     */
    void ConstructL(CVoIPILBCEncoderIntfcImpl* aFormatIntfcImpl);

protected:
    CVoIPILBCEncoderIntfcImpl* iFormatIntfcImpl;

    };


#endif //__VOIPILBCENCFORMATINTFC_H

// End of file
