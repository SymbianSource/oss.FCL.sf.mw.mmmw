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
/*
Media Helper Class for handling surface events

Shy Ward
*/

#ifndef XA_CNGAVIDEOSINK_H
#define XA_CNGAVIDEOSINK_H

#define ENABLE_GRAPHICS_SURFACE_INTEGRATION


#ifdef __cplusplus

#include <e32base.h>
#ifdef ENABLE_GRAPHICS_SURFACE_INTEGRATION
#include <graphics/surface.h> //TSurfaceId
#include <mmf/common/mmfvideo.h> //TVideoAspectRatio
#include <mediaclientvideodisplay.h>
#endif

NONSHARABLE_CLASS(CNgaVideoSink) : public CBase,
                      public CActive
    {
public:

    static CNgaVideoSink* NewL();
    ~CNgaVideoSink();
    void SurfaceCreated(int surfaceid0,int surfaceid1,int surfaceid2,int surfaceid3,int crop_rect_tl_x,
         							 int crop_rect_tl_y,int crop_rect_br_x,int crop_rect_br_y,int aspect_ratio_num,int aspect_ratio_denom);    
    void SetNativeDisplayInformation(void* display_info);
    
    // CActive methods
    virtual void RunL();
    virtual void DoCancel();
    void Activate();
    void RemoveWindow();
    void CompleteRequest(TInt status = KErrNone);    
    
private:
    CNgaVideoSink();
    void ConstructL();
    
    RThread     m_ThreadHandle;
    
    enum InitializeState
    {
        EInitializedReady = 0,
        EInitializedSuccess,
        EInitializedFailed,
    };
    
#ifdef ENABLE_GRAPHICS_SURFACE_INTEGRATION
    CMediaClientVideoDisplay* iMediaClientVideoDisplay;
    TSurfaceId surfaceId;
    TRect m_clipRect;
    TRect m_videoExtent;    
    TRect m_cropRegion;
    TVideoAspectRatio m_pixelAspectRatio;
    RWindow* m_pWindow;
    TRect m_cropRect;
    TSurfaceId m_surfaceId;
    InitializeState     m_MediaHelperInitialized;    
    TVideoRotation m_rotation;
    TReal32 m_scaleWidth;
    TReal32 m_scaleHeight;
    TInt m_horizPos;
    TInt m_vertPos;    

#endif /*ENABLE_GRAPHICS_SURFACE_INTEGRATION*/    
    };
#else  /* __cplusplus */


extern void* nga_video_sink_init();

extern void  surface_created(void* context,int surfaceid0,int surfaceid1,int surfaceid2,int surfaceid3,int crop_rect_tl_x,
                     									int crop_rect_tl_y,int crop_rect_br_x,int crop_rect_br_y,int aspect_ratio_num,int aspect_ratio_denom);
extern void  setup_native_display(void* context, void* display_info);


#endif /* __cplusplus */

#endif /* XA_CNGAVIDEOSINK_H */
