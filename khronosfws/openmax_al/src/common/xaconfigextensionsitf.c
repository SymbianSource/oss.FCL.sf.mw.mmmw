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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "xaglobals.h"
#include "xaconfigextensionsitf.h"

static XAConfigExtensionsItfImpl* GetImpl(XAConfigExtensionsItf self)
{
    if( self )
    {
        XAConfigExtensionsItfImpl* impl = (XAConfigExtensionsItfImpl*)(*self);
        if( impl && (impl == impl->self) )
        {
            return impl;
        }
    }
    return NULL;
}

/**
 * Base interface XAConfigExtensionsItf implementation
 */
XAresult XAConfigExtensionsItfImpl_SetConfiguration(
                            XAConfigExtensionsItf self,
                            const XAchar* configKey,
                            XAuint32 valueSize,
                            const void* pConfigValue)
{
    XAConfigExtensionsItfImpl* impl = GetImpl(self);
    XAresult res = XA_RESULT_SUCCESS;
    DEBUG_API("->XAConfigExtensionsItfImpl_SetConfiguration");
    if( !impl || !configKey )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAConfigExtensionsItfImpl_SetConfiguration");
        return XA_RESULT_PARAMETER_INVALID;
    }
    if( strcmp((char *)configKey, CPCONFIGKEY) == 0
        && pConfigValue )
    {
        XAuint8 numValue = *(XAuint8*)(pConfigValue);
        impl->testbufferconf = numValue;
        if( numValue>4 )
        {   /* Conformance tester uses only reference-to-string values, try to parse string to integer
             * First char is enough since legal values are 0 to 4
             */
            XAchar* chVal = *(XAchar**)(pConfigValue);
            if(chVal)
            {
                impl->testbufferconf = *chVal;
                numValue = (*chVal) - 48;
            }
            if( numValue>4 )
            {
                res = XA_RESULT_PARAMETER_INVALID;
            }
        }
        DEBUG_INFO_A1("CP conf value %d", numValue);
        if(res == XA_RESULT_SUCCESS)
        {

            /*Inform adaptation*/
            if( impl->ctx )
            {
/*                res = XAAdaptationBase_SetCPConfiguration(impl->ctx,impl->testbufferconf);*/
            }

        }
    }
    else
    {
        DEBUG_ERR("Unknown key or invalid value pointer");
        res = XA_RESULT_PARAMETER_INVALID;
    }
    DEBUG_API("<-XAConfigExtensionsItfImpl_SetConfiguration");
    return res;
}

XAresult XAConfigExtensionsItfImpl_GetConfiguration(
                            XAConfigExtensionsItf self,
                            const XAchar* configKey,
                            XAuint32* pValueSize,
                            void* pConfigValue)
{
    XAConfigExtensionsItfImpl* impl = GetImpl(self);
    XAresult res = XA_RESULT_SUCCESS;
    DEBUG_API("->XAConfigExtensionsItfImpl_GetConfiguration");

    if( !impl )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAConfigExtensionsItfImpl_SetConfiguration");
        return XA_RESULT_PARAMETER_INVALID;
    }

    if( strcmp((char *)configKey, CPCONFIGKEY) == 0
            && pValueSize
            && sizeof(XAuint8) <= *pValueSize
            && pConfigValue )
    {
        memcpy(*(XAuint8**)pConfigValue, &(impl->testbufferconf),1);
        *pValueSize = sizeof(XAuint8);
        res = XA_RESULT_SUCCESS;
    }
    else
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        res = XA_RESULT_PARAMETER_INVALID;
    }
    DEBUG_API("<-XAConfigExtensionsItfImpl_GetConfiguration");
    return res;
}

 
XAresult XAConfigExtensionsItfImpl_SetContext(XAConfigExtensionsItfImpl* self,XAAdaptationBaseCtx* adptctx)
    {
    XAresult res = XA_RESULT_SUCCESS;
    DEBUG_API("->XAConfigExtensionsItfImpl_SetContext");
    if( adptctx && self)
    {
        self->ctx = adptctx;
        res = XA_RESULT_SUCCESS;
    }
    else
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        res = XA_RESULT_PARAMETER_INVALID;
    }
    DEBUG_API("<-XAConfigExtensionsItfImpl_SetContext");
    return res;
    }

/**
 * XAConfigExtensionsItfImpl -specific methods
 **/
XAConfigExtensionsItfImpl* XAConfigExtensionsItfImpl_Create()
{
    XAConfigExtensionsItfImpl* self = (XAConfigExtensionsItfImpl*)
        calloc(1,sizeof(XAConfigExtensionsItfImpl));
    DEBUG_API("->XAConfigExtensionsItfImpl_Create");

    if(self)
    {
        /* init itf default implementation */
        self->itf.GetConfiguration =
            XAConfigExtensionsItfImpl_GetConfiguration;
        self->itf.SetConfiguration =
            XAConfigExtensionsItfImpl_SetConfiguration;
        /* init variables */
        self->self = self;
    }
    DEBUG_API("<-XAConfigExtensionsItfImpl_Create");
    return self;
}

void XAConfigExtensionsItfImpl_Free(XAConfigExtensionsItfImpl* self)
{
    DEBUG_API("->XAConfigExtensionsItfImpl_Free");
    assert(self==self->self);
    free(self);
    DEBUG_API("<-XAConfigExtensionsItfImpl_Free");
}
