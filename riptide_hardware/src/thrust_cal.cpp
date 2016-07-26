#include "ros/ros.h"
#include "riptide_msgs/ThrustStamped.h"
#include "riptide_msgs/PwmStamped.h"

class ThrustCal
{
  private:
    ros::NodeHandle nh;
    ros::Subscriber thrust;
    ros::Publisher pwm;
    riptide_msgs::PwmStamped us;
    int clockwise_propeller(double raw_force);
    int counterclockwise_propeller(double raw_force);

  public:
    ThrustCal();
    void callback(const riptide_msgs::ThrustStamped::ConstPtr& thrust);
    void loop();
};

int main(int argc, char **argv)
{
  ros::init(argc, argv, "thruster_calibration");
  ThrustCal thrust_cal;
  thrust_cal.loop();
}

ThrustCal::ThrustCal() : nh()
{
  thrust = nh.subscribe<riptide_msgs::ThrustStamped>("command/thrust", 1, &ThrustCal::callback, this);
  pwm = nh.advertise<riptide_msgs::PwmStamped>("command/pwm", 1);
}

void ThrustCal::callback(const riptide_msgs::ThrustStamped::ConstPtr& thrust)
{
  us.header.stamp = thrust->header.stamp;

  us.pwm.surge_port_hi = clockwise_propeller(thrust->force.surge_port_hi);
  us.pwm.surge_stbd_hi = counterclockwise_propeller(thrust->force.surge_stbd_hi);
  us.pwm.surge_port_lo = counterclockwise_propeller(thrust->force.surge_port_lo);
  us.pwm.surge_stbd_lo = clockwise_propeller(thrust->force.surge_stbd_lo);
  us.pwm.sway_fwd = counterclockwise_propeller(thrust->force.sway_fwd);
  us.pwm.sway_aft = clockwise_propeller(thrust->force.sway_aft);
  us.pwm.heave_port_fwd = counterclockwise_propeller(thrust->force.heave_port_fwd);
  us.pwm.heave_stbd_fwd = clockwise_propeller(thrust->force.heave_stbd_fwd);
  us.pwm.heave_port_aft = clockwise_propeller(thrust->force.heave_port_aft);
  us.pwm.heave_stbd_aft = counterclockwise_propeller(thrust->force.heave_stbd_aft);

  pwm.publish(us);
}

void ThrustCal::loop()
{
  ros::spin();
}

int ThrustCal::counterclockwise_propeller(double raw_force)
{
  return 1500 + int(raw_force * 14);
}

int ThrustCal::clockwise_propeller(double raw_force)
{
  return 1500 - int(raw_force * 14);
}
