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
/* Define all debug macros here */

#ifndef XADEBUG_H
#define XADEBUG_H

#define _DEBUG_API_     1
#define _DEBUG_ERR_     2
#define _DEBUG_INFO_    4

/*
 * Define wanted debug level via DEBUG_LEVEL macro:
 * _DEPUG_API_ - prints entry/exit messages in the methods
 * _DEBUG_ERR_ - prints error messages
 * _DEBUG_INFO_ - prints info messages
 *
 * e.g. DEBUG_LEVEL ( _DEBUG_API_ | _DEBUG_ERR_ ) prints entry/exit and
 * error messages.
 */
#define DEBUG_LEVEL 0


/*********************************************************/
/*
 * Debug definitions do not touch
 */
#ifdef _DEBUG
#include <stdio.h>
    #if ( DEBUG_LEVEL & _DEBUG_API_ )
        #define DEBUG_API(X) printf(X"\n");
        #define DEBUG_API_A1(X,A1) printf(X"\n",A1);
        #define DEBUG_API_A2(X,A1,A2) printf(X"\n",A1,A2);
        #define DEBUG_API_A3(X,A1,A2,A3) printf(X"\n",A1,A2,A3);
        #define DEBUG_API_A4(X,A1,A2,A3,A4) printf(X"\n",A1,A2,A3,A4);
    #else // ( DEBUG_LEVEL & _DEBUG_API_ ) 
        #define DEBUG_API(X);
        #define DEBUG_API_A1(X,A1);
        #define DEBUG_API_A2(X,A1,A2);
        #define DEBUG_API_A3(X,A1,A2,A3);
        #define DEBUG_API_A4(X,A1,A2,A3,A4);
    #endif // ( DEBUG_LEVEL & _DEBUG_API_ ) 

    #if ( DEBUG_LEVEL & _DEBUG_ERR_ )
        #define DEBUG_ERR(X) printf("  ERR: "X"\n");
        #define DEBUG_ERR_A1(X,A1) printf("  ERR: "X"\n",A1);
        #define DEBUG_ERR_A2(X,A1,A2) printf("  ERR: "X"\n",A1,A2);
        #define DEBUG_ERR_A3(X,A1,A2,A3) printf("  ERR: "X"\n",A1,A2,A3);
    #else // ( DEBUG_LEVEL & _DEBUG_ERR_ ) 
        #define DEBUG_ERR(X);
        #define DEBUG_ERR_A1(X,A1);
        #define DEBUG_ERR_A2(X,A1,A2);
        #define DEBUG_ERR_A3(X,A1,A2,A3);
    #endif // ( DEBUG_LEVEL & _DEBUG_ERR_ ) 

    #if ( DEBUG_LEVEL & _DEBUG_INFO_ )
        #define DEBUG_INFO(X) printf("  :"X"\n");
        #define DEBUG_INFO_A1(X,A1) printf("  :"X"\n",A1);
        #define DEBUG_INFO_A2(X,A1,A2) printf("  :"X"\n",A1,A2);
        #define DEBUG_INFO_A3(X,A1,A2,A3) printf("  :"X"\n",A1,A2,A3);
        #define DEBUG_INFO_A4(X,A1,A2,A3,A4) printf("  :"X"\n",A1,A2,A3,A4);
    #else // ( DEBUG_LEVEL & _DEBUG_INFO_ ) 
        #define DEBUG_INFO(X);
        #define DEBUG_INFO_A1(X,A1);
        #define DEBUG_INFO_A2(X,A1,A2);
        #define DEBUG_INFO_A3(X,A1,A2,A3);
        #define DEBUG_INFO_A4(X,A1,A2,A3,A4);
    #endif // ( DEBUG_LEVEL & _DEBUG_INFO_ )

#else 

        #define DEBUG_API(X)
        #define DEBUG_API_A1(X,A1)
        #define DEBUG_API_A2(X,A1,A2)
        #define DEBUG_API_A3(X,A1,A2,A3)
        #define DEBUG_API_A4(X,A1,A2,A3,A4)

        #define DEBUG_ERR(X)
        #define DEBUG_ERR_A1(X,A1)
        #define DEBUG_ERR_A2(X,A1,A2)
        #define DEBUG_ERR_A3(X,A1,A2,A3)

        #define DEBUG_INFO(X)
        #define DEBUG_INFO_A1(X,A1)
        #define DEBUG_INFO_A2(X,A1,A2)
        #define DEBUG_INFO_A3(X,A1,A2,A3)
        #define DEBUG_INFO_A4(X,A1,A2,A3,A4);
#endif /* _DEBUG */

#endif /* XADEBUG_H */
