/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
#include <StifTestInterface.h>
#include <systemtoneservice.h>
#include "systemtoneservicestif.h"



TInt CSystemToneServiceStif::RunMethodL(CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "Create", CSystemToneServiceStif::CreateSystemToneService ),
        ENTRY( "Delete", CSystemToneServiceStif::DeleteSystemToneService ),
        ENTRY( "PlayTone", CSystemToneServiceStif::PlaySystemToneService ),
        ENTRY( "PlayToneWithContext", CSystemToneServiceStif::PlaySystemToneServiceWithContext ),
        ENTRY( "StopTone", CSystemToneServiceStif::StopSystemToneService ),
        


        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }





void CSystemToneServiceStif::Delete()
    {

    }



// -----------------------------------------------------------------------------
// CSystemToneServiceStif::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSystemToneServiceStif::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( Ksystemtoneservicestif, "systemtoneservicestif" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, Ksystemtoneservicestif, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, Ksystemtoneservicestif, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }

TInt CSystemToneServiceStif::CreateSystemToneService(  )
    {
    // Print to UI
        _LIT( Ksystemtoneservicestif, "systemtoneservicestif" );
        _LIT( KPrint, "In CreateSystemToneService" );
        TestModuleIf().Printf( 0, Ksystemtoneservicestif, KPrint );
        // Print to log file
        iLog->Log( KPrint );
        
        iSts = CSystemToneService::Create();    
        
        iLog->Log(_L("CreateSystemToneService complete"));
        return KErrNone;
    }

TInt CSystemToneServiceStif::DeleteSystemToneService( )
    {
    // Print to UI
        _LIT( Ksystemtoneservicestif, "systemtoneservicestif" );
        _LIT( KPrint, "In DeleteSystemToneService" );
        TestModuleIf().Printf( 0, Ksystemtoneservicestif, KPrint );
        // Print to log file
        iLog->Log( KPrint );
        
        CSystemToneService::Delete(iSts);  
        return KErrNone;
    }
    
    
TInt CSystemToneServiceStif::PlaySystemToneService( CStifItemParser& aItem )
    {
        // Print to UI
            _LIT( Ksystemtoneservicestif, "systemtoneservicestif" );
            _LIT( KPrint, "In PlaySystemToneService" );
            TestModuleIf().Printf( 0, Ksystemtoneservicestif, KPrint );
            // Print to log file
            iLog->Log( KPrint );
            
               TInt lRetVal = KErrNone;
               TInt toneType = 0;

               lRetVal = aItem.GetNextInt(toneType);
              
        if ( lRetVal != KErrNone )
        {
            iLog->Log(_L("Csystemtoneservicestif::PlaySystemToneService tone type missing in config file "));
            iLog->Log(_L("Playing Default Tone"));
            iSts->PlayTone(CSystemToneService::EDefaultBeep);
            lRetVal = KErrNone;
        }
				else
        {
            iSts->PlayTone(CSystemToneService::TToneType(toneType));
            iLog->Log(_L("CSystemToneService::TToneType(toneType) %d"),CSystemToneService::TToneType(toneType) );
        }
        
        return lRetVal;
}


TInt CSystemToneServiceStif::PlaySystemToneServiceWithContext( CStifItemParser& aItem )
    {
        // Print to UI
            _LIT( Ksystemtoneservicestif, "systemtoneservicestif" );
            _LIT( KPrint, "In PlaySystemToneServiceWithContext" );
            TestModuleIf().Printf( 0, Ksystemtoneservicestif, KPrint );
            // Print to log file
            iLog->Log( KPrint );
            
               TInt lRetVal = KErrNone;
               TInt toneType = 0;

               lRetVal = aItem.GetNextInt(toneType);

               if ( lRetVal != KErrNone )
                {
                   iLog->Log(_L("CSystemToneServiceStif::PlaySystemToneService tone type missing in config file "));
                   iLog->Log(_L("Playing Default Tone"));
                   
            iSts->PlayTone(CSystemToneService::EDefaultBeep, iCurrentContext);
        }
			  else
        {
            //iSts->PlayTone(CSystemToneService::EClockAlarm, iCurrentContext);
            iSts->PlayTone(CSystemToneService::TToneType(toneType), iCurrentContext);
            iLog->Log(_L("CSystemToneService::TToneType(toneType) %d"),CSystemToneService::TToneType(toneType) );
        }
        
        return lRetVal;       
            
        }


TInt  CSystemToneServiceStif::StopSystemToneService(CStifItemParser& aItem  )
    {
    TPtrC StopType;
		TInt error = KErrNone;
		
    error = aItem.GetNextString(StopType);
    
    
    if (StopType == KTrue)
    {
				iLog->Log(_L("Stop Tone with recognized context number"));
          
        iSts->StopTone(iCurrentContext);
		}
		else
		{
		    iLog->Log(_L("Stop Tone with unrecognized context number"));
          
        iSts->StopTone(iCurrentContext+1);
    }
    
    return error;
}

