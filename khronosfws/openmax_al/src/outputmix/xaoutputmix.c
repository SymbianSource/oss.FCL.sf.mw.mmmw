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

#include "xaoutputmix.h"
#include "xaobjectitf.h"
#include "xadynintmgmtitf.h"
#include "xaconfigextensionsitf.h"
#include "xaequalizeritf.h"
#include "xavolumeitf.h"
#include "xaoutputmixitf.h"
#include "xathreadsafety.h"
#include "xaoutputmixadaptctx.h"
#include "xacapabilitiesmgr.h"

/* Static mapping of enumeration XAOMixInterfaces to interface iids */
static const XAInterfaceID* xaOMixItfIIDs[OMIX_ITFCOUNT]={
    &XA_IID_OBJECT,
    &XA_IID_DYNAMICINTERFACEMANAGEMENT,
    &XA_IID_CONFIGEXTENSION,
    &XA_IID_OUTPUTMIX,
    &XA_IID_EQUALIZER,
    &XA_IID_VOLUME
};

/*****************************************************************************
 * Global methods
 *****************************************************************************/


/*
 *XAresult XAOMixImpl_CreateOutputMix(XAObjectItf *pMix,
 *                                    XAuint32 numInterfaces,
 *                                   const XAInterfaceID *pInterfaceIds,
 *                                   const XAboolean *pInterfaceRequired)
 * Description: Create object
 */
XAresult XAOMixImpl_CreateOutputMix(FrameworkMap* mapper,
                                    XACapabilities* capabilities,
                                    XAObjectItf *pMix,
                                    XAuint32 numInterfaces,
                                    const XAInterfaceID *pInterfaceIds,
                                    const XAboolean *pInterfaceRequired)
{

    XAObjectItfImpl* pBaseObj = NULL;
    XAOMixImpl* pImpl = NULL;
    XAuint32 itfIdx = 0;
    DEBUG_API("->XAOMixImpl_CreateOutputMix");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSOutputMix);

    
    
    if(!pMix)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSOutputMix);
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAOMixImpl_CreateOutputMix");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* instantiate object */
    pImpl = (XAOMixImpl*)calloc(1,sizeof(XAOMixImpl));
    if(!pImpl)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSOutputMix);
        /* memory allocation failed */
        DEBUG_ERR("XA_RESULT_MEMORY_FAILURE");
        DEBUG_API("<-XAOMixImpl_CreateOutputMix");
        return XA_RESULT_MEMORY_FAILURE;
    }
    pBaseObj = &pImpl->baseObj;

    XAObjectItfImpl_Init(pBaseObj,
                         OMIX_ITFCOUNT,
                         xaOMixItfIIDs,
                         XAOMixImpl_DoRealize,
                         XAOMixImpl_DoResume,
                         XAOMixImpl_FreeResources );

    /* Mark interfaces that need to be exposed */
    /* Implicit and mandated interfaces */
    pBaseObj->interfaceMap[OMIX_OUTPUTMIXITF].required = XA_BOOLEAN_TRUE;
    pBaseObj->interfaceMap[OMIX_DIMITF].required = XA_BOOLEAN_TRUE;

    /* Explicit interfaces */
    if((numInterfaces != 0) && pInterfaceIds && pInterfaceRequired)
    {
        /* Check required interfaces */
        for(itfIdx = 0; itfIdx < numInterfaces; itfIdx++)
        {
            /* If mapEntry is null then required interface is not supported.*/
            XAObjItfMapEntry *entry =
                XAObjectItfImpl_GetItfEntry((XAObjectItf)&(pBaseObj), pInterfaceIds[itfIdx]);
            if( !entry  )
            {
                if( pInterfaceRequired[itfIdx] )
                {
                    /* required interface cannot be accommodated - fail creation */
                    XAObjectItfImpl_Destroy((XAObjectItf)&(pBaseObj));
                    XA_IMPL_THREAD_SAFETY_EXIT(XATSOutputMix);
                    DEBUG_ERR("Required interface not found - abort creation!");
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
    pBaseObj->interfaceMap[OMIX_EQUALIZERITF].isDynamic = XA_BOOLEAN_TRUE;

    /* This code is put here to return Feature Not Supported since adaptation is not present*/
    /*************************************************/
    XAObjectItfImpl_Destroy((XAObjectItf)&(pBaseObj));
    XA_IMPL_THREAD_SAFETY_EXIT(XATSOutputMix);
    DEBUG_ERR("Required interface not found - abort creation!");
    return XA_RESULT_FEATURE_UNSUPPORTED;
    /*************************************************/    
    
/*    // Create outputmix adaptation context 
    pImpl->adaptationCtx = XAOutputMixAdapt_Create();
 
     //Set ObjectItf to point to newly created object 
    *pMix = (XAObjectItf)&(pBaseObj->self);

    XA_IMPL_THREAD_SAFETY_EXIT(XATSOutputMix);
   
    DEBUG_API("<-XAOMixImpl_CreateOutputMix");
    return XA_RESULT_SUCCESS;*/
}

/*
 * XAresult XAOMixImpl_QueryNumSupportedInterfaces(XAuint32 * pNumSupportedInterfaces)
 * Description: Query number of supported interfaces
 */
XAresult XAOMixImpl_QueryNumSupportedInterfaces(XAuint32 * pNumSupportedInterfaces)
{
    DEBUG_API("->XAOMixImpl_QueryNumSupportedInterfaces");
    if( pNumSupportedInterfaces )
    {
        *pNumSupportedInterfaces = OMIX_ITFCOUNT;
        DEBUG_API_A1("<-XAOMixImpl_QueryNumSupportedInterfaces - %lu", *pNumSupportedInterfaces );
        return XA_RESULT_SUCCESS;
    }
    else
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAOMixImpl_QueryNumSupportedInterfaces");
        return XA_RESULT_PARAMETER_INVALID;
    }
}

/*
 * XAresult XAOMixImpl_QuerySupportedInterfaces(XAuint32 index, XAInterfaceID * pInterfaceId)
 * Description: Statically query supported interfaces
 */
XAresult XAOMixImpl_QuerySupportedInterfaces(XAuint32 index,
                                             XAInterfaceID * pInterfaceId)
{
    DEBUG_API("->XAOMixImpl_QuerySupportedInterfaces");
    if (index >= OMIX_ITFCOUNT || !pInterfaceId)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAOMixImpl_QuerySupportedInterfaces");
        return XA_RESULT_PARAMETER_INVALID;
    }
    else
    {

        *pInterfaceId = *(xaOMixItfIIDs[index]);

        DEBUG_API("<-XAOMixImpl_QuerySupportedInterfaces");
        return XA_RESULT_SUCCESS;
    }
}

/*****************************************************************************
 * base object XAObjectItfImpl methods
 *****************************************************************************/

/* XAresult XAOMixImpl_DoRealize(XAObjectItf self)
 * Description: Realize all implicit and explicitly wanted interfaces.
 * Create and initialize implementation-specific variables.
 * Called from base object XAObjectItfImpl
 */
XAresult XAOMixImpl_DoRealize(XAObjectItf self)
{

    XAObjectItfImpl* pObj = (XAObjectItfImpl*)(*self);
    XAresult ret = XA_RESULT_SUCCESS;
    XAOMixImpl* pMixImpl = (XAOMixImpl*)(pObj);
    XAuint8 itfIdx = 0;
    DEBUG_API("->XAOMixImpl_DoRealize");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSOutputMix);

    /* check casting from correct pointer type */
    if( !pMixImpl || pObj != pMixImpl->baseObj.self )
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSOutputMix);
        /* invalid parameter */
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XAOMixImpl_DoRealize");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* Initialize adaptation */
    ret = XAOutputMixAdapt_PostInit( (XAAdaptationGstCtx*)pMixImpl->adaptationCtx );
    if( ret != XA_RESULT_SUCCESS )
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSOutputMix);
        DEBUG_API("<-XAOMixImpl_DoRealize");
        return ret;
    }

    /* Realize all implicit and explicitly wanted interfaces */
    for( itfIdx = 0; itfIdx < OMIX_ITFCOUNT; itfIdx++)
    {
        if( !(pObj->interfaceMap[itfIdx].pItf) && pObj->interfaceMap[itfIdx].required )
        {
            void *pItf = NULL;
            switch( itfIdx )
            {
                case OMIX_CONFIGEXTENSIONITF:
                    pItf = XAConfigExtensionsItfImpl_Create();
                    break;
                case OMIX_DIMITF:
                    pItf = XADIMItfImpl_Create();
                    if(pItf)
                    {
                        XADIMItfImpl_Init(pItf, self,
                                          XAOMixImpl_DoAddItf,
                                          XAOMixImpl_DoResumeItf,
                                          XAOMixImpl_DoRemoveItf);
                    }
                    break;
                  case OMIX_EQUALIZERITF:
                      pItf = XAEqualizerItfImpl_Create(pMixImpl->adaptationCtx);
                      break;
                  case OMIX_VOLUMEITF:
                      pItf = XAVolumeItfImpl_Create(pMixImpl->adaptationCtx);
                      break;
                  case OMIX_OUTPUTMIXITF:
                      pItf = XAOutputMixItfImpl_Create(pMixImpl->adaptationCtx);
                      break;
                  default:
                      break;
            }
            if( !pItf )
            {
                XA_IMPL_THREAD_SAFETY_EXIT(XATSOutputMix);
                DEBUG_ERR("XA_RESULT_MEMORY_FAILURE");
                /* memory allocation failed */
                DEBUG_API("<-XAOMixImpl_DoRealize");
                return XA_RESULT_MEMORY_FAILURE;
            }
            else
            {
                pObj->interfaceMap[itfIdx].pItf = pItf;
            }
        }
    }

    pObj->state = XA_OBJECT_STATE_REALIZED;
    XA_IMPL_THREAD_SAFETY_EXIT(XATSOutputMix);
    
    DEBUG_API("<-XAOMixImpl_DoRealize");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XAOMixImpl_DoResume(XAObjectItf self)
 * Description: Resume object from suspended state
 */
XAresult XAOMixImpl_DoResume(XAObjectItf self)
{
    DEBUG_API("->XAOMixImpl_DoResume");
    DEBUG_API("<-XAOMixImpl_DoResume");
    /* This implementation does not support suspended state */
    return XA_RESULT_PRECONDITIONS_VIOLATED;
}

/*
 * void XAOMixImpl_FreeResources(XAObjectItf self)
 * Description: Free all resources reserved at XACameraDeviceImpl_DoRealize()
 */
void XAOMixImpl_FreeResources(XAObjectItf self)
{

    XAObjectItfImpl* pObj = (XAObjectItfImpl*)(*self);
    XAOMixImpl* pImpl = (XAOMixImpl*)(*self);
    XAuint8 itfIdx = 0;
    DEBUG_API("->XAOMixImpl_FreeResources");
    XA_IMPL_THREAD_SAFETY_ENTRY_FOR_VOID_FUNCTIONS(XATSOutputMix);
    assert( pObj && pImpl && pObj == pObj->self );

    if ( pImpl->adaptationCtx != NULL )
    {
        XAOutputMixAdapt_Destroy( (XAAdaptationGstCtx*)pImpl->adaptationCtx );
        pImpl->adaptationCtx = NULL;
    }

    /* free all allocated interfaces */
    for(itfIdx = 0; itfIdx < OMIX_ITFCOUNT; itfIdx++)
    {
        void *pItf = pObj->interfaceMap[itfIdx].pItf;
        if(pItf)
        {
            switch(itfIdx)
            {
               case OMIX_CONFIGEXTENSIONITF:
                   XAConfigExtensionsItfImpl_Free( pItf );
                   break;
               case OMIX_DIMITF:
                    XADIMItfImpl_Free( pItf );
                    break;
               case OMIX_EQUALIZERITF:
                   XAEqualizerItfImpl_Free(pItf);
                   break;
               case OMIX_VOLUMEITF:
                   XAVolumeItfImpl_Free(pItf);
                   break;
               case OMIX_OUTPUTMIXITF:
                   XAOutputMixItfImpl_Free( pItf );
                   break;
               default:
                   break;
             }
             pObj->interfaceMap[itfIdx].pItf = NULL;
         }
     }

    if ( pImpl->adaptationCtx != NULL )
    {
        XAOutputMixAdapt_Destroy( (XAAdaptationGstCtx*)pImpl->adaptationCtx );
        pImpl->adaptationCtx = NULL;
    }

    XA_IMPL_THREAD_SAFETY_EXIT_FOR_VOID_FUNCTIONS(XATSOutputMix);
    
    DEBUG_API("<-XAOMixImpl_FreeResources");
}

/*****************************************************************************
 * MediaRecorderImpl -specific methods
 *****************************************************************************/

/* XAOMixImpl_DoAddItf
 * Dynamically add an interface, object specific parts
 */
XAresult XAOMixImpl_DoAddItf(XAObjectItf self, XAObjItfMapEntry *mapEntry  )
{

    XAObjectItfImpl* pObj = (XAObjectItfImpl*)(*self);
    XAOMixImpl* pImpl = (XAOMixImpl*)(pObj);

    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XAOMixImpl_DoAddItf");
    if(mapEntry)
    {
        switch( mapEntry->mapIdx )
        {
        case OMIX_EQUALIZERITF:

            mapEntry->pItf = XAEqualizerItfImpl_Create( pImpl->adaptationCtx );

            break;
        default:
            DEBUG_ERR("XAOMixImpl_DoAddItf unknown id");
            ret = XA_RESULT_FEATURE_UNSUPPORTED;
            break;
        }
        if( !mapEntry->pItf && ret == XA_RESULT_SUCCESS)
        {
            DEBUG_ERR("XAOMixImpl_DoAddItf itf creation failed");
            ret = XA_RESULT_MEMORY_FAILURE;
        }
    }
    else
    {
        ret = XA_RESULT_PARAMETER_INVALID;
    }


    DEBUG_API("<-XAOMixImpl_DoAddItf");
    return ret;
}

/* XAOMixImpl_DoResumeItf
 * Try to resume lost interface, object specific parts
 */
XAresult XAOMixImpl_DoResumeItf(XAObjectItf self, XAObjItfMapEntry *mapEntry  )
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XAOMixImpl_DoResumeItf");
    /* For now, no difference between suspended and unrealised itfs */
    ret = XAOMixImpl_DoAddItf(self,mapEntry);
    DEBUG_API("<-XAOMixImpl_DoResumeItf");
    return ret;
}

/* XAOMixImpl_DoRemoveItf
 * Dynamically remove an interface, object specific parts
 */
XAresult XAOMixImpl_DoRemoveItf(XAObjectItf self, XAObjItfMapEntry *mapEntry )
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XAOMixImpl_DoRemoveItf");

    if(mapEntry)
    {
        switch( mapEntry->mapIdx )
        {
        case OMIX_EQUALIZERITF:
            XAEqualizerItfImpl_Free( mapEntry->pItf );
            break;
        default:
            DEBUG_ERR("XAOMixImpl_DoRemoveItf unknown id");
            ret = XA_RESULT_FEATURE_UNSUPPORTED;
            break;
        }
        mapEntry->pItf = NULL;
    }
    else
    {
        ret = XA_RESULT_PARAMETER_INVALID;
    }

    DEBUG_API("<-XAOMixImpl_DoRemoveItf");
    return ret;
}

/*END OF FILE*/

