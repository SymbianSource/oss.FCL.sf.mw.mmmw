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
* Description:  Class definition for the ProgressiveDownloadUtility functions.
*
*/

#ifndef __CPROGRESSIVEDOWNLOADUTILITY_H__
#define __CPROGRESSIVEDOWNLOADUTILITY_H__

#include <e32std.h>
#include <e32base.h>
#include <f32file.h>


//Forward Declaration
class MAudioPdPlayUtility;
class MAudioPdPlayUtilityCallback;
class CPdProperties;


// CLASS DEFINITIONS

/**
*  This class provides Progressive download utility functions.
*
*  @lib ProgressiveDownloadUtility.lib
*  @since 3.0
*/
class CProgressiveDownloadUtility: public CBase 
	{

public:
    /**
    * Two-phased constructor.
    */
	IMPORT_C static CProgressiveDownloadUtility* NewL();

	/**
    * Destructor.
    */
	IMPORT_C virtual ~CProgressiveDownloadUtility();

    /**
    * From CProgressiveDownloadUtility
    * @param aFileName
	* @param aCallback
	* @return MAudioPdPlayUtility*
	*/	
	IMPORT_C MAudioPdPlayUtility* OpenL(const TDesC& aFileName,MAudioPdPlayUtilityCallback& aCallback);

    /**
    * From CProgressiveDownloadUtility
    * @param aFileHandle
	* @param aCallback
	* @return MAudioPdPlayUtility*
	*/			
	IMPORT_C MAudioPdPlayUtility* OpenL(const RFile& aFileHandle,MAudioPdPlayUtilityCallback& aCallback);
	
protected:

	
private:
	/**
    * C++ default constructor.
    */
	CProgressiveDownloadUtility();

	/**
    * By default Symbian 2nd phase constructor is private.
    */
	void ConstructL();

	CPdProperties* iProperties;
   
	}; 

#endif
