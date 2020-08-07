/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */
#pragma once
#include <string>

enum class SonarTypeEnum
{
  BALLUF = 0,
  BOSH = 1
};

struct SonarMeasurment
{
  uint8_t u8ID;
  float fRange;
};

struct BallufSonar
{
  static constexpr float min_range{ 0.3 };
  static constexpr float max_range{ 5.0 };
};

struct BoshSonar
{
  static constexpr float min_range{ 0.2 };
  static constexpr float max_range{ 3.0 };
};

struct SonarDevice
{
  int ID;
  std ::string topic;
  SonarTypeEnum type;
  float min_range;
  float max_range;
  bool vSetType(std::string sonar_type)
  {
    if (sonar_type == "balluf")
    {
      type = SonarTypeEnum::BALLUF;
      min_range = BallufSonar::min_range;
      max_range = BallufSonar::max_range;
      return true;
    }
    else if (sonar_type == "bosh")
    {
      type = SonarTypeEnum::BOSH;
      min_range = BoshSonar::min_range;
      max_range = BoshSonar::max_range;
      return true;
    }
    else
    {
      return false;
    }
  }
};
