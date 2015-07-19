//
//  KeXAudio2AudioEffect.cpp
//
//  Created by Shogun3D on 6/28/15.
//  Copyright (c) 2015 Shogun3D. All rights reserved.
//

#include "Ke.h"
#include "KeAudioDevice.h"
#include "KeXAudio2AudioDevice.h"

/*
 * Name: IKeXAudio2AudioEffect::Destroy
 * Desc: Handles destruction of this interface instance.
 */
void IKeXAudio2AudioEffect::Destroy()
{
    /* Delete this instance */
    delete this;
}