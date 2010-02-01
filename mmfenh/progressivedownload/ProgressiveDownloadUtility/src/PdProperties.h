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



#ifndef __PDPROPERTIES_H__
#define __PDPROPERTIES_H__

#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include "AudioPdPlayUtility.h"

//Forward Declaration


class CPdProperties :public CBase,
                     public MAudioPdPlayUtilityCallback
    {
    public:

        IMPORT_C static CPdProperties* NewL();
        IMPORT_C static CPdProperties* NewLC();

        IMPORT_C ~CPdProperties();

        IMPORT_C CAudioPdPlayUtility* OpenL(const TDesC& aFileName,MAudioPdPlayUtilityCallback& aCallback);
        IMPORT_C CAudioPdPlayUtility* OpenL(const RFile& aFileHandle,MAudioPdPlayUtilityCallback& aCallback);

    private:
        CPdProperties();
        void ConstructL();
        void HandleDownloadEventL(TUint aTransactionID,
                              TBrCtlDownloadEvent aEvent,
                              TUint aValue);

        TInt CalculatePercentageDownloaded();
        void SavePosition();
        void HandleDownloadUpdateL();

        // This function is no where used. Remove it.
        //void StartPaused();
        void StartPlaying();

        void Playing();
        void Paused();

        //From MMdaAudioPlayerCallback
        void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
        void MapcPlayComplete(TInt aError);

        enum TState
            {
            EStopped,
            /*EOpening,*/
            /*EPaused,*/
            EStoppedAutoResume, /* State which will automatically re-start playback*/
            EPlaying
            };

        TTimeIntervalMicroSeconds iTimePosition;
        MAudioPdPlayUtilityCallback* iCallback;
        CAudioPdPlayUtility* iAudioPdPlayUtility;
        HBufC* iFileName;
        RFile iFileHandle;
        TBool iFileOpened;
        TInt  iAttemptsToOpen;
        TBool iUseFileHandle;
        //TInt  iError; /* Not used */
        TInt  iFileSize;
        TInt  iBytesDownloaded;
        TInt  iFileBytePosition;
        TBool iDownloadCompleted;
        enum  TState iState;
        TInt  iPercentageToBuffer;
        TBool iOpenInProgress;
    };

#endif

// End of file
