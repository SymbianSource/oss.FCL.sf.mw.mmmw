/**
 * src\adaptation\XAStreamInformationItfAdaptation.c
 * Part of OpenMAX AL Implementation
 * Copyright (c) 2009 Nokia Inc.
 *
 * This file contains implementation of XAStreamInformationItfAdaptation
 **/

#include "xadebug.h"
#include "xathreadsafety.h"
#include "xaadaptationgst.h"
#include "xamediaplayeradaptctx.h"
#include "xastreaminformationitfadaptation.h"
#include "xaplayitfadaptation.h"

/* local functions */
XAresult GetDomainTypeFromCodecBinSrcPads(
                    GList *sourcePads,
                    XAuint32 containerType,
                    XAuint32 streamIndex,
                    XAuint32 *domain);

XAresult GetAudioStreamInfoFromPads(
                    GstPad *srcPad,
                    XAuint32 *channels,
                    XAuint32 *sampleRate,
                    XAuint32 *bitRate);

XAresult GetVideoStreamInfoFromPads(
                    GstPad *srcPad,
                    XAuint32 *width,
                    XAuint32 *height,
                    XAuint32 *frameRate,
                    XAuint32 *bitRate);

XAresult GetImageStreamInfoFromPads(
                    GstPad *srcPad,
                    XAuint32 *width,
                    XAuint32 *height);

/*
 * XAStreamInformationItfAdapt_QueryMediaContainerInformation(
 *   XAAdaptationBaseCtx *adaptCtx,
 *   XAMediaContainerInformation *info);
 * Description: Obtains media container information
 */
XAresult  XAStreamInformationItfAdapt_QueryMediaContainerInformation(
    XAAdaptationBaseCtx *adaptCtx,
    XAuint32 *containerType,
    XAmillisecond *mediaDuration,
    XAuint32 *numStreams)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XADataSource *xaDataSource = NULL;
    GstElement *codecbin = NULL;

    DEBUG_API("->XAStreamInformationItfAdapt_QueryMediaContainerInformation");
    /* Both Media Player and Metadata Extractor objec uses the same ctxId */
    if( !adaptCtx ||
        (adaptCtx->ctxId != XAMediaPlayerAdaptation) )
/*        !((adaptCtx->ctxId == XAMediaPlayerAdaptation) ||
         (adaptCtx->ctxId == XAMDAdaptation) ) )
*/
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("-<XAStreamInformationItfAdapt_QueryMediaContainerInformation");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* Get media container type */
    DEBUG_INFO_A2("Adaptation Context- ID is [%u] Extended ID[%u]",
                (unsigned int)adaptCtx->ctxId,
                (unsigned int)((XAMediaPlayerAdaptationCtx*) adaptCtx)->extendedCtxId);

    /* Initialize to unspecified */
    *containerType = XA_CONTAINERTYPE_UNSPECIFIED;
    xaDataSource = ((XAMediaPlayerAdaptationCtx*) adaptCtx)->xaSource;
/*    DEBUG_INFO("Obtained handle to xaDataSource");*/
    if ( xaDataSource &&
         xaDataSource->pFormat &&
         ( (*(XAuint32*)xaDataSource->pFormat ) == XA_DATAFORMAT_MIME) )
    {
        *containerType = ((XADataFormat_MIME*)(xaDataSource->pFormat))->containerType;
        DEBUG_INFO_A1("Container type obtained from xaDataSource->pFormat is [%u]", (unsigned int)(*containerType));
    }
    else
    {
        DEBUG_INFO("Container type unspecified");
    }

    /* Get media duration */
    ret = XAPlayItfAdaptGST_GetDuration((XAAdaptationGstCtx*)adaptCtx, mediaDuration);
    if ( ret != XA_RESULT_SUCCESS )
        return ret;

    /* Determine number of streams */
    codecbin = ((XAMediaPlayerAdaptationCtx*) adaptCtx)->codecbin;
    if ( codecbin == NULL )
    {
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("-<XAStreamInformationItfAdapt_QueryMediaContainerInformation");
        return XA_RESULT_INTERNAL_ERROR;
    }
    *numStreams = codecbin->numsrcpads;

    DEBUG_API("-<XAStreamInformationItfAdapt_QueryMediaContainerInformation");
    return XA_RESULT_SUCCESS;
}

/*
 * XAresult XAStreamInformationItfAdapt_QueryStreamType(
 *               XAAdaptationBaseCtx *adaptCtx,
 *               XAuint32 streamIndex,
 *               XAuint32 *domain)
 * Description: Obtains stream type information
 */
XAresult XAStreamInformationItfAdapt_QueryStreamType(
                XAAdaptationBaseCtx *adaptCtx,
                XAuint32 streamIndex,
                XAuint32 *domain)
{
    XAresult ret;
    XAuint32 containerType;
    XAmillisecond mediaDuration;
    XAuint32 numStreams;
    XADataSource *xaDataSource = NULL;


    DEBUG_API("->XAStreamInformationItfAdapt_QueryStreamType");

    ret = XAStreamInformationItfAdapt_QueryMediaContainerInformation(
            adaptCtx, &containerType, &mediaDuration, &numStreams);
    if (ret != XA_RESULT_SUCCESS)
    {
        DEBUG_ERR_A1("XA_RESULT_[0x%x]", (unsigned int)ret);
        DEBUG_API("-<XAStreamInformationItfAdapt_QueryStreamType");
        return ret;
    }

    /* validate stream index */
    if ( streamIndex > numStreams )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("-<XAStreamInformationItfAdapt_QueryStreamType");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* if format type is raw image */
    xaDataSource = ((XAMediaPlayerAdaptationCtx*) adaptCtx)->xaSource;
    if( xaDataSource->pFormat &&
      ( (*(XAuint32*)xaDataSource->pFormat ) == XA_DATAFORMAT_RAWIMAGE) )
    {
        *domain = XA_DOMAINTYPE_IMAGE;
        return XA_RESULT_SUCCESS;
    }
    else
    {
        GstElement *codecbin = ((XAMediaPlayerAdaptationCtx*) adaptCtx)->codecbin;
        if ( codecbin == NULL )
        {
            DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
            DEBUG_API("-<XAStreamInformationItfAdapt_QueryStreamType");
            return XA_RESULT_INTERNAL_ERROR;
        }

        GST_OBJECT_LOCK( codecbin );
        ret = GetDomainTypeFromCodecBinSrcPads(
                    codecbin->srcpads,
                    containerType,
                    streamIndex,
                    domain);
        GST_OBJECT_UNLOCK( codecbin );
    }

    DEBUG_API("-<XAStreamInformationItfAdapt_QueryStreamType");
    return ret;
}

/*
 * XAresult XAStreamInformationItfAdapt_QueryStreamInformation(
 *               XAAdaptationBaseCtx *adaptCtx,
 *               XAuint32 streamIndex,
 *               void * info)
 * Description: Obtains stream information
 */
XAresult XAStreamInformationItfAdapt_QueryStreamInformation(
                XAAdaptationBaseCtx *adaptCtx,
                XAuint32 streamIndex,
                void * info)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAuint32 domain;
    XAuint32 codecId = 0;
    XAuint32 channels;
    XAmilliHertz sampleRate;
    XAuint32 bitRate;
    XAmillisecond duration;
    XAuint32 width;
    XAuint32 height;
    XAuint32 frameRate;
    /*TODO fix this call below in capabilitymgr*/
    /*XACapsType filter;*/
    GstElement *codecbin = NULL;

    DEBUG_API("->XAStreamInformationItfAdapt_QueryStreamInformation");

    /* stream index is validated in XAStreamInformationItfAdapt_QueryStreamType */
    ret = XAStreamInformationItfAdapt_QueryStreamType(
            adaptCtx, streamIndex, &domain);

    if (ret != XA_RESULT_SUCCESS)
    {
        DEBUG_ERR_A1("XA_RESULT_[0x%x]", (unsigned int)ret);
        DEBUG_API("-<XAStreamInformationItfAdapt_QueryStreamInformation");
        return ret;
    }
    if ( domain == XA_DOMAINTYPE_UNKNOWN )
    {
        DEBUG_ERR("XA_RESULT_CONTENT_UNSUPPORTED");
        DEBUG_API("-<XAStreamInformationItfAdapt_QueryStreamInformation");
        return XA_RESULT_CONTENT_UNSUPPORTED;
    }

    codecbin = ((XAMediaPlayerAdaptationCtx*) adaptCtx)->codecbin;
    if ( codecbin == NULL )
    {
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("-<XAStreamInformationItfAdapt_QueryStreamInformation");
        return XA_RESULT_INTERNAL_ERROR;
    }

    /* For midi none of the properties can be queried from gstreamer */
    if ( domain != XA_DOMAINTYPE_MIDI )
    {
        GstPad* srcPad = NULL;
        gchar* codecbinFactoryName = NULL;

        GST_OBJECT_LOCK( codecbin );
        srcPad = (GstPad*) g_list_nth_data(codecbin->srcpads, (streamIndex -1 ));
        DEBUG_INFO_A1("Obtaining SrcPad%u from Decode bin", (unsigned int)(streamIndex-1));
        if (srcPad == NULL)
        {
            GST_OBJECT_UNLOCK( codecbin );
            DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
            DEBUG_API("-<QueryStreamInformation");
            return XA_RESULT_INTERNAL_ERROR;
        }
        switch (domain)
        {
            /*TODO fix this call below in capabilitymgr*/
            /*case XA_DOMAINTYPE_AUDIO:
                filter = XACAP_AUDIO;
                break;
            case XA_DOMAINTYPE_VIDEO:
                filter = XACAP_VIDEO;
                break;
            case XA_DOMAINTYPE_IMAGE:
                filter = XACAP_IMAGE;
                break;*/
        };

        codecbinFactoryName = gst_element_get_name(gst_element_get_factory (codecbin));
        /* decode bin has the sink pads ghosted from targetPad.
        So retreive the name of the original decoder element*/
        if ( strcmp(codecbinFactoryName, "decodebin") == 0 )
        {
            GstPad* targetPad = NULL;
            gchar* decoderElementName = NULL; /* for retreiving codec id*/

            DEBUG_INFO("Decodebin: Obtaining target pad of SrcPad");
            targetPad = gst_ghost_pad_get_target ((GstGhostPad*)srcPad);
            decoderElementName = gst_element_get_name(gst_element_get_factory (gst_pad_get_parent_element(targetPad)));
            /*TODO fix this call below in capabilitymgr*/
            /*
            ret = XAStaticCapsAdapt_GetCodecIDByPluginName(
                        filter,
                        (XAchar*)decoderElementName,
                        &codecId);
             */
            gst_object_unref(targetPad);
            g_free(decoderElementName);
        }
        else /* for identity, audioparse and videoparse elements */
        {
            DEBUG_INFO("Not decode bin, but a passthru element");
            /*TODO fix this call below in capabilitymgr*/
/*            ret = XAStaticCapsAdapt_GetCodecIDByPluginName(
                        filter,
                        (XAchar*)codecbinFactoryName,
                        &codecId);*/
        }
        g_free(codecbinFactoryName);
        if ( ret == XA_RESULT_SUCCESS )
        {
            /* retreive srcPad properties before releasing lock on codecbin */
            switch(domain)
            {
                case XA_DOMAINTYPE_AUDIO:
                    DEBUG_INFO("Getting Audio Stream Info from SrcPad");
                    ret = GetAudioStreamInfoFromPads(
                                srcPad,
                                &channels,
                                &sampleRate,
                                &bitRate);
                    break;
                case XA_DOMAINTYPE_VIDEO:
                    DEBUG_INFO("Getting Video Stream Info from SrcPad");
                    ret = GetVideoStreamInfoFromPads(
                                srcPad,
                                &width,
                                &height,
                                &frameRate,
                                &bitRate);
                    break;
                case XA_DOMAINTYPE_IMAGE:
                    DEBUG_INFO("Getting Image Stream Info from SrcPad");
                    ret = GetImageStreamInfoFromPads(
                                srcPad,
                                &width,
                                &height
                                );
                    break;
                default:
                    break;
            };
        }
        /* Now unlock the bin */
        GST_OBJECT_UNLOCK( codecbin );
    }    /*if ( domain != XA_DOMAIN_TYPE_MIDI )*/

    if ( ret == XA_RESULT_SUCCESS)
    {
        DEBUG_INFO("Getting media duration");
        ret = XAPlayItfAdaptGST_GetDuration((XAAdaptationGstCtx*)adaptCtx, &duration);
    }

    /* Now assign properties to data structure */
    if ( ret == XA_RESULT_SUCCESS)
    {
        switch(domain)
        {
            case XA_DOMAINTYPE_AUDIO:
            {
                XAAudioStreamInformation* streamInfo = (XAAudioStreamInformation*)info;
                DEBUG_INFO("Assigning values for XAAudioStreamInformation");
                streamInfo->codecId = codecId;
                streamInfo->channels = channels;
                streamInfo->sampleRate = sampleRate;
                streamInfo->bitRate = bitRate;
                streamInfo->langCountry[0] = '\0';
                streamInfo->duration = duration;
            }
                break;
            case XA_DOMAINTYPE_MIDI:
            {
                XAMIDIStreamInformation* streamInfo = (XAMIDIStreamInformation*)info;
                DEBUG_INFO("Assigning values for XAMIDIStreamInformation");
                /*currently there is no way to get these information from gstreamer*/
                streamInfo->channels = XA_MIDI_UNKNOWN;
                streamInfo->tracks = XA_MIDI_UNKNOWN;
                streamInfo->bankType = XA_MIDIBANK_DEVICE;
                /*streamInfo->bitRate = 0;*/
                streamInfo->langCountry[0] = '\0';
                streamInfo->duration = duration;
            }
                break;
            case XA_DOMAINTYPE_VIDEO:
            {
                XAVideoStreamInformation* streamInfo = (XAVideoStreamInformation*)info;
                DEBUG_INFO("Assigning values for XAVideoStreamInformation");
                streamInfo->codecId = codecId;
                streamInfo->width = width;
                streamInfo->height = height;
                streamInfo->bitRate = bitRate;
                streamInfo->duration = duration;
            }
                break;
            case XA_DOMAINTYPE_IMAGE:
            {
                XAImageStreamInformation* streamInfo = (XAImageStreamInformation*)info;
                DEBUG_INFO("Assigning values for XAImageStreamInformation");
                streamInfo->codecId = codecId;
                streamInfo->width = width;
                streamInfo->height = height;
                streamInfo->presentationDuration = duration;
            }
                break;
            default:
                break;
        };
    } /*if ( ret == XA_RESULT_SUCCESS)*/

    DEBUG_API("-<XAStreamInformationItfAdapt_QueryStreamInformation");
    return ret;
}

/*
 * XAresult XAStreamInformationItfAdapt_QueryStreamName(
 *               XAAdaptationBaseCtx *adaptCtx,
 *               XAuint32 streamIndex,
 *               XAuint16 * pNameSize,
 *               XAchar * pName)
 * Description: Obtains stream name
 */
XAresult XAStreamInformationItfAdapt_QueryStreamName(
                XAAdaptationBaseCtx *adaptCtx,
                XAuint32 streamIndex,
                XAuint16 * pNameSize,
                XAchar * pName)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAuint32 numStreams;
    GstElement *codecbin = NULL;

    DEBUG_API("->XAStreamInformationItfAdapt_QueryStreamName");
    /* Both Media Player and Metadata Extractor objec uses the same ctxId */
    if( !adaptCtx ||
        (adaptCtx->ctxId != XAMediaPlayerAdaptation) )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("-<XAStreamInformationItfAdapt_QueryStreamName");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* validate stream index */
    codecbin = ((XAMediaPlayerAdaptationCtx*) adaptCtx)->codecbin;
    if ( codecbin == NULL )
    {
        DEBUG_INFO("Unable to get codec bin");
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("-<XAStreamInformationItfAdapt_QueryStreamName");
        return XA_RESULT_INTERNAL_ERROR;
    }
    numStreams = codecbin->numsrcpads;

    /* validate stream index */
    if ( streamIndex > numStreams )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("-<XAStreamInformationItfAdapt_QueryStreamName");
        return XA_RESULT_PARAMETER_INVALID;
    }

    /* TODO there is no straight way to retreive stream name from gstreamer*/
    /*XAuint16 inputNameSize = *pNameSize;*/
    *pNameSize = 0; /* Replace this with actual size*/
    if ( pName )
    {
        /* TODO copy stream name into pName taking into account inputNameSize */
    }

    DEBUG_API("-<XAStreamInformationItfAdapt_QueryStreamName");
    return ret;
}

/*
 * XAresult XAStreamInformationItfAdapt_QueryActiveStreams(
 *               XAAdaptationBaseCtx *adaptCtx,
 *               XAuint32 *numStreams,
 *               XAboolean *activeStreams);
 * Description: Obtains active stream information
 */
 XAresult XAStreamInformationItfAdapt_QueryActiveStreams(
                XAAdaptationBaseCtx *adaptCtx,
                XAuint32 *numStreams,
                XAboolean *activeStreams)
{
    XAuint32 inputNumStreams;
    GstElement *codecbin;

    DEBUG_API("->XAStreamInformationItfAdapt_QueryActiveStreams");

    /* Both Media Player and Metadata Extractor objec uses the same ctxId */
    if( !adaptCtx ||
        (adaptCtx->ctxId != XAMediaPlayerAdaptation) )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("-<XAStreamInformationItfAdapt_QueryActiveStreams");
        return XA_RESULT_PARAMETER_INVALID;
    }

    if( adaptCtx->ctxId ==  XAMDAdaptation)
    {
        DEBUG_ERR("XA_RESULT_PRECONDITIONS_VIOLATED");
        DEBUG_API("-<XAStreamInformationItfAdapt_QueryActiveStreams");
        return XA_RESULT_PRECONDITIONS_VIOLATED;
    }

    inputNumStreams = *numStreams;

    /* Determine number of streams */
    codecbin = ((XAMediaPlayerAdaptationCtx*) adaptCtx)->codecbin;
    if ( codecbin == NULL )
    {
        DEBUG_INFO("Unable to get codec bin");
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("-<XAStreamInformationItfAdapt_QueryActiveStreams");
        return XA_RESULT_INTERNAL_ERROR;
    }
    *numStreams = codecbin->numsrcpads;

    if ( activeStreams )
    {
        XAuint32 loopIndex;
        if (inputNumStreams < *numStreams )
        {
            DEBUG_INFO("numStreams param value is smaller than number of streams");
            DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
            DEBUG_API("-<XAStreamInformationItfAdapt_QueryActiveStreams");
            return XA_RESULT_PARAMETER_INVALID;
        }
        for (loopIndex = 0; loopIndex < *numStreams; loopIndex++)
        {
            /* TODO currently gstreamer doesn't allow multiple dynamic streams
             * all streams are active streams */
            activeStreams[loopIndex] = XA_BOOLEAN_TRUE;
        }
    }

    DEBUG_API("->XAStreamInformationItfAdapt_QueryActiveStreams");
    return XA_RESULT_SUCCESS;
}

XAresult XAStreamInformationItfAdapt_SetActiveStream(
                XAAdaptationBaseCtx *adaptCtx,
                XAuint32 streamNum,
                XAboolean active,
                XAboolean commitNow)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAuint32 numStreams;
    GstElement *codecbin;

    DEBUG_API("->XAStreamInformationItfAdapt_SetActiveStream");

    /* Both Media Player and Metadata Extractor objec uses the same ctxId */
    if( !adaptCtx ||
        (adaptCtx->ctxId != XAMediaPlayerAdaptation) )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("-<XAStreamInformationItfAdapt_SetActiveStream");
        return XA_RESULT_PARAMETER_INVALID;
    }

    if( adaptCtx->ctxId ==  XAMDAdaptation)
    {
        DEBUG_ERR("XA_RESULT_PRECONDITIONS_VIOLATED");
        DEBUG_API("-<XAStreamInformationItfAdapt_SetActiveStream");
        return XA_RESULT_PRECONDITIONS_VIOLATED;
    }

    /* Determine number of streams */
    codecbin = ((XAMediaPlayerAdaptationCtx*) adaptCtx)->codecbin;
    if ( codecbin == NULL )
    {
        DEBUG_INFO("Unable to get codec bin");
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("-<XAStreamInformationItfAdapt_SetActiveStream");
        return XA_RESULT_INTERNAL_ERROR;
    }
    numStreams = codecbin->numsrcpads;

    /* validate stream index */
    if ( streamNum > numStreams )
    {
        DEBUG_ERR("XA_RESULT_PARAMETER_INVALID");
        DEBUG_API("-<XAStreamInformationItfAdapt_SetActiveStream");
        return XA_RESULT_PARAMETER_INVALID;
    }

    ret = XA_RESULT_FEATURE_UNSUPPORTED;

    DEBUG_API("->XAStreamInformationItfAdapt_SetActiveStream");
    return ret;
}

/* local functions */

XAresult GetDomainTypeFromCodecBinSrcPads(
                    GList *sourcePads,
                    XAuint32 containerType,
                    XAuint32 streamIndex,
                    XAuint32 *domain)
{
    GstPad* srcPad;
    GstCaps* negotiatedCaps;
    const GstStructure* gstStruct;
    const gchar *mime;

    DEBUG_API("->GetDomainTypeFromCodecBinSrcPads");


    srcPad = (GstPad*) g_list_nth_data(sourcePads, (streamIndex -1 ));
    if (srcPad == NULL)
    {
        DEBUG_INFO_A1("Error obtaining SrcPad%u", (unsigned int)streamIndex);
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("-<GetDomainTypeFromCodecBinSrcPads");
        return XA_RESULT_INTERNAL_ERROR;
    }
    DEBUG_INFO_A1("Obtained SrcPad%u from SrcPads list", (unsigned int)(streamIndex-1));

    negotiatedCaps = gst_pad_get_negotiated_caps(srcPad);
    if ( negotiatedCaps == NULL )
    {
        DEBUG_INFO("Obtaining allowed caps from SrcPad.");
        negotiatedCaps = gst_pad_get_allowed_caps (srcPad);
    }

    if ( negotiatedCaps == NULL )
    {
        DEBUG_INFO("Unable to obtain negotiated and allowed caps from SrcPad");
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("-<GetDomainTypeFromCodecBinSrcPads");
        return XA_RESULT_INTERNAL_ERROR;
    }

    DEBUG_INFO("Obtained negotiated/allowed GstCaps from SrcPad");

    gstStruct = gst_caps_get_structure (negotiatedCaps, 0);
    if ( gstStruct == NULL )
    {
        gst_object_unref (negotiatedCaps);
        DEBUG_INFO("Error getting GstStructure from GstCaps");
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("-<GetDomainTypeFromCodecBinSrcPads");
        return XA_RESULT_INTERNAL_ERROR;
    }
    DEBUG_INFO("Obtained GstStructure from GstCaps");

    mime = gst_structure_get_name (gstStruct);

    DEBUG_INFO_A1("MimeType associated with SrcPad is [%s]", mime);

    *domain = XA_DOMAINTYPE_UNKNOWN;
    if (strncmp (mime, "audio", 5) == 0)
    {
        gchar* elementFactoryName = NULL;

        *domain = XA_DOMAINTYPE_AUDIO;

        /*check if it is midi or not by checking the sink pad's mime-type*/
        /* this should done by checking the sinkpad's mime-type of parent element */
        elementFactoryName = gst_element_get_name(gst_element_get_factory (gst_pad_get_parent_element(srcPad)));

        /*printf("Element factory name [%s]\n", elementFactoryName);*/

        /* decode bin has the sink pads ghosted from targetPad.
        So retreive the name of the original decoder element*/
        if ( strcmp(elementFactoryName, "decodebin") == 0 )
        {
            GstPad* targetPad = NULL;
            gchar* decoderElementName = NULL;

            DEBUG_INFO("Decodebin: Obtaining target pad of SrcPad");
            targetPad = gst_ghost_pad_get_target ((GstGhostPad*)srcPad);

            decoderElementName = gst_element_get_name (gst_element_get_factory (gst_pad_get_parent_element (targetPad)));
            /* better solution would be to get mime-type from decoder and decide based on that */
            /* currently we cannot retrieive sinkpad's caps. Perhaps a bug in gstreamer. We need */
            /* to revisit this when we move to newer version of gstreamer */
            if ( strcmp(decoderElementName, "wildmidi") == 0 )
            {
                *domain = XA_DOMAINTYPE_MIDI;
            }
            g_free(decoderElementName);
            gst_object_unref(targetPad);
        }
        g_free(elementFactoryName);

    }
    else if (strncmp (mime, "video", 5) == 0)
    {
        switch(containerType)
        {
            case XA_CONTAINERTYPE_BMP:
            case XA_CONTAINERTYPE_JPG:
            case XA_CONTAINERTYPE_JPG2000:
                *domain = XA_DOMAINTYPE_IMAGE;
                break;
            default:
                *domain = XA_DOMAINTYPE_VIDEO;
                break;
        };
    }
    /* TODO find out why this causes assertion failure */
    /*gst_object_unref (negotiatedCaps);*/

    DEBUG_API("-<GetDomainTypeFromCodecBinSrcPads");

    return XA_RESULT_SUCCESS;
}

XAresult GetAudioStreamInfoFromPads(
                    GstPad *srcPad,
                    XAuint32 *channels,
                    XAuint32 *sampleRate,
                    XAuint32 *bitRate)
{
    GstCaps* negotiatedCaps;
    const GstStructure* gstStruct;
    gint negotiated_sampleRate;
    gint negotiated_channels;
    gint negotiated_bitRate;

    DEBUG_API("->GetAudioStreamInfoFromPads");

    negotiatedCaps = gst_pad_get_negotiated_caps(srcPad);
    if ( negotiatedCaps == NULL )
    {
        DEBUG_INFO("Obtaining allowed caps from SrcPad.");
        negotiatedCaps = gst_pad_get_allowed_caps (srcPad);
    }

    if ( negotiatedCaps == NULL )
    {
        DEBUG_INFO("Unable to obtain negotiated and allowed caps from SrcPad");
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("-<GetAudioStreamInfoFromPads");
        return XA_RESULT_INTERNAL_ERROR;
    }

    DEBUG_INFO("Obtained negotiated/allowed GstCaps from SrcPad");

    gstStruct = gst_caps_get_structure (negotiatedCaps, 0);
    if ( gstStruct == NULL )
    {
        gst_object_unref (negotiatedCaps);
        DEBUG_INFO("Error getting GstStructure from GstCaps");
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("-<GetAudioStreamInfoFromPads");
        return XA_RESULT_INTERNAL_ERROR;
    }
    DEBUG_INFO("Obtained GstStructure from GstCaps");

    *channels = 0;
    *sampleRate = 0;
    *bitRate = 0;

    gst_structure_get_int (gstStruct, "channels", &negotiated_channels);
    gst_structure_get_int (gstStruct, "rate", &negotiated_sampleRate);
    gst_structure_get_int (gstStruct, "bitrate", &negotiated_bitRate);

    *channels = negotiated_channels;
    *sampleRate = (XAmilliHertz) negotiated_sampleRate * 1000;
    *bitRate = negotiated_bitRate;

    DEBUG_INFO_A1("channels[%u]", (unsigned int)*channels);
    DEBUG_INFO_A1("sampleRate[%u]", (unsigned int)*sampleRate);
    DEBUG_INFO_A1("bitRate[%u]", (unsigned int)*bitRate);
    /* TODO find out why this is causing assertion failure
    gst_object_unref (negotiatedCaps);*/

    DEBUG_API("-<GetAudioStreamInfoFromPads");

    return XA_RESULT_SUCCESS;
}

XAresult GetVideoStreamInfoFromPads(
                    GstPad *srcPad,
                    XAuint32 *width,
                    XAuint32 *height,
                    XAuint32 *frameRate,
                    XAuint32 *bitRate)
{
    GstCaps* negotiatedCaps;
    const GstStructure* gstStruct;
    gint negotiated_width;
    gint negotiated_height;
    gint negotiated_frameRate_numerator;
    gint negotiated_frameRate_denominator;
    gint negotiated_bitRate;

    DEBUG_API("->GetVideoStreamInfoFromPads");

    negotiatedCaps = gst_pad_get_negotiated_caps(srcPad);
    if ( negotiatedCaps == NULL )
    {
        DEBUG_INFO("Obtaining allowed caps from SrcPad.");
        negotiatedCaps = gst_pad_get_allowed_caps (srcPad);
    }

    if ( negotiatedCaps == NULL )
    {
        DEBUG_INFO("Unable to obtain negotiated and allowed caps from SrcPad");
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("-<GetVideoStreamInfoFromPads");
        return XA_RESULT_INTERNAL_ERROR;
    }

    DEBUG_INFO("Obtained negotiated/allowed GstCaps from SrcPad");

    gstStruct = gst_caps_get_structure (negotiatedCaps, 0);
    if ( gstStruct == NULL )
    {
        gst_object_unref (negotiatedCaps);
        DEBUG_INFO("Error getting GstStructure from GstCaps");
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("-<GetVideoStreamInfoFromPads");
        return XA_RESULT_INTERNAL_ERROR;
    }
    DEBUG_INFO("Obtained GstStructure from GstCaps");

    *width = 0;
    *height = 0;
    *frameRate = 0;
    *bitRate = 0;

    gst_structure_get_int (gstStruct, "width", &negotiated_width);
    gst_structure_get_int (gstStruct, "height", &negotiated_height);
    gst_structure_get_fraction (gstStruct, "framerate",
                                    &negotiated_frameRate_numerator,
                                    &negotiated_frameRate_denominator);
    gst_structure_get_int (gstStruct, "bitrate", &negotiated_bitRate);

    *width = negotiated_width;
    *height = negotiated_height;
    *bitRate = negotiated_bitRate;

    DEBUG_INFO_A1("width[%u]", (unsigned int)*width);
    DEBUG_INFO_A1("height[%u]", (unsigned int)*height);
    DEBUG_INFO_A2("framerate[%u/%u]", (unsigned int)negotiated_frameRate_numerator, (unsigned int)negotiated_frameRate_denominator);
    DEBUG_INFO_A1("bitrate[%u]", (unsigned int)*bitRate);
    /* TODO find out why this is causing assertion failure
    gst_object_unref (negotiatedCaps);*/

    DEBUG_API("-<GetVideoStreamInfoFromPads");

    return XA_RESULT_SUCCESS;
}


XAresult GetImageStreamInfoFromPads(
                    GstPad *srcPad,
                    XAuint32 *width,
                    XAuint32 *height)
{
    GstCaps* negotiatedCaps;
    const GstStructure* gstStruct;
    gint negotiated_width;
    gint negotiated_height;

    DEBUG_API("->GetImageStreamInfoFromPads");

    negotiatedCaps = gst_pad_get_negotiated_caps(srcPad);
    if ( negotiatedCaps == NULL )
    {
        DEBUG_INFO("Obtaining allowed caps from SrcPad.");
        negotiatedCaps = gst_pad_get_allowed_caps (srcPad);
    }

    if ( negotiatedCaps == NULL )
    {
        DEBUG_INFO("Unable to obtain negotiated and allowed caps from SrcPad");
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("-<GetImageStreamInfoFromPads");
        return XA_RESULT_INTERNAL_ERROR;
    }

    DEBUG_INFO("Obtained negotiated/allowed GstCaps from SrcPad");

    gstStruct = gst_caps_get_structure (negotiatedCaps, 0);
    if ( gstStruct == NULL )
    {
        gst_object_unref (negotiatedCaps);
        DEBUG_INFO("Error getting GstStructure from GstCaps");
        DEBUG_ERR("XA_RESULT_INTERNAL_ERROR");
        DEBUG_API("-<GetImageStreamInfoFromPads");
        return XA_RESULT_INTERNAL_ERROR;
    }
    DEBUG_INFO("Obtained GstStructure from GstCaps");

    *width = 0;
    *height = 0;

    gst_structure_get_int (gstStruct, "width", &negotiated_width);
    gst_structure_get_int (gstStruct, "height", &negotiated_height);

    *width = negotiated_width;
    *height = negotiated_height;

    DEBUG_INFO_A1("width[%u]", (unsigned int)*width);
    DEBUG_INFO_A1("height[%u]", (unsigned int)*height);
    /* TODO find out why this is causing assertion failure
    gst_object_unref (negotiatedCaps);*/

    DEBUG_API("-<GetImageStreamInfoFromPads");

    return XA_RESULT_SUCCESS;
}

