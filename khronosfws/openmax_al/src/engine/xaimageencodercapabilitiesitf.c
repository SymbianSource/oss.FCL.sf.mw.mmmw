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
#include "xacapabilitiesmgr.h"  
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
  
            XACapabilities temp;
            memset(pDescriptor,0,sizeof(XAImageCodecDescriptor));
            /* here pEncoderId refers to index rather than codec id */
            res = XACapabilitiesMgr_GetCapsByIdx(NULL, (XACapsType)(XACAP_ENCODER|XACAP_IMAGE), *pEncoderId, &temp);
            if( res == XA_RESULT_SUCCESS )
            {
                XAImageCodecDescriptor* desc = (XAImageCodecDescriptor*)(&temp.pEntry);
                /* map applicable values to XAAudioCodecCapabilities */
                pDescriptor->codecId = temp.xaid;
                pDescriptor->maxWidth = desc->maxWidth;
                pDescriptor->maxHeight = desc->maxHeight;
            }

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

        res = XACapabilitiesMgr_QueryColorFormats(NULL, pIndex, pColorFormats);
        
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

  
        /* init variables */
        assert( XACapabilitiesMgr_GetCapsCount( NULL, (XACapsType)(XACAP_ENCODER|XACAP_IMAGE),
                                  &(self->numCodecs) ) == XA_RESULT_SUCCESS );

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
