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

#include <string.h>
#include <gst/gst.h>
#include "xaadaptationgst.h"
#include "xametadataadaptctx.h"
#include "xamediaplayeradaptctx.h"
#include "xamediarecorderadaptctx.h"
#include "xametadataadaptation.h"

/* forward declarations of internal methods */
XAMetadataAdaptVars* XAMetadataAdapt_GetMetadataVars(XAAdaptationGstCtx *bCtx);
GstStructure* XAMetadataAdapt_GetChildStructure(XAMetadataAdaptVars* mdv, XAuint32 index);
gboolean XAMetadataAdapt_CountTags(GQuark gkey, const GValue *gvalue, gpointer counter);
void XAMetadataAdapt_ParseTag(const GstTagList* list, const gchar* tag, gpointer listptr);
gboolean XAMetadataAdapt_ParseItem(GQuark field_id, const GValue * value, gpointer listptr);
gboolean XAMetadataAdapt_GstTagCb( GstBus *bus, GstMessage *message, gpointer data );

/*NOTE: no way to query these dynamically?*/
/*supported keys for metadata insertion*/
static char* xaMetadataAdaptSupportedKeys[] = {
    KHRONOS_TITLE,
    KHRONOS_ALBUM,
    KHRONOS_TRACK_NUMBER,
    KHRONOS_ARTIST,
    KHRONOS_GENRE,
    KHRONOS_COMMENT,
    KHRONOS_COPYRIGHT,
    GST_TAG_TITLE,
    GST_TAG_TITLE_SORTNAME,
    GST_TAG_ARTIST,
    GST_TAG_ARTIST_SORTNAME,
    GST_TAG_ALBUM,
    GST_TAG_ALBUM_SORTNAME,
    GST_TAG_COMPOSER,
    GST_TAG_DATE,
    GST_TAG_GENRE,
    GST_TAG_COMMENT,
    GST_TAG_EXTENDED_COMMENT,
    GST_TAG_TRACK_NUMBER,
    GST_TAG_TRACK_COUNT,
    GST_TAG_ALBUM_VOLUME_NUMBER,
    GST_TAG_ALBUM_VOLUME_COUNT,
    GST_TAG_LOCATION,
    GST_TAG_DESCRIPTION,
    GST_TAG_VERSION,
    GST_TAG_ISRC,
    GST_TAG_ORGANIZATION,
    GST_TAG_COPYRIGHT,
    GST_TAG_COPYRIGHT_URI,
    GST_TAG_CONTACT,
    GST_TAG_LICENSE,
    GST_TAG_LICENSE_URI,
    GST_TAG_PERFORMER,
    NULL
};

/******************************************************************************
 * COMMONS
 ******************************************************************************/


/* const char* XAMetadataAdapt_ParseKhronosKey(const char* pKKey)
 * Translates Khronos key string to adaptation-specific key string
 * @return Translated key string
 */
const XAchar* XAMetadataAdapt_ParseKhronosKey(const XAchar* pKKey)
{
    const XAchar* newKey;
    DEBUG_API_A1("->XAMetadataAdapt_ParseKhronosKey: \"%s\"", (char*)pKKey);
    if( strcmp((char*)pKKey,KHRONOS_TITLE) ==0 ) newKey = (XAchar*)GST_TAG_TITLE;
    else if( strcmp((char*)pKKey,KHRONOS_ALBUM) ==0 ) newKey = (XAchar*)GST_TAG_ALBUM;
    else if( strcmp((char*)pKKey,KHRONOS_TRACK_NUMBER) ==0 ) newKey = (XAchar*)GST_TAG_TRACK_NUMBER;
    else if( strcmp((char*)pKKey,KHRONOS_ARTIST) ==0 ) newKey = (XAchar*)GST_TAG_ARTIST;
    else if( strcmp((char*)pKKey,KHRONOS_GENRE) ==0 ) newKey = (XAchar*)GST_TAG_GENRE;
    else if( strcmp((char*)pKKey,KHRONOS_COMMENT) ==0 ) newKey = (XAchar*)GST_TAG_COMMENT;
    else if( strcmp((char*)pKKey,KHRONOS_COPYRIGHT) ==0 ) newKey = (XAchar*)GST_TAG_COPYRIGHT;
    else newKey = pKKey;
/* No Gst keys for following:
    else if( strcmp(pKKey,KHRONOS_YEAR) ==0 ) newKey = ;
    else if( strcmp(pKKey,KHRONOS_ARTIST_URL) ==0 ) newKey = ;
    else if( strcmp(pKKey,KHRONOS_CONTENT_URL) ==0 ) newKey = ;
    else if( strcmp(pKKey,KHRONOS_RATING) ==0 ) newKey = ;
    else if( strcmp(pKKey,KHRONOS_ALBUM_ART) ==0 ) newKey = ;
 */
    DEBUG_API_A1("<-XAMetadataAdapt_ParseKhronosKey: => \"%s\"", (char*)newKey);
    return newKey;
}

/* XAresult XAMetadataAdapt_PreInit(XAAdaptationGstCtx *bCtx)
 * Initialize values needed before preroll
 * @return XAresult ret - Success value
 */
XAresult XAMetadataAdapt_PreInit(XAAdaptationGstCtx *bCtx)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMetadataAdaptVars* mdv;
    DEBUG_API("->XAMetadataAdapt_PreInit");
    if( bCtx->baseObj.ctxId == XAMDAdaptation ||
        bCtx->baseObj.ctxId == XAMediaPlayerAdaptation )
    {
        mdv = (XAMetadataAdaptVars*) calloc(1, sizeof(XAMetadataAdaptVars));
        mdv->currentchild = &(mdv->generaltags);
        mdv->traversemode=XA_METADATATRAVERSALMODE_NODE;
        ((XAMediaPlayerAdaptationCtx*)bCtx)->metadatavars = mdv;

    }
    else if ( bCtx->baseObj.ctxId == XAMediaRecorderAdaptation )
    {
        mdv = (XAMetadataAdaptVars*) calloc(1, sizeof(XAMetadataAdaptVars));
        mdv->currentchild = &(mdv->generaltags);
        mdv->traversemode=XA_METADATATRAVERSALMODE_NODE;
        ((XAMediaRecorderAdaptationCtx*)bCtx)->metadatavars = mdv;
    }
    else
    {
        DEBUG_ERR("Unsupported metadata context!!");
        ret = XA_RESULT_FEATURE_UNSUPPORTED;
    }

    if(ret == XA_RESULT_SUCCESS)
    {
        if(bCtx->bus)
        {
            g_signal_connect(bCtx->bus, "message::tag", G_CALLBACK(XAMetadataAdapt_GstTagCb), bCtx );
        }
        else
        {
            DEBUG_ERR("No bus in context!!");
            ret = XA_RESULT_PRECONDITIONS_VIOLATED;
        }
    }
    DEBUG_API_A1("<-XAMetadataAdapt_PreInit (%d)", (int)ret);
    return ret;
}
/* XAresult XAMetadataAdapt_PostInit(XAAdaptationGstCtx *bCtx)
 * Initialize values after preroll
 * @return XAresult ret - Success value
 */
XAresult XAMetadataAdapt_PostInit(XAAdaptationGstCtx *bCtx)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMetadataAdaptVars* mdv=NULL;
    XAMediaPlayerAdaptationCtx* mCtx=NULL;
    GstPad* tmppad=NULL;
    GstCaps* tmpcaps=NULL;
    GstElement* tmpelement=NULL;
    XAAdaptEvent event = {XA_METADATAEVENTS, XA_ADAPT_MDE_TAGS_AVAILABLE, 0, NULL };

    DEBUG_API("->XAMetadataAdapt_PostInit");
    if( bCtx->baseObj.ctxId == XAMDAdaptation ||
        bCtx->baseObj.ctxId == XAMediaPlayerAdaptation )
    {
        mdv = XAMetadataAdapt_GetMetadataVars(bCtx);
        if( mdv )
        {
            mdv->childcount = 0;
            /* try to dig out audio and video pads from decodebin for stream info tags */
            /* NOTE: currently no good deterministic way to dig out undecoded pads
             * from decodebin's internal demuxer, this is just ugly way to try to cope
             * with most demuxers.
             */
            mCtx = ((XAMediaPlayerAdaptationCtx*)bCtx);
            if(GST_IS_BIN(mCtx->codecbin))
            {
                tmpelement=gst_bin_get_by_name(GST_BIN(mCtx->codecbin),"typefind");
                if(tmpelement)
                {
                    tmppad = gst_element_get_static_pad(GST_ELEMENT(tmpelement),"src");
                    tmpelement=NULL;
                    if(tmppad)
                    {
                        tmppad = gst_pad_get_peer(tmppad);
                        if(tmppad)
                        {
                            tmpelement = gst_pad_get_parent_element(tmppad);
                        }
                    }
                    /* now we have demuxer, if existing */
                    if(tmpelement)
                    {
                        tmppad = gst_element_get_pad( tmpelement, "audio_00");
                        if(tmppad)
                        {
                            tmpcaps = gst_pad_get_negotiated_caps( GST_PAD(tmppad) );
                            if(tmpcaps==NULL || gst_caps_is_any(tmpcaps) || gst_caps_is_empty(tmpcaps))
                            {
                                mdv->audiotags = NULL;
                                DEBUG_INFO("no usable audio properties found from pad !");
                            }
                            else
                            {
                                mdv->audiotags = gst_caps_get_structure(tmpcaps,0);
                                mdv->childcount++;
                                DEBUG_INFO_A1("found audio node: %s",gst_caps_to_string(tmpcaps));
                            }
                        }
                        tmppad = gst_element_get_pad( tmpelement, "video_00");
                        if(tmppad)
                        {
                            tmpcaps = gst_pad_get_negotiated_caps( GST_PAD(tmppad) );
                            if(tmpcaps==NULL || gst_caps_is_any(tmpcaps) || gst_caps_is_empty(tmpcaps))
                            {
                                mdv->videotags = NULL;
                                DEBUG_INFO("no usable video properties found from pad !");
                            }
                            else
                            {
                                mdv->videotags = gst_caps_get_structure(tmpcaps,0);
                                mdv->childcount++;
                                DEBUG_INFO_A1("found video node: %s",gst_caps_to_string(tmpcaps));
                            }
                        }
                    }
                }
            }
            XAAdaptationBase_SendAdaptEvents(&bCtx->baseObj, &event );
        }
    }

    if ( tmpelement )
    {
    	gst_object_unref( tmpelement );
    }
    if ( tmppad )
    {
    	gst_object_unref( tmppad );
    }
    if ( tmpcaps )
    {
    	gst_object_unref( tmpcaps );
    }

    DEBUG_API_A1("<-XAMetadataAdapt_PostInit (%d)", (int)ret);
    return ret;
}

/*
 * void XAMetadataAdapt_FreeImplTagList(XAMetadataImplTagList *list, XAboolean freeItems)
 * deallocate elements inside taglist
 */
void XAMetadataAdapt_FreeImplTagList(XAMetadataImplTagList *list, XAboolean freeItems)
{
    XAuint16 i;
    DEBUG_API("->XAMetadataExtractionItfAdapt_FreeTagList");
    if(list->mdeKeys)
    {
        for(i=0;i<list->itemcount;i++)
        {
            if(list->mdeKeys[i])
            {
                if(freeItems)
                    free(list->mdeKeys[i]);
                list->mdeKeys[i]=NULL;
            }
        }
        free(list->mdeKeys);
        list->mdeKeys = NULL;
    }
    if(list->mdeValues)
    {
        for(i=0;i<list->itemcount;i++)
        {
            if(list->mdeValues[i])
            {
                if(freeItems)
                    free(list->mdeValues[i]);
                list->mdeValues[i]=NULL;
            }
        }
        free(list->mdeValues);
        list->mdeValues = NULL;
    }
    list->itemcount = 0;
    DEBUG_API("<-XAMetadataExtractionItfAdapt_FreeTagList");
}

/*
 * void XAMetadataAdapt_FreeVars(XAMetadataAdaptVars *vars)
 * deallocate XAMetadataAdaptVars
 */
void XAMetadataAdapt_FreeVars(XAMetadataAdaptVars *vars)
{
    DEBUG_API("->XAMetadataAdapt_FreeVars");
    if(vars)
    {
        if( vars->generaltags )
        {
            gst_tag_list_free(vars->generaltags);
        }
        if( vars->audiotags )
        {
            gst_structure_set_parent_refcount(vars->audiotags,NULL);
            gst_structure_free(vars->audiotags);
        }
        if( vars->videotags )
        {
            gst_structure_set_parent_refcount(vars->videotags,NULL);
            gst_structure_free(vars->videotags);
        }
        free(vars);
    }
    DEBUG_API("<-XAMetadataAdapt_FreeVars");
}

/******************************************************************************
 * EXTRACTION
 ******************************************************************************/

/*
 * XAresult XAMetadataExtractionItfAdapt_FillTagList()
 * @param tagList - contains pointer to tag list to be updated
 * @return XAresult ret - Success value
 */
XAresult XAMetadataExtractionItfAdapt_FillTagList(XAAdaptationGstCtx *bCtx,
                                                  XAMetadataImplTagList* tagList)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMetadataAdaptVars* mdv;
    XAuint32 newcount = 0;
    DEBUG_API("->XAMetadataExtractionItfAdapt_FillTagList");

    mdv = XAMetadataAdapt_GetMetadataVars(bCtx);
    if( mdv )
    {
        XAMetadataAdapt_FreeImplTagList(tagList, XA_BOOLEAN_TRUE);
        /* get number of tags and allocate memory for them */
        if(mdv->traversemode==XA_METADATATRAVERSALMODE_ALL)
        {
            if(mdv->generaltags)
                gst_structure_foreach(mdv->generaltags, XAMetadataAdapt_CountTags, &newcount);
            if(mdv->audiotags)
                gst_structure_foreach(mdv->audiotags, XAMetadataAdapt_CountTags, &newcount);
            if(mdv->videotags)
                gst_structure_foreach(mdv->videotags, XAMetadataAdapt_CountTags, &newcount);
        }
        else
        {
            if(mdv->currentchild && *(mdv->currentchild))
                gst_structure_foreach(*(mdv->currentchild), XAMetadataAdapt_CountTags, &newcount);
        }
        DEBUG_INFO_A1("tag count = %d",(int)newcount)
        tagList->mdeKeys = (XAMetadataInfo**)calloc(newcount,sizeof(XAMetadataInfo*));
        tagList->mdeValues = (XAMetadataInfo**)calloc(newcount,sizeof(XAMetadataInfo*));

        /* fill in given tag list */
        tagList->itemcount = 0;
        if(mdv->traversemode==XA_METADATATRAVERSALMODE_ALL)
        {
            if(mdv->generaltags)
                gst_tag_list_foreach(mdv->generaltags, XAMetadataAdapt_ParseTag, tagList);
            if(mdv->audiotags)
                gst_structure_foreach(mdv->audiotags, XAMetadataAdapt_ParseItem, tagList);
            if(mdv->videotags)
                gst_structure_foreach(mdv->videotags, XAMetadataAdapt_ParseItem, tagList);
        }
        else
        {
            if(mdv->currentchild && *(mdv->currentchild))
            {
                if(GST_IS_TAG_LIST(*(mdv->currentchild)))
                {
                    gst_tag_list_foreach(*(mdv->currentchild), XAMetadataAdapt_ParseTag, tagList);
                }
                else
                {
                    gst_structure_foreach(*(mdv->currentchild), XAMetadataAdapt_ParseItem, tagList);
                }
            }
        }
    }
    else
    {
        DEBUG_ERR("Metadata variables not found!!");
        ret = XA_RESULT_PARAMETER_INVALID;
    }

    DEBUG_API_A1("<-XAMetadataExtractionItfAdapt_FillTagList (%d)", (int)ret);
    return ret;
}

/******************************************************************************
 * TRAVERSAL
 *****************************************************************************/

/*
 * XAresult XAMetadataTraversalItfAdapt_SetMode(XAAdaptationGstCtx *bCtx, XAuint32 mode)
 */
XAresult XAMetadataTraversalItfAdapt_SetMode(XAAdaptationGstCtx *bCtx, XAuint32 mode)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMetadataAdaptVars* mdv=NULL;
    XAAdaptEvent event = {XA_METADATAEVENTS, XA_ADAPT_MDE_TAGS_AVAILABLE, 0, NULL };

    DEBUG_API("->XAMetadataTraversalItfAdapt_SetMode");
    mdv = XAMetadataAdapt_GetMetadataVars(bCtx);
    if( mdv )
    {
        mdv->traversemode = mode;
        /* inform extractor to update tag list */
        XAAdaptationBase_SendAdaptEvents(&bCtx->baseObj, &event );
        if(mdv->traversemode==XA_METADATATRAVERSALMODE_ALL)
        {
            mdv->nodedepth = 0;
            mdv->currentchild = &(mdv->generaltags);
        }
    }
    else
    {
        DEBUG_ERR("Metadata variables not found!!");
        ret = XA_RESULT_PARAMETER_INVALID;
    }

    DEBUG_API("<-XAMetadataTraversalItfAdapt_SetMode");
    return ret;
}

/*
 * XAresult XAMetadataTraversalItfAdapt_GetChildCount(XAAdaptationGstCtx *bCtx, XAuint32 *pCount)
 */
XAresult XAMetadataTraversalItfAdapt_GetChildCount(XAAdaptationGstCtx *bCtx, XAuint32 *pCount)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMetadataAdaptVars* mdv=NULL;

    DEBUG_API("->XAMetadataTraversalItfAdapt_GetChildCount");
    mdv = XAMetadataAdapt_GetMetadataVars(bCtx);
    if( mdv )
    {
        /* only depth of 1 is supported */
        if( mdv->nodedepth == 0 )
        {
            *pCount=mdv->childcount;
        }
        else
        {
            *pCount=0;
        }
    }
    else
    {
        DEBUG_ERR("Metadata variables not found!!");
        ret = XA_RESULT_PARAMETER_INVALID;
        *pCount=0;
    }
    DEBUG_API_A2("<-XAMetadataTraversalItfAdapt_GetChildCount(%d) count=%d", (int)ret, (int)*pCount);
    return ret;
}

/*
 * XAresult XAMetadataTraversalItfAdapt_GetChildMIMETypeSize(XAAdaptationGstCtx *bCtx,
 *                                                         XAuint32 index,
 *                                                         XAuint32 *pSize)
 */
XAresult XAMetadataTraversalItfAdapt_GetChildMIMETypeSize(XAAdaptationGstCtx *bCtx,
                                                          XAuint32 index,
                                                          XAuint32 *pSize)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMetadataAdaptVars* mdv=NULL;

    DEBUG_API("->XAMetadataTraversalItfAdapt_GetChildMIMETypeSize");
    mdv = XAMetadataAdapt_GetMetadataVars(bCtx);
    if( mdv )
    {
        GstStructure* child = XAMetadataAdapt_GetChildStructure(mdv,index);
        if(child)
        {
            *pSize = strlen(gst_structure_get_name(child));
        }
    }
    else
    {
        DEBUG_ERR("Metadata variables not found!!");
        ret = XA_RESULT_PARAMETER_INVALID;
        *pSize=0;
    }
    DEBUG_API("<-XAMetadataTraversalItfAdapt_GetChildMIMETypeSize");
    return ret;
}

/*
 * XAresult XAMetadataTraversalItfAdapt_GetChildInfo(XAAdaptationGstCtx *bCtx,
 *                                                  XAuint32 index,
 *                                                 XAint32 *pNodeID,
 *                                                 XAuint32 *pType,
 *                                                 XAuint32 size,
 *                                                 XAchar *pMimeType)
 */
XAresult XAMetadataTraversalItfAdapt_GetChildInfo(XAAdaptationGstCtx *bCtx,
                                                  XAuint32 index,
                                                  XAint32 *pNodeID,
                                                  XAuint32 *pType,
                                                  XAuint32 size,
                                                  XAchar *pMimeType)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMetadataAdaptVars* mdv=NULL;
    const gchar* mime;
    GstStructure* child;

    DEBUG_API("->XAMetadataTraversalItfAdapt_GetChildInfo");
    mdv = XAMetadataAdapt_GetMetadataVars(bCtx);
    if( mdv )
    {
        child = XAMetadataAdapt_GetChildStructure(mdv,index);
        if(child)
        {
            *pNodeID = (XAint32)child;
            mime = gst_structure_get_name(child);
            if(strncmp(mime,"audio",5)==0)
            {
                *pType = XA_NODETYPE_AUDIO;
            }
            if(strncmp(mime,"video",5)==0)
            {
                *pType = XA_NODETYPE_VIDEO;
            }
            if(strncmp(mime,"image",5)==0)
            {
                *pType = XA_NODETYPE_IMAGE;
            }
            else
            {
                *pType = XA_NODETYPE_UNSPECIFIED;
            }
            strncpy((char*)pMimeType,mime,size);
        }
    }
    else
    {
        DEBUG_ERR("Metadata variables not found!!");
        ret = XA_RESULT_PARAMETER_INVALID;
    }
    DEBUG_API("<-XAMetadataTraversalItfAdapt_GetChildInfo");
    return ret;
}

/*
 * XAresult XAMetadataTraversalItfAdapt_SetActiveNode(XAAdaptationGstCtx *bCtx,
 *                                                  XAuint32 index)
 */
XAresult XAMetadataTraversalItfAdapt_SetActiveNode(XAAdaptationGstCtx *bCtx,
                                                   XAuint32 index)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMetadataAdaptVars* mdv=NULL;
    GstStructure* child;

    DEBUG_API("->XAMetadataTraversalItfAdapt_SetActiveNode");
    mdv = XAMetadataAdapt_GetMetadataVars(bCtx);
    if( mdv )
    {
        child = XAMetadataAdapt_GetChildStructure(mdv,index);
        if(child)
        {
            mdv->currentchild = &child;
            if(index==XA_NODE_PARENT) mdv->nodedepth--;
            else mdv->nodedepth++;
        }
    }
    else
    {
        DEBUG_ERR("Metadata variables not found!!");
        ret = XA_RESULT_PARAMETER_INVALID;
    }
    DEBUG_API("<-XAMetadataTraversalItfAdapt_SetActiveNode");
    return ret;
}

/*****************************************************************************
 * INSERTION
 *****************************************************************************/

/*
 * XAresult XAMetadataInsertionItfAdapt_CreateChildNode(XAAdaptationGstCtx *bCtx,
 *                                                   XAint32 parentNodeID,
 *                                                   XAuint32 type,
 *                                                   XAchar *mimeType,
 *                                                   XAint32 *pChildNodeID)
 */
XAresult XAMetadataInsertionItfAdapt_CreateChildNode(XAAdaptationGstCtx *bCtx,
                                                    XAint32 parentNodeID,
                                                    XAuint32 type,
                                                    XAchar *mimeType,
                                                    XAint32 *pChildNodeID)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMetadataAdaptVars* mdv=NULL;
    XAMediaRecorderAdaptationCtx* mCtx;

    DEBUG_API("->XAMetadataInsertionItfAdapt_CreateChildNode");
    if(bCtx && bCtx->baseObj.ctxId == XAMediaRecorderAdaptation)
    {
        /* no support for more than 1 levels of childs*/
        if(parentNodeID==XA_ROOT_NODE_ID)
        {
            XAMediaType mediaType;
            mCtx = (XAMediaRecorderAdaptationCtx*)bCtx;

            /* first check sink type */
            XACommon_CheckDataSource((XADataSource*)(mCtx->xaSink),&mediaType);
            if( ((mediaType==XA_MEDIATYPE_AUDIO||mediaType==XA_MEDIATYPE_VIDEO) && type==XA_NODETYPE_IMAGE) ||
                (mediaType==XA_MEDIATYPE_IMAGE && (type==XA_NODETYPE_AUDIO || type==XA_NODETYPE_VIDEO)) )
            {
                DEBUG_ERR("Nodetype not supported!");
                ret = XA_RESULT_CONTENT_UNSUPPORTED;
                DEBUG_API_A1("<-XAMetadataInsertionItfAdapt_CreateChildNode (%d)", (int)ret);
                return ret;            }

            mdv = mCtx->metadatavars;
            if(!mdv)
            {
                DEBUG_ERR("Metadata variables not initialized!");
                ret = XA_RESULT_PRECONDITIONS_VIOLATED;
            }
            switch(type)
            {
            case XA_NODETYPE_IMAGE:
                /*fall-through to use video pipe tags for image*/
            case XA_NODETYPE_VIDEO:
                if(!mCtx->videosource)
                {
                    DEBUG_ERR("Nodetype not found!");
                    ret = XA_RESULT_CONTENT_UNSUPPORTED;
                }
                else
                {
                    if(!mdv->videotags)
                    {
                        if(strlen((char*)mimeType)==0)
                        {
                            mdv->videotags = gst_structure_empty_new("video/unknown");
                        }
                        else
                        {
                            mdv->videotags = gst_structure_empty_new((gchar*)mimeType);
                        }
                    }
                    *pChildNodeID = (XAint32)mdv->videotags;
                }
                break;
            case XA_NODETYPE_AUDIO:
                if(!mCtx->audiosource)
                {
                    DEBUG_ERR("Nodetype not found!");
                    ret = XA_RESULT_CONTENT_UNSUPPORTED;
                }
                else
                {
                    if(!mdv->audiotags)
                    {
                        if(strlen((char*)mimeType)==0)
                        {
                            mdv->audiotags = gst_structure_empty_new("audio/unknown");
                        }
                        else
                        {
                            mdv->audiotags = gst_structure_empty_new((gchar*)mimeType);
                        }
                    }
                    *pChildNodeID = (XAint32)mdv->audiotags;
                }
                break;
            default:
                DEBUG_ERR("Nodetype not found!");
                ret = XA_RESULT_CONTENT_UNSUPPORTED;
                break;
            }
        }
        else
        {
            DEBUG_ERR("Only root childs supported!!");
            ret = XA_RESULT_CONTENT_UNSUPPORTED;
        }
    }
    else
    {
        DEBUG_ERR("Unsupported metadata insertion context!!");
        ret = XA_RESULT_PARAMETER_INVALID;
    }
    DEBUG_API_A1("<-XAMetadataInsertionItfAdapt_CreateChildNode (%d)", (int)ret);
    return ret;
}

/*
 * XAresult XAMetadataInsertionItfAdapt_GetSupportedKeysCount(XAAdaptationGstCtx *bCtx,
 *                                                         XAint32 nodeID,
 *                                                         XAboolean *pFreeKeys,
 *                                                         XAuint32 *pKeyCount,
 *                                                         XAuint32 *pEncodingCount)
 */
XAresult XAMetadataInsertionItfAdapt_GetSupportedKeysCount(XAAdaptationGstCtx *bCtx,
                                                          XAint32 nodeID,
                                                          XAboolean *pFreeKeys,
                                                          XAuint32 *pKeyCount,
                                                          XAuint32 *pEncodingCount)
{
    XAresult ret = XA_RESULT_SUCCESS;

    DEBUG_API("->XAMetadataInsertionItfAdapt_GetSupportedKeysCount");
    if(bCtx && bCtx->baseObj.ctxId == XAMediaRecorderAdaptation)
    {
        *pKeyCount = 0;
        *pFreeKeys = XA_BOOLEAN_FALSE;
        while(xaMetadataAdaptSupportedKeys[*pKeyCount])
        {
            (*pKeyCount)++;
        }
        *pEncodingCount = 1;
    }
    else
    {
        DEBUG_ERR("Unsupported metadata insertion context!!");
        ret = XA_RESULT_PARAMETER_INVALID;
    }
    DEBUG_API("<-XAMetadataInsertionItfAdapt_GetSupportedKeysCount");
    return ret;
}

/*
 * XAresult XAMetadataInsertionItfAdapt_GetKeySize(XAAdaptationGstCtx *bCtx,
 *                                              XAint32 nodeID,
 *                                              XAuint32 keyIndex,
 *                                              XAuint32 *pKeySize)
 */
XAresult XAMetadataInsertionItfAdapt_GetKeySize(XAAdaptationGstCtx *bCtx,
                                               XAint32 nodeID,
                                               XAuint32 keyIndex,
                                               XAuint32 *pKeySize)
{
    XAresult ret = XA_RESULT_SUCCESS;

    DEBUG_API("->XAMetadataInsertionItfAdapt_GetKeySize");
    if(bCtx && bCtx->baseObj.ctxId == XAMediaRecorderAdaptation)
    {
        if(xaMetadataAdaptSupportedKeys[keyIndex])
        {
            *pKeySize = sizeof(XAMetadataInfo)+strlen(xaMetadataAdaptSupportedKeys[keyIndex]);
        }
        else
        {
            ret = XA_RESULT_PARAMETER_INVALID;
        }
    }
    else
    {
        DEBUG_ERR("Unsupported metadata insertion context!!");
        ret = XA_RESULT_PARAMETER_INVALID;
    }
    DEBUG_API("<-XAMetadataInsertionItfAdapt_GetKeySize");
    return ret;
}

/*
 * XAresult XAMetadataInsertionItfAdapt_GetKey(XAAdaptationGstCtx *bCtx,
 *                                          XAint32 nodeID,
 *                                          XAuint32 keyIndex,
 *                                          XAuint32 keySize,
 *                                          XAMetadataInfo *pKey)
 */
XAresult XAMetadataInsertionItfAdapt_GetKey(XAAdaptationGstCtx *bCtx,
                                           XAint32 nodeID,
                                           XAuint32 keyIndex,
                                           XAuint32 keySize,
                                           XAMetadataInfo *pKey)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAuint32 datasize, neededsize;

    DEBUG_API("->XAMetadataInsertionItfAdapt_GetKey");
    if(bCtx && bCtx->baseObj.ctxId == XAMediaRecorderAdaptation)
    {
        memset(pKey,0,keySize);
        if(xaMetadataAdaptSupportedKeys[keyIndex] && (keySize > sizeof(XAMetadataInfo)))
        {
            neededsize = strlen(xaMetadataAdaptSupportedKeys[keyIndex])+1;
            datasize = keySize-sizeof(XAMetadataInfo)+1;
            if(neededsize>datasize)
            {
                strncpy((char*)pKey->data,xaMetadataAdaptSupportedKeys[keyIndex],datasize);
                pKey->size=datasize;
                ret=XA_RESULT_BUFFER_INSUFFICIENT;
            }
            else
            {
                strncpy((char*)pKey->data,xaMetadataAdaptSupportedKeys[keyIndex],neededsize);
                pKey->size=neededsize;
            }
            pKey->encoding = XA_CHARACTERENCODING_ASCII;
            strcpy((char*)(pKey->langCountry),"en");
        }
        else
        {
            ret = XA_RESULT_PARAMETER_INVALID;
        }
    }
    else
    {
        DEBUG_ERR("Unsupported metadata insertion context!!");
        ret = XA_RESULT_PARAMETER_INVALID;
    }
    DEBUG_API("<-XAMetadataInsertionItfAdapt_GetKey");
    return ret;
}

/*
 * XAresult XAMetadataInsertionItfAdapt_GetFreeKeysEncoding(XAAdaptationGstCtx *bCtx,
 *                                                       XAint32 nodeID,
 *                                                       XAuint32 encodingIndex,
 *                                                       XAuint32 *pEncoding)
 */
XAresult XAMetadataInsertionItfAdapt_GetFreeKeysEncoding(XAAdaptationGstCtx *bCtx,
                                                        XAint32 nodeID,
                                                        XAuint32 encodingIndex,
                                                        XAuint32 *pEncoding)
{
    DEBUG_API("->XAMetadataInsertionItfAdapt_GetFreeKeysEncoding");
    DEBUG_API("<-XAMetadataInsertionItfAdapt_GetFreeKeysEncoding");
    /* no free keys*/
    return XA_RESULT_PRECONDITIONS_VIOLATED;
}

/*
 * XAresult XAMetadataInsertionItfAdapt_InsertMetadataItem(XAAdaptationGstCtx *bCtx,
 *                                                      XAint32 nodeID,
 *                                                      XAMetadataInfo *pKey,
 *                                                      XAMetadataInfo *pValue,
 *                                                      XAboolean overwrite)
 */
XAresult XAMetadataInsertionItfAdapt_InsertMetadataItem(XAAdaptationGstCtx *bCtx,
                                                       XAint32 nodeID,
                                                       XAMetadataInfo *pKey,
                                                       XAMetadataInfo *pValue,
                                                       XAboolean overwrite)
{
    XAresult ret = XA_RESULT_SUCCESS;
    XAMetadataAdaptVars* mdv=NULL;
    XAMediaRecorderAdaptationCtx* mCtx;
    GstTagMergeMode mode = (overwrite?GST_TAG_MERGE_REPLACE:GST_TAG_MERGE_KEEP);
    gchar* parsedKey = (gchar*)XAMetadataAdapt_ParseKhronosKey((XAchar*)pKey->data);
    DEBUG_API("->XAMetadataInsertionItfAdapt_InsertMetadataItem");
    if(bCtx && bCtx->baseObj.ctxId == XAMediaRecorderAdaptation)
    {
        mCtx = (XAMediaRecorderAdaptationCtx*)bCtx;
        mdv = mCtx->metadatavars;
        if(mdv)
        {
            if( nodeID==XA_ROOT_NODE_ID )
            {
                if(!mdv->generaltags)
                {
                    mdv->generaltags = gst_tag_list_new();
                }
                if (gst_tag_get_type (parsedKey) == G_TYPE_STRING)
                {
                    gst_tag_list_add(mdv->generaltags, mode, parsedKey, (gchar*)pValue->data, NULL );
                }
                else
                {
                    gst_tag_list_add(mdv->generaltags, mode, parsedKey, atoi((gchar*)pValue->data), NULL );
                }
            }
            else
            {
                if(nodeID == (XAint32)mdv->audiotags || nodeID == (XAint32)mdv->videotags)
                {
                    if (gst_tag_get_type (parsedKey) == G_TYPE_STRING)
                    {
                        gst_structure_set((GstStructure*)nodeID, parsedKey, G_TYPE_STRING, g_strdup((gchar*)pValue->data), NULL );
                    }
                    else
                    {
                        gst_structure_set((GstStructure*)nodeID, parsedKey, gst_tag_get_type (parsedKey), atoi((gchar*)pValue->data), NULL );
                    }
                }
                else
                {
                    DEBUG_ERR("Unknown node!!");
                    ret = XA_RESULT_PARAMETER_INVALID;
                }
            }
            if( ret == XA_RESULT_SUCCESS )
            {   /* let (possible) extraction itf to know new tags  */
                XAAdaptEvent event = {XA_METADATAEVENTS, XA_ADAPT_MDE_TAGS_AVAILABLE, 0, NULL };
                XAAdaptationBase_SendAdaptEvents(&bCtx->baseObj, &event );
            }
        }
        else
        {
            DEBUG_ERR("Metadata not initialized!!");
            ret = XA_RESULT_PARAMETER_INVALID;
        }
    }
    else
    {
        DEBUG_ERR("Unsupported metadata insertion context!!");
        ret = XA_RESULT_PARAMETER_INVALID;
    }
    DEBUG_API("<-XAMetadataInsertionItfAdapt_InsertMetadataItem");
    return ret;
}

/*****************************************************************************
 * INTERNALS
 *****************************************************************************/


/*
 * GstStructure* XAMetadataAdapt_GetChildStructure(XAMetadataAdaptVars* mdv, XAuint32 index)
 * helper to get gst structure corresponding to child index
 */
GstStructure* XAMetadataAdapt_GetChildStructure(XAMetadataAdaptVars* mdv, XAuint32 index)
{
    if(mdv->nodedepth == 0)
    {   /*root childs, if existent, the order is 1)video 2)audio*/
        if (index==0)
        {
            if(mdv->videotags) return mdv->videotags;
            else if (mdv->audiotags) return mdv->audiotags;
        }
        else if (index==1)
        {
            if (mdv->audiotags) return mdv->audiotags;
        }
    }
    else
    {
        if(index==XA_NODE_PARENT)
        {
            return GST_STRUCTURE(mdv->generaltags);
        }
    }
    /*else*/
    return NULL;
}

/*
 * XAMetadataAdaptVars* XAMetadataAdapt_GetMetadataVars(XAAdaptationGstCtx *bCtx)
 * Return metadata variables from different types of context
 */
XAMetadataAdaptVars* XAMetadataAdapt_GetMetadataVars(XAAdaptationGstCtx *bCtx)
{
    if( bCtx->baseObj.ctxId == XAMDAdaptation ||
        bCtx->baseObj.ctxId == XAMediaPlayerAdaptation )
    {
        return ((XAMediaPlayerAdaptationCtx*)bCtx)->metadatavars;
    }
    else if( bCtx->baseObj.ctxId == XAMediaRecorderAdaptation )
    {
        return ((XAMediaRecorderAdaptationCtx*)bCtx)->metadatavars;
    }
    else
    {
        DEBUG_ERR("Invalid context for metadata");
        return NULL;
    }
}

/*
 * gboolean XAMetadataAdapt_CountTags(GQuark field_id, const GValue* value, gpointer counter)
 * GstStructureForeachFunc to count number of items inside list
 */
gboolean XAMetadataAdapt_CountTags(GQuark field_id, const GValue* value, gpointer counter)
{
    XAuint32* cnt = (XAuint32*)counter;
    (*cnt)++;
    return TRUE;
}

/*
 * void XAMetadataAdapt_ParseTag(const GstTagList* list, const gchar* tag, gpointer listptr)
 * GstTaglistForeachFunc to parse items inside gsttaglist
 */
void XAMetadataAdapt_ParseTag(const GstTagList* list, const gchar* tag, gpointer listptr)
{
    XAMetadataImplTagList* tagList = (XAMetadataImplTagList*)listptr;
    const gchar *key;
    gchar *value;
    guint32 keylen, valuelen;

    DEBUG_API("->XAMetadataAdapt_ParseTag");
    key = gst_tag_get_nick(tag);
    /*include null-terminator*/
    keylen = strlen(key)+1;
    tagList->mdeKeys[tagList->itemcount] = (XAMetadataInfo*)calloc(1,keylen+sizeof(XAMetadataInfo));
    tagList->mdeKeys[tagList->itemcount]->size = keylen;
    strncpy((char*)tagList->mdeKeys[tagList->itemcount]->data,key,keylen-1);

    if (gst_tag_get_type (tag) == G_TYPE_STRING)
    {
        gst_tag_list_get_string_index(list, tag, 0, &value);
    }
    else
    {
        value = g_strdup_value_contents (gst_tag_list_get_value_index (list, tag, 0));
    }
    /*include null-terminator*/
    valuelen = strlen(value)+1;
    tagList->mdeValues[tagList->itemcount] = (XAMetadataInfo*)calloc(1,valuelen+sizeof(XAMetadataInfo));
    tagList->mdeValues[tagList->itemcount]->size = valuelen;
    strncpy((char*)tagList->mdeValues[tagList->itemcount]->data,value,valuelen-1);

    /* NOTE: for now, encoding and language fixed */
    tagList->mdeKeys[tagList->itemcount]->encoding = XA_CHARACTERENCODING_ASCII;
    tagList->mdeValues[tagList->itemcount]->encoding = XA_CHARACTERENCODING_ASCII;
    strcpy((char*)tagList->mdeKeys[tagList->itemcount]->langCountry,"en");
    strcpy((char*)tagList->mdeValues[tagList->itemcount]->langCountry,"en");
    tagList->itemcount++;
    DEBUG_API_A2("<-XAMetadataAdapt_ParseTag: added %s : %s",
                 tagList->mdeKeys[tagList->itemcount-1]->data,
                 tagList->mdeValues[tagList->itemcount-1]->data);
}

/*
 * gboolean XAMetadataAdapt_ParseItem(GQuark gkey,
 *                                const GValue* gvalue,
 *                                gpointer listptr)
 * GstStructureForeachFunc to parse items inside caps structure
 */
gboolean XAMetadataAdapt_ParseItem(GQuark gkey,
                                 const GValue* gvalue,
                                 gpointer listptr)
{
    XAMetadataImplTagList* tagList = (XAMetadataImplTagList*)listptr;
    const gchar *key;
    gchar *value;
    guint32 keylen, valuelen;

    DEBUG_API("->XAMetadataAdapt_ParseItem");
    key = g_quark_to_string(gkey);
    /*include null-terminator*/
    keylen = strlen(key)+1;
    tagList->mdeKeys[tagList->itemcount] = (XAMetadataInfo*)calloc(1,keylen+sizeof(XAMetadataInfo));
    tagList->mdeKeys[tagList->itemcount]->size = keylen;
    strncpy((char*)tagList->mdeKeys[tagList->itemcount]->data,key,keylen-1);

    value = gst_value_serialize(gvalue);
    /*include null-terminator*/
    valuelen = strlen(value)+1;
    tagList->mdeValues[tagList->itemcount] = (XAMetadataInfo*)calloc(1,valuelen+sizeof(XAMetadataInfo));
    tagList->mdeValues[tagList->itemcount]->size = valuelen;
    strncpy((char*)tagList->mdeValues[tagList->itemcount]->data,value,valuelen-1);

    /* for Gst, encoding and language fixed */
    tagList->mdeKeys[tagList->itemcount]->encoding = XA_CHARACTERENCODING_ASCII;
    tagList->mdeValues[tagList->itemcount]->encoding = XA_CHARACTERENCODING_ASCII;
    strcpy((char*)tagList->mdeKeys[tagList->itemcount]->langCountry,"en");
    strcpy((char*)tagList->mdeValues[tagList->itemcount]->langCountry,"en");
    tagList->itemcount++;
    DEBUG_API_A2("<-XAMetadataAdapt_ParseItem: added %15s : %s",
                 tagList->mdeKeys[tagList->itemcount-1]->data,
                 tagList->mdeValues[tagList->itemcount-1]->data);
    return TRUE;
}

/*
 * gboolean XAMetadataAdapt_GstTagCb( GstBus *bus, GstMessage *message, gpointer data )
 * Metadata callback - called when new tags found from stream
 */
gboolean XAMetadataAdapt_GstTagCb( GstBus *bus, GstMessage *message, gpointer data )
{
    GstTagList *new_tags;
    GstTagList **old_tags = NULL;
    XAAdaptationGstCtx* bCtx;
    GstTagMergeMode mode;

    bCtx = (XAAdaptationGstCtx*)data;
    if(GST_MESSAGE_TAG==GST_MESSAGE_TYPE(message))
    {
        DEBUG_API_A2("->XAMetadataAdapt_GstTagCb:\"%s\" from object \"%s\"",
                        GST_MESSAGE_TYPE_NAME(message), GST_OBJECT_NAME(GST_MESSAGE_SRC(message)));
        gst_message_parse_tag (message, &new_tags);
        /* NOTE: only general tags received this way (not child nodes)*/
        if( bCtx->baseObj.ctxId == XAMDAdaptation ||
            bCtx->baseObj.ctxId == XAMediaPlayerAdaptation )
        {
            old_tags = &((XAMediaPlayerAdaptationCtx*)bCtx)->metadatavars->generaltags;
            mode = GST_TAG_MERGE_REPLACE;
        }
        else if( bCtx->baseObj.ctxId == XAMediaRecorderAdaptation )
        {
            old_tags = &((XAMediaRecorderAdaptationCtx*)bCtx)->metadatavars->generaltags;
            /* keep user's tags */
            mode = GST_TAG_MERGE_KEEP;
        }
        else
        {   /*context not applicable*/
            return TRUE;
        }

        if( *old_tags )
            *old_tags = gst_tag_list_merge (*old_tags, new_tags, mode);
        else
            *old_tags = new_tags;

        if( GST_STATE(bCtx->bin)==GST_STATE_PLAYING )
        {   /* send event only for live stream, otherwise wait for preroll to complete */
            XAAdaptEvent event = {XA_METADATAEVENTS, XA_ADAPT_MDE_TAGS_AVAILABLE, 0, NULL };
            XAAdaptationBase_SendAdaptEvents(&bCtx->baseObj, &event );
        }
        DEBUG_API("<-XAMetadataAdapt_GstTagCb");
    }
    return TRUE;
}

/*
 * void XAMetadataAdapt_TryWriteTag(const GstTagList* list, const gchar* tag, gpointer taggerptr)
 * GstTaglistForeachFunc to write single tag entries to stream
 */
void XAMetadataAdapt_TryWriteTag(const GstTagList* list, const gchar* tag, gpointer taggerptr)
{
    GstTagSetter* tagger = (GstTagSetter*)taggerptr;
    const gchar *key;
    gchar *value;

    DEBUG_API("->XAMetadataAdapt_ParseTag");
    key = gst_tag_get_nick(tag);
    if (gst_tag_get_type (tag) == G_TYPE_STRING)
    {
        gst_tag_list_get_string_index(list, tag, 0, &value);
    }
    else
    {
        value = g_strdup_value_contents (gst_tag_list_get_value_index (list, tag, 0));
    }
    gst_tag_setter_add_tags(tagger, GST_TAG_MERGE_REPLACE, key, value, NULL);
}

/*
 * XAresult XAMetadataAdapt_TryWriteTags(XAAdaptationGstCtx* bCtx, GstBin* binToWriteTo)
 */
XAresult XAMetadataAdapt_TryWriteTags(XAAdaptationGstCtx* bCtx, GstBin* binToWriteTo)
{
    XAresult ret = XA_RESULT_SUCCESS;
    GstTagSetter *tagger=NULL;
    /* Find metadata tagger from bin (if any) */
    GstIterator *iter = gst_bin_iterate_all_by_interface( binToWriteTo, GST_TYPE_TAG_SETTER );
    gboolean done = FALSE;
    XAMediaRecorderAdaptationCtx* mCtx;
    XAAdaptEvent mdevent = {XA_METADATAEVENTS, XA_ADAPT_MDE_TAGS_WRITTEN, 0, NULL };

    DEBUG_API("->XAMetadataAdapt_TryWriteTags");

    if( bCtx->baseObj.ctxId != XAMediaRecorderAdaptation )
    {
        return XA_RESULT_FEATURE_UNSUPPORTED;
    }
    mCtx = ((XAMediaRecorderAdaptationCtx*)bCtx);
    while (!done)
    {
        switch (gst_iterator_next(iter, (gpointer)&tagger))
        {
        case GST_ITERATOR_OK:
            if(mCtx->metadatavars)
            {
                if(mCtx->metadatavars->generaltags)
                {
                    gst_tag_setter_merge_tags(tagger, mCtx->metadatavars->generaltags, GST_TAG_MERGE_REPLACE);
                }
            }
            done = TRUE;
        break;
        case GST_ITERATOR_ERROR:
            done = TRUE;
            DEBUG_ERR("Pipeline does not support tag setting");
            ret = XA_RESULT_CONTENT_UNSUPPORTED;
        break;
        case GST_ITERATOR_DONE:
            done = TRUE;
            DEBUG_ERR("Pipeline does not support tag setting");
            ret = XA_RESULT_CONTENT_UNSUPPORTED;
        break;
        case GST_ITERATOR_RESYNC:
            gst_iterator_resync(iter);
        break;
        }
    }
    /*here datasize field is used for return value*/
    mdevent.datasize = ret;
    XAAdaptationBase_SendAdaptEvents(&bCtx->baseObj, &mdevent );
    gst_iterator_free (iter);
    DEBUG_API("<-XAMetadataAdapt_TryWriteTags");
    return ret;
}
