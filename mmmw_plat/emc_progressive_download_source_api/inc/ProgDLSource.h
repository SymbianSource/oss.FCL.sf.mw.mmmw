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
* Description:  Header of ProgDLSource class.
*
*/


#ifndef PROGDLSOURCE_H
#define PROGDLSOURCE_H

#include <e32base.h>
#include <SourceControl.h>

namespace multimedia
    {
    
    const TUid KProgDLSourceControl = {0x10207B43};
    

    // Class declaration
    class MProgDLSource : public MSourceControl
        {
        public:
        
            enum TDownloadStatus
            {
                EUnknown,
                EConnecting,
                EStarted,
                EPaused,
                EFailed,
                EDeleted,
                ECompleted
            };
            
            // Control msg
            virtual TInt Open(const TDesC& aFileName,TInt32 aDLTxId ) = 0;
            virtual TInt MoveFile(const TDesC& aDestFileName ) = 0;
            virtual TInt CancelDownload() = 0;
            virtual TInt ResumeDownload() = 0;
            virtual TInt GetCurrentFileSize( TUint& aSize ) = 0;
            virtual TInt GetExpectedFileSize( TUint& aSize ) = 0;
            virtual TDownloadStatus GetDownloadStatus() = 0;
            virtual TInt IsDownloadComplete(TBool& aBool) = 0;
            virtual TInt GetPercentageDownloaded(TUint& aPercent) = 0;
            virtual TInt GetPercentageBuffered(TUint& aPercent) = 0;            
            virtual TInt GetDownloadingRate(TUint& aRate) = 0;
            virtual TInt GetBitRate(TUint& aRate) = 0;
            virtual TInt FileName(TPtr& aFileName) = 0;            
        };
    } // namespace multimedia

#endif // PROGDLSOURCE_H

// End of file
