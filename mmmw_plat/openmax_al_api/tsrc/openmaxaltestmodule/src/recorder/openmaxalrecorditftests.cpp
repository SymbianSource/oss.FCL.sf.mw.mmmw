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
void RecordItfCallback (
    XARecordItf caller,
    void * pContext,
    XAuint32 event
);
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

TInt COpenMAXALTestModule::al_recorditf_SetRecordState( CStifItemParser& aItem )
    {
    TInt status(KErrNone);
    TInt state(0);
    XAresult res;
    status = aItem.GetNextInt(state);
    if(!status)
        {
        if(m_RecordItf)
            {
            res = (*m_RecordItf)->SetRecordState(
                    m_RecordItf, state);              
            status = res;
            }
        else
            {
            status = KErrNotFound;
            }    
        }

    return status;
    }

TInt COpenMAXALTestModule::al_recorditf_GetRecordState( CStifItemParser& /*aItem*/ )
    {
    TInt status(KErrNone);
    XAresult res;
    XAuint32 state;
    if(m_RecordItf)
        {
        res = (*m_RecordItf)->GetRecordState(
                m_RecordItf, &state);              
        status = res;
        }
    else
        {
        status = KErrNotFound;
        }    

    return status;
    }

TInt COpenMAXALTestModule::al_recorditf_SetDurationLimit( CStifItemParser& aItem )
    {
    TInt status(KErrNone);
    TUint duration(0);
    XAresult res;
    status = aItem.GetNextInt(duration);
    if(!status)
        {
        if(m_RecordItf)
            {
            res = (*m_RecordItf)->SetDurationLimit(
                    m_RecordItf, duration);              
            status = res;
            }
        else
            {
            status = KErrNotFound;
            }    
        }

    return status;
    }

TInt COpenMAXALTestModule::al_recorditf_GetPosition( CStifItemParser& /*aItem*/ )
    {
    TInt status(KErrNone);
    XAresult res;
    XAmillisecond pos;
    if(m_RecordItf)
        {
        res = (*m_RecordItf)->GetPosition(
                m_RecordItf, &pos);              
        status = res;
        }
    else
        {
        status = KErrNotFound;
        }    

    return status;
    }

TInt COpenMAXALTestModule::al_recorditf_RegisterCallback( CStifItemParser& /*aItem*/ )
    {
    TInt status(KErrNone);
    XAresult res;

    if(m_RecordItf)
        {
        res = (*m_RecordItf)->RegisterCallback(
                m_RecordItf, &RecordItfCallback, (void*)this);              
        status = res;
        }
    else
        {
        status = KErrNotFound;
        }    

    return status;
    }

TInt COpenMAXALTestModule::al_recorditf_SetCallbackEventsMask( CStifItemParser& aItem )
    {
    TInt status(KErrNone);
    TUint flags(0);
    XAresult res;
    status = aItem.GetNextInt(flags);
    if(!status)
        {
        if(m_RecordItf)
            {
            res = (*m_RecordItf)->SetCallbackEventsMask(
                    m_RecordItf, flags);              
            status = res;
            }
        else
            {
            status = KErrNotFound;
            }    
        }

    return status;
    }

TInt COpenMAXALTestModule::al_recorditf_GetCallbackEventsMask( CStifItemParser& /*aItem*/ )
    {
    TInt status(KErrNone);
    XAresult res;
    XAuint32 flags;
    if(m_RecordItf)
        {
        res = (*m_RecordItf)->GetCallbackEventsMask(
                m_RecordItf, &flags);              
        status = res;
        }
    else
        {
        status = KErrNotFound;
        }    

    return status;
    }

TInt COpenMAXALTestModule::al_recorditf_SetMarkerPosition( CStifItemParser& aItem )
    {
    TInt status(KErrNone);
    TUint pos(0);
    XAresult res;
    status = aItem.GetNextInt(pos);
    if(!status)
        {
        if(m_RecordItf)
            {
            res = (*m_RecordItf)->SetMarkerPosition(
                    m_RecordItf, pos);              
            status = res;
            }
        else
            {
            status = KErrNotFound;
            }    
        }

    return status;
    }

TInt COpenMAXALTestModule::al_recorditf_ClearMarkerPosition( CStifItemParser& /*aItem*/ )
    {
    TInt status(KErrNone);
    XAresult res;

    if(m_RecordItf)
        {
        res = (*m_RecordItf)->ClearMarkerPosition(m_RecordItf);              
        status = res;
        }
    else
        {
        status = KErrNotFound;
        }    

    return status;
    }

TInt COpenMAXALTestModule::al_recorditf_GetMarkerPosition( CStifItemParser& /*aItem*/ )
    {
    TInt status(KErrNone);
    XAresult res;
    XAmillisecond pos;
    if(m_RecordItf)
        {
        res = (*m_RecordItf)->GetMarkerPosition(
                m_RecordItf, &pos);              
        status = res;
        }
    else
        {
        status = KErrNotFound;
        }    

    return status;
    }

TInt COpenMAXALTestModule::al_recorditf_SetPositionUpdatePeriod( CStifItemParser& aItem )
    {
    TInt status(KErrNone);
    TUint updatePeriod(0);
    XAresult res;
    status = aItem.GetNextInt(updatePeriod);
    if(!status)
        {
        if(m_RecordItf)
            {
            res = (*m_RecordItf)->SetPositionUpdatePeriod(
                    m_RecordItf, updatePeriod);              
            status = res;
            }
        else
            {
            status = KErrNotFound;
            }    
        }

    return status;
    }

TInt COpenMAXALTestModule::al_recorditf_GetPositionUpdatePeriod( CStifItemParser& /*aItem*/ )
    {
    TInt status(KErrNone);
    XAresult res;
    XAmillisecond pos;
    if(m_RecordItf)
        {
        res = (*m_RecordItf)->GetPositionUpdatePeriod(
                m_RecordItf, &pos);              
        status = res;
        }
    else
        {
        status = KErrNotFound;
        }    

    return status;
    }

