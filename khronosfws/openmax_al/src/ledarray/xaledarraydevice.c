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
#include "xaledarraydevice.h"
#include "xaledarrayitf.h"
#include "xaconfigextensionsitf.h"
#include "xadynintmgmtitf.h"
#include "xathreadsafety.h"


/* Static mapping of enumeration XALEDArrayDeviceInterfaces to interface iids */
static const XAInterfaceID* XALEDArrayDeviceItfIIDs[LEDARRAY_ITFCOUNT]=
{
    &XA_IID_OBJECT,
    &XA_IID_LED,
    &XA_IID_CONFIGEXTENSION,
    &XA_IID_DYNAMICINTERFACEMANAGEMENT,
};


/*****************************************************************************
 * Global methods
 *****************************************************************************/

/* XAResult XALEDArrayDeviceImpl_Create
 * Description: Create object
 */
XAresult XALEDArrayDeviceImpl_CreateLEDArrayDevice(XAObjectItf* pDevice,
                                                   XAuint32 deviceID,
                                                   XAuint32 numInterfaces,
                                                   const XAInterfaceID * pInterfaceIds,
                                                   const XAboolean * pInterfaceRequired)
{

    
    XAuint8 itfIndex = 0;

    XALEDArrayDeviceImpl* pImpl = NULL;
    XAObjectItfImpl* pBaseObj = NULL;
    

    DEBUG_API("->XALEDArrayDeviceImpl_Create");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSLEDArray);

    if( !pDevice )
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSLEDArray);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XALEDArrayDeviceImpl_Create");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* instantiate object implementation */
    pImpl = (XALEDArrayDeviceImpl*)calloc(1,sizeof(XALEDArrayDeviceImpl));
    if( !pImpl )
    {
        /* memory allocation failed */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSLEDArray);
        DEBUG_ERR("XA_RESULT_MEMORY_FAILURE");
        DEBUG_API("<-XALEDArrayDeviceImpl_Create");
        return XA_RESULT_MEMORY_FAILURE;
    }
    pBaseObj = &pImpl->baseObj;



    /* Initialize base object default implementation */
    XAObjectItfImpl_Init(pBaseObj,
                         LEDARRAY_ITFCOUNT,
                         XALEDArrayDeviceItfIIDs,
                         XALEDArrayDeviceImpl_DoRealize,
                         XALEDArrayDeviceImpl_DoResume,
                         XALEDArrayDeviceImpl_FreeResources);

    /* Mark interfaces that need to be exposed */
    /* Implicit and mandated interfaces */
    pBaseObj->interfaceMap[LEDARRAY_LEDARRAYITF].required = XA_BOOLEAN_TRUE;
    pBaseObj->interfaceMap[LEDARRAY_DIMITF].required = XA_BOOLEAN_TRUE;

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
                    XA_IMPL_THREAD_SAFETY_EXIT(XATSLEDArray);
                    DEBUG_ERR("Required interface not found - abort creation!");
                    DEBUG_API("<-XALEDArrayDeviceImpl_Create");
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

    /* Initialize XALEDArrayDeviceImpl variables */
    pImpl->deviceID = deviceID;
#ifdef _GSTREAMER_BACKEND_  
    pImpl->adaptationCtx = XALEDArrayAdapt_Create(pImpl->deviceID);
#endif
    
    /* This code is put here to return Feature Not Supported since adaptation is not present*/
    /*************************************************/
    XAObjectItfImpl_Destroy((XAObjectItf)&(pBaseObj));
    XA_IMPL_THREAD_SAFETY_EXIT(XATSLEDArray);
    DEBUG_ERR("Required interface not found - abort creation!");
    DEBUG_API("<-XALEDArrayDeviceImpl_Create");
    return XA_RESULT_FEATURE_UNSUPPORTED;
    /*************************************************/
    
    /* Set ObjectItf to point to newly created object */
/*
    *pDevice = (XAObjectItf)&(pBaseObj->self);
    XA_IMPL_THREAD_SAFETY_EXIT(XATSLEDArray);
    DEBUG_API("<-XALEDArrayDeviceImpl_Create");
    return XA_RESULT_SUCCESS;
*/
}

/* XAResult XALEDArrayDeviceImpl_QueryNumSupportedInterfaces
 * Description: Statically query number of supported interfaces
 */
XAresult XALEDArrayDeviceImpl_QueryNumSupportedInterfaces( XAuint32 *pNumSupportedInterfaces )
{
    DEBUG_API("->XALEDArrayDeviceImpl_QueryNumSupportedInterfaces");
    if( pNumSupportedInterfaces )
    {
#ifdef _GSTREAMER_BACKEND_  
        *pNumSupportedInterfaces = LEDARRAY_ITFCOUNT;
#endif
        DEBUG_API_A1("<-XALEDArrayDeviceImpl_QueryNumSupportedInterfaces - %ld", *pNumSupportedInterfaces );
        return XA_RESULT_SUCCESS;
    }
    else
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XALEDArrayDeviceImpl_QueryNumSupportedInterfaces");
        return XA_RESULT_PARAMETER_INVALID;
    }
}
/* XAResult XALEDArrayDeviceImpl_QuerySupportedInterfaces
 * Description: Statically query supported interfaces
 */
XAresult XALEDArrayDeviceImpl_QuerySupportedInterfaces( XAuint32 index,
                                                      XAInterfaceID * pInterfaceId )
{
    DEBUG_API("->XALEDArrayDeviceImpl_QuerySupportedInterfaces");

#ifdef _GSTREAMER_BACKEND_  
    if( index >= LEDARRAY_ITFCOUNT || !pInterfaceId )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XALEDArrayDeviceImpl_QuerySupportedInterfaces");
        return XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
        *pInterfaceId = *(XALEDArrayDeviceItfIIDs[index]);
#endif
        DEBUG_API("<-XALEDArrayDeviceImpl_QuerySupportedInterfaces");
        return XA_RESULT_SUCCESS;
#ifdef _GSTREAMER_BACKEND_  
    }
#endif    
}


/*****************************************************************************
 * base object XAObjectItfImpl methods
 *****************************************************************************/

/* XAresult XALEDArrayDeviceImpl_DoRealize( XAObjectItf self )
 * Description: Realize all implicit and explicitly wanted interfaces.
 * Create and initialize implementation-specific variables.
 * Called from base object XAObjectItfImpl
 */
XAresult XALEDArrayDeviceImpl_DoRealize( XAObjectItf self )
{
#ifdef _GSTREAMER_BACKEND_
    XAObjectItfImpl* pObj = (XAObjectItfImpl*)(*self);
    XAuint8 itfIdx = 0;
    XALEDArrayDeviceImpl* pObjImpl = (XALEDArrayDeviceImpl*)(pObj);
    XAresult ret = XA_RESULT_SUCCESS;

    DEBUG_API("->XALEDArrayDeviceImpl_DoRealize");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSLEDArray);

    /* check casting from correct pointer type */
    if( !pObjImpl || pObj != pObjImpl->baseObj.self )
    {
        /* invalid parameter */
        XA_IMPL_THREAD_SAFETY_EXIT(XATSLEDArray);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XALEDArrayDeviceImpl_DoRealize");
        return XA_RESULT_PARAMETER_INVALID;
    }

    ret = XALEDArrayAdapt_PostInit( pObjImpl->adaptationCtx );
    if( ret != XA_RESULT_SUCCESS )
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSLEDArray);
        DEBUG_ERR_A1("Error: %d",ret);
        DEBUG_API("<-XALEDArrayDeviceImpl_DoRealize");
        return ret;
    }

    /* Realize all implicit and explicitly wanted interfaces */
    for( itfIdx = 0; itfIdx < LEDARRAY_ITFCOUNT; itfIdx++)
    {
        if( !(pObj->interfaceMap[itfIdx].pItf) &&
            pObj->interfaceMap[itfIdx].required )
        {
            void *pItf = NULL;
            switch( itfIdx )
            {
                case LEDARRAY_LEDARRAYITF:
                    pItf = XALEDArrayItfImpl_Create( pObjImpl->adaptationCtx );
                    break;
                case LEDARRAY_CONFIGEXTENSIONITF:
                    pItf = XAConfigExtensionsItfImpl_Create();
                    break;
                case LEDARRAY_DIMITF:
                    pItf = XADIMItfImpl_Create();
                    break;
                default:
                    break;
            }
            if( !pItf )
            {
                /* memory allocation failed */
                XA_IMPL_THREAD_SAFETY_EXIT(XATSLEDArray);
                DEBUG_ERR("XA_RESULT_MEMORY_FAILURE");
                DEBUG_API("<-XALEDArrayDeviceImpl_DoRealize");
                return XA_RESULT_MEMORY_FAILURE;
            }
            else
            {
                pObj->interfaceMap[itfIdx].pItf = pItf;
            }
        }
    }

    pObj->state = XA_OBJECT_STATE_REALIZED;
    XA_IMPL_THREAD_SAFETY_EXIT(XATSLEDArray);
#endif    
    DEBUG_API("<-XALEDArrayDeviceImpl_DoRealize");
    return XA_RESULT_SUCCESS;
}

/* XAresult XALEDArrayDeviceImpl_DoResume
 * Description: Resume object from suspended state
 */
XAresult XALEDArrayDeviceImpl_DoResume(XAObjectItf self)
{
    DEBUG_API("->XALEDArrayDeviceImpl_DoResume");
    DEBUG_API("<-XALEDArrayDeviceImpl_DoResume");
    /* This implementation does not support suspended state */
    return XA_RESULT_PRECONDITIONS_VIOLATED;
}

/* void XALEDArrayDeviceImpl_FreeResources
 * Description: Free all resources reserved at XALEDArrayDeviceImpl_DoRealize()
 */
void XALEDArrayDeviceImpl_FreeResources(XAObjectItf self)
{
#ifdef _GSTREAMER_BACKEND_  
    XAObjectItfImpl* pObj = (XAObjectItfImpl*)(*self);
    XALEDArrayDeviceImpl* pImpl = (XALEDArrayDeviceImpl*)(*self);
    XAuint8 itfIdx = 0;
    DEBUG_API("->XALEDArrayDeviceImpl_FreeResources");
    XA_IMPL_THREAD_SAFETY_ENTRY_FOR_VOID_FUNCTIONS(XATSLEDArray);
    assert( pObj && pImpl && pObj == pObj->self );

    if ( pImpl->adaptationCtx != NULL )
    {
        XALEDArrayAdapt_Destroy( pImpl->adaptationCtx );
        pImpl->adaptationCtx = NULL;
    }

    /* free all allocated interfaces */
    for(itfIdx = 0; itfIdx < LEDARRAY_ITFCOUNT; itfIdx++)
    {
        void *pItf = pObj->interfaceMap[itfIdx].pItf;
        if(pItf)
        {
            switch(itfIdx)
            {
                case LEDARRAY_LEDARRAYITF:
                    XALEDArrayItfImpl_Free( pItf );
                    break;
                case LEDARRAY_CONFIGEXTENSIONITF:
                    XAConfigExtensionsItfImpl_Free( pItf );
                    break;
                case LEDARRAY_DIMITF:
                    XADIMItfImpl_Free( pItf );
                    break;

                default:
                    break;
            }
            pObj->interfaceMap[itfIdx].pItf = NULL;
        }
    }

    if ( pImpl->adaptationCtx != NULL )
    {
        XALEDArrayAdapt_Destroy( pImpl->adaptationCtx );
        pImpl->adaptationCtx = NULL;
    }

    XA_IMPL_THREAD_SAFETY_EXIT_FOR_VOID_FUNCTIONS(XATSLEDArray);
    DEBUG_API("<-XALEDArrayDeviceImpl_FreeResources");
#endif    
    return;
}
/* END OF FILE */
