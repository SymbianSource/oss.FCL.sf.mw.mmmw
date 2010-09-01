/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class implements an 3GPExt metadata parser
*
*/

// INCLUDE FILES
#include "3GPExtParser.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C3GPExtParser::C3GPExtParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
C3GPExtParser::C3GPExtParser(MP4Handle& aMP4Handle): iMP4Handle(aMP4Handle), ilst(NULL, 0)
	{
    }

// -----------------------------------------------------------------------------
// C3GPExtParser::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void C3GPExtParser::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// C3GPExtParser::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C C3GPExtParser* C3GPExtParser::NewL(MP4Handle& aMP4Handle)
    {
	C3GPExtParser* self = new( ELeave ) C3GPExtParser (aMP4Handle);
    return self;
    }

// Destructor
C3GPExtParser::~C3GPExtParser()
	{
	}

// -----------------------------------------------------------------------------
// 3GPExtParser::GetilstBoxesL
// -----------------------------------------------------------------------------
//
EXPORT_C void C3GPExtParser::GetilstBoxesL(TMetaDataFieldId /*aFieldId*/, HBufC** /*aBuf*/)
{
    User::Leave(KErrNotSupported);    
}
// -----------------------------------------------------------------------------
// 3GPExtParser::GetilstBoxesL for 8-bit descriptor fields 
// -----------------------------------------------------------------------------
//
EXPORT_C void C3GPExtParser::GetilstBoxesL(TMetaDataFieldId /*aFieldId*/, HBufC8** /*aBuf*/)
{
    User::Leave(KErrNotSupported);   
}

// -----------------------------------------------------------------------------
// C3GPExtParser::Is3GPExtMetadataL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool C3GPExtParser::Is3GPExtMetadataL()
{
    User::Leave(KErrNotSupported); 
    return EFalse;   
}

// -----------------------------------------------------------------------------
// 3GPExtParser::MapID3v1GenreToString()
// -----------------------------------------------------------------------------
//
void C3GPExtParser::ParseIlstBoxesL()
{
}

// -----------------------------------------------------------------------------
// 3GPExtParser::MapID3v1GenreToString()
// -----------------------------------------------------------------------------
//
void C3GPExtParser::MapID3GenreToString(TInt /*aNum*/, TDes& /*aGenrePtr*/)
{
}
		
			
// -----------------------------------------------------------------------------
// 3GPExtParser::TranslateMP4Err
// -----------------------------------------------------------------------------
//
TInt C3GPExtParser::TranslateMP4Err(MP4Err /*aError*/)
{
	return KErrNotSupported;
}
	

	
//  End of File
