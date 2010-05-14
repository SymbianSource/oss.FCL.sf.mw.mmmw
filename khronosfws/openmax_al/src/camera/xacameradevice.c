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
#include "xacameradevice.h"
#include "xacameraitf.h"
#include "xaconfigextensionsitf.h"
#include "xadynintmgmtitf.h"
#include "xaimagecontrolsitf.h"
#include "xaimageeffectsitf.h"
#include "xavideopostprocessingitf.h"
#include "xathreadsafety.h"
#include "xaframeworkmgr.h"
#include "xacameraadaptctx.h"
#include "xacapabilitiesmgr.h"

/* Static mapping of enumeration XACameraDeviceInterfaces to interface iids */
static const XAInterfaceID* XACameraDeviceItfIIDs[CAMERA_ITFCOUNT]=
{
    &XA_IID_OBJECT,
    &XA_IID_CAMERA,
    &XA_IID_CONFIGEXTENSION,
    &XA_IID_DYNAMICINTERFACEMANAGEMENT,
    &XA_IID_IMAGECONTROLS,
    &XA_IID_IMAGEEFFECTS,
    &XA_IID_VIDEOPOSTPROCESSING
};


/*****************************************************************************
 * Global methods
 *****************************************************************************/

/* XAResult XACameraDeviceImpl_Create
 * Description: Create object
 */
XAresult XACameraDeviceImpl_CreateCameraDevice(FrameworkMap* mapper,
                                                XACapabilities* capabilities,   
                                               XAObjectItf* pDevice,
                                               XAuint32 deviceID,
                                               XAuint32 numInterfaces,
                                               const XAInterfaceID * pInterfaceIds,
                                               const XAboolean * pInterfaceRequired)
{
    XACameraDeviceImpl* pImpl = NULL;
    XAObjectItfImpl* pBaseObj = NULL;
    XAuint8 itfIndex = 0;

    DEBUG_API("->XACameraDeviceImpl_Create");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSCamera);

    if( !pDevice )
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraDeviceImpl_Create");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* instantiate object implementation */
    pImpl = (XACameraDeviceImpl*)calloc(1,sizeof(XACameraDeviceImpl));
    if( !pImpl )
    {
        /* memory allocation failed */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
        DEBUG_ERR("XA_RESULT_MEMORY_FAILURE");
        DEBUG_API("<-XACameraDeviceImpl_Create");
        return XA_RESULT_MEMORY_FAILURE;
    }
    pBaseObj = &pImpl->baseObj;

    /* Initialize base object default implementation */
    XAObjectItfImpl_Init(pBaseObj,
                         CAMERA_ITFCOUNT,
                         XACameraDeviceItfIIDs,
                         XACameraDeviceImpl_DoRealize,
                         XACameraDeviceImpl_DoResume,
                         XACameraDeviceImpl_FreeResources);

    /* Mark interfaces that need to be exposed */
    /* Implicit and mandated interfaces */
    pBaseObj->interfaceMap[CAMERA_CAMERAITF].required = XA_BOOLEAN_TRUE;
    pBaseObj->interfaceMap[CAMERA_DIMITF].required = XA_BOOLEAN_TRUE;

    /* Explicit interfaces */
    if( (numInterfaces != 0) && pInterfaceIds && pInterfaceRequired )
    {
        /* Check required interfaces */
        for( itfIndex = 0; itfIndex < numInterfaces; itfIndex++ )
        {
            /* If mapEntry is null then required interface is not supported.*/
            XAObjItfMapEntry *entry =
                XAObjectItfImpl_GetItfEntry((XAObjectItf)&(pBaseObj), pInterfaceIds[itfIndex]);
            if( !entry  )
            {
                if( pInterfaceRequired[itfIndex] )
                {
                    /* required interface cannot be accommodated - fail creation */
                    XAObjectItfImpl_Destroy((XAObjectItf)&(pBaseObj));
                    XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
                    DEBUG_ERR("Required interface not found - abort creation!");
                    DEBUG_API("<-XACameraDeviceImpl_Create");
                    return XA_RESULT_FEATURE_UNSUPPORTED;
                }
                else
                {
                    DEBUG_INFO("Requested (not required) interface not found - continue creation");
                }
            }
            else
            {
                entry->required = XA_BOOLEAN_TRUE;
            }
        }
    }

    /* Mark interfaces that can be handled dynamically */
    /* Mandated dynamic itfs */
    pBaseObj->interfaceMap[CAMERA_IMAGEEFFECTSITF].isDynamic = XA_BOOLEAN_TRUE;

    /* This code is put here to return Feature Not Supported since adaptation is not present*/
    /*************************************************/
    XAObjectItfImpl_Destroy((XAObjectItf)&(pBaseObj));
    XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
    DEBUG_ERR("Required interface not found - abort creation!");
    DEBUG_API("<-XACameraDeviceImpl_Create");
    return XA_RESULT_FEATURE_UNSUPPORTED;
    /*************************************************/
    
/*     Initialize XACameraDeviceImpl variables 
    pImpl->deviceID = deviceID;

    pImpl->adaptationCtx = XACameraAdapt_Create(pImpl->deviceID);

     Set ObjectItf to point to newly created object 
    *pDevice = (XAObjectItf)&(pBaseObj->self);

    XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
    DEBUG_API("<-XACameraDeviceImpl_Create");
    return XA_RESULT_SUCCESS;*/
}

/* XAResult XACameraDeviceImpl_QueryNumSupportedInterfaces
 * Description: Statically query number of supported interfaces
 */
XAresult XACameraDeviceImpl_QueryNumSupportedInterfaces( XAuint32 *pNumSupportedInterfaces )
{
    DEBUG_API("->XACameraDeviceImpl_QueryNumSupportedInterfaces");
    if( pNumSupportedInterfaces )
    {
        *pNumSupportedInterfaces = CAMERA_ITFCOUNT;

        DEBUG_API_A1("<-XACameraDeviceImpl_QueryNumSupportedInterfaces - %i", (int)(*pNumSupportedInterfaces) );
        return XA_RESULT_SUCCESS;
    }
    else
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraDeviceImpl_QueryNumSupportedInterfaces");
        return XA_RESULT_PARAMETER_INVALID;
    }
}
/* XAResult XACameraDeviceImpl_QuerySupportedInterfaces
 * Description: Statically query supported interfaces
 */
XAresult XACameraDeviceImpl_QuerySupportedInterfaces( XAuint32 index,
                                                      XAInterfaceID * pInterfaceId )
{
    DEBUG_API("->XACameraDeviceImpl_QuerySupportedInterfaces");

    if( index >= CAMERA_ITFCOUNT || !pInterfaceId )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraDeviceImpl_QuerySupportedInterfaces");
        return XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
        *pInterfaceId = *(XACameraDeviceItfIIDs[index]);

        DEBUG_API("<-XACameraDeviceImpl_QuerySupportedInterfaces");
        return XA_RESULT_SUCCESS;
    }
}


/*****************************************************************************
 * base object XAObjectItfImpl methods
 *****************************************************************************/

/* XAresult XACameraDeviceImpl_DoRealize( XAObjectItf self )
 * Description: Realize all implicit and explicitly wanted interfaces.
 * Create and initialize implementation-specific variables.
 * Called from base object XAObjectItfImpl
 */
XAresult XACameraDeviceImpl_DoRealize( XAObjectItf self )
{
    XAuint8 itfIdx = 0;
    XAObjectItfImpl* pObj = (XAObjectItfImpl*)(*self);
    XACameraDeviceImpl* pObjImpl = (XACameraDeviceImpl*)(pObj);
    XAresult ret = XA_RESULT_SUCCESS;


    DEBUG_API("->XACameraDeviceImpl_DoRealize");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSCamera);

    /* check casting from correct pointer type */
    if( !pObjImpl || pObj != pObjImpl->baseObj.self )
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XACameraDeviceImpl_DoRealize");
        return XA_RESULT_PARAMETER_INVALID;
    }

 
    ret = XACameraAdapt_PostInit( pObjImpl->adaptationCtx );

    if( ret != XA_RESULT_SUCCESS )
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
        DEBUG_ERR_A1("Camera adapt postinit - %d", ret);
        DEBUG_API("<-XACameraDeviceImpl_DoRealize");
        return ret;
    }

    /* Realize all implicit and explicitly wanted interfaces */
    for( itfIdx = 0; itfIdx < CAMERA_ITFCOUNT; itfIdx++)
    {
        if( !(pObj->interfaceMap[itfIdx].pItf) &&
            pObj->interfaceMap[itfIdx].required )
        {
            void *pItf = NULL;
            switch( itfIdx )
            {
                case CAMERA_DIMITF:
                    pItf = XADIMItfImpl_Create();
                    if(pItf)
                    {
                        XADIMItfImpl_Init(pItf, self,
                                XACameraDeviceImpl_DoAddItf,
                                XACameraDeviceImpl_DoResumeItf,
                                XACameraDeviceImpl_DoRemoveItf);
                    }
                    break;
  
                case CAMERA_CAMERAITF:
                    pItf = XACameraItfImpl_Create( pObjImpl->adaptationCtx );
                    break;
                case CAMERA_CONFIGEXTENSIONITF:
                    pItf = XAConfigExtensionsItfImpl_Create();
                    break;
                case CAMERA_IMAGECONTROLSITF:
                   pItf = XAImageControlsItfImpl_Create( pObjImpl->adaptationCtx );
                   break;
                case CAMERA_IMAGEEFFECTSITF:
                   pItf = XAImageEffectsItfImpl_Create( pObjImpl->adaptationCtx );
                   break;
                case CAMERA_VIDEOPOSTPROCESSINGITF:
                     pItf = XAVideoPostProcessingItfImpl_Create( pObjImpl->adaptationCtx );
                     break;
                  
                default:
                    break;
            }
            if( !pItf )
            {
                /* memory allocation failed */
                XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
                DEBUG_ERR("XA_RESULT_MEMORY_FAILURE");
                DEBUG_API("<-XACameraDeviceImpl_DoRealize");
                return XA_RESULT_MEMORY_FAILURE;
            }
            else
            {
                pObj->interfaceMap[itfIdx].pItf = pItf;
            }
        }
    }

    pObj->state = XA_OBJECT_STATE_REALIZED;
    XA_IMPL_THREAD_SAFETY_EXIT(XATSCamera);
    DEBUG_API("<-XACameraDeviceImpl_DoRealize");
    return XA_RESULT_SUCCESS;
}

/* XAresult XACameraDeviceImpl_DoResume
 * Description: Resume object from suspended state
 */
XAresult XACameraDeviceImpl_DoResume(XAObjectItf self)
{
    DEBUG_API("->XACameraDeviceImpl_DoResume");
    DEBUG_API("<-XACameraDeviceImpl_DoResume");
    /* This implementation does not support suspended state */
    return XA_RESULT_PRECONDITIONS_VIOLATED;
}

/* void XACameraDeviceImpl_FreeResources
 * Description: Free all resources reserved at XACameraDeviceImpl_DoRealize()
 */
void XACameraDeviceImpl_FreeResources(XAObjectItf self)
{
    XAObjectItfImpl* pObj = (XAObjectItfImpl*)(*self);
    XAuint8 itfIdx = 0;
    XACameraDeviceImpl* pImpl = (XACameraDeviceImpl*)(*self);
    assert( pObj && pImpl && pObj == pObj->self );
    DEBUG_API("->XACameraDeviceImpl_FreeResources");
    XA_IMPL_THREAD_SAFETY_ENTRY_FOR_VOID_FUNCTIONS(XATSCamera);
    

    

    /* free all allocated interfaces */
    for(itfIdx = 0; itfIdx < CAMERA_ITFCOUNT; itfIdx++)
    {
        void *pItf = pObj->interfaceMap[itfIdx].pItf;
        if(pItf)
        {
            switch(itfIdx)
            {
                case CAMERA_CAMERAITF:
                    XACameraItfImpl_Free( pItf );
                    break;
                case CAMERA_CONFIGEXTENSIONITF:
                    XAConfigExtensionsItfImpl_Free( pItf );
                    break;
                case CAMERA_DIMITF:
                    XADIMItfImpl_Free( pItf );
                    break;
                case CAMERA_IMAGECONTROLSITF:
                    XAImageControlsItfImpl_Free( pItf );
                    break;
                case CAMERA_IMAGEEFFECTSITF:
                    XAImageEffectsItfImpl_Free( pItf );
                    break;
                case CAMERA_VIDEOPOSTPROCESSINGITF:
                    XAVideoPostProcessingItfImpl_Free( pItf );
                    break;
                default:
                    break;
            }
            pObj->interfaceMap[itfIdx].pItf = NULL;
        }
    }
  
    if ( pImpl->adaptationCtx != NULL )
    {
        XACameraAdapt_Destroy( pImpl->adaptationCtx );
        pImpl->adaptationCtx = NULL;
    }

    XA_IMPL_THREAD_SAFETY_EXIT_FOR_VOID_FUNCTIONS(XATSCamera);
    DEBUG_API("<-XACameraDeviceImpl_FreeResources");
    return;
}

/*****************************************************************************
 * CameraDeviceImpl -specific methods
 *****************************************************************************/

/* XACameraDeviceImpl_DoAddItf
 * Dynamically add an interface, object specific parts
 */
XAresult XACameraDeviceImpl_DoAddItf(XAObjectItf self, XAObjItfMapEntry *mapEntry  )
{

    XAObjectItfImpl* pObj = (XAObjectItfImpl*)(*self);
    XACameraDeviceImpl* pImpl = (XACameraDeviceImpl*)(pObj);

    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XACameraDeviceImpl_DoAddItf");
    if(mapEntry)
    {
        switch( mapEntry->mapIdx )
        {
        case CAMERA_IMAGEEFFECTSITF:
            mapEntry->pItf = XAImageEffectsItfImpl_Create( pImpl->adaptationCtx );
            break;
        default:
            DEBUG_ERR("XACameraDeviceImpl_DoAddItf unknown id");
            ret = XA_RESULT_FEATURE_UNSUPPORTED;
            break;
        }

        if( !mapEntry->pItf && ret == XA_RESULT_SUCCESS)
            {
                DEBUG_ERR("XACameraDeviceImpl_DoAddItf itf creation failed");
                ret = XA_RESULT_MEMORY_FAILURE;
            }
    }
    else
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        ret = XA_RESULT_PARAMETER_INVALID;
    }

    DEBUG_API("<-XACameraDeviceImpl_DoAddItf");
    return ret;
}

/* XACameraDeviceImpl_DoResumeItf
 * Try to resume lost interface, object specific parts
 */
XAresult XACameraDeviceImpl_DoResumeItf(XAObjectItf self, XAObjItfMapEntry *mapEntry  )
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XACameraDeviceImpl_DoResumeItf");
    /* For now, no difference between suspended and unrealised itfs */
    ret = XACameraDeviceImpl_DoAddItf(self,mapEntry);
    DEBUG_API("<-XACameraDeviceImpl_DoResumeItf");
    return ret;
}

/* XACameraDeviceImpl_DoRemoveItf
 * Dynamically remove an interface, object specific parts
 */
XAresult XACameraDeviceImpl_DoRemoveItf(XAObjectItf self, XAObjItfMapEntry *mapEntry )
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XACameraDeviceImpl_DoRemoveItf");
    if(mapEntry)
    {
        switch( mapEntry->mapIdx )
        {
        case CAMERA_IMAGEEFFECTSITF:
            XAImageEffectsItfImpl_Free( mapEntry->pItf );
            break;
        default:
            DEBUG_ERR("XACameraDeviceImpl_DoRemoveItf unknown id");
            ret = XA_RESULT_FEATURE_UNSUPPORTED;
            break;
        }
        mapEntry->pItf = NULL;
    }
    else
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        ret = XA_RESULT_PARAMETER_INVALID;
    }

    DEBUG_API("<-XACameraDeviceImpl_DoRemoveItf");
    return ret;
}
/* END OF FILE */
