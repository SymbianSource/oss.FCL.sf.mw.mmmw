/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  EMC Api Test App
*
*/



// INCLUDE FILES
#include "EnhanMediaTestClass.h"
#include "debug.h"
#include <StreamControlObserver.h>
#include <SourceControlObserver.h>


using namespace multimedia;

using multimedia::MSourceControlObserver;
using multimedia::MStreamControlObserver;





/************************************************************************************************************/

void CEnhanMediaTestClass::Event( MControl* aControl, TUint aEventType, TAny* aEventObject )
	{
	TInt controlTy;
   // TControlType controlType;
    controlTy = aControl->ControlType();
    RDebug::Print(_L("CEnhanMediaTestClass::Event [%d]"),controlTy);
	switch ( controlTy )
		{
		case EStreamControl:
			    {
		    	switch (aEventType)
			        {
			        case MStreamControlObserver::KStateChangedEvent:
			            {
			            MStreamControl* control1 = (MStreamControl*)(aControl);
			            RDebug::Print(_L("CEnhanMediaTestClass::Event:EStateChanged"));
			            switch(control1->GetState())
			                {
			                case MStreamControl::CLOSED:
								RDebug::Print(_L("CEnhanMediaTestClass::Event:EStateChanged[Closed]"));
			                    break;
			                case MStreamControl::INITIALIZED:
							    {
					            iLog->Log(_L("CEnhanMediaTestClass::Event:EStateChanged[Opened]"));
							    RDebug::Print(_L("CEnhanMediaTestClass::Event:EStateChanged[Opened]"));
							    }
			                    break;
			                case MStreamControl::PRIMED:
			                	{
					            iLog->Log(_L("CEnhanMediaTestClass::Event:EStateChanged[Primed]"));
			                	RDebug::Print(_L("CEnhanMediaTestClass::Event:EStateChanged[Primed]"));
			                	}
							    break;
			                case MStreamControl::EXECUTING:
			                	RDebug::Print(_L("CEnhanMediaTestClass::Event:EStateChanged[Playing]"));
			                    break;
			                case MStreamControl::BUFFERING:
			                	RDebug::Print(_L("CEnhanMediaTestClass::Event:EStateChanged[Buffering]"));
			                    break;
			                case MStreamControl::PAUSED:
							    RDebug::Print(_L("CEnhanMediaTestClass::Event:EStateChanged[Paused]"));
			                    break;
			                default:
			                    break;
			                };
			            }
			            break;

			        case MStreamControlObserver::KDurationChangedEvent:
			        	RDebug::Print(_L("CEnhanMediaTestClass::Event:KDurationChangedEvent"));
			            break;
					};
				}
        		break;

        case ESourceControl:
 			    {
 			    switch (aEventType)
			        {
			        case MSourceControlObserver::KPercentageDownloadedChangedEvent:
			        	{
			        	TUint percent;
			        	MProgDLSource* control1 = (MProgDLSource*)(aControl);
			        	control1->GetPercentageBuffered(percent);
			            iLog->Log(_L("KPercentageDownloadedChangedEvent"));
			        	}
			            break;
			        case MSourceControlObserver::KBufferingTypesSupportChangedEvent:
			        	{
			            iLog->Log(_L("KBufferingTypesSupportChangedEvent"));
			        	}
			            break;
			        case MSourceControlObserver::KBufferProcessedEvent:
			        	{
			            MBufferProcessedEvent* evt = (MBufferProcessedEvent*)aEventObject;
			            RDebug::Print(_L("CEnhanMediaTestClass::Event:EBufferProcessed[AudioBuffer[0x%x]Reason[%d]]") , evt->GetDataBuffer(), evt->GetErrorCode() );
			            iLog->Log(_L("CEnhanMediaTestClass::Event:EBufferProcessed[AudioBuffer[0x%x]Reason[%d]]") , evt->GetDataBuffer(), evt->GetErrorCode() );
			        	}
			            break;
			        case MSourceControlObserver::KBitRateChangedEvent:
			        	{
		            	TUint rate;
			            iMFileSource->GetBitRate(rate);
			            RDebug::Print(_L("CEnhanMediaTestClass::Event:BitRateChanged[%d]"),rate);
			            iLog->Log(_L("CEnhanMediaTestClass::Event:BitRateChanged[%d]"),rate);
			        	}
			            break;
			        case MSourceControlObserver::KDownloadStatusChangedEvent:
			            {
			            MProgDLSource* control1 = (MProgDLSource*)(aControl);
			            RDebug::Print(_L("CMPlayerAudioUIController::Event:DownloadStatus[%d]"),control1->GetDownloadStatus());
			            iLog->Log(_L("CMPlayerAudioUIController::Event:DownloadStatus[%d]"),control1->GetDownloadStatus());
			            switch(control1->GetDownloadStatus())
			            	{

			            	case MProgDLSource::EConnecting:
			            	    {
			            	    RDebug::Print(_L("CEnhanMediaTestClass::Event:DownloadStatus [connecting]"));
					            iLog->Log(_L("CEnhanMediaTestClass::Event:DownloadStatus [connecting]"));
			            	    }
			            	    break;

			            	case MProgDLSource::EStarted:
			            		{
			            		RDebug::Print(_L("CEnhanMediaTestClass::Event:DownloadStatus [started]"));
					            iLog->Log(_L("CEnhanMediaTestClass::Event:DownloadStatus [started]"));
			            		}
			            	    break;

			            	case MProgDLSource::EPaused:
			            		{
			            	    RDebug::Print(_L("CEnhanMediaTestClass::Event:DownloadStatus [paused]"));
					            iLog->Log(_L("CEnhanMediaTestClass::Event:DownloadStatus [paused]"));
			            		}
			            	    break;

			            	case MProgDLSource::EDeleted:
			            		{
			            	    RDebug::Print(_L("CEnhanMediaTestClass::Event:DownloadStatus [deleted]"));
					            iLog->Log(_L("CEnhanMediaTestClass::Event:DownloadStatus [deleted]"));
			            		}
			                    break;
			            	case MProgDLSource::ECompleted:
			            		{
			            	    RDebug::Print(_L("CEnhanMediaTestClass::Event:DownloadStatus [completed]"));
					            iLog->Log(_L("CEnhanMediaTestClass::Event:DownloadStatus [completed]"));
			            		}
			            	    break;
			   	            case MProgDLSource::EFailed:
			                    {
			        	        RDebug::Print(_L("CEnhanMediaTestClass::Event:DownloadStatus [failed]"));
					            iLog->Log(_L("CEnhanMediaTestClass::Event:DownloadStatus [failed]"));
			                    break;
			                    }
			    	        case MProgDLSource::EUnknown:
			        	        {
			        	        RDebug::Print(_L("CEnhanMediaTestClass::Event:DownloadStatus [EUnknown]"));
					            iLog->Log(_L("CEnhanMediaTestClass::Event:DownloadStatus [EUnknown]"));
			        	        }
			        	        break;
			            	default:
			            	  break;
			            	}
			            }
			            break;

			        case MSourceControlObserver::KFileMoveCompleteEvent:
			            {
				            RDebug::Print(_L("CEnhanMediaTestClass::Event:KFileMoveCompleteEvent"));
				            iLog->Log(_L("CEnhanMediaTestClass::Event:KFileMoveCompleteEvent"));
			            }
			            break;


			        };
			    }
       			break;

       case ESinkControl:
			    {
                TUid controlUid = aControl->Type();
	            iLog->Log(_L("CEnhanMediaTestClass::Event Sink[%x] Event[%d]"),controlUid, aEventType);
                RDebug::Print(_L("CEnhanMediaTestClass::Event Sink[%x] Event[%d]"),controlUid, aEventType);
			    }
                break;

        case EEffectControl:
			    {
                TUid controlUid = aControl->Type();
	            iLog->Log(_L("CEnhanMediaTestClass::Event Effect[%x] Event[%d]"),controlUid, aEventType);
                RDebug::Print(_L("CEnhanMediaTestClass::Event Effect[%x] Event[%d]"),controlUid, aEventType);
			    }
			    break;
        default:
            break;
		};
	}


void CEnhanMediaTestClass::HandleDMgrEventL( RHttpDownload& aDownload, THttpDownloadEvent aEvent )
    {
    RDebug::Print(_L("HandleDMgrEventL DownloadState[%d] ProgressState[%d]"),aEvent.iDownloadState,aEvent.iProgressState);
    iLog->Log(_L("HandleDMgrEventL DownloadState[%d] ProgressState[%d]"),aEvent.iDownloadState,aEvent.iProgressState);

    TInt32 downloadSize = 0;
    TInt32 length = 0;
    TInt status = aDownload.GetIntAttribute(EDlAttrDownloadedSize, downloadSize);
    status = aDownload.GetIntAttribute(EDlAttrLength, length);


    TInt percentageDL = (downloadSize*100/length);

    if(percentageDL > 8 && iActive->IsStarted())
        {
        iActive->AsyncStop();
        }

    //iLog->Log(_L("iMProgDLSource->GetDownloadingRate = [%d]"),rate);
   // RDebug::Print(_L("CEnhanMediaTestClass::Event:EStateChanged[Opened]"));


    if(aEvent.iDownloadState == EHttpDlPaused)
        {
        iDownload->Start();
        }

    if(aEvent.iDownloadState == EHttpDlCreated)
        {
        iLog->Log(_L("EHttpDlCreated"));
        }

    if(aEvent.iDownloadState == EHttpDlInprogress)
        {
        iLog->Log(_L("EHttpDlInprogress "));
        }
    if(aEvent.iDownloadState == EHttpDlPaused)
        {
        iLog->Log(_L("EHttpDlPaused "));
        }
    if(aEvent.iDownloadState == EHttpDlCompleted)
        {
        iLog->Log(_L("EHttpDlCompleted "));
        }
    if(aEvent.iDownloadState == EHttpDlFailed)
        {
        iLog->Log(_L("EHttpDlFailed "));
        }
    }
// End of File
