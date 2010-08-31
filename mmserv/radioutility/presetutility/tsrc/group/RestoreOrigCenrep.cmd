@REM
@REM Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
@REM All rights reserved.
@REM This component and the accompanying materials are made available
@REM under the terms of "Eclipse Public License v1.0"
@REM which accompanies this distribution, and is available
@REM at the URL "http://www.eclipse.org/legal/epl-v10.html".
@REM
@REM Initial Contributors:
@REM Nokia Corporation - initial contribution.
@REM
@REM Contributors:
@REM
@REM Description:  Wins Command file for Preset Utility
@REM
@REM
@echo Original environment restoration by presetutilitytestmodule...
del \epoc32\winscw\c\TestFramework\PresetUtilityTestModule.cfg
del \epoc32\wins\c\TestFramework\TestFramework.ini
del \epoc32\winscw\c\TestFramework\TestFramework.ini
copy ..\cenrep\10207b87.txt  \epoc32\data\z\private\10202be9\10207b87.txt
copy ..\cenrep\10207b87.txt  \epoc32\RELEASE\winscw\UDEB\Z\private\10202be9\10207b87.txt
copy ..\cenrep\10207b87.txt  \epoc32\RELEASE\winscw\UREL\Z\private\10202be9\10207b87.txt
del \epoc32\winscw\c\private\10202BE9\persists\10207b87.cre
@echo Original environment restoration by presetutilitytestmodule... Finished.