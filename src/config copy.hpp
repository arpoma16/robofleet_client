#pragma once
#include <amrl_msgs/Localization2DMsg.h>
#include <amrl_msgs/RobofleetStatus.h>
#include <amrl_msgs/RobofleetSubscription.h>
#include <amrl_msgs/VisualizationMsg.h>
#include <nav_msgs/Odometry.h>
#include <sensor_msgs/CompressedImage.h>
#include <sensor_msgs/LaserScan.h>
#include <sensor_msgs/NavSatFix.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/BatteryState.h>
#include <sensor_msgs/Imu.h>
#include <geometry_msgs/Vector3Stamped.h>
#include <dji_osdk_ros/ObstacleInfo.h>
#include <dji_osdk_ros/WaypointV2MissionStatePush.h>
#include <aerialcore_common/ConfigMission.h>
#include <geometry_msgs/PoseStamped.h>
#include <std_msgs/String.h>
#include <std_srvs/SetBool.h>
#include <geometry_msgs/TwistStamped.h>


#include <string>

#include "RosClientNode.hpp"
#include "WebVizConstants.hpp"
#include "topic_config.hpp"

using namespace topic_config;

namespace config {
static const std::string ros_node_name = "robofleet_client";

// URL of robofleet_server instance (ignored in direct mode)
static const std::string host_url = "ws://localhost:8082/?name=uav_13";
// AMRL Robofleet server URL
// static const std::string host_url = "ws://10.0.0.1:8080";

/**
 * Anti-backpressure for normal mode.
 * Uses Websocket PING/PONG protocol to gauge when server has actually received
 * a message. If true, wait for PONGs before sending more messages.
 */
static const bool wait_for_pongs = true;

/**
 * If wait_for_acks, how many more messages to send before waiting for first
 * PONG? This can be set to a value greater than 1 to compensate for network
 * latency and fully saturate available bandwidth, but if it is set too high, it
 * could cause message lag.
 */
static const uint64_t max_queue_before_waiting = 5;

/**
 * Whether to run a Websocket server instead of a client, to bypass the need
 * for a centralized instance of robofleet_server.
 */
static const bool direct_mode = false;
static const quint16 direct_mode_port =
    8082;  // what port to serve on in direct mode
static const quint64 direct_mode_bytes_per_sec =
    2048000;  // avoid network backpressure in direct mode: sets maximum upload
              // speed

/**
  * Controls the verbosity of the logging to standard output
  * 0 - Minimal Logging
  * 1 - Log for subscriptions, new message types, etc.
  * 2 - Full logging, including indication of every received message
  */
static const int verbosity = 2;


/**
 * @brief Configure which topics and types of messages the client will handle.
 *
 * You should use the .configure() method on the RosClientNode, and supply
 * either a SendLocalTopic or a ReceiveRemoteTopic config.
 *
 * To properly integrate with Robofleet, you need to run this client with a
 * ROS namespace representing the robot's name.
 *
 * Absolute topic names begin with a "/"; they will not be prefixed with the
 * current ROS namespace (robot name). Many of your local ROS nodes may publish
 * on absolute-named topics.
 * Most topics must be relative (not begin with "/") on the server side to
 * avoid name collisions between robots.
 *
 * Tips:
 * - When SENDING TO or RECEIVING FROM the server, the topic name should almost
 *   always be relative to avoid name collisions between robots.
 * - When SENDING FROM or RECEIVING TO a local topic, the topic name will often
 *   need to be absolute since many ROS nodes might not use namespaces.
 * - To send to a special webviz topic, make use of webviz_constants.
 */
static void configure_msg_types(RosClientNode& cn) {
  // Read all of the above documentation before modifying

  // must send to status topic to list robot in webviz
  cn.configure(SendLocalTopic<amrl_msgs::RobofleetStatus>()
                   .from("/status")
                   .to(webviz_constants::status_topic)
                   .rate_limit_hz(1));

  // must send to subscriptions topic to receive messages from other robots
  // don't drop or rate limit this topic.
  cn.configure(SendLocalTopic<amrl_msgs::RobofleetSubscription>()
                   .from("/subscriptions")
                   .to(webviz_constants::subscriptions_topic)
                   .no_drop(true));

  // send messages for webviz
  //cn.configure(SendLocalTopic<amrl_msgs::Localization2DMsg>()
  //                 .from("/localization")
  //                 .to(webviz_constants::localization_topic)
  //                 .rate_limit_hz(10)
  //                 .priority(20));
//
  //cn.configure(SendLocalTopic<nav_msgs::Odometry>()
  //                 .from("/odometry/raw")
  //                 .to(webviz_constants::odometry_topic)
  //                 .rate_limit_hz(15)
  //                 .priority(20));
//
  //cn.configure(SendLocalTopic<sensor_msgs::LaserScan>()
  //                 .from("/velodyne_2dscan")
  //                 .to(webviz_constants::lidar_2d_topic)
  //                 .rate_limit_hz(15)
  //                 .priority(2));
//
  //cn.configure(SendLocalTopic<sensor_msgs::LaserScan>()
  //                 .from("/obstacle_scan")
  //                 .to(webviz_constants::obstacle_scan_topic)
  //                 .rate_limit_hz(15)
  //                 .priority(2));
//
  //cn.configure(SendLocalTopic<sensor_msgs::PointCloud2>()
  //                 .from("/velodyne_points")
  //                 .to(webviz_constants::point_cloud_topic)
  //                 .rate_limit_hz(10)
  //                 .priority(1));
//
  //cn.configure(SendLocalTopic<sensor_msgs::CompressedImage>()
  //                 .from("/stereo/left/image_raw/compressed")
  //                 .to(webviz_constants::compressed_image_prefix + "left")
  //                 .rate_limit_hz(10)
  //                 .priority(1));
  //cn.configure(SendLocalTopic<sensor_msgs::CompressedImage>()
  //                 .from("/stereo/right/image_raw/compressed")
  //                 .to(webviz_constants::compressed_image_prefix + "right")
  //                 .rate_limit_hz(10)
  //                 .priority(1));
//
  //cn.configure(SendLocalTopic<amrl_msgs::VisualizationMsg>()
  //                 .from("/visualization")
  //                 .to(webviz_constants::visualization_topic)
  //                 .rate_limit_hz(10)
  //                 .priority(2));
//
  cn.configure(SendLocalTopic<sensor_msgs::NavSatFix>()
                   .from("dji_osdk_ros/gps_position")
                   .to(webviz_constants::localization_topic)
                   .rate_limit_hz(15)
                   .priority(2));


  cn.configure(SendLocalTopic<sensor_msgs::BatteryState>()
                   .from("dji_osdk_ros/battery_state")
                   .to(webviz_constants::battery_topic)
                   .rate_limit_hz(1)
                   .priority(2));

  cn.configure(SendLocalTopic<sensor_msgs::Imu>()
                   .from("dji_osdk_ros/imu")
                   .to(webviz_constants::imu_topic)
                   .rate_limit_hz(15)
                   .priority(2));
  
//  cn.configure(SendLocalTopic<geometry_msgs::Vector3Stamped>()
//                 .from("dji_osdk_ros/velocity")
//                 .to(webviz_constants::speed_topic)
//                 .rate_limit_hz(15)
//                 .priority(2));
                   
  cn.configure(SendLocalTopic<geometry_msgs::Vector3Stamped>()
                 .from("dji_osdk_ros/gimbal_angle")
                 .to(webviz_constants::gimbal_topic)
                 .rate_limit_hz(15)
                 .priority(2));

  cn.configure(SendLocalTopic<dji_osdk_ros::ObstacleInfo>()
                 .from("dji_osdk_ros/obstacle_info")
                 .to(webviz_constants::Obstacle_topic)
                 .rate_limit_hz(15)
                 .priority(2));
    cn.configure(SendLocalTopic<dji_osdk_ros::WaypointV2MissionStatePush>()
                 .from("dji_osdk_ros/waypointV2_mission_state")
                 .to(webviz_constants::mission_state_topic)
                 .rate_limit_hz(15)
                 .priority(2));
  // flight_status:
  //   name: '/dji_osdk_ros/flight_status'
  //   messageType: 'std_msgs/UInt8'
  cn.configure(SendLocalTopic<std_msgs::UInt8>()
              .from("dji_osdk_ros/flight_status")
              .to(webviz_constants::flight_status_topic)
              .rate_limit_hz(15)
              .priority(2));
//
  // receive remote commands
  cn.configure(ReceiveRemoteTopic<geometry_msgs::PoseStamped>()
                   .from("move_base_simple/goal")
                   .to("/move_base_simple/goal"));

  cn.configure(ReceiveRemoteTopic<amrl_msgs::Localization2DMsg>()
                   .from("initialpose")
                   .to("/initialpose"));

  cn.configure(ReceiveLocalService<aerialcore_common::ConfigMission>()
                   .from("configureMission")
                   .to("dji_control/configure_mission"));

  cn.configure(ReceiveLocalService<std_srvs::SetBool>()
                   .from("commandMission")
                   .to("dji_control/start_mission"));

  //cn.configure(ReceiveRemoteTopic<std_srvs/SetBool>()
  //                 .from("CameraFileDownload")
  //                 .to("/dji_control/start_mission"));

  //cn.configure(ReceiveRemoteTopic<std_srvs/SetBool>()
  //                 .from("CameraFileDownload")
  //                 .to("/dji_control/start_mission"));

  // Add additional topics to subscribe and publish here.
}
}  // namespace config
