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
#include "xardsitf.h"
#include "xardsitfadaptation.h"
#include "xathreadsafety.h"
#include "xaadaptationgst.h"
/**
 * XARDSItfImpl* GetImpl(XARDSItf self)
 * Description: Validated interface pointer and cast it to implementations pointer.
 **/
static XARDSItfImpl* GetImpl(XARDSItf self)
{
    if(self)
    {
        XARDSItfImpl *impl = (XARDSItfImpl*)(*self);
        if(impl && impl == impl->self)
        {
            return impl;
        }
    }
    return NULL;
}

/**
 * Base interface XARDSItf implementation
 */

/**
 * XAresult XARDSItfImpl_QueryRDSSignal(XARDSItf self, XAboolean * isSignal)
 * Description: Returns the status of the RDS reception.
 **/
XAresult XARDSItfImpl_QueryRDSSignal(XARDSItf self, XAboolean * isSignal)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);
    DEBUG_API("->XARDSItfImpl_QueryRDSSignal");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSRadio);
    if(!impl || !isSignal)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_QueryRDSSignal");
        return XA_RESULT_PARAMETER_INVALID;
    }


//    ret = XARDSItfAdapt_QueryRDSSignal((XAAdaptationGstCtx*)impl->adapCtx, isSignal);

    XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
    DEBUG_API("<-XARDSItfImpl_QueryRDSSignal");
    return ret;
}

/**
 * XAresult XARDSItfImpl_GetProgrammeServiceName(XARDSItf self, XAchar * ps)
 * Description: Gets the current Programme Service name (PS).
 **/
XAresult XARDSItfImpl_GetProgrammeServiceName(XARDSItf self, XAchar * ps)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);
    DEBUG_API("->XARDSItfImpl_GetProgrammeServiceName");
    if(!impl || !ps)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_GetProgrammeServiceName");
        return XA_RESULT_PARAMETER_INVALID;
    }


//    ret = XARDSItfAdapt_GetProgrammeServiceName((XAAdaptationGstCtx*)impl->adapCtx, ps);

    DEBUG_API("<-XARDSItfImpl_GetProgrammeServiceName");
    return ret;
}

/**
 * XAresult XARDSItfImpl_GetRadioText(XARDSItf self, XAchar * rt)
 * Description: Gets the current Radio Text (RT).
 **/
XAresult XARDSItfImpl_GetRadioText(XARDSItf self, XAchar * rt)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);
    DEBUG_API("->XARDSItfImpl_GetRadioText");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSRadio);

    if(!impl || !rt)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_GetRadioText");
        return XA_RESULT_PARAMETER_INVALID;
    }


//    ret = XARDSItfAdapt_GetRadioText((XAAdaptationGstCtx*)impl->adapCtx, rt);

    XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
    DEBUG_API("<-XARDSItfImpl_GetRadioText");
    return ret;
}

/**
 * XAresult XARDSItfImpl_GetRadioTextPlus(XARDSItf self,
 *                                        XAuint8 contentType,
 *                                        XAchar * informationElement,
 *                                        XAchar * descriptor,
 *                                        XAuint8 * descriptorContentType)
 * Description: Gets the current Radio Text+ (RT+) information element based
 *              on the given class code.
 **/
XAresult XARDSItfImpl_GetRadioTextPlus(XARDSItf self,
                                       XAuint8 contentType,
                                       XAchar * informationElement,
                                       XAchar * descriptor,
                                       XAuint8 * descriptorContentType)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);
    DEBUG_API("->XARDSItfImpl_GetRadioTextPlus");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSRadio);

    if(!impl || contentType < XA_RDSRTPLUS_ITEMTITLE || contentType > XA_RDSRTPLUS_GETDATA
            || !informationElement || !descriptor || !descriptorContentType)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_GetRadioTextPlus");
        return XA_RESULT_PARAMETER_INVALID;
    }


 //   ret = XARDSItfAdapt_GetRadioTextPlus((XAAdaptationGstCtx*)impl->adapCtx, contentType, informationElement,
 //                                       descriptor, descriptorContentType);

    XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
    DEBUG_API("<-XARDSItfImpl_GetRadioTextPlus");
    return ret;
}

/**
 * XAresult XARDSItfImpl_GetProgrammeType(XARDSItf self, XAuint32 * pty)
 * Description: Gets the current Programme TYpe (PTY) as short. The
 *              return value zero corresponds to No Programme Type
 *              or to undefined type. Please note that PTYs in RBDS
 *              differ from the ones in RDS.
 **/
XAresult XARDSItfImpl_GetProgrammeType(XARDSItf self, XAuint32 * pty)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);
    DEBUG_API("->XARDSItfImpl_GetProgrammeType");

    if(!impl || !pty)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_GetProgrammeType");
        return XA_RESULT_PARAMETER_INVALID;
    }


//    ret = XARDSItfAdapt_GetProgrammeType((XAAdaptationGstCtx*)impl->adapCtx, pty);

    DEBUG_API("<-XARDSItfImpl_GetProgrammeType");
    return ret;
}

/**
 * XAresult XARDSItfImpl_GetProgrammeTypeString(XARDSItf self,
 *                                              XAboolean isLengthMax16,
 *                                              XAchar * pty)
 * Description: Gets the current Programme TYpe (PTY) as a String with
 *              the maximum of 8 or 16 characters in English (char set TBD)
 *              as defined in RDS and RBDS specifications. Please note
 *              that PTYs in RBDS differ from the ones in RDS.
 **/
XAresult XARDSItfImpl_GetProgrammeTypeString(XARDSItf self,
                                             XAboolean isLengthMax16,
                                             XAchar * pty)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);
    DEBUG_API("->XARDSItfImpl_GetProgrammeTypeString");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSRadio);

    if(!impl || !pty)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_GetProgrammeTypeString");
        return XA_RESULT_PARAMETER_INVALID;
    }


//    ret = XARDSItfAdapt_GetProgrammeTypeString((XAAdaptationGstCtx*)impl->adapCtx, isLengthMax16, pty);

    XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
    DEBUG_API("<-XARDSItfImpl_GetProgrammeTypeString");
    return ret;
}

/**
 * XAresult XARDSItfImpl_GetProgrammeIdentificationCode(XARDSItf self, XAint16 * pi)
 * Description: Gets the current Programme Identification code (PI). The PI is not
 *              intended for directly displaying to the end user, but instead to
 *              identify uniquely a programme. This can be used to detect that two
 *              frequencies are transmitting the same programme.
 **/
XAresult XARDSItfImpl_GetProgrammeIdentificationCode(XARDSItf self, XAint16 * pi)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);
    DEBUG_API("->XARDSItfImpl_GetProgrammeIdentificationCode");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSRadio);

    if(!impl || !pi)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_GetProgrammeIdentificationCode");
        return XA_RESULT_PARAMETER_INVALID;
    }


//    ret = XARDSItfAdapt_GetProgrammeIdentificationCode((XAAdaptationGstCtx*)impl->adapCtx, pi);

    XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
    DEBUG_API("<-XARDSItfImpl_GetProgrammeIdentificationCode");
    return ret;
}

/**
 * XAresult XARDSItfImpl_GetClockTime(XARDSItf self, XAtime * dateAndTime)
 * Description: Gets the current Clock Time and date (CT).
 **/
XAresult XARDSItfImpl_GetClockTime(XARDSItf self, XAtime * dateAndTime)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);
    DEBUG_API("->XARDSItfImpl_GetClockTime");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSRadio);

    if(!impl || !dateAndTime)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_GetClockTime");
        return XA_RESULT_PARAMETER_INVALID;
    }


 //   ret = XARDSItfAdapt_GetClockTime((XAAdaptationGstCtx*)impl->adapCtx, dateAndTime);

    XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
    DEBUG_API("<-XARDSItfImpl_GetClockTime");
    return ret;
}

/**
 * XAresult XARDSItfImpl_GetTrafficAnnouncement(XARDSItf self, XAboolean * ta)
 * Description: Gets the current status of the Traffic Announcement (TA) switch.
 **/
XAresult XARDSItfImpl_GetTrafficAnnouncement(XARDSItf self, XAboolean * ta)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);
    DEBUG_API("->XARDSItfImpl_GetTrafficAnnouncement");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSRadio);

    if(!impl || !ta)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_GetTrafficAnnouncement");
        return XA_RESULT_PARAMETER_INVALID;
    }


//    ret = XARDSItfAdapt_GetTrafficAnnouncement((XAAdaptationGstCtx*)impl->adapCtx, ta);

    XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
    DEBUG_API("<-XARDSItfImpl_GetTrafficAnnouncement");
    return ret;
}

/**
 * XAresult XARDSItfImpl_GetTrafficProgramme(XARDSItf self, XAboolean * tp)
 * Description: Gets the current status of the Traffic Programme (TP) switch.
 **/
XAresult XARDSItfImpl_GetTrafficProgramme(XARDSItf self, XAboolean * tp)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);
    DEBUG_API("->XARDSItfImpl_GetTrafficProgramme");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSRadio);

    if(!impl || !tp)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_GetTrafficProgramme");
        return XA_RESULT_PARAMETER_INVALID;
    }


//    ret = XARDSItfAdapt_GetTrafficProgramme((XAAdaptationGstCtx*)impl->adapCtx, tp);

    XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
    DEBUG_API("<-XARDSItfImpl_GetTrafficProgramme");
    return ret;
}

/**
 * XAresult XARDSItfImpl_SeekByProgrammeType(XARDSItf self,
 *                                           XAuint32 pty,
 *                                           XAboolean upwards)
 * Description: Seeks for the frequency sending the given Programme TYpe (PTY).
 *              If the end of the tuner’s frequency band is reached before the
 *              given Programme TYpe is found, the scan continues from the other
 *              end until the given Programme TYpe is found or the original frequency
 *              is reached. Asynchronous - tuner callback xaRadioCallback() and
 *              XA_RADIO_EVENT_SEEK_COMPLETED is used for notifying of the result.
 *              StopSeeking() method of XARadioItf can be used to abort an ongoing seek.
 **/
XAresult XARDSItfImpl_SeekByProgrammeType(XARDSItf self,
                                          XAuint32 pty,
                                          XAboolean upwards)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);
    DEBUG_API("->XARDSItfImpl_SeekByProgrammeType");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSRadio);
    if(!impl || pty > XA_RDSPROGRAMMETYPE_RDSPTY_ALARM)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_SeekByProgrammeType");
        return XA_RESULT_PARAMETER_INVALID;
    }


 //   ret = XARDSItfAdapt_SeekByProgrammeType((XAAdaptationGstCtx*)impl->adapCtx, pty, upwards);

    XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
    DEBUG_API("<-XARDSItfImpl_SeekByProgrammeType");
    return ret;
}

/**
 * XAresult XARDSItfImpl_SeekTrafficAnnouncement(XARDSItf self, XAboolean upwards)
 * Description: Seeks for a frequency sending Traffic Announcement (TA). If the end
 *              of the tuner’s frequency band is reached before a Traffic Announcement
 *              is found, the scan continues from the other end until a Traffic
 *              Announcement is found or the original frequency is reached. Asynchronous
 *              - tuner callback xaRadioCallback() and XA_RADIO_EVENT_SEEK_COMPLETED
 *              is used for notifying of the result. StopSeeking() method of XARadioItf
 *              can be used to abort an ongoing seek.
 **/
XAresult XARDSItfImpl_SeekTrafficAnnouncement(XARDSItf self, XAboolean upwards)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);
    DEBUG_API("->XARDSItfImpl_SeekTrafficAnnouncement");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSRadio);
    if(!impl)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_SeekTrafficAnnouncement");
        return XA_RESULT_PARAMETER_INVALID;
    }


 //   ret = XARDSItfAdapt_SeekTrafficAnnouncement((XAAdaptationGstCtx*)impl->adapCtx, upwards);

    XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
    DEBUG_API("<-XARDSItfImpl_SeekTrafficAnnouncement");
    return ret;
}

/**
 * XAresult XARDSItfImpl_SeekTrafficProgramme(XARDSItf self, XAboolean upwards)
 * Description: Seeks for a frequency sending Traffic Programme (TP). If the end
 *              of the tuner’s frequency band is reached before a Traffic Programme
 *              is found, the scan continues from the other end until a Traffic
 *              Programme is found or the original frequency is reached. Asynchronous
 *              - tuner callback xaRadioCallback() and XA_RADIO_EVENT_SEEK_COMPLETED
 *              is used for notifying of the result. StopSeeking() method of XARadioItf
 *              can be used to abort an ongoing seek.
 **/
XAresult XARDSItfImpl_SeekTrafficProgramme(XARDSItf self, XAboolean upwards)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);
    DEBUG_API("->XARDSItfImpl_SeekTrafficProgramme");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSRadio);
    if(!impl)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_SeekTrafficProgramme");
        return XA_RESULT_PARAMETER_INVALID;
    }


 //   ret = XARDSItfAdapt_SeekTrafficProgramme((XAAdaptationGstCtx*)impl->adapCtx, upwards);

    XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
    DEBUG_API("<-XARDSItfImpl_SeekTrafficProgramme");
    return ret;
}

/**
 * XAresult XARDSItfImpl_SetAutomaticSwitching(XARDSItf self, XAboolean automatic)
 * Description: Sets the automatic switching of the transmitter in the case of a
 *              stronger transmitter with the same PI presence. Based on AF and/or
 *              EON fields. Please note that NOT ALL IMPLEMENTATIONS SUPPORT THIS
 *              FUNCTIONALITY.
 **/
XAresult XARDSItfImpl_SetAutomaticSwitching(XARDSItf self, XAboolean automatic)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);
    DEBUG_API("->XARDSItfImpl_SetAutomaticSwitching");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSRadio);
    if(!impl)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_SetAutomaticSwitching");
        return XA_RESULT_PARAMETER_INVALID;
    }


 //   ret = XARDSItfAdapt_SetAutomaticSwitching((XAAdaptationGstCtx*)impl->adapCtx, automatic);

    XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
    DEBUG_API("<-XARDSItfImpl_SetAutomaticSwitching");
    return ret;
}

/**
 * XAresult XARDSItfImpl_GetAutomaticSwitching(XARDSItf self, XAboolean * automatic)
 * Description: Gets the mode of the automatic switching of the transmitter in
 *              case of a stronger transmitter with the same PI presence.
 **/
XAresult XARDSItfImpl_GetAutomaticSwitching(XARDSItf self, XAboolean * automatic)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);
    DEBUG_API("->XARDSItfImpl_GetAutomaticSwitching");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSRadio);
    if(!impl || !automatic)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_GetAutomaticSwitching");
        return XA_RESULT_PARAMETER_INVALID;
    }


//    ret = XARDSItfAdapt_GetAutomaticSwitching((XAAdaptationGstCtx*)impl->adapCtx, automatic);

    XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
    DEBUG_API("<-XARDSItfImpl_GetAutomaticSwitching");
    return ret;
}

/**
 * XAresult XARDSItfImpl_SetAutomaticTrafficAnnouncement(XARDSItf self, XAboolean automatic)
 * Description: Sets the automatic switching of the program in case of the presence
 *              of Traffic Announcement in another program. Based on TP and TA fields.
 *              Please note that NOT ALL IMPLEMENTATIONS SUPPORT THIS FUNCTIONALITY.
 **/
XAresult XARDSItfImpl_SetAutomaticTrafficAnnouncement(XARDSItf self, XAboolean automatic)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);
    DEBUG_API("->XARDSItfImpl_SetAutomaticTrafficAnnouncement");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSRadio);
    if(!impl)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_SetAutomaticTrafficAnnouncement");
        return XA_RESULT_PARAMETER_INVALID;
    }


//    ret = XARDSItfAdapt_SetAutomaticTrafficAnnouncement((XAAdaptationGstCtx*)impl->adapCtx, automatic);

    XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
    DEBUG_API("<-XARDSItfImpl_SetAutomaticTrafficAnnouncement");
    return ret;
}

/**
 * XAresult XARDSItfImpl_GetAutomaticTrafficAnnouncement(XARDSItf self, XAboolean * automatic)
 * Description: Gets the mode of the automatic switching of the program in case of
 *              the presence of Traffic Announcement in another program. Based on
 *              TP and TA fields.
 **/
XAresult XARDSItfImpl_GetAutomaticTrafficAnnouncement(XARDSItf self, XAboolean * automatic)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);
    DEBUG_API("->XARDSItfImpl_GetAutomaticTrafficAnnouncement");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSRadio);
    if(!impl || !automatic)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_GetAutomaticTrafficAnnouncement");

        return XA_RESULT_PARAMETER_INVALID;
    }


 //   ret = XARDSItfAdapt_GetAutomaticTrafficAnnouncement((XAAdaptationGstCtx*)impl->adapCtx, automatic);

    XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
    DEBUG_API("<-XARDSItfImpl_GetAutomaticTrafficAnnouncement");
    return ret;
}

/**
 * XAresult XARDSItfImpl_GetODAGroup(XARDSItf self,
 *                                   XAint16 AID,
 *                                   xaGetODAGroupCallback callback,
 *                                   void * pContext)
 * Description: Returns asynchronously via callback (xaGetODAGroupCallback())
 *              the application Group and the message bits concerning the given
 *              ODA (Open Data Application). ODA is a mechanism that a broadcaster
 *              can use to transfer data that is not explicitly specified in the RDS
 *              standard. Open Data Applications are subject to a registration process.
 *              Transmission protocols used by ODAs may be public or private. See RDS
 *              Forum web page (http://www.rds.org.uk/)for details.
 **/
XAresult XARDSItfImpl_GetODAGroup(XARDSItf self,
                                  XAuint16 AID,
                                  xaGetODAGroupCallback callback,
                                  void * pContext)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);
    DEBUG_API("->XARDSItfImpl_GetODAGroup");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSRadio);
    if(!impl)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_GetODAGroup");
        return XA_RESULT_PARAMETER_INVALID;
    }

    impl->odaGroupCallback = callback;
    impl->odaGroupContext = pContext;
    impl->odaGroupCbPtrToSelf = self;

    if (callback)
    {

 //       ret = XARDSItfAdapt_GetODAGroup((XAAdaptationGstCtx*)impl->adapCtx, AID);

    }

    XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
    DEBUG_API("<-XARDSItfImpl_GetODAGroup");

    return ret;
}

/**
 * XAresult XARDSItfImpl_SubscribeODAGroup(XARDSItf self,
 *                                         XAint16 group,
 *                                         XAboolean useErrorCorrection)
 * Description: Subscribes the given ODA group. If the given group was already
 *              subscribed, this call doesn’t do anything. Only new data in groups
 *              that have been subscribed will cause a newODA callback.
 **/
XAresult XARDSItfImpl_SubscribeODAGroup(XARDSItf self,
                                        XAint16 group,
                                        XAboolean useErrorCorrection)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);

    DEBUG_API("->XARDSItfImpl_SubscribeODAGroup");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSRadio);

    if(!impl)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_SubscribeODAGroup");
        return XA_RESULT_PARAMETER_INVALID;
    }


//    ret = XARDSItfAdapt_SubscribeODAGroup((XAAdaptationGstCtx*)impl->adapCtx, group, useErrorCorrection);

    XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
    DEBUG_API("<-XARDSItfImpl_SubscribeODAGroup");
    return ret;
}

/**
 * XAresult XARDSItfImpl_UnsubscribeODAGroup(XARDSItf self, XAint16 group)
 * Description: Unsubscribes the given ODA group. If the given group has not
 *              been subscribed, this doesn’t do anything. Only new data in
 *              groups that have been subscribed will cause a newODA callback.
 **/
XAresult XARDSItfImpl_UnsubscribeODAGroup(XARDSItf self, XAint16 group)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);

    DEBUG_API("->XARDSItfImpl_UnsubscribeODAGroup");
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSRadio);

    if(!impl)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_UnsubscribeODAGroup");

        return XA_RESULT_PARAMETER_INVALID;
    }


 //   ret = XARDSItfAdapt_UnsubscribeODAGroup((XAAdaptationGstCtx*)impl->adapCtx, group);

    XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
    DEBUG_API("<-XARDSItfImpl_UnsubscribeODAGroup");
    return ret;
}

/**
 * XAresult XARDSItfImpl_ListODAGroupSubscriptions(XARDSItf self,
 *                                                 XAint16* pGroups,
 *                                                 XAuint32* pLength);
 * Description: Lists ODA groups that are currently subscribed.
 **/
XAresult XARDSItfImpl_ListODAGroupSubscriptions(XARDSItf self,
                                                XAint16* pGroups,
                                                XAuint32* pLength)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);
    XA_IMPL_THREAD_SAFETY_ENTRY(XATSRadio);
    DEBUG_API("->XARDSItfImpl_ListODAGroupSubscriptions");

    if(!impl || !pLength)
    {
        XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_ListODAGroupSubscriptions");
        return XA_RESULT_PARAMETER_INVALID;
    }


//    XARDSItfAdapt_ListODAGroupSubscriptions((XAAdaptationGstCtx*)impl->adapCtx, pGroups, pLength);

    XA_IMPL_THREAD_SAFETY_EXIT(XATSRadio);
    DEBUG_API("<-XARDSItfImpl_ListODAGroupSubscriptions");
    return ret;
}

/**
 * XAresult XARDSItfImpl_RegisterRDSCallback(XARDSItf self,
 *                                           xaRDSCallback callback,
 *                                           void * pContext)
 * Description: Sets or clears the xaNewODADataCallback(). xaNewODADataCallback()
 *              is used tranfer the actual ODA data to the application.
 **/
XAresult XARDSItfImpl_RegisterRDSCallback(XARDSItf self,
                                          xaRDSCallback callback,
                                          void * pContext)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);
    DEBUG_API("->XARDSItfImpl_RegisterRDSCallback");

    if(!impl)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_RegisterRDSCallback");
        return XA_RESULT_PARAMETER_INVALID;
    }

    impl->rdsCallback = callback;
    impl->rdsContext = pContext;
    impl->rdsCbPtrToSelf = self;

    DEBUG_API("<-XARDSItfImpl_RegisterRDSCallback");
    return ret;
}

/**
 * XAresult XARDSItfImpl_RegisterODADataCallback(XARDSItf self,
 *                                               xaNewODADataCallback callback,
 *                                               void * pContext)
 * Description: Sets or clears the xaRDSCallback(). xaRDSCallback() is used to monitor
 *              changes in RDS fields.
 **/
XAresult XARDSItfImpl_RegisterODADataCallback(XARDSItf self,
                                              xaNewODADataCallback callback,
                                              void * pContext)
{

    XAresult ret = XA_RESULT_SUCCESS;
    XARDSItfImpl* impl = GetImpl(self);
    DEBUG_API("->XARDSItfImpl_RegisterODADataCallback");

    if(!impl)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        /* invalid parameter */
        DEBUG_API("<-XARDSItfImpl_RegisterODADataCallback");
        return XA_RESULT_PARAMETER_INVALID;
    }

    impl->odaDataCallback = callback;
    impl->odaDataContext = pContext;
    impl->odaDataCbPtrToSelf = self;

    DEBUG_API("<-XARDSItfImpl_RegisterODADataCallback");
    return ret;
}

/**
 * XARDSItfImpl -specific methods
 **/


/**
 * XARDSItfImplImpl* XARDSItfImpl_Create()
 * @return  XARDSItfImplImpl* - Pointer to  RDSItf interface implementation
 **/
XARDSItfImpl* XARDSItfImpl_Create(XAAdaptationBaseCtx *adapCtx)
{
    XARDSItfImpl *self = (XARDSItfImpl*)
        calloc(1,sizeof(XARDSItfImpl));
    DEBUG_API("->XARDSItfImpl_Create");
    if(self)
    {
        /* init itf default implementation */
        self->itf.QueryRDSSignal = XARDSItfImpl_QueryRDSSignal;
        self->itf.GetProgrammeServiceName = XARDSItfImpl_GetProgrammeServiceName;
        self->itf.GetRadioText = XARDSItfImpl_GetRadioText;
        self->itf.GetRadioTextPlus = XARDSItfImpl_GetRadioTextPlus;
        self->itf.GetProgrammeType = XARDSItfImpl_GetProgrammeType;
        self->itf.GetProgrammeTypeString = XARDSItfImpl_GetProgrammeTypeString;
        self->itf.GetProgrammeIdentificationCode = XARDSItfImpl_GetProgrammeIdentificationCode;
        self->itf.GetClockTime = XARDSItfImpl_GetClockTime;
        self->itf.GetTrafficAnnouncement = XARDSItfImpl_GetTrafficAnnouncement;
        self->itf.GetTrafficProgramme = XARDSItfImpl_GetTrafficProgramme;
        self->itf.SeekByProgrammeType = XARDSItfImpl_SeekByProgrammeType;
        self->itf.SeekTrafficAnnouncement = XARDSItfImpl_SeekTrafficAnnouncement;
        self->itf.SeekTrafficProgramme = XARDSItfImpl_SeekTrafficProgramme;
        self->itf.SetAutomaticSwitching = XARDSItfImpl_SetAutomaticSwitching;
        self->itf.GetAutomaticSwitching = XARDSItfImpl_GetAutomaticSwitching;
        self->itf.SetAutomaticTrafficAnnouncement = XARDSItfImpl_SetAutomaticTrafficAnnouncement;
        self->itf.GetAutomaticTrafficAnnouncement = XARDSItfImpl_GetAutomaticTrafficAnnouncement;
        self->itf.GetODAGroup = XARDSItfImpl_GetODAGroup;
        self->itf.SubscribeODAGroup = XARDSItfImpl_SubscribeODAGroup;
        self->itf.UnsubscribeODAGroup = XARDSItfImpl_UnsubscribeODAGroup;
        self->itf.ListODAGroupSubscriptions = XARDSItfImpl_ListODAGroupSubscriptions;
        self->itf.RegisterRDSCallback = XARDSItfImpl_RegisterRDSCallback;
        self->itf.RegisterODADataCallback = XARDSItfImpl_RegisterODADataCallback;

        /* init variables */

        self->odaDataCallback = NULL;
        self->odaDataContext = NULL;
        self->odaDataCbPtrToSelf = NULL;
        self->odaGroupCallback = NULL;
        self->odaGroupContext = NULL;
        self->odaGroupCbPtrToSelf = NULL;
        self->rdsCallback = NULL;
        self->rdsContext = NULL;
        self->rdsCbPtrToSelf = NULL;

        XAAdaptationBase_AddEventHandler( adapCtx, &XARDSItfImpl_AdaptCb, XA_RDSITFEVENTS, self );

        self->adapCtx = adapCtx;

        self->self = self;
    }

    DEBUG_API("<-XARDSItfImpl_Create");
    return self;
}

/* void XARDSItfImpl_AdaptCb( void *pHandlerCtx, XAAdaptEvent *event )
 * @param void *pHandlerCtx - pointer to cb context (XARadioItfImpl)
 * @param XAAdaptEvent *event  - Event
 * Description: Event handler for adaptation events
 */
void XARDSItfImpl_AdaptCb( void *pHandlerCtx, XAAdaptEvent *event )
{
    XARDSItfImpl* impl =(XARDSItfImpl*)pHandlerCtx;
    DEBUG_API("->XARDSItfImpl_AdaptCb");

    if(!impl)
    {
        DEBUG_ERR("XARadioItfImpl_AdaptCb, invalid context pointer!");
        DEBUG_API("<-XARDSItfImpl_AdaptCb");
        return;
    }
    assert(event);

    if( event->eventid == XA_ADAPT_RDS_GET_ODA_GROUP_DONE && impl->odaGroupCallback )
    {
        /* stubbed (enough for emulation purposes) */
        /* XA_BOOLEAN_FALSE and -1 means oda group not found */
        impl->odaGroupCallback( (XARadioItf) impl->odaGroupCbPtrToSelf, impl->odaGroupContext, XA_BOOLEAN_FALSE, -1, 0 );
    }
    DEBUG_API("<-XARDSItfImpl_AdaptCb");
}

/**
 * void XARDSItfImpl_Free(XARDSItfImpl* self)
 * @param  XARDSItfImpl* self -
 **/
void XARDSItfImpl_Free(XARDSItfImpl* self)
{
    DEBUG_API("->XARDSItfImpl_Free");
    XA_IMPL_THREAD_SAFETY_ENTRY_FOR_VOID_FUNCTIONS(XATSRadio);
    assert(self==self->self);

    XAAdaptationBase_RemoveEventHandler( self->adapCtx, &XARDSItfImpl_AdaptCb );

    XARDSItfAdapt_Free(self->adapCtx);

    free(self);
    XA_IMPL_THREAD_SAFETY_EXIT_FOR_VOID_FUNCTIONS(XATSRadio);
    DEBUG_API("<-XARDSItfImpl_Free");
}

