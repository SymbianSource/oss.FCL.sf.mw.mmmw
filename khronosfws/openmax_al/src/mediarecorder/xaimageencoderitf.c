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
#include "xaimageencoderitf.h"

#include "xaimageencoderitfadaptation.h"
#include "xarecorditfadaptation.h"
#include "xathreadsafety.h"
#include "xacapabilitiesmgr.h"

#define APPROX_MAXRATIO (90/100)

/* static XAImageEncoderItfImpl* GetImpl(XAImageEncoderItf self)
 * Description: Validate interface pointer and cast it to implementation pointer.
 */
static XAImageEncoderItfImpl* GetImpl(XAImageEncoderItf self)
{
    if( self )
    {
        XAImageEncoderItfImpl* impl = (XAImageEncoderItfImpl*)(*self);
        if( impl && (impl == impl->self) )
        {
            return impl;
        }
    }
    return NULL;
}

/*****************************************************************************
 * Base interface XAImageEncoderItf implementation
 *****************************************************************************/

/* XAresult XAImageEncoderItfImpl_SetImageSettings
 * Description: Set image encoder settings.
 */
XAresult XAImageEncoderItfImpl_SetImageSettings(XAImageEncoderItf self,
                                                const XAImageSettings *pSettings)
{
    XAresult res = XA_RESULT_SUCCESS;
    XACapabilities temp;
    XAImageEncoderItfImpl *impl = GetImpl(self);

    DEBUG_API("->XAImageEncoderItfImpl_SetImageSettings");
    XA_IMPL_THREAD_SAFETY_ENTRY( XATSMediaRecorder );

    if( !impl || !pSettings )
    {
        XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaRecorder );
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAImageEncoderItfImpl_SetImageSettings");
        return XA_RESULT_PARAMETER_INVALID;
    }
    if(impl->adaptCtx->fwtype == FWMgrFWGST)
        {
        res = XACapabilitiesMgr_GetCapsById(NULL, (XACapsType)(XACAP_ENCODER|XACAP_IMAGE), pSettings->encoderId, &temp);
        if( res == XA_RESULT_SUCCESS )
            {
#ifdef OMAX_CAMERABIN
            res = XAImageEncoderItfAdapt_SetImageSettings((XAAdaptationGstCtx*)impl->adaptCtx, pSettings);
#endif
            }
        else
            {
            DEBUG_ERR("XA_RESULT_FEATURE_UNSUPPORTED");
            res = XA_RESULT_FEATURE_UNSUPPORTED;
            }
        }
    
    XA_IMPL_THREAD_SAFETY_EXIT( XATSMediaRecorder );

    DEBUG_API("<-XAImageEncoderItfImpl_SetImageSettings");
    return res;
}

/* XAresult XAImageEncoderItfImpl_GetImageSettings
 * Description: Get image encoder settings.
 */
XAresult XAImageEncoderItfImpl_GetImageSettings(XAImageEncoderItf self,
                                                XAImageSettings *pSettings)
{
    XAresult res = XA_RESULT_SUCCESS;
    XAImageEncoderItfImpl *impl = GetImpl(self);

    DEBUG_API("->XAImageEncoderItfImpl_GetImageSettings");
    if( !impl || !pSettings )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAImageEncoderItfImpl_GetImageSettings");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }
    if(impl->adaptCtx->fwtype == FWMgrFWGST)
        {
#ifdef OMAX_CAMERABIN
        res = XAImageEncoderItfAdapt_GetImageSettings((XAAdaptationGstCtx*)impl->adaptCtx, pSettings);
#endif
        }
    else
        {
        DEBUG_ERR("XA_RESULT_FEATURE_UNSUPPORTED");
        DEBUG_API("<-XAImageEncoderItfImpl_GetImageSettings");
        return XA_RESULT_FEATURE_UNSUPPORTED;    
        }

    DEBUG_API("<-XAImageEncoderItfImpl_GetImageSettings");
    return res;
}

/* XAresult XAImageEncoderItfImpl_GetSizeEstimate
 * Description: Get estimated image size.
 */
XAresult XAImageEncoderItfImpl_GetSizeEstimate(XAImageEncoderItf self,
                                               XAuint32 *pSize)
{
    XAuint32 bpp = 0; /*bytes per pixel*/
    XAuint32 compPct = 0; /*compression percentage*/
    XAImageEncoderItfImpl *impl = GetImpl(self);
    XAImageSettings curSettings = {0,0,0,0,0};
    DEBUG_API("->XAImageEncoderItfImpl_GetImageSettings");

    if( !impl || !pSize )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAImageEncoderItfImpl_GetImageSettings");
        /* invalid parameter */
        return XA_RESULT_PARAMETER_INVALID;
    }
    /* calculate size estimate */

    if(impl->adaptCtx->fwtype == FWMgrFWGST)
        {
#ifdef OMAX_CAMERABIN
        XAImageEncoderItfAdapt_GetImageSettings((XAAdaptationGstCtx*)impl->adaptCtx, &curSettings);
#endif
        }
    else
        {
        DEBUG_ERR("XA_RESULT_FEATURE_UNSUPPORTED");
        DEBUG_API("<-XAImageEncoderItfImpl_GetSizeEstimate");
        return XA_RESULT_FEATURE_UNSUPPORTED;   
        }
    
    switch(curSettings.colorFormat)
    {
        case XA_COLORFORMAT_MONOCHROME:
            bpp = 1;
            break;
        case XA_COLORFORMAT_L2:
            bpp = 2;
            break;
        case XA_COLORFORMAT_L4:
            bpp = 4;
            break;
        case XA_COLORFORMAT_8BITRGB332:
        case XA_COLORFORMAT_RAWBAYER8BIT:
        case XA_COLORFORMAT_RAWBAYER8BITCOMPRESSED:
        case XA_COLORFORMAT_L8:
            bpp = 8;
            break;
        case XA_COLORFORMAT_RAWBAYER10BIT:
            bpp = 10;
            break;
        case XA_COLORFORMAT_12BITRGB444:
        case XA_COLORFORMAT_YUV444INTERLEAVED:
        case XA_COLORFORMAT_YUV411PLANAR:
        case XA_COLORFORMAT_YUV420PLANAR:
        case XA_COLORFORMAT_YUV420SEMIPLANAR:
            bpp = 12;
            break;
        case XA_COLORFORMAT_16BITARGB4444:
        case XA_COLORFORMAT_16BITARGB1555:
        case XA_COLORFORMAT_16BITRGB565:
        case XA_COLORFORMAT_16BITBGR565:
        case XA_COLORFORMAT_YCBYCR:
        case XA_COLORFORMAT_YCRYCB:
        case XA_COLORFORMAT_CBYCRY:
        case XA_COLORFORMAT_CRYCBY:
        case XA_COLORFORMAT_L16:
        case XA_COLORFORMAT_YUV422PLANAR:
        case XA_COLORFORMAT_YUV422SEMIPLANAR:
            bpp = 16;
            break;
        case XA_COLORFORMAT_18BITRGB666:
        case XA_COLORFORMAT_18BITARGB1665:
        case XA_COLORFORMAT_18BITBGR666:
            bpp = 18;
            break;
        case XA_COLORFORMAT_19BITARGB1666:
            bpp = 19;
            break;
        case XA_COLORFORMAT_24BITRGB888:
        case XA_COLORFORMAT_24BITBGR888:
        case XA_COLORFORMAT_24BITARGB1887:
        case XA_COLORFORMAT_24BITARGB6666:
        case XA_COLORFORMAT_24BITABGR6666:
        case XA_COLORFORMAT_L24:
            bpp = 24;
            break;
        case XA_COLORFORMAT_25BITARGB1888:
            bpp = 25;
            break;
        case XA_COLORFORMAT_32BITBGRA8888:
        case XA_COLORFORMAT_32BITARGB8888:
        case XA_COLORFORMAT_L32:
            bpp = 32;
            break;
        default:
            bpp = 8;
            break;
    }

    compPct = (1000 - (curSettings.compressionLevel*APPROX_MAXRATIO))/1000;

    /* numofpixels * colorformat(bits/pixel) * compression rate */
    *pSize = (curSettings.height * curSettings.width ) * bpp/8 * compPct;

    DEBUG_API("<-XAImageEncoderItfImpl_GetImageSettings");
    return XA_RESULT_SUCCESS;
}
/*****************************************************************************
 * XAImageEncoderItfImpl -specific methods
 *****************************************************************************/


/* XAImageEncoderItfImpl* XAImageEncoderItfImpl_Create()
 * Description: Allocate and initialize ImageEncoderItfImpl
 */
XAImageEncoderItfImpl* XAImageEncoderItfImpl_Create(XAMediaRecorderImpl* impl)
{
    XAImageEncoderItfImpl* self = (XAImageEncoderItfImpl*)
        calloc(1,sizeof(XAImageEncoderItfImpl));
    //XAMediaRecorderAdaptationCtx* mCtx = (XAMediaRecorderAdaptationCtx*)(impl->adaptationCtx);
    
    DEBUG_API("->XAImageEncoderItfImpl_Create");
    if( self )
    {
        //if(mCtx->fwtype == FWMgrFWGST)
            {
            /* init itf default implementation */
            self->itf.GetImageSettings = XAImageEncoderItfImpl_GetImageSettings;
            self->itf.SetImageSettings = XAImageEncoderItfImpl_SetImageSettings;
            self->itf.GetSizeEstimate = XAImageEncoderItfImpl_GetSizeEstimate;
            }

        /* init variables */
        self->adaptCtx = impl->adaptationCtx;
        
        self->self = self;
    }
    DEBUG_API("<-XAImageEncoderItfImpl_Create");
    return self;
}

/* void XAImageEncoderItfImpl_Free(XAImageEncoderItfImpl* self)
 * Description: Free all resources reserved at XAImageEncoderItfImpl_Create()
 */
void XAImageEncoderItfImpl_Free(XAImageEncoderItfImpl* self)
{
    DEBUG_API("->XAImageEncoderItfImpl_Free");
    assert( self==self->self );
    free( self );
    DEBUG_API("<-XAImageEncoderItfImpl_Free");
}

