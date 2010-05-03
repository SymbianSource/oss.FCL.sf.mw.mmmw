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
 * The file defines the interface for the STS Tester.
 */

#ifndef STSTESTER_H_
#define STSTESTER_H_

#include "testappbase.h"

class CSystemToneService;

enum TOperations
    {
    EOperation_PlayDefaultBeep = KOperation_FirstCustomIndex,
    EOperation_PlayClockAlarm,
    EOperation_StopClockAlarm
    };

class CStsTester : public CTestAppBase
    {
public:
    // Methods

    static void ExecuteL();

private:
    // Methods

    CStsTester();

    ~CStsTester();

    void InitL();

    void Main();

    void MainL();

    // inherited from CTestAppBase
    virtual void ExecuteOperation(TInt aOperation,
            const TDesC& aOperationText);

private:
    // Attributes

    enum TPlayState
        {
        EStopped, EPlaying
        };

    // STS attributes
    CSystemToneService* iSts;
    unsigned int iCurrentContext;
    TPlayState iPlayState;

    };

#endif // STSTESTER_H_
