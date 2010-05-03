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
 * The file contains the implementation of the STS Tester.
 */

#include "ststester.h"

#include <systemtoneservice.h>
#define PROFILE_TIME 1
#include "profileutilmacro.h"

const TInt KKeyMapPageCount = 1;

const TInt KFontSize = 15;

const TOperationsPage KKeyMap[KKeyMapPageCount] =
    {
        {STR("Standard controls"), 5, // default softkey index
           {
               // Enter
               {STR(""),                   KOperation_ExecuteOption},
               // Up / Down / Left / Right
               {STR(""),                   KOperation_PreviousOption},
               {STR(""),                   KOperation_NextOption},
               {STR(""),                   KOperation_PreviousOptionPage},
               {STR(""),                   KOperation_NextOptionPage},
               // 0 - 9
               {STR("Play Default Beep"),  EOperation_PlayDefaultBeep},
               {STR("Play Clock Alarm"),   EOperation_PlayClockAlarm},
               {STR("Stop Clock Alarm"),   EOperation_StopClockAlarm},
               {STR(""),                   KOperation_None},
               {STR(""),                   KOperation_None},
               {STR(""),                   KOperation_None},
               {STR(""),                   KOperation_None},
               {STR(""),                   KOperation_None},
               {STR(""),                   KOperation_None},
               {STR("Exit"),               KOperation_Exit}
           }
        }
    };

void CStsTester::ExecuteL()
    {
    CStsTester* self = new (ELeave) CStsTester;
    CleanupStack::PushL(self);
    self->InitL();
    self->Main();
    CleanupStack::PopAndDestroy(self);
    }

CStsTester::CStsTester() :
    CTestAppBase(KFontSize), iPlayState(EStopped)
    {
    }

CStsTester::~CStsTester()
    {
    CSystemToneService::Delete( iSts);
    }

void CStsTester::InitL()
    {
    BaseConstructL(KKeyMap, KKeyMapPageCount);
    iSts = CSystemToneService::Create();
    }

void CStsTester::Main()
    {
    TRAP_IGNORE( MainL() );
    }

void CStsTester::MainL()
    {
    _LIT( KPlayDefault, "Play Default Beep" );
    _LIT( KPlayClockAlarm, "Play Clock Alarm" );
    _LIT( KStopClockAlarm, "Stop Clock Alarm" );
    _LIT( KExit, "Exit");

    bool done = false;

    while (!done)
        {
        RPointerArray<TDesC> operations;
        operations.Append(&KPlayDefault);
        operations.Append(&KPlayClockAlarm);
        operations.Append(&KStopClockAlarm);
        operations.Append(&KExit);

        TInt index = SelectFromListL(TPoint(0, 0), iDisplaySize,
                _L("Select STS operation to perform:"), operations);

        operations.Reset();

        TPtrC operationName(STR("Play Default Beep"));

        switch (index)
            {
            case -1:
                done = true;
                break;
            case 0:
                ExecuteOperation(EOperation_PlayDefaultBeep, operationName);
                break;
            case 1:
                ExecuteOperation(EOperation_PlayClockAlarm, operationName);
                break;
            case 2:
                ExecuteOperation(EOperation_StopClockAlarm, operationName);
                break;
            case 3:
                done = true;
                break;
            }
        }
    }

void CStsTester::ExecuteOperation(TInt aOperation, const TDesC& /*aOperationText*/)
    {
    switch (aOperation)
        {
        case EOperation_PlayDefaultBeep:
            {
            TAG_TIME_PROFILING_BEGIN;
            iSts->PlayTone(CSystemToneService::EDefaultBeep);
            TAG_TIME_PROFILING_END;
            PRINT_TO_CONSOLE_TIME_DIFF;
            break;
            }
        case EOperation_PlayClockAlarm:
            {
            // Only play if not already playing
            if (iPlayState != EPlaying)
                {
                TAG_TIME_PROFILING_BEGIN;
                iSts->PlayTone(CSystemToneService::EClockAlarm,
                        iCurrentContext);
                TAG_TIME_PROFILING_END;
                PRINT_TO_CONSOLE_TIME_DIFF;
                iPlayState = EPlaying;
                }
            break;
            }
        case EOperation_StopClockAlarm:
            {
            TAG_TIME_PROFILING_BEGIN;
            iSts->StopTone(iCurrentContext);
            TAG_TIME_PROFILING_END;
            PRINT_TO_CONSOLE_TIME_DIFF;
            iPlayState = EStopped;
            break;
            }
        default:
            {
            break;
            }
        }
    }
