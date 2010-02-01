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
* Description: STIF testclass declaration
*
*/

#ifndef UI_EQUALIZERTEST_H
#define UI_EQUALIZERTEST_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <TestclassAssert.h>
#include <EqualizerPresetsDialog.h>
#include <mdaaudiosampleplayer.h>

#include <mdaaudiosampleeditor.h>
#include <mpxcustomcommandobserver.h>
#include "edwintestcontrol.h"



// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0


#ifdef __WINSCW__
// Logging path
_LIT( Kui_equalizertestLogPath, "\\logs\\testframework\\ui_equalizertest\\" ); 
// Log file
_LIT( Kui_equalizertestLogFile, "ui_equalizertest.txt" ); 
_LIT( Kui_equalizertestLogFileWithTitle, "ui_equalizertest_[%S].txt" );
_LIT( Kui_equalizer_TestData_Path, "c:\\testing\\data\\" );
#else
// Logging path
_LIT( Kui_equalizertestLogPath, "\\logs\\testframework\\ui_equalizertest\\" ); 
// Log file
_LIT( Kui_equalizertestLogFile, "ui_equalizertest.txt" ); 
_LIT( Kui_equalizertestLogFileWithTitle, "ui_equalizertest_[%S].txt" );
_LIT( Kui_equalizer_TestData_Path, "e:\\testing\\data\\" );
#endif



// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class Cui_equalizertest;
class CCustomCommandUtility;
class CCustomCommand;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  Cui_equalizertest test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(Cui_equalizertest) : public CScriptBase,
//									   public MMdaAudioPlayerCallback,
									   public MEqualizerPresetActivationObserver//,
//									   public MCustomInterface,
//									   public MMdaObjectStateChangeObserver,
//									   public MMPXPlaybackObserver,
//									   public MMPXCustomCommandObserver
									  
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static Cui_equalizertest* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~Cui_equalizertest();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );
    	
       
        /**
         * From MEqualizerPresetActivationObserver
         * Handle preset id activation from Equalizer dialog.
         * 
         * @param aActivatedPreset Current activated preset id.
         */
        virtual void HandlePresetActivation( TInt aActivatedPreset );
  	
    	

	    
	    /**
	     * Handle custom commands.
	     *
	     * @since S60 3.2.3
	     * @param aUid Command category Uid. Different command set will have 
	     *             different Uid to avoid clash.
	     * @param aCommand Command to be handled.
	     * @param aData data for the specific command
	     */
	     void HandleCustomCommand( 
	        const TUid& /*aUid*/,
	        const TInt /*aCommand*/, 
	        const TDesC8& aData = KNullDesC8 ){};
        
    	
    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
        
      
        
    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        Cui_equalizertest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
        virtual TInt ExampleL( CStifItemParser& aItem );
        
        // Test functions for EqualizerPresetsDialog.h
        TInt CEqualizerPresetsDialogNewL( CStifItemParser& aItem );        
        TInt ExecuteLD( CStifItemParser& aItem );
        TInt SetAudioEqualizerL( CStifItemParser& aItem );
        TInt Release( CStifItemParser& aItem );
        
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;
    	// Used to store a pointer to CAudioEqualizerUtility
        CAudioEqualizerUtility* iAudEqUtility;
        CEqualizerPresetsDialog* iEqDialog;          
        CCustomCommand* iCustomCommand;
        CEdwinTestControl* iEdwinTestControl;
        TInt iInitStatus;

    };

#endif      // UI_EQUALIZERTEST_H

// End of File
