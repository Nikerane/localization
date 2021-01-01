#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot 
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    
    // Call the safe_move service and pass the requested joint angles
    if (!client.call(srv))
        ROS_ERROR("Call service: Failed");  
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
	
    for (int i = 0; i < img.height; i++) {
	    for (int j = 0; j < img.step; j++) {
        	int pos_index = i * img.step + j;
        	if (img.data[pos_index] == white_pixel && 
			    img.data[pos_index+1] == white_pixel &&
			    img.data[pos_index+2] == white_pixel) {
				
                
                ROS_INFO("white ball detected ");

			    if((float)j/img.step <= 0.33) {
                    ROS_INFO("Command to turn left");
				    drive_robot(0.0, 0.25);  // turn left               
            	}   
                else if ((float)j/img.step > 0.67) {
                        ROS_INFO("Command to turn right");
				        drive_robot(0.0, -0.25);  // turn right 
			        } 
                else {
                        ROS_INFO("Command to move forward");
				        drive_robot(1.0, 0.0); // move straight
			        }
            
			
		    }
            
	    }
    }

    drive_robot(0.0, 0.0);

    return;
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}