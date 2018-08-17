#include "dyros_jet_controller/mujoco_interface.h"

namespace dyros_jet_controller {

mujoco_interface::mujoco_interface(ros::NodeHandle &nh, double Hz):
    ControlBase(nh,Hz), rate_(Hz), dyn_hz(Hz)
{
    simulation_running_= true;
    mujoco_joint_set_pub_=nh.advertise<sensor_msgs::JointState>("/mujoco_ros_interface/joint_set",1);
    mujoco_sim_command_pub_=nh.advertise<std_msgs::String>("/mujoco_ros_interface/sim_command_con2sim",100);
    mujoco_sim_command_sub_=nh.subscribe("/mujoco_ros_interface/sim_command_sim2con",100,&mujoco_interface::simCommandCallback,this);

    mujoco_joint_state_sub_ = nh.subscribe("/mujoco_ros_interface/joint_states",1,&mujoco_interface::jointStateCallback,this,ros::TransportHints().tcpNoDelay(true));
    mujoco_sim_time_sub_ = nh.subscribe("/mujoco_ros_interface/sim_time",1,&mujoco_interface::simTimeCallback,this,ros::TransportHints().tcpNoDelay(true));
    mujoco_sensor_state_sub_=nh.subscribe("/mujoco_ros_interface/sensor_states",1,&mujoco_interface::sensorStateCallback,this,ros::TransportHints().tcpNoDelay(true));

    mujoco_joint_set_msg_.position.resize(total_dof_);
    mujoco_joint_set_msg_.effort.resize(total_dof_);
    mujoco_joint_set_msg_.name.resize(total_dof_);
;
    for(int i=0; i<total_dof_; i++)
    {
      mujoco_joint_set_msg_.name[i] = DyrosJetModel::JOINT_NAME[i];
    }
    mujoco_sim_time =0.0;
    simready();
}

void mujoco_interface::simready()
{/*
  ros::Rate poll_rate(100);
  while(!mujoco_ready &&ros::ok())
  {
    ros::spinOnce();
    poll_rate.sleep();
  }
  mujoco_ready=false;
*/
/*
  while(!mujoco_init_receive &&ros::ok()){
    ros::spinOnce();
    poll_rate.sleep();
  }
  mujoco_init_receive=false;*/
}

void mujoco_interface::simTimeCallback(const std_msgs::Float32ConstPtr &msg)
{
  mujoco_sim_time = msg->data;
//  control_time_ = mujoco_sim_time;
}

void mujoco_interface::jointStateCallback(const sensor_msgs::JointStateConstPtr &msg)
{

    for(int i=0;i<total_dof_-4;i++)
    {
        for(int j=0; j<msg->name.size();j++){
            if(DyrosJetModel::JOINT_NAME[i] == msg->name[j].data())
            {
                q_(i) = msg->position[j];
             //   q_virtual_(i+6) = msg->position[j];
                q_dot_(i) = msg->velocity[j];
             //   q_dot_virtual_(i+6) = msg->velocity[j];
                torque_(i) = msg->effort[j];
            }
        }

        joint_name_mj[i] = msg->name[i+6].data();
    }
    std::cout << "q_" << q_ << std::endl;
    //virtual joint
    for(int i=0;i<6;i++){
    //  q_virtual_(i) = msg->position[i];
    //  q_dot_virtual_(i) = msg->velocity[i];
    }
}


void mujoco_interface::sensorStateCallback(const mujoco_ros_msgs::SensorStateConstPtr &msg)
{
    for(int i=0;i<msg->sensor.size();i++){
        if(msg->sensor[i].name=="Gyro_Pelvis_IMU"){
            for(int j=0;j<3;j++){
            //    q_dot_virtual_(j+3)=msg->sensor[i].data[j];
            }

        }

    }
}

void mujoco_interface::simCommandCallback(const std_msgs::StringConstPtr &msg)
{

  std::string buf;
  buf = msg->data;


  ROS_INFO("CB from simulator : %s", buf.c_str());
  if(buf == "RESET"){
    parameterInitialize();
    mujoco_sim_last_time = 0.0;

    mujoco_ready=true;

    std_msgs::String rst_msg_;
    rst_msg_.data="RESET";
    mujoco_sim_command_pub_.publish(rst_msg_);

    ros::Rate poll_rate(100);
    while(!mujoco_init_receive &&ros::ok()){
      ros::spinOnce();
      poll_rate.sleep();
    }
    mujoco_init_receive=false;
  }


  if(buf=="INIT"){
    mujoco_init_receive =true;
    std_msgs::String rst_msg_;
    rst_msg_.data="INIT";
    mujoco_sim_command_pub_.publish(rst_msg_);
  }

}

void mujoco_interface::update()
{
    ControlBase::update();
}

void mujoco_interface::compute()
{
    ControlBase::compute();
}

void mujoco_interface::writeDevice()
{
  for(int i=0;i<total_dof_;i++) {
    mujoco_joint_set_msg_.position[i] = desired_q_(i);}
  mujoco_joint_set_pub_.publish(mujoco_joint_set_msg_);
}

void mujoco_interface::wait()
{/*
    ros::Rate poll_rate(200);

    while((mujoco_sim_time<(mujoco_sim_last_time+1.0/dyn_hz))&&ros::ok()){
        //ROS_INFO("WAIT WHILE");
        ros::spinOnce();
        poll_rate.sleep();

    }
*/

}


}
