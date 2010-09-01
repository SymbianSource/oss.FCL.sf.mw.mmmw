/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Progressive Download Utility
*
*/

#include <bautils.h>
#include <utf.h>
#include <mmfpaniccodes.h>
#include "EMCPdPlayUtility.h"
#include "EMCPdProperties.h"
#include "MProgressiveDownloadUtility.h"
#include <apgcli.h>
#include <AudioPreference.h>
#include <SCCustomInterfaceUIDs.h>
#include <drmagents.h>
#include <DRMConfigIntfc.h>
#include <content.h>
#include <data.h>
#include <MetaDataFieldContainer.h>
#include <MetaDataUtility.h>
#include <mmfmeta.h>

_LIT8(KWMAMimeType,"audio/x-ms-wma");

using namespace ContentAccess;
using namespace DRM;

#ifdef _DEBUG
#define DEBPRN1(str)        RDebug::Print(str);
#define DEBPRN2(str, val1)   RDebug::Print(str, val1);
#define DEBPRN3(str, val1, val2)   RDebug::Print(str, val1, val2);
#define DEBPRN4(str, val1, val2, val3)   RDebug::Print(str, val1, val2, val3);
#define DEBPRN5(str, val1, val2, val3, val4)   RDebug::Print(str, val1, val2, val3, val4);
#else
#define DEBPRN1(str)
#define DEBPRN2(str, val1)
#define DEBPRN3(str, val1, val2)
#define DEBPRN4(str, val1, val2, val3)
#define DEBPRN5(str, val1, val2, val3, val4)
#endif // _DEBUG

const TInt KNumberOfBuffers = 10;
//const TUid KUidProgressiveDlSource	= {KProgressiveDownloadSourceUid};
const TInt KBufferingAmount = 40960;

void Panic(TInt aPanicCode)
	{
	_LIT(KProgressiveDownloadUtility, "ProgressiveDownloadUtility");
	User::Panic(KProgressiveDownloadUtility, aPanicCode);
	}

CAudioPdPlayUtility* CAudioPdPlayUtility::NewL(MAudioPdPlayUtilityCallback& aCallback)
	{
    CAudioPdPlayUtility* self = new(ELeave) CAudioPdPlayUtility();
	CleanupStack::PushL(self);

 	self->iProperties = CAudioPdProperties::NewL(aCallback);
	CleanupStack::Pop(self);
	return self;
	}

CAudioPdPlayUtility::~CAudioPdPlayUtility()
	{
	delete iProperties;
	}

CAudioPdPlayUtility::CAudioPdPlayUtility()
	{

	}

void CAudioPdPlayUtility::ConstructL()
	{

	}

//For Download Status
void CAudioPdPlayUtility::HandleDownloadEventL(TUint aTransactionID,
                                               TBrCtlDownloadEvent aEvent,
                                               TUint aValue)
	{
    iProperties->HandleDownloadEventL(aTransactionID,aEvent,aValue);
	}


void CAudioPdPlayUtility::Play()
	{
    iProperties->Play();
	}
void CAudioPdPlayUtility::Stop()
	{
    iProperties->Stop();
	}
TInt CAudioPdPlayUtility::Pause()
	{
    return iProperties->Pause();
	}
void CAudioPdPlayUtility::SetVolume(TInt aVolume)
	{
    iProperties->SetVolume(aVolume);
	}
void CAudioPdPlayUtility::SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence)
	{
    iProperties->SetRepeats(aRepeatNumberOfTimes,aTrailingSilence);
	}
void CAudioPdPlayUtility::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)
	{
    iProperties->SetVolumeRamp(aRampDuration);
	}
const TTimeIntervalMicroSeconds& CAudioPdPlayUtility::Duration()
	{
	return iProperties->Duration();
	}
TInt CAudioPdPlayUtility:: MaxVolume()
	{
	return iProperties->MaxVolume();
	}
void CAudioPdPlayUtility::Close()
	{
    iProperties->Close();
	}
TInt CAudioPdPlayUtility::GetPosition(TTimeIntervalMicroSeconds& aPosition)
	{
    return iProperties->GetPosition(aPosition);
	}
void CAudioPdPlayUtility::SetPosition(const TTimeIntervalMicroSeconds& aPosition)
	{
    iProperties->SetPosition(aPosition);
	}

TInt CAudioPdPlayUtility::SetPriority(TInt aPriority, TMdaPriorityPreference aPref)
	{
    return iProperties->SetPriority(aPriority,aPref);
	}
TInt CAudioPdPlayUtility::GetVolume(TInt& aVolume)
	{
    return iProperties->GetVolume(aVolume);
	}
TInt CAudioPdPlayUtility::GetNumberOfMetaDataEntries(TInt& aNumEntries)
	{
    return iProperties->GetNumberOfMetaDataEntries(aNumEntries);
	}


CMMFMetaDataEntry* CAudioPdPlayUtility::GetMetaDataEntryL(TInt aMetaDataIndex)
	{
	return iProperties->GetMetaDataEntryL(aMetaDataIndex);
	}

TInt CAudioPdPlayUtility::SetPlayWindow(const TTimeIntervalMicroSeconds& aStart,
								const TTimeIntervalMicroSeconds& aEnd)
	{
    return iProperties->SetPlayWindow(aStart,aEnd);
	}

TInt CAudioPdPlayUtility::ClearPlayWindow()
	{
    return iProperties->ClearPlayWindow();
	}
TInt CAudioPdPlayUtility::SetBalance(TInt aBalance)
	{
    return iProperties->SetBalance(aBalance);
	}
TInt CAudioPdPlayUtility::GetBalance(TInt& aBalance)
	{
    return iProperties->GetBalance(aBalance);
	}
TInt CAudioPdPlayUtility::GetBitRate(TUint& aBitRate)
	{
    return iProperties->GetBitRate(aBitRate);
	}


void CAudioPdPlayUtility::RegisterForAudioLoadingNotification(MAudioLoadingObserver& aCallback)
	{
    iProperties->RegisterForAudioLoadingNotification(aCallback);
	}

void CAudioPdPlayUtility::GetAudioLoadingProgressL(TInt& aPercentageProgress)
	{
    iProperties->GetAudioLoadingProgressL(aPercentageProgress);
	}

const CMMFControllerImplementationInformation& CAudioPdPlayUtility::ControllerImplementationInformationL()
	{
	return iProperties->ControllerImplementationInformationL();
	}


TInt CAudioPdPlayUtility::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom)
	{
    return iProperties->CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
	}
TInt CAudioPdPlayUtility::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2)
	{
    return iProperties->CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
	}
void CAudioPdPlayUtility::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus)
	{
	iProperties->CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
	}
void CAudioPdPlayUtility::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus)
	{
	iProperties->CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
	}

void CAudioPdPlayUtility::OpenFileL(const TDesC& aFileName)
	{
	iProperties->OpenFileL(aFileName);
	}


void CAudioPdPlayUtility::OpenFileL(const RFile& aFile)
	{
	RFile& file = const_cast<RFile&>(aFile);
	iProperties->OpenFileL(file);
	}

void CAudioPdPlayUtility::DlCompleteOpenFileL(const TDesC& aFileName)
	{
	iProperties->DlCompleteOpenFileL(aFileName);
	}

TInt CAudioPdPlayUtility::GetFilePosition(TInt& aFilePosition)
	{
	return iProperties->GetFilePosition(aFilePosition);
	}

TInt CAudioPdPlayUtility::SetFileSize(TInt aFileSize)
	{
	return iProperties->SetFileSize(aFileSize);
	}

TInt CAudioPdPlayUtility::SetBytesDownloaded(TInt aBytesDownloaded, TBool aDownloadComplete)
	{
	return iProperties->SetBytesDownloaded(aBytesDownloaded,aDownloadComplete);
	}

MMMFDRMCustomCommand* CAudioPdPlayUtility::GetDRMCustomCommand()
	{
	//ASSERT(iProperties);
	return iProperties->GetDRMCustomCommand();
	}


TInt CAudioPdPlayUtility::CheckAudioPlayerState()
	{
	return iProperties->CheckAudioPlayerState();
	}


//===============================================================
// CAudioPdProperties implementation begins
//
//===============================================================
CAudioPdProperties* CAudioPdProperties::NewL(MAudioPdPlayUtilityCallback& aCallback)
	{
	CAudioPdProperties* self = new(ELeave) CAudioPdProperties(aCallback);
	CleanupStack::PushL(self);
	self->ConstructL(aCallback);
	CleanupStack::Pop(self);
	return self;
	}
CAudioPdProperties* CAudioPdProperties::NewLC(MAudioPdPlayUtilityCallback& /*aCallback*/)
	{
	return NULL;
	}

CAudioPdProperties::~CAudioPdProperties()
	{

    if(iMimeType)
        {
        delete iMimeType;
        iMimeType = NULL;
        }

    if(iBufferEmptiedEventAO->IsActive())
        {
        iBufferEmptiedEventAO->Cancel();
        }

    delete iBufferEmptiedEventAO;

    iFactory->DeleteStreamControl(iStreamControl);

    while(iBuffers.Count())
        {
        MDataBuffer* temp = iBuffers[0];
        iBuffers.Remove(0);
        iFactory->DeleteDataBuffer(temp);
        }

    if(iMDataBufferSource)
        {
        MSourceControl* objPtr = iMDataBufferSource;
        iFactory->DeleteSourceControl(objPtr);
        iMDataBufferSource = NULL;
        }

    MSinkControl* objPtr1 = iMAudioSink;
    iFactory->DeleteSinkControl(objPtr1);

	if(iMVolumeControl)
		{
		MEffectControl* effectObj = iMVolumeControl;
		iFactory->DeleteEffectControl(effectObj);
		iMVolumeControl = NULL;
		}

    if(iFileName)
        {
        delete iFileName;
        iFileName = NULL;
        }

    if (iMetaDataEntries.Count())
    	{
        iMetaDataEntries.ResetAndDestroy();
		iMetaDataEntries.Close();
		}

    delete iFile;
    iFile = NULL;
    iFs.Close();
    iBuffers.ResetAndDestroy();
    iBuffers.Close();
    iAvailable.Close();

 	delete iFactory;

	delete iControllerImplementationInformation;
	delete iAsyncCallBack;
//	iController.Close();
	}

CAudioPdProperties::CAudioPdProperties(/*MMdaAudioPlayerCallback*/MAudioPdPlayUtilityCallback& aCallback):
	iCallback(aCallback),
	iAudioPlayDeviceCommands(iController),
	iAudioPlayControllerCommands(iController),
	iDRMCustomCommands(iController)
	{
	iState = EStopped;
	iPrioritySettings.iPriority = KAudioPriorityRealOnePlayer;//80
	iPrioritySettings.iPref =(TMdaPriorityPreference) KAudioPrefRealOneLocalPlayback;// 0x01420001;
	iPlayStart = TTimeIntervalMicroSeconds(0);
	iPlayEnd = TTimeIntervalMicroSeconds(0);
	iPlayWindowSet = ENone;
	iBuffering = EFalse;
	iControllerPtr = NULL;
	iStopCalled = EFalse;
	iFileName = NULL;
	}

void CAudioPdProperties::ConstructL(MAudioPdPlayUtilityCallback& /*aCallback*/)
	{
	iAsyncCallBack = CMMFMdaAudioPlayerCallBack::NewL(iCallback);
    iBufferEmptiedEventAO = CBufferEmptiedEventAO::NewL(*this);
    iMimeType = HBufC8::NewL(20);
    TInt status = CMultimediaFactory::CreateFactory( iFactory );

    TInt err = iFactory->CreateStreamControl( KStreamControl, iStreamControl );
    User::LeaveIfError(err);
    iStreamControl->AddObserver( *this );

    MSinkControl* tempSinkCtrl(NULL);
    err = iFactory->CreateSinkControl( KMMFAudioOutputSinkControl, tempSinkCtrl );
    User::LeaveIfError(err);
	iMAudioSink = tempSinkCtrl;
    iStreamControl->AddSink( *iMAudioSink );

    MSourceControl* tempCtrl(NULL);
    err = iFactory->CreateSourceControl( KDataBufferSourceControl, tempCtrl );
    User::LeaveIfError(err);
    CreateAudioBuffersL();
    iMDataBufferSource = static_cast<MDataBufferSource*>(tempCtrl);
    iMDataBufferSource->AddObserver( *this );

	//Create Volume Control
	MEffectControl* effectCtrl(NULL);
	err = iFactory->CreateEffectControl( KVolumeEffectControl, effectCtrl );
	User::LeaveIfError(err);

	iMVolumeControl = static_cast<MVolumeControl*>(effectCtrl);
	err = iStreamControl->AddEffect( *iMVolumeControl );
	}

void CAudioPdProperties::HandleDownloadEventL(TUint aTransactionID,
                                              TBrCtlDownloadEvent aEvent,
                                              TUint aValue)
	{
	iCallback.HandleDownloadEventL(aTransactionID,aEvent,aValue);
	}


void CAudioPdProperties::CreateAudioBuffersL()
    {
    MDataBuffer* buffer;
    iBuffers.ResetAndDestroy();
    iAvailable.Reset();
    for (int i = 0; i < KNumberOfBuffers; i++ )
        {
        iFactory->CreateDataBuffer(KDataBufferSourceControl,4096,buffer);
        User::LeaveIfError(iBuffers.Append(buffer));
        buffer = NULL;
        iAvailable.AppendL(ETrue);
        }
    }

void CAudioPdProperties::DetermineFNMimeType(const TDesC& aFileName, TDes8& aMimeType)
    {
    RApaLsSession ls;
    TInt err;
    err=ls.Connect();

    TDataRecognitionResult result;
    err = ls.RecognizeData(aFileName,KNullDesC8(),result);
    if(!err && (result.iConfidence >= CApaDataRecognizerType::EProbable))
        {
        aMimeType.Copy(result.iDataType.Des8());
        }
    ls.Close();
    }

void CAudioPdProperties::DetermineFHMimeType(const RFile& aFile, TDes8& aMimeType)
    {
    RApaLsSession ls;
    TInt err;
    err=ls.Connect();

    TDataRecognitionResult result;
    err = ls.RecognizeData(aFile,result);
    if(!err && (result.iConfidence >= CApaDataRecognizerType::EProbable))
        {
        aMimeType.Copy(result.iDataType.Des8());
        }
    ls.Close();
    }


void CAudioPdProperties::OpenFileL(const TDesC& aFileName)
	{
    TBuf8<20> mimeType;
    User::LeaveIfError(iFs.Connect());

    if(iFileName)
        {
        delete iFileName;
        iFileName = NULL;
        }

    iFileName = HBufC::NewL(aFileName.Length());
    TPtr des = iFileName->Des();
    des.Copy(aFileName);

	TInt err = ExtractDRMRestrictionsL();
	User::LeaveIfError(err);
    if(!isProtected)
        {
        DetermineFNMimeType(aFileName,mimeType);
        iMimeType->Des().Copy(mimeType);
        }


    if(iMimeType->Des().Length())
        {
        DEBPRN1(_L("CAudioPdProperties::OpenFileL() Before iFile Open "));
    	iFile =  CContentFile::NewL(iFs,aFileName, KNullDesC, EFileShareAny,EFalse);
        DEBPRN2(_L("CAudioPdProperties::OpenFileL() After iFile Open Error[%d]"),err);
        iStreamControl->AddSource(*iMDataBufferSource);
    	MDataBuffer* dataBuffer = NULL;
    	err = iMDataBufferSource->Open(*iMimeType,*dataBuffer);
    	User::LeaveIfError(err);
        SetPriority(iPrioritySettings.iPriority,(TMdaPriorityPreference)iPrioritySettings.iPref);
    	iStreamControl->Open();
        }
    else
        {
        User::Leave(KErrNotSupported);
        }
	}


void CAudioPdProperties::OpenFileL(const RFile& aFile)
	{
    TBuf8<20> mimeType;
    iFileHandle = aFile;
	TInt err = ExtractDRMRestrictionsL();
	User::LeaveIfError(err);
    if(!isProtected)
        {
        DetermineFHMimeType(iFileHandle,mimeType);
        iMimeType->Des().Copy(mimeType);
        }

    iFile =  CContentFile::NewL(iFileHandle, KNullDesC ,EFalse);

    if(iMimeType->Des().Length())
        {
        MDataBuffer* dataBuffer = NULL;
    	TInt err = iMDataBufferSource->Open(*iMimeType,*dataBuffer);
    	User::LeaveIfError(err);
        SetPriority(iPrioritySettings.iPriority,(TMdaPriorityPreference)iPrioritySettings.iPref);
    	iStreamControl->Open();
        }
    else
        {
        User::Leave(KErrNotSupported);
        }
	}

void CAudioPdProperties::DlCompleteOpenFileL(const TDesC& aFileName)
	{
	OpenFileL(aFileName);
	}


void CAudioPdProperties::Play()
	{
    DEBPRN2(_L("CAudioPdProperties::Play() enter iIsEOFReached[%d]"),iIsEOFReached);
	if (iPlayWindowSet == ESet)
		{
        iAudioPlayControllerCommands.SetPlaybackWindow(iPlayStart, iPlayEnd);
		}
	else if (iPlayWindowSet == EClear)
		{
		iAudioPlayControllerCommands.DeletePlaybackWindow();
		iPlayWindowSet = ENone;	// assume window will stay cleared
		}

    if(iIsEOFReached)
        {
        delete iFile;
        iFile = NULL;
        if(iFileName)
            {
            DEBPRN2(_L("CAudioPdProperties::Play() iIsEOFReached[%d] "),iIsEOFReached);
           	iFile =  CContentFile::NewL(iFs,*iFileName, KNullDesC, EFileShareAny,EFalse);
            }
        else
            {
            DEBPRN2(_L("CAudioPdProperties::Play() iIsEOFReached[%d] "),iIsEOFReached);
            iFile =  CContentFile::NewL(iFileHandle, KNullDesC ,EFalse);
            }
        iIsEOFReached = EFalse;
        }

    if(iState != EStopped)
        {
        FillSourceBuffers();
        }

    if(!iDisableAutoIntent && iFile)
        {
        iFile->ExecuteIntent((iState == EPaused) ? ContentAccess::EContinue : ContentAccess::EPlay);
        }

    iStreamControl->Start();
	}

void CAudioPdProperties::Stop()
	{
    DEBPRN1(_L("CAudioPdProperties::Stop() enter"));
    iStopCalled = ETrue;
    iStreamControl->Stop();
	}
TInt CAudioPdProperties::Pause()
	{
    TInt err = KErrNone;
    DEBPRN1(_L("CAudioPdProperties::Pause() enter"));
    if(!iDisableAutoIntent && iFile)
        {
        iFile->ExecuteIntent(ContentAccess::EPause);
        }

    err = iStreamControl->Pause();
	return err;
	}

void CAudioPdProperties::SetVolume(TInt aVolume)
	{
    if(iMVolumeControl)
	    {
	    iMVolumeControl->SetVolume(aVolume);
	    iMVolumeControl->Apply();
        }
	}

void CAudioPdProperties::SetRepeats(TInt aRepeatNumberOfTimes, const TTimeIntervalMicroSeconds& aTrailingSilence)
	{
	iNumberOfTimesToRepeat = aRepeatNumberOfTimes;
	iTrailingSilence = aTrailingSilence;
	}

void CAudioPdProperties::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)
	{
   	TInt curVol = 0;
   	MVolumeControl::TVolumeRampMode aMode= MVolumeControl::EIMMEDIATE;
   	TUint64 duration = aRampDuration.Int64();
   	TInt err = iMVolumeControl->GetVolume(curVol);
    if(iMVolumeControl)
    	{
     	iMVolumeControl->SetVolumeRamp(0,curVol,duration,
                                   aMode);
		iMVolumeControl->Apply();
    	}
	}
const TTimeIntervalMicroSeconds& CAudioPdProperties::Duration()
	{
	TInt64 duration = 0;
	TInt err = iStreamControl->GetDuration(duration);
	iDuration = duration;
	return iDuration;
	}

TInt CAudioPdProperties:: MaxVolume()
	{
    TInt volume;
    if(iMVolumeControl)
    	iMVolumeControl->GetMaxVolume(volume);
    return volume;
	}

void CAudioPdProperties::Close()
	{
	// Reset the audio player state.
    DEBPRN1(_L("CAudioPdProperties::Close() enter"));
	Stop();
    DEBPRN1(_L("CAudioPdProperties::Close() Before File Close "));
	delete iFile;
	iFile = NULL;
    DEBPRN1(_L("CAudioPdProperties::Close() After File Close "));
	}

TInt CAudioPdProperties::GetPosition(TTimeIntervalMicroSeconds& aPosition)
	{
    TInt error = KErrNone;
    TInt64 position = 0;
	error = iStreamControl->GetPosition(position);
	aPosition = iPosition = position;
	return error;
	}

void CAudioPdProperties::SetPosition(const TTimeIntervalMicroSeconds& aPosition)
	{
	TInt64 position = aPosition.Int64();
	TTimeIntervalMicroSeconds currentPosition(0);
	GetPosition(currentPosition);

	// We only set the Position if the Current Position is different then the
    // the new Position
	if(currentPosition != aPosition)
	    {
	    //iStreamControl->SetPosition(position);
	    }
	}

TInt CAudioPdProperties::SetPriority(TInt aPriority, TMdaPriorityPreference aPref)
	{
	TInt err = iStreamControl->SetPriority(aPriority,aPref);
	return err;
	}

TInt CAudioPdProperties::GetVolume(TInt& aVolume)
	{
    TInt error = iAudioPlayDeviceCommands.GetVolume(aVolume);
	return error;
	}

TInt CAudioPdProperties::GetNumberOfMetaDataEntries(TInt& aNumEntries)
	{
    TInt status(KErrNotReady);
    if(!iMimeType->Des().Compare(KWMAMimeType()))
        {
        status = iController.GetNumberOfMetaDataEntries(aNumEntries);
        }
    else
        {
        if (!iMetaDataRead && iControllerLoaded)
            {
            CMetaDataUtility* metaDataUtility = CMetaDataUtility::NewL();
            CleanupStack::PushL(metaDataUtility);

    		RArray<TMetaDataFieldId> wantedFields;
    		CleanupClosePushL(wantedFields);
    		wantedFields.Append(EMetaDataSongTitle);
    		wantedFields.Append(EMetaDataArtist);
    		wantedFields.Append(EMetaDataAlbum);
    		wantedFields.Append(EMetaDataYear);
    		wantedFields.Append(EMetaDataComment);
    		wantedFields.Append(EMetaDataAlbumTrack);
    		wantedFields.Append(EMetaDataGenre);
    		wantedFields.Append(EMetaDataComposer);
    		wantedFields.Append(EMetaDataCopyright);
    		wantedFields.Append(EMetaDataOriginalArtist);
    		wantedFields.Append(EMetaDataUrl);
    		wantedFields.Append(EMetaDataJpeg);
    		wantedFields.Append(EMetaDataUserUrl);

			if(iFileName)
			    {
			    TRAP(status,metaDataUtility->OpenFileL(*iFileName, wantedFields));
			    }
			else
			    {
			    TRAP(status,metaDataUtility->OpenFileL(iFileHandle, wantedFields));
			    }

            if(status != KErrNone)
                {
        		CleanupStack::PopAndDestroy(&wantedFields);	// wantedFields
        		CleanupStack::PopAndDestroy(metaDataUtility);	//metaDataUtility
                return status;
                }
            TInt count = metaDataUtility->MetaDataCount();

            if (count > 0)
    			{
    			const CMetaDataFieldContainer& container = metaDataUtility->MetaDataFieldsL();
    			TMetaDataFieldId id;
    			for (TInt i = 0; i < count; i++)
    				{
    				TPtrC content = container.At(i, id);
    				CMMFMetaDataEntry* metaData = NULL;
    				switch (id)
    					{
    					case EMetaDataSongTitle:
    						metaData = CMMFMetaDataEntry::NewL(KMMFMetaEntrySongTitle, content);
    						break;
    					case EMetaDataArtist:
    						metaData = CMMFMetaDataEntry::NewL(KMMFMetaEntryArtist, content);
    						break;
    					case EMetaDataAlbum:
    						metaData = CMMFMetaDataEntry::NewL(KMMFMetaEntryAlbum, content);
    						break;
    					case EMetaDataYear:
    						metaData = CMMFMetaDataEntry::NewL(KMMFMetaEntryYear, content);
    						break;
    					case EMetaDataComment:
    						metaData = CMMFMetaDataEntry::NewL(KMMFMetaEntryComment, content);
    						break;
    					case EMetaDataAlbumTrack:
    						metaData = CMMFMetaDataEntry::NewL(KMMFMetaEntryAlbumTrack, content);
    						break;
    					case EMetaDataGenre:
    						metaData = CMMFMetaDataEntry::NewL(KMMFMetaEntryGenre, content);
    						break;
    					case EMetaDataComposer:
    						metaData = CMMFMetaDataEntry::NewL(KMMFMetaEntryComposer, content);
    						break;
    					case EMetaDataCopyright:
    						metaData = CMMFMetaDataEntry::NewL(KMMFMetaEntryCopyright, content);
    						break;
    					case EMetaDataOriginalArtist:
    						metaData = CMMFMetaDataEntry::NewL(KMMFMetaEntryOriginalArtist, content);
    						break;
    					case EMetaDataUrl:
    						metaData = CMMFMetaDataEntry::NewL(KMMFMetaEntryWOAF, content);
    						break;
    					case EMetaDataJpeg:
    						metaData = CMMFMetaDataEntry::NewL(KMMFMetaEntryAPIC, content);
    						break;
    					case EMetaDataUserUrl:
    						metaData = CMMFMetaDataEntry::NewL(KMMFMetaEntryWXXX, content);
    						break;
    					default:	// Should never get here really...
    						break;
    					}
    				if (metaData)
    					{
    					CleanupStack::PushL(metaData);
    					User::LeaveIfError(iMetaDataEntries.Append(metaData));
    					CleanupStack::Pop(metaData);	// metaData
    					}
    				}
    			}
    		iMetaDataRead = ETrue;
    		CleanupStack::PopAndDestroy(&wantedFields);	// wantedFields
    		CleanupStack::PopAndDestroy(metaDataUtility);	//metaDataUtility
    		}
        aNumEntries = iMetaDataEntries.Count();
        status = KErrNone;
        }
    return status;
	}


CMMFMetaDataEntry* CAudioPdProperties::GetMetaDataEntryL(TInt aMetaDataIndex)
	{
    if(!iMimeType->Des().Compare(KWMAMimeType()))
        {
        return iController.GetMetaDataEntryL(aMetaDataIndex);
        }
	else
        {
        if (aMetaDataIndex > iMetaDataEntries.Count() - 1)
        	{
            User::Leave(KErrArgument);
    		}

        return CMMFMetaDataEntry::NewL(*iMetaDataEntries[aMetaDataIndex]);
        }
	}

TInt CAudioPdProperties::SetPlayWindow(const TTimeIntervalMicroSeconds& aPlayStart,
								const TTimeIntervalMicroSeconds& aPlayEnd)
	{
    TInt error = KErrNone;

	if (aPlayStart >= TTimeIntervalMicroSeconds(0) &&
		aPlayStart < iDuration &&
			aPlayStart < aPlayEnd &&
			aPlayEnd <= iDuration )
		{
		iPlayStart = aPlayStart;
		iPlayEnd = aPlayEnd;
		iPlayWindowSet = ESet;

		if (iState==EPlaying)
			error = iAudioPlayControllerCommands.SetPlaybackWindow(aPlayStart, aPlayEnd);
		}
	else
		error = KErrArgument;

	return error;
	}

TInt CAudioPdProperties::ClearPlayWindow()
	{
    	// clear play window start - very important because this is assigned
	// to iPosition when we stop & is used to set the position on the next Play()
	iPosition = iPlayStart = iPlayEnd = TTimeIntervalMicroSeconds(0);

	iPlayWindowSet = EClear;
	TInt err = KErrNone;
	if (iState==EPlaying)
		err = iAudioPlayControllerCommands.DeletePlaybackWindow();
	return err;
	}

TInt CAudioPdProperties::SetBalance(TInt aBalance)
	{
    TInt err = iAudioPlayDeviceCommands.SetBalance(aBalance);
	return err;
	}

TInt CAudioPdProperties::GetBalance(TInt& aBalance)
	{
	TInt err = iAudioPlayDeviceCommands.GetBalance(aBalance);
	return err;
	}

TInt CAudioPdProperties::GetBitRate(TUint& aBitRate)
	{
    TInt status = KErrNone;
    if(iMDataBufferSource)
        {
        status = iMDataBufferSource->GetBitRate(aBitRate);
        }
    return status;
	}

void CAudioPdProperties::RegisterForAudioLoadingNotification(MAudioLoadingObserver& aLoadingObserver)
	{
    iLoadingObserver = &aLoadingObserver;
	}

void CAudioPdProperties::GetAudioLoadingProgressL(TInt& aPercentageProgress)
	{
    User::LeaveIfError(iAudioPlayControllerCommands.GetLoadingProgress(aPercentageProgress));
	}

const CMMFControllerImplementationInformation& CAudioPdProperties::ControllerImplementationInformationL()
	{
	if (!iControllerImplementationInformation)
		{
		TUid* contUid = static_cast<TUid*>(iStreamControl->CustomInterface(KUidSCControllerUid));
		iControllerUid = *contUid;
		if (iControllerUid==KNullUid)
			User::Leave(KErrNotReady);
		iControllerImplementationInformation = CMMFControllerImplementationInformation::NewL(iControllerUid);
		}
	return *iControllerImplementationInformation;
	}

TInt CAudioPdProperties::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom)
	{
    return iController.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom);
	}
TInt CAudioPdProperties::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2)
	{
    return iController.CustomCommandSync(aDestination, aFunction, aDataTo1, aDataTo2);
	}
void CAudioPdProperties::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus)
	{
	iController.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aDataFrom, aStatus);
	}
void CAudioPdProperties::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus)
	{
    iController.CustomCommandAsync(aDestination, aFunction, aDataTo1, aDataTo2, aStatus);
	}

TInt CAudioPdProperties::GetFilePosition(TInt& /*aFilePosition*/)
	{
    return KErrNone;
	}

TInt CAudioPdProperties::SetFileSize(TInt aFileSize)
	{
    TInt status(KErrNone);
    if(!isProtected)
        {
        iFileSize = aFileSize;
        status = iMDataBufferSource->SetSize(aFileSize);
        }
    return status;
	}

TInt CAudioPdProperties::SetBytesDownloaded(TInt aBytesDownloaded, TBool aDownloadComplete)
	{
    TInt status(KErrNone);
    iBytesDownloaded = aBytesDownloaded;
    iDownloadComplete = aDownloadComplete;

    if(iDownloadComplete && iFile)
        {
        ReOpenCAF();
        }

    if(iBuffering)
        {
        if(((aBytesDownloaded - iBytesReadFromFile) > KBufferingAmount)
            || ((iFileSize - aBytesDownloaded) < KBufferingAmount))
            {
            FillSourceBuffers();
            }
        }
    return status;
	}


MMMFDRMCustomCommand* CAudioPdProperties::GetDRMCustomCommand()
	{
    return this;
	}


TInt CAudioPdProperties::DisableAutomaticIntent(TBool aDisableAutoIntent)
    {
	if (iDRMCustomCommands.IsSupported())
		{
        iDisableAutoIntent = aDisableAutoIntent;
        return KErrNone;
		}
	else
		{
		return KErrNotSupported;
		}
    }

TInt CAudioPdProperties::ExecuteIntent(ContentAccess::TIntent aIntent)
    {
	if (iDRMCustomCommands.IsSupported())
		{
	    if(iFile)
	        {
	        return iFile->ExecuteIntent(aIntent);
	        }
        else
            {
            return KErrNotReady;
            }
		}
	else
		{
		return KErrNotSupported;
		}
    }

TInt CAudioPdProperties::EvaluateIntent(ContentAccess::TIntent aIntent)
    {
	if (iDRMCustomCommands.IsSupported())
		{
	    if(iFile)
	        {
            return iFile->EvaluateIntent(aIntent);
	        }
        else
            {
            return KErrNotReady;
            }
		}
	else
		{
		return KErrNotSupported;
		}
    }

TInt CAudioPdProperties::SetAgentProperty(ContentAccess::TAgentProperty aProperty, TInt aValue)
    {
	if (iDRMCustomCommands.IsSupported())
		{
	    if(iFile)
	        {
    		return iFile->SetAgentProperty(aProperty,aValue);
	        }
        else
            {
            return KErrNotReady;
            }
		}
	else
		{
		return KErrNotSupported;
		}
    }

TInt CAudioPdProperties::CheckAudioPlayerState()
	{
	return iState;
	}

CRepeatTrailingSilenceTimer* CRepeatTrailingSilenceTimer::NewL(MRepeatTrailingSilenceTimerObs& aObs)
	{
	CRepeatTrailingSilenceTimer* s = new(ELeave) CRepeatTrailingSilenceTimer(aObs);
	CleanupStack::PushL(s);
	s->ConstructL();
	CleanupStack::Pop();
	return s;
	}

void CRepeatTrailingSilenceTimer::RunL()
	{
	iObs.RepeatTrailingSilenceTimerComplete();
	}

CRepeatTrailingSilenceTimer::CRepeatTrailingSilenceTimer(MRepeatTrailingSilenceTimerObs& aObs) :
	CTimer(EPriorityHigh),
	iObs(aObs)
	{
	CActiveScheduler::Add(this);
	}


CMMFMdaAudioPlayerCallBack* CMMFMdaAudioPlayerCallBack::NewL(MAudioPdPlayUtilityCallback& aCallback)
	{
	return new(ELeave) CMMFMdaAudioPlayerCallBack(aCallback);
	}

CMMFMdaAudioPlayerCallBack::CMMFMdaAudioPlayerCallBack(MAudioPdPlayUtilityCallback& aCallback) :
	CActive(CActive::EPriorityHigh), iCallback(aCallback)
	{
	CActiveScheduler::Add(this);
	}

CMMFMdaAudioPlayerCallBack::~CMMFMdaAudioPlayerCallBack()
	{
	Cancel();
	}

void CMMFMdaAudioPlayerCallBack::InitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration)
	{
	iError = aError;
	iDuration = aDuration;
	iState = ECallbackInitComplete;
	if (!IsActive())
		{
		TRequestStatus* s = &iStatus;
		SetActive();
		User::RequestComplete(s, KErrNone);
		}
	}

void CMMFMdaAudioPlayerCallBack::PlayComplete(TInt aError)
	{
	iError = aError;
	iState = ECallbackPlayComplete;
	if (!IsActive())
		{
		TRequestStatus* s = &iStatus;
		SetActive();
		User::RequestComplete(s, KErrNone);
		}
	}

void CMMFMdaAudioPlayerCallBack::PlayingCallback()
	{
	iState = ECallbackPlaying;
	if (!IsActive())
		{
		TRequestStatus* s = &iStatus;
		SetActive();
		User::RequestComplete(s, KErrNone);
		}
	}

void CMMFMdaAudioPlayerCallBack::PausedCallback()
	{
	iState = ECallbackPaused;
	if (!IsActive())
		{
		TRequestStatus* s = &iStatus;
		SetActive();
		User::RequestComplete(s, KErrNone);
		}
	}


void CMMFMdaAudioPlayerCallBack::RunL()
	{
	switch (iState)
		{
		case ECallbackInitComplete:
			iCallback.MapcInitComplete(iError, iDuration);
			break;
		case ECallbackPlayComplete:
			iCallback.MapcPlayComplete(iError);
			break;
		case ECallbackPlaying:
			iCallback.Playing();
			break;
		case ECallbackPaused:
			iCallback.Paused();
			break;

		}
	}

void CMMFMdaAudioPlayerCallBack::DoCancel()
	{
	// Nothing to cancel
	}

void CAudioPdProperties::Event( MControl* /*aControl*/, TUint aEventType, TAny* aEventObject )
    {
    switch (aEventType)
        {
        case MStreamControlObserver::KStateChangedEvent:
            {
            //MStreamControl* control1 = (MStreamControl*)(aControl);
            //MErrorCode* evt = (MErrorCode*)aEventObject;
            MStateChangedEvent* evt = (MStateChangedEvent*)aEventObject;
            DEBPRN1(_L("CAudioPdProperties::Event:EStateChanged"));
            iStreamState = evt->GetState();
            switch(evt->GetState())
                {
                case MStreamControl::CLOSED:
                    iState = EStopped;
                    iControllerLoaded = EFalse;
                    DEBPRN1(_L("CAudioPdProperties::Event:EStateChanged[Closed]"));
                    break;
                case MStreamControl::INITIALIZED:
                    DEBPRN3(_L("CAudioPdProperties::Event:EStateChanged[Opened] [%d]StopCalled[%d]"),evt->GetErrorCode(),iStopCalled);
                    iState = EOpening;
                    if( evt->GetErrorCode() == KErrEof || iStopCalled)
                        {
                        iIsEOFReached = ETrue;
                        iBytesReadFromFile = 0;
                        DEBPRN1(_L("CAudioPdProperties::Event() Before File Close "));
                        delete iFile;
                        iFile = NULL;
                        DEBPRN1(_L("CAudioPdProperties::Event() After File Close "));
                        if(!iStopCalled)
                            {
                            iAsyncCallBack->PlayComplete(KErrNone);
                            }
                        iStopCalled = EFalse;
                        }
                    else if(evt->GetErrorCode() == KErrNone )
                        {

                        iControllerPtr = static_cast<RMMFController*> (iStreamControl->CustomInterface(KUidSCControllerRef));
                        iController = *iControllerPtr;
                        iControllerLoaded = ETrue;
                        if(iFileSize == -1 || isProtected)
                            {
                            iFile->Size(iFileSize);
                            iMDataBufferSource->SetSize(iFileSize);
                            }
                        else
                            {
                            SetFileSize(iFileSize);
                            }
                        FillSourceBuffers();
                        iStreamControl->Prime();
                        }
                    else
                        {
                        switch(evt->GetErrorCode())
                            {
                            case KErrNotSupported:
                            case KErrPermissionDenied:
                            case KErrCANoRights:
                                iAsyncCallBack->PlayComplete(evt->GetErrorCode());
                                iIsEOFReached = ETrue;
                                delete iFile;
                                iFile = NULL;
                                break;

                            case KErrDied:
                                Stop();
                                iAsyncCallBack->PlayComplete(evt->GetErrorCode());
                                iIsEOFReached = ETrue;
                                delete iFile;
                                iFile = NULL;
                                break;
                            }
                        }
                    break;
                case MStreamControl::PRIMED:
                    iState = EPrimed;
                    DEBPRN1(_L("CAudioPdProperties::Event:EStateChanged[Primed]"));
                    iAsyncCallBack->InitComplete(evt->GetErrorCode(), Duration());
                    break;
                case MStreamControl::EXECUTING:
                    DEBPRN1(_L("CAudioPdProperties::Event:EStateChanged[Playing]"));
                    iStopCalled = EFalse;
                    iState = EPlaying;
                    iAsyncCallBack->PlayingCallback();
                    break;
                case MStreamControl::BUFFERING:
                    DEBPRN1(_L("CAudioPdProperties::Event:EStateChanged[Buffering]"));
                    if(iBuffering)
                        {
                        iStreamControl->Pause();
                        //iAsyncCallBack->PlayComplete(KErrDied);
                        iAsyncCallBack->PlayComplete(KErrUnderflow);
                        }
                    iState = EBuffering;
                    break;
                case MStreamControl::PAUSED:
                    iState = EPaused;
                    iAsyncCallBack->PausedCallback();
                    DEBPRN2(_L("CAudioPdProperties::Event:EStateChanged[Paused] [%d]"),evt->GetErrorCode());
                    break;
                default:
                    break;
                };
            }
            break;


        case MSourceControlObserver::KBufferProcessedEvent:
            {
            MBufferProcessedEvent* evt = (MBufferProcessedEvent*)aEventObject;
            DEBPRN3(_L("CAudioPdProperties::Event:EBufferProcessed[AudioBuffer[0x%x]Reason[%d]]") , evt->GetDataBuffer(), evt->GetErrorCode() );
            TInt index = iBuffers.Find( evt->GetDataBuffer() );
            if ( index >= 0 )
                {
                iAvailable[index] = ETrue;
                }

            if(!iBufferEmptiedEventAO->IsActive() && !iIsEOFReached)
                {
                iBufferEmptiedEventAO->SetActive();
                iBufferEmptiedEventAO->SetBuffer(evt->GetDataBuffer());
                }

            if(iBufferEmptiedEventAO->IsActive())
                {
                TRequestStatus* status = &(iBufferEmptiedEventAO->iStatus);
                User::RequestComplete(status,evt->GetErrorCode());
                }
            }
            break;

        case MSourceControlObserver::KBitRateChangedEvent:
            {
            //TUint rate = 0;
            //DEBPRN2(_L("CAudioPdProperties::Event:BitRateChanged[%d]"),rate);
            }
            break;

        case MStreamControlObserver::KDurationChangedEvent:
            break;
        default:
            break;
        };
    }

TInt CAudioPdProperties::ReadFromFileAndWriteToStream(TInt aIndex)
    {
    MDataBuffer* buffer = iBuffers[aIndex];
    buffer->GetBufferPtr().FillZ();
    // Read data into CAudioBuffer
    TInt filePos = 0;
    iFile->Seek(ESeekCurrent,filePos);

    DEBPRN4(_L("CAudioPdProperties::ReadFromFileAndWriteToStream \
    BytesDL[%d] filePos[%d] MaxLength[%d]"),iBytesDownloaded,filePos, buffer->GetBufferPtr().MaxLength());
    TInt err1(KErrNone);
    if(iBytesDownloaded - filePos > buffer->GetBufferPtr().MaxLength() || iDownloadComplete)
        {
        err1 = iFile->Read( buffer->GetBufferPtr(),  buffer->GetBufferPtr().MaxLength());
        }
    else
        {
        iBuffering =  ETrue;
        return KErrUnderflow;
        }
    DEBPRN2(_L("CAudioPdProperties::ReadFromFileAndWriteToStream File Read Error [%d]"),err1);
    User::LeaveIfError(err1);
    iBytesReadFromFile += buffer->GetBufferPtr().Length();

    DEBPRN3(_L("CAudioPdProperties::ReadFromFileAndWriteToStream\
    bufferReadLen[%d] Buf Len[%d]"),buffer->GetBufferPtr().Length(),buffer->GetBufferPtr().MaxLength() );

    if (buffer->GetBufferPtr().Length() < buffer->GetBufferPtr().MaxLength() && (iBytesReadFromFile < iFileSize))
        {

        iBytesReadFromFile = iBytesReadFromFile - buffer->GetBufferPtr().Length();
        TInt seekOffset = -(buffer->GetBufferPtr().Length());
        iFile->Seek( ESeekCurrent, seekOffset );
        iBuffering =  ETrue;
        DEBPRN3(_L("CAudioPdProperties::ReadFromFileAndWriteToStream\
        BytesReadFromFile[%d] seekOffset[%d]"),iBytesReadFromFile,seekOffset );
        return KErrOverflow;
        }
    else if(buffer->GetBufferPtr().Length() < buffer->GetBufferPtr().MaxLength())
        {
        buffer->SetLastBuffer( ETrue );
        iIsEOFReached = ETrue;
        //iFile.Close();
        }
    else
        {
        buffer->SetLastBuffer( EFalse );
        }

    // Write data into iMDataBufferSource
    DEBPRN4(_L("CAudioPdProperties::ReadFromFileAndWriteToStream\
    [AudioBuffer[0x%x]FileSize[%d]BytesRead[%d]]") , buffer, iFileSize, iBytesReadFromFile );

    TInt err(KErrNone);
    err = iMDataBufferSource->WriteData( *buffer );

    if (err == KErrNone)
        {
        iAvailable[aIndex] = EFalse;
        }
    else if ( err == KErrOverflow )
        {
        // There isn't enough memory in the player to buffer the data.
        // reset the file pos
        TInt size = -(buffer->GetBufferPtr().Length());
        iFile->Seek( ESeekCurrent, size );
        iBytesReadFromFile -= size;
        }
    return err;
    }


void CAudioPdProperties::BufferEmptiedBySource()
    {
    TInt i = 0;
    if (!iIsEOFReached &&
        ( iBufferEmptiedEventAO->Error() == KErrNone) )
        {

        for(i=0; i < iAvailable.Count(); i++)
            {
            if(iAvailable[i])
                break;
            }

        if(!iBuffering)
            {
            TInt err = ReadFromFileAndWriteToStream( i );
            if ( err != KErrNone)
                {
                // Do Something..
                }
            }
        }
    else if (iBufferEmptiedEventAO->Error() == KErrCancel)
        {

        /*if(!iStopCalled)
            {
            MDataBuffer* buffer = iBufferEmptiedEventAO->GetBuffer();
            iBytesReadFromFile = iBytesReadFromFile - buffer->GetBufferPtr().Length();
            TInt seekOffset = -(buffer->GetBufferPtr().Length());
            iFile.Seek( ESeekCurrent, seekOffset );

            TInt filePos = 0;
            iFile.Seek(ESeekCurrent,filePos);

            DEBPRN4(_L("CAudioPdProperties::BufferEmptiedBySource() \
            BytesReadFromFile[%d] filePos[%d] MaxLength[%d]"),iBytesReadFromFile,filePos, buffer->GetBufferPtr().MaxLength());
            }*/
        }
    }

void CAudioPdProperties::FillSourceBuffers()
    {
    DEBPRN1(_L("CAudioPdProperties::FillSourceBuffers() enter"));
    TInt index = iAvailable.Find(ETrue);
    DEBPRN2(_L("CAudioPdProperties::FillSourceBuffers() index[%d]"),index);
    while ( (index != KErrNotFound ) && (!iIsEOFReached))
        {
        TInt err = ReadFromFileAndWriteToStream( index );
        if(err)
            {
            iBuffering = ETrue;
            break;
            }
        else
            {
            iBuffering = EFalse;
            }
        // Exit the loop if file EOF.
        if ( !iIsEOFReached )
            {
            // Get the next free buffer
            index = iAvailable.Find(ETrue);
            }
        }
    DEBPRN2(_L("CAudioPdProperties::FillSourceBuffers() exit Buffering[%d]"), iBuffering);
    }

TInt CAudioPdProperties::ExtractDRMRestrictionsL()
    {
    DEBPRN1(_L("CAudioPdProperties::ExtractDRMRestrictionsL() enter"));
    TInt status(KErrNone);

    ContentAccess::CContent* content(NULL);
	ContentAccess::CData* data(NULL);
	CDRMConfigIntfc* drmConfigIntfc(NULL);


    if(iFileName)
        {
	    content = CContent::NewL(*iFileName, EContentShareReadWrite) ;
        }
    else
        {
        content = CContent::NewL(iFileHandle);
        }
	data = content->OpenContentL(EPeek,EContentShareReadWrite);

	status = data->GetAttribute(EIsProtected, isProtected);

	if(!isProtected)
	    {
        DEBPRN2(_L("CAudioPdProperties::ExtractDRMRestrictionsL() isProtected[%d]"),isProtected);
        delete data;
        delete content;
        return status;
	    }

    TInt value = 0;
    content->GetAttribute(EDrmAllowedOutputs,value);
    TPtr8 des = iMimeType->Des();
    data->GetMimeTypeL(des);

    TAny* intfc(NULL);
    TVersion ver(KDRMConfigIntfcMajorVer1, KDRMConfigIntfcMinorVer1, KDRMConfigIntfcBuildVer1);
    status = iMDataBufferSource->GetInterface(KDRMConfigIntfc,
                                                  ver,
                                                  intfc);
    if (status == KErrNone)
        {
        drmConfigIntfc = (CDRMConfigIntfc*)intfc;
        }
    else
        {
        delete data;
        delete content;
        return status;
        }

    if(value & EDrmAllowAudioAnalog)
        {
        drmConfigIntfc->AppendAllowedOutputDevice(EAudioAllowAnalog);
        }
    if(value & EDrmAllowAudioFmTransmitter)
        {
        drmConfigIntfc->AppendAllowedOutputDevice(EAudioAllowFMTx);
        }
    if(value & EDrmAllowAudioBluetooth)
        {
        drmConfigIntfc->AppendAllowedOutputDevice(EAudioAllowBTA2DP);
        drmConfigIntfc->AppendAllowedOutputDevice(EAudioAllowBTHFPHSP);
        }
    if(value & EDrmAllowAudioUplink)
        {
        drmConfigIntfc->AppendAllowedOutputDevice(EAudioAllowUplink);
        }
    if(value & EDrmAllowVideoAnalog)
        {
        drmConfigIntfc->AppendAllowedOutputDevice(EVideoAllowAnalog);
        }
    if(value & EDrmAllowVideoMacroVision)
        {
        drmConfigIntfc->AppendAllowedOutputDevice(EVideoAllowMacroVision);
        }

    if(value == EDrmAllowAll)
        {
        drmConfigIntfc->AppendAllowedOutputDevice(EAudioAllowAll);
        }
    /**
	* RIM CR 417-7642: HDMI with HDCP to Resctricted Audio Output API
	* Due to addition of new ENUMs to CRestrictedAudioOutput::TAllowedOutputPreference for HDMI and HDCP
	* EAllowAudioHDMI and EAllowAudioHdmiHdcpRequired,the same is matched by adding
	* EAudioAllowHDMI and EAudioAllowHdmiHdcpRequired. These ENUM values are matched for the values from drmagents.h
	*/

    if(value == EDrmAllowAudioHdmi)
        {
        drmConfigIntfc->AppendAllowedOutputDevice(EAudioAllowHDMI);
        }
    if(value == EDrmAllowAudioHdmiHdcpRequired)
        {
        drmConfigIntfc->AppendAllowedOutputDevice(EAudioAllowHdmiHdcpRequired);
        }

    delete data;
    delete content;
    delete drmConfigIntfc;

    return status;

    }

TInt CAudioPdProperties::ReOpenCAF()
    {
    TInt status(KErrNone);
    DEBPRN1(_L("CAudioPdProperties::ReOpenCAF"));

    // Read data into CAudioBuffer
    TInt filePos = 0;
    iFile->Seek(ESeekCurrent,filePos);

    delete iFile;
    iFile = NULL;

    if(iFileName)
        {
       	iFile =  CContentFile::NewL(iFs,*iFileName, KNullDesC, EFileShareAny,EFalse);
        }
    else
        {
        iFile =  CContentFile::NewL(iFileHandle, KNullDesC ,EFalse);
        }

    if(iFile)
        {
        status = iFile->Seek(ESeekStart,filePos);
        if(isProtected)
            {
            iFile->Size(iFileSize);
            SetFileSize(iFileSize);
            }
        }

    DEBPRN3(_L("CAudioPdProperties::ReOpenCAF Exit status[%d] iFileSize[%d]"),status,iFileSize);
    return status;
    }

// End of File
