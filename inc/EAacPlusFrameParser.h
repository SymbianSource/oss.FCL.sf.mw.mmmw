/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Telephony Multimedia Service
 *
 */


#ifndef 	EAACPLUSFRAMEPARSER_H
#define 	EAACPLUSFRAMEPARSER_H

// INCLUDES
#include <e32base.h>
#include "EAacPlusUtil.h"

class CFrameTable;
class CMMFBuffer;

// CLASS DECLARATION

/**
*  This class implements utility functions to extract AAC frame
*  information.
*
*  @lib MmfEAacPlusUtil.lib
*  @since 3.2
*/
class CEAacPlusFrameParser : public CBase
	{
	public:
	
    class TEAacPlusParserConfig
        {
    public:
		CEAacPlusUtil::TEAacPlusFrameLenConfig iFrameLenConfig; // config info needed by util for frame length.
        TInt    	iOutSamplingFrequency;		 // output sampling freq of decoder
      	TInt		iNumOfSamples;				 // samples per frame
      	CFrameTable* iFrameTable;				 // the frame table
        };

    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CEAacPlusFrameParser* NewL();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CEAacPlusFrameParser();

    public: // New functions

		/**
        * Used to parse the aac frames and populate the seek table.
        * @since 3.2
        * @param	aBuf                 buffer of AAC data from start of aac content
        * @param	aBufLen              length of aBuf
        * @return   status
        */
        IMPORT_C TInt Parse(const CMMFBuffer& aSrc, TInt& aSrcUsed);

		/**
        * Used to configure the utility with information needed to enable the apis.
        * This api can be used if GetAacFrameInfo is not used to initialize the utility.
        * @since 3.2
        * @param	aFormatType format type
        * @param	aSamplingFrequency sampling frequency
        * @return   status
        */
		IMPORT_C TInt Config(TEAacPlusParserConfig& aParserConfig);
		IMPORT_C TInt ReConfig();

    private:

        /**
        * C++ default constructor.
        */
        CEAacPlusFrameParser();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

 	private:	// Data
 		CEAacPlusUtil* iAacUtil;
 		CFrameTable* iFrameTable;
	
		// local input buffer
        TUint8* iTmpBuf;
        const TUint8* iTmpBufPtr;
        TInt 	iTmpBufBytes;
        TInt	iBytesCopied;
        
        TBool   iUsingTmpBuf;
        
        TInt iPos;
//		CEAacPlusUtil::TEAacPlusFormatType iFormatType;
//		TInt iInSamplingFrequency;
//		TInt iOutSamplingFrequency;
//		TInt iNumOfSamples;
		TEAacPlusParserConfig iParserConfig;
        
	};

#endif      // EAACPLUSFRAMEPARSER_H

// End of File
