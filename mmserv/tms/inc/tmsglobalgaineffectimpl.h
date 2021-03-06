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

#ifndef TMS_GLOBAL_GAIN_EFFECT_IMPL_H
#define TMS_GLOBAL_GAIN_EFFECT_IMPL_H

#include <glib.h>
#include <tmsglobalgaineffect.h>

namespace TMS {

// TMSGlobalGainEffectImpl class
class TMSGlobalGainEffectImpl : public TMSGlobalGainEffect
    {
public:
    virtual ~TMSGlobalGainEffectImpl();
    static gint Create(TMSEffect*& tmseffect);

private:
    TMSGlobalGainEffectImpl();
    gint PostConstruct();
    gint SetParent(TMSEffect*& parent);
    };

} //namespace TMS

#endif // TMS_GLOBAL_GAIN_EFFECT_IMPL_H

