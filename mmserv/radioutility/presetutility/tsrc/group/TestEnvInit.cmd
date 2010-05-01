@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: 
@rem

@echo Initializing test environment for presetutilitytestmodule...
copy ..\conf\PresetUtilityTestModule.cfg \epoc32\winscw\c\TestFramework\PresetUtilityTestModule.cfg
copy ..\init\PresetUtilityTestModule.ini \epoc32\wins\c\TestFramework\TestFramework.ini
copy ..\init\PresetUtilityTestModule.ini \epoc32\winscw\c\TestFramework\TestFramework.ini
copy ..\conf\10207b87.txt  \epoc32\data\z\private\10202be9\10207b87.txt
copy ..\conf\10207b87.txt  \epoc32\RELEASE\winscw\UDEB\Z\private\10202be9\10207b87.txt
copy ..\conf\10207b87.txt  \epoc32\RELEASE\winscw\UREL\Z\private\10202be9\10207b87.txt
del \epoc32\winscw\c\private\10202BE9\persists\10207b87.cre
@echo Test environment initialization for presetutilitytestmodule... Finished.