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

#ifndef SYSTEMTONESERVICE_H_
#define SYSTEMTONESERVICE_H_

// System includes
#include <e32base.h>

// Forward declarations
NONSHARABLE_CLASS( CStsImplementation);

//Observer declaration
/**
 *  Play Alarm Observer API definition.
 *  This defines the interface that must be implemented by Play Alarm
 *  Observers.
 *
 */
class MStsPlayAlarmObserver
    {
public:
    /**
     * Informs the observer when the play alarm as indicated by the alarm
     * context is complete.  This method will not be called when this alarm
     * associated with the alarm context has been stopped with the StopAlarm
     * method.
     *
     * @param aAlarmContext The context of the alarm that has completed.
     */
    virtual void PlayAlarmComplete(unsigned int aAlarmContext) = 0;
    };

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
 *       sts->PlayAlarm(CSystemToneService::ECalendarAlarm, &calendarAlarmContext);
 *       ...
 *       sts->StopAlarm(calendarAlarmContext);
 *       ...
 *   }
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
        // Capture Tones
        EBurstMode = 0x0001,
        ECapture = 0x0002,
        ECallRecording = 0x0003,
        ERecordingStart = 0x0004,
        ERecordingStop = 0x0005,
        ESelfTimer = 0x0006,

        // General Tones
        EConfirmationBeep = 0x0101,
        EDefaultBeep = 0x0102,
        EErrorBeep = 0x0103,
        EInformationBeep = 0x0104,
        EWarningBeep = 0x0105,
        EIntegratedHandsFreeActivated = 0x0106,

        // Key Tones
        ETouchScreen = 0x0201,

        // Location Tones
        ELocationRequest = 0x0301,

        // Messaging Tones
        EChatAlert = 0x0401,
        EEmailAlert = 0x0402,
        EMmsAlert = 0x0403,
        ESmsAlert = 0x0404,
        EDeliveryReport = 0x0405,
        EMessageSendFailure = 0x0406,

        // Power Tones
        EBatteryLow = 0x0501,
        EBatteryRecharged = 0x0502,
        EPowerOn = 0x0503,
        EPowerOff = 0x0504,
        EWakeUp = 0x0505,
        EWrongCharger = 0x0506,

        // Telephony Tones
        EAutomaticRedialComplete = 0x0604,

        // Voice Recognition Tones
        EVoiceStart = 0x0701,
        EVoiceError = 0x0702,
        EVoiceAbort = 0x0703
        };

    /** The type of System Alarms that are supported by this API. */
    enum TAlarmType
        {
        // Calendar Alarms
        ECalendarAlarm = 0x8001,
        EClockAlarm = 0x8002,
        EToDoAlarm = 0x8003,

        // Telephony Alarms
        EIncomingCall = 0x8101,
        EIncomingCallLine2 = 0x8102,
        EIncomingDataCall = 0x8103
        };

    /**
     * Plays the specified tone.  If the tone type is not recognized a default tone will
     * be played.  This method is for fixed duration tones that are expected 
     * to play to completion and do not need to be stopped by the client.
     *
     * @param aTone An input parameter that indicates the type of tone to play.
     */
    IMPORT_C void PlayTone(TToneType aTone);

    /**
     * Plays the specified alarm.  If the alarm type is not recognized a default alarm will
     * be played.  Alarms are tones that are not fixed duration such as a calendar alarm
     * that can be manually stopped by the client, or the client needs to know when the alarm
     * has completed playing.
     *
     * @param aAlarn An input parameter that indicates the type of alarm to play.
     * @param aAlarmContext An output parameter that provides back a unique context to
     *  the client for this alarm that can be used for stopping the alarm.
     */
    IMPORT_C void PlayAlarm(TAlarmType aAlarm, unsigned int& aAlarmContext, MStsPlayAlarmObserver& aObserver);

    /**
     * Stops the specified alarm playback.  If the playback has already completed or the
     * context is not valid, this method does nothing.
     *
     * @param aAlarmContext The context to the alarm that is to be stopped.
     */
    IMPORT_C void StopAlarm(unsigned int aAlarmContext);

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

#endif  // SYSTEMTONESERVICE_H_
