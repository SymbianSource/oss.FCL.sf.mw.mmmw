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
 * Description: Telephony Multimedia Service
 *
 */

#ifndef MCCPDTMFOBSERVER_H
#define MCCPDTMFOBSERVER_H

#include <e32base.h>

namespace TMS {

/**
 * CCP DTMF related events observer class.
 * This class is used for notifying DTMF specific events.
 */
class TMSCCPDTMFObserver
    {
public:

    /** DTMF Events */
    enum TCCPDtmfEvent
        {
        /** Unknown */
        ECCPDtmfUnknown = 0,
        /** DTMF sending started manually */
        ECCPDtmfManualStart = 5,
        /** DTMF sending stopped manually */
        ECCPDtmfManualStop = 6,
        /** DTMF sending aborted manually */
        ECCPDtmfManualAbort = 7,
        /** Automatic DTMF sending initialized */
        ECCPDtmfSequenceStart = 9,
        /** Automatic DTMF sending stopped */
        ECCPDtmfSequenceStop = 10,
        /** Automatic DTMF sending aborted */
        ECCPDtmfSequenceAbort = 11,
        /** There was stop mark in DTMF string */
        ECCPDtmfStopInDtmfString,
        /** DTMF sending completed succesfully */
        ECCPDtmfStringSendingCompleted
        };

    //protected:
    /**
     * Protects the observer being deleted through the observer interface.
     */
    //virtual inline ~TMSCCPDTMFObserver() {};

public:

    /**
     * HandleDTMFEvents.
     * @param aEvent Event type
     * @param aError Error code
     * @param aTone Character
     * @return none
     */
    virtual void HandleDTMFEvent(const TMSCCPDTMFObserver::TCCPDtmfEvent aEvent,
            const TInt aError, const TChar aTone) /*const*/ = 0;
    };

} //namespace TMS

#endif // MCCPDTMFOBSERVER_H

// End of File
