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
* Description:  Defines APIs common to all supported codecs.
*
*/


#ifndef __VOIPFORMATINTFC_H
#define __VOIPFORMATINTFC_H

#include <voipaudiocommon.h>


// FORWARD DECLARATIONS
class CVoIPFormatIntfc;
class CVoIPFormatIntfcImpl;


/**
 *  MVoIPFormatObserver class
 *
 *  ?more_complete_description
 *
 *  @lib VoIPAudioIntfc.dll
 *  @since S60 v5.0
 */
class MVoIPFormatObserver
    {
public:
    static const TUint KSilenceBegin     = 1;
    static const TUint KSilenceUpdate    = 2;
    static const TUint KSilenceEnd       = 3;
    static const TUint KObserverEventMax = 4;

public:
    /**
     * ?description
     *
     * @since S60 v5.0
     * @param const CVoIPFormatIntfc&
     * @param TInt - event type
     * @return void
     */
    virtual void Event(const CVoIPFormatIntfc& aSrc, TInt aEventType) = 0;
    };


/**
 *  CVoIPFormatIntfc class
 *
 *  Base class for codec format configuration
 *
 *  @lib VoIPAudioIntfc.dll
 *  @since S60 v5.0
 */
class CVoIPFormatIntfc
    {
public:
    /**
     *  TG711CodecMode
     *  ?description
     *  @since S60 v5.0
     */
    enum TG711CodecMode
        {
        EG711ALaw,
        EG711uLaw
        };

    /**
     *  TILBCCodecMode
     *  ?description
     *  @since S60 v5.0
     */
    enum TILBCCodecMode
        {
        EiLBC20mSecFrame,
        EiLBC30mSecFrame
        };

public:
    /**
     * Destructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    IMPORT_C virtual ~CVoIPFormatIntfc();

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param MVoIPFormatObserver&
     * @return TInt
     */
    IMPORT_C TInt SetObserver(MVoIPFormatObserver& aObserver);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param RArray<TUint>&
     * @return TInt
     */
    IMPORT_C TInt GetSupportedBitRates(RArray<TUint>& aArray);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TUint
     * @return TInt
     */
    IMPORT_C TInt SetBitRate(TUint aBitrate);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TUint&
     * @return TInt
     */
    IMPORT_C TInt GetBitRate(TUint& aBitrate);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TBool
     * @return TInt
     */
    IMPORT_C TInt SetVAD(TBool aVad);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TBool&
     * @return TInt
     */
    IMPORT_C TInt GetVAD(TBool& aVad);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TBool&
     * @return TInt
     */
    IMPORT_C TInt FrameModeRequiredForErrorConcealment(TBool& aMode);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TBool
     * @return TInt
     */
    IMPORT_C TInt SetFrameMode(TBool aMode);
    /**
     * ?description
     *
     * @since S60 v5.0
     * @param TBool&
     * @return TInt
     */
    IMPORT_C TInt GetFrameMode(TBool& aMode);

    /**
     * ?description
     *
     * @since S60 v5.0
     * @param none
     * @return TInt
     */
    IMPORT_C TInt ConcealErrorForNextBuffer();

protected:

    /**
     * Constructor
     *
     * @since S60 v5.0
     * @param none
     * @return none
     */
    CVoIPFormatIntfc();

    /**
     * 2-nd phase constructor
     *
     * @since S60 v5.0
     * @param CVoIPFormatIntfcImpl*
     * @return void
     */
    void ConstructL(CVoIPFormatIntfcImpl* aVoIPFormatIntfcImpl);

protected:
    CVoIPFormatIntfcImpl* iVoIPFormatIntfcImpl;

    };

#endif //__VOIPFORMATINTFC_H

// End of file
