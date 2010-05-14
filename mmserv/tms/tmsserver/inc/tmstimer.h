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
 *    Utility class for CTimer object
 *
 */

#ifndef TMSTIMER_H
#define TMSTIMER_H

//  INCLUDES
#include <e32base.h>

namespace TMS {

/**
*  Mixin class for phone timer
*/
class TMSTimerObsrv
    {

    public: // New functions

        /**
        * Destructor
        */
        virtual ~TMSTimerObsrv(){};

        /**
        * This function is called after on timeout
        */
        virtual void HandleTimeOutL() = 0;
    };

// CLASS DECLARATION

/**
 *  Utility class for timer
 */
class TMSTimer : public CTimer
    {
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    IMPORT_C static TMSTimer* NewL(
            TInt aPriority = CActive::EPriorityStandard);

    /**
     * Destructor.
     */
    IMPORT_C virtual ~TMSTimer();

public:
    // New functions

    /**
     * The Callback function is called after the interval
     * All former request will be canceled first
     */
    IMPORT_C void After(TTimeIntervalMicroSeconds32 anInterval,
            TCallBack aCallBack);

    /**
     * Use the maxin class to notify the timer
     * All former request will be canceled first
     */
    IMPORT_C void After(TTimeIntervalMicroSeconds32 anInterval,
            TMSTimerObsrv* aObserver);

    /**
     * Cancel the timer if needed
     */
    IMPORT_C void CancelTimer();

protected:
    // Functions from base classes

    /**
     * From CTimer::RunL()
     */
    virtual void RunL();

    /**
     * From CTimer::RunL()
     */
    virtual TInt RunError(TInt aError);

    /**
     * From CTimer::DoCancel()
     */
    virtual void DoCancel();

private:

    /**
     * C++ default constructor.
     */
    TMSTimer(TInt aPriority);

    /**
     * By default Symbian OS constructor is private.
     */
    void ConstructL();

    // By default, prohibit copy constructor
    TMSTimer(const TMSTimer&);

    // Prohibit assigment operator
    TMSTimer& operator = (const TMSTimer&);

private:
    // Data

    // Used by After( anInterval, aCallBack )
    TCallBack iCallBack;

    // Used by After( anInterval, aObserver )
    TMSTimerObsrv* iTimerObserver;
    };

} // namespace TMS

#endif // TMSTIMER_H

// End of File
