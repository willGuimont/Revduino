/*
 * Copyright (c) 2024-2025 REV Robotics
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of REV Robotics nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "rev/ServoChannel.h"

#include "rev/CANServoHubDriver.h"

using namespace rev;
using namespace rev::servohub;

ServoChannel::ServoChannel(ChannelId channelId, void* servoHubHandle)
    : m_channelId{channelId}, m_servoHubHandle{servoHubHandle} {}

ServoChannel::~ServoChannel() { m_servoHubHandle = nullptr; }

#define TO_SERVO_HUB_CHANNEL(channelId) \
    static_cast<c_ServoHub_Channel>(static_cast<int>(channelId))

int ServoChannel::GetPulseWidth() const {
    int tmp;
    c_ServoHub_GetChannelPulseWidth(
        static_cast<c_ServoHub_handle>(m_servoHubHandle),
        TO_SERVO_HUB_CHANNEL(m_channelId), &tmp);
    return tmp;
}

bool ServoChannel::IsEnabled() const {
    bool enabled;
    c_ServoHub_GetChannelEnabled(
        static_cast<c_ServoHub_handle>(m_servoHubHandle),
        TO_SERVO_HUB_CHANNEL(m_channelId), &enabled);
    return enabled;
}

double ServoChannel::GetCurrent() const {
    float tmp;
    c_ServoHub_GetChannelCurrent(
        static_cast<c_ServoHub_handle>(m_servoHubHandle),
        TO_SERVO_HUB_CHANNEL(m_channelId), &tmp);
    return static_cast<double>(tmp);
}

REVLibError ServoChannel::SetPulseWidth(int pulseWidth_us) {
    auto status = c_ServoHub_SetChannelPulseWidth(
        static_cast<c_ServoHub_handle>(m_servoHubHandle),
        TO_SERVO_HUB_CHANNEL(m_channelId), pulseWidth_us);
    return static_cast<REVLibError>(status);
}

REVLibError ServoChannel::SetEnabled(bool enabled) {
    auto status = c_ServoHub_SetChannelEnabled(
        static_cast<c_ServoHub_handle>(m_servoHubHandle),
        TO_SERVO_HUB_CHANNEL(m_channelId), enabled);
    return static_cast<REVLibError>(status);
}

REVLibError ServoChannel::SetPowered(bool powered) {
    auto status = c_ServoHub_SetChannelPowered(
        static_cast<c_ServoHub_handle>(m_servoHubHandle),
        TO_SERVO_HUB_CHANNEL(m_channelId), powered);
    return static_cast<REVLibError>(status);
}
