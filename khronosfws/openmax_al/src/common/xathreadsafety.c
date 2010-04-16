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
* Description: 
*
*/

#include <stdlib.h>
#include "xathreadsafety.h"

typedef struct
{
    XAImplMutexHandle mutexTable[XATSCount];
    XAboolean tsEnabled;
}XAThreadSafetyImpl;

static XAThreadSafetyImpl* threadSafety;

/*
 * XAresult XAThreadSafety_Init()
 * Description: Creates mutex table for thread safety support
 * @return: Success value
 */
XAresult XAThreadSafety_Init( XAboolean tsEnable )
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAint32 i = 0;
    DEBUG_API_A1("->XAThreadSafety_Init - tsEnable:%lu",tsEnable);

    /* Initialize thread safety only once */
    if ( !threadSafety )
    {
        threadSafety = (XAThreadSafetyImpl *)calloc(1,sizeof(XAThreadSafetyImpl));
        if ( !threadSafety )
        {
            DEBUG_ERR("XA_RESULT_MEMORY_FAILURE");
            DEBUG_API("<-XAThreadSafety_Init");
            /* memory allocation failed */
            return XA_RESULT_MEMORY_FAILURE;
        }

        threadSafety->tsEnabled = tsEnable;

        if ( tsEnable )
        {
            for( i = 0; i < XATSCount; i++ )
            {
                ret = XAImpl_CreateMutex( &threadSafety->mutexTable[i] );
                if ( ret != XA_RESULT_SUCCESS )
                {
                    break;
                }
                DEBUG_INFO_A2("Created %s:%x",MEDIAOBJECTNAME(i), threadSafety->mutexTable[i] );
            }
        }
        else
        {
            DEBUG_INFO("Thread safety: disabled.");
        }
    }

    DEBUG_API("<-XAThreadSafety_Init");
    return ret;
}

/*
 * XAresult XAThreadSafety_Destroy()
 * Description: Destroys mutex table created for thread safety support
 */
XAresult XAThreadSafety_Destroy()
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAint32 i = 0;
    DEBUG_API("->XAThreadSafety_Destroy");

    if ( threadSafety )
    {
        if ( threadSafety->tsEnabled )
        {
            for( i = 0; i < XATSCount; i++ )
            {
                DEBUG_INFO_A2("Free %s:%x",MEDIAOBJECTNAME(i), threadSafety->mutexTable[i] );
                XAImpl_DeleteMutex( threadSafety->mutexTable[i] );
            }
        }
        free( threadSafety);
    }
    else
    {
        DEBUG_INFO("Thread safety: disabled.");
    }

    DEBUG_API("<-XAThreadSafety_Destroy");
    return ret;

}

/*
 * XAresult XAThreadSafety_Unlock( XAThreadSafetyMediaObjects mediaObject )
 * Description:
 * @param XAThreadSafetyMediaObjects mediaObject
 * @return
 */
XAresult XAThreadSafety_Unlock( XAThreadSafetyMediaObjects mediaObject )
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XAThreadSafety_Unlock");

    if ( threadSafety )
    {
        if ( threadSafety->tsEnabled )
        {
            ret = XAImpl_UnlockMutex( threadSafety->mutexTable[mediaObject] );
            if ( ret == XA_RESULT_SUCCESS)
            {
                DEBUG_INFO_A2("Released lock for %s:%x",MEDIAOBJECTNAME(mediaObject), threadSafety->mutexTable[mediaObject] );
            }
        }
    }
    else
    {
        DEBUG_INFO("Thread safety: disabled.");
    }
    DEBUG_API("<-XAThreadSafety_Unlock");
    return ret;
}
/*
 * XAresult XAThreadSafety_TryLock( XAThreadSafetyMediaObjects mediaObject );
 * Description:
 * @param XAThreadSafetyMediaObjects mediaObject
 * @return
 */
XAresult XAThreadSafety_TryLock( XAThreadSafetyMediaObjects mediaObject )
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XAThreadSafety_TryLock");

    if ( threadSafety )
    {
        if ( threadSafety->tsEnabled )
        {
            ret = XAImpl_TryLockMutex( threadSafety->mutexTable[mediaObject]);
            if ( ret == XA_RESULT_SUCCESS)
            {
                DEBUG_INFO_A2("Locked %s:%x",MEDIAOBJECTNAME(mediaObject), threadSafety->mutexTable[mediaObject] );
            }
        }
    }
    else
    {
        DEBUG_INFO("Thread safety: disabled.");
    }

    DEBUG_API("<-XAThreadSafety_TryLock");
    return ret;
}

