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
 * Description:
 * This file defines the API for System Tone Service which is
 * implemented in the systemtoneservice.dll.  This API uses
 * the private implementation pattern to help improve the BC
 * of the API by decoupling the implementation from the
 * interface. 
 *
 */

#ifndef __SYSTEMTONESERVICE_H__
#define __SYSTEMTONESERVICE_H__

// System includes
#include <e32base.h>

// Forward declarations
NONSHARABLE_CLASS( CStsImplementation);

// Class declaration
/**
 *  System Tone Service API definition.
 *  This is the native C++ API for applications and middleware components
 *  to play standard system tones.
 *
 *  @code
 *   CSystemToneService* sts = CSystemToneService::Create();
 *   TInt calendarAlarmContext;
 *   ...
 *   if (sts)
 *   {
 *       ...
 *       sts->PlayTone(CSystemToneService::EWarningTone);
 *       ...
 *       sts->Playtone(CSystemToneService::ECalendarAlarm, &calendarAlarmContext);
 *       ...
 *   }
 *   ...
 *   sts->StopTone(calendarAlarmContext);
 *   ...
 *   CSystemToneService::Delete(sts);
 *  @endcode
 *
 */
NONSHARABLE_CLASS(CSystemToneService) : public CBase
    {
public:
    //** Constructor - returns NULL if construction fails */
    IMPORT_C static CSystemToneService* Create();

    //** Destructor */
    IMPORT_C static void Delete(CSystemToneService* aSystemToneService);

public:
    // Data types
    /** The type of System Tones that are supported by this API. */
    enum TToneType
        {
        // Calendar Tones
        ECalendarAlarm = 0x0001,
        EClockAlarm = 0x0002,
        EToDoAlarm = 0x0003,

        // Capture Tones
        EBurstMode = 0x1001,
        ECapture = 0x1002,
        ECallRecording = 0x1003,
        ERecordingStart = 0x1004,
        ERecordingStop = 0x1005,
        ESelfTimer = 0x1006,

        // General Tones
        EConfirmationBeep = 0x2001,
        EDefaultBeep = 0x2002,
        EErrorBeep = 0x2003,
        EInformationBeep = 0x2004,
        EWarningBeep = 0x2005,
        EIntegratedHandsFreeActivated = 0x2006,

        // Key Tones
        ETouchScreen = 0x3001,

        // Location Tones
        ELocationRequest = 0x4001,

        // Messaging Tones
        EChatAlert = 0x5001,
        EEmailAlert = 0x5002,
        EMmsAlert = 0x5003,
        ESmsAlert = 0x5004,
        EDeliveryReport = 0x5005,
        EMessageSendFailure = 0x5006,

        // Power Tones
        EBatteryLow = 0x6001,
        EBatteryRecharged = 0x6002,
        EPowerOn = 0x6003,
        EPowerOff = 0x6004,
        EWakeUp = 0x6005,
        EWrongCharger = 0x6006,

        // Telephony Tones
        EIncomingCall = 0x7001,
        EIncomingCallLine2 = 0x7002,
        EIncomingDataCall = 0x7003,
        EAutomaticRedialComplete = 0x7004,

        // Voice Recognition Tones
        EVoiceStart = 0x8001,
        EVoiceError = 0x8002,
        EVoiceAbort = 0x8003
        };

    /**
     * Plays the specified tone.  If the tone type is not recognized a default tone will
     * be played.  This method is for fixed duration tones that are expected 
     * to play to completion and do not need to be stopped by the client.
     *
     * @param aTone An input parameter that indicates the type of tone to play.
     * @return description
     */
    IMPORT_C void PlayTone(TToneType aTone);

    /**
     * Plays the specified tone.  If the tone type is not recognized a default tone will
     * be played.  This method is used for tones that may not be fixed duration such as
     * infinitely looping tones, or for tones that can be manually stopped by the client.
     *
     * @param aTone An input parameter that indicates the type of tone to play.
     * @param aPlayToneContext An output parameter that provides back a unique context to
     *  the client for this tone play that can be used for stopping the playback.
     * @return description
     */
    IMPORT_C void PlayTone(TToneType aTone, unsigned int& aPlayToneContext);

    /**
     * Stops the specified tone playback.  If the playback has already completed or the
     * context is not recognized, this method does nothing.
     *
     * @param aPlayToneContext The context to the Tone Playing that is to be stopped.
     * @return description
     */
    IMPORT_C void StopTone(unsigned int aPlayToneContext);

protected:
    // Protected constructors and destructors
    CSystemToneService(CStsImplementation& aImplementation);
    ~CSystemToneService();

protected:
    // Protected data
    /**
     * A reference to the implementation class for this API.
     */
    CStsImplementation& iImplementation;
    };

#endif  // __SYSTEMTONESERVICE_H__
