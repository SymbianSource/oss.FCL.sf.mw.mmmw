/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MmfTsPlay class member functions
*
*/



// INCLUDE FILES
#include <Stiftestinterface.h>
#include "MmfTsPlay.h"
#include <MmfMeta.h>

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmfTsPlay::CMmfTsPlay
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMmfTsPlay::CMmfTsPlay()
    {

    }

// -----------------------------------------------------------------------------
// CMmfTsPlay::ConstructL
// Symbian 2nd phase constructor can leave.
// Note: If OOM test case uses STIF Logger, then STIF Logger must be created
// with static buffer size parameter (aStaticBufferSize). Otherwise Logger
// allocates memory from heap and therefore causes error situations with OOM
// testing. For more information about STIF Logger construction, see STIF Users
// Guide.
// -----------------------------------------------------------------------------
//
/*void CMmfTsPlay::ConstructL()
    {
    iLog = CStifLogger::NewL( KMmfTsPlayLogPath,
                          KMmfTsPlayLogFile);
    }*/

void CMmfTsPlay::ConstructL()
    {
	if ( ! iScheduler)
		{
		iScheduler = new (ELeave) CActiveScheduler;
		CActiveScheduler::Install(iScheduler); // install as active scheduler
		}
    }

// -----------------------------------------------------------------------------
// CMmfTsPlay::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmfTsPlay* CMmfTsPlay::NewL()
    {
    CMmfTsPlay* self = new (ELeave) CMmfTsPlay;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CMmfTsPlay::~CMmfTsPlay()
    {
   // delete iLog;
	delete iScheduler;
	delete iLogger;
    }

/*-------------------------------------------------------------------------------

    Class: CMmfTsPlay

    Method: InitL
    Description: Initialization function.
    Init is used to initialize the Test Module.
    Parameters:	TName& aIniFile: in: Filename:
                    Configuration file for the test module

    Return Values: KErrNone: No error
                   <value_1: Description
		            value_n: Description line 1
			                 description line 2>

    Errors/Exceptions: None
    Status: Draft

------------------------------------------------------------------------------- */
TInt CMmfTsPlay::InitL( TFileName& aIniFile , TBool /*aFirstTime*/ )
    {
	CStifParser *parser;
	RDebug::Print(_L("CMmfTsPlay::InitL:"));
	TParsePtrC fp(aIniFile);
	//fp.Set();
	parser = CStifParser::NewL(fp.DriveAndPath() , fp.NameAndExt() );
	CleanupStack::PushL(parser);

	if (parser)
		{
		RDebug::Print(_L("CMmfTsPlay::InitL: parser is not null"));
		CStifSectionParser *section;

		section = parser->SectionL(KSectionStart, KSectionEnd);
		CleanupStack::PushL(section);
		TPtrC logFileName;
		TPtrC logPath;
		if (section)
			{
			RDebug::Print(_L("CMmfTsPlay::InitL: section is not null"));


			if ( (section->GetLine(KLogFileTag, logFileName, ENoTag) == KErrNone)  && (section->GetLine(KLogPathTag, logPath, ENoTag) == KErrNone) )
				{
				RDebug::Print(_L("CMmfTsPlay::InitL: InitLoggerL(logPath, logFileName);"));
				InitLoggerL(logPath, logFileName);
				}
			}
		if (!iLogger)
			{
			RDebug::Print(_L("CMmfTsPlay::InitL: InitLoggerL(); again"));
			InitLoggerL();
			}
		iLogger->Log(KOpeningLogFilepath, &logPath);
		iLogger->Log(KLoggerStarted, &aIniFile);
		CleanupStack::PopAndDestroy(section);	//section
		}

	CleanupStack::PopAndDestroy(parser);	//parser

	// Binh
    iMetaDataFieldName[EUnknownMetaDataField] = _L("Unknown");
    iMetaDataFieldName[EMetaDataSongTitle] = KMMFMetaEntrySongTitle;
    iMetaDataFieldName[EMetaDataArtist] = KMMFMetaEntryArtist;
    iMetaDataFieldName[EMetaDataAlbum] = KMMFMetaEntryAlbum;
    iMetaDataFieldName[EMetaDataYear] = KMMFMetaEntryYear;
    iMetaDataFieldName[EMetaDataComment] = KMMFMetaEntryComment;
    iMetaDataFieldName[EMetaDataAlbumTrack] = KMMFMetaEntryAlbumTrack;
    iMetaDataFieldName[EMetaDataGenre] = KMMFMetaEntryGenre;
    iMetaDataFieldName[EMetaDataComposer] = KMMFMetaEntryComposer;
    iMetaDataFieldName[EMetaDataCopyright] = KMMFMetaEntryCopyright;
    iMetaDataFieldName[EMetaDataOriginalArtist] = KMMFMetaEntryOriginalArtist;
    iMetaDataFieldName[EMetaDataUrl] = KMMFMetaEntryWOAF;
    iMetaDataFieldName[EMetaDataUserUrl] = KMMFMetaEntryWXXX;
    iMetaDataFieldName[EMetaDataJpeg] = KMMFMetaEntryAPIC;
    iMetaDataFieldName[EMetaDataVendor] = KMMFMetaEntryVendorID;
    iMetaDataFieldName[EMetaDataRating] = _L("Rating");
    iMetaDataFieldName[EMetaDataUniqueFileIdentifier] = _L("UniqueFileID");
    iMetaDataFieldName[EMetaDataDuration] = _L("Duration");
    iMetaDataFieldName[EMetaDataDate] = _L("Date");

    iMetaDataFieldID[EUnknownMetaDataField] = EUnknownMetaDataField;
	iMetaDataFieldID[EMetaDataSongTitle] = EMetaDataSongTitle;
    iMetaDataFieldID[EMetaDataArtist] = EMetaDataArtist;
	iMetaDataFieldID[EMetaDataAlbum] = EMetaDataAlbum;
    iMetaDataFieldID[EMetaDataYear] = EMetaDataYear;
	iMetaDataFieldID[EMetaDataComment] = EMetaDataComment;
    iMetaDataFieldID[EMetaDataAlbumTrack] = EMetaDataAlbumTrack,
	iMetaDataFieldID[EMetaDataGenre] = EMetaDataGenre;
    iMetaDataFieldID[EMetaDataComposer] = EMetaDataComposer;
	iMetaDataFieldID[EMetaDataCopyright] = EMetaDataCopyright;
    iMetaDataFieldID[EMetaDataOriginalArtist] = EMetaDataOriginalArtist;
	iMetaDataFieldID[EMetaDataUrl] = EMetaDataUrl;
    iMetaDataFieldID[EMetaDataUserUrl] = EMetaDataUserUrl;
	iMetaDataFieldID[EMetaDataJpeg] = EMetaDataJpeg;
    iMetaDataFieldID[EMetaDataVendor] = EMetaDataVendor;
	iMetaDataFieldID[EMetaDataRating] = EMetaDataRating;
    iMetaDataFieldID[EMetaDataUniqueFileIdentifier] = EMetaDataUniqueFileIdentifier;
	iMetaDataFieldID[EMetaDataDuration] = EMetaDataDuration;
    iMetaDataFieldID[EMetaDataDate] = EMetaDataDate;

    return KErrNone;
    }

/* -------------------------------------------------------------------------------

    Class: CMmfTsPlay
    Method: InitLoggerL
    Description: Initialize the logger for this test module
    Parameters:	File name where to append data
    Return Values: Error value
    Errors/Exceptions: <description how errors and exceptions are handled>
    Status: Draft

------------------------------------------------------------------------------- */
void CMmfTsPlay::InitLoggerL( const TFileName& aLogPath , const TFileName& aLogFile)
    {
		//iLogger = CStifLogger::NewL(aLogPath, aLogFile, ETxt, EFile, EFalse);	//, ETrue, ETrue, ETrue, ETrue, EFalse);
		iLogger = CStifLogger::NewL(aLogPath, aLogFile, CStifLogger::ETxt, CStifLogger::EFile, EFalse) ;
		if (  ! iLogger  )
			{
			RDebug::Print(_L("CMmfTsPlay::InitL: InitLoggerL(); with filename failed"));
			InitLoggerL();
			}
    }

void CMmfTsPlay::InitLoggerL()
	{
	RDebug::Print(_L("CMmfTsPlay::InitL: InitLoggerL(); with default"));
	iLogger = CStifLogger::NewL(KDefLogPath, KDefLogFile, CStifLogger::ETxt, CStifLogger::EFile, EFalse);
	}

// -----------------------------------------------------------------------------
// CMmfTsPlay::GetTestCasesL
// GetTestCases is used to inquire test cases from the Test Module. Test
// cases are stored to array of test cases. The Test Framework will be
// the owner of the data in the RPointerArray after GetTestCases return
// and it does the memory deallocation.
// -----------------------------------------------------------------------------
//
TInt CMmfTsPlay::GetTestCasesL(
    const TFileName& aConfig,
    RPointerArray<TTestCaseInfo>& aTestCases )
    {

   //Get a parser for the config file
	iLogger->Log(KOpeningConfig, &aConfig);

	if (! aConfig.Length() ) {
		iLogger->Log(KConfigRequired() );
		return KErrConfigInvalid;
	}

	TParsePtrC fp(aConfig);
	CStifParser *parser = CStifParser::NewL(fp.DriveAndPath() , fp.NameAndExt() );
	CleanupStack::PushL(parser);

	//Get a section parser
	CStifSectionParser *section;
	TInt x = 0;

	//Read all cases in config file
	//while ( section = parser->SectionL(KTestCaseConfigStart , KTestCaseConfigEnd, x++) )

	while ( (section = parser->NextSectionL(KTestCaseConfigStart , KTestCaseConfigEnd) ),section  )	// use comma to avoid warning
		{
		CleanupStack::PushL(section);
		TInt id = -1;
		TPtrC TestCaseTitle;
		TInt ErrorResult = KErrNone;

		CStifItemParser *item;
		item = section->GetItemLineL(KTagId);
		if (item)
			{
			CleanupStack::PushL(item);

			if (!item->GetInt(KTagId, id))
				{
				ErrorResult = section->GetLine(KTagTitle, TestCaseTitle, ENoTag);
				if (!ErrorResult) {iLogger->Log( _L("Adding case: (%d) %S") , id, &TestCaseTitle ); }
				}
			else
				{
				id = -1;
				}
			CleanupStack::PopAndDestroy(item);	//item
			}

        //leer el id del test case
        //leer el titulo del caso
        //si es valido el id entonces crear el caso
        if (0 < id && id <= MAX_TESTCASE_ID)
            {
            TTestCaseInfo* testCase = new (ELeave) TTestCaseInfo();
            CleanupStack::PushL(testCase);
            testCase->iCaseNumber = ++x;
            iLogger->Log(_L("CaseNumber: (%d)"), testCase->iCaseNumber);
            testCase->iTitle.Format(KTestCaseTitleFormat, id, &TestCaseTitle); //testCase->iTitle.Copy( KTestCaseTitleFormat() );

			User::LeaveIfError( aTestCases.Append( testCase ) );

			CleanupStack::Pop( testCase );	//testCase
			}

		CleanupStack::PopAndDestroy(section);	//section
		}

	CleanupStack::PopAndDestroy(parser);	//parser

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CMmfTsPlay::RunTestCaseL
// RunTestCaseL is used to run an individual test case specified
// by aTestCase. Test cases that can be run may be requested from
// Test Module by GetTestCases method before calling RunTestCase.
// -----------------------------------------------------------------------------
//
TInt CMmfTsPlay::RunTestCaseL( const TInt aCaseNumber , const TFileName& aConfig , TTestResult& aResult )
    {
	TInt id=-1;

	//Get a parser for the config file
	iLogger->Log(KOpeningConfig, &aConfig);

	if (! aConfig.Length() ) {
		iLogger->Log(KConfigRequired() );
		aResult.iResultDes.Copy(KConfigRequired());
		aResult.iResult = KErrNotExecuted;
		return KErrConfigInvalid;
	}

    TParsePtrC fp(aConfig);
    iLogger->Log(_L("Opening configuration file [%S], case #%d"), &aConfig,
            aCaseNumber);
    CStifParser *parser = NULL;
    TRAPD(error, parser = CStifParser::NewL(fp.DriveAndPath() , fp.NameAndExt() ));
    iLogger->Log(_L("CStifParser::NewL [%d] \n"), error);
    if (error == KErrNone)
        {
        CleanupStack::PushL(parser);
        }

    //Get a section parser
    TInt retVal = 0;
    CStifSectionParser *section = 0;
    CStifItemParser *item = 0;
     section = parser->NextSectionL(KTestCaseConfigStart, KTestCaseConfigEnd,
            aCaseNumber);
    
    if (section)
        {
        CleanupStack::PushL(section);
        item = section->GetItemLineL(KTagId);

		if (item) {
			CleanupStack::PushL(item);

			if (!item->GetInt(KTagId, id)) {
				TPtrC TestCaseTitle;

				TInt TitleErrorResult = section->GetLine(KTagTitle, TestCaseTitle, ENoTag);
				if (!TitleErrorResult) {iLogger->Log( _L("Executing test case (%d) %S") , id, &TestCaseTitle ); }

				//Call Funtion to perform test
				__UHEAP_MARK;
				TRAPD(error, retVal = RunTestCaseFunctionL(id, section, aResult););
				REComSession::FinalClose();
				__UHEAP_MARKEND;

				if (error)
					{
					retVal = KErrLeft;
					aResult.iResult=error;
					aResult.iResultDes.Copy( KTestCaseResultLeft() );
					}

				iLogger->Log( _L("Finished Executing test case (%d)"), id);
				iLogger->Log( _L("  Error code (%d)"), retVal);
				iLogger->Log( _L("  aResult.iResult=(%d)") , aResult.iResult);
				iLogger->Log( _L("  aResult.iResultDes=(%S)\r\n") , &aResult.iResultDes);

			} else {
				id=-1;
			}
			CleanupStack::PopAndDestroy(item);
		}
		CleanupStack::PopAndDestroy(section);
	}
	CleanupStack::PopAndDestroy(parser);

	if (-1!=id)
	{
		return retVal;
	}
	iLogger->Log(KConfigInvalid() );
	aResult.iResultDes.Copy(KConfigInvalid());
	aResult.iResult = KErrNotFound;
	return KErrNotFound;
    }

//  End of File
// -----------------------------------------------------------------------------
// CNormalEx::OOMTestQueryL
// Used to check if a particular test case should be run in OOM conditions and
// which memory allocations should fail.
//
// NOTE: This method is virtual and must be implemented only if test case
// should be executed using OOM conditions.
// -----------------------------------------------------------------------------
//
TBool CMmfTsPlay::OOMTestQueryL(
                                const TFileName& /* aTestCaseFile */,
                                const TInt /* aCaseNumber */,
                                TOOMFailureType& /* aFailureType */,
                                TInt& /* aFirstMemFailure */,
                                TInt& /* aLastMemFailure */ )
    {
    _LIT( KOOMTestQueryL, "CMmfTsPlay::OOMTestQueryL" );
    iLogger->Log( KOOMTestQueryL );

    return EFalse;

    }

// -----------------------------------------------------------------------------
// CMmfTsPlay::OOMTestInitializeL
// Used to perform the test environment setup for a particular OOM test case.
// Test Modules may use the initialization file to read parameters for Test
// Module initialization but they can also have their own configure file or
// some other routine to initialize themselves.
//
// NOTE: This method is virtual and must be implemented only if test case
// should be executed using OOM conditions.
// -----------------------------------------------------------------------------
//
void CMmfTsPlay::OOMTestInitializeL(
                                const TFileName& /* aTestCaseFile */,
                                const TInt /* aCaseNumber */ )
    {
    }

// -----------------------------------------------------------------------------
// CMmfTsPlay::OOMHandleWarningL
// In some cases the heap memory allocation should be skipped, either due to
// problems in the OS code or components used by the code being tested, or even
// inside the tested components which are implemented this way on purpose (by
// design), so it is important to give the tester a way to bypass allocation
// failures.
//
// NOTE: This method is virtual and must be implemented only if test case
// should be executed using OOM conditions.
// -----------------------------------------------------------------------------
//
void CMmfTsPlay::OOMHandleWarningL(
                                const TFileName& /* aTestCaseFile */,
                                const TInt /* aCaseNumber */,
                                TInt& /* aFailNextValue */ )
    {
    }

// -----------------------------------------------------------------------------
// CMmfTsPlay::OOMTestFinalizeL
// Used to perform the test environment cleanup for a particular OOM test case.
//
// NOTE: This method is virtual and must be implemented only if test case
// should be executed using OOM conditions.
// -----------------------------------------------------------------------------
//
void CMmfTsPlay::OOMTestFinalizeL(
                                const TFileName& /* aTestCaseFile */,
                                const TInt /* aCaseNumber */ )
    {
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point
// Returns: CTestModuleBase*: Pointer to Test Module object
// -----------------------------------------------------------------------------
//
EXPORT_C CTestModuleBase* LibEntryL()
    {
    return CMmfTsPlay::NewL();

    }

// -----------------------------------------------------------------------------
// SetRequirements handles test module parameters(implements evolution
// version 1 for test module's heap and stack sizes configuring).
// Returns: TInt: Symbian error code.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SetRequirements( CTestModuleParam*& /*aTestModuleParam*/,
                                TUint32& /*aParameterValid*/ )
    {

    /* --------------------------------- NOTE ---------------------------------
    USER PANICS occurs in test thread creation when:
    1) "The panic occurs when the value of the stack size is negative."
    2) "The panic occurs if the minimum heap size specified is less
       than KMinHeapSize".
       KMinHeapSize: "Functions that require a new heap to be allocated will
       either panic, or will reset the required heap size to this value if
       a smaller heap size is specified".
    3) "The panic occurs if the minimum heap size specified is greater than
       the maximum size to which the heap can grow".
    Other:
    1) Make sure that your hardware or Symbian OS is supporting given sizes.
       e.g. Hardware might support only sizes that are divisible by four.
    ------------------------------- NOTE end ------------------------------- */

    // Normally STIF uses default heap and stack sizes for test thread, see:
    // KTestThreadMinHeap, KTestThreadMinHeap and KStackSize.
    // If needed heap and stack sizes can be configured here by user. Remove
    // comments and define sizes.

/*
    aParameterValid = KStifTestModuleParameterChanged;

    CTestModuleParamVer01* param = CTestModuleParamVer01::NewL();
    // Stack size
    param->iTestThreadStackSize= 16384; // 16K stack
    // Heap sizes
    param->iTestThreadMinHeap = 4096;   // 4K heap min
    param->iTestThreadMaxHeap = 1048576;// 1M heap max

    aTestModuleParam = param;
*/
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// E32Dll is a DLL entry point function
// Returns: KErrNone: No error
// -----------------------------------------------------------------------------
//
#ifndef EKA2 // Hide Dll entry point to EKA2
GLDEF_C TInt E32Dll(
    TDllReason /*aReason*/) // Reason
    {
    return(KErrNone);

    }
#endif // EKA2

//  End of File
