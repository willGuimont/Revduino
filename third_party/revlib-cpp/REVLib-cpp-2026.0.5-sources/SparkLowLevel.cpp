/*
 * Copyright (c) 2018-2025 REV Robotics
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

#include "rev/SparkLowLevel.h"

#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

#include <fmt/format.h>

#include "rev/CANSparkDriver.h"

namespace rev::spark {

const uint16_t SparkLowLevel::kAPIMajorVersion = c_Spark_kAPIMajorVersion;
const uint8_t SparkLowLevel::kAPIMinorVersion = c_Spark_kAPIMinorVersion;
const uint8_t SparkLowLevel::kAPIBuildVersion = c_Spark_kAPIBuildVersion;
const uint32_t SparkLowLevel::kAPIVersion = c_Spark_kAPIVersion;

SparkLowLevel::SparkLowLevel(int deviceID, MotorType type, SparkModel model)
    : m_motorType(type), m_deviceID(deviceID), m_expectedSparkModel(model) {
    if (c_Spark_RegisterId(deviceID) == c_REVLibError_DuplicateCANId) {
        throw std::runtime_error(fmt::format(
            "A SparkMax instance has already been created with this device "
            "ID: {}",
            deviceID));
    }

    c_REVLib_ErrorCode status;
    m_sparkMaxHandle = static_cast<void*>(
        c_Spark_Create(deviceID, static_cast<c_Spark_MotorType>(type),
                       static_cast<c_Spark_SparkModel>(model), &status));

    if (status == c_REVLibError_CantFindFirmware) {
        // Don't throw exception when no firmware is found. It's possible the
        // device is disconnected and we don't want to stop the program if that
        // is the case.
    } else if (status == c_REVLibError_FirmwareTooOld) {
        throw std::runtime_error(
            fmt::format("The firmware version of SPARK #{} is too old and "
                        "needs to be updated.",
                        deviceID));
    } else if (status == c_REVLibError_FirmwareTooNew) {
        throw std::runtime_error(
            fmt::format("The firmware version of SPARK #{} is too new for this "
                        "version of REVLib",
                        deviceID));
    } else if (status == c_REVLibError_SparkFlexBrushedWithoutDock) {
        throw std::runtime_error(
            fmt::format("Cannot set motor type to kBrushed for SPARK #{} "
                        "without a dock connected.",
                        deviceID));
    }
}

SparkLowLevel::~SparkLowLevel() {
    c_Spark_Close(static_cast<c_Spark_handle>(m_sparkMaxHandle));
    c_Spark_Destroy(static_cast<c_Spark_handle>(m_sparkMaxHandle));
}

uint32_t SparkLowLevel::GetFirmwareVersion() {
    bool isDebugBuild;
    return GetFirmwareVersion(isDebugBuild);
}

uint32_t SparkLowLevel::GetFirmwareVersion(bool& isDebugBuild) {
    c_Spark_FirmwareVersion fwVersion;

    c_Spark_GetFirmwareVersion(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                               &fwVersion);

    isDebugBuild = fwVersion.debugBuild != 0;
    return fwVersion.versionRaw;
}

std::string SparkLowLevel::GetFirmwareString() {
    c_Spark_FirmwareVersion fwVersion;

    c_Spark_GetFirmwareVersion(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                               &fwVersion);

    if (fwVersion.debugBuild != 0) {
        return fmt::format("v{}.{}.{} Debug Build", fwVersion.major,
                           fwVersion.minor, fwVersion.build);
    } else {
        return fmt::format("v{}.{}.{}", fwVersion.major, fwVersion.minor,
                           fwVersion.build);
    }
}

std::vector<uint8_t> SparkLowLevel::GetSerialNumber() { return {}; }

int SparkLowLevel::GetDeviceId() const { return m_deviceID; }

SparkLowLevel::MotorType SparkLowLevel::GetMotorType() { return m_motorType; }

void SparkLowLevel::SetPeriodicFrameTimeout(int timeoutMs) {
    c_Spark_SetPeriodicFrameTimeout(
        static_cast<c_Spark_handle>(m_sparkMaxHandle), timeoutMs);
}

void SparkLowLevel::SetCANMaxRetries(int numRetries) {
    c_Spark_SetCANMaxRetries(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                             numRetries);
}

void SparkLowLevel::SetControlFramePeriodMs(int periodMs) {
    c_Spark_SetControlFramePeriod(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                                  periodMs);
}

SparkLowLevel::PeriodicStatus0 SparkLowLevel::GetPeriodicStatus0() {
    c_Spark_PeriodicStatus0 cStatus0;
    c_Spark_GetPeriodicStatus0(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                               &cStatus0);

    PeriodicStatus0 status0;
    status0.appliedOutput = cStatus0.appliedOutput;
    status0.voltage = cStatus0.voltage;
    status0.current = cStatus0.current;
    status0.motorTemperature = cStatus0.motorTemperature;
    status0.hardForwardLimitReached = cStatus0.hardForwardLimitReached;
    status0.hardReverseLimitReached = cStatus0.hardReverseLimitReached;
    status0.softForwardLimitReached = cStatus0.softForwardLimitReached;
    status0.softReverseLimitReached = cStatus0.softReverseLimitReached;
    status0.inverted = cStatus0.inverted;
    status0.primaryHeartbeatLock = cStatus0.primaryHeartbeatLock;
    status0.timestamp = cStatus0.timestamp;

    return status0;
}

static inline float unpackFloat32ToInt32(int32_t val) {
    float f;
    std::memcpy(&f, &val, sizeof(f));
    return f;
}

SparkLowLevel::PeriodicStatus1 SparkLowLevel::GetPeriodicStatus1() {
    c_Spark_PeriodicStatus1 cStatus1;
    c_Spark_GetPeriodicStatus1(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                               &cStatus1);

    PeriodicStatus1 status1;
    status1.otherFault = cStatus1.otherFault;
    status1.motorTypeFault = cStatus1.motorTypeFault;
    status1.sensorFault = cStatus1.sensorFault;
    status1.canFault = cStatus1.canFault;
    status1.temperatureFault = cStatus1.temperatureFault;
    status1.drvFault = cStatus1.drvFault;
    status1.escEepromFault = cStatus1.escEepromFault;
    status1.firmwareFault = cStatus1.firmwareFault;
    status1.brownoutWarning = cStatus1.brownoutWarning;
    status1.overcurrentWarning = cStatus1.overcurrentWarning;
    status1.escEepromWarning = cStatus1.escEepromWarning;
    status1.extEepromWarning = cStatus1.extEepromWarning;
    status1.sensorWarning = cStatus1.sensorWarning;
    status1.stallWarning = cStatus1.stallWarning;
    status1.hasResetWarning = cStatus1.hasResetWarning;
    status1.otherWarning = cStatus1.otherWarning;
    status1.otherStickyFault = cStatus1.otherStickyFault;
    status1.motorTypeStickyFault = cStatus1.motorTypeStickyFault;
    status1.sensorStickyFault = cStatus1.sensorStickyFault;
    status1.canStickyFault = cStatus1.canStickyFault;
    status1.temperatureStickyFault = cStatus1.temperatureStickyFault;
    status1.drvStickyFault = cStatus1.drvStickyFault;
    status1.escEepromStickyFault = cStatus1.escEepromStickyFault;
    status1.firmwareStickyFault = cStatus1.firmwareStickyFault;
    status1.brownoutStickyWarning = cStatus1.brownoutStickyWarning;
    status1.overcurrentStickyWarning = cStatus1.overcurrentStickyWarning;
    status1.escEepromStickyWarning = cStatus1.escEepromStickyWarning;
    status1.extEepromStickyWarning = cStatus1.extEepromStickyWarning;
    status1.sensorStickyWarning = cStatus1.sensorStickyWarning;
    status1.stallStickyWarning = cStatus1.stallStickyWarning;
    status1.hasResetStickyWarning = cStatus1.hasResetStickyWarning;
    status1.otherStickyWarning = cStatus1.otherStickyWarning;
    status1.isFollower = cStatus1.isFollower;
    status1.timestamp = cStatus1.timestamp;

    return status1;
}

SparkLowLevel::PeriodicStatus2 SparkLowLevel::GetPeriodicStatus2() {
    c_Spark_PeriodicStatus2 cStatus2;
    c_Spark_GetPeriodicStatus2(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                               &cStatus2);

    PeriodicStatus2 status2;
    status2.primaryEncoderVelocity = cStatus2.primaryEncoderVelocity;
    status2.primaryEncoderPosition = cStatus2.primaryEncoderPosition;
    status2.timestamp = cStatus2.timestamp;

    return status2;
}

SparkLowLevel::PeriodicStatus3 SparkLowLevel::GetPeriodicStatus3() {
    c_Spark_PeriodicStatus3 cStatus3;
    c_Spark_GetPeriodicStatus3(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                               &cStatus3);

    PeriodicStatus3 status3;
    status3.analogVoltage = cStatus3.analogVoltage;
    status3.analogVelocity = cStatus3.analogVelocity;
    status3.analogPosition = cStatus3.analogPosition;
    status3.timestamp = cStatus3.timestamp;

    return status3;
}

SparkLowLevel::PeriodicStatus4 SparkLowLevel::GetPeriodicStatus4() {
    c_Spark_PeriodicStatus4 cStatus4;
    c_Spark_GetPeriodicStatus4(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                               &cStatus4);

    PeriodicStatus4 status4;
    status4.externalOrAltEncoderVelocity =
        cStatus4.externalOrAltEncoderVelocity;
    status4.externalOrAltEncoderPosition =
        cStatus4.externalOrAltEncoderPosition;
    status4.timestamp = cStatus4.timestamp;

    return status4;
}

SparkLowLevel::PeriodicStatus5 SparkLowLevel::GetPeriodicStatus5() {
    c_Spark_PeriodicStatus5 cStatus5;
    c_Spark_GetPeriodicStatus5(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                               &cStatus5);

    PeriodicStatus5 status5;
    status5.dutyCycleEncoderVelocity = cStatus5.dutyCycleEncoderVelocity;
    status5.dutyCycleEncoderPosition = cStatus5.dutyCycleEncoderPosition;
    status5.timestamp = cStatus5.timestamp;

    return status5;
}

SparkLowLevel::PeriodicStatus6 SparkLowLevel::GetPeriodicStatus6() {
    c_Spark_PeriodicStatus6 cStatus6;
    c_Spark_GetPeriodicStatus6(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                               &cStatus6);

    PeriodicStatus6 status6;
    status6.unadjustedDutyCycle = cStatus6.unadjustedDutyCycle;
    status6.dutyCyclePeriod = cStatus6.dutyCyclePeriod;
    status6.dutyCycleNoSignal = cStatus6.dutyCycleNoSignal;
    status6.timestamp = cStatus6.timestamp;

    return status6;
}

SparkLowLevel::PeriodicStatus7 SparkLowLevel::GetPeriodicStatus7() {
    c_Spark_PeriodicStatus7 cStatus7;
    c_Spark_GetPeriodicStatus7(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                               &cStatus7);

    PeriodicStatus7 status7;
    status7.iAccumulation = cStatus7.iAccumulation;
    status7.timestamp = cStatus7.timestamp;

    return status7;
}

SparkLowLevel::PeriodicStatus8 SparkLowLevel::GetPeriodicStatus8() {
    c_Spark_PeriodicStatus8 cStatus8;
    c_Spark_GetPeriodicStatus8(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                               &cStatus8);

    PeriodicStatus8 status8;
    status8.setpoint = cStatus8.setpoint;
    status8.isAtSetpoint = cStatus8.isAtSetpoint;
    status8.selectedPidSlot =
        static_cast<ClosedLoopSlot>(cStatus8.selectedPidSlot);
    status8.timestamp = cStatus8.timestamp;

    return status8;
}

SparkLowLevel::PeriodicStatus9 SparkLowLevel::GetPeriodicStatus9() {
    c_Spark_PeriodicStatus9 cStatus9;
    c_Spark_GetPeriodicStatus9(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                               &cStatus9);

    PeriodicStatus9 status9;
    status9.maxMotionSetpointPosition = cStatus9.maxmotion_setpoint_position;
    status9.maxMotionSetpointVelocity = cStatus9.maxmotion_setpoint_velocity;
    status9.timestamp = cStatus9.timestamp;

    return status9;
}

rev::REVLibError SparkLowLevel::SetpointCommand(double value, ControlType ctrl,
                                                ClosedLoopSlot pidSlot,
                                                double arbFeedforward,
                                                int arbFFUnits) {
    auto status = c_Spark_SetpointCommand(
        static_cast<c_Spark_handle>(m_sparkMaxHandle), value,
        static_cast<c_Spark_ControlType>(ctrl), static_cast<int>(pidSlot),
        arbFeedforward, arbFFUnits);
    return static_cast<rev::REVLibError>(status);
}

float SparkLowLevel::GetSafeFloat(float f) {
    if (std::isinf(f) || std::isnan(f)) return 0;
    return f;
}

void SparkLowLevel::CreateSimFaultManager() {
    c_SIM_Spark_CreateSimFaultManager(
        static_cast<c_Spark_handle>(m_sparkMaxHandle));
}
}  // namespace rev::spark
