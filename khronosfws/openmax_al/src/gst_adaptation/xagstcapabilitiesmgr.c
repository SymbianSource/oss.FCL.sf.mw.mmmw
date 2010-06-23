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
#include "xagstcapabilitiesmgr.h"
#include "xaframeworkmgr.h"

static XAresult XAGSTCapabilitiesMgr_GetAudioAACEncoderCapabilities(
        XACapabilities **ppNode);
static XAresult XAGSTCapabilitiesMgr_GetAudioAMREncoderCapabilities(
        XACapabilities **ppNode);
static XAresult XAGSTCapabilitiesMgr_GetAudioPCMEncoderCapabilities(
        XACapabilities **ppNode);

/* XAresult XAGSTCapabilitiesMgr_UpdateCapabilitieList
 * Description: Update the capabilities list supported by GStreamer framework.
 */
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

    entries->maxChannels = 2;
    entries->minBitsPerSample = 16;
    entries->maxBitsPerSample = 16;
    entries->minSampleRate = 8000000; /*milliHz*/
    entries->maxSampleRate = 48000000;
    entries->isFreqRangeContinuous = XA_BOOLEAN_FALSE;
    entries->numSampleRatesSupported = 7;
    entries->pSampleRatesSupported = (XAmilliHertz*) calloc(
            entries->numSampleRatesSupported, sizeof(XAmilliHertz));
    if (!entries->pSampleRatesSupported)
        {
        res = XA_RESULT_MEMORY_FAILURE;
        XACapabilitiesMgr_DeleteCapabilitieList(&newNode);
        return res;
        }
    /* entries in milliHz */
    entries->pSampleRatesSupported[0] = 8000000;
    entries->pSampleRatesSupported[1] = 11025000;
    entries->pSampleRatesSupported[2] = 16000000;
    entries->pSampleRatesSupported[3] = 22050000;
    entries->pSampleRatesSupported[4] = 32000000;
    entries->pSampleRatesSupported[5] = 44100000;
    entries->pSampleRatesSupported[6] = 48000000;

    entries->minBitRate = 32000;
    entries->maxBitRate = 256000;
    entries->isBitrateRangeContinuous = XA_BOOLEAN_FALSE;
    entries->numBitratesSupported = 8;
    entries->pBitratesSupported = (XAuint32*) calloc(
            entries->numBitratesSupported, sizeof(XAuint32));
    if (!entries->pBitratesSupported)
        {
        res = XA_RESULT_MEMORY_FAILURE;
        XACapabilitiesMgr_DeleteCapabilitieList(&newNode);
        return res;
        }
    (entries->pBitratesSupported)[0] = 32000;
    (entries->pBitratesSupported)[1] = 64000;
    (entries->pBitratesSupported)[2] = 96000;
    (entries->pBitratesSupported)[3] = 128000;
    (entries->pBitratesSupported)[4] = 160000;
    (entries->pBitratesSupported)[5] = 192000;
    (entries->pBitratesSupported)[6] = 224000;
    (entries->pBitratesSupported)[7] = 256000;

    entries->profileSetting = XA_AUDIOPROFILE_AAC_AAC;
    entries->modeSetting = XA_AUDIOMODE_AAC_LC;

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

    entries->maxChannels = 1;
    entries->minBitsPerSample = 8;
    entries->maxBitsPerSample = 8;
    entries->minSampleRate = 8000000; /*milliHz*/
    entries->maxSampleRate = 8000000;
    entries->isFreqRangeContinuous = XA_BOOLEAN_TRUE;
    entries->numSampleRatesSupported = 1;
    entries->minBitRate = 4750;
    entries->maxBitRate = 12200;
    entries->isBitrateRangeContinuous = XA_BOOLEAN_FALSE;
    entries->numBitratesSupported = 8;
    entries->pBitratesSupported = (XAuint32*) calloc(
            entries->numBitratesSupported, sizeof(XAuint32));
    if (!entries->pBitratesSupported)
        {
        res = XA_RESULT_MEMORY_FAILURE;
        XACapabilitiesMgr_DeleteCapabilitieList(&newNode);
        return res;
        }
    (entries->pBitratesSupported)[0] = 4750;
    (entries->pBitratesSupported)[1] = 5150;
    (entries->pBitratesSupported)[2] = 5900;
    (entries->pBitratesSupported)[3] = 6700;
    (entries->pBitratesSupported)[4] = 7400;
    (entries->pBitratesSupported)[5] = 7950;
    (entries->pBitratesSupported)[6] = 10200;
    (entries->pBitratesSupported)[7] = 12200;

    entries->profileSetting = XA_AUDIOPROFILE_AMR;
    entries->modeSetting = 0;

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

    entries->maxChannels = 2;
    entries->minBitsPerSample = 16;
    entries->maxBitsPerSample = 16;
    entries->minSampleRate = 8000000; /*milliHz*/
    entries->maxSampleRate = 96000000;
    entries->isFreqRangeContinuous = XA_BOOLEAN_FALSE;
    entries->numSampleRatesSupported = 10;
    entries->pSampleRatesSupported = (XAmilliHertz*) calloc(
            entries->numSampleRatesSupported, sizeof(XAmilliHertz));
    if (!entries->pSampleRatesSupported)
        {
        res = XA_RESULT_MEMORY_FAILURE;
        XACapabilitiesMgr_DeleteCapabilitieList(&newNode);
        return res;
        }
    /* entries in milliHz */
    entries->pSampleRatesSupported[0] = 12000000;
    entries->pSampleRatesSupported[1] = 16000000;
    entries->pSampleRatesSupported[2] = 22050000;
    entries->pSampleRatesSupported[3] = 24000000;
    entries->pSampleRatesSupported[4] = 32000000;
    entries->pSampleRatesSupported[5] = 44100000;
    entries->pSampleRatesSupported[6] = 48000000;
    entries->pSampleRatesSupported[7] = 64000000;
    entries->pSampleRatesSupported[8] = 88200000;
    entries->pSampleRatesSupported[9] = 96000000;

    entries->minBitRate = 0;
    entries->maxBitRate = 0;
    entries->isBitrateRangeContinuous = XA_BOOLEAN_FALSE;
    entries->pBitratesSupported = NULL;
    entries->numBitratesSupported = 0;
    entries->profileSetting = XA_AUDIOPROFILE_PCM;
    entries->modeSetting = 0;

    newNode->pEntry = (void*) entries;

    *ppNode = newNode;
    return res;
    }
