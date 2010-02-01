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
* Description:   Defines the interface for implementation of the Windows DRM commands for MTP
*                An implementor should derive from the class contained in this header file and
*                supply a DLL with a single export which returns a pointer to an instance of
*                the MMtpJanusDrmIf class.  The ordinal 1 export may not be a leaving function
*                - all memory allocation should be done on the ConstructL() function call,
*                which may leave.
*
*/


#ifndef MMTPJANUSDRMIF_H
#define MMTPJANUSDRMIF_H

#include <e32std.h>


/**
 *  Interface class of Windows DRM for MTP
 *
 *  @lib MtpServer.lib
 *  @since S60 3.2
 */
class MMtpJanusDrmIf
    {
    public:
    
        /**
         * Second phase constructor. All allocation for the library should be performed at this call
         *
         * @since S60 3.2
         */
        virtual void ConstructL() = 0;
        
        /**
         * Called before the library is unloaded. All destruction and clean-up should be performed at this call
         *
         * @since S60 3.2
         */
        virtual void Close() = 0;
                
        /**
         * Gets a secure time challenge from the initiator
         *
         * @since S60 3.2
         * @param aDataPhase Descriptor to populate with the challenge data (responder->initiator)
         * @param aResponseParamOne Contains the DRM_RESULT (OUT)
         * @return Error state. Negative values are Symbian error codes, positive values are MTP error codes
         */
        virtual TInt GetSecureTimeChallenge(TDes8& aDataPhase, TUint32& aResponseParamOne) = 0;
        
        /**
         * Sends a secure time response to the responder
         *
         * @since S60 3.2
         * @param aDataPhase Descriptor containing the response data (initiator->responder)
         * @param aResponseParamOne Contains the DRM_RESULT (OUT)
         * @return Error state. Negative values are Symbian error codes, positive values are MTP error codes
         */
        virtual TInt SetSecureTimeResponse(const TDesC8& aDataPhase, TUint32& aResponseParamOne) = 0;
    
        /**
         * Sends a license response object to the responder
         *
         * @since S60 3.2
         * @param aDataPhase Descriptor containing the license response data
         * @param aResponseParamOne Contains the DRM_RESULT (OUT)
         * @return Error state. Negative values are Symbian error codes, positive values are MTP error codes
         */
        virtual TInt SetLicenseResponse(const TDesC8& aDataPhase, TUint32& aResponseParamOne) = 0;
        
        /**
         * Sends a list of items to the initiator, based on the parameters in the query
         *
         * @since S60 3.2
         * @param aDataPhase Descriptor containing the sync list requested (responder -> initiator)
         * @param aParamOne Contains the maximum remaining count value for the query (IN) and the DRM_RESULT (OUT)
         * @param aParamTwo Contains the maximum remaining song validity for the query (IN) and the next starting index (OUT)
         * @param aParamThree Contains the starting index for the data phase (IN) and the number of items processed (OUT)
         * @param aParamFour Contains the number of items that should be processed in this call (IN)
         * @return Error state. Negative values are Symbian error codes, positive values are MTP error codes
         */
        virtual TInt GetSyncList(TDes8& aDataPhase, TUint32& aParamOne, TUint32& aParamTwo, 
                                 TUint32& aParamThree, TUint32 aParamFour) = 0;
    
        /**
         * Sends a meter challenge query.
         *
         * @since S60 3.2
         * @param aDataPhase Descriptor containing the meter challenge query (initiator -> responder)
         * @param aResponseParamOne Contains the DRM_RESULT (OUT)
         * @return Error state. Negative values are Symbian error codes, positive values are MTP error codes
         */
        virtual TInt SendMeterChallengeQuery(const TDesC8& aDataPhase, TUint32& aResponseParamOne) = 0;
        
        /**
         * Gets a meter challenge from the initiator
         *
         * @since S60 3.2
         * @param aDataPhase Descriptor to populate with the challenge data (responder->initiator)
         * @param aResponseParamOne Contains the DRM_RESULT (OUT)
         * @return Error state. Negative values are Symbian error codes, positive values are MTP error codes
         */
        virtual TInt GetMeterChallenge(TDes8& aDataPhase, TUint32& aResponseParamOne) = 0;
        
        /**
         * Sends a meter response to the responder
         *
         * @since S60 3.2
         * @param aDataPhase Descriptor containing the response data (initiator->responder)
         * @param aResponseParamOne Contains the DRM_RESULT (OUT)
         * @param aMeterResponseFlags Contains the bit mask of flags to be passed to the initiator
         * @return Error state. Negative values are Symbian error codes, positive values are MTP error codes
         */
        virtual TInt SetMeterResponse(const TDesC8& aDataPhase, TUint32& aResponseParamOne, TUint32& aMeterResponseFlags) = 0;
        
        /**
         * Instructs the mobile device to clean the data store
         *
         * @since S60 3.2
         * @param aParamOne Contains the cleaning flags bitmask (IN) and the DRM_RESULT (OUT)
         * @return Error state. Negative values are Symbian error codes, positive values are MTP error codes
         */
        virtual TInt CleanDataStore(TUint32& aParamOne) = 0;

        /**
         * Gets the state of the license requested in the data phase
         *
         * @since S60 3.2
         * @param aDataPhase Descriptor containing the state query data
         * @param aParamOne Contains the DRM_RESULT (OUT)
         * @param aParamTwo Contains the category (OUT)
         * @param aParamThree Contains the DRM license state (OUT)
         * @param aParamFour Contains the DRM license state (OUT)
         * @param aParamFive Contains reserved data (OUT)
         * @return Error state. Negative values are Symbian error codes, positive values are MTP error codes
         */
        virtual TInt GetLicenseState(const TDesC8& aDataPhase, TUint32& aParamOne, TUint32& aParamTwo, 
                                     TUint32& aParamThree, TUint32& aParamFour, TUint32& aParamFive) = 0;
                                 
        /**
         * Sends a WMDRM command to the portable device for processing
         *
         * @since S60 3.2
         * @param aDataPhase Descriptor containing the command data phase (initiator -> responder)
         * @param aParamOne Contains the operation code (IN) and the DRM_RESULT (OUT)
         * @param aParamTwo Contains the first request argument (IN) and the first response result (OUT)
         * @param aParamThree Contains the second request argument (IN) and the second response result (OUT)
         * @param aParamFour Contains the third request argument (IN) and the third response result (OUT)
         * @param aParamFive Contains the fourth request argument (IN) and the fourth response result (OUT)
         * @return Error state. Negative values are Symbian error codes, positive values are MTP error codes
         */
        virtual TInt SendWMDRMCommand(const TDesC8& aDataPhase, TUint32& aParamOne, TUint32& aParamTwo, 
                                      TUint32& aParamThree, TUint32& aParamFour, TUint32& aParamFive) = 0;

        /**
         * Sends a WMDRM request to the portable device for processing
         *
         * @since S60 3.2
         * @param aDataPhase Descriptor containing the command data phase (responder -> initiator)
         * @param aParamOne Contains the operation code (IN) and the DRM_RESULT (OUT)
         * @param aParamTwo Contains the first request argument (IN) and the first response result (OUT)
         * @param aParamThree Contains the second request argument (IN) and the second response result (OUT)
         * @param aParamFour Contains the third request argument (IN) and the third response result (OUT)
         * @param aParamFive Contains the fourth request argument (IN) and the fourth response result (OUT)
         * @return Error state. Negative values are Symbian error codes, positive values are MTP error codes
         */
        virtual TInt SendWMDRMRequest(TDes8& aDataPhase, TUint32& aParamOne, TUint32& aParamTwo, 
                                      TUint32& aParamThree, TUint32& aParamFour, TUint32& aParamFive) = 0;
        
        /**
         * Get the device certificate to pass back as the device property. The descriptor returned should
         * contain the device certificate information as required by the GetDeviceProperty MTP command for
         * the MTP_PROPERTYCODE_JANUS_DEVICECERTIFICATE dataset. 
         * Ownership of the descriptor is passed back
         *
         * @since S60 3.2
         * @return A populated buffer of the device certificate dataset, or NULL in the case of an error
         */
        virtual HBufC* DeviceCertificate() = 0;

        /**
         * Get the secure time of the device.
         * Ownership of the descriptor is passed back
         *
         * @since S60 3.2
         * @return A populated buffer of the device secure time, or NULL in the case of an error
         */
//        virtual HBufC* SecureTime() = 0;
    };

#endif // MMTPJANUSDRMIF_H
