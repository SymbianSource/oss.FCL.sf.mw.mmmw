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
 * Description: Telephony Multimedia Service
 *
 */

#ifndef TMS_GAIN_EFFECT_IMPL_H
#define TMS_GAIN_EFFECT_IMPL_H

#include <glib.h>
#include <tmsgaineffect.h>

namespace TMS {

// FORWARD DECLARATIONS
class TMSCallProxy;

// TMSGainEffectImpl class
class TMSGainEffectImpl : public TMSGainEffect
    {
public:
    ~TMSGainEffectImpl();

    static gint Create(TMSEffect*& tmseffect);
    gint SetProxy(TMSCallProxy* aProxy, gpointer queuehandler);
    gint SetParentEffect(TMSEffect*& parenteffect);

private:
    TMSGainEffectImpl();
    gint PostConstruct();
    };

} //namespace TMS

#endif // TMS_GAIN_EFFECT_IMPL_H

// End of file
