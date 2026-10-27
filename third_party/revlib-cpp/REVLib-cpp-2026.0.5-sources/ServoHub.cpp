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

#include "rev/ServoHub.h"

#include <string>

#include <fmt/format.h>

#include "rev/CANServoHubDriver.h"

using namespace rev;
using namespace rev::servohub;

ServoHub::ServoHub(int deviceID)
    : ServoHubLowLevel(deviceID),
      configAccessor{m_servoHubHandle},
      m_servoChannels(
          {ServoChannel(ServoChannel::ChannelId::kChannelId0, m_servoHubHandle),
           ServoChannel(ServoChannel::ChannelId::kChannelId1, m_servoHubHandle),
           ServoChannel(ServoChannel::ChannelId::kChannelId2, m_servoHubHandle),
           ServoChannel(ServoChannel::ChannelId::kChannelId3, m_servoHubHandle),
           ServoChannel(ServoChannel::ChannelId::kChannelId4, m_servoHubHandle),
           ServoChannel(ServoChannel::ChannelId::kChannelId5,
                        m_servoHubHandle)}) {}

ServoHub::~ServoHub() {}

REVLibError ServoHub::Configure(ServoHubConfig& config, ResetMode resetMode) {
    return Configure(config, static_cast<rev::ResetMode>(resetMode));
}

REVLibError ServoHub::Configure(ServoHubConfig& config,
                                rev::ResetMode resetMode) {
    REVLibError status = static_cast<REVLibError>(c_ServoHub_Configure(
        static_cast<c_ServoHub_handle>(m_servoHubHandle),
        config.Flatten().c_str(),
        resetMode == rev::ResetMode::kResetSafeParameters));

    if (status != REVLibError::kOk) {
        throw std::runtime_error(
            c_REVLib_ErrorFromCode(static_cast<c_REVLib_ErrorCode>(status)));
    }
    return status;
}

REVLibError ServoHub::ConfigureAsync(ServoHubConfig& config,
                                     ResetMode resetMode) {
    return Configure(config, static_cast<rev::ResetMode>(resetMode));
}

REVLibError ServoHub::ConfigureAsync(ServoHubConfig& config,
                                     rev::ResetMode resetMode) {
    REVLibError status = static_cast<REVLibError>(c_ServoHub_ConfigureAsync(
        static_cast<c_ServoHub_handle>(m_servoHubHandle),
        config.Flatten().c_str(),
        resetMode == rev::ResetMode::kResetSafeParameters));
    return status;
}

bool ServoHub::HasActiveFault() const {
    uint16_t faults;
    c_ServoHub_GetFaults(static_cast<c_ServoHub_handle>(m_servoHubHandle),
                         &faults);
    return faults != 0;
}

bool ServoHub::HasStickyFault() const {
    uint16_t faults;
    c_ServoHub_GetStickyFaults(static_cast<c_ServoHub_handle>(m_servoHubHandle),
                               &faults);
    return faults != 0;
}

bool ServoHub::HasActiveWarning() const {
    uint16_t warnings;
    c_ServoHub_GetWarnings(static_cast<c_ServoHub_handle>(m_servoHubHandle),
                           &warnings);
    return warnings != 0;
}

bool ServoHub::HasStickyWarning() const {
    uint16_t warnings;
    c_ServoHub_GetStickyWarnings(
        static_cast<c_ServoHub_handle>(m_servoHubHandle), &warnings);
    return warnings != 0;
}

ServoHub::Faults::Faults(uint16_t faults) {
    rawBits = faults;
    regulatorPowerGood =
        (faults & c_ServoHub_FaultMask_kRegulatorPowerGood) != 0;
    hardware = (faults & c_ServoHub_FaultMask_kHardware) != 0;
    firmware = (faults & c_ServoHub_FaultMask_kFirmware) != 0;
    lowBattery = (faults & c_ServoHub_FaultMask_kLowBattery) != 0;
}

ServoHub::Faults ServoHub::GetFaults() const {
    uint16_t rawBits;
    c_ServoHub_GetFaults(static_cast<c_ServoHub_handle>(m_servoHubHandle),
                         &rawBits);
    Faults faults{rawBits};
    return faults;
}

ServoHub::Faults ServoHub::GetStickyFaults() const {
    uint16_t rawBits;
    c_ServoHub_GetStickyFaults(static_cast<c_ServoHub_handle>(m_servoHubHandle),
                               &rawBits);
    Faults faults{rawBits};
    return faults;
}

ServoHub::Warnings::Warnings(uint16_t warnings) {
    rawBits = warnings;
    brownout = (warnings & c_ServoHub_WarningMask_kBrownout) != 0;
    canWarning = (warnings & c_ServoHub_WarningMask_kCanWarning) != 0;
    canBusOff = (warnings & c_ServoHub_WarningMask_kCanBusOff) != 0;
    hasReset = (warnings & c_ServoHub_WarningMask_kHasReset) != 0;
    channel0Overcurrent =
        (warnings & c_ServoHub_WarningMask_kChannel0Overcurrent) != 0;
    channel1Overcurrent =
        (warnings & c_ServoHub_WarningMask_kChannel1Overcurrent) != 0;
    channel2Overcurrent =
        (warnings & c_ServoHub_WarningMask_kChannel2Overcurrent) != 0;
    channel3Overcurrent =
        (warnings & c_ServoHub_WarningMask_kChannel3Overcurrent) != 0;
    channel4Overcurrent =
        (warnings & c_ServoHub_WarningMask_kChannel4Overcurrent) != 0;
    channel5Overcurrent =
        (warnings & c_ServoHub_WarningMask_kChannel5Overcurrent) != 0;
}

ServoHub::Warnings ServoHub::GetWarnings() const {
    uint16_t rawBits;
    c_ServoHub_GetWarnings(static_cast<c_ServoHub_handle>(m_servoHubHandle),
                           &rawBits);
    Warnings warnings{rawBits};
    return warnings;
}

ServoHub::Warnings ServoHub::GetStickyWarnings() const {
    uint16_t rawBits;
    c_ServoHub_GetStickyWarnings(
        static_cast<c_ServoHub_handle>(m_servoHubHandle), &rawBits);
    Warnings warnings{rawBits};
    return warnings;
}

REVLibError ServoHub::ClearFaults() {
    c_REVLib_ErrorCode status = c_ServoHub_ClearFaults(
        static_cast<c_ServoHub_handle>(m_servoHubHandle));
    return status == c_REVLibError_None ? REVLibError::kOk
                                        : REVLibError::kHALError;
}

double ServoHub::GetDeviceVoltage() const {
    float tmp;
    c_ServoHub_GetDeviceVoltage(
        static_cast<c_ServoHub_handle>(m_servoHubHandle), &tmp);
    return static_cast<double>(tmp);
}

double ServoHub::GetDeviceCurrent() const {
    float tmp;
    c_ServoHub_GetDeviceCurrent(
        static_cast<c_ServoHub_handle>(m_servoHubHandle), &tmp);
    return static_cast<double>(tmp);
}

double ServoHub::GetServoVoltage() const {
    float tmp;
    c_ServoHub_GetServoVoltage(static_cast<c_ServoHub_handle>(m_servoHubHandle),
                               &tmp);
    return static_cast<double>(tmp);
}

#define TO_SERVO_HUB_CHANNEL(channel) \
    static_cast<c_ServoHub_Channel>(static_cast<int>(channel))

ServoChannel& ServoHub::GetServoChannel(ServoChannel::ChannelId channelId) {
    return m_servoChannels[static_cast<size_t>(channelId)];
}

#define TO_SERVO_HUB_BANK(bank) \
    static_cast<c_ServoHub_Bank>(static_cast<int>(bank))

REVLibError ServoHub::SetBankPulsePeriod(Bank bank, int pulsePeriod_us) {
    auto status = c_ServoHub_SetBankPulsePeriod(
        static_cast<c_ServoHub_handle>(m_servoHubHandle),
        TO_SERVO_HUB_BANK(bank), pulsePeriod_us);
    return static_cast<REVLibError>(status);
}
