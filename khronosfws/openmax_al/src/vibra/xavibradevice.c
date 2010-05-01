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
#include "xavibradevice.h"
#include "xavibraitf.h"
#include "xaconfigextensionsitf.h"
#include "xadynintmgmtitf.h"
#include "xavibraadaptctx.h"

/* Static mapping of enumeration XAVibraDeviceInterfaces to interface iids */
static const XAInterfaceID* XAVibraDeviceItfIIDs[VIBRA_ITFCOUNT]=
{
    &XA_IID_OBJECT,
    &XA_IID_CONFIGEXTENSION,
    &XA_IID_VIBRA,
    &XA_IID_DYNAMICINTERFACEMANAGEMENT
};


/*****************************************************************************
 * Global methods
 *****************************************************************************/
/*
 * XAresult XAVibraDeviceImpl_CreateVibraDevice
 * Description: Create object
 */
XAresult XAVibraDeviceImpl_CreateVibraDevice(FrameworkMap* mapper,
                                             XAObjectItf* pDevice,
                                             XAuint32 deviceID,
                                             XAuint32 numInterfaces,
                                             const XAInterfaceID * pInterfaceIds,
                                             const XAboolean * pInterfaceRequired)
{
    XAVibraDeviceImpl* pImpl = NULL;
    XAObjectItfImpl* pBaseObj = NULL;
    XAuint8 itfIndex = 0;

    DEBUG_API("->XAVibraDeviceImpl_CreateVibraDevice");

    if( !pDevice )
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVibraDeviceImpl_CreateVibraDevice");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* instantiate object implementation */
    pImpl = (XAVibraDeviceImpl*)calloc(1,sizeof(XAVibraDeviceImpl));
    if( !pImpl )
    {
        /* memory allocation failed */
        DEBUG_ERR("XA_RESULT_MEMORY_FAILURE");
        DEBUG_API("<-XAVibraDeviceImpl_CreateVibraDevice");
        return XA_RESULT_MEMORY_FAILURE;
    }
    pBaseObj = &pImpl->baseObj;

    /* Initialize base object default implementation */
    XAObjectItfImpl_Init(pBaseObj,
                         VIBRA_ITFCOUNT,
                         XAVibraDeviceItfIIDs,
                         XAVibraDeviceImpl_DoRealize,
                         XAVibraDeviceImpl_DoResume,
                         XAVibraDeviceImpl_FreeResources);

    /* Mark interfaces that need to be exposed */
    /* Implicit and mandated interfaces */
    pBaseObj->interfaceMap[VIBRA_VIBRAITF].required = XA_BOOLEAN_TRUE;
    pBaseObj->interfaceMap[VIBRA_DIMITF].required = XA_BOOLEAN_TRUE;

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
                    DEBUG_ERR("Required interface not found - abort creation!");
                    DEBUG_API("<-XAVibraDeviceImpl_CreateVibraDevice");
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

    /* This code is put here to return Feature Not Supported since adaptation is not present*/
    /*************************************************/
    DEBUG_ERR("Required interface not found - abort creation!");
    XAObjectItfImpl_Destroy((XAObjectItf)&(pBaseObj));
    DEBUG_API("<-XAVibraDeviceImpl_CreateVibraDevice");
    return XA_RESULT_FEATURE_UNSUPPORTED;
    /*************************************************/
    
/*    // Initialize XAVibraDeviceImpl variables 
    pImpl->deviceID = deviceID;


    pImpl->adaptationCtx = XAVibraAdapt_Create(pImpl->deviceID);

     //Set ObjectItf to point to newly created object 
    *pDevice = (XAObjectItf)&(pBaseObj->self);

    DEBUG_API("<-XAVibraDeviceImpl_Create");
    return XA_RESULT_SUCCESS;*/
}
/* XAResult XAVibraDeviceImpl_QueryNumSupportedInterfaces
 * Description: Statically query number of supported interfaces
 */
XAresult XAVibraDeviceImpl_QueryNumSupportedInterfaces(XAuint32 *pNumSupportedInterfaces)
{
    DEBUG_API("->XAVibraDeviceImpl_QueryNumSupportedInterfaces");
    if( pNumSupportedInterfaces )
    {
        *pNumSupportedInterfaces = VIBRA_ITFCOUNT;

        DEBUG_API_A1("<-XAVibraDeviceImpl_QueryNumSupportedInterfaces - %ld", *pNumSupportedInterfaces );
        return XA_RESULT_SUCCESS;
    }
    else
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVibraDeviceImpl_QueryNumSupportedInterfaces");
        return XA_RESULT_PARAMETER_INVALID;
    }
}
/* XAResult XACVibraDeviceImpl_QuerySupportedInterfaces
 * Description: Statically query supported interfaces
 */
XAresult XAVibraDeviceImpl_QuerySupportedInterfaces(XAuint32 index,
                                                    XAInterfaceID *pInterfaceId)
{
    DEBUG_API("->XAVibraDeviceImpl_QuerySupportedInterfaces");

    if( index >= VIBRA_ITFCOUNT || !pInterfaceId )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVibraDeviceImpl_QuerySupportedInterfaces");
        return XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
        *pInterfaceId = *(XAVibraDeviceItfIIDs[index]);

        DEBUG_API("<-XAVibraDeviceImpl_QuerySupportedInterfaces");
        return XA_RESULT_SUCCESS;
    }
}
/*****************************************************************************
 * base object XAObjectItfImpl methods
 *****************************************************************************/
/* XAResult XAVibraDeviceImpl_DoRealize
 * Description: Realize all implicit and explicitly wanted interfaces.
 * Create and initialize implementation-specific variables.
 * Called from base object XAObjectItfImpl
 */
XAresult XAVibraDeviceImpl_DoRealize(XAObjectItf self)
{
    XAuint8 itfIdx = 0;
    XAObjectItfImpl* pObj = (XAObjectItfImpl*)(*self);
    XAVibraDeviceImpl* pObjImpl = (XAVibraDeviceImpl*)(pObj);
    XAresult ret = XA_RESULT_SUCCESS;

    DEBUG_API("->XAVibraDeviceImpl_DoRealize");

    /* check casting from correct pointer type */
    if( !pObjImpl || pObj != pObjImpl->baseObj.self )
    {
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAVibraDeviceImpl_DoRealize");
        return XA_RESULT_PARAMETER_INVALID;
    }


    ret = XAVibraAdapt_PostInit( pObjImpl->adaptationCtx );

    if( ret != XA_RESULT_SUCCESS )
    {
        DEBUG_API("<-XAVibraDeviceImpl_DoRealize");
        return ret;
    }

    /* Realize all implicit and explicitly wanted interfaces */
    for( itfIdx = 0; itfIdx < VIBRA_ITFCOUNT; itfIdx++)
    {
        if( !(pObj->interfaceMap[itfIdx].pItf) &&
            pObj->interfaceMap[itfIdx].required )
        {
            void *pItf = NULL;
            switch( itfIdx )
            {
                case VIBRA_VIBRAITF:

                    pItf = XAVibraItfImpl_Create(pObjImpl->adaptationCtx);

                    break;
                case VIBRA_CONFIGEXTENSIONITF:
                    pItf = XAConfigExtensionsItfImpl_Create();
                    break;
                case VIBRA_DIMITF:
                    pItf = XADIMItfImpl_Create();
                    break;
                default:
                    break;
            }
            if( !pItf )
            {
                /* memory allocation failed */
                DEBUG_ERR("XA_RESULT_MEMORY_FAILURE");
                DEBUG_API("<-XAVibraDeviceImpl_DoRealize");
                return XA_RESULT_MEMORY_FAILURE;
            }
            else
            {
                pObj->interfaceMap[itfIdx].pItf = pItf;
            }
        }
    }

    pObj->state = XA_OBJECT_STATE_REALIZED;
    DEBUG_API("<-XAVibraDeviceImpl_DoRealize");
    return XA_RESULT_SUCCESS;
}

/* XAResult XAVibraDeviceImpl_DoResume
 * Description: Resume object from suspended state
 */
XAresult XAVibraDeviceImpl_DoResume(XAObjectItf self)
{
    DEBUG_API("->XAVibraDeviceImpl_DoResume");
    DEBUG_API("<-XAVibraDeviceImpl_DoResume");
    /* This implementation does not support suspended state */
    return XA_RESULT_PRECONDITIONS_VIOLATED;
}
/* void XAVibraDeviceImpl_FreeResources
 * Description: Free all resources reserved at XAVibraDeviceImpl_DoRealize()
 */
void XAVibraDeviceImpl_FreeResources(XAObjectItf self)
{
    XAObjectItfImpl* pObj = (XAObjectItfImpl*) (*self);
    XAVibraDeviceImpl* pImpl = (XAVibraDeviceImpl*) (*self);
    XAuint8 itfIdx = 0;
    DEBUG_API("->XAVibraDeviceImpl_FreeResources");

    
    assert( pObj && pImpl && pObj == pObj->self );
    if (pImpl->adaptationCtx != NULL)
    {
        XAVibraAdapt_Destroy(pImpl->adaptationCtx);
        pImpl->adaptationCtx = NULL;
    }

    /* free all allocated interfaces */
    for (itfIdx = 0; itfIdx < VIBRA_ITFCOUNT; itfIdx++)
    {
        void *pItf = pObj->interfaceMap[itfIdx].pItf;
        if (pItf)
        {
            switch (itfIdx)
            {
                 case VIBRA_VIBRAITF:
                     XAVibraItfImpl_Free( pItf );
                     break;
                 case VIBRA_CONFIGEXTENSIONITF:
                     XAConfigExtensionsItfImpl_Free( pItf );
                     break;
                 case VIBRA_DIMITF:
                     XADIMItfImpl_Free( pItf );
                     break;
                 default:
                     break;
            }
            pObj->interfaceMap[itfIdx].pItf = NULL;
        }
    }

    DEBUG_API("<-XAVibraDeviceImpl_FreeResources");

    return;
}
/* END OF FILE*/
