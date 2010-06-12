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
 * Description: Capability Manager to query capabilities from different
 *              adaptations and store it in a linked list.
 *
 */

#include "xacapabilitiesmgr.h"
#include "xammfcapabilitiesmgr.h"
#include "xagstcapabilitiesmgr.h"
#include <stdio.h>
#include <string.h>

static XAresult XACapabilitiesMgr_GetAudioInputDeviceCapabilities(
        XACapabilities **ppNode);
static XAresult XACapabilitiesMgr_GetAudioOutputDeviceCapabilities(
        XACapabilities **ppNode);

/* XAresult XAGSTCapabilitiesMgr_UpdateAudioIOCapabilitieList
 * Description: Update the capabilities list supported by GStreamer framework.
 */
XAresult XACapabilitiesMgr_UpdateAudioIOCapabilitieList(
        FrameworkMap *frameworkMap, XACapabilities **ppListHead)

    {
    XAresult res = XA_RESULT_SUCCESS;
    XACapabilities *lastNode;
    XACapabilities *firstNode;
    XACapabilities *newNode = NULL;

    if (!frameworkMap || !ppListHead)
        {
        res = XA_RESULT_PARAMETER_INVALID;
        return res;
        }

    lastNode = *ppListHead;
    firstNode = *ppListHead;

    DEBUG_API("->XACapabilitiesMgr_UpdateAudioIOCapabilitieList");

    /* traverse and point to the last node in the list */
    while (lastNode && lastNode->next)
        {
        lastNode = lastNode->next;
        }

    /* If no input devices are supported, the function returns
     * XA_RESULT_SUCCESS and newNode will be NULL*/
    newNode = NULL;
    res = XACapabilitiesMgr_GetAudioInputDeviceCapabilities(&newNode);
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

    /* If no input devices are supported, the function returns
     * XA_RESULT_SUCCESS and newNode will be NULL*/
    newNode = NULL;
    res = XACapabilitiesMgr_GetAudioOutputDeviceCapabilities(&newNode);
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
    /* if empty list, then append first node as the head */
    if (!(*ppListHead))
        {
        *ppListHead = firstNode;
        }DEBUG_API("<-XACapabilitiesMgr_UpdateAudioIOCapabilitieList");
    return res;
    }

XAresult XACapabilitiesMgr_CreateCapabilitieList(FrameworkMap* frameworkMap,
        XACapabilities** ppListHead)
    {
    XACapabilities* list = NULL;
    XAresult res = XA_RESULT_SUCCESS;
    if (!ppListHead)
        {
        res = XA_RESULT_PARAMETER_INVALID;
        return res;
        }

    *ppListHead = NULL;

    res = XACapabilitiesMgr_UpdateAudioIOCapabilitieList(frameworkMap, &list);
    if (res != XA_RESULT_SUCCESS)
        {
        XACapabilitiesMgr_DeleteCapabilitieList(&list);
        return res;
        }

    res = XAGSTCapabilitiesMgr_UpdateCapabilitieList(frameworkMap, &list);
    if (res != XA_RESULT_SUCCESS)
        {
        XACapabilitiesMgr_DeleteCapabilitieList(&list);
        return res;
        }

    res = XAMMFCapabilitiesMgr_UpdateCapabilitieList(frameworkMap, &list);
    if (res != XA_RESULT_SUCCESS)
        {
        XACapabilitiesMgr_DeleteCapabilitieList(&list);
        return res;
        }

    *ppListHead = list;
    return res;
    }

XAresult XACapabilitiesMgr_DeleteCapabilitieList(XACapabilities** ppListHead)
    {
    XACapabilities* currNode = NULL;
    XACapabilities* nextNode = NULL;
    XAresult res = XA_RESULT_SUCCESS;

    if (!ppListHead)
        {
        res = XA_RESULT_PARAMETER_INVALID;
        return res;
        }

    currNode = *ppListHead;
    while (currNode)
        {
        if (currNode->capsType == AUD_I)
            {
            XAAudioInputDescriptor* entries =
                    (XAAudioInputDescriptor*) currNode->pEntry;
            XAint32 i;
            for (i = 0; i < currNode->noOfEntries; i++)
                {
                free(entries[i].samplingRatesSupported);
                free(entries[i].deviceName);
                }
            free(entries);
            }
        else if (currNode->capsType == AUD_O)
            {
            XAAudioOutputDescriptor* entries =
                    (XAAudioOutputDescriptor*) currNode->pEntry;
            XAint32 i;
            for (i = 0; i < currNode->noOfEntries; i++)
                {
                free(entries[i].samplingRatesSupported);
                free(entries[i].pDeviceName);
                }
            free(entries);
            }
        else if (currNode->capsType == AUD_E)
            {
            XAAudioCodecDescriptor* entries =
                    (XAAudioCodecDescriptor*) currNode->pEntry;
            XAint32 i;
            for (i = 0; i < currNode->noOfEntries; i++)
                {
                free(entries[i].pSampleRatesSupported);
                free(entries[i].pBitratesSupported);
                }
            free(entries);

            }

        if (currNode->adaptId)
            {
            free(currNode->adaptId);
            }

        nextNode = currNode->next;
        free(currNode);
        currNode = nextNode;
        }

    *ppListHead = NULL;
    return res;
    }

/* XAresult XACapabilitiesMgr_GetCapsCount
 * Description: Count capabilities of certain type. Filter is specified by
 *              bitmasking XACapsType values.
 */
XAresult XACapabilitiesMgr_GetCapsCount(XACapabilities* pListHead,
        XACapsType filter, XAuint32* count)
    {
    XAresult res = XA_RESULT_SUCCESS;
    XACapabilities* currNode = pListHead;

    DEBUG_API("->XACapabilitiesMgr_GetCapsCount");
    if (!currNode || !count)
        {
        res = XA_RESULT_PARAMETER_INVALID;
        return res;
        }

    (*count) = 0;
    while (currNode)
        {
        if ((currNode->capsType & filter) == filter)
            {
            (*count)++;
            }
        currNode = currNode->next;
        }

    DEBUG_API("<-XACapabilitiesMgr_GetCapsCount");
    return res;
    }

/* XAresult XACapabilitiesMgr_GetCapsById
 * Description: Get capabilities of type XACapsType and matching id
 */
XAresult XACapabilitiesMgr_GetCapsById(XACapabilities* pListHead,
        XACapsType filter, XAuint32 maxId, XACapabilities* data)
    {
    XAresult res = XA_RESULT_SUCCESS;
    XACapabilities* currNode = pListHead;
    XAboolean found = XA_BOOLEAN_FALSE;

    DEBUG_API("->XACapabilitiesMgr_GetCapsById");

    if (!currNode)
        {
        res = XA_RESULT_PARAMETER_INVALID;
        return res;
        }

    while (currNode)
        {
        if (((currNode->capsType & filter) == filter) && (maxId
                == currNode->xaid))
            {
            memcpy(data, currNode, sizeof(XACapabilities));
            found = XA_BOOLEAN_TRUE;
            break;
            }
        currNode = currNode->next;
        }

    if (!found)
        {
        res = XA_RESULT_FEATURE_UNSUPPORTED;
        return res;
        }

    DEBUG_API("<-XACapabilitiesMgr_GetCapsById");
    return res;
    }

/* XAresult XACapabilitiesMgr_GetCapsByIdx
 * Description: Get n'th capabilities of type XACapsType
 */
XAresult XACapabilitiesMgr_GetCapsByIdx(XACapabilities* pListHead,
        XACapsType filter, XAuint32 idx, XACapabilities* data)
    {
    XAresult res = XA_RESULT_SUCCESS;
    XACapabilities* currNode = pListHead;
    XAboolean found = XA_BOOLEAN_FALSE;
    XAuint32 j = 0;

    DEBUG_API("->XACapabilitiesMgr_GetCapsByIdx");

    if (!currNode)
        {
        res = XA_RESULT_PARAMETER_INVALID;
        return res;
        }

    while (currNode)
        {
        if ((currNode->capsType & filter) == filter)
            {
            if (idx == j++)
                {
                memcpy(data, currNode, sizeof(XACapabilities));
                found = XA_BOOLEAN_TRUE;
                break;
                }
            }
        currNode = currNode->next;
        }

    if (!found)
        {
        res = XA_RESULT_PARAMETER_INVALID;
        return res;
        }

    DEBUG_API("<-XACapabilitiesMgr_GetCapsByIdx");
    return res;
    }

/* XAresult XACapabilitiesMgr_QueryColorFormats
 * Description: Get color formats associated with the XA_IMAGECODEC_RAW codec.
 */
XAresult XACapabilitiesMgr_QueryColorFormats(XACapabilities* pListHead,
        XAuint32* pIndex, XAuint32* pColorFormats)
    {
    XAresult res = XA_RESULT_SUCCESS;
    XACapabilities temp;

    DEBUG_API("->XACapabilitiesMgr_QueryColorFormats");

    if (!pIndex)
        {
        DEBUG_ERR("illegal NULL parameter");
        res = XA_RESULT_PARAMETER_INVALID;
        return res;
        }

    res = XACapabilitiesMgr_GetCapsById(pListHead,
            (XACapsType) ((XACapsType) (XACAP_ENCODER | XACAP_IMAGE)),
            XA_IMAGECODEC_RAW, &temp);
    if (res == XA_RESULT_SUCCESS)
        {
        if (!pColorFormats)
            { /* query number of color formats */
            *pIndex = 1; /* one used by camera context */
            }
        else
            { /* query color format */
            if (*pIndex >= 1) /* one used by camera context */
                {
                DEBUG_ERR("index parameter invalid");
                res = XA_RESULT_PARAMETER_INVALID;
                }
            else
                { /* internal format used by pipeline, look camera context for details  */
                *pColorFormats = XA_COLORFORMAT_YUV420PLANAR;
                }
            }
        }
    else
        {
        *pIndex = 0;
        }

    DEBUG_API("<-XACapabilitiesMgr_QueryColorFormats");
    return res;
    }

/* Add AudioInputDeviceCaps */
XAresult XACapabilitiesMgr_GetAudioInputDeviceCapabilities(
        XACapabilities** ppNode)
    {
    XAresult res = XA_RESULT_SUCCESS;
    XACapabilities *newNode = NULL;
    XAAudioInputDescriptor *entries = NULL;
    XAchar micDeviceName[] = "Default Mic";
    int strLen = 0;

    newNode = (XACapabilities *) calloc(1, sizeof(XACapabilities));
    if (!newNode)
        {
        res = XA_RESULT_MEMORY_FAILURE;
        return res;
        }

    newNode->capsType = AUD_I;
    newNode->xaid = 0xAD7E5001;
    newNode->noOfEntries = 1;

    /* Allocate array */
    entries = (XAAudioInputDescriptor*) calloc(1,
            sizeof(XAAudioInputDescriptor));
    if (!entries)
        {
        res = XA_RESULT_MEMORY_FAILURE;
        XACapabilitiesMgr_DeleteCapabilitieList(&newNode);
        return res;
        }

    newNode->pEntry = (void*) entries;

    strLen = strlen((char*) micDeviceName);
    entries->deviceName = (XAchar *) calloc(strLen + 1, sizeof(XAchar));
    if (!entries->deviceName)
        {
        res = XA_RESULT_MEMORY_FAILURE;
        XACapabilitiesMgr_DeleteCapabilitieList(&newNode);
        return res;
        }

    strncpy((char*) entries->deviceName, (char*) micDeviceName, strLen);
    entries->deviceName[strLen] = '\0'; /*Null terminate it*/
    entries->deviceConnection = XA_DEVCONNECTION_INTEGRATED;
    entries->deviceScope = XA_DEVSCOPE_ENVIRONMENT;
    entries->deviceLocation = XA_DEVLOCATION_HANDSET;
    entries->isForTelephony = XA_BOOLEAN_FALSE;
    entries->minSampleRate = 8000000; /* milliHz */
    entries->maxSampleRate = 96000000; /* milliHz */
    entries->isFreqRangeContinuous = XA_BOOLEAN_FALSE;
    entries->numOfSamplingRatesSupported = 12;
    entries->samplingRatesSupported = (XAmilliHertz*) calloc(
            entries->numOfSamplingRatesSupported, sizeof(XAmilliHertz));
    if (!entries->samplingRatesSupported)
        {
        res = XA_RESULT_MEMORY_FAILURE;
        XACapabilitiesMgr_DeleteCapabilitieList(&newNode);
        return res;
        }
    /* entries in milliHz */
    entries->samplingRatesSupported[0] = 8000000;
    entries->samplingRatesSupported[1] = 11025000;
    entries->samplingRatesSupported[2] = 12000000;
    entries->samplingRatesSupported[3] = 16000000;
    entries->samplingRatesSupported[4] = 22050000;
    entries->samplingRatesSupported[5] = 24000000;
    entries->samplingRatesSupported[6] = 32000000;
    entries->samplingRatesSupported[7] = 44100000;
    entries->samplingRatesSupported[8] = 48000000;
    entries->samplingRatesSupported[9] = 64000000;
    entries->samplingRatesSupported[10] = 88200000;
    entries->samplingRatesSupported[11] = 96000000;
    entries->maxChannels = 2;

    newNode->pEntry = (void*) entries;

    *ppNode = newNode;
    return res;
    }

XAresult XACapabilitiesMgr_GetAudioOutputDeviceCapabilities(
        XACapabilities **ppNode)
    {
    XAresult res = XA_RESULT_SUCCESS;
    XACapabilities *newNode = NULL;
    XAAudioOutputDescriptor *entries = NULL;
    XAchar outputDeviceName[] = "Default Speaker";
    int strLen = 0;

    newNode = (XACapabilities *) calloc(1, sizeof(XACapabilities));
    if (!newNode)
        {
        res = XA_RESULT_MEMORY_FAILURE;
        return res;
        }

    newNode->capsType = AUD_O;
    newNode->xaid = 0xAD7E5002;
    newNode->noOfEntries = 1;

    /* Allocate array */
    entries = (XAAudioOutputDescriptor*) calloc(1,
            sizeof(XAAudioOutputDescriptor));
    if (!entries)
        {
        res = XA_RESULT_MEMORY_FAILURE;
        XACapabilitiesMgr_DeleteCapabilitieList(&newNode);
        return res;
        }

    newNode->pEntry = (void*) entries;

    strLen = strlen((char*) outputDeviceName);
    entries->pDeviceName = (XAchar *) calloc(strLen + 1, sizeof(XAchar));
    if (!entries->pDeviceName)
        {
        res = XA_RESULT_MEMORY_FAILURE;
        XACapabilitiesMgr_DeleteCapabilitieList(&newNode);
        return res;
        }
    strncpy((char*) entries->pDeviceName, (char*) outputDeviceName, strLen);
    entries->pDeviceName[strLen] = '\0'; /*Null terminate it*/
    entries->deviceConnection = XA_DEVCONNECTION_INTEGRATED;
    entries->deviceScope = XA_DEVSCOPE_ENVIRONMENT;
    entries->deviceLocation = XA_DEVLOCATION_HANDSET;
    entries->isForTelephony = XA_BOOLEAN_FALSE;
    entries->minSampleRate = 8000000; /* milliHz */
    entries->maxSampleRate = 96000000; /* milliHz */
    entries->isFreqRangeContinuous = XA_BOOLEAN_FALSE;
    entries->numOfSamplingRatesSupported = 12;
    entries->samplingRatesSupported = (XAmilliHertz*) calloc(
            entries->numOfSamplingRatesSupported, sizeof(XAmilliHertz));
    if (!entries->samplingRatesSupported)
        {
        res = XA_RESULT_MEMORY_FAILURE;
        XACapabilitiesMgr_DeleteCapabilitieList(&newNode);
        return res;
        }
    /* entries in milliHz */
    entries->samplingRatesSupported[0] = 8000000;
    entries->samplingRatesSupported[1] = 11025000;
    entries->samplingRatesSupported[2] = 12000000;
    entries->samplingRatesSupported[3] = 16000000;
    entries->samplingRatesSupported[4] = 22050000;
    entries->samplingRatesSupported[5] = 24000000;
    entries->samplingRatesSupported[6] = 32000000;
    entries->samplingRatesSupported[7] = 44100000;
    entries->samplingRatesSupported[8] = 48000000;
    entries->samplingRatesSupported[9] = 64000000;
    entries->samplingRatesSupported[10] = 88200000;
    entries->samplingRatesSupported[11] = 96000000;
    entries->maxChannels = 2;

    newNode->pEntry = (void*) entries;

    *ppNode = newNode;
    return res;
    }

