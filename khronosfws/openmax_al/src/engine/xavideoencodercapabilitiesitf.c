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
#ifdef _GSTREAMER_BACKEND_  
#include "XAStaticCapsAdaptation.h"
#endif
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
                
#ifdef _GSTREAMER_BACKEND_  
                XAuint32 i = 0;
                XAStaticCapsData temp;
                for( i=0; i<impl->numCodecs; i++ )
                {
                    /* query encoder id from adaptation using index value */
                    XAStaticCapsAdapt_GetCapsByIdx(XACAP_ENCODER|XACAP_VIDEO, i, &temp);
                    pEncoderIds[i] = temp.xaid;
                }
#endif
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
#ifdef _GSTREAMER_BACKEND_  
                /* query capabilities from adaptation using codec id */
                XAStaticCapsData temp;
                memset(pDescriptor,0,sizeof(XAVideoCodecDescriptor));
                res = XAStaticCapsAdapt_GetCapsById(XACAP_ENCODER|XACAP_VIDEO, encoderId, &temp);
                if( res == XA_RESULT_SUCCESS )
                {
                    /* map applicable values to XAAudioCodecCapabilities */
                    pDescriptor->codecId = temp.xaid;
                    pDescriptor->maxWidth = temp.maxW;
                    pDescriptor->maxHeight = temp.maxH;
                    pDescriptor->maxFrameRate = (temp.maxFR & 0xffff)<<16;
                    pDescriptor->maxBitRate = temp.maxBR;
                    /*other caps undefined*/
                    pDescriptor->rateControlSupported = XA_RATECONTROLMODE_CONSTANTBITRATE;
                    pDescriptor->profileSetting = 0; /* unknown for theora or motionjpeg */
                    pDescriptor->levelSetting = 0; /* unknown for theora or motionjpeg */
                }
#endif            
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

#ifdef _GSTREAMER_BACKEND_  
        /* init variables */
        assert( XAStaticCapsAdapt_GetCapsCount( XACAP_ENCODER|XACAP_VIDEO,
                                  &(self->numCodecs) ) == XA_RESULT_SUCCESS );
#endif
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
