/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Telephony Multimedia Service - STIF TEST
 *
 */

// INCLUDE FILES
#include "TmsAudioServicesTestClass.h"
#include "debug.h"

void CTmsAudioServicesTestClass::TMSStreamEvent(const TMSStream& stream,
        TMSSignalEvent event)
    {
    iLog->Log(_L("[tms cb]CTmsAudioServicesTestClass::TMSStreamEvent"));
    switch (const_cast<TMSStream&>(stream).GetStreamType())
        {
        case TMS_STREAM_UPLINK:
            {
            switch (event.type)
                {
                case TMS_EVENT_STREAM_STATE_CHANGED:
                    {
                    iLog->Log(_L("[tms cb]Uplink state change"));
                    switch (event.curr_state)
                        {
                        case TMS_STREAM_INITIALIZED:
                            iUpLinkStatus = INITIALIZED;
                            iLog->Log(_L("[tms cb]TMSStreamEvent EOpenUplinkComplete"));
                            ProcessEvent(EOpenUplinkComplete, KErrNone);
                            break;
                        case TMS_STREAM_UNINITIALIZED:
                            iUpLinkStatus = UNINITIALIZED;
                            iLog->Log(_L("[tms cb]TMSStreamEvent EUplinkClosed"));
                            ProcessEvent(EUplinkClosed, KErrNone);
                            break;
                        case TMS_STREAM_PAUSED:
                            iUpLinkStatus = PAUSED;
                            iLog->Log(_L("[tms cb]TMSStreamEvent EStreamPaused"));
                            ProcessEvent(EStreamPaused, KErrNone);
                            break;
                        case TMS_STREAM_STARTED:
                            iUpLinkStatus = STARTED;
                            iLog->Log(_L("[tms cb]TMSStreamEvent EStreamStarted"));
                            ProcessEvent(EStreamStarted, KErrNone);
                            break;
                        default:
                            break;
                        }
                    break;
                    }
                default:
                    break;
                }
            break;
            }
        case TMS_STREAM_DOWNLINK:
            {
            switch (event.type)
                {
                case TMS_EVENT_STREAM_STATE_CHANGED:
                    {
                    switch (event.curr_state)
                        {
                        case TMS_STREAM_INITIALIZED:
                            iDnLinkStatus = INITIALIZED;
                            iLog->Log(_L("[tms cb]TMSStreamEvent EOpenDownlinkComplete"));
                            ProcessEvent(EOpenDownlinkComplete, KErrNone);
                            break;
                        case TMS_STREAM_UNINITIALIZED:
                            iDnLinkStatus = UNINITIALIZED;
                            iLog->Log(_L("[tms cb]TMSStreamEvent EDownlinkClosed"));
                            ProcessEvent(EDownlinkClosed, KErrNone);
                            break;
                        case TMS_STREAM_PAUSED:
                            iDnLinkStatus = PAUSED;
                            iLog->Log(_L("[tms cb]TMSStreamEvent EStreamPaused"));
                            ProcessEvent(EStreamPaused, KErrNone);
                            break;
                        case TMS_STREAM_STARTED:
                            iDnLinkStatus = STARTED;
                            iLog->Log(_L("[tms cb]TMSStreamEvent EStreamStarted"));
                            ProcessEvent(EStreamStarted, KErrNone);
                            break;
                        default:
                            break;
                        }
                    break;
                    }
                default:
                    break;
                }
            break;
            }
        default:
            break;
        }
    }

//From TMSClientSourceObserver
void CTmsAudioServicesTestClass::FillBuffer(TMSBuffer& buffer)
    {
    iLog->Log(_L("[tms cb]CTmsAudioServicesTestClass::FillBuffer"));
    ProcessEvent(EFillBuffer, KErrNone);
    iPlayBufReady = ETrue;
    iPlayBuf = &buffer;
    if (iDnLinkStatus == STARTED)
        {
        DoLoopback();
        }
    }

void CTmsAudioServicesTestClass::BufferProcessed(const TMSBuffer* /*buffer*/,
        gint /*reason*/)
    {
    iLog->Log(_L("[tms cb]CTmsAudioServicesTestClass::BufferProcessed"));
    }

//From TMSClientSinkObserver
void CTmsAudioServicesTestClass::ProcessBuffer(const TMSBuffer* buffer)
    {
    iLog->Log(_L("[tms cb]CTmsAudioServicesTestClass::ProcessBuffer"));
    ProcessEvent(EEmptyBuffer, KErrNone);
    iRecBufReady = ETrue;
    iRecBuf = const_cast<TMSBuffer*>(buffer);
    if (iUpLinkStatus == STARTED)
        {
        // Process recorded buffer here.
        DoLoopback();
        //static_cast<TMSClientSink*>(iTmsSink)->BufferProcessed(iRecBuf);
        }
    }

void CTmsAudioServicesTestClass::EffectsEvent(const TMSEffect& tmseffect,
        TMSSignalEvent /*event*/)
    {
    iLog->Log(_L("[tms cb]CTmsAudioServicesTestClass::EffectsEvent"));
    TMSEffectType effecttype;
    const_cast<TMSEffect&>(tmseffect).GetType(effecttype);
    switch (effecttype)
        {
        case TMS_EFFECT_VOLUME:
        case TMS_EFFECT_GAIN:
            break;
        default:
            break;
        }
    }

// From TMSGlobalRoutingObserver
void CTmsAudioServicesTestClass::GlobalRoutingEvent(
        const TMSGlobalRouting& /*routing*/, TMSSignalEvent event,
        TMSAudioOutput /*output*/)
    {
    iLog->Log(_L("[tms cb]CTmsAudioServicesTestClass::GlobalRoutingEvent"));
    switch (event.type)
        {
        case TMS_EVENT_ROUTING_AVAIL_OUTPUTS_CHANGED:
            iLog->Log(_L("Available outputs changed"));
            break;
        case TMS_EVENT_ROUTING_OUTPUT_CHANGED:
            iLog->Log(_L("output changed"));
            ProcessEvent(EOutputChanged, KErrNone);
            break;
        case TMS_EVENT_ROUTING_SET_OUTPUT_COMPLETE:
            iLog->Log(_L("set output complete"));
            ProcessEvent(ESetOutputComplete, KErrNone);
            break;
        default:
            break;
        }
    }
