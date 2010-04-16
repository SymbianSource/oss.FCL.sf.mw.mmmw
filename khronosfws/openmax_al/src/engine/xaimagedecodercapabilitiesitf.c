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
#include "xaimagedecodercapabilitiesitf.h"
#ifdef _GSTREAMER_BACKEND_  
#include "XAStaticCapsAdaptation.h"
#endif
/* XAImageDecoderCapabilitiesItfImpl* GetImpl
 * Description: Validate interface pointer and cast it to implementation pointer.
 */
static XAImageDecoderCapabilitiesItfImpl* GetImpl(XAImageDecoderCapabilitiesItf self)
{
    if( self )
    {
        XAImageDecoderCapabilitiesItfImpl* impl = (XAImageDecoderCapabilitiesItfImpl*)(*self);
        if( impl && (impl == impl->self) )
        {
            return impl;
        }
    }
    return NULL;
}

/**
 * Base interface XAImageDecoderCapabilitiesItf implementation
 **/

/* XAresult XAImageDecoderCapabilitiesItfImpl_GetImageDecoderCapabilities
 * Description: Retrieves image decoder capabilities.
 */
XAresult XAImageDecoderCapabilitiesItfImpl_GetImageDecoderCapabilities(
                            XAImageDecoderCapabilitiesItf self,
                            XAuint32* pDecoderId,
                            XAImageCodecDescriptor* pDescriptor)
{
    XAImageDecoderCapabilitiesItfImpl* impl = NULL;
    XAresult res = XA_RESULT_SUCCESS;
    DEBUG_API("->XAImageDecoderCapabilitiesItfImpl_GetImageDecoderCapabilities");

    impl = GetImpl(self);
    if( !impl || !pDecoderId )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        res = XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
        if( !pDescriptor )
        {   /* query number of image decoders */
            *pDecoderId = impl->numCodecs;
        }
        else
        {
            /* query capabilities from adaptation using codec id */
#ifdef _GSTREAMER_BACKEND_  
            XAStaticCapsData temp;
            memset(pDescriptor,0,sizeof(XAImageCodecDescriptor));
            /* here pEncoderId refers to index rather than codec id */
            res = XAStaticCapsAdapt_GetCapsByIdx(XACAP_DECODER|XACAP_IMAGE, *pDecoderId, &temp);
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

    DEBUG_API("<-XAImageDecoderCapabilitiesItfImpl_GetImageDecoderCapabilities");
    return res;
}

/* XAresult XAImageDecoderCapabilitiesItfImpl_QueryColorFormats
 * Description: This method is used to query the color formats supported
 *              by the image decoder.
 */
XAresult XAImageDecoderCapabilitiesItfImpl_QueryColorFormats(
                            const XAImageDecoderCapabilitiesItf self,
                            XAuint32* pIndex,
                            XAuint32* pColorFormats)
{
    XAImageDecoderCapabilitiesItfImpl* impl = NULL;
    XAresult res = XA_RESULT_SUCCESS;
    DEBUG_API("->XAImageDecoderCapabilitiesItfImpl_QueryColorFormats");
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
    DEBUG_API("<-XAImageDecoderCapabilitiesItfImpl_QueryColorFormats");
    return res;
}

/**
 * XAImageDecoderCapabilitiesItfImpl -specific methods
 **/

/* XAImageDecoderCapabilitiesItfImpl_Create
 * Description: Allocate and initialize XAImageDecoderCapabilitiesItfImpl
 */
XAImageDecoderCapabilitiesItfImpl* XAImageDecoderCapabilitiesItfImpl_Create()
{
    XAImageDecoderCapabilitiesItfImpl* self = (XAImageDecoderCapabilitiesItfImpl*)
        calloc(1,sizeof(XAImageDecoderCapabilitiesItfImpl));
    DEBUG_API("->XAImageDecoderCapabilitiesItfImpl_Create");

    if( self )
    {
        /* init itf default implementation */
        self->itf.GetImageDecoderCapabilities =
            XAImageDecoderCapabilitiesItfImpl_GetImageDecoderCapabilities;
        self->itf.QueryColorFormats =
            XAImageDecoderCapabilitiesItfImpl_QueryColorFormats;

#ifdef _GSTREAMER_BACKEND_  
        /* init variables */
        assert( XAStaticCapsAdapt_GetCapsCount( XACAP_DECODER|XACAP_IMAGE,
                                  &(self->numCodecs) ) == XA_RESULT_SUCCESS );
#endif
        self->self = self;
    }
    DEBUG_API("<-XAImageDecoderCapabilitiesItfImpl_Create");
    return self;
}

/* void XAImageDecoderCapabilitiesItfImpl_Free
 * Description: Free all resources reserved at XAImageDecoderCapabilitiesItfImpl
 */
void XAImageDecoderCapabilitiesItfImpl_Free(XAImageDecoderCapabilitiesItfImpl* self)
{
    DEBUG_API("->XAImageDecoderCapabilitiesItfImpl_Free");
    assert(self==self->self);
    free(self);
    DEBUG_API("<-XAImageDecoderCapabilitiesItfImpl_Free");
}
