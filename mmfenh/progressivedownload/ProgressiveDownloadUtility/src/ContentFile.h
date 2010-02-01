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
* Description:  Progressive Download Utility  Header of ContentFile Classes.
*
*/

#ifndef __CONTENT_FILE_H__
#define __CONTENT_FILE_H__

#include <content.h>
#include <data.h>
#include <mmf/common/mmcaf.h>

// Provides content access framework based file access
class CContentFile : public CBase
    {
    public:
        static CContentFile* NewL(RFs& aSession, const TDesC& aPath, const TDesC& aUniqueId, TUint aMode, TBool aEnableUI);
        void ConstructL(RFs& aSession, const TDesC& aPath, const TDesC& aUniqueId, TUint aMode, TBool aEnableUI);

        static CContentFile* NewL(RFile& aFile, const TDesC& aUniqueId, TBool aEnableUI);
        void ConstructL(RFile& aFile, const TDesC& aUniqueId, TBool aEnableUI);

        virtual ~CContentFile();

        // from MGenericFile
        virtual TInt Seek(TSeek aSeekMode, TInt& aPosition);
        virtual TInt Read(TDes8& aDes,TInt aLength);
        virtual void Read(TDes8& aDes, TInt aLength, TRequestStatus& aStatus);
        virtual TInt Write(const TDesC8& aDes,TInt aLength);
        virtual void Write(const TDesC8& aDes, TInt aLength, TRequestStatus& aStatus);
        virtual TInt Size(TInt& aSize);
        virtual TInt SetSize(TInt aSize);
        virtual TInt EvaluateIntent(const ContentAccess::TIntent aIntent) const;
        virtual TInt ExecuteIntent(const ContentAccess::TIntent aIntent);
        virtual TBool IsProtected() const;
        virtual RFile& FileL();

        virtual TInt SetAgentProperty(ContentAccess::TAgentProperty, TInt aValue);

    private:
        CContentFile();


    private:
        RFs* iSession; // not owned
        HBufC* iFilePath;
        ContentAccess::CData* iData;

        RFile iLegacyFile;
        TBool iLegacyFileOpen;
    };

inline CContentFile::CContentFile()
    {
    }

#endif //__CONTENT_FILE_H__
