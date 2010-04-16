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
#include "xaimageencodercapabilitiesitf.h"
#ifdef _GSTREAMER_BACKEND_  
#include "XAStaticCapsAdaptation.h"
#endif
/* XAImageEncoderCapabilitiesItfImpl* GetImpl
 * Description: Validate interface pointer and cast it to implementation pointer.
 */
static XAImageEncoderCapabilitiesItfImpl* GetImpl(XAImageEncoderCapabilitiesItf self)
{
    if( self )
    {
        XAImageEncoderCapabilitiesItfImpl* impl = (XAImageEncoderCapabilitiesItfImpl*)(*self);
        if( impl && (impl == impl->self) )
        {
            return impl;
        }
    }
    return NULL;
}

/**
 * Base interface XAImageEncoderCapabilitiesItf implementation
 **/

/* XAresult XAImageEncoderCapabilitiesItfImpl_GetImageEncoderCapabilities
 * Description: Retrieves image encoder capabilities.
 */
XAresult XAImageEncoderCapabilitiesItfImpl_GetImageEncoderCapabilities(
                            XAImageEncoderCapabilitiesItf self,
                            XAuint32* pEncoderId,
                            XAImageCodecDescriptor* pDescriptor)
{
    XAImageEncoderCapabilitiesItfImpl* impl = NULL;
    XAresult res = XA_RESULT_SUCCESS;
    DEBUG_API("->XAImageEncoderCapabilitiesItfImpl_GetImageEncoderCapabilities");

    impl = GetImpl(self);

    if( !impl || !pEncoderId )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        res = XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
        if( !pDescriptor )
        {   /* query number of image encoders */
            *pEncoderId = impl->numCodecs;
        }
        else
        {
            /* query capabilities from adaptation using codec id */
#ifdef _GSTREAMER_BACKEND_  
            XAStaticCapsData temp;
            memset(pDescriptor,0,sizeof(XAImageCodecDescriptor));
            /* here pEncoderId refers to index rather than codec id */
            res = XAStaticCapsAdapt_GetCapsByIdx(XACAP_ENCODER|XACAP_IMAGE, *pEncoderId, &temp);
            if( res == XA_RESULT_SUCCESS )
            {
                /* map applicable values to XAAudioCodecCapabilities */
                pDescriptor->codecId = temp.xaid;
                pDescriptor->maxWidth = temp.maxW;
                pDescriptor->maxHeight = temp.maxH;
            }
#endif
        }
    }

    DEBUG_API("<-XAImageEncoderCapabilitiesItfImpl_GetImageEncoderCapabilities");
    return res;
}

/* XAresult XAImageEncoderCapabilitiesItfImpl_QueryColorFormats
 * Description: This method is used to query the color formats supported
 *              by the image encoder.
 */
XAresult XAImageEncoderCapabilitiesItfImpl_QueryColorFormats(
                            const XAImageEncoderCapabilitiesItf self,
                            XAuint32* pIndex,
                            XAuint32* pColorFormats)
{
    XAImageEncoderCapabilitiesItfImpl* impl = NULL;
    XAresult res = XA_RESULT_SUCCESS;
    DEBUG_API("->XAImageEncoderCapabilitiesItfImpl_QueryColorFormats");

    impl = GetImpl(self);
    if( !impl || !pIndex )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        res = XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
#ifdef _GSTREAMER_BACKEND_
        res = XAStaticCapsAdapt_QueryColorFormats(pIndex, pColorFormats);
#endif        
    }
    DEBUG_API("<-XAImageEncoderCapabilitiesItfImpl_QueryColorFormats");
    return res;
}

/**
 * XAImageEncoderCapabilitiesItfImpl -specific methods
 **/

/* XAImageEncoderCapabilitiesItfImpl_Create
 * Description: Allocate and initialize XAImageEncoderCapabilitiesItfImpl
 */
XAImageEncoderCapabilitiesItfImpl* XAImageEncoderCapabilitiesItfImpl_Create()
{
    XAImageEncoderCapabilitiesItfImpl* self = (XAImageEncoderCapabilitiesItfImpl*)
        calloc(1,sizeof(XAImageEncoderCapabilitiesItfImpl));
    DEBUG_API("->XAImageEncoderCapabilitiesItfImpl_Create");

    if( self )
    {
        /* init itf default implementation */
        self->itf.GetImageEncoderCapabilities =
            XAImageEncoderCapabilitiesItfImpl_GetImageEncoderCapabilities;
        self->itf.QueryColorFormats =
            XAImageEncoderCapabilitiesItfImpl_QueryColorFormats;

#ifdef _GSTREAMER_BACKEND_  
        /* init variables */
        assert( XAStaticCapsAdapt_GetCapsCount( XACAP_ENCODER|XACAP_IMAGE,
                                  &(self->numCodecs) ) == XA_RESULT_SUCCESS );
#endif
        self->self = self;
    }
    DEBUG_API("<-XAImageEncoderCapabilitiesItfImpl_Create");
    return self;
}

/* void XAImageEncoderCapabilitiesItfImpl_Free
 * Description: Free all resources reserved at XAImageEncoderCapabilitiesItfImpl
 */
void XAImageEncoderCapabilitiesItfImpl_Free(XAImageEncoderCapabilitiesItfImpl* self)
{
    DEBUG_API("->XAImageEncoderCapabilitiesItfImpl_Free");
    assert(self==self->self);
    free(self);
    DEBUG_API("<-XAImageEncoderCapabilitiesItfImpl_Free");
}
