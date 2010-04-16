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
#include "xadynamicsourceitf.h"
#ifdef _GSTREAMER_BACKEND_   
#include "XAAdaptationContextBase.h"
#include "XADynamicSourceItfAdaptation.h"
#endif

/*****************************************************************************
 * Base interface XADynamicSourceItf implementation
 *****************************************************************************/

/* XAResult XADynamicSourceItfImpl_SetSource
 * Description: Sets the data source for the object.
 */
XAresult XADynamicSourceItfImpl_SetSource(XADynamicSourceItf self,
                                          XADataSource *pDataSource)
{
    XAresult res = XA_RESULT_SUCCESS;
    XADynamicSourceItfImpl* impl = (XADynamicSourceItfImpl*)(*self);
    DEBUG_API("->XADynamicSourceItfImpl_SetSource");
    /* check casting */
    if( !impl ||  impl != impl->self || !pDataSource )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XADynamicSourceItfImpl_SetSource");
        return XA_RESULT_PARAMETER_INVALID;
    }
#ifdef _GSTREAMER_BACKEND_   
    if( !impl->adaptCtx )
    {
        DEBUG_ERR("Adaptation not ready!!");
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("<-XADynamicSourceItfImpl_SetSource");
        res = XA_RESULT_INTERNAL_ERROR;
    }
    else
    {
        res = XACommon_CheckDataSource(pDataSource,NULL);
        if(res==XA_RESULT_SUCCESS)
        {
            res = XADynamicSourceItfAdapt_SetSource(impl->adaptCtx, pDataSource );
        }
    }
#endif
    DEBUG_API("<-XADynamicSourceItfImpl_SetSource");
    return res;
}

/*****************************************************************************
 * XADynamicSourceItfImpl -specific methods
 *****************************************************************************/
#ifdef _GSTREAMER_BACKEND_   

/* XADynamicSourceItfImpl* XADynamicSourceItfImpl_Create()
 * Description: Allocate and initialize DynamicSourceItfImpl
 */
XADynamicSourceItfImpl* XADynamicSourceItfImpl_Create( XAAdaptationBaseCtx *adaptCtx )
{
    XADynamicSourceItfImpl* self = (XADynamicSourceItfImpl*)
        calloc(1,sizeof(XADynamicSourceItfImpl));
    DEBUG_API("->XADynamicSourceItfImpl_Create");
    if( self )
    {
        /* init itf default implementation */
        self->itf.SetSource = XADynamicSourceItfImpl_SetSource;
        /* init variables */
        self->adaptCtx = adaptCtx;
        self->self = self;
    }
    DEBUG_API("<-XADynamicSourceItfImpl_Create");
    return self;
}
#endif
/* void XADynamicSourceItfImpl_Free(XADynamicSourceItfImpl* self)
 * Description: Free all resources reserved at XADynamicSourceItfImpl_Create
 */
void XADynamicSourceItfImpl_Free(XADynamicSourceItfImpl* self)
{
    DEBUG_API("->XADynamicSourceItfImpl_Free");
    assert( self==self->self );
    free( self );
    DEBUG_API("<-XADynamicSourceItfImpl_Free");
}


