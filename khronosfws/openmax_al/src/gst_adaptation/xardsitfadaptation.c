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

#include <unistd.h>
#include <time.h>
#include <string.h>

#include "xaadaptationgst.h"
#include "xaradioadaptctx.h"
#include "xardsitfadaptation.h"
#include "xaradioitfadaptation.h"

/* emulated device data and constraints */

#define NUM_OF_CHANNELS 2
#define MAX_PS_STRING_LENGHT 17
#define MAX_RT_STRING_LENGHT 129
#define MAX_ODA_GROUP_SUBSCRIPTIONS 100
#define MAX_PTY_STRING_SHORT_LENGHT 17
#define MAX_PTY_STRING_LONG_LENGHT 33
#define NONE_STRING_LENGHT 4

/* emulated rds data structure */
typedef struct
{
    XAuint32 frequency;
    XAchar *progServiceName;
    XAchar *radioText;
    XAuint32 programmeType;
    XAchar *ptyStringShort;
    XAchar *ptyStringLong;
    XAint16 programmeID;
    XAboolean trafficAnnouncement;
    XAboolean trafficProgramme;
} RDSData;

#define TRAFFIC_PROGRAMME_CHANNEL 0
#define TRAFFIC_ANNOUNCEMENT_CHANNEL 0
#define POPMUSIC_CHANNEL 1

/* emulated rds data from channels */
static const RDSData rdsData[NUM_OF_CHANNELS] = {
        {88500000, (XAchar *)"Traffic", (XAchar *)"All your traffic needs", XA_RDSPROGRAMMETYPE_RDSPTY_NONE,
            (XAchar *)"None", (XAchar *)"None", 0x0001, XA_BOOLEAN_TRUE, XA_BOOLEAN_TRUE },
        {89300000, (XAchar *)"Popular", (XAchar *)"For the populous", XA_RDSPROGRAMMETYPE_RDSPTY_POPMUSIC,
            (XAchar *)"Pop music", (XAchar *)"Pop music for the masses", 0x0002, XA_BOOLEAN_FALSE, XA_BOOLEAN_FALSE }
};

static const XAchar* noneString = (XAchar *)"None";

static XAint16 subscribedODAGroups[MAX_ODA_GROUP_SUBSCRIPTIONS];
static XAuint16 subscribedODAGroupCount = 0;

//static XAuint32 freqSetAfterSeek;

const RDSData* GetCurrentRdsData( XAAdaptationGstCtx *bCtx );
void * XARDSItfAdapt_AsyncSeek(void* args);
void * XARDSItfAdapt_AsyncGetODAGroup(void* args);

/* exposing radio itf adaptation internal function here */
//XAresult XARadioItfAdapt_SyncSetFrequency(XAAdaptationGstCtx *bCtx, XAuint32 freq);


/*
 * XAresult XARDSItfAdapt_QueryRDSSignal(XAAdaptationGstCtx *bCtx, XAboolean * isSignal)
 */
XAresult XARDSItfAdapt_QueryRDSSignal(XAAdaptationGstCtx *bCtx, XAboolean * isSignal)
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XARDSItfAdapt_QueryRDSSignal");

    if(!bCtx || !isSignal)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARDSItfAdapt_QueryRDSSignal");
        return XA_RESULT_PARAMETER_INVALID;
    }

    if (GetCurrentRdsData(bCtx))
    {
        *isSignal = XA_BOOLEAN_TRUE;
    }
    else
    {
        *isSignal = XA_BOOLEAN_FALSE;
    }

    DEBUG_API("<-XARDSItfAdapt_QueryRDSSignal");
    return ret;
}

/*
 * XAresult XARDSItfAdapt_GetProgrammeServiceName(XAAdaptationGstCtx *bCtx, XAchar * ps)
 */
XAresult XARDSItfAdapt_GetProgrammeServiceName(XAAdaptationGstCtx *bCtx, XAchar * ps)
{
    XAresult ret = XA_RESULT_SUCCESS;
    const RDSData* data;
    DEBUG_API("->XARDSItfAdapt_GetProgrammeServiceName");


    if(!bCtx || !ps)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARDSItfAdapt_GetProgrammeServiceName");
        return XA_RESULT_PARAMETER_INVALID;
    }

    data = GetCurrentRdsData( bCtx );

    if (data)
    {
        strncpy( (char *)ps, (char *)data->progServiceName, MAX_PS_STRING_LENGHT );
    }
    else
    {
        *ps = 0;
    }

    DEBUG_API("<-XARDSItfAdapt_GetProgrammeServiceName");
    return ret;
}

/*
 * XAresult XARDSItfAdapt_GetRadioText(XAAdaptationGstCtx *bCtx, XAchar * rt)
 */
XAresult XARDSItfAdapt_GetRadioText(XAAdaptationGstCtx *bCtx, XAchar * rt)
{
    XAresult ret = XA_RESULT_SUCCESS;
    const RDSData* data;
    DEBUG_API("->XARDSItfAdapt_GetRadioText");

    if(!bCtx || !rt)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARDSItfAdapt_GetRadioText");
        return XA_RESULT_PARAMETER_INVALID;
    }

    data = GetCurrentRdsData( bCtx );

    if (data)
    {
        strncpy( (char *)rt, (char *)data->radioText, MAX_RT_STRING_LENGHT );
    }
    else
    {
        *rt = 0;
    }

    DEBUG_API("<-XARDSItfAdapt_GetRadioText");
    return ret;
}

/*
 * XAresult XARDSItfAdapt_GetRadioTextPlus(XAAdaptationGstCtx *bCtx,
 *                                         XAuint8 contentType,
 *                                         XAchar * informationElement,
 *                                         XAchar * descriptor,
 *                                         XAuint8 * descriptorContentType)
 */
XAresult XARDSItfAdapt_GetRadioTextPlus(XAAdaptationGstCtx *bCtx,
                                        XAuint8 contentType,
                                        XAchar * informationElement,
                                        XAchar * descriptor,
                                        XAuint8 * descriptorContentType)
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XARDSItfAdapt_GetRadioTextPlus");

    if(!bCtx || contentType < XA_RDSRTPLUS_ITEMTITLE || contentType > XA_RDSRTPLUS_GETDATA
            || !informationElement || !descriptor || !descriptorContentType)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARDSItfAdapt_GetRadioTextPlus");
        return XA_RESULT_PARAMETER_INVALID;
    }

    *informationElement = 0;
    *descriptorContentType = 0;

    DEBUG_API("<-XARDSItfAdapt_GetRadioTextPlus");
    return ret;
}

/*
 * XAresult XARDSItfAdapt_GetProgrammeType(XAAdaptationGstCtx *bCtx, XAuint32 * pty)
 */
XAresult XARDSItfAdapt_GetProgrammeType(XAAdaptationGstCtx *bCtx, XAuint32 * pty)
{
    XAresult ret = XA_RESULT_SUCCESS;
    const RDSData* data;
    DEBUG_API("->XARDSItfAdapt_GetProgrammeType");

    if(!bCtx || !pty)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARDSItfAdapt_GetProgrammeType");
        return XA_RESULT_PARAMETER_INVALID;
    }

    data = GetCurrentRdsData( bCtx );

    if (data)
    {
        *pty = data->programmeType;
    }
    else
    {
        *pty = 0;
    }

    DEBUG_API("<-XARDSItfAdapt_GetProgrammeType");
    return ret;
}

/*
 * XAresult XARDSItfAdapt_GetProgrammeTypeString(XAAdaptationGstCtx *bCtx,
 *                                               XAboolean isLengthMax16,
 *                                               XAchar * pty)
 *
 */
XAresult XARDSItfAdapt_GetProgrammeTypeString(XAAdaptationGstCtx *bCtx,
                                             XAboolean isLengthMax16,
                                             XAchar * pty)
{
    XAresult ret = XA_RESULT_SUCCESS;
    const RDSData* data;
    DEBUG_API("->XARDSItfAdapt_GetProgrammeTypeString");

    if(!bCtx || !pty)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARDSItfAdapt_GetProgrammeTypeString");
        return XA_RESULT_PARAMETER_INVALID;
    }

    data = GetCurrentRdsData( bCtx );

    if (data)
    {
        if (isLengthMax16)
        {
            DEBUG_API_A1("Programme type string %s", data->ptyStringLong);
            strncpy( (char *)pty, (char *)data->ptyStringLong, MAX_PTY_STRING_LONG_LENGHT );
        }
        else
        {
            DEBUG_API_A1("Programme type string %s", data->ptyStringShort);
            strncpy( (char *)pty, (char *)data->ptyStringShort, MAX_PTY_STRING_SHORT_LENGHT );
        }
    }
    else
    {
        DEBUG_API_A1("Programme type string %s", noneString);
        strncpy( (char *)pty, (char *)noneString, NONE_STRING_LENGHT );
    }

    DEBUG_API("<-XARDSItfAdapt_GetProgrammeTypeString");
    return ret;
}

/*
 * XAresult XARDSItfAdapt_GetProgrammeIdentificationCode(XAAdaptationGstCtx *bCtx, XAint16 * pi)
 */
XAresult XARDSItfAdapt_GetProgrammeIdentificationCode(XAAdaptationGstCtx *bCtx, XAint16 * pi)
{
    XAresult ret = XA_RESULT_SUCCESS;
    const RDSData* data;
    DEBUG_API("->XARDSItfAdapt_GetProgrammeIdentificationCode");

    if(!bCtx || !pi)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARDSItfAdapt_GetProgrammeIdentificationCode");
        return XA_RESULT_PARAMETER_INVALID;
    }

    data = GetCurrentRdsData( bCtx );

    if (data)
    {
        *pi = data->programmeID;
    }
    else
    {
        *pi = 0;
    }

    DEBUG_API("<-XARDSItfAdapt_GetProgrammeIdentificationCode");
    return ret;
}

/*
 * XAresult XARDSItfAdapt_GetClockTime(XAAdaptationGstCtx *bCtx, XAtime * dateAndTime)
 */
XAresult XARDSItfAdapt_GetClockTime(XAAdaptationGstCtx *bCtx, XAtime * dateAndTime)
{
    XAresult ret = XA_RESULT_SUCCESS;
    time_t timeRet=0;
    DEBUG_API("->XARDSItfAdapt_GetClockTime");

    if(!bCtx || !dateAndTime)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARDSItfAdapt_GetClockTime");
        return XA_RESULT_PARAMETER_INVALID;
    }

    timeRet = time(NULL);

    if (timeRet == -1) /* could not get time for some reason */
    {
        ret = XA_RESULT_INTERNAL_ERROR;
    }
    else
    {
        *dateAndTime = timeRet;
    }

    DEBUG_API("<-XARDSItfAdapt_GetClockTime");
    return ret;
}

/*
 * XAresult XARDSItfAdapt_GetTrafficAnnouncement(XAAdaptationGstCtx *bCtx, XAboolean * ta)
 */
XAresult XARDSItfAdapt_GetTrafficAnnouncement(XAAdaptationGstCtx *bCtx, XAboolean * ta)
{
    XAresult ret = XA_RESULT_SUCCESS;
    const RDSData* data;
    DEBUG_API("->XARDSItfAdapt_GetTrafficAnnouncement");

    if(!bCtx || !ta)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARDSItfAdapt_GetTrafficAnnouncement");
        return XA_RESULT_PARAMETER_INVALID;
    }

    data = GetCurrentRdsData( bCtx );

    if (data)
    {
        *ta = data->trafficAnnouncement;
    }
    else
    {
        *ta = XA_BOOLEAN_FALSE;
    }

    DEBUG_API("<-XARDSItfAdapt_GetTrafficAnnouncement");
    return ret;
}

/*
 * XAresult XARDSItfAdapt_GetTrafficProgramme(XAAdaptationGstCtx *bCtx, XAboolean * tp)
 */
XAresult XARDSItfAdapt_GetTrafficProgramme(XAAdaptationGstCtx *bCtx, XAboolean * tp)
{
    XAresult ret = XA_RESULT_SUCCESS;
    const RDSData* data;
    DEBUG_API("->XARDSItfAdapt_GetTrafficProgramme");

    if(!bCtx || !tp)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARDSItfAdapt_GetTrafficProgramme");
        return XA_RESULT_PARAMETER_INVALID;
    }

    data = GetCurrentRdsData( bCtx );

    if (data)
    {
        *tp = data->trafficProgramme;
    }
    else
    {
        *tp = XA_BOOLEAN_FALSE;
    }

    DEBUG_API("<-XARDSItfAdapt_GetTrafficProgramme");
    return ret;
}

/*
 * XAresult XARDSItfAdapt_SeekByProgrammeType(XAAdaptationGstCtx *bCtx,
 *                                            XAuint32 pty,
 *                                            XAboolean upwards)
 */
XAresult XARDSItfAdapt_SeekByProgrammeType(XAAdaptationGstCtx *bCtx,
                                           XAuint32 pty,
                                           XAboolean upwards)
{
    XAresult ret = XA_RESULT_SUCCESS;
    int pt_ret=0;
    XARadioAdaptationCtx* mCtx = NULL;
    DEBUG_API("->XARDSItfAdapt_SeekByProgrammeType");

    if(!bCtx)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARDSItfAdapt_SeekByProgrammeType");
        return XA_RESULT_PARAMETER_INVALID;
    }

    mCtx = (XARadioAdaptationCtx*) bCtx;

    if (mCtx->state != XA_RADIO_IDLE)
    {
      //  pthread_cancel(mCtx->emulationThread);
        mCtx->state = XA_RADIO_IDLE;
    }

    if (pty == XA_RDSPROGRAMMETYPE_RDSPTY_POPMUSIC)
    {
        //freqSetAfterSeek = rdsData[POPMUSIC_CHANNEL].frequency;
    }
    else
    {
        //freqSetAfterSeek = mCtx->frequency;
    }

    mCtx->state = XA_RADIO_SEEKING;
    pt_ret = pthread_create(&(mCtx->emulationThread), NULL, (XARDSItfAdapt_AsyncSeek),(void*)bCtx);

    if(pt_ret)
    {
        DEBUG_ERR_A1("could not create thread!! (%d)",ret);
        DEBUG_API("<-XARDSItfAdapt_SeekByProgrammeType");
        return XA_RESULT_INTERNAL_ERROR;
    }

    DEBUG_API("<-XARDSItfAdapt_SeekByProgrammeType");
    return ret;
}

/*
 * XAresult XARDSItfAdapt_SeekTrafficAnnouncement(XAAdaptationGstCtx *bCtx, XAboolean upwards)
 */
XAresult XARDSItfAdapt_SeekTrafficAnnouncement(XAAdaptationGstCtx *bCtx, XAboolean upwards)
{
    XAresult ret = XA_RESULT_SUCCESS;
    int pt_ret=0;
    XARadioAdaptationCtx* mCtx = NULL;
    DEBUG_API("->XARDSItfAdapt_SeekTrafficAnnouncement");

    if(!bCtx)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARDSItfAdapt_SeekTrafficAnnouncement");
        return XA_RESULT_PARAMETER_INVALID;
    }

    mCtx = (XARadioAdaptationCtx*) bCtx;

    if (mCtx->state != XA_RADIO_IDLE)
    {
     //   pthread_cancel(mCtx->emulationThread);
        mCtx->state = XA_RADIO_IDLE;
    }

    //freqSetAfterSeek = rdsData[TRAFFIC_ANNOUNCEMENT_CHANNEL].frequency;

    mCtx->state = XA_RADIO_SEEKING;
    pt_ret = pthread_create(&(mCtx->emulationThread), NULL, (XARDSItfAdapt_AsyncSeek),(void*)bCtx);

    if(pt_ret)
    {
        DEBUG_ERR_A1("could not create thread!! (%d)",ret);
        DEBUG_API("<-XARDSItfAdapt_SeekTrafficAnnouncement");
        return XA_RESULT_INTERNAL_ERROR;
    }

    DEBUG_API("<-XARDSItfAdapt_SeekTrafficAnnouncement");
    return ret;
}

/*
 * XAresult XARDSItfAdapt_SeekTrafficProgramme(XAAdaptationGstCtx *bCtx, XAboolean upwards)
 */
XAresult XARDSItfAdapt_SeekTrafficProgramme(XAAdaptationGstCtx *bCtx, XAboolean upwards)
{
    XAresult ret = XA_RESULT_SUCCESS;
    int pt_ret=0;
    XARadioAdaptationCtx* mCtx = NULL;
    DEBUG_API("->XARDSItfAdapt_SeekTrafficProgramme");

    if(!bCtx)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARDSItfAdapt_SeekTrafficProgramme");
        return XA_RESULT_PARAMETER_INVALID;
    }

    mCtx = (XARadioAdaptationCtx*) bCtx;

    if (mCtx->state != XA_RADIO_IDLE)
    {
      //  pthread_cancel(mCtx->emulationThread);
        mCtx->state = XA_RADIO_IDLE;
    }

    //freqSetAfterSeek = rdsData[TRAFFIC_PROGRAMME_CHANNEL].frequency;

    mCtx->state = XA_RADIO_SEEKING;
    pt_ret = pthread_create(&(mCtx->emulationThread), NULL, (XARDSItfAdapt_AsyncSeek),(void*)bCtx);

    if(pt_ret)
    {
        DEBUG_ERR_A1("could not create thread!! (%d)",ret);
        DEBUG_API("<-XARDSItfAdapt_SeekTrafficProgramme");
        return XA_RESULT_INTERNAL_ERROR;
    }

    DEBUG_API("<-XARDSItfAdapt_SeekTrafficProgramme");
    return ret;
}

/*
 * void * XARadioItfAdapt_AsyncSeek(void* args)
 */
void * XARDSItfAdapt_AsyncSeek(void* args)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAAdaptationGstCtx *bCtx = (XAAdaptationGstCtx*)args;

    XARadioAdaptationCtx* mCtx = (XARadioAdaptationCtx*) bCtx;

    DEBUG_API("->XARDSItfAdapt_AsyncSeek");

    usleep(100000); /* 1/10 seconds */

    DEBUG_INFO("Seek done!");

    //ret = XARadioItfAdapt_SyncSetFrequency( bCtx, freqSetAfterSeek );
    if (ret != XA_RESULT_SUCCESS)
    {
        DEBUG_ERR("XARadioItfAdapt_SyncSetFrequency FAILED");
    }

    /* Send event to RadioItf */
    {
        XAAdaptEvent event = {XA_RADIOITFEVENTS, XA_ADAPT_RADIO_SEEK_COMPLETE, 0, NULL };
        XAAdaptationBase_SendAdaptEvents(&bCtx->baseObj, &event );
    }
    mCtx->state = XA_RADIO_IDLE;

    DEBUG_API("<-XARDSItfAdapt_AsyncSeek");
    pthread_exit(NULL);
    return NULL;
}

/*
 * XAresult XARDSItfAdapt_SetAutomaticSwitching(XAAdaptationGstCtx *bCtx, XAboolean automatic)
 */
XAresult XARDSItfAdapt_SetAutomaticSwitching(XAAdaptationGstCtx *bCtx, XAboolean automatic)
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XARDSItfAdapt_SetAutomaticSwitching");

    if(!bCtx)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARDSItfAdapt_SetAutomaticSwitching");
        return XA_RESULT_PARAMETER_INVALID;
    }

    ret = XA_RESULT_FEATURE_UNSUPPORTED;

    DEBUG_API("<-XARDSItfAdapt_SetAutomaticSwitching");
    return ret;
}

/*
 * XAresult XARDSItfAdapt_GetAutomaticSwitching(XAAdaptationGstCtx *bCtx, XAboolean * automatic)
 */
XAresult XARDSItfAdapt_GetAutomaticSwitching(XAAdaptationGstCtx *bCtx, XAboolean * automatic)
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XARDSItfAdapt_GetAutomaticSwitching");

    if(!bCtx || !automatic)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARDSItfAdapt_GetAutomaticSwitching");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* Feature not supported */
    *automatic = XA_BOOLEAN_FALSE;

    DEBUG_API("<-XARDSItfAdapt_GetAutomaticSwitching");
    return ret;
}

/*
 * XAresult XARDSItfAdapt_SetAutomaticTrafficAnnouncement(XAAdaptationGstCtx *bCtx, XAboolean automatic)
 */
XAresult XARDSItfAdapt_SetAutomaticTrafficAnnouncement(XAAdaptationGstCtx *bCtx, XAboolean automatic)
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XARDSItfAdapt_SetAutomaticTrafficAnnouncement");
    if(!bCtx)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARDSItfAdapt_SetAutomaticTrafficAnnouncement");
        return XA_RESULT_PARAMETER_INVALID;
    }

    ret = XA_RESULT_FEATURE_UNSUPPORTED;

    DEBUG_API("<-XARDSItfAdapt_SetAutomaticTrafficAnnouncement");
    return ret;
}

/*
 * XAresult XARDSItfAdapt_GetAutomaticTrafficAnnouncement(XAAdaptationGstCtx *bCtx, XAboolean * automatic)
 */
XAresult XARDSItfAdapt_GetAutomaticTrafficAnnouncement(XAAdaptationGstCtx *bCtx, XAboolean * automatic)
{
    XAresult ret = XA_RESULT_SUCCESS;
    DEBUG_API("->XARDSItfAdapt_GetAutomaticTrafficAnnouncement");

    if(!bCtx || !automatic)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARDSItfAdapt_GetAutomaticTrafficAnnouncement");
        return XA_RESULT_PARAMETER_INVALID;
    }

    *automatic = XA_BOOLEAN_FALSE;

    DEBUG_API("<-XARDSItfAdapt_GetAutomaticTrafficAnnouncement");
    return ret;
}

/*
 * XAresult XARDSItfAdapt_GetODAGroup(XAAdaptationGstCtx *bCtx,
 *                                    XAint16 AID)
 */
XAresult XARDSItfAdapt_GetODAGroup(XAAdaptationGstCtx *bCtx, XAint16 AID)
{
    XAresult ret = XA_RESULT_SUCCESS;
    int pt_ret=0;
    XARadioAdaptationCtx* mCtx=NULL;
    DEBUG_API("->XARDSItfAdapt_GetODAGroup");

    if(!bCtx)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARDSItfAdapt_GetODAGroup");
        return XA_RESULT_PARAMETER_INVALID;
    }

    mCtx = (XARadioAdaptationCtx*) bCtx;

    if (mCtx->rdsState != XA_RADIO_RDS_IDLE)
    {
      //  pthread_cancel(mCtx->rdsEmulationThread);
        mCtx->rdsState = XA_RADIO_RDS_IDLE;
        DEBUG_INFO("Previous getodagroup async call cancelled")
    }

    pt_ret = pthread_create(&(mCtx->rdsEmulationThread), NULL, (XARDSItfAdapt_AsyncGetODAGroup),(void*)bCtx);

    if(pt_ret)
    {
        DEBUG_ERR_A1("could not create thread!! (%d)",ret);
        DEBUG_API("<-XARDSItfAdapt_GetODAGroup");
        return XA_RESULT_INTERNAL_ERROR;
    }

    DEBUG_API("<-XARDSItfAdapt_GetODAGroup");
    return ret;
}

/*
 * void * XARDSItfAdapt_AsyncGetODAGroup(void* args)
 */
void * XARDSItfAdapt_AsyncGetODAGroup(void* args)
{
    XARadioAdaptationCtx* mCtx = (XARadioAdaptationCtx*)args;

    DEBUG_API("->XARDSItfAdapt_AsyncGetODAGroup");
    mCtx->rdsState = XA_RADIO_RDS_GETTING_ODA_GROUP;

    DEBUG_INFO("Get oda group async wait")
    usleep(100000); /* 1/10 seconds */

    /* Send event */
    {
        XAAdaptEvent event = {XA_RDSITFEVENTS, XA_ADAPT_RDS_GET_ODA_GROUP_DONE, 0, NULL };
        XAAdaptationBase_SendAdaptEvents(args, &event );
    }
    mCtx->rdsState = XA_RADIO_RDS_IDLE;
    DEBUG_API("<-XARDSItfAdapt_AsyncGetODAGroup");
    pthread_exit(NULL);
    return NULL;
}

/*
 * XAresult XARDSItfAdapt_SubscribeODAGroup(XAAdaptationGstCtx *bCtx,
 *                                          XAint16 group,
 *                                          XAboolean useErrorCorrection)
 */
XAresult XARDSItfAdapt_SubscribeODAGroup(XAAdaptationGstCtx *bCtx,
                                        XAint16 group,
                                        XAboolean useErrorCorrection)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAuint8 index=0;
    XAboolean groupFound = XA_BOOLEAN_FALSE;
    DEBUG_API("->XARDSItfAdapt_SubscribeODAGroup");

    if(!bCtx)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARDSItfAdapt_SubscribeODAGroup");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* Look for group in list, if not there add it */
    if (subscribedODAGroupCount < MAX_ODA_GROUP_SUBSCRIPTIONS)
    {
        for (index = 0; index < subscribedODAGroupCount; index++)
        {
            if (subscribedODAGroups[index] == group)
            {
                groupFound = XA_BOOLEAN_TRUE;
            }
        }
        if (!groupFound)
        {
            subscribedODAGroups[subscribedODAGroupCount] = group;
            subscribedODAGroupCount++;
        }
    }
    else
    {
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("<-XARDSItfAdapt_SubscribeODAGroup");
        return XA_RESULT_INTERNAL_ERROR;
    }

    DEBUG_API("<-XARDSItfAdapt_SubscribeODAGroup");
    return ret;
}

/*
 * XAresult XARDSItfAdapt_UnsubscribeODAGroup(XAAdaptationGstCtx *bCtx, XAint16 group)
 */
XAresult XARDSItfAdapt_UnsubscribeODAGroup(XAAdaptationGstCtx *bCtx, XAint16 group)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAuint8 index=0;
    DEBUG_API("->XARDSItfAdapt_UnsubscribeODAGroup");

    if(!bCtx)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARDSItfAdapt_UnsubscribeODAGroup");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* Look for group in list, if found, remove it */
    for (index = 0; index < subscribedODAGroupCount; index++)
    {
        if (subscribedODAGroups[index] == group)
        {
            subscribedODAGroups[index] = subscribedODAGroups[subscribedODAGroupCount-1];
            subscribedODAGroupCount--;
        }
    }

    DEBUG_API("<-XARDSItfAdapt_UnsubscribeODAGroup");
    return ret;
}

/*
 * XAresult XARDSItfAdapt_ListODAGroupSubscriptions(XAAdaptationGstCtx *bCtx,
 *                                                  XAint16* pGroups,
 *                                                  XAuint32* pLength)
 */
XAresult XARDSItfAdapt_ListODAGroupSubscriptions(XAAdaptationGstCtx *bCtx,
                                                XAint16* pGroups,
                                                XAuint32* pLength)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAuint8 index=0;
    DEBUG_API("->XARDSItfAdapt_ListODAGroupSubscriptions");

    if(!bCtx || !pLength)
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("<-XARDSItfAdapt_ListODAGroupSubscriptions");
        return XA_RESULT_PARAMETER_INVALID;
    }

    if (!pGroups)
    {
        *pLength = subscribedODAGroupCount;
    }
    else
    {
        if (*pLength < subscribedODAGroupCount)
        {
            ret = XA_RESULT_BUFFER_INSUFFICIENT;
        }
        for (index = 0; index < *pLength; index++)
        {
            pGroups[index] = subscribedODAGroups[index];
        }
    }

    DEBUG_API("<-XARDSItfAdapt_ListODAGroupSubscriptions");
    return ret;
}

/*
 * const RDSData* GetCurrentRdsData( XAAdaptationGstCtx *bCtx )
 */
const RDSData* GetCurrentRdsData( XAAdaptationGstCtx *bCtx )
{
    XAuint8 index=0;
    XARadioAdaptationCtx* mCtx = (XARadioAdaptationCtx*) bCtx;

    for (index = 0; index < NUM_OF_CHANNELS; index++ )
    {
        if (rdsData[index].frequency == mCtx->frequency)
        {
            DEBUG_API("<-GetCurrentRdsData");
            return &(rdsData[index]);
        }
    }
    DEBUG_API("<-GetCurrentRdsData");
    return NULL;
}


/*
 * void XARDSItfAdapt_Free(XAAdaptationGstCtx *bCtx)
 */
void XARDSItfAdapt_Free(XAAdaptationBaseCtx *bCtx)
{
    XARadioAdaptationCtx* mCtx = (XARadioAdaptationCtx*) bCtx;
    DEBUG_API("->XARDSItfAdapt_Free");

    if (mCtx->state != XA_RADIO_IDLE)
    {
       // pthread_cancel(mCtx->emulationThread);
        mCtx->state = XA_RADIO_IDLE;
    }

    if (mCtx->rdsState != XA_RADIO_RDS_IDLE)
    {
      //  pthread_cancel(mCtx->rdsEmulationThread);
        mCtx->rdsState = XA_RADIO_RDS_IDLE;
    }

    DEBUG_API("<-XARDSItfAdapt_Free");
}
