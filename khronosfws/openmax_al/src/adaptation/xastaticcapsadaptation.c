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

#include <gst.h>
#include <string.h>
#include <gstparamspecs.h>
#include "XAStaticCapsAdaptation.h"

#define AUD_D (XACAP_DECODER|XACAP_AUDIO)
#define AUD_E (XACAP_ENCODER|XACAP_AUDIO)
#define VID_D (XACAP_DECODER|XACAP_VIDEO)
#define VID_E (XACAP_ENCODER|XACAP_VIDEO)
#define IMG_D (XACAP_DECODER|XACAP_IMAGE)
#define IMG_E (XACAP_ENCODER|XACAP_IMAGE)
#define AUD_O (XACAP_DEVSNK|XACAP_AUDIO)
#define AUD_I (XACAP_DEVSRC|XACAP_AUDIO)
#define VID_O (XACAP_DEVSNK|XACAP_VIDEO)
#define VID_I (XACAP_DEVSRC|XACAP_VIDEO)
#define ANY 0x7FFFFFFF


/* initially contains default "any"values, later filled in by querying GStreamer plugins */
static XAStaticCapsData allCaps[] =
    {/* { type,  id,                        adaptId,            maxCh, minBPS, maxBPS, minSR, maxSR, minBR, maxBR, maxFR, maxW, maxH, extra }, */
     //   { AUD_D, XA_AUDIOCODEC_PCM,         (XAchar *)"audioparse",       0,0,0,0,0,0,0,0,0,0,0,0 },
        { AUD_E, XA_AUDIOCODEC_PCM,         NULL,                         0,0,0,0,0,0,0,0,0,0,0,0 },
     //   { AUD_D, XA_ADAPTID_VORBIS,         (XAchar *)"vorbisdec",        0,0,0,0,0,0,0,0,0,0,0,0 },
     //   { AUD_E, XA_ADAPTID_VORBIS,         (XAchar *)"vorbisenc",        0,0,0,0,0,0,0,0,0,0,0,0 },
     //   { VID_D, XA_ADAPTID_MOTIONJPEG,     (XAchar *)"jpegdec",          0,0,0,0,0,0,0,0,0,0,0,0 },
     //   { VID_E, XA_ADAPTID_MOTIONJPEG,     (XAchar *)"jpegenc",          0,0,0,0,0,0,0,0,0,0,0,0 },
     //   { VID_D, XA_ADAPTID_THEORA,         (XAchar *)"theoradec",        0,0,0,0,0,0,0,0,0,0,0,0 },
     //  { VID_E, XA_ADAPTID_THEORA,         (XAchar *)"theoraenc",        0,0,0,0,0,0,0,0,0,0,0,0 },
     //   { VID_D, XA_ADAPTID_RAWVIDEO,       (XAchar *)"videoparse",       0,0,0,0,0,0,0,0,0,0,0,0 },
        { VID_E, XA_ADAPTID_RAWVIDEO,       NULL,                         0,0,0,0,0,0,0,0,0,0,0,0 },
     //   { IMG_D, XA_IMAGECODEC_JPEG,        (XAchar *)"jpegdec",          0,0,0,0,0,0,0,0,0,0,0,0 },
     //   { IMG_E, XA_IMAGECODEC_JPEG,        (XAchar *)"jpegenc",          0,0,0,0,0,0,0,0,0,0,0,0 },
     //   { IMG_D, XA_IMAGECODEC_RAW,         (XAchar *)"videoparse",       0,0,0,0,0,0,0,0,0,0,0,0 },
        { IMG_E, XA_IMAGECODEC_RAW,         NULL,                         0,0,0,0,0,0,0,0,0,0,0,0 },
        { AUD_O, XA_ADAPTID_DEVSOUNDSINK,       (XAchar *)"devsoundsink",     0,0,0,0,0,0,0,0,0,0,0,0 },
        { AUD_I, XA_ADAPTID_DEVSOUNDSRC,        (XAchar *)"devsoundsrc",          0,0,0,0,0,0,0,0,0,0,0,0 },
      //  { AUD_O, XA_ADAPTID_ALSASINK,       (XAchar *)"alsasink",         0,0,0,0,0,0,0,0,0,0,0,0 },
      //  { AUD_I, XA_ADAPTID_ALSASRC,        (XAchar *)"alsasrc",          0,0,0,0,0,0,0,0,0,0,0,0 }
//#ifdef XA_USE_TEST_PLUGINS
//        ,{ AUD_I, XA_ADAPTID_AUDIOTESTSRC,   (XAchar *)"audiotestsrc",     0,0,0,0,0,0,0,0,0,0,0,0 }
//#endif
    };

/*
 * XAresult XAStaticCapsAdapt_ReadImageCaps(GstCaps* caps, XAStaticCapsData* data)
 */
XAresult XAStaticCapsAdapt_ReadImageCaps(GstCaps* caps, XAStaticCapsData* data)
{
    XAresult res = XA_RESULT_SUCCESS;

    XAuint32 widthMax = 0;
    XAuint32 heightMax = 0;
    gint capsCount = 0;
    gint iterator = 0;
    GstStructure *capsStruct = NULL;
    G_CONST_RETURN GValue* value = NULL;

    DEBUG_API("->XAStaticCapsAdapt_ReadImageCaps");
    capsCount = gst_caps_get_size( caps );
    for( iterator = 0; iterator < capsCount; iterator++ )
    {
        capsStruct = gst_caps_get_structure( caps, iterator );

        DEBUG_INFO_A1("Caps from gst: %s", gst_structure_to_string(capsStruct));

        if (gst_structure_n_fields( capsStruct ) > 0) /* Any check*/
        {
            value = gst_structure_get_value( capsStruct, "width" );
            if (GST_VALUE_HOLDS_INT_RANGE(value))
            {
                widthMax = gst_value_get_int_range_max( value );
                DEBUG_INFO_A1("widthMax:%d",(int)widthMax);
            }
            else
            {
                DEBUG_ERR("Cannot get width from capabilities struct");
                DEBUG_API("<-XAStaticCapsAdapt_ReadImageCaps - XA_RESULT_INTERNAL_ERROR");
                return XA_RESULT_INTERNAL_ERROR;
            }

            value = gst_structure_get_value( capsStruct, "height" );
            if (GST_VALUE_HOLDS_INT_RANGE(value))
            {
                heightMax = gst_value_get_int_range_max( value );
                DEBUG_INFO_A1("heightMax:%d",(int)heightMax);
            }
            else
            {
                DEBUG_ERR("Cannot get height from capabilities struct");
                DEBUG_API("<-XAStaticCapsAdapt_ReadImageCaps - XA_RESULT_INTERNAL_ERROR");
                return XA_RESULT_INTERNAL_ERROR;
            }

            if (heightMax < data->maxH)
            {
                data->maxH = heightMax;
            }
            if (widthMax < data->maxW)
            {
                data->maxW = widthMax;
            }
        }
    }
    DEBUG_API("<-XAStaticCapsAdapt_ReadImageCaps");
    return res;
}

/*
 * XAresult XAStaticCapsAdapt_ReadAudioCaps(GstCaps* caps, XAStaticCapsData* data, GstElement* element)
 */
XAresult XAStaticCapsAdapt_ReadAudioCaps(GstCaps* caps, XAStaticCapsData* data, GstElement* element)
{
    XAresult res = XA_RESULT_SUCCESS;

    XAuint32 channelsMax = ANY;
    XAuint32 minRate = 0;
    XAuint32 maxRate = ANY;
    XAuint32 minBPS = 0;
    XAuint32 maxBPS = ANY;
    XAuint32 minBR = 0;
    XAuint32 maxBR = ANY;
    XAboolean intCapsUsed = XA_BOOLEAN_FALSE;

    gint capsCount = 0;
    gint iterator = 0;
    GstStructure *capsStruct = NULL;
    G_CONST_RETURN GValue* value = NULL;
    const char* capsName = NULL;

    DEBUG_API("->XAStaticCapsAdapt_ReadAudioCaps");
    capsCount = gst_caps_get_size( caps );
    for( iterator = 0; iterator < capsCount; iterator++ )
    {
        capsStruct = gst_caps_get_structure( caps, iterator );

        DEBUG_INFO_A1("Caps from gst: %s", gst_structure_to_string(capsStruct));
        capsName = gst_structure_get_name(capsStruct);

        if (intCapsUsed == XA_BOOLEAN_TRUE)
        {
            break; // preferred caps used 
        }
        else if (!strcmp( capsName,"audio/x-raw-int"))
        {
            intCapsUsed = XA_BOOLEAN_TRUE;
        }

        if (gst_structure_n_fields( capsStruct ) > 0) // Any check
        {
            value = gst_structure_get_value( capsStruct, "channels" );
            if (GST_VALUE_HOLDS_INT_RANGE(value))
            {
                channelsMax = gst_value_get_int_range_max( value );
                DEBUG_INFO_A1("channelsMax:%d",(int)channelsMax);
            }
            else if (G_VALUE_HOLDS_INT(value))
            {
                channelsMax = g_value_get_int( value );
                DEBUG_INFO_A1("channelsMax:%d",(int)channelsMax);
            }
            else
            {
                DEBUG_ERR("Cannot get channels from capabilities struct");
                DEBUG_API("<-XAStaticCapsAdapt_ReadAudioCaps - XA_RESULT_INTERNAL_ERROR");
                return XA_RESULT_INTERNAL_ERROR;
            }

            value = gst_structure_get_value( capsStruct, "rate" );
            if (GST_VALUE_HOLDS_INT_RANGE(value))
            {
                minRate = gst_value_get_int_range_min( value );
                maxRate = gst_value_get_int_range_max( value );
                DEBUG_INFO_A2("minRate:%d maxRate:%d",(int)minRate,(int)maxRate);
            }
            else
            {
                DEBUG_ERR("Cannot get rates from capabilities struct");
                DEBUG_API("<-XAStaticCapsAdapt_ReadAudioCaps - XA_RESULT_INTERNAL_ERROR");
                return XA_RESULT_INTERNAL_ERROR;
            }

            value = gst_structure_get_value( capsStruct, "width" );
            if (GST_VALUE_HOLDS_LIST(value))
            {
                XAuint32 count = 0;
                XAuint32 i = 0;
                XAuint32 bps = 0;
                count = gst_value_list_get_size(value);
                for (i = 0; i < count; i++  )
                {
                    G_CONST_RETURN GValue* v = gst_value_list_get_value(value, i);
                    bps =  g_value_get_int(v);
                    if (bps < minBPS || i == 0)
                    {
                        minBPS = bps;
                    }
                    if (bps > maxBPS || i == 0)
                    {
                        maxBPS = bps;
                    }
                    DEBUG_INFO_A2("minBPS:%d maxBPS:%d",(int)minBPS,(int)maxBPS);
                }
            }
            else if (G_VALUE_HOLDS_INT(value))
            {
                minBPS = g_value_get_int(value);
                maxBPS = g_value_get_int(value);
                DEBUG_INFO_A2("minBPS:%d maxBPS:%d",(int)minBPS,(int)maxBPS);
            }
            else
            {
                // if not there read from depth 
                value = gst_structure_get_value( capsStruct, "depth" );
                if (GST_VALUE_HOLDS_INT_RANGE(value))
                {
                    minBPS = gst_value_get_int_range_min( value );
                    maxBPS = gst_value_get_int_range_max( value );
                    DEBUG_INFO_A2("minBPS:%d maxBPS:%d",(int)minBPS,(int)maxBPS);
                }
                else
                {
                    DEBUG_ERR("Cannot get width from capabilities struct");
                    DEBUG_API("<-XAStaticCapsAdapt_ReadAudioCaps - XA_RESULT_INTERNAL_ERROR");
                    return XA_RESULT_INTERNAL_ERROR;
                }
            }

            value = gst_structure_get_value( capsStruct, "bitrate" );
            if( value && GST_VALUE_HOLDS_INT_RANGE(value) )
            {
                minBR = gst_value_get_int_range_min( value );
                maxBR = gst_value_get_int_range_max( value );
                DEBUG_INFO_A2("minBR:%d maxBR:%d",(int)minBR,(int)maxBR);
            }
            else
            {
            	// TL: Replaced this code...
              /*  GParamSpec* param = g_object_class_find_property(G_OBJECT_GET_CLASS(element), "bitrate");
                if(param && G_IS_PARAM_SPEC_INT(param))
                {
                    GParamSpecInt *pint = G_PARAM_SPEC_INT (param);
                    if(pint->minimum>0) minBR = pint->minimum;
                    else minBR = 1;
                    maxBR = pint->maximum;
                }
                */
                
                // ...with this:
                // GstBitRate *bitRate; // This object doesn't exist yet
                // GObject *minBitRate = G_OBJECT(bitRate->minBitRate);
                // GObject *maxBitRate = G_OBJECT(bitRate->maxBitRate);
                //if (g_object_class_find_property(G_OBJECT_GET_CLASS(minBitRate), "bitrate"))
                //{
                //	if (minBitRate > 0)
                //		minBR = minBitRate;
                //	else
                //		minBR = 1;
              	//}
              	//else
              	//	minBR = 0;
                //if (g_object_class_find_property(G_OBJECT_GET_CLASS(maxBitRate), "bitrate"))             		
                //	maxBR = maxBitRate;
                // else
                // maxBR = 0;
            }

            if (channelsMax < data->maxCh)
            {
                data->maxCh = channelsMax;
            }
            if (minRate > data->minSR)
            {
                data->minSR = minRate;
            }
            if (maxRate < data->maxSR)
            {
                data->maxSR = maxRate;
            }
            if (minBPS > data->minBPS)
            {
                data->minBPS = minBPS;
            }
            if (maxBPS < data->maxBPS)
            {
                data->maxBPS = maxBPS;
            }
            if (minBR > data->minBR)
            {
                data->minBR = minBR;
            }
            if (maxBR < data->maxBR)
            {
                data->maxBR = maxBR;
            }
        }
    }
    
    DEBUG_API("<-XAStaticCapsAdapt_ReadAudioCaps");
    return res;
}

/*
 * XAresult XAStaticCapsAdapt_ReadVideoCaps(GstCaps* caps, XAStaticCapsData* data)
 */
XAresult XAStaticCapsAdapt_ReadVideoCaps(GstCaps* caps, XAStaticCapsData* data)
{
    XAresult res = XA_RESULT_SUCCESS;

    XAuint32 widthMax = ANY;
    XAuint32 heightMax = ANY;
    XAuint32 frMax = ANY;
    const GValue* framerate = NULL;

    gint capsCount = 0;
    gint iterator = 0;
    GstStructure *capsStruct = NULL;
    G_CONST_RETURN GValue* value = NULL;

    DEBUG_API("->XAStaticCapsAdapt_ReadVideoCaps");
    capsCount = gst_caps_get_size( caps );
    for( iterator = 0; iterator < capsCount; iterator++ )
    {
        capsStruct = gst_caps_get_structure( caps, iterator );

        DEBUG_INFO_A1("Caps from gst: %s", gst_structure_to_string(capsStruct));

        if (gst_structure_n_fields( capsStruct ) > 0) /* Any check*/
        {
            value = gst_structure_get_value( capsStruct, "width" );
            if (GST_VALUE_HOLDS_INT_RANGE(value))
            {
              widthMax = gst_value_get_int_range_max( value );
              DEBUG_INFO_A1("widthMax:%d",(int)widthMax);
            }
            else
            {
              DEBUG_ERR("Cannot get width from capabilities struct");
              DEBUG_API("<-XAStaticCapsAdapt_ReadVideoCaps - XA_RESULT_INTERNAL_ERROR");
              return XA_RESULT_INTERNAL_ERROR;
            }

            value = gst_structure_get_value( capsStruct, "height" );
            if (GST_VALUE_HOLDS_INT_RANGE(value))
            {
              heightMax = gst_value_get_int_range_max( value );
              DEBUG_INFO_A1("heightMax:%d",(int)heightMax);
            }
            else
            {
              DEBUG_ERR("Cannot get height from capabilities struct");
              DEBUG_API("<-XAStaticCapsAdapt_ReadVideoCaps - XA_RESULT_INTERNAL_ERROR");
              return XA_RESULT_INTERNAL_ERROR;
            }

            value = gst_structure_get_value( capsStruct, "framerate" );
            if (GST_VALUE_HOLDS_FRACTION_RANGE(value))
            {
              framerate = gst_value_get_fraction_range_max( value );
              frMax = gst_value_get_fraction_numerator( framerate );
              DEBUG_INFO_A1("frMax:%d",(int)frMax);
            }
            else
            {
              DEBUG_ERR("Cannot get height from capabilities struct");
              DEBUG_API("<-XAStaticCapsAdapt_ReadVideoCaps - XA_RESULT_INTERNAL_ERROR");
              return XA_RESULT_INTERNAL_ERROR;
            }

            if (heightMax < data->maxH)
            {
              data->maxH = heightMax;
            }
            if (widthMax < data->maxW)
            {
              data->maxW = widthMax;
            }
            if (frMax < data->maxFR)
            {
              data->maxFR = frMax;
            }
        }
    }

    DEBUG_API("<-XAStaticCapsAdapt_ReadVideoCaps");
    return res;
}

/*
 * XAresult XAStaticCapsAdapt_ReadAudioIOCaps(GstCaps* caps, XAStaticCapsData* data)
 */
XAresult XAStaticCapsAdapt_ReadAudioIOCaps(GstCaps* caps, XAStaticCapsData* data)
{
    XAresult res = XA_RESULT_SUCCESS;

    XAuint32 minSR = 0;
    XAuint32 maxSR = ANY;
    gint capsCount = 0;
    gint iterator = 0;
    GstStructure *capsStruct = NULL;

    const char* capsName = NULL;
    XAuint32 depth = 0;

    DEBUG_API("->XAStaticCapsAdapt_ReadAudioIOCaps");
    capsCount = gst_caps_get_size( caps );
    for( iterator = 0; iterator < capsCount; iterator++ )
    {
        capsStruct = gst_caps_get_structure( caps, iterator );

        DEBUG_INFO_A1("Caps from gst: %s", gst_structure_to_string(capsStruct));

        capsName = gst_structure_get_name(capsStruct);
        if (!strcmp(capsName, "audio/x-raw-int"))
        {
            if (gst_structure_get_int( capsStruct, "depth", (gint*)&depth))
            {
                DEBUG_INFO_A1("depth:%d",(int)depth);
            }
            else
            {
                DEBUG_ERR("Cannot get depth from capabilities struct");
                DEBUG_API("<-XAStaticCapsAdapt_ReadAudioIOCaps - XA_RESULT_INTERNAL_ERROR");
                return XA_RESULT_INTERNAL_ERROR;
            }
            if (minSR == 0 && maxSR == 0)
            {
                minSR = depth;
                maxSR = depth;
            }
            else
            {
                if (depth < minSR)
                {
                    minSR = depth;
                }
                if (depth > maxSR)
                {
                    maxSR = depth;
                }
            }
            switch (depth)
            {
            case XA_PCMSAMPLEFORMAT_FIXED_8:
                data->pcmProfilesSupported |= XA_ADAPT_PCMSAMPLEFORMAT_SUPPORTED_8;
                break;
            case XA_PCMSAMPLEFORMAT_FIXED_16:
                data->pcmProfilesSupported |= XA_ADAPT_PCMSAMPLEFORMAT_SUPPORTED_16;
                break;
            case XA_PCMSAMPLEFORMAT_FIXED_20:
                data->pcmProfilesSupported |= XA_ADAPT_PCMSAMPLEFORMAT_SUPPORTED_20;
                break;
            case XA_PCMSAMPLEFORMAT_FIXED_24:
                data->pcmProfilesSupported |= XA_ADAPT_PCMSAMPLEFORMAT_SUPPORTED_24;
                break;
            case XA_PCMSAMPLEFORMAT_FIXED_28:
                data->pcmProfilesSupported |= XA_ADAPT_PCMSAMPLEFORMAT_SUPPORTED_28;
                break;
            case XA_PCMSAMPLEFORMAT_FIXED_32:
                data->pcmProfilesSupported |= XA_ADAPT_PCMSAMPLEFORMAT_SUPPORTED_32;
                break;
            default:
                DEBUG_ERR("Invalid depth");
                DEBUG_API("<-XAStaticCapsAdapt_ReadAudioIOCaps - XA_RESULT_INTERNAL_ERROR");
                return XA_RESULT_INTERNAL_ERROR;
            }
        }
    }
    data->minSR = minSR;
    data->maxSR = maxSR;


    DEBUG_API("<-XAStaticCapsAdapt_ReadAudioIOCaps");
    return res;
}

/*
 * XAresult XAStaticCapsAdapt_GetCapsFromGst(XAStaticCapsData* data)
 */
XAresult XAStaticCapsAdapt_GetCapsFromGst(XAStaticCapsData* data)
{
    XAresult res = XA_RESULT_SUCCESS;
    GstCaps* caps = NULL;
    GstPad* pad = NULL;
    GstPad* pads[2] = {NULL, NULL};
    GstElement* element = NULL;
    XAuint32 padCount = 2; /* default src/sink */
    XAuint32 padLoop = 0;

    DEBUG_API("->XAStaticCapsAdapt_GetCapsFromGst");

    /* by default no constraints */
    data->maxCh = ANY;
    data->minBPS = 0;
    data->maxBPS = ANY;
    data->minSR = 0;
    data->maxSR = ANY;
    data->minBR = 0;
    data->maxBR = ANY;
    data->maxFR = ANY;
    data->maxW = ANY;
    data->maxH = ANY;
    data->extra = 0;
    data->pcmProfilesSupported = 0;
    data->numBitrates = 0;

    if (data->xaid == XA_AUDIOCODEC_PCM)
    {
        data->extra = XA_AUDIOPROFILE_PCM;
    }

    if (data->adaptId != NULL)
    {
        element =  gst_element_factory_make( (char *)data->adaptId, (char *)data->adaptId);
        if (!element)
        {
            DEBUG_ERR("Cannot make gst element!");
            DEBUG_API("<-XAStaticCapsAdapt_GetelementCaps - XA_RESULT_INTERNAL_ERROR");
            return XA_RESULT_INTERNAL_ERROR;
        }

        if (data->captype == AUD_I || data->captype == AUD_O)
        {
            padCount = 1;
            if (data->captype == AUD_I)
            {
                pads[0] = gst_element_get_static_pad( element, "src");
            }
            else
            {
                pads[0] = gst_element_get_static_pad( element, "sink");
            }
        }
        else
        {
            padCount = 2;
            pads[0] = gst_element_get_static_pad( element, "sink");
            pads[1] = gst_element_get_static_pad( element, "src");
        }

        if (!pads[0] || (!pads[1] && padCount == 2))
        {
            DEBUG_ERR("Cannot get pad!");
            DEBUG_API("<-XAStaticCapsAdapt_GetCapsFromGst - XA_RESULT_INTERNAL_ERROR");
            return XA_RESULT_INTERNAL_ERROR;
        }

        for (padLoop = 0; padLoop < padCount; padLoop++)
        {
            pad = pads[padLoop];

            caps = gst_pad_get_caps( pad );
            if( !caps )
            {
                DEBUG_ERR("Cannot receive caps from pad!");
                DEBUG_API("<-XAStaticCapsAdapt_GetCapsFromGst - XA_RESULT_INTERNAL_ERROR");
                return XA_RESULT_INTERNAL_ERROR;
            }

            if (data->captype & XACAP_IMAGE)
            {
                res = XAStaticCapsAdapt_ReadImageCaps(caps, data);
            }
            else if(data->captype & XACAP_AUDIO)
            {
                res = XAStaticCapsAdapt_ReadAudioCaps(caps, data, element);
            }
            else if(data->captype & XACAP_VIDEO)
            {
                res = XAStaticCapsAdapt_ReadVideoCaps(caps, data);
            }
            if (data->captype == AUD_I || data->captype == AUD_O)
            {
                res = XAStaticCapsAdapt_ReadAudioIOCaps(caps, data);
            }
        }
    }

    DEBUG_API("<-XAStaticCapsAdapt_GetCapsFromGst");
    return res;
}

/*
 * XAresult XAStaticCapsAdapt_InitCaps()
 */
XAresult XAStaticCapsAdapt_InitCaps()
{
    XAresult res = XA_RESULT_SUCCESS;
    XAuint32 i = 0;
    XAuint32 capCount = (sizeof(allCaps)/sizeof(XAStaticCapsData));
    DEBUG_API("->XAStaticCapsAdapt_InitCaps");

    for( i = 0; i < capCount; i++)
    {
        res = XAStaticCapsAdapt_GetCapsFromGst(&allCaps[i]);
        if (res != XA_RESULT_SUCCESS)
        {
            DEBUG_API("<-XAStaticCapsAdapt_InitCaps");
            return res;
        }
    }

    DEBUG_API("<-XAStaticCapsAdapt_InitCaps");
    return res;
}

/* XAresult XAStaticCapsAdapt_GetCapsCount
 * Description: Count capabilities of certain type. Filter is specified by
 *              bitmasking XACapsType values.
 */
XAresult XAStaticCapsAdapt_GetCapsCount(XACapsType filter, XAuint32 *count)
{
    XAresult res = XA_RESULT_SUCCESS;
    XAuint32 capCount = (sizeof(allCaps)/sizeof(XAStaticCapsData));
    DEBUG_API("->XAStaticCapsAdapt_GetCapsCount");
    if(!count)
    {
        res = XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
        XAuint32 i = 0;
        (*count)=0;
        for( i=0; i<capCount; i++ )
        {
            if ( (allCaps[i].captype & filter) == filter )
            {
                (*count)++;
            }
        }
    }
    DEBUG_API("<-XAStaticCapsAdapt_GetCapsCount");
    return res;
}

/* XAresult XAStaticCapsAdapt_GetCapsById
 * Description: Get capabilities of type XACapsType and matching id
 */
XAresult XAStaticCapsAdapt_GetCapsById(XACapsType filter, XAuint32 maxId, XAStaticCapsData* data)
{
    XAresult res = XA_RESULT_SUCCESS;
    XAuint32 capCount = (sizeof(allCaps)/sizeof(XAStaticCapsData));
    DEBUG_API("->XAStaticCapsAdapt_GetCapsById");
    if(!data)
    {
        res = XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
        XAuint32 i = 0;
        XAboolean found = XA_BOOLEAN_FALSE;
        for( i=0; i<capCount; i++ )
        {
            if ( ((allCaps[i].captype & filter) == filter) && (maxId==allCaps[i].xaid) )
            {
                memcpy(data, &allCaps[i], sizeof(XAStaticCapsData));
                found = XA_BOOLEAN_TRUE;
                break;
            }
        }
        if(!found)
        {
           res = XA_RESULT_FEATURE_UNSUPPORTED;
        }
    }
    DEBUG_API("<-XAStaticCapsAdapt_GetCapsById");
    return res;
}

/* XAresult XAStaticCapsAdapt_GetCapsByIdx
 * Description: Get n'th capabilities of type XACapsType
 */
XAresult XAStaticCapsAdapt_GetCapsByIdx(XACapsType filter, XAuint32 idx, XAStaticCapsData* data)
{
    XAresult res = XA_RESULT_SUCCESS;
    XAuint32 capCount = (sizeof(allCaps)/sizeof(XAStaticCapsData));
    DEBUG_API("->XAStaticCapsAdapt_GetCapsByIdx");
    if(!data)
    {
        res = XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
        XAuint32 i = 0,j=0;
        XAboolean found = XA_BOOLEAN_FALSE;
        for( i=0; i<capCount; i++ )
        {
            if ( (allCaps[i].captype & filter) == filter )
            {
                if( idx == j++ )
                {
                    memcpy(data, &allCaps[i], sizeof(XAStaticCapsData));
                    found = XA_BOOLEAN_TRUE;
                    break;
                }
            }
        }
        if(!found)
        {
           res = XA_RESULT_PARAMETER_INVALID;
        }
    }
    DEBUG_API("<-XAStaticCapsAdapt_GetCapsByIdx");
    return res;
}

/* XAresult XAStaticCapsAdapt_QueryColorFormats
 * Description: Get color formats associated with the XA_IMAGECODEC_RAW codec.
 */
XAresult XAStaticCapsAdapt_QueryColorFormats(XAuint32* pIndex, XAuint32* pColorFormats)
{
    XAresult res = XA_RESULT_SUCCESS;
    DEBUG_API("->XAStaticCapsAdapt_QueryColorFormats");

    if( !pIndex )
    {
        DEBUG_ERR("illegal NULL parameter");
        res = XA_RESULT_PARAMETER_INVALID;
    }
    else
    {
        XAStaticCapsData temp;
        res = XAStaticCapsAdapt_GetCapsById(XACAP_ENCODER|XACAP_IMAGE, XA_IMAGECODEC_RAW, &temp);
        if( res == XA_RESULT_SUCCESS )
        {
            if( !pColorFormats )
            {   /* query number of color formats */
                *pIndex = 1; /* one used by camera context */
            }
            else
            {   /* query color format */
                if( *pIndex >= 1 ) /* one used by camera context */
                {
                    DEBUG_ERR("index parameter invalid");
                    res = XA_RESULT_PARAMETER_INVALID;
                }
                else
                {   /* internal format used by pipeline, look camera context for details  */
                    *pColorFormats = XA_COLORFORMAT_YUV420PLANAR;
                }
            }
        }
        else
        {
            *pIndex = 0;
        }
    }

    DEBUG_API("<-XAStaticCapsAdapt_QueryColorFormats");
    return res;
}

