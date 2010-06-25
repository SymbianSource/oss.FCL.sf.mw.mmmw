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
 * Description: GST Capabilities Mgr. Here you can query the capabilities
 *               of various GST components and store it in a list.
 *
 */

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <gst/gst.h>
#include <gobject_global.h>

#include "xagstcapabilitiesmgr.h"
#include "xaframeworkmgr.h"

static XAresult XAGSTCapabilitiesMgr_GetAudioAACEncoderCapabilities(
        XACapabilities **ppNode);
static XAresult XAGSTCapabilitiesMgr_GetAudioAMREncoderCapabilities(
        XACapabilities **ppNode);
static XAresult XAGSTCapabilitiesMgr_GetAudioPCMEncoderCapabilities(
        XACapabilities **ppNode);
static XAresult XAGSTCapabilitiesMgr_GetCapabilities_FromGstElement(
        XAAudioCodecDescriptor **entries, XAuint32 type);

/* XAresult XAGSTCapabilitiesMgr_UpdateCapabilitieList
 * Description: Update the capabilities list supported by GStreamer framework.
 */
static gboolean populate_field(GQuark field, const GValue * value,
        gpointer pfx)
    {
    gchar *field_name;
    gpointer *pfxd = (gpointer*) pfx;
    XAAudioCodecDescriptor *ctxx = (XAAudioCodecDescriptor *) pfxd;

    field_name = (gchar*) g_quark_to_string(field);

    if ((strcasecmp((const char*) field_name, "channels") == 0))
        {
        if (GST_VALUE_HOLDS_INT_RANGE(value) == TRUE)
            {
            (ctxx)->maxChannels = gst_value_get_int_range_max(value);
            }
        else
            (ctxx)->maxChannels = g_value_get_int(value);
        }
    if ((strcasecmp((const char*) field_name, "depth") == 0))
        {
        if (GST_VALUE_HOLDS_INT_RANGE(value) == TRUE)
            {
            (ctxx)->minBitsPerSample = gst_value_get_int_range_min(value);
            }
        else
            (ctxx)->minBitsPerSample = g_value_get_int(value);
        }
    if ((strcasecmp((const char*) field_name, "bitrate") == 0))
        {
        if (GST_VALUE_HOLDS_INT_RANGE(value) == TRUE)
            {
            (ctxx)->minBitRate = gst_value_get_int_range_min(value);
            (ctxx)->maxBitRate = gst_value_get_int_range_max(value);
            }
        else
            {
            (ctxx)->minBitRate = g_value_get_int(value);
            (ctxx)->maxBitRate = g_value_get_int(value);
            }
        }
    if ((strcasecmp((const char*) field_name, "width") == 0))
        {
        if (GST_VALUE_HOLDS_INT_RANGE(value) == TRUE)
            {
            (ctxx)->maxBitsPerSample = gst_value_get_int_range_max(value);
            }
        else
            (ctxx)->maxBitsPerSample = g_value_get_int(value);
        }
    if ((strcasecmp((const char*) field_name, "rate") == 0))
        {
        if (GST_VALUE_HOLDS_INT_RANGE(value) == TRUE)
            {
            (ctxx)->minSampleRate = gst_value_get_int_range_min(value) * 1000;
            (ctxx)->maxSampleRate = gst_value_get_int_range_max(value) * 1000;
            }
        else
            {
            (ctxx)->minSampleRate = g_value_get_int(value) * 1000;
            (ctxx)->maxSampleRate = g_value_get_int(value) * 1000;
            }
        }

    return TRUE;
    }

XAresult XAGSTCapabilitiesMgr_GetCapabilities_FromGstElement(
        XAAudioCodecDescriptor **entries, XAuint32 type)
    {
    int i = 0;
    GstElement *element = NULL;
    GstPad *elementpad = NULL;
    GstCaps *elementcaps = NULL;
    GstStructure *s = NULL;
    GParamSpec *elementproperty;
    GParamSpecInt *pint;
    GParamSpecEnum *penum;
    gchar *findamr = "\0";
    gpointer *myentries;


    if (type == XA_AUDIOCODEC_AAC)
        {
        element = gst_element_factory_make("nokiaaacenc", "encoderelement");
        if (element)
            {
            elementpad = gst_element_get_static_pad(element, "sink");
            if (!elementpad)
                {
                DEBUG_ERR("no pad found for AAC Encoder");
                return XA_RESULT_INTERNAL_ERROR;
                }
            elementcaps = gst_pad_get_caps(elementpad);
            s = gst_caps_get_structure(elementcaps, 0);

            myentries = (gpointer*) *entries;
            gst_structure_foreach(s, populate_field, (gpointer) myentries);

            elementproperty = g_object_class_find_property(
                    G_OBJECT_GET_CLASS (element), "bitrate");
            if (elementproperty)
                {
                pint = G_PARAM_SPEC_INT (elementproperty);
                (*entries)->minBitRate = pint->minimum;
                (*entries)->maxBitRate = pint->maximum;
                }
            elementproperty = g_object_class_find_property(
                    G_OBJECT_GET_CLASS (element), "profile");
            if (elementproperty)
                {
                penum = G_PARAM_SPEC_ENUM(elementproperty);
                (*entries)->modeSetting = penum->default_value;
                }

            (*entries)->profileSetting = XA_AUDIOPROFILE_AAC_AAC;
            (*entries)->isFreqRangeContinuous = XA_BOOLEAN_TRUE;
            (*entries)->isBitrateRangeContinuous = XA_BOOLEAN_TRUE;
            }
        else
            {
            DEBUG_ERR("failed to get the AACencoder element");
            }
        }
    else if (type == XA_AUDIOCODEC_AMR)
        {
        element = gst_element_factory_make("devsoundsrc", "encoderelement");
        if (element)
            {
            elementpad = gst_element_get_pad(element, "src");
            if (!elementpad)
                {
                DEBUG_ERR("no source pad for Devsound(amr) element");
                return XA_RESULT_INTERNAL_ERROR;
                }

            elementcaps = gst_pad_get_caps(elementpad);
            while (!(strcasecmp(findamr, "audio/amr") == 0) && i
                    < elementcaps->structs->len)
                {
                s = gst_caps_get_structure(elementcaps, i);
                findamr = (char*) gst_structure_get_name(s);
                i++;
                }

            //populating the Other Values
            myentries = (gpointer*) *entries;
            gst_structure_foreach(s, populate_field, (gpointer) myentries);

            elementproperty = g_object_class_find_property(
                    G_OBJECT_GET_CLASS (element), "speechbitrate");
            if (elementproperty)
                {
                pint = G_PARAM_SPEC_INT (elementproperty);
                (*entries)->minBitRate = pint->minimum;
                (*entries)->maxBitRate = pint->maximum;
                }

            (*entries)->profileSetting = XA_AUDIOPROFILE_AMR;
            (*entries)->modeSetting = 0;
            (*entries)->isFreqRangeContinuous = XA_BOOLEAN_TRUE;
            (*entries)->isBitrateRangeContinuous = XA_BOOLEAN_TRUE;

            }
        else
            {
            DEBUG_ERR("failed to get the Devsoundsrc(amr encoder) element");
            }
        }

    else if (type == XA_AUDIOCODEC_PCM)
        {
        element = gst_element_factory_make("wavenc", "encoderelement");
        if (element)
            {
            elementpad = gst_element_get_pad(element, "sink");
            if (!elementpad)
                {
                DEBUG_ERR("failed to get Sink pad on Wave encoder element");
                return XA_RESULT_INTERNAL_ERROR;
                }
            elementcaps = gst_pad_get_caps(elementpad);
            s = gst_caps_get_structure(elementcaps, 2);
            myentries = (gpointer*) *entries;
            gst_structure_foreach(s, populate_field, (gpointer) myentries);
            elementproperty = g_object_class_find_property(
                    G_OBJECT_GET_CLASS (element), "bitrate");
            if (elementproperty)
                {
                pint = G_PARAM_SPEC_INT (elementproperty);
                (*entries)->minBitRate = pint->minimum;
                (*entries)->maxBitRate = pint->maximum;
                }

            (*entries)->profileSetting = XA_AUDIOPROFILE_PCM;
            (*entries)->modeSetting = 0;
            (*entries)->isFreqRangeContinuous = XA_BOOLEAN_TRUE;
            (*entries)->isBitrateRangeContinuous = XA_BOOLEAN_TRUE;
            }
        else
            {
            DEBUG_ERR("failed to get the wavencoder element");
            }
        }

    if (elementcaps != NULL)
        gst_caps_unref(elementcaps);
    if (elementpad != NULL)
        gst_object_unref(elementpad);
    if (element != NULL)
        gst_object_unref(element);

    return XA_RESULT_SUCCESS;
    }
XAresult XAGSTCapabilitiesMgr_UpdateCapabilitieList(
        FrameworkMap *frameworkMap, XACapabilities **ppListHead)

    {
    XAresult res = XA_RESULT_SUCCESS;
    XACapabilities *newNode = NULL;
    FWMgrFwType fwtype = FWMgrFWUknown;
    char *uri = NULL;
    XACapabilities *lastNode;
    XACapabilities *firstNode;

    DEBUG_API("->XAGSTCapabilitiesMgr_UpdateCapabilitieList");

    if (!frameworkMap || !ppListHead)
        {
        res = XA_RESULT_PARAMETER_INVALID;
        return res;
        }

    lastNode = firstNode = *ppListHead;

    /* traverse and point to the last node in the list */
    while (lastNode && lastNode->next)
        {
        lastNode = lastNode->next;
        }

    uri = "file:///c:/test.mp4";
    fwtype = XAFrameworkMgr_GetFramework(frameworkMap, uri, FWMgrMORecorder);

    if (fwtype == FWMgrFWGST)
        {
        GError* gerror = 0;
        //initialize gstreamer
        //AL_PROFILING_BEGIN;
        if (!gst_init_check(NULL, NULL, &gerror))
            {
            DEBUG_ERR("Gst Initalization failure.");
            return XA_RESULT_INTERNAL_ERROR;
            }
        //AL_PROFILING_END_PRINT("gst_init_check");

        }

    if (fwtype == FWMgrFWGST)
        {
        /* Add codec capabilities */
        newNode = NULL;
        res = XAGSTCapabilitiesMgr_GetAudioAACEncoderCapabilities(&newNode);
        if (res != XA_RESULT_SUCCESS)
            {
            return res;
            }
        if (lastNode)
            {
            lastNode->next = newNode;
            }
        if (newNode)
            { /* if a new node is created move lastNode to the new item */
            if (!firstNode)
                firstNode = newNode;
            lastNode = newNode;
            }
        }

    uri = "file:///c:/test.amr";
    fwtype = XAFrameworkMgr_GetFramework(frameworkMap, uri, FWMgrMORecorder);

    if (fwtype == FWMgrFWGST)
        {
        newNode = NULL;
        res = XAGSTCapabilitiesMgr_GetAudioAMREncoderCapabilities(&newNode);
        if (res != XA_RESULT_SUCCESS)
            {
            return res;
            }
        if (lastNode)
            {
            lastNode->next = newNode;
            }
        if (newNode)
            { /* if a new node is created move lastNode to the new item */
            if (!firstNode)
                firstNode = newNode;
            lastNode = newNode;
            }
        }

    uri = "file:///c:/test.wav";
    fwtype = XAFrameworkMgr_GetFramework(frameworkMap, uri, FWMgrMORecorder);

    if (fwtype == FWMgrFWGST)
        {
        newNode = NULL;
        res = XAGSTCapabilitiesMgr_GetAudioPCMEncoderCapabilities(&newNode);
        if (res != XA_RESULT_SUCCESS)
            {
            return res;
            }
        if (lastNode)
            {
            lastNode->next = newNode;
            }
        if (newNode)
            { /* if a new node is created move lastNode to the new item */
            if (!firstNode)
                firstNode = newNode;
            lastNode = newNode;
            }
        }
    /* if empty list, then append first node as the head */
    if (!(*ppListHead))
        {
        *ppListHead = firstNode;
        }DEBUG_API("<-XAGSTCapabilitiesMgr_UpdateCapabilitieList");
    return res;
    }

XAresult XAGSTCapabilitiesMgr_GetAudioAACEncoderCapabilities(
        XACapabilities **ppNode)
    {
    XAresult res = XA_RESULT_SUCCESS;
    XACapabilities *newNode = NULL;
    XAAudioCodecDescriptor *entries = NULL;
    XAchar aacencelement[] = "nokiaaacenc";
    int strLen = 0;

    newNode = (XACapabilities *) calloc(1, sizeof(XACapabilities));
    if (!newNode)
        {
        res = XA_RESULT_MEMORY_FAILURE;
        return res;
        }

    newNode->capsType = AUD_E;
    newNode->xaid = XA_AUDIOCODEC_AAC;
    newNode->noOfEntries = 1;

    strLen = strlen((char*) aacencelement);
    newNode->adaptId = (XAchar *) calloc(strLen + 1, sizeof(XAchar));
    if (!newNode->adaptId)
        {
        res = XA_RESULT_MEMORY_FAILURE;
        XACapabilitiesMgr_DeleteCapabilitieList(&newNode);
        return res;
        }

    strncpy((char*) newNode->adaptId, (char*) aacencelement, strLen);
    newNode->adaptId[strLen] = '\0'; /*Null terminate it*/

    /* Allocate array */
    entries = (XAAudioCodecDescriptor*) calloc(1,
            sizeof(XAAudioCodecDescriptor));
    if (!entries)
        {
        res = XA_RESULT_MEMORY_FAILURE;
        XACapabilitiesMgr_DeleteCapabilitieList(&newNode);
        return res;
        }

    newNode->pEntry = (void*) entries;

    res = XAGSTCapabilitiesMgr_GetCapabilities_FromGstElement(&entries,
            newNode->xaid);

    newNode->pEntry = (void*) entries;

    *ppNode = newNode;
    return res;
    }

XAresult XAGSTCapabilitiesMgr_GetAudioAMREncoderCapabilities(
        XACapabilities **ppNode)
    {
    XAresult res = XA_RESULT_SUCCESS;
    XACapabilities *newNode = NULL;
    XAAudioCodecDescriptor *entries = NULL;

    newNode = (XACapabilities *) calloc(1, sizeof(XACapabilities));
    if (!newNode)
        {
        res = XA_RESULT_MEMORY_FAILURE;
        return res;
        }

    newNode->capsType = AUD_E;
    newNode->xaid = XA_AUDIOCODEC_AMR;
    newNode->noOfEntries = 1;

    /* Allocate array */
    entries = (XAAudioCodecDescriptor*) calloc(1,
            sizeof(XAAudioCodecDescriptor));
    if (!entries)
        {
        res = XA_RESULT_MEMORY_FAILURE;
        XACapabilitiesMgr_DeleteCapabilitieList(&newNode);
        return res;
        }

    newNode->pEntry = (void*) entries;

    XAGSTCapabilitiesMgr_GetCapabilities_FromGstElement(&entries,
            newNode->xaid);

    newNode->pEntry = (void*) entries;

    *ppNode = newNode;
    return res;
    }

XAresult XAGSTCapabilitiesMgr_GetAudioPCMEncoderCapabilities(
        XACapabilities **ppNode)
    {
    XAresult res = XA_RESULT_SUCCESS;
    XACapabilities *newNode = NULL;
    XAAudioCodecDescriptor *entries = NULL;

    newNode = (XACapabilities *) calloc(1, sizeof(XACapabilities));
    if (!newNode)
        {
        res = XA_RESULT_MEMORY_FAILURE;
        return res;
        }

    newNode->capsType = AUD_E;
    newNode->xaid = XA_AUDIOCODEC_PCM;
    newNode->noOfEntries = 1;

    /* Allocate array */
    entries = (XAAudioCodecDescriptor*) calloc(1,
            sizeof(XAAudioCodecDescriptor));
    if (!entries)
        {
        res = XA_RESULT_MEMORY_FAILURE;
        XACapabilitiesMgr_DeleteCapabilitieList(&newNode);
        return res;
        }

    newNode->pEntry = (void*) entries;

    XAGSTCapabilitiesMgr_GetCapabilities_FromGstElement(&entries,
            newNode->xaid);

    newNode->pEntry = (void*) entries;

    *ppNode = newNode;
    return res;
    }
