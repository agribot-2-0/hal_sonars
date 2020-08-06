/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */
#pragma once
#include <ros/ros.h>
#include <sensor_msgs/Range.h>
#include <memory>
#include <SonarTypes.h>
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
  bool vPublishSonarData(SonarMeasurment* data);
  bool vLoadSonars();
  bool vCreatePublishers();
};
