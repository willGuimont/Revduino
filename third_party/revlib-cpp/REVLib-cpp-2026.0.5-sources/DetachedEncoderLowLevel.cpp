/*
 * Copyright (c) 2025-2026 REV Robotics
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

#include "rev/DetachedEncoderLowLevel.h"

#include <fmt/format.h>

#include "rev/CANDetachedEncoderDriver.h"

using namespace rev::detached;

DetachedEncoderLowLevel::DetachedEncoderLowLevel(int deviceID,
                                                 EncoderModel model)
    : m_deviceID{deviceID} {
    if (c_Detached_RegisterId(deviceID) == c_REVLibError_DuplicateCANId) {
        throw std::runtime_error(
            fmt::format("A Detached Encoder instance has already been created "
                        "with this device "
                        "ID: {}",
                        deviceID));
    }

    c_REVLib_ErrorCode status;
    m_detachedEncoderHandle = static_cast<void*>(c_Detached_Create(
        deviceID, static_cast<c_Detached_EncoderModel>(model), &status));

    if (status == c_REVLibError_CantFindFirmware) {
        // Don't throw exception when no firmware is found. It's possible the
        // device is disconnected and we don't want to stop the program if that
        // is the case.
    } else if (status == c_REVLibError_FirmwareTooOld) {
        throw std::runtime_error(
            fmt::format("The firmware version of Detached Encoder #{} is too"
                        "old and needs to be updated.",
                        deviceID));
    } else if (status == c_REVLibError_FirmwareTooNew) {
        throw std::runtime_error(
            fmt::format("The firmware version of Detached Encoder #{} is too "
                        "new for this version of REVLib",
                        deviceID));
    } else if (status != c_REVLibError_None) {
        throw std::runtime_error(
            fmt::format("Error ({}) creating Detached Encoder #{}.",
                        static_cast<int>(status), deviceID));
    }
}

DetachedEncoderLowLevel::~DetachedEncoderLowLevel() {
    c_Detached_Close(static_cast<c_Detached_handle>(m_detachedEncoderHandle));
    c_Detached_Destroy(static_cast<c_Detached_handle>(m_detachedEncoderHandle));
}

int DetachedEncoderLowLevel::GetDeviceId() const { return m_deviceID; }

DetachedEncoderLowLevel::EncoderModel DetachedEncoderLowLevel::GetEncoderModel()
    const {
    c_Detached_EncoderModel model;
    c_Detached_GetEncoderModel(
        static_cast<c_Detached_handle>(m_detachedEncoderHandle), &model);
    return static_cast<EncoderModel>(model);
}

DetachedEncoderLowLevel::FirmwareVersion
DetachedEncoderLowLevel::GetFirmwareVersion() const {
    c_Detached_FirmwareVersion cVersion;
    c_Detached_GetFirmwareVersion(
        static_cast<c_Detached_handle>(m_detachedEncoderHandle), &cVersion);

    FirmwareVersion version;
    version.year = cVersion.fwYear;
    version.minor = cVersion.fwMinor;
    version.fix = cVersion.fwFix;
    version.prerelease = cVersion.fwPrerelease;
    version.hardwareMajor = cVersion.hwMajor;
    version.hardwareMinor = cVersion.hwMinor;

    return version;
}

DetachedEncoderLowLevel::PeriodicStatus0
DetachedEncoderLowLevel::GetPeriodicStatus0() const {
    c_Detached_PeriodicStatus0 cStatus0;
    c_Detached_GetPeriodicStatus0(
        static_cast<c_Detached_handle>(m_detachedEncoderHandle), &cStatus0);

    PeriodicStatus0 status0;
    status0.model = static_cast<EncoderModel>(cStatus0.encoderModel);

    return status0;
}

DetachedEncoderLowLevel::PeriodicStatus1
DetachedEncoderLowLevel::GetPeriodicStatus1() const {
    c_Detached_PeriodicStatus1 cStatus1;
    c_Detached_GetPeriodicStatus1(
        static_cast<c_Detached_handle>(m_detachedEncoderHandle), &cStatus1);

    PeriodicStatus1 status1;
    status1.unexpectedFault = cStatus1.unexpectedFault;
    status1.hasResetFault = cStatus1.hasResetFault;
    status1.canTxFault = cStatus1.canTxFault;
    status1.canRxFault = cStatus1.canRxFault;
    status1.eepromFault = cStatus1.eepromFault;

    status1.stickyUnexpectedFault = cStatus1.unexpectedStickyFault;
    status1.stickyHasResetFault = cStatus1.hasResetStickyFault;
    status1.stickyCanTxFault = cStatus1.canTxStickyFault;
    status1.stickyCanRxFault = cStatus1.canRxStickyFault;
    status1.stickyEepromFault = cStatus1.eepromStickyFault;

    return status1;
}

DetachedEncoderLowLevel::PeriodicStatus2
DetachedEncoderLowLevel::GetPeriodicStatus2() const {
    c_Detached_PeriodicStatus2 cStatus2;
    c_Detached_GetPeriodicStatus2(
        static_cast<c_Detached_handle>(m_detachedEncoderHandle), &cStatus2);

    PeriodicStatus2 status2;
    status2.angle = cStatus2.absoluteAngle;
    status2.rawAngle = cStatus2.rawAbsoluteAngle;

    return status2;
}

DetachedEncoderLowLevel::PeriodicStatus3
DetachedEncoderLowLevel::GetPeriodicStatus3() const {
    c_Detached_PeriodicStatus3 cStatus3;
    c_Detached_GetPeriodicStatus3(
        static_cast<c_Detached_handle>(m_detachedEncoderHandle), &cStatus3);

    PeriodicStatus3 status3;
    status3.position = cStatus3.relativePosition;

    return status3;
}

DetachedEncoderLowLevel::PeriodicStatus4
DetachedEncoderLowLevel::GetPeriodicStatus4() const {
    c_Detached_PeriodicStatus4 cStatus4;
    c_Detached_GetPeriodicStatus4(
        static_cast<c_Detached_handle>(m_detachedEncoderHandle), &cStatus4);

    PeriodicStatus4 status4;
    status4.velocity = cStatus4.encoderVelocity;

    return status4;
}

void DetachedEncoderLowLevel::CreateSimFaultManager() {
    c_SIM_Detached_CreateSimFaultManager(
        static_cast<c_Detached_handle>(m_detachedEncoderHandle));
}
