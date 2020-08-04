/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */
#pragma once
struct BallufSonar
{
  float min_range = 0.3;
  float max_range = 5.0;
  float range;
};

struct MotecSonar
{
  float min_range = 0.2;
  float max_range = 3.0;
  float range;
};

struct SonarMeasurment
{
  uint8_t u8ID;
  float fRange;
};
