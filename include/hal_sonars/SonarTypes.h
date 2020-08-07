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
  std ::string link_name;
  SonarTypeEnum type;
  float min_range;
  float max_range;
  bool bCreateAndWriteLinkName(std::string source)
  {
    bool first_char = true;
    link_name.clear();
    if (!source.empty())
    {
      for (auto& str_elem : source)
      {
        if (!first_char)
        {
          if (str_elem == '/')
          {
            link_name = link_name + "_";
          }
          else
          {
            link_name = link_name + str_elem;
          }
        }
        first_char = false;
      }
      link_name = link_name + "_link";
      return true;
    }
    return false;
  }
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
