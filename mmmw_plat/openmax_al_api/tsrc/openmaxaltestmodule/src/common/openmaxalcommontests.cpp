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

TInt COpenMAXALTestModule::al_SetDataSink( CStifItemParser& aItem )
    {
    TInt status(KErrNone);
    TInt type;
    TInt formattype(0);

    TInt sinktype(0);
    XADataSink commonSink = {NULL,NULL};
    
    status = aItem.GetNextInt(type);
    if(!status)
        {
        switch(type)
            {
            case XA_DATALOCATOR_URI:
                {
                commonSink.pLocator = &m_Uri;
                }
                break;
            case XA_DATALOCATOR_ADDRESS:
            case XA_DATALOCATOR_IODEVICE:
                {
                commonSink.pLocator = &m_IODevice;
                }
            case XA_DATALOCATOR_OUTPUTMIX:
            case XA_DATALOCATOR_NATIVEDISPLAY:
                break;
            default:
                status = KErrGeneral;
                break;
            }
        }

    if(!status)
        {
        status = aItem.GetNextInt(formattype);
        
        switch(formattype)
            {
            case XA_DATAFORMAT_MIME:
                {
                commonSink.pFormat = &m_Mime;
                }
                break;
            case XA_DATAFORMAT_PCM:
            case XA_DATAFORMAT_RAWIMAGE:
                break;
            default:
                status = KErrGeneral;
                break;
            }

        }

    if(!status)
         {
         status = aItem.GetNextInt(sinktype);
         if(!status)
             {
             switch(sinktype)
                 {
                 case 1:
                     m_AudioSink = commonSink;
                     break;
                 case 2:
                     m_VideoSink = commonSink;
                     break;
                 case 3:
                     m_LEDSink = commonSink;
                     break;
                 case 4:
                     m_VibraSink = commonSink;
                     break;
                 case 5:
                     m_DataSink = commonSink;
                     break;
                 default:
                     status = KErrGeneral;
                     break;
                 }
             }
         }    
    return status;
    }

TInt COpenMAXALTestModule::al_SetDataSource( CStifItemParser& aItem )
    {
    TInt status(KErrNone);
    TInt type;
    TInt sourcetype(0);
    TInt formattype(0);

    XADataSource commonSource = {NULL,NULL};
    status = aItem.GetNextInt(type);
    if(!status)
        {
        switch(type)
            {
            case XA_DATALOCATOR_URI:
                {
                commonSource.pLocator = &m_Uri;
                }                
                break;
            case XA_DATALOCATOR_ADDRESS:
            case XA_DATALOCATOR_IODEVICE:
                {
                commonSource.pLocator = &m_IODevice;
                }
            case XA_DATALOCATOR_OUTPUTMIX:
            case XA_DATALOCATOR_NATIVEDISPLAY:
                break;
            default:
                status = KErrGeneral;
                break;
            }
        }

    if(!status)
        {
        status = aItem.GetNextInt(formattype);
        if(!status)
            {
            switch(formattype)
                {
                case XA_DATAFORMAT_MIME:
                    {
                    commonSource.pFormat = (void*) &m_Mime;
                    }
                    break;
                case XA_DATAFORMAT_PCM:
                case XA_DATAFORMAT_RAWIMAGE:
                    break;
                default:
                    status = KErrGeneral;
                    break;
                }
            }
        }
    
    
    if(!status)
        {
        status = aItem.GetNextInt(sourcetype);
        if(!status)
            {
            switch(sourcetype)
                {
                case 1:
                    m_AudioSource = commonSource;
                    break;
                case 2:
                    m_VideoSource = commonSource;
                    break;
                case 3:
                    m_MOMetadataExtractorSource = commonSource;
                    break;
                default:
                    status = KErrGeneral;
                    break;
                }
            }
        }
    
    return status;  
    }

TInt COpenMAXALTestModule::al_SetDataLocator( CStifItemParser& aItem )
    {
    TInt status(KErrNone);
    TInt type;

    status = aItem.GetNextInt(type);
    
    switch(type)
        {
        case XA_DATALOCATOR_URI:
            {
            TPtrC uri;
            if(m_URIName)
                {
                delete m_URIName;
                m_URIName = NULL;
                }
            //status = aItem.SetParsingType(CStifItemParser::EQuoteStyleParsing);
            status = aItem.GetNextString(uri);
            if(!status)
                {
                m_URIName = HBufC8::NewL(uri.Length()+1); 
                TPtr8 desc = m_URIName->Des();
                desc.Copy(uri);
                m_Uri.locatorType = XA_DATALOCATOR_URI;
                m_Uri.URI = (XAchar*) desc.PtrZ();
                }
            else
                {
                status = KErrGeneral;
                }
            }
            break;
        case XA_DATALOCATOR_IODEVICE:
            {
            TInt devicetype;
            TUint deviceId;
            
            status = aItem.GetNextInt(devicetype);
            if(!status)
                {
                status = aItem.GetNextInt(deviceId);
                if(!status)
                    {
                    m_IODevice.deviceID = deviceId;
                    m_IODevice.deviceType = devicetype;
                    m_IODevice.locatorType = XA_DATALOCATOR_IODEVICE;
                    }
                else
                    {
                    status = KErrGeneral;
                    }
                }
            else
                {
                status = KErrGeneral;
                }
            }
            break;
        case XA_DATALOCATOR_OUTPUTMIX:
        case XA_DATALOCATOR_NATIVEDISPLAY:
        case XA_DATALOCATOR_ADDRESS:
            break;
        default:
            status = KErrGeneral;
            break;
        }
    return status;   
    }

TInt COpenMAXALTestModule::al_SetDataFormat( CStifItemParser& aItem )
    {
    TInt status(KErrNone);
    TInt type;
    TInt contType(0);
    TPtrC mimetype;
    status = aItem.GetNextInt(type);
    
    switch(type)
        {
        case XA_DATAFORMAT_MIME:
            {
            status = aItem.GetNextString(mimetype);
            if(!status)
                {
                status = aItem.GetNextInt(contType);
                if(!status)
                    {
                    if(m_MimeType)
                        {
                        delete m_MimeType;
                        m_MimeType = NULL;
                        }
                    m_MimeType = HBufC8::NewL(mimetype.Length()+1); 
                    TPtr8 desc = m_MimeType->Des();
                    desc.Copy(mimetype);
                    m_Mime.formatType = XA_DATAFORMAT_MIME;
                    m_Mime.mimeType = (XAchar*) desc.PtrZ();
                    m_Mime.containerType = contType;
                    }
                else
                    {
                    status = KErrGeneral;
                    }
                }
            else
                {
                status = KErrGeneral;
                }
            }
            break;
        case XA_DATAFORMAT_PCM:
        case XA_DATAFORMAT_RAWIMAGE:
            break;
        default:
            status = KErrGeneral;
            break;
        }
    return status;   
    }

