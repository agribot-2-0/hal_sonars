/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */
#include <SonarsCanHandler.h>

constexpr uint addr_mask = 0xFFFFFF;
constexpr uint sonars_frame_CAN_ID = 0x000FFD864;
constexpr uint cons_scale = 1000;
constexpr uint8_t id_byte = 0;
constexpr uint8_t range_byte = 1;

SonarsCanHandler::SonarsCanHandler(const char* pchCanChannel)
{
  _CanHandler.iInitialize(pchCanChannel);
}

void SonarsCanHandler::vReceiveCanFrames()
{
  can_frame message;
  while (_CanHandler.bReceiveFrame(&message))
  {
    _FramesQueue.push(message);
  }
}

SonarsCanHandler::~SonarsCanHandler()
{
  while (!_FramesQueue.empty())
  {
    _FramesQueue.pop();
  }
}

int16_t SonarsCanHandler::i16GetTwoBytes(uint8_t u8StartBit, can_frame CanFrame)
{
  return ((int16_t)CanFrame.data[u8StartBit]) << 8 | ((int16_t)CanFrame.data[u8StartBit + 1]);
}

uint8_t SonarsCanHandler::u8ParseCanFrames()
{
  uint8_t u8Status = 0;
  can_frame message;
  while (!_FramesQueue.empty())
  {
    message = _FramesQueue.front();
    if ((sonars_frame_CAN_ID & addr_mask) == (message.can_id & addr_mask))
    {
      _SonarsID.push(message.data[0]);
      _SonarsRange.push((static_cast<float>(i16GetTwoBytes(range_byte, message)) / cons_scale));
      u8Status |= received_sonars_frame;
    }
    _FramesQueue.pop();
  }
  return u8Status;
}

void SonarsCanHandler::vReturnSonarRange(uint8_t* u8sonars_id, float* fRange)
{
  if (_SonarsID.empty())
  {
    *u8sonars_id = empty_id;
  }
  else
  {
    *u8sonars_id = _SonarsID.front();
    *fRange = _SonarsRange.front();
    _SonarsID.pop();
    _SonarsRange.pop();
  }
}
