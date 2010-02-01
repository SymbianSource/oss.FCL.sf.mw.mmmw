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
* Description:  Progressive Download Utility  Implementation of ContentFile Classes.
*
*/

#include <f32file.h>
#include <e32std.h>
#include <caf/attribute.h>
#include <caf/bitset.h>

using namespace ContentAccess;

#include "ContentFile.h"


CContentFile::~CContentFile()
    {
    delete iData;

    if (iLegacyFileOpen)
        iLegacyFile.Close();
    delete iFilePath;
    }


CContentFile* CContentFile::NewL(RFs& aSession, const TDesC& aFilePath, const TDesC& aUniqueId, TUint aMode, TBool aEnableUI)
    {
    CContentFile* self = new (ELeave) CContentFile;
    CleanupStack::PushL(self);
    self->ConstructL(aSession, aFilePath, aUniqueId, aMode, aEnableUI);
    CleanupStack::Pop(self);
    return self;
    }

void CContentFile::ConstructL(RFs& aSession, const TDesC& aPath, const TDesC& aUniqueId, TUint /*aMode*/, TBool aEnableUI)
    {
    iSession = &aSession;
    iFilePath = aPath.AllocL();

    // Assume that we want the content to be shared and read-only.
    if (aUniqueId.Length() > 0)
        {
        iData = CData::NewL(TVirtualPathPtr(*iFilePath, aUniqueId), EContentShareReadWrite);
        }
    else
        {
        iData = CData::NewL(TVirtualPathPtr(*iFilePath), EContentShareReadWrite);
        }

    TInt err = iData->SetProperty(EAgentPropertyAgentUI, aEnableUI);
    if (err != KErrNone && err != KErrCANotSupported)
        {
        User::Leave(err);
        }
    User::LeaveIfError(iData->EvaluateIntent(EPeek));
    }



TInt CContentFile::EvaluateIntent(TIntent aIntent) const
    {
    ASSERT(iData);
    return iData->EvaluateIntent(aIntent);
    }


TInt CContentFile::ExecuteIntent(TIntent aIntent)
    {
    ASSERT(iData);
    return iData->ExecuteIntent(aIntent);
    }

TBool CContentFile::IsProtected() const
    {
    ASSERT(iData);
    TInt value = 0;
    TInt err =iData->GetAttribute(EIsProtected, value);
    return (err == KErrNone && value);
    }

TInt CContentFile::SetAgentProperty(ContentAccess::TAgentProperty aProperty, TInt aValue)
    {
    ASSERT(iData);
    return iData->SetProperty(aProperty, aValue);
    }

TInt CContentFile::Seek(TSeek aSeekMode, TInt& aPos)
    {
    ASSERT(iData);
    return iData->Seek(aSeekMode, aPos);
    }

TInt CContentFile::Read(TDes8& aDes,TInt aLength)
    {
    ASSERT(iData);
    return iData->Read(aDes,aLength);
    }

void CContentFile::Read(TDes8& aDes, TInt aLength, TRequestStatus& aStatus)
    {
    ASSERT(iData);
    iData->Read(aDes, aLength, aStatus);
    }

TInt CContentFile::Write(const TDesC8& /*aDes*/,TInt /*aLength*/)
    {
    return KErrAccessDenied;
    }
void CContentFile::Write(const TDesC8& /*aDes*/, TInt /*aLength*/, TRequestStatus& aStatus)
    {
    TRequestStatus* status = &aStatus;
    User::RequestComplete(status, KErrAccessDenied);
    }

TInt CContentFile::SetSize(TInt /*aSize*/)
    {
    // Only a source is implemented, hence this cannot be allowed
    return KErrAccessDenied;
    }


// Get the size of file
// this method opens a new, read-only, RFile the first time this method is called

TInt CContentFile::Size(TInt& aSize)
    {
    ASSERT(iData);
    TRAPD(err, iData->DataSizeL(aSize));
    return err;
    }

    /**
    * return a RFile for the legacy RFile method
    * this method opens a new, read-only, RFile the first time this method is called
    * @internalTechnology
    * @return Reference to RFile handle to current file
*/
RFile& CContentFile::FileL()
    {
    if (!iLegacyFileOpen)
        {
        User::LeaveIfError(iLegacyFile.Open(*iSession, *iFilePath, EFileRead | EFileStream | EFileShareReadersOnly));
        iLegacyFileOpen = ETrue;
        }
    return iLegacyFile;
    }

CContentFile* CContentFile::NewL(RFile& aFile, const TDesC& aUniqueId, TBool aEnableUI)
    {
    CContentFile* self = new (ELeave) CContentFile;
    CleanupStack::PushL(self);
    self->ConstructL(aFile, aUniqueId, aEnableUI);
    CleanupStack::Pop(self);
    return self;
    }

void CContentFile::ConstructL(RFile& aFile, const TDesC& aUniqueId, TBool aEnableUI)
    {
    iData = CData::NewL(aFile, aUniqueId);
    TInt err = iData->SetProperty(EAgentPropertyAgentUI, aEnableUI);
    if (err != KErrNone && err != KErrCANotSupported)
        {
        User::Leave(err);
        }
    User::LeaveIfError(iData->EvaluateIntent(EPeek));
    }
