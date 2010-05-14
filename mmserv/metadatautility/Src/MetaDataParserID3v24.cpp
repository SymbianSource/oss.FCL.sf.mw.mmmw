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
* Description:  This class implements parsing of ID3v2.4 according to the
*                specification found in www.id3.org. Note that only a selected
*				 frames are supported at this time.
*
*/



// INCLUDE FILES
#include    <libc/stdlib.h>
#include	"MetaDataParserID3v24.h"

// CONSTANTS
// (ID3v2.4 specification found in www.id3.org)
const TInt KID3v2HeaderLength 			= 10;
const TInt KID3v2ExtendedHeaderLength 	= 6;
const TInt KID3v2FrameHeaderLength 		= 10;
const TInt KID3v2FrameIdLength 			= 4;
const TInt KID3v2GenreNameLength 		= 24;

_LIT8( KID3v2FrameIdTitle, 			"TIT2" );
_LIT8( KID3v2FrameIdArtist, 		"TPE1" );
_LIT8( KID3v2FrameIdAlbum, 			"TALB" );
_LIT8( KID3v2FrameIdYear, 			"TDRC" );
_LIT8( KID3v2FrameIdComment, 		"COMM" );
_LIT8( KID3v2FrameIdTrack, 			"TRCK" );
_LIT8( KID3v2FrameIdGenre, 			"TCON" );
_LIT8( KID3v2FrameIdComposer, 		"TCOM" );
_LIT8( KID3v2FrameIdCopyright, 		"TCOP" );
_LIT8( KID3v2FrameIdOriginalArtist,	"TOPE" );
_LIT8( KID3v2FrameIdUrl, 			"WOAF" );
_LIT8( KID3v2FrameIdUserUrl, 		"WXXX" );
_LIT8( KID3v2FrameIdJpeg, 			"APIC" );
_LIT8( KID3v2FrameIdDuration, 		"TLEN" );
_LIT8( KID3v2FrameIdDate, 			"TDRC" ); // same as year
_LIT8( KID3v2FrameIdRating,			"POPM" );

_LIT8( KID3v2ImageFormatPngMime, 	"image/png" );
_LIT8( KID3v2ImageFormatPng, 		"png" );
_LIT8( KID3v2ImageFormatJpegMime, 	"image/jpeg" );
_LIT8( KID3v2ImageFormatJpgMime, 	"image/jpg" );
_LIT8( KID3v2ImageFormatJpeg, 		"jpeg" );


// -----------------------------------------------------------------------------
//   (ID3v2.4 specification found in www.id3.org)
//
//   $00   ISO-8859-1 [ISO-8859-1]. Terminated with $00.
//   $01   UTF-16 [UTF-16] encoded Unicode [UNICODE] with BOM. All
//         strings in the same frame SHALL have the same byteorder.
//         Terminated with $00 00.
//   $02   UTF-16BE [UTF-16] encoded Unicode [UNICODE] without BOM.
//         Terminated with $00 00.
//   $03   UTF-8 [UTF-8] encoded Unicode [UNICODE]. Terminated with $00.
// -----------------------------------------------------------------------------
const TInt KMetaDataParserAsciiEncoding		= 0;
const TInt KMetaDataParserUnicodeEncoding	= 1;
const TInt KMetaDataParserUnicodeBEEncoding	= 2;
const TInt KMetaDataParserUnicode8Encoding	= 3;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMetaDataParserID3v24::CMetaDataParserID3v24
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMetaDataParserID3v24::CMetaDataParserID3v24(
	CMetaDataSource& aSource )
	:	CMetaDataParserID3v2(aSource)
	{
    }

// -----------------------------------------------------------------------------
// CMetaDataParserID3v24::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMetaDataParserID3v24::ConstructL()
    {
	User::LeaveIfError(iFs.Connect());
	iExists = ETrue;
#ifdef _DEBUG
	RDebug::Print(_L("CMetaDataParserID3v24::ConstructL - Done"));
#endif
    }

// -----------------------------------------------------------------------------
// CMetaDataParserID3v24::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMetaDataParserID3v24* CMetaDataParserID3v24::NewL(
	CMetaDataSource& aSource )
    {
#ifdef _DEBUG
	RDebug::Print(_L("CMetaDataParserID3v24::NewL"));
#endif
	CMetaDataParserID3v24* self = new( ELeave ) CMetaDataParserID3v24( aSource );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CMetaDataParserID3v24::~CMetaDataParserID3v24()
	{
	iFs.Close();
	}

// -----------------------------------------------------------------------------
// CMetaDataParserID3v24::PrepareToParseL
// -----------------------------------------------------------------------------
//
void CMetaDataParserID3v24::PrepareToParseL()
    {
#ifdef _DEBUG
	RDebug::Print(_L("CMetaDataParserID3v24::PrepareToParseL"));
#endif
	// Most of validation was already done by builder.
	TBuf8<KID3v2HeaderLength> header;
	iSource->ReadL( 0, header );
    // ID3v2.4 header consists of following parts:
	// File identifier	"ID3"
	// Version			$04 00
	// Flags			%abcd0000
	// Size				4 * %0xxxxxxx

	// Read the data length
	iFrameDataSize = 0;
	for ( TInt i = 6; i < 10; i++ )
		{
		iFrameDataSize <<= 7;
		iFrameDataSize |= header[i] & 0x7f;
		}

	// Check if extended header is present; bit-b in Flags above
	if ( header[5] & 0x40 )
		{
		TBuf8<KID3v2ExtendedHeaderLength> extHeader;
		iSource->ReadL( KID3v2HeaderLength, extHeader );
		// ID3v2.4 extended header consists of following parts:
		// Extended header size		4 * %0xxxxxxx
		// Extended flags			$01
		// Size	of padding			$xx

		// Read the data length
		TInt extHeaderSize = 0;
		for ( TInt i = 0; i < 4; i++ )
			{
			extHeaderSize <<= 7;
			extHeaderSize |= extHeader[i] & 0x7f;
			}
		iFrameOffset = KID3v2HeaderLength + KID3v2ExtendedHeaderLength + extHeaderSize;
		}
	else
		{
		iFrameOffset = KID3v2HeaderLength;
		}

	// iFrameDataSize is the entire size of ID3 tag including the header
	iFrameDataSize += iFrameOffset;
    }

// -----------------------------------------------------------------------------
// CMetaDataParserID3v24::GetNextFieldL
// -----------------------------------------------------------------------------
//
void CMetaDataParserID3v24::GetNextFieldL(
	TMetaDataFieldId& aFieldId,
	TInt& aFrameHeaderSize,
	TInt& aFrameSize )
	{
	TBuf8<KID3v2FrameHeaderLength> frameHeader;
	iSource->ReadL( iFrameOffset, frameHeader );
    // ID3v2.4 frame header consists of following parts:
	// Frame identifier	$xx xx xx xx
	// Size				4 * %0xxxxxxx
	// Flags			$xx xx

	aFrameSize = 0;

	if(!frameHeader.Length())
			User::Leave(KErrCorrupt);

	for ( TInt i = 4; i < 8; i++ )
		{
		aFrameSize <<= 7;
		aFrameSize |= frameHeader[i] & 0x7f;
		}

	aFrameHeaderSize = KID3v2FrameHeaderLength;
	TPtrC8 frameId = frameHeader.Left( KID3v2FrameIdLength );
	_LIT8(KEMPTY, "\0\0\0\0");
	TBuf8<10> empty (KEMPTY);
	if ( aFrameSize == 0 && frameId == empty)
		{
		// We have hit the padding -> no more fields to read
		aFieldId = EUnknownMetaDataField;
		aFrameHeaderSize = 0;
		return;
		}
	else if (aFrameSize == 0)
	{
		aFieldId = EUnknownMetaDataField;
		return;
	}

	if ( frameId.Compare( KID3v2FrameIdTitle ) == 0 )
		{
		aFieldId = EMetaDataSongTitle;
		}
	else if ( frameId.Compare( KID3v2FrameIdArtist ) == 0 )
		{
		aFieldId = EMetaDataArtist;
		}
	else if ( frameId.Compare( KID3v2FrameIdAlbum ) == 0 )
		{
		aFieldId = EMetaDataAlbum;
		}
	else if ( frameId.Compare( KID3v2FrameIdYear ) == 0 )
		{
		aFieldId = EMetaDataYear;
		}
	else if ( frameId.Compare( KID3v2FrameIdComment ) == 0 )
		{
		aFieldId = EMetaDataComment;
		}
	else if ( frameId.Compare( KID3v2FrameIdTrack ) == 0 )
		{
		aFieldId = EMetaDataAlbumTrack;
		}
	else if ( frameId.Compare( KID3v2FrameIdGenre ) == 0 )
		{
		aFieldId = EMetaDataGenre;
		}
	else if ( frameId.Compare( KID3v2FrameIdComposer ) == 0 )
		{
		aFieldId = EMetaDataComposer;
		}
	else if ( frameId.Compare( KID3v2FrameIdCopyright ) == 0 )
		{
		aFieldId = EMetaDataCopyright;
		}
	else if ( frameId.Compare( KID3v2FrameIdOriginalArtist ) == 0 )
		{
		aFieldId = EMetaDataOriginalArtist;
		}
	else if ( frameId.Compare( KID3v2FrameIdUrl ) == 0 )
		{
		aFieldId = EMetaDataUrl;
		}
	else if ( frameId.Compare( KID3v2FrameIdUserUrl ) == 0 )
		{
		aFieldId = EMetaDataUserUrl;
		}
	else if ( frameId.Compare( KID3v2FrameIdJpeg ) == 0 )
		{
		aFieldId = EMetaDataJpeg;
		}
	else if ( frameId.Compare( KID3v2FrameIdDuration ) == 0 )
		{
		aFieldId = EMetaDataDuration;
		}
	else if ( frameId.Compare( KID3v2FrameIdDate ) == 0 )
		{
		aFieldId = EMetaDataDate;
		}
	else if ( frameId.Compare( KID3v2FrameIdRating ) == 0 )
		{
		aFieldId = EMetaDataRating;
		}
	else
		{
		aFieldId = EUnknownMetaDataField;
		}
	}

// -----------------------------------------------------------------------------
// CMetaDataParserID3v23::GetUserUrlL
// -----------------------------------------------------------------------------
//
void CMetaDataParserID3v24::GetUserUrlL(
	TInt aSize )
	{
#ifdef _DEBUG
	RDebug::Print(_L("CMetaDataParserID3v23::GetUserUrlL"));
#endif
	HBufC8* frame = HBufC8::NewLC( aSize );
	TPtr8 des( frame->Des() );

	iSource->ReadL( iFrameOffset + KID3v2FrameHeaderLength, des, aSize );

	if ( des.Length() < aSize )
		{
		// Partial frame
		User::Leave( KErrCorrupt );
		}

	TInt encoding = (TInt) (des.Left(1))[0];

	if(encoding == 0x00)
	{
		_LIT8(KNULL, "\0");
		TInt descOffset = des.Find(KNULL);
		if(descOffset + 2 > aSize)
		{
			CleanupStack::PopAndDestroy();  // frame
			return; // empty frame
		}
		TPtrC8 info = des.Mid(descOffset + 2);
		TInt length = info.Length();
		if ( length )
			{
			HBufC* data16 = HBufC::NewLC( length );
			TPtr unicode( data16->Des() );
			unicode.Copy( info );
			iContainer->AppendL( EMetaDataUserUrl, unicode );
			CleanupStack::PopAndDestroy();  // data16
			}
	}
	else
	{
		_LIT8(KNULL, "\0\0");
		TInt descOffset = des.Find(KNULL);
		if(descOffset + 3 > aSize)
		{
			CleanupStack::PopAndDestroy();  // frame
			return; // empty frame
		}
		TPtrC8 info = des.Mid(descOffset + 3);
		TInt length = info.Length();
		if ( length )
			{
			HBufC* data16 = HBufC::NewLC( length );
			TPtr unicode( data16->Des() );
			unicode.Copy( info );
			iContainer->AppendL( EMetaDataUserUrl, unicode );
			CleanupStack::PopAndDestroy();  // data16
			}
	}

	CleanupStack::PopAndDestroy();  // frame
	}

// -----------------------------------------------------------------------------
// CMetaDataParserID3v24::GetTextInfoL
// -----------------------------------------------------------------------------
//
void CMetaDataParserID3v24::GetTextInfoL(
	TMetaDataFieldId aFieldId,
	TInt aSize )
	{
	HBufC8* frame = HBufC8::NewLC( aSize );
	TPtr8 des( frame->Des() );

	iSource->ReadL( iFrameOffset + KID3v2FrameHeaderLength, des, aSize );

	if ( des.Length() < aSize )
		{
		// Partial frame
		User::Leave( KErrCorrupt );
		}

	TInt encoding = (TInt) (des.Left(1))[0];
	TPtrC8 info = StripTrailingZeroes( des.Mid(1), encoding );
	TInt length = info.Length();
	if ( length )
		{
		HBufC* data16 = HBufC::NewLC( length );
		TPtr unicode( data16->Des() );
		if ( ConvertToUnicodeL(encoding, info, unicode) == KErrNone )
			{
			iContainer->AppendL( aFieldId, unicode );
			}
		CleanupStack::PopAndDestroy();  // data16
		}

	CleanupStack::PopAndDestroy();  // frame
	}

// -----------------------------------------------------------------------------
// CMetaDataParserID3v24::GetGenreL
// -----------------------------------------------------------------------------
//
void CMetaDataParserID3v24::GetGenreL(
	TInt aSize )
	{
#ifdef _DEBUG
	RDebug::Print(_L("CMetaDataParserID3v24::GetGenreL"));
#endif

	HandleV2GetGenreL( aSize, KID3v2FrameHeaderLength, KID3v2GenreNameLength  );
	}
// CMetaDataParserID3v24::GetTextYearL
// -----------------------------------------------------------------------------
//
void CMetaDataParserID3v24::GetTextYearL(
	TInt aSize )
	{
	HBufC8* frame = HBufC8::NewLC( aSize );
	TPtr8 des( frame->Des() );

	iSource->ReadL( iFrameOffset + KID3v2FrameHeaderLength, des, aSize );

	if ( des.Length() < aSize )
		{
		// Partial frame
		User::Leave( KErrCorrupt );
		}

	TInt encoding = (TInt) (des.Left(1))[0];
	// Timestamp may be one of the following formats:
	// yyyy, yyyy-MM, yyyy-MM-dd, yyyy-MM-ddTHH, yyyy-MM-ddTHH:mm and yyyy-MM-ddTHH:mm:ss
	// We are only interested in year.
	TPtrC8 drc = StripTrailingZeroes( des.Mid(1), encoding );
	TInt length = drc.Length();
	if ( length >= 4) // 4 bytes year
		{
		TPtrC8 info = drc.Mid(0, 4);
		length = info.Length();
		HBufC* data16 = HBufC::NewLC( length );
		TPtr unicode( data16->Des() );
		if ( ConvertToUnicodeL(encoding, info, unicode) == KErrNone )
			{
			iContainer->AppendL( EMetaDataYear, unicode );
			}
		CleanupStack::PopAndDestroy();  // data16
		}

	CleanupStack::PopAndDestroy();  // frame
	}

// -----------------------------------------------------------------------------
// CMetaDataParserID3v24::GetCommentL
// -----------------------------------------------------------------------------
//
void CMetaDataParserID3v24::GetCommentL(
	TInt aSize )
	{
	HBufC8* frame = HBufC8::NewLC( aSize );
	TPtr8 des( frame->Des() );

	iSource->ReadL( iFrameOffset + KID3v2FrameHeaderLength, des, aSize );

	if ( des.Length() < aSize )
		{
		// Partial frame
		User::Leave( KErrCorrupt );
		}
		
	if(aSize <= 4) // emppty frame
	{
		CleanupStack::PopAndDestroy();  // frame
		return;
	}
	TInt encoding = (TInt) (des.Left(1))[0];
	// Skip over TextEncoding(1 byte) and Language(3 bytes)
	TPtr8 text = des.MidTPtr(4);
	// Skip over Content description from Comment
	TInt contentDesLength;
	if(encoding == 0 || encoding == 3)
		{
		_LIT8(KNull, "\0");
		contentDesLength = text.Find(KNull);
		text = text.MidTPtr(contentDesLength + 1);
		}
	else
		{
		_LIT8(KNull, "\0\0");
		contentDesLength = text.Find(KNull);
		text = text.MidTPtr(contentDesLength + 2);
		}
	TPtrC8 info = StripTrailingZeroes( text, encoding);
	TInt length = info.Length();
	if ( length )
		{
		HBufC* data16 = HBufC::NewLC( length );
		TPtr unicode( data16->Des() );
		if ( ConvertToUnicodeL(encoding, info, unicode) == KErrNone )
			{
			iContainer->AppendL( EMetaDataComment, unicode );
			}
		CleanupStack::PopAndDestroy();  // data16
		}

	CleanupStack::PopAndDestroy();  // frame
	}

// -----------------------------------------------------------------------------
// CMetaDataParserID3v24::GetUrlL
// -----------------------------------------------------------------------------
//
void CMetaDataParserID3v24::GetUrlL(
	TInt aSize )
	{
	HBufC8* frame = HBufC8::NewLC( aSize );
	TPtr8 des( frame->Des() );

	iSource->ReadL( iFrameOffset + KID3v2FrameHeaderLength, des, aSize );

	if ( des.Length() < aSize )
		{
		// Partial frame
		User::Leave( KErrCorrupt );
		}

	// Ignore information followed by termination $00
    TInt trueEnd( des.Locate(0) );
    if ( trueEnd == KErrNotFound )
        {
        trueEnd = des.Length();
		}

	TPtrC8 info = des.Left( trueEnd );
	TInt length = info.Length();
	if ( length )
		{
		HBufC* data16 = HBufC::NewLC( length );
		TPtr unicode( data16->Des() );
		unicode.Copy( info );
		iContainer->AppendL( EMetaDataUrl, unicode );
		CleanupStack::PopAndDestroy();  // data16
		}

	CleanupStack::PopAndDestroy();  // frame
	}

// -----------------------------------------------------------------------------
// CMetaDataParserID3v24::GetJpegL
// -----------------------------------------------------------------------------
//
void CMetaDataParserID3v24::GetJpegL(
	TInt aSize )
	{
	HBufC8* frame = HBufC8::NewLC( aSize );
	TPtr8 des( frame->Des() );

	iSource->ReadL( iFrameOffset + KID3v2FrameHeaderLength, des, aSize );

	if ( des.Length() < aSize )
		{
		// Partial frame
		User::Leave( KErrCorrupt );
		}

	TInt encoding = (TInt) (des.Left(1))[0];

	TInt mimeLength = des.Mid(1).Locate(0);
	if(mimeLength <= 0 )
		{
		User::Leave(KErrCorrupt);	
		}
	TPtrC8 mime = des.Mid(1, mimeLength);

	if ( ( mime.Compare( KID3v2ImageFormatJpeg ) != 0 ) &&
		 ( mime.Compare( KID3v2ImageFormatJpegMime ) != 0 ) && 
		 ( mime.Compare( KID3v2ImageFormatJpgMime ) != 0 ) &&
		 ( mime.Compare( KID3v2ImageFormatPngMime ) != 0 ) &&
		 ( mime.Compare( KID3v2ImageFormatPng ) != 0 )
		 )
		{
		CleanupStack::PopAndDestroy();  // frame
		return;
		}

	if(mimeLength+3 > des.Length())
		{
		CleanupStack::PopAndDestroy();  // frame
		return;
		}

	TInt pictureType = (TInt) (des.Mid(mimeLength + 2, 1))[0];
	if ( !NeedRetrieveAlbumArt(pictureType) )
		{
		CleanupStack::PopAndDestroy();  // frame
		return;
		}

    TInt picOffset;
    TPtrC8 text8 = des.Mid(mimeLength + 3 );
    if ( encoding == KMetaDataParserAsciiEncoding )
        {
        picOffset = text8.Locate(0);
        if ( picOffset == KErrNotFound )
            {
            CleanupStack::PopAndDestroy();  // frame
            return;
            }
        
        picOffset++;        // Description termination mark ($00)
        }
    else if ( encoding == KMetaDataParserUnicodeEncoding )
        {
        _LIT8(KNULL, "\0\0");               // 2 bytes of NULL
        picOffset = text8.Find(KNULL);
        if ( picOffset == KErrNotFound )
            {
            CleanupStack::PopAndDestroy();  // frame
            return;
            }
        if (picOffset % 2)      // check for offset odd number 
            {
            picOffset++;        // add 1 for word boundary
            }
		picOffset += 2;		// Description termination mark ($00 00)
		}
	else
		{
		CleanupStack::PopAndDestroy();  // frame
		return;
		}

	TPtrC8 pic = des.Mid(picOffset + mimeLength + 3);
	TInt length = pic.Length();
    if ( length )
        {
        // save the offset and the size for retrieving the album art later
        iAlbumOffset = picOffset + mimeLength + 3;
        iAlbumType = pictureType;
        
        if (iAlbumPtr)
            {
            delete iAlbumPtr;       // delete the previous buffer
            }
        CleanupStack::Pop();        // frame - pop from Cleanup stack
        iAlbumPtr = frame;          // save the buffer
        }
    else
        {
        CleanupStack::PopAndDestroy();  // frame        
        }
	}

// -----------------------------------------------------------------------------
// CMetaDataParserID3v24::GetDurationL
// -----------------------------------------------------------------------------
//
void CMetaDataParserID3v24::GetDurationL(
	TInt aSize )
	{
#ifdef _DEBUG
	RDebug::Print(_L("CMetaDataParserID3v23::GetDurationL"));
#endif
	HBufC8* frame = HBufC8::NewLC( aSize );
	TPtr8 des( frame->Des() );

	iSource->ReadL( iFrameOffset + KID3v2FrameHeaderLength, des, aSize );

	if ( des.Length() < aSize )
		{
		// Partial frame
		User::Leave( KErrCorrupt );
		}

	TInt encoding = (TInt) (des.Left(1))[0];
	TPtrC8 info = StripTrailingZeroes( des.Mid(1), encoding ); // $00
	TInt length = info.Length();
	if ( length )
		{
		HBufC* data16 = HBufC::NewLC( length );
		TPtr unicode( data16->Des() );
		if ( ConvertToUnicodeL(encoding, info, unicode) == KErrNone )
			{
			TLex16 lex(unicode);
			TReal milliSec = 0;
			TBuf16<10> info1;
			
			lex.Val(milliSec);				
			info1.Num(milliSec/1000, TRealFormat (9, 3));
			iContainer->AppendL( EMetaDataDuration, info1 );
			}
		CleanupStack::PopAndDestroy();  // data16
		
		}

	CleanupStack::PopAndDestroy();  // frame
	}



// -----------------------------------------------------------------------------
// CMetaDataParserID3v24::GetTextDateL
// -----------------------------------------------------------------------------
//
void CMetaDataParserID3v24::GetTextDateL(
	TInt aSize )
	{
#ifdef _DEBUG
	RDebug::Print(_L("CMetaDataParserID3v24::GetTextDateL"));
#endif
	GetTextInfoL( EMetaDataDate, aSize );
	}

// -----------------------------------------------------------------------------
// CMetaDataParserID3v24::GetRatingL
// -----------------------------------------------------------------------------
//
void CMetaDataParserID3v24::GetRatingL(	TInt aSize )
	{
#ifdef _DEBUG
	RDebug::Print(_L("CMetaDataParserID3v24::GetRatingL"));
#endif
	HBufC8* frame = HBufC8::NewLC( aSize );
	TPtr8 des( frame->Des() );

	iSource->ReadL( iFrameOffset + KID3v2FrameHeaderLength, des, aSize );

	if ( des.Length() < aSize )
		{
		// Partial frame
		User::Leave( KErrCorrupt );
		}
	_LIT8(KNull, "\0");
	TInt offset = des.Find(KNull);
	if(offset == KErrNotFound)
		{
		CleanupStack::PopAndDestroy();
		return;
		}
	TUint8 rating = des[offset];
	if ( rating > 0)
		{
		TBuf<8> data16;
		data16.AppendNum(rating);
		iContainer->AppendL( EMetaDataRating, data16 );
		}
	CleanupStack::PopAndDestroy();  // frame
	}

// -----------------------------------------------------------------------------
// CMetaDataParserID3v24::ConvertToUnicodeL
// -----------------------------------------------------------------------------
//
TInt CMetaDataParserID3v24::ConvertToUnicodeL(
	TInt aEncoding,
	const TDesC8& aDesc,
	TDes16& aUnicode )
	{
#ifdef _DEBUG
	RDebug::Print(_L("CMetaDataParserID3v23::ConvertToUnicodeL"));
#endif
	TPtrC8 unicodeData;
    TUint characterSetId = 0;
	TInt err;

	CCnvCharacterSetConverter* charSetConv = CCnvCharacterSetConverter::NewLC();
	TInt state = CCnvCharacterSetConverter::KStateDefault;

	if ( aEncoding == KMetaDataParserAsciiEncoding )
		{
		if(iAutoDetectChinese || iAutoDetectRussian || iAutoDetectJapanese)
			{
			err = AutoDetectL(aDesc, aUnicode);
			CleanupStack::PopAndDestroy();	// charSetConv
	        return err;
			}
		else
			{
			// ISO-8859-1
			characterSetId = KCharacterSetIdentifierIso88591;
			unicodeData.Set( aDesc );
			}
		}
	else if ( aEncoding == KMetaDataParserUnicodeEncoding )
		{
		// UTF-16 Big Endian with BOM
		TInt bom = UnicodeBOM(aDesc);
		if ( bom == KUnicodeBOMBigEndian )
			{
			//characterSetId = KCharacterSetIdentifierUcs2;
			characterSetId = KCharacterSetIdentifierUnicodeBig;
			// Skip BOM & Terminate marks
			unicodeData.Set( aDesc.Mid( 2, aDesc.Length() - 2 ) );
			}
		else if ( bom == KUnicodeBOMLittleEndian )
			{
			characterSetId = KCharacterSetIdentifierUnicodeLittle;
			// Skip BOM & Terminate marks
			unicodeData.Set( aDesc.Mid( 2, aDesc.Length() - 2 ) );
			}
		else
			{
			// UTF-16 Big Endian without BOM
			characterSetId = KCharacterSetIdentifierUnicodeBig;
			unicodeData.Set( aDesc );
			}
		}
	else if ( aEncoding == KMetaDataParserUnicodeBEEncoding )
		{
		// UTF-16 Big Endian without BOM
		characterSetId = KCharacterSetIdentifierUnicodeBig;
		unicodeData.Set( aDesc );
		}
	else if ( aEncoding == KMetaDataParserUnicode8Encoding )
		{
		// UTF-8
		characterSetId = KCharacterSetIdentifierUtf8;
		unicodeData.Set( aDesc );
		}
	else
		{
		User::Leave(KErrNotSupported);
		}

	charSetConv->PrepareToConvertToOrFromL(characterSetId, *iCharacterSet, iFs);
	err = charSetConv->ConvertToUnicode(aUnicode, unicodeData, state);

#ifdef _DEBUG
	RDebug::Print(_L("CMetaDataParserID3v24::ConvertToUnicode :-> Tag Size[%d] Unicode Tag Size[%d]Bytes Unconverted[%d]"),
		unicodeData.Length(), aUnicode.Length(), err);
#endif

	CleanupStack::PopAndDestroy();	// charSetConv
	return err;
	}

//  End of File
