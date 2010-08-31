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
#include <StifTestInterface.h>
#include "openmaxaltestmodule.h"

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

TInt COpenMAXALTestModule::al_audioencodercapitf_GetAudioEncoders( CStifItemParser& aItem )
    {
    TInt status(KErrNone);
    TInt numEncoders(0);
    XAuint32 deviceIdArr[4];
    XAresult res;
    XAuint32 numEnc(0);
    
    status = aItem.GetNextInt(numEncoders);
    if(!status)
        {
        numEnc = numEncoders;
        if(m_AEncCapItf)
            {
            res = (*m_AEncCapItf)->GetAudioEncoders(
                    m_AEncCapItf, &numEnc, deviceIdArr);
            status = res;
            }
        else
            {
            status = KErrNotFound;
            }            
        }
    else
        {
        if(m_AEncCapItf)
            {
            res = (*m_AEncCapItf)->GetAudioEncoders(
                    m_AEncCapItf, &numEnc, NULL);
            status = res;
            }
        else
            {
            status = KErrNotFound;
            }            
        }
    return status;
    }

TInt COpenMAXALTestModule::al_audioencodercapitf_GetAudioEncoderCapabilities( CStifItemParser& aItem )
    {
    TInt status(KErrNone);
    TInt encId(0);
    XAuint32 encoderid(0);
    XAuint32 index(0);
    XAAudioCodecDescriptor desc;
    XAresult res;
    
    status = aItem.GetNextInt(encId);
    if(!status)
        {
        encoderid = encId;
        if(m_AEncCapItf)
            {
            res = (*m_AEncCapItf)->GetAudioEncoderCapabilities(
                    m_AEncCapItf, encoderid, &index, &desc);
            status = res;
            }
        else
            {
            status = KErrNotFound;
            }                
        }
    return status;
    }
