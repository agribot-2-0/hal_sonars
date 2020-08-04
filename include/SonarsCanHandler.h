/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */
#pragma once

#include <agribot_common/Can.h>
#include <queue>
#include <cmath>

constexpr uint received_sonars_frame = 0x01;
constexpr uint8_t empty_id = 255;
constexpr float out_of_range_value = 10000;

class SonarsCanHandler
{
  Can _CanHandler;
  std::queue<can_frame> _FramesQueue;
  std::queue<uint8_t> _SonarsID;
  std::queue<float> _SonarsRange;
  int16_t i16GetTwoBytes(uint8_t u8StartBit, can_frame CanFrame);

public:
  explicit SonarsCanHandler(const char* pchCanChannel);
  ~SonarsCanHandler();
  void vReceiveCanFrames();
  // Takes the first pair of sonar ID and range
  // return ID 255 when the queue is empty
  void vReturnSonarRange(uint8_t* id, float* range);
  uint8_t u8ParseCanFrames();
};
