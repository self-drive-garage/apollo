/******************************************************************************
 * Copyright 2021 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#pragma once

#include "modules/common_msgs/sensor_msgs/nano_radar.pb.h"

#include "modules/drivers/canbus/can_comm/protocol_data.h"

namespace apollo {
namespace drivers {
namespace nano_radar {

using apollo::drivers::NanoRadar;

class SoftwareVersion700
    : public apollo::drivers::canbus::ProtocolData<NanoRadar> {
 public:
  static const uint32_t ID;
  SoftwareVersion700();
  void Parse(const std::uint8_t* bytes, int32_t length,
             NanoRadar* nano_radar) const override;

 private:
  int soft_major_release(const std::uint8_t* bytes, int32_t length) const;

  int soft_minor_release(const std::uint8_t* bytes, int32_t length) const;

  int soft_patch_level(const std::uint8_t* bytes, int32_t length) const;
};

}  // namespace nano_radar
}  // namespace drivers
}  // namespace apollo
