#pragma once
#include <ros/ros.h>
#include <sensor_msgs/Range.h>
#include <SonarTypes.h>
#include <unordered_map>
#include <vector>

class Sonars
{
private:
  // ROS handlers
  ros::NodeHandle* nh;

  // Sonars map
  std::vector<SonarDevice> sonarsList;
  // Publishers map with id as a key
  std::unordered_map<uint8_t, std::shared_ptr<ros::Publisher>> sonarsPublishers;

public:
  Sonars(ros::NodeHandle* _nh);
  ~Sonars();
  void vPublishSonarData(SonarMeasurment data);
  void vLoadSonars();
  void vCreatePublishers();
};