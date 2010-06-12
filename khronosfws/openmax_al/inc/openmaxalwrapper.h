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
* Description:  Internal Header to Include Khronos Headers in One Place.
*               Also redefining the XA_API macro to export the IID value
*               to DEF files.
*
*/
#ifndef _OPENMAXAL_WRAPPER_H_
#define _OPENMAXAL_WRAPPER_H_

#include <OpenMAXAL_Platform.h>
#undef XA_API

#define XA_API __declspec(dllexport)
 
#include <OpenMAXAL.h>
#include <xanokiavolumeextitf.h>
#include <xanokialinearvolumeitf.h>

#endif
