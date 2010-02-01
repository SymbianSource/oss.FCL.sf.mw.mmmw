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

#include <tms.h>
#include "callipadpt.h"
#include "callcsadpt.h"
#include "calladpt.h"
#include "tmsutility.h"

using namespace TMS;

// -----------------------------------------------------------------------------
// CallAdpt::CallAdpt
//
// -----------------------------------------------------------------------------
//
CallAdpt::CallAdpt()
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CallAdpt::~CallAdpt
//
// -----------------------------------------------------------------------------
//
CallAdpt::~CallAdpt()
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    }

// -----------------------------------------------------------------------------
// CallAdpt::CreateCallL
//
// -----------------------------------------------------------------------------
//
gint CallAdpt::CreateCallL(gint callType, CallAdpt*& callAdpt)
    {
    TRACE_PRN_FN_ENT;
    CallAdpt* self(NULL);
    gint retVal(TMS_RESULT_SUCCESS);
    switch (callType)
        {
        case TMS_CALL_IP:
            retVal = TMS_RESULT_INSUFFICIENT_MEMORY;
            self = CallIPAdpt::NewL();
            if (self)
                {
                retVal = self->PostConstruct();
                if (retVal != TMS_RESULT_SUCCESS)
                    {
                    delete self;
                    self = NULL;
                    }
                }
            break;

        case TMS_CALL_CS:
            retVal = TMS_RESULT_INSUFFICIENT_MEMORY;
            self = new CallCSAdpt();
            if (self)
                {
                retVal = self->PostConstruct();
                if (retVal != TMS_RESULT_SUCCESS)
                    {
                    delete self;
                    self = NULL;
                    }
                }
            break;
        default:
            retVal = TMS_RESULT_CALL_TYPE_NOT_SUPPORTED;
            break;
        }
    callAdpt = self;
    TRACE_PRN_FN_EXT;
    return retVal;
    }

// -----------------------------------------------------------------------------
// CallAdpt::PostConstruct
//
// -----------------------------------------------------------------------------
//
gint CallAdpt::PostConstruct()
    {
    TRACE_PRN_FN_ENT;
    TRACE_PRN_FN_EXT;
    return TMS_RESULT_SUCCESS;
    }

// End of file
