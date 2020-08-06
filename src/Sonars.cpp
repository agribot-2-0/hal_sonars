/*
 * Copyright (C) 2020 Agribot. All rights reserved.
 */
#include "Sonars.h"

Sonars::Sonars(ros::NodeHandle* _nh) : nh(_nh)
{
  if (!vLoadSonars())
    ros::shutdown();
  if (!vCreatePublishers())
    ros::shutdown();
}

Sonars::~Sonars()
{
}

bool Sonars::vLoadSonars()
{
  bool allParametersLoaded = false;
  uint8_t sonar_count = 0;
  std::string sonarField, typeField, topicField, idField;
  int id = 0;
  std::string topic, type;
  SonarDevice sonarDevTemp;
  BallufSonar balluf_sonar;
  MotecSonar motec_sonar;
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
      nh->getParam(idField, id);
      sonarDevTemp.ID = id;
      if (nh->hasParam(topicField) && nh->hasParam(typeField))
      {
        nh->getParam(topicField, topic);
        sonarDevTemp.topic = topic;
        nh->getParam(typeField, type);
        if (!sonarDevTemp.vSetType(type))
        {
          // Error in configuration - wrong type for this sonar
          ROS_ERROR_STREAM("[HAL Sonars] Error in type configuration, input: " << type << " for device sonar" << id);
          break;
        }
        std::pair<uint8_t, SonarDevice> sonarPair(id, sonarDevTemp);
        sonarsMap.insert(sonarPair);
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
    ROS_ERROR_STREAM("[HAL Sonars] Error in configuration, last proper sonar ID read: " << id);
    return false;
  }
  return true;
}

bool Sonars::vCreatePublishers()
{
  for (auto sonar = sonarsMap.begin(); sonar != sonarsMap.end(); ++sonar)
  {
    // Create publisher shared ptr to avoid copying, when inserting into map
    std::shared_ptr<ros::Publisher> pub = std::make_shared<ros::Publisher>();
    // Set up a publisher
    *pub = nh->advertise<sensor_msgs::Range>(sonar->second.topic, 1000);
    // and add Publisher to subscribers map
    std::pair<uint8_t, std::shared_ptr<ros::Publisher>> pubPair(sonar->first, pub);
    sonarsPublishers.insert(pubPair);
  }
  return true;
}

bool Sonars::vPublishSonarData(SonarMeasurment* sonar_data)
{
  // Find the sonar data
  std::unordered_map<uint8_t, SonarDevice>::iterator sonar;
  sonar = sonarsMap.find(sonar_data->u8ID);
  if (sonar == sonarsMap.end())
  {
    ROS_ERROR_STREAM("[HAL Sonars] Attempted to read unregistered sonar with ID: " << sonar_data->u8ID);
    return false;
  }
  else
  {
    sensor_msgs::Range RangeMsg;
    // Standard for every sensor
    RangeMsg.header.stamp = ros::Time::now();
    RangeMsg.header.frame_id = "sonars_link";
    RangeMsg.radiation_type = RangeMsg.ULTRASOUND;
    // from sonar specification
    RangeMsg.min_range = sonar->second.min_range;
    RangeMsg.max_range = sonar->second.max_range;
    RangeMsg.range = sonar_data->fRange;
    // Find the publisher
    std::unordered_map<uint8_t, std::shared_ptr<ros::Publisher>>::iterator pub;
    pub = sonarsPublishers.find(sonar_data->u8ID);
    if (pub == sonarsPublishers.end())
    {
      ROS_ERROR_STREAM("[HAL Sonars] Attempted to publish unregistered sonar with ID: " << sonar_data->u8ID);
      return false;
    }
    else
    {
      pub->second->publish(RangeMsg);
      return true;
    }
  }
}
