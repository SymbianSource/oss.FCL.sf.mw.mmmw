cls
@echo off
echo.

rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of the "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
echo Description:  VAS Stif test package creation utility
rem

del VoIPAudioServicesTestClass.sis*
makesis VoIPAudioServicesTestClass.pkg VoIPAudioServicesTestClass.sis
signsis VoIPAudioServicesTestClass.sis VoIPAudioServicesTestClass.sisx rd.crt rd.key
