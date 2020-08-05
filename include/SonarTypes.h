/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */
#pragma once
#include <string>

enum SonarTypeEnum
{
  BALLUF = 0,
  MOTEC = 1
};

struct SonarMeasurment
{
  uint8_t u8ID;
  float fRange;
};

struct BallufSonar
{
  float min_range = 0.3;
  float max_range = 5.0;
};

struct MotecSonar
{
  float min_range = 0.2;
  float max_range = 3.0;
};

struct SonarDevice
{
  int ID;
  std ::string topic;
  SonarTypeEnum type;
  float min_range;
  float max_range;

  void vSetType(BallufSonar balluf_type)
  {
    type = BALLUF;
    min_range = balluf_type.min_range;
    max_range = balluf_type.max_range;
  }

  void vSetType(MotecSonar motec_type)
  {
    type = MOTEC;
    min_range = motec_type.min_range;
    max_range = motec_type.max_range;
  }
};
