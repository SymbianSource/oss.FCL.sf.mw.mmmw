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
#include "xavideodecodercapabilitiesitf.h"
#ifdef _GSTREAMER_BACKEND_  
#include "XAStaticCapsAdaptation.h"
#endif
/* XAVideoDecoderCapabilitiesItfImpl* GetImpl
 * Description: Validate interface pointer and cast it to implementation pointer.
 */
static XAVideoDecoderCapabilitiesItfImpl* GetImpl(XAVideoDecoderCapabilitiesItf self)
{
    if( self )
    {
        XAVideoDecoderCapabilitiesItfImpl* impl = (XAVideoDecoderCapabilitiesItfImpl*)(*self);
        if( impl && (impl == impl->self) )
        {
            return impl;
        }
    }
    return NULL;
}

/**
 * Base interface XAVideoDecoderCapabilitiesItf implementation
 **/

/* XAresult XAVideoDecoderCapabilitiesItfImpl_GetVideoDecoders
 * Description: Retrieves available video decoders.
 */
XAresult XAVideoDecoderCapabilitiesItfImpl_GetVideoDecoders(
                            XAVideoDecoderCapabilitiesItf self,
                            XAuint32* pNumDecoders,
                            XAuint32* pDecoderIds)
{
    XAVideoDecoderCapabilitiesItfImpl* impl = NULL;
    XAresult res = XA_RESULT_SUCCESS;

    DEBUG_API("->XAVideoDecoderCapabilitiesItfImpl_GetVideoDecoders");

    impl = GetImpl(self);
    if( !impl || !pNumDecoders )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        res = XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
        if( pDecoderIds )
        {   /* query array of decoders */
            if( *pNumDecoders < impl->numCodecs )
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
                    /* query decoder id from adaptation using index value */
                    XAStaticCapsAdapt_GetCapsByIdx(XACAP_DECODER|XACAP_VIDEO, i, &temp);
                    pDecoderIds[i] = temp.xaid;
                }
#endif
            }
        }
        /* return number of decoders */
        *pNumDecoders = impl->numCodecs;
    }

    DEBUG_API("<-XAVideoDecoderCapabilitiesItfImpl_GetVideoDecoders");
    return res;
}

/* XAresult XAVideoDecoderCapabilitiesItfImpl_GetVideoDecoderCapabilities
 * Description: Retrieves video decoder capabilities.
 */
XAresult XAVideoDecoderCapabilitiesItfImpl_GetVideoDecoderCapabilities(
                            XAVideoDecoderCapabilitiesItf self,
                            XAuint32 decoderId,
                            XAuint32* pIndex,
                            XAVideoCodecDescriptor* pDescriptor)
{
    XAVideoDecoderCapabilitiesItfImpl* impl = NULL;
    XAresult res = XA_RESULT_SUCCESS;
    DEBUG_API("->XAVideoDecoderCapabilitiesItfImpl_GetDecoderCapabilities");
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
#ifdef _GSTREAMER_BACKEND_  
                XAStaticCapsData temp;
                memset(pDescriptor,0,sizeof(XAVideoCodecDescriptor));
                res = XAStaticCapsAdapt_GetCapsById(XACAP_DECODER|XACAP_VIDEO, decoderId, &temp);
                if( res == XA_RESULT_SUCCESS )
                {
                    /* map applicable values to XAVideoCodecCapabilities */
                    pDescriptor->codecId = temp.xaid;
                    pDescriptor->maxWidth = temp.maxW;
                    pDescriptor->maxHeight = temp.maxH;
                    pDescriptor->maxFrameRate = (temp.maxFR & 0xffff)<<16;
                    pDescriptor->maxBitRate = temp.maxBR;
                    /*other caps undefined*/
                    pDescriptor->rateControlSupported = 0; /* reserved in decoders */
                    pDescriptor->profileSetting = 0; /* unknown for theora or motionjpeg */
                    pDescriptor->levelSetting = 0; /* unknown for theora or motionjpeg */
                }
#endif
            }
        }
    }

    DEBUG_API("<-XAVideoDecoderCapabilitiesItfImpl_GetDecoderCapabilities");
    return res;
}

/**
 * XAVideoDecoderCapabilitiesItfImpl -specific methods
 **/

/* XAVideoDecoderCapabilitiesItfImpl_Create
 * Description: Allocate and initialize XAVideoDecoderCapabilitiesItfImpl
 */
XAVideoDecoderCapabilitiesItfImpl* XAVideoDecoderCapabilitiesItfImpl_Create()
{
    XAVideoDecoderCapabilitiesItfImpl* self = (XAVideoDecoderCapabilitiesItfImpl*)
        calloc(1,sizeof(XAVideoDecoderCapabilitiesItfImpl));
    DEBUG_API("->XAVideoDecoderCapabilitiesItfImpl_Create");

    if( self )
    {
        /* init itf default implementation */
        self->itf.GetVideoDecoders =
            XAVideoDecoderCapabilitiesItfImpl_GetVideoDecoders;
        self->itf.GetVideoDecoderCapabilities =
            XAVideoDecoderCapabilitiesItfImpl_GetVideoDecoderCapabilities;

#ifdef _GSTREAMER_BACKEND_  
        /* init variables */
        assert( XAStaticCapsAdapt_GetCapsCount( XACAP_DECODER|XACAP_VIDEO,
                                  &(self->numCodecs) ) == XA_RESULT_SUCCESS );
#endif
        self->self = self;
    }
    DEBUG_API("<-XAVideoDecoderCapabilitiesItfImpl_Create");
    return self;
}

/* void XAVideoDecoderCapabilitiesItfImpl_Free
 * Description: Free all resources reserved at XAVideoDecoderCapabilitiesItfImpl_Create
 */
void XAVideoDecoderCapabilitiesItfImpl_Free(XAVideoDecoderCapabilitiesItfImpl* self)
{
    DEBUG_API("->XAVideoDecoderCapabilitiesItfImpl_Free");
    assert(self==self->self);
    free(self);
    DEBUG_API("<-XAVideoDecoderCapabilitiesItfImpl_Free");
}
