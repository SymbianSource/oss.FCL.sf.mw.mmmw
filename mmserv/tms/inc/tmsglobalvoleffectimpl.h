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

#ifndef TMS_GLOBAL_VOLUME_EFFECT_IMPL_H
#define TMS_GLOBAL_VOLUME_EFFECT_IMPL_H

#include <glib.h>
#include <tmsglobalvoleffect.h>

namespace TMS {

// FORWARD DECLARATIONS
class TMSProxy;

// TMSGlobalVolEffectImpl class
class TMSGlobalVolEffectImpl : public TMSGlobalVolEffect
    {
public:
    ~TMSGlobalVolEffectImpl();
    static gint Create(TMSEffect*& tmseffect);

private:
    TMSGlobalVolEffectImpl();
    gint PostConstruct();
    gint SetParentEffect(TMSEffect*& parenteffect);
    };

} //namespace TMS

#endif // TMS_GLOBAL_VOLUME_EFFECT_IMPL_H

// End of file
