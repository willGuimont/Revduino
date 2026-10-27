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

#include "rev/DetachedEncoder.h"

#include <stdexcept>
#include <string>

#include "rev/CANDetachedEncoderDriver.h"

using namespace rev::detached;

DetachedEncoder::DetachedEncoder(int deviceID, EncoderModel model)
    : DetachedEncoderLowLevel{deviceID, model},
      configAccessor{m_detachedEncoderHandle} {}

double DetachedEncoder::GetPosition() const {
    float tmp;

    c_Detached_GetEncoderPosition(
        static_cast<c_Detached_handle>(m_detachedEncoderHandle), &tmp);
    return tmp;
}

double DetachedEncoder::GetVelocity() const {
    float tmp;
    c_Detached_GetEncoderVelocity(
        static_cast<c_Detached_handle>(m_detachedEncoderHandle), &tmp);
    return tmp;
}

double DetachedEncoder::GetAngle() const {
    float tmp;

    c_Detached_GetEncoderAngle(
        static_cast<c_Detached_handle>(m_detachedEncoderHandle), &tmp);
    return tmp;
}

double DetachedEncoder::GetRawAngle() const {
    float tmp;

    c_Detached_GetEncoderRawAngle(
        static_cast<c_Detached_handle>(m_detachedEncoderHandle), &tmp);
    return tmp;
}

rev::REVLibError DetachedEncoder::SetPosition(double position) {
    c_REVLib_ErrorCode status;
    status = c_Detached_SetEncoderPosition(
        static_cast<c_Detached_handle>(m_detachedEncoderHandle), position);
    return static_cast<rev::REVLibError>(status);
}

rev::REVLibError DetachedEncoder::Configure(DetachedEncoderConfig& config,
                                            rev::ResetMode resetMode) {
    std::string flattenedString = config.Flatten();
    rev::REVLibError status =
        static_cast<rev::REVLibError>(c_Detached_Configure(
            static_cast<c_Detached_handle>(m_detachedEncoderHandle),
            flattenedString.c_str(),
            resetMode == rev::ResetMode::kResetSafeParameters));

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

DetachedEncoder::Faults DetachedEncoder::GetFaults() const {
    uint16_t rawBits;
    c_Detached_GetFaults(
        static_cast<c_Detached_handle>(m_detachedEncoderHandle), &rawBits);

    Faults faults{
        .unexpected{(rawBits & c_Detached_FaultMask_kUnexpected) != 0},
        .hasReset{(rawBits & c_Detached_FaultMask_kHasReset) != 0},
        .canTx{(rawBits & c_Detached_FaultMask_kCanTx) != 0},
        .canRx{(rawBits & c_Detached_FaultMask_kCanRx) != 0},
        .eeprom{(rawBits & c_Detached_FaultMask_kEeprom) != 0},

        .rawBits{rawBits}};
    return faults;
}

DetachedEncoder::Faults DetachedEncoder::GetStickyFaults() const {
    uint16_t rawBits;
    c_Detached_GetStickyFaults(
        static_cast<c_Detached_handle>(m_detachedEncoderHandle), &rawBits);

    Faults faults{
        .unexpected{(rawBits & c_Detached_FaultMask_kUnexpected) != 0},
        .hasReset{(rawBits & c_Detached_FaultMask_kHasReset) != 0},
        .canTx{(rawBits & c_Detached_FaultMask_kCanTx) != 0},
        .canRx{(rawBits & c_Detached_FaultMask_kCanRx) != 0},
        .eeprom{(rawBits & c_Detached_FaultMask_kEeprom) != 0},

        .rawBits{rawBits}};
    return faults;
}

rev::REVLibError DetachedEncoder::ClearFaults() {
    auto status = c_Detached_ClearFaults(
        static_cast<c_Detached_handle>(m_detachedEncoderHandle));
    return static_cast<rev::REVLibError>(status);
}
