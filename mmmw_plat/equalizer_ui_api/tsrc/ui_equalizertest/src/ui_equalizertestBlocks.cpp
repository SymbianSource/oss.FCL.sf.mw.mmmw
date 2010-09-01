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
* Description: This file contains testclass implementation.
*
*/

// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "ui_equalizertest.h"

#include <CustomCommandUtility.h>


    class CCustomCommand : public CCustomCommandUtility
    	{
    	public:
    	CCustomCommand()
			{

			}
    	
    	~CCustomCommand()
    		{
    		
    		}
    	
    	TInt CustomCommandSync(const TMMFMessageDestinationPckg&  /*aDestination*/,
    	                                     TInt                         aFunction,
    	                               const TDesC8&                      /*aDataTo1*/,
    	                               const TDesC8&                      /*aDataTo2*/,
    	                               TDes8&                       aDataFrom)
    		{
    		const TUid KUidCustomInterfaceBuilder = {0x10200017};
    		const TUid KUidAudioEqualizerEffect = { 0x1020382A };
    		if( aFunction == 3 )
    			{
				TPckgBuf<TUint8> buf( 1 );
				aDataFrom = buf;
    			}
    		//if( aFunction == 1 || aDestination().Interface() == KUidCustomInterfaceBuilder )
    		else
    			{
				TMMFMessageDestination     builderHandle(KUidCustomInterfaceBuilder);
				TMMFMessageDestinationPckg builderHandlePckg( builderHandle );
				aDataFrom = builderHandlePckg;
    			}
    		return 0; 
			}

    	TInt CustomCommandSync(const TMMFMessageDestinationPckg&  /*aDestination*/,
    	                                     TInt                         /*aFunction*/,
    	                               const TDesC8&                      /*aDataTo1*/,
    	                               const TDesC8&                      /*aDataTo2*/)
    		{
    		return 0; 
    		}
    	
    	void CustomCommandAsync(const TMMFMessageDestinationPckg& /*aDestination*/,
    	                                      TInt                        /*aFunction*/,
    	                                const TDesC8&                     /*aDataTo1*/,
    	                                const TDesC8&                     /*aDataTo2*/,
    	                                      TDes8&                      /*aDataFrom*/,
    	                                      TRequestStatus&             /*aStatus*/)
    		{
    		
    		}
    	
    	void CustomCommandAsync(const TMMFMessageDestinationPckg& /*aDestination*/,
    	                                      TInt                        /*aFunction*/,
    	                                const TDesC8&                     /*aDataTo1*/,
    	                                const TDesC8&                     /*aDataTo2*/,
    	                                      TRequestStatus&             /*aStatus*/)
    		{
    		
    		}
    	};








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
// Cui_equalizertest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//


void Cui_equalizertest::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Cui_equalizertest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cui_equalizertest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", Cui_equalizertest::ExampleL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove
        // Test functions for EqualizerPresetsDialog.h
        ENTRY( "CEqualizerPresetsDialogNewL", Cui_equalizertest::CEqualizerPresetsDialogNewL ),
        ENTRY( "ExecuteLD", Cui_equalizertest::ExecuteLD ),
        ENTRY( "SetAudioEqualizerL", Cui_equalizertest::SetAudioEqualizerL ),
        ENTRY( "Release", Cui_equalizertest::Release ),
        
              
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }



// ---------------------------------------------------------------------------
// From MEqualizerPresetActivationObserver
// Handle preset id activation from Equalizer dialog.
// ---------------------------------------------------------------------------
//
void Cui_equalizertest::HandlePresetActivation(
    TInt aActivatedPreset )
    {
    iLog->Log(_L("Cui_equalizertest::HandlePresetActivation :-> aActivatedPreset[%d]"), aActivatedPreset );
    }

// -----------------------------------------------------------------------------
// Cui_equalizertest::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cui_equalizertest::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( Kui_equalizertest, "ui_equalizertest" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, Kui_equalizertest, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, Kui_equalizertest, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }


// ---------------------------------------------------------------------------
// Cui_equalizertest::CEqualizerPresetsDialogNewL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cui_equalizertest::CEqualizerPresetsDialogNewL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cui_equalizertest::CEqualizerPresetsDialogNewL"));
    TInt err = KErrNone;  
    TInt presetId = 0;


    iCustomCommand = new (ELeave) CCustomCommand;
    
    iAudEqUtility = CAudioEqualizerUtility::NewL( iCustomCommand );

    //iEqDialog = CEqualizerPresetsDialog::NewL( iAudEqUtility, presetId, *this );
    
    TRAP( err , iEqDialog = CEqualizerPresetsDialog::NewL( iAudEqUtility, presetId, *this ) ); 
    
    if ( err == KErrNone )
    	{    	
    	iLog->Log(_L("CEqualizerPresetsDialogNewL return with no error!"));   		
    	}
    else
    	{
    	iLog->Log(_L("ERROR:CEqualizerPresetsDialogNewL fails!%d returns!"),err);
    	}
    return err;
    }

// ---------------------------------------------------------------------------
// Cui_equalizertest::ExecuteLD
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cui_equalizertest::ExecuteLD( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cui_equalizertest::ExecuteLD"));
    TInt err = KErrNone;    
    TInt ret = 0;
    
    iEdwinTestControl = CEdwinTestControl::NewL();
    CleanupStack::PushL( iEdwinTestControl );
    CCoeEnv::Static()->AppUi()->AddToStackL( iEdwinTestControl );
    iEdwinTestControl->MakeVisible(ETrue);
    CleanupStack::Pop( iEdwinTestControl );
    
    //TRAP_IGNORE( ret = iEqDialog->ExecuteLD() );
    //TRAP( err,ret = iEqDialog->ExecuteLD() );
    ret = iEqDialog->ExecuteLD();
    
    //iEqDialog->OfferKeyEventL( ret );
    
    if ( err == KErrNone )
    	{    	
    	iLog->Log(_L("ExecuteLD return with no error!"));   	
    	iLog->Log(_L("ExecuteLD() return [%d]!"),ret); 
    	}
    else
    	{
    	iLog->Log(_L("ERROR:ExecuteLD fails!%d returns!"),err);
    	}
    return err;
    }

// ---------------------------------------------------------------------------
// Cui_equalizertest::SetAudioEqualizerL
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cui_equalizertest::SetAudioEqualizerL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cui_equalizertest::SetAudioEqualizerL"));
    TInt err = KErrNone;    
    if( iAudEqUtility )
    	{
        //iEqDialog->SetAudioEqualizerL( iAudEqUtility );
        TRAP( err,iEqDialog->SetAudioEqualizerL( iAudEqUtility ) );
        if ( err == KErrNone )
        	{    	
        	iLog->Log(_L("SetAudioEqualizerL return with no error!"));   		
        	}
        else
        	{
        	iLog->Log(_L("ERROR:SetAudioEqualizerL fails!%d returns!"),err);
        	}
    	}
    else
    	{
    	iLog->Log(_L("iAudEqUtility is NULL, can not call SetAudioEqualizerL(). User leave here."));
    	User::Leave( 1000 );
    	}
    return err;
    }

// ---------------------------------------------------------------------------
// Cui_equalizertest::Release
// ?implementation_description
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt Cui_equalizertest::Release( CStifItemParser& /*aItem*/ )
    {
    iLog->Log(_L("Cui_equalizertest::Release"));
    TInt err = KErrNone; 
    if( iEqDialog )
    	{
    	delete iEqDialog;
    	}
    if( iAudEqUtility )
    	{
    	delete iAudEqUtility;
    	}
    if( iEdwinTestControl )
    	{
    	delete iEdwinTestControl;
    	}
    return err;
    }

// -----------------------------------------------------------------------------
// Cui_equalizertest::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt Cui_equalizertest::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
