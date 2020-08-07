/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */
#pragma once
#include <hal_sonars/SonarTypes.h>
#include <sensor_msgs/Range.h>
#include <ros/ros.h>
#include <memory>
#include <unordered_map>
#include <vector>

class Sonars
{
private:
  // ROS handlers
  ros::NodeHandle* nh;

  // Sonars map with ID as a key
  std::unordered_map<uint8_t, SonarDevice> sonarsMap;
  // Publishers map with id as a key
  std::unordered_map<uint8_t, std::shared_ptr<ros::Publisher>> sonarsPublishers;

public:
  explicit Sonars(ros::NodeHandle* _nh);
  ~Sonars();
  bool vPublishSonarData(const SonarMeasurment* data);
  bool bLoadSonars();
  bool bCreatePublishers();
};
