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
#include "xavideoencodercapabilitiesitf.h"

#include "xacapabilitiesmgr.h"

/* XAVideoEncoderCapabilitiesItfImpl* GetImpl
 * Description: Validate interface pointer and cast it to implementation pointer.
 */
static XAVideoEncoderCapabilitiesItfImpl* GetImpl(XAVideoEncoderCapabilitiesItf self)
{
    if( self )
    {
        XAVideoEncoderCapabilitiesItfImpl* impl = (XAVideoEncoderCapabilitiesItfImpl*)(*self);
        if( impl && (impl == impl->self) )
        {
            return impl;
        }
    }
    return NULL;
}

/**
 * Base interface XAVideoEncoderCapabilitiesItf implementation
 **/

/* XAresult XAVideoEncoderCapabilitiesItfImpl_GetVideoEncoders
 * Description: Retrieves available video encoders.
 */
XAresult XAVideoEncoderCapabilitiesItfImpl_GetVideoEncoders(
                            XAVideoEncoderCapabilitiesItf self,
                            XAuint32* pNumEncoders,
                            XAuint32* pEncoderIds)
{
    XAVideoEncoderCapabilitiesItfImpl* impl = GetImpl(self);
    XAresult res = XA_RESULT_SUCCESS;
    DEBUG_API("->XAVideoEncoderCapabilitiesItfImpl_GetVideoEncoders");

    if( !impl || !pNumEncoders )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        res = XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
        if( pEncoderIds )
        {   /* query array of encoders */
            if( *pNumEncoders < impl->numCodecs )
            {
                DEBUG_ERR("XA_RESULT_BUFFER_INSUFFICIENT");
                res = XA_RESULT_BUFFER_INSUFFICIENT;
            }
            else
            {
                
  
                XAuint32 i = 0;
                XACapabilities temp;
                for( i=0; i<impl->numCodecs; i++ )
                {
                    /* query encoder id from adaptation using index value */
                    XACapabilitiesMgr_GetCapsByIdx(NULL, (XACapsType)(XACAP_ENCODER|XACAP_VIDEO), i, &temp);
                    pEncoderIds[i] = temp.xaid;
                }

            }
        }
        /* return number of encoders */
        *pNumEncoders = impl->numCodecs;
    }

    DEBUG_API("<-XAVideoEncoderCapabilitiesItfImpl_GetVideoEncoders");
    return res;
}

/* XAresult XAVideoEncoderCapabilitiesItfImpl_GetVideoEncoderCapabilities
 * Description: Retrieves video encoder capabilities.
 */
XAresult XAVideoEncoderCapabilitiesItfImpl_GetVideoEncoderCapabilities(
                            XAVideoEncoderCapabilitiesItf self,
                            XAuint32 encoderId,
                            XAuint32* pIndex,
                            XAVideoCodecDescriptor* pDescriptor)
{
    XAVideoEncoderCapabilitiesItfImpl* impl = NULL;
    XAresult res = XA_RESULT_SUCCESS;

    DEBUG_API("->XAVideoEncoderCapabilitiesItfImpl_GetEncoderCapabilities");
    impl = GetImpl(self);
    if( !impl || !pIndex )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        res = XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
        if( !pDescriptor )
        {   /* query number of capa structures */
            *pIndex = 1;
        }
        else
        {   /* query capabilities */
            if( *pIndex >= 1 )
            {
                DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
                res = XA_RESULT_PARAMETER_INVALID;
            }
            else
            {
  
                /* query capabilities from adaptation using codec id */
                XACapabilities temp;
                memset(pDescriptor,0,sizeof(XAVideoCodecDescriptor));
                res = XACapabilitiesMgr_GetCapsById(NULL, (XACapsType)(XACAP_ENCODER|XACAP_VIDEO), encoderId, &temp);
                if( res == XA_RESULT_SUCCESS )
                {
                    XAVideoCodecDescriptor* desc = (XAVideoCodecDescriptor*)(&temp.pEntry);
                    /* map applicable values to XAVideoCodecCapabilities */
                    pDescriptor->codecId = temp.xaid;
                    pDescriptor->maxWidth = desc->maxWidth;
                    pDescriptor->maxHeight = desc->maxHeight;
                    pDescriptor->maxFrameRate = (desc->maxFrameRate & 0xffff)<<16;
                    pDescriptor->maxBitRate = desc->maxBitRate;
                    /*other caps undefined*/
                    pDescriptor->rateControlSupported = XA_RATECONTROLMODE_CONSTANTBITRATE;
                    pDescriptor->profileSetting = 0; /* unknown for theora or motionjpeg */
                    pDescriptor->levelSetting = 0; /* unknown for theora or motionjpeg */
                }
            
            }
        }
    }

    DEBUG_API("<-XAVideoEncoderCapabilitiesItfImpl_GetEncoderCapabilities");
    return res;
}

/**
 * XAVideoEncoderCapabilitiesItfImpl -specific methods
 **/

/* XAVideoEncoderCapabilitiesItfImpl_Create
 * Description: Allocate and initialize XAVideoEncoderCapabilitiesItfImpl
 */
XAVideoEncoderCapabilitiesItfImpl* XAVideoEncoderCapabilitiesItfImpl_Create()
{
    XAVideoEncoderCapabilitiesItfImpl* self = (XAVideoEncoderCapabilitiesItfImpl*)
        calloc(1,sizeof(XAVideoEncoderCapabilitiesItfImpl));
    DEBUG_API("->XAVideoEncoderCapabilitiesItfImpl_Create");

    if( self )
    {
        /* init itf default implementation */
        self->itf.GetVideoEncoders =
            XAVideoEncoderCapabilitiesItfImpl_GetVideoEncoders;
        self->itf.GetVideoEncoderCapabilities =
            XAVideoEncoderCapabilitiesItfImpl_GetVideoEncoderCapabilities;

  
        /* init variables */
        assert( XACapabilitiesMgr_GetCapsCount( NULL, (XACapsType)(XACAP_ENCODER|XACAP_VIDEO),
                                  &(self->numCodecs) ) == XA_RESULT_SUCCESS );

        self->self = self;
    }
    DEBUG_API("<-XAVideoEncoderCapabilitiesItfImpl_Create");
    return self;
}

/* void XAVideoEncoderCapabilitiesItfImpl_Free
 * Description: Free all resources reserved at XAVideoEncoderCapabilitiesItfImpl_Create
 */
void XAVideoEncoderCapabilitiesItfImpl_Free(XAVideoEncoderCapabilitiesItfImpl* self)
{
    DEBUG_API("->XAVideoEncoderCapabilitiesItfImpl_Free");
    assert(self==self->self);
    free(self);
    DEBUG_API("<-XAVideoEncoderCapabilitiesItfImpl_Free");
}
