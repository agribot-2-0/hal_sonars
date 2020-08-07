/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */
#include "hal_sonars/Sonars.h"

Sonars::Sonars(ros::NodeHandle* _nh) : nh(_nh)
{
}

Sonars::~Sonars()
{
}

bool Sonars::bLoadSonars()
{
  bool allParametersLoaded = false;
  uint8_t sonar_count = 0;
  std::string sonarField, typeField, topicField, idField;
  std::string type;
  SonarDevice sonarDevTemp;
  ROS_INFO_STREAM("[HAL Sonars] Loading sonars");
  // Check all sonars parameters, starting from 0
  while (!allParametersLoaded)
  {
    sonarField = "/sonars_node/sonar" + std::to_string(sonar_count);
    idField = sonarField + "_ID";
    topicField = sonarField + "_topic";
    typeField = sonarField + "_type";
    if (nh->hasParam(idField))
    {
      nh->getParam(idField, sonarDevTemp.ID);
      if (nh->hasParam(topicField) && nh->hasParam(typeField))
      {
        nh->getParam(topicField, sonarDevTemp.topic);
        nh->getParam(typeField, type);
        if (!sonarDevTemp.vSetType(type))
        {
          // Error in configuration - wrong type for this sonar
          ROS_ERROR_STREAM("[HAL Sonars] Error in type configuration, input: " << type << " for device sonar"
                                                                               << sonarDevTemp.ID);
          break;
        }
        sonarsMap.insert({ sonarDevTemp.ID, sonarDevTemp });
        ROS_INFO_STREAM("[HAL Sonars] ID: " << sonarDevTemp.ID << " Topic: " << sonarDevTemp.topic
                                            << " Type: " << static_cast<int>(sonarDevTemp.type) << "\n min range: "
                                            << sonarDevTemp.min_range << "max range: " << sonarDevTemp.max_range);
      }
      else
      {
        break;
      }
      ++sonar_count;
    }
    else
    {
      allParametersLoaded = true;
    }
  }
  if (!allParametersLoaded)
  {
    // Error in configuration - no threshold for this sonar
    ROS_ERROR_STREAM("[HAL Sonars] Error in configuration, last proper sonar ID read: sonar"
                     << static_cast<int>(sonar_count));
    return false;
  }
  return true;
}

bool Sonars::bCreatePublishers()
{
  for (const auto& [id, sonar] : sonarsMap)
  {
    // Create publisher shared ptr to avoid copying, when inserting into map
    std::shared_ptr<ros::Publisher> pub = std::make_shared<ros::Publisher>();
    // Set up a publisher
    *pub = nh->advertise<sensor_msgs::Range>(sonar.topic, 1000);
    // and add Publisher to publishers map
    sonarsPublishers.insert({ id, pub });
  }
  return true;
}

bool Sonars::vPublishSonarData(const SonarMeasurment* sonar_data)
{
  // Find the publisher
  // first is ID, second pub shared ptr
  auto pub = sonarsPublishers.find(sonar_data->u8ID);
  if (pub == sonarsPublishers.end())
  {
    ROS_ERROR_STREAM(
        "[HAL Sonars] Attempted to publish unregistered sonar with ID: " << static_cast<int>(sonar_data->u8ID));
    return false;
  }
  // Find the sonar data
  // first is ID, second is SonarDevice object
  auto sonar = sonarsMap.find(sonar_data->u8ID);
  if (sonar == sonarsMap.end())
  {
    ROS_ERROR_STREAM(
        "[HAL Sonars] Attempted to read unregistered sonar with ID: " << static_cast<int>(sonar_data->u8ID));
    return false;
  }
  sensor_msgs::Range RangeMsg;
  // Standard for every sensor
  RangeMsg.header.stamp = ros::Time::now();
  RangeMsg.header.frame_id = "sonars_link";
  RangeMsg.radiation_type = RangeMsg.ULTRASOUND;
  // from sonar specification
  RangeMsg.min_range = sonar->second.min_range;
  RangeMsg.max_range = sonar->second.max_range;
  RangeMsg.range = sonar_data->fRange;
  pub->second->publish(RangeMsg);
  return true;
}
