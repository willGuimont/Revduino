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

#include "rev/SparkBase.h"

#include <string>

#include <fmt/format.h>
#include <frc/Errors.h>

#include "rev/CANSparkDriver.h"
#include "rev/SparkAbsoluteEncoder.h"
#include "rev/SparkAnalogSensor.h"
#include "rev/SparkClosedLoopController.h"
#include "rev/SparkLimitSwitch.h"
#include "rev/SparkRelativeEncoder.h"
#include "rev/SparkSoftLimit.h"
#include "rev/config/SparkBaseConfig.h"

using namespace rev::spark;

SparkBase::SparkBase(int deviceID, MotorType type, SparkModel model)
    : SparkLowLevel(deviceID, type, model),
      m_RelativeEncoder{*this},
      m_AnalogSensor{*this},
      m_AbsoluteEncoder{*this},
      m_ClosedLoopController{*this},
      m_ForwardLimitSwitch{*this, SparkLimitSwitch::Direction::kForward},
      m_ReverseLimitSwitch{*this, SparkLimitSwitch::Direction::kReverse},
      m_ForwardSoftLimit{*this, SparkSoftLimit::Direction::kForward},
      m_ReverseSoftLimit{*this, SparkSoftLimit::Direction::kReverse} {}

void SparkBase::Set(double speed) {
    // Only for 'get' API
    m_setpoint = speed;
    SetpointCommand(speed, ControlType::kDutyCycle);
}

void SparkBase::SetVoltage(units::volt_t output) {
    // simple conversion too keep Get() trivial
    // Use GetAppliedOutput() instead of Get() for
    // actual applied duty cycle
    double dOutput = units::unit_cast<double>(output);
    m_setpoint = dOutput / 12.0;
    SetpointCommand(dOutput, ControlType::kVoltage);
}

double SparkBase::Get() const { return m_setpoint; }

void SparkBase::SetInverted(bool isInverted) {
    FRC_ReportError(frc::warn::Warning,
                    "The inversion setting should be set via a SparkMaxConfig "
                    "or a SparkFlexConfig object");
    c_Spark_SetInverted(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                        isInverted);
}

bool SparkBase::GetInverted() const {
    FRC_ReportError(frc::warn::Warning,
                    "The inversion setting should be retrieved via the "
                    "configAccessor field of a SparkMax or SparkFlex object");
    uint8_t inverted;
    c_Spark_GetInverted(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                        &inverted);
    return inverted ? true : false;
}

void SparkBase::Disable() { Set(0); }

void SparkBase::StopMotor() { Set(0); }

rev::REVLibError SparkBase::Configure(SparkBaseConfig& config,
                                      ResetMode resetMode,
                                      PersistMode persistMode) {
    return Configure(config, static_cast<rev::ResetMode>(resetMode),
                     static_cast<rev::PersistMode>(persistMode));
}

rev::REVLibError SparkBase::Configure(SparkBaseConfig& config,
                                      rev::ResetMode resetMode,
                                      rev::PersistMode persistMode) {
    std::string flattenedString = config.Flatten();
    rev::REVLibError status = static_cast<rev::REVLibError>(c_Spark_Configure(
        static_cast<c_Spark_handle>(m_sparkMaxHandle), flattenedString.c_str(),
        resetMode == rev::ResetMode::kResetSafeParameters,
        persistMode == rev::PersistMode::kPersistParameters));

    if (status != REVLibError::kOk) {
        // Check if fatal error
        if (status == REVLibError::kTimeout ||
            status == REVLibError::kCannotPersistParametersWhileEnabled) {
            return status;
        }

        throw std::runtime_error(
            c_REVLib_ErrorFromCode(static_cast<c_REVLib_ErrorCode>(status)));
    }
    return status;
}

rev::REVLibError SparkBase::ConfigureAsync(SparkBaseConfig& config,
                                           ResetMode resetMode,
                                           PersistMode persistMode) {
    return ConfigureAsync(config, static_cast<rev::ResetMode>(resetMode),
                          static_cast<rev::PersistMode>(persistMode));
}

rev::REVLibError SparkBase::ConfigureAsync(SparkBaseConfig& config,
                                           rev::ResetMode resetMode,
                                           rev::PersistMode persistMode) {
    std::string flattenedString = config.Flatten();
    rev::REVLibError status =
        static_cast<rev::REVLibError>(c_Spark_ConfigureAsync(
            static_cast<c_Spark_handle>(m_sparkMaxHandle),
            flattenedString.c_str(),
            resetMode == rev::ResetMode::kResetSafeParameters,
            persistMode == rev::PersistMode::kPersistParameters));

    return status;
}

SparkRelativeEncoder& SparkBase::GetEncoder() {
    m_relativeEncoderCreated.exchange(true);
    return m_RelativeEncoder;
}

SparkAnalogSensor& SparkBase::GetAnalog() {
    m_analogSensorCreated.exchange(true);
    return m_AnalogSensor;
}

SparkAbsoluteEncoder& SparkBase::GetAbsoluteEncoder() {
    m_absoluteEncoderCreated.exchange(true);
    return m_AbsoluteEncoder;
}

SparkClosedLoopController& SparkBase::GetClosedLoopController() {
    m_closedLoopControllerCreated.exchange(true);
    return m_ClosedLoopController;
}

SparkLimitSwitch& SparkBase::GetForwardLimitSwitch() {
    m_forwardLimitSwitchCreated.exchange(true);
    return m_ForwardLimitSwitch;
}

SparkLimitSwitch& SparkBase::GetReverseLimitSwitch() {
    m_reverseLimitSwitchCreated.exchange(true);
    return m_ReverseLimitSwitch;
}

SparkSoftLimit& SparkBase::GetForwardSoftLimit() {
    m_forwardSoftLimitCreated.exchange(true);
    return m_ForwardSoftLimit;
}

SparkSoftLimit& SparkBase::GetReverseSoftLimit() {
    m_reverseSoftLimitCreated.exchange(true);
    return m_ReverseSoftLimit;
}

uint8_t SparkBase::GetMotorInterface() {
    uint8_t motorInterface;
    c_Spark_GetMotorInterface(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                              &motorInterface);

    return motorInterface;
}

SparkBase::SparkModel SparkBase::GetSparkModel() {
    c_Spark_SparkModel model;
    c_Spark_GetSparkModel(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                          &model);
    return static_cast<SparkBase::SparkModel>(model);
}

rev::REVLibError SparkBase::ResumeFollowerMode() {
    auto status = c_Spark_StartFollowerMode(
        static_cast<c_Spark_handle>(m_sparkMaxHandle));
    return static_cast<rev::REVLibError>(status);
}

rev::REVLibError SparkBase::ResumeFollowerModeAsync() {
    auto status = c_Spark_StartFollowerModeAsync(
        static_cast<c_Spark_handle>(m_sparkMaxHandle));
    return static_cast<rev::REVLibError>(status);
}

rev::REVLibError SparkBase::PauseFollowerMode() {
    auto status =
        c_Spark_StopFollowerMode(static_cast<c_Spark_handle>(m_sparkMaxHandle));
    return static_cast<rev::REVLibError>(status);
}

rev::REVLibError SparkBase::PauseFollowerModeAsync() {
    auto status = c_Spark_StopFollowerModeAsync(
        static_cast<c_Spark_handle>(m_sparkMaxHandle));
    return static_cast<rev::REVLibError>(status);
}

bool SparkBase::IsFollower() {
    uint8_t isFollower;
    c_Spark_IsFollower(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                       &isFollower);
    return isFollower != 0;
}

bool SparkBase::HasActiveFault() {
    uint16_t faults;
    c_Spark_GetFaults(static_cast<c_Spark_handle>(m_sparkMaxHandle), &faults);
    return faults != 0;
}

bool SparkBase::HasStickyFault() {
    uint16_t faults;
    c_Spark_GetStickyFaults(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                            &faults);
    return faults != 0;
}

bool SparkBase::HasActiveWarning() {
    uint16_t warnings;
    c_Spark_GetWarnings(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                        &warnings);
    return warnings != 0;
}

bool SparkBase::HasStickyWarning() {
    uint16_t warnings;
    c_Spark_GetStickyWarnings(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                              &warnings);
    return warnings != 0;
}

SparkBase::Faults SparkBase::GetFaults() {
    uint16_t rawBits;
    c_Spark_GetFaults(static_cast<c_Spark_handle>(m_sparkMaxHandle), &rawBits);
    Faults faults{rawBits};
    return faults;
}

SparkBase::Faults SparkBase::GetStickyFaults() {
    uint16_t rawBits;
    c_Spark_GetStickyFaults(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                            &rawBits);
    Faults faults{rawBits};
    return faults;
}

SparkBase::Warnings SparkBase::GetWarnings() {
    uint16_t rawBits;
    c_Spark_GetWarnings(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                        &rawBits);
    Warnings warnings{rawBits};
    return warnings;
}

SparkBase::Warnings SparkBase::GetStickyWarnings() {
    uint16_t rawBits;
    c_Spark_GetStickyWarnings(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                              &rawBits);
    Warnings warnings{rawBits};
    return warnings;
}

double SparkBase::GetBusVoltage() {
    float tmp;
    c_Spark_GetBusVoltage(static_cast<c_Spark_handle>(m_sparkMaxHandle), &tmp);
    return static_cast<double>(tmp);
}

double SparkBase::GetAppliedOutput() {
    float tmp;
    c_Spark_GetAppliedOutput(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                             &tmp);
    return static_cast<double>(tmp);
}

double SparkBase::GetOutputCurrent() {
    float tmp;
    c_Spark_GetOutputCurrent(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                             &tmp);
    return static_cast<double>(tmp);
}

double SparkBase::GetMotorTemperature() {
    float tmp;
    c_Spark_GetMotorTemperature(static_cast<c_Spark_handle>(m_sparkMaxHandle),
                                &tmp);
    return static_cast<double>(tmp);
}

rev::REVLibError SparkBase::ClearFaults() {
    auto status =
        c_Spark_ClearFaults(static_cast<c_Spark_handle>(m_sparkMaxHandle));
    return static_cast<rev::REVLibError>(status);
}

rev::REVLibError SparkBase::SetCANTimeout(int milliseconds) {
    auto status = c_Spark_SetCANTimeout(
        static_cast<c_Spark_handle>(m_sparkMaxHandle), milliseconds);
    return static_cast<rev::REVLibError>(status);
}

rev::REVLibError SparkBase::GetLastError() {
    return static_cast<rev::REVLibError>(
        c_Spark_GetLastError(static_cast<c_Spark_handle>(m_sparkMaxHandle)));
}

// Used by the HIL tester
SparkRelativeEncoder SparkBase::GetEncoderEvenIfAlreadyCreated() {
    return GetEncoder();
}
