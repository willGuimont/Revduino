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

#include "rev/ServoHubLowLevel.h"

#include <string>

#include <fmt/format.h>

#include "rev/CANServoHubDriver.h"

using namespace rev;
using namespace rev::servohub;

ServoHubLowLevel::ServoHubLowLevel(int deviceID) : m_deviceID(deviceID) {
    c_REVLib_ErrorCode status{c_ServoHub_RegisterId(deviceID)};
    if (status == c_REVLibError_InvalidCANId) {
        throw std::runtime_error(
            fmt::format("deviceId must be between 0-62; ID: {}", deviceID));
    }

    if (status == c_REVLibError_DuplicateCANId) {
        throw std::runtime_error(fmt::format(
            "A ServoHub instance has already been created with this device "
            "ID: {}",
            deviceID));
    }

    m_servoHubHandle = static_cast<void*>(c_ServoHub_Create(deviceID, &status));

    if (status != c_REVLibError_None) {
        throw std::runtime_error(fmt::format("Error ({}) creating ServoHub#{}.",
                                             static_cast<int>(status),
                                             deviceID));
    }
}

ServoHubLowLevel::~ServoHubLowLevel() {
    c_ServoHub_Close(static_cast<c_ServoHub_handle>(m_servoHubHandle));
    c_ServoHub_Destroy(static_cast<c_ServoHub_handle>(m_servoHubHandle));
}

int ServoHubLowLevel::GetDeviceId() const { return m_deviceID; }

ServoHubLowLevel::FirmwareVersion ServoHubLowLevel::GetFirmwareVersion() const {
    c_ServoHub_FirmwareVersion fwVersion;

    c_ServoHub_GetFirmwareVersion(
        static_cast<c_ServoHub_handle>(m_servoHubHandle), &fwVersion);

    return FirmwareVersion{fwVersion.fwFix, fwVersion.fwMinor, fwVersion.fwYear,
                           fwVersion.hwMinor, fwVersion.hwMajor};
}

std::string ServoHubLowLevel::GetFirmwareVersionString() const {
    FirmwareVersion fwVersion{GetFirmwareVersion()};

    return fmt::format("fw v{}.{}.{}, hw v{}.{}", fwVersion.firmwareYear,
                       fwVersion.firmwareMinor, fwVersion.firmwareFix,
                       fwVersion.hardwareMajor, fwVersion.hardwareMinor);
}

void ServoHubLowLevel::SetPeriodicFrameTimeout(int timeout_ms) {
    c_ServoHub_SetPeriodicFrameTimeout(
        static_cast<c_ServoHub_handle>(m_servoHubHandle), timeout_ms);
}

REVLibError ServoHubLowLevel::SetCANTimeout(int timeout_ms) {
    auto status = c_ServoHub_SetCANTimeout(
        static_cast<c_ServoHub_handle>(m_servoHubHandle), timeout_ms);
    return static_cast<REVLibError>(status);
}

void ServoHubLowLevel::SetCANMaxRetries(int numRetries) {
    c_ServoHub_SetCANMaxRetries(
        static_cast<c_ServoHub_handle>(m_servoHubHandle), numRetries);
}

void ServoHubLowLevel::SetControlFramePeriodMs(int period_ms) {
    c_ServoHub_SetControlFramePeriod(
        static_cast<c_ServoHub_handle>(m_servoHubHandle), period_ms);
}
int ServoHubLowLevel::GetControlFramePeriodMs() const {
    return c_ServoHub_GetControlFramePeriod(
        static_cast<c_ServoHub_handle>(m_servoHubHandle));
}

ServoHubLowLevel::PeriodicStatus0 ServoHubLowLevel::GetPeriodicStatus0() const {
    c_ServoHub_PeriodicStatus0 cStatus0;
    c_ServoHub_GetPeriodicStatus0(
        static_cast<c_ServoHub_handle>(m_servoHubHandle), &cStatus0);

    const PeriodicStatus0 status0{
        .voltage{cStatus0.voltage},
        .servoVoltage{cStatus0.servoVoltage},
        .deviceCurrent{cStatus0.deviceCurrent},
        .primaryHeartbeatLock{static_cast<bool>(cStatus0.primaryHeartbeatLock)},
        .systemEnabled{static_cast<bool>(cStatus0.systemEnabled)},
        .communicationMode{
            static_cast<CommunicationMode>(cStatus0.communicationMode)},
        .programmingEnabled{static_cast<bool>(cStatus0.programmingEnabled)},
        .activelyProgramming{static_cast<bool>(cStatus0.activelyProgramming)},
        .timestamp{cStatus0.timestamp}};

    return status0;
}

ServoHubLowLevel::PeriodicStatus1 ServoHubLowLevel::GetPeriodicStatus1() const {
    c_ServoHub_PeriodicStatus1 cStatus1;
    c_ServoHub_GetPeriodicStatus1(
        static_cast<c_ServoHub_handle>(m_servoHubHandle), &cStatus1);

    const PeriodicStatus1 status1{
        .regulatorPowerGoodFault{
            static_cast<bool>(cStatus1.regulatorPowerGoodFault)},
        .brownout{static_cast<bool>(cStatus1.brownout)},
        .canWarning{static_cast<bool>(cStatus1.canWarning)},
        .canBusOff{static_cast<bool>(cStatus1.canBusOff)},
        .hardwareFault{static_cast<bool>(cStatus1.hardwareFault)},
        .firmwareFault{static_cast<bool>(cStatus1.firmwareFault)},
        .hasReset{static_cast<bool>(cStatus1.hasReset)},
        .channel0Overcurrent{static_cast<bool>(cStatus1.channel0Overcurrent)},
        .channel1Overcurrent{static_cast<bool>(cStatus1.channel1Overcurrent)},
        .channel2Overcurrent{static_cast<bool>(cStatus1.channel2Overcurrent)},
        .channel3Overcurrent{static_cast<bool>(cStatus1.channel3Overcurrent)},
        .channel4Overcurrent{static_cast<bool>(cStatus1.channel4Overcurrent)},
        .channel5Overcurrent{static_cast<bool>(cStatus1.channel5Overcurrent)},
        .stickyRegulatorPowerGoodFault{
            static_cast<bool>(cStatus1.stickyRegulatorPowerGoodFault)},
        .stickyBrownout{static_cast<bool>(cStatus1.stickyBrownout)},
        .stickyCanWarning{static_cast<bool>(cStatus1.stickyCanWarning)},
        .stickyCanBusOff{static_cast<bool>(cStatus1.stickyCanBusOff)},
        .stickyHardwareFault{static_cast<bool>(cStatus1.stickyHardwareFault)},
        .stickyFirmwareFault{static_cast<bool>(cStatus1.stickyFirmwareFault)},
        .stickyHasReset{static_cast<bool>(cStatus1.stickyHasReset)},
        .stickyChannel0Overcurrent{
            static_cast<bool>(cStatus1.stickyChannel0Overcurrent)},
        .stickyChannel1Overcurrent{
            static_cast<bool>(cStatus1.stickyChannel1Overcurrent)},
        .stickyChannel2Overcurrent{
            static_cast<bool>(cStatus1.stickyChannel2Overcurrent)},
        .stickyChannel3Overcurrent{
            static_cast<bool>(cStatus1.stickyChannel3Overcurrent)},
        .stickyChannel4Overcurrent{
            static_cast<bool>(cStatus1.stickyChannel4Overcurrent)},
        .stickyChannel5Overcurrent{
            static_cast<bool>(cStatus1.stickyChannel5Overcurrent)},
        .timestamp{cStatus1.timestamp}};

    return status1;
}

ServoHubLowLevel::PeriodicStatus2 ServoHubLowLevel::GetPeriodicStatus2() const {
    c_ServoHub_PeriodicStatus2 cStatus2;
    c_ServoHub_GetPeriodicStatus2(
        static_cast<c_ServoHub_handle>(m_servoHubHandle), &cStatus2);

    const PeriodicStatus2 status2{
        .channel0PulseWidth{cStatus2.channel0PulseWidth},
        .channel1PulseWidth{cStatus2.channel1PulseWidth},
        .channel2PulseWidth{cStatus2.channel2PulseWidth},
        .channel0Enabled{static_cast<bool>(cStatus2.channel0Enabled)},
        .channel1Enabled{static_cast<bool>(cStatus2.channel1Enabled)},
        .channel2Enabled{static_cast<bool>(cStatus2.channel2Enabled)},
        .channel0OutOfRange{static_cast<bool>(cStatus2.channel0OutOfRange)},
        .channel1OutOfRange{static_cast<bool>(cStatus2.channel1OutOfRange)},
        .channel2OutOfRange{static_cast<bool>(cStatus2.channel2OutOfRange)},
        .timestamp{cStatus2.timestamp}};

    return status2;
}

ServoHubLowLevel::PeriodicStatus3 ServoHubLowLevel::GetPeriodicStatus3() const {
    c_ServoHub_PeriodicStatus3 cStatus3;
    c_ServoHub_GetPeriodicStatus3(
        static_cast<c_ServoHub_handle>(m_servoHubHandle), &cStatus3);

    const PeriodicStatus3 status3{
        .channel3PulseWidth{cStatus3.channel3PulseWidth},
        .channel4PulseWidth{cStatus3.channel4PulseWidth},
        .channel5PulseWidth{cStatus3.channel5PulseWidth},
        .channel3Enabled{static_cast<bool>(cStatus3.channel3Enabled)},
        .channel4Enabled{static_cast<bool>(cStatus3.channel4Enabled)},
        .channel5Enabled{static_cast<bool>(cStatus3.channel5Enabled)},
        .channel3OutOfRange{static_cast<bool>(cStatus3.channel3OutOfRange)},
        .channel4OutOfRange{static_cast<bool>(cStatus3.channel4OutOfRange)},
        .channel5OutOfRange{static_cast<bool>(cStatus3.channel5OutOfRange)},
        .timestamp{cStatus3.timestamp}};

    return status3;
}

ServoHubLowLevel::PeriodicStatus4 ServoHubLowLevel::GetPeriodicStatus4() const {
    c_ServoHub_PeriodicStatus4 cStatus4;
    c_ServoHub_GetPeriodicStatus4(
        static_cast<c_ServoHub_handle>(m_servoHubHandle), &cStatus4);

    PeriodicStatus4 status4;
    status4.channel0Current = cStatus4.channel0Current;
    status4.channel1Current = cStatus4.channel1Current;
    status4.channel2Current = cStatus4.channel2Current;
    status4.channel3Current = cStatus4.channel3Current;
    status4.channel4Current = cStatus4.channel4Current;
    status4.channel5Current = cStatus4.channel5Current;
    status4.timestamp = cStatus4.timestamp;

    return status4;
}

void ServoHubLowLevel::CreateSimFaultManager() {
    c_SIM_ServoHub_CreateSimFaultManager(
        static_cast<c_ServoHub_handle>(m_servoHubHandle));
}
