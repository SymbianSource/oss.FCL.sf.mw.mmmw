/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   TNE server
*
*/



#include"HXTneserver.h"
#include"HXTneclientservercommon.h"
#include <flogger.h>
#include <f32file64.h>
static const TInt kDefaultStack = 0x4000;



// PRINT macro
#ifdef _DEBUG
#include <e32svr.h>
#define PRINT(x) RDebug::Print x
#else
#define PRINT(x)
#endif

#ifdef _DEBUG
#define ENABLE_S60_TNE_LOGGING 1
#endif

//implementation
#ifdef ENABLE_S60_TNE_LOGGING
#define FLOG FileLog
inline static void FileLog(TRefByValue<const TDesC16> aFmt,...)
{
    VA_LIST  args;
    VA_START (args, aFmt);
    RFileLogger::WriteFormat(_L("tne"), _L("S60tne.log"), 
                        EFileLoggingModeAppend, aFmt, args);
}

#else // logging is disabled
#define FLOG LogFNull
inline static void LogFNull(TRefByValue<const TDesC16> aFmt,...){}

#endif // enable logging




const TInt KMaxPacketToDecode = 32;

//////////////Server///////////////


CTneServer::CTneServer(CActive::TPriority aActiveObjectPriority)
    : CServer2(aActiveObjectPriority)
    {
    }



CSession2* CTneServer::NewSessionL(const TVersion& /* aVersion */,const RMessage2& 
/*aMessage*/) const 

{
	 return new (ELeave) CTneSession;
	}
	
	
	
GLDEF_C  TInt CTneServer::ThreadFunction(TAny*  aName)
{
	HBufC   *pServerName = (HBufC *) aName;
	CTrapCleanup* cleanup = NULL;
	CActiveScheduler *pA = NULL;
	CTneServer *pS = NULL;
	TInt err = KErrNoMemory;

    FLOG(_L("CTneServer::ThreadFunction ServerName=%S"), pServerName); 		
	  // get clean-up stack
	if (pServerName)
	{
    		cleanup =CTrapCleanup::New(); 
    	 	pA = new CActiveScheduler;    
    		pS = new CTneServer(EPriorityStandard);
    		
    	 }
     
    
    if (cleanup && pA && pS )
   {
   		// OK
   		CActiveScheduler::Install(pA);      
      	// Start the server
    	err =  pS->Start(*pServerName);
  	}
  	
  	RThread::Rendezvous(err);
   	
    CActiveScheduler::Start();

      // Tidy up...    
    delete pS;
    delete pA;
    delete cleanup; 
    
    
    return(err);
}


//global function

TInt  StartThread(RThread& aServerThread,  HBufC *pServerName)
    {
    TInt res=KErrNone;
    
    
       res=aServerThread.Create(*pServerName,
            CTneServer::ThreadFunction,
            kDefaultStack, &User::Heap(),            
            (TAny *)pServerName  );
            
          // The thread has been created OK so get it started - however
          // we need to make sure that it has started before we continue.
        if (res==KErrNone)
            {
            TRequestStatus rendezvousStatus;
            
            aServerThread.SetPriority(EPriorityNormal);
            aServerThread.Rendezvous(rendezvousStatus);
            aServerThread.Resume();
            User::WaitForRequest(rendezvousStatus);
            res = rendezvousStatus.Int();
             }
            
          // The thread has not been created - clearly there's been a problem.
        else
            {
            aServerThread.Close();
             }
       
    return res;
    }




	
	////////////////////////ServerSession//////////////////////////////
	
	
CTneSession::CTneSession(): 
        iWidth(0),                 
        iHeight(0) ,                                            
        iDuration(0)  ,                                        
        iFrameCount(0)    ,                                     
        m_LastError(KErrNone),                                          
        m_uPacketsReceived(0)    ,                                   
        m_bDone(EFalse),                                                      
        iFs(NULL),                                                    
        iYUVBuffer(NULL),
        iClientYUVBufferPtrPtr(NULL),                         
        m_bOpenFileLPending(EFalse),                          
        iGetThumbPending(EFalse),                        
        iCloseHandle(EFalse),        	                        
        ibOpenFilePending(EFalse),  
        iThumbIndex(0),
        iUtil(NULL),                                                
        m_State( ENotReady),
        m_bMetaDataReady(EFalse)                                   
        {
     FLOG(_L("CTneSession::CTneSession()in this=%x"), this);                      
     FLOG(_L("CTneSession::CTneSession()out this=%x"), this); 
          }                           
                         
                         
                                     
   CTneSession::~CTneSession() 
{                          
	 FLOG(_L("CTneSession::~CTneSession()in this=%x"), this); 	
     if(iYUVBuffer)        
     {    	               
        User::Free(iYUVBuffer);
        iYUVBuffer = NULL; 
	 }                     
	                       
	 if (iCloseHandle)     
	 {	                   
	    iFileHandle.Close();
	 }                     
	 if (iUtil)            
	 {                     
	    delete iUtil;      
	    iUtil = NULL;      
	 }                     
	 if(iFs)               
    {                      
	    iFs->Close();      
	    delete iFs;
	    iFs = NULL;
    }
	 CompleteRequest(KErrCancel);
	 CompleteCancelRequest();	 
	 CActiveScheduler::Stop();
   
    FLOG(_L("CTneSession::~CTneSession()out this=%x"), this);                	
}


	/**
Services a client request.
*/
void CTneSession::ServiceL(const RMessage2& aMessage)
    {
        DispatchMessageL(aMessage); 
    }


void CTneSession::DispatchMessageL(const RMessage2& aMessage)
    {

   	TBool bCompleteRequest = EFalse;
   	TInt lError = KErrNone;
    RFile* pFileHandle;	
    TFileName *pFileName;
    TInt aPosition;
    TNEMetaData* pMetaData;
    TNEThumbRequest *pThumbRequestData;
    RFile64 aFilehandle;
    
    FLOG(_L("CTneSession::DispatchMessageL in type=%d"), aMessage.Function());
    
    switch (aMessage.Function())
        {        	        
    case  EOpenFileRFmsg:
        m_State = EStartGettingMetadata;
        bCompleteRequest = ETrue;
        iClientRequest = aMessage;
     	pFileHandle = ( RFile* ) aMessage.Ptr0(); // Handle to read Message data     
     	aPosition  = (TInt  ) aMessage.Ptr1();
     	
     	lError = iFileHandle.Duplicate(*pFileHandle);
     	if (lError == KErrNone)
     	{
            iCloseHandle = ETrue;
            lError = DoOpenFile(iFileHandle, aPosition);            
        }
       
        if( m_State == EStartGettingThumbNailWithIndex)
        {
            lError = ReOpenFile(iFileHandle);            
        }
                
        CompleteCancelRequest(); // it will check also if cancel needs to be done.
        
        break;
        
//fix me - Avoid duplicate code -common func (Future Work)
    case  EOpenFIleNamemsg:
        iFs = NULL;
        m_State = EStartGettingMetadata;
        iClientRequest = aMessage;
     	pFileName = (TFileName* ) aMessage.Ptr0();    	      
     	aPosition  = (TInt  ) aMessage.Ptr1();
     
     iFs = new RFs;
     if(iFs == NULL)
	 {
	 	lError = KErrNoMemory;
	 	    	bCompleteRequest = ETrue;
 	 }
    else if ( (lError  = iFs->Connect())!= KErrNone)
    {
    	    	bCompleteRequest = ETrue;
     }
     else if ((lError = aFilehandle.Open(*iFs,*pFileName, EFileShareReadersOnly | EFileStream | EFileRead))!= KErrNone)
    {
    	    	bCompleteRequest = ETrue;
      }
	else if ((lError = iFileHandle.Duplicate(aFilehandle))!= KErrNone)
	{
    	    	bCompleteRequest = ETrue;
    	    	   aFilehandle.Close();

      }
     else if ( (lError = DoOpenFile(iFileHandle, aPosition)) != KErrNone )
        {
            bCompleteRequest = ETrue;
            aFilehandle.Close();      
            iCloseHandle = ETrue;
        }
        else 
        {
            bCompleteRequest = ETrue;
            aFilehandle.Close();
            iCloseHandle = ETrue;
        }

        
        if( m_State == EStartGettingThumbNailWithIndex)
        {
            lError = ReOpenFile(iFileHandle);            
        }
                
        CompleteCancelRequest(); // it will check also if cancel needs to be done.
        
        if (lError  !=  KErrNone)
        {
            bCompleteRequest = ETrue;
        }                        
        
        break;
      case EGetMetaDatamsg:
        iClientRequest = aMessage;
      	pMetaData = ( TNEMetaData* ) aMessage.Ptr0(); 
        pMetaData->iWidth = iWidth;
        pMetaData->iHeight = iHeight;
        pMetaData->iFrameCount = iFrameCount;
        bCompleteRequest = ETrue;
       
        break;
        
      case EGetThumbmsg:
        
        iClientRequest = aMessage;
        pThumbRequestData  = ( TNEThumbRequest * ) aMessage.Ptr0();
        // store thumb request parameters       	    
        iClientYUVBufferPtrPtr  = &(pThumbRequestData->iYUVBuffer);        
        iThumbIndex = pThumbRequestData->iIndex;
        if (iThumbIndex == KBestThumbIndex || iThumbIndex <= 1)
        {
            m_State = EStartGettingThumbNail;
            DoGetThumb();
        }
        else
        {
            m_State = EStartGettingThumbNailWithIndex;
			if (!ibOpenFilePending)
			{
			lError = ReOpenFile(iFileHandle); 
			}                
		}
        break;
    case ECancelThumbmsg:
        iGetThumbPending = EFalse;
        iCancelRequest = aMessage;        
        m_State = ECancelling;               
        if(iUtil)
        {
        	iUtil->CancelThumb();
        }
        // cancel any pending getthumb or openfile request.
        lError = KErrCancel;
        CompleteRequest(lError); 
        
        if (!ibOpenFilePending)
        {
           CompleteCancelRequest(); 
        }
        break;    
    
   default:
      //  PanicClient(aMessage,EBadRequest);
        return;
        
      }

 	if (bCompleteRequest)
 	{
 	    CompleteRequest(lError);
	}
	
	FLOG(_L("CTneSession::DispatchMessageL out type=%d"), aMessage.Function());
}



TInt CTneSession::DoOpenFile(RFile &aFileHandle, TUint uStartTime)
{
      FLOG(_L(" CTneSession::DoOpenFile in"));
        
   MHXThumbnailUtilityImplObserver*	pObserver = 
				(MHXThumbnailUtilityImplObserver *) this;
	
	ibOpenFilePending = ETrue;	
	TFileName aFilename;
	aFileHandle.FullName(aFilename);	
	 FLOG(_L("aFilename=%S"),&aFilename ); 	
	TRAPD(err, iUtil = CHXThumbnailUtility::NewL(*pObserver));
	   m_bDone = EFalse;
 	   m_uPacketsReceived = 0;
	if (err == KErrNone)
	{
        TRAP(err, iUtil->OpenFileL(aFileHandle, uStartTime));
    }
       FLOG(_L(" CTneSession::DoOpenFile  out err = %d"),err);
    ibOpenFilePending = EFalse;
    return err;
}

TInt CTneSession::ReOpenFile(RFile &aFileHandle)
{
	iGetThumbPending = ETrue; 
	TInt lError = KErrNone;
    TUint uStartingTime = 0;
    lError = GetStartingTime(uStartingTime);
    if (lError == KErrNone)
    {       
    	if (iUtil)
    	{	
    	    iUtil->CancelThumb();
    		delete iUtil;
    		iUtil = NULL;    						    
    	}
    	    		   
    	lError = DoOpenFile(aFileHandle, uStartingTime);    	    	    	
    }   
    
    return lError; 	
}



void CTneSession::FetchBasicMetaData()
{
    TUint lCount = 0;
    TUint i=0;
    TUint lFrameRateinSec  = 0;
    
    iUtil->GetMetaDataCount(lCount);
    
    TBool bGotFrameSize = EFalse, bGotDuration = EFalse, bGotFrameRate = EFalse;
    
    for (i=0; i<lCount; i++)
    {
        HBufC *pDes = NULL;           
        HXMetaDataKeys::EHXMetaDataId id;        
        iUtil->GetMetaDataAt(i, id, pDes);
        
        if (id == HXMetaDataKeys::EHXFrameSize && pDes)
        {
            TPtr pFrameSizePtr = pDes->Des();
            _LIT(KChar_x, "x");
            TInt xLoc = pFrameSizePtr.Find(KChar_x);
            if (xLoc != KErrNotFound)
            {            	            
            TLex lexWidth(pFrameSizePtr.Mid(0, xLoc));
            TLex lexHeight(pFrameSizePtr.Mid(xLoc+1));
            lexWidth.Val(iWidth);  // Storing into local iWidth variable
            lexHeight.Val(iHeight);  // Storing into local iHeight variable  
            bGotFrameSize = ETrue;
            FLOG(_L(" iWidth=%d,iHeight=%d"), iWidth,iHeight);
            }
        }
        else if (id == HXMetaDataKeys::EHXDuration && pDes)
        {
            TLex lexDuration(pDes->Des());
            lexDuration.Val(iDuration);
            bGotDuration = ETrue;
        }        
        else if (id == HXMetaDataKeys::EHXFramesPerSecond && pDes)
        {
        	TLex lexFramesPerSecond(pDes->Des());        	
            lexFramesPerSecond.Val(lFrameRateinSec);            
        	bGotFrameRate = ETrue;
        }
        
        if (bGotDuration && bGotFrameRate && bGotFrameSize)
        {        	
        	break;	
        }
        
    } // end of for
    
    if (bGotDuration && bGotFrameRate)
    {
    	// approximate frame count
    	// iDuration is in msec.
    	iFrameCount =   ( (iDuration+500)/1000 ) * lFrameRateinSec;
    }
    
    else
    {
        iFrameCount =   ( (iDuration+500)/1000 ) * 10;
    }

    
}


void CTneSession::DoGetThumb()
{
    iGetThumbPending = ETrue;
    if(m_bDone)
	{						
        if (m_LastError != KErrNone)
        {
            if (iYUVBuffer)
            {
                User::Free(iYUVBuffer);
		    	iYUVBuffer = NULL;
            }
        }
                                
		NotifyIfGetThumbPending(m_LastError, iYUVBuffer);
	}
}

void CTneSession::MetaDataReady(TInt aError)
{

    FLOG(_L("CTneSession::MetaDataReady in callbacks from Utility aError=%d"), aError);
    if ( m_State == EStartGettingMetadata)
    {    	    	   	    	    	 
	    // extract some minimum metadata to be used for thumbnail generation
	    if (aError == KErrNone)
	    {	    		    
		    FetchBasicMetaData();		    
		    if(iYUVBuffer)
		    {    	    
		    	User::Free(iYUVBuffer);
		    	iYUVBuffer = NULL;
			}
					
		    // Allocate memory for iYUVBuffer    
		    if( iWidth > 4 && iHeight > 4 ) // for all frame sizes 
		    {	    		   
			    TInt length = iWidth * iHeight;
			    length += (length>>1);    
			    iYUVBuffer = (TUint8*)User::Alloc(length);
			        
			    if (!iYUVBuffer)
			    {
			        FLOG(_L("CTneSession::MetaDataReady Error No memory for iYUVBuffer")); 
			    	aError = KErrNoMemory;    	
			    }
		    }
		    else
		    {
		    	FLOG(_L("CTneSession::MetaDataReady Error Width/Height not found"));                	
		    	aError = KErrNotSupported;	
		    	
		    }
    	}
	    m_LastError = aError;
	    
	    // only complete the request if there is any error. If the request is not completed here then 
	    // it will be completed after the DoOpenFile() returns.	 
	    if (aError != KErrNone)
	    {
	     	CompleteRequest(aError);
	    }
	   
    }
    FLOG(_L("CTneSession::MetaDataReady out aError=%d"), aError);      
    m_bMetaDataReady = ETrue;          	

}


void CTneSession::PacketReady(TInt aError, 
                void *pData, TUint32 aDataSize)
{	
    FLOG(_L("CTneSession::PacketReady aError=%d m_State=%d aDataSize=%d"), aError, m_State, aDataSize);
    if(aDataSize > (iWidth*iHeight*3/2 ))
    {
	aDataSize  = iWidth*iHeight*3/2 ;  //restore back to values coming from header if they differ 
    	
      }
    if (m_State == ECancelling)
    {
        FLOG(_L("CTneSession::PacketReady no op"));
        return;
    }
    if(aDataSize < (iWidth*iHeight*3/2 ))  // check to avoid getting very low size
    {
    	if(m_uPacketsReceived == 0)
    	{
    		m_LastError = KErrNotSupported ;  // if this is the first packet then only send KErrNotSupported		
    	}
    	
    	m_bDone = ETrue;
    }
    
    if (m_LastError != KErrNone || aError != KErrNone || pData == NULL) 
    {
        FLOG(_L("CTneSession::PacketReady aError=%d m_State=%d pData=%x"), aError, m_State, pData);
        // MetadataReady might set m_LastError                    
		m_bDone = ETrue;
		
		if (m_LastError == KErrNone)
		{
		    if (aError == KErrNone)
		    {
		        m_LastError = KErrNotSupported;
		    }
		    else
		    {
		        m_LastError = aError;
		    }
		}				
	}
	else			
	{		
		m_uPacketsReceived++;
		if ( IsGoodFrame((TUint8*)pData )  && !m_bDone )
		{
			m_bDone = ETrue;
			m_LastError = KErrNone;	
	    	Mem::Copy(iYUVBuffer, pData, aDataSize);	
		}
		else if (m_uPacketsReceived == 1) // we copy the first packet
		{
		    Mem::Copy(iYUVBuffer, pData, aDataSize);
		}
		else if (m_uPacketsReceived >= KMaxPacketToDecode)
		{		
			// we decoded upto KMaxPacketToDecode and haven't found
			// a good frame yet.
			FLOG(_L("CHXTNEVideoClipInfoImp::PacketReady max count reached %d"), 
					m_uPacketsReceived);
					Mem::Copy(iYUVBuffer, pData, aDataSize);	// we  copy and use the last frame 
					m_bDone = ETrue;
					m_LastError = KErrNone;
		}
	}
    
	
	// we are done. Tell the TNUtil that no more packets are needed.
	if (m_bDone)
	{
		iUtil->CancelThumb();
	}
	
	if (iGetThumbPending)
	{		
				
		// we are done either success or failure.
		if(m_bDone)
		{						
            if (m_LastError != KErrNone)
            {
                if (iYUVBuffer)
                {
                    User::Free(iYUVBuffer);
		    	    iYUVBuffer = NULL;
                }
            }
                                    
			NotifyIfGetThumbPending(m_LastError, iYUVBuffer);
		}
	}
	
	
}

void CTneSession::EndOfPackets()
{	
	if (!m_bDone)
		{	 
			if(m_uPacketsReceived >= 1)
				{
	 				 NotifyIfGetThumbPending(KErrNone, iYUVBuffer);
				 }   
			else
				{
				  	TUint8*     pYUVBuffer = NULL;
  					NotifyIfGetThumbPending(KErrNotFound, pYUVBuffer);
				}
			// If end of packet has been received then thumbnailengine should be done
			m_bDone = ETrue;
		}        

	if(!m_bMetaDataReady)
        {
            MetaDataReady(KErrCompletion);
        }
}

TBool CTneSession::IsGoodFrame(TUint8* aYUVDataPtr)
{
	
 	TInt i;
    TInt minValue = 255;
    TInt maxValue = 0;
    TBool goodFrame = ETrue;
    TInt runningSum=0;
    TInt averageValue=0;
    TInt pixelSkips = 4;
    TInt numberOfSamples=0;
    TInt minMaxDeltaThreshold = 20; 
    TInt extremeRegionThreshold = 20; 
    TInt ySize = iWidth*iHeight; 
    
    // gather image statistics
    for(i=0, numberOfSamples=0; i<ySize; i+=pixelSkips, aYUVDataPtr+=pixelSkips, numberOfSamples++)
    {
        
        
        runningSum += *aYUVDataPtr;
        if(*aYUVDataPtr > maxValue)
            maxValue = *aYUVDataPtr;
        if(*aYUVDataPtr < minValue)
            minValue = *aYUVDataPtr;
    }
    //VDASSERT(numberOfSamples,10);
    if (numberOfSamples == 0)
    {
        FLOG(_L("CTneSession::IsGoodFrame numberOfSamples is zero")); 
    }
    else 
    {
        averageValue = runningSum/numberOfSamples;
    }
    
    // make decision based statistics
    if((maxValue - minValue) < minMaxDeltaThreshold)
        goodFrame = EFalse;
    else 
    {
        if(averageValue < (minValue + extremeRegionThreshold) || 
            averageValue > (maxValue - extremeRegionThreshold))
            goodFrame = EFalse;
    }
    return goodFrame;
}

TInt CTneSession::GetStartingTime(TUint &uStartingTime)
{	
	TInt err = KErrNone;
	
	if (iThumbIndex <= iFrameCount)
	{
		TReal tempDuration = iDuration;
		uStartingTime = (tempDuration/iFrameCount) * iThumbIndex;
		// thumbnail beyond the clip duration.
		if (uStartingTime >= iDuration)
		{
			uStartingTime = 0;
		}
	}
	else
	{
		err = KErrNotSupported;		
	}
	FLOG(_L("CTNEVideoClipInfo::GetStartingTime StartingTime=%d iDuration=%d iFrameCount=l:%d h:%d iThumbIndex=%d err=%d"), 
		 uStartingTime, iDuration, I64LOW(iFrameCount),I64HIGH(iFrameCount), iThumbIndex, err);
	return err;
}



void CTneSession::CompleteRequest(TInt aError)
{
     if(!iClientRequest.IsNull())
     {
        iClientRequest.Complete(aError);
     }
}




// ownership of pBitMap will be passed to Observer
void CTneSession::NotifyIfGetThumbPending(TInt aError, TUint8 *&pYUVBuffer) 
{
	if (iGetThumbPending && !iClientRequest.IsNull())
	{		
		iGetThumbPending = EFalse;        
        *iClientYUVBufferPtrPtr = pYUVBuffer;
        iClientRequest.Complete(aError);
    
	}	
}

void CTneSession::CompleteCancelRequest()
{
    
     if (!iCancelRequest.IsNull())
     {
      
        if (iUtil)
        {
            delete iUtil;
            iUtil = NULL;
          }
            iCancelRequest.Complete(KErrNone);
     }    
}
