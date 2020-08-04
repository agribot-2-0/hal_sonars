/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */
#pragma once

#include <agribot_common/Can.h>
#include <queue>
#include <cmath>
#include <SonarTypes.h>

class SonarsCanHandler
{
  static constexpr uint addr_mask = 0xFFFFFF;
  static constexpr uint sonars_frame_CAN_ID = 0x000FFD864;
  static constexpr uint cons_scale = 1000;
  static constexpr uint8_t id_byte = 0;
  static constexpr uint8_t range_byte = 1;
  Can _CanHandler;
  std::queue<can_frame> _FramesQueue;
  std::queue<SonarMeasurment> _SonarsReading;
  int16_t i16GetTwoBytes(uint8_t u8StartBit, can_frame CanFrame);

public:
  static constexpr uint8_t empty_id = 255;
  static constexpr float out_of_range_value = 10000;
  static constexpr uint received_sonars_frame = 0x01;
  explicit SonarsCanHandler(const char* pchCanChannel);
  ~SonarsCanHandler();
  void vReceiveCanFrames();
  // Takes the first pair of sonar ID and range
  // return ID 255 when the queue is empty
  void vReturnSonarRange(SonarMeasurment* Measurement);
  uint8_t u8ParseCanFrames();
};
