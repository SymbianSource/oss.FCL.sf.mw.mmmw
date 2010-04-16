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

#include "XANGAVideoSink.h"
#include "openmaxalwrapper.h"


CNgaVideoSink* CNgaVideoSink::NewL()
{
	CNgaVideoSink* self = new CNgaVideoSink();
	self->ConstructL();
	return self;
}

CNgaVideoSink::~CNgaVideoSink()
{
	iMediaClientVideoDisplay->RemoveDisplayWindow(*m_pWindow);
	delete iMediaClientVideoDisplay;
  if(IsActive())
  {
     Cancel();
  }	
}
    
CNgaVideoSink::CNgaVideoSink()
                  :CActive(EPriorityStandard)
{
	iMediaClientVideoDisplay = NULL;
  CActiveScheduler::Add(this);
  m_ThreadHandle.Duplicate(RThread());
  m_MediaHelperInitialized = EInitializedReady;
  m_rotation = EVideoRotationNone;
  m_scaleWidth = (100.0f);
  m_scaleHeight = (100.0f);
  m_horizPos = (EHorizontalAlignCenter);
  m_vertPos = (EVerticalAlignCenter);      
}

void CNgaVideoSink::RunL()
{
	  iMediaClientVideoDisplay->SurfaceCreated(m_surfaceId,m_cropRect,m_pixelAspectRatio,m_cropRegion);	
}

void CNgaVideoSink::Activate()
{
    if(!IsActive())
    {
        iStatus = KRequestPending;
        SetActive();
    }
}

void CNgaVideoSink::DoCancel()
{
      if(iStatus.Int() == KRequestPending)
      {
          TRequestStatus* pStatus = &iStatus;
          m_ThreadHandle.RequestComplete(pStatus, KErrCancel);
    }
}

void CNgaVideoSink::CompleteRequest(TInt status)
{
    // Checking for IsActive alone results in this block getting executed 
    // from other thread more thanone time. Hence the chk for iStatus
    if(IsActive() && iStatus == KRequestPending)
    {
        TRequestStatus* pStatus = &iStatus;
        m_ThreadHandle.RequestComplete(pStatus, status);
    }
}

void CNgaVideoSink::SurfaceCreated(int surfaceid0,int surfaceid1,int surfaceid2,int surfaceid3,int crop_rect_tl_x,
                     									 int crop_rect_tl_y,int crop_rect_br_x,int crop_rect_br_y,int aspect_ratio_num,int aspect_ratio_denom)
{
   
   m_surfaceId.iInternal[0] = surfaceid0;
   m_surfaceId.iInternal[1] = surfaceid1;
   m_surfaceId.iInternal[2] = surfaceid2;
   m_surfaceId.iInternal[3] = surfaceid3;
   m_cropRect.iTl.iX = crop_rect_tl_x;
   m_cropRect.iTl.iY = crop_rect_tl_y;
   m_cropRect.iBr.iX = crop_rect_br_x;
   m_cropRect.iBr.iY = crop_rect_br_y;
   m_pixelAspectRatio.iNumerator = aspect_ratio_num;
   m_pixelAspectRatio.iDenominator = aspect_ratio_denom;  
   
   Activate();
   CompleteRequest(KErrNone);
   	
}

void CNgaVideoSink::ConstructL()
{
	iMediaClientVideoDisplay = CMediaClientVideoDisplay::NewL(0);
	Activate();
} 

void CNgaVideoSink::SetNativeDisplayInformation(void* display_info)
{
	//display_info is of type XADataSink
	//display_info.pLocator is of type XADataLocator_NativeDisplay
	XADataLocator_NativeDisplay* nativeDisplay;
	XADataSink* videoSink = (XADataSink*)display_info;
	
	nativeDisplay = (XADataLocator_NativeDisplay*) (videoSink->pLocator);
	//TODO: scrDevice is not been passed Properly
  // Add the display window
  m_cropRegion = TRect(((RWindow*)(nativeDisplay->hWindow))->Size());
  m_videoExtent = TRect(((RWindow*)(nativeDisplay->hWindow))->Size());
  m_cropRect = TRect(((RWindow*)(nativeDisplay->hWindow))->Size());
  m_clipRect = TRect(((RWindow*)(nativeDisplay->hWindow))->Size());
  m_cropRegion = TRect(((RWindow*)(nativeDisplay->hWindow))->Size());
  m_pWindow = ((RWindow*)(nativeDisplay->hWindow));
  

  iMediaClientVideoDisplay->AddDisplayWindowL(m_pWindow, m_clipRect, m_cropRegion, m_videoExtent, m_scaleWidth, m_scaleHeight,
                                               m_rotation, EAutoScaleBestFit, m_horizPos, m_vertPos, m_pWindow);  

}

extern "C" {

    void* nga_video_sink_init()
    { 
          return CNgaVideoSink::NewL();
    }
        
    void surface_created(void* context,int surfaceid0,int surfaceid1,int surfaceid2,int surfaceid3,int crop_rect_tl_x,
                         									int crop_rect_tl_y,int crop_rect_br_x,int crop_rect_br_y,int aspect_ratio_num,int aspect_ratio_denom)
    {
            ((CNgaVideoSink*)(context))->SurfaceCreated(surfaceid0,surfaceid1,surfaceid2,surfaceid3,crop_rect_tl_x,
                         									                  crop_rect_tl_y,crop_rect_br_x,crop_rect_br_y,aspect_ratio_num,aspect_ratio_denom);
    }
        
    void  setup_native_display(void* context, void* display_info)
    {
            ((CNgaVideoSink*)(context))->SetNativeDisplayInformation(display_info);
    }

}
