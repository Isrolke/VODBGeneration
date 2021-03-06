// ** Written By : Sagi Vald **

#include "ros/ros.h"
#include <iostream> // Strings.
#include <image_transport/image_transport.h> // Getting image from camera sensor.
// OpenCV , CV Bridge.
#include <opencv2/highgui/highgui.hpp> 
#include <cv_bridge/cv_bridge.h>
// Gazebo model.
#include "gazebo_msgs/SetModelState.h" 
#include "gazebo_msgs/GetModelState.h" 
#include "gazebo_msgs/GetPhysicsProperties.h"
#include "gazebo_msgs/ModelState.h"
// Math.
#include <math.h>       /* sin */
#include <cstdlib>	/* rand */
// Quaternion.
#include "tf/transform_listener.h"
#include "tf/LinearMath/Matrix3x3.h"
#include "tf/LinearMath/Quaternion.h"
#include "fcl/math/transform.h"
// Globals.
#define PI 3.14159265

int num = 0; // Image counter.
// Tree1 W=2.5 , H=4.5 , Barrel1 W=0.6 , H=1.1 , Cube1 W=1.5 , H =1.5 , Bush1 W=1.1 , H=1.1 , Tree2 W=6.2 , H=6.2 , Tree3 W=2 , H=4.2 , Tree4 W=3.25 , H=5.7
float W=3.25,H=5.7; // Object width and height in meters. 
int v_ang=80; // Camera view angle in degree.
int x_roi,y_roi,nx_roi=1200,ny_roi=900,nx=3200,ny=2400,i,j,k,l;
float r,theta,Theta,phi,px,py,x,y,xc,yc;

// Random float number between interval function.
float RandomFloat(float a, float b) {
    float diff,min,r;
    float random = ((float) rand()) / (float) RAND_MAX;
    if (b>a)
    {
    	diff = b - a;
	min = a;
    }  
    else
    {
      	diff = a - b;
	min = b;
    }
    r = random * diff;
    return min + r;
}
// Random int number between interval function.
float RandomInt(float a, float b) {
    int max,min;
    if (b>a)
    {
    	max = b;
	min = a;
    }  
    else
    {
      	max = a;
	min = b;
    } 
    return min + (rand() % (int)(max - min) + 1);
}


 // Utility function for creating fcl quanterion from roll, pitch and yaw.
 
fcl::Quaternion3f Quanterion3f_from_RPY(float Roll,float Pitch,float Yaw)
{
	tf::Matrix3x3 obs_mat;
	obs_mat.setEulerYPR(Yaw,Pitch,Roll);

	tf::Quaternion q_tf;
	obs_mat.getRotation(q_tf);

	fcl::Quaternion3f q(q_tf.getW(),q_tf.getX(),q_tf.getY(),q_tf.getZ());
	return(q);
}

// Function for catching the camera frame and croping it.
void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{

  x_roi=RandomInt((int)((nx+px)/2)-nx_roi,x),y_roi=RandomInt((int)((ny+py)/2)-ny_roi,y); // Making sure the cropped area will always contain the object.
  xc = x-x_roi; // Using the current system to produce the x rotation and the y rotation.
  yc = y-y_roi;
  cv::Rect roi(x_roi,y_roi,nx_roi,ny_roi);
  cv::Mat im_roi; // Region of interest.
  num = num +1;
  std::ostringstream picname,cr,rect; // Image name string.
  picname << "img.png";
  cr << "crop_im" << num << "_x_"<< xc <<"_y_" << yc << "_px_"<< px <<"_py_" << py << "_1200x900.png"; 
  i=(int)xc;j=(int)yc;k=(int)(xc+px);l=(int)(yc+py); 
  try
  {
    cv::imwrite(picname.str(), cv_bridge::toCvShare(msg, "bgr8")->image); // Saving image.
    im_roi=cv::imread(picname.str()); // Reading image.
      im_roi=im_roi(roi); // Cropping image.
    cv::imwrite(cr.str(), im_roi); // End cropping and saving.
    cv::rectangle(im_roi,cv::Point(i,j),cv::Point(k,l),cv::Scalar(0,0,255),1); // Adding red rectangle bracket.
    cv::imwrite("rect/"+cr.str(), im_roi); // Saving with bracket.
	cv::imshow("view",im_roi); // Showing image.
    cv::waitKey(30); // Waiting time 30 seconds before launching the scenario.
  }
  catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str()); // Common error for the ROS to produce when trying to run the process itself 
  }
}




int main(int argc, char** argv)
   {
	srand(time(NULL)); // Initialize random per run.
	ros::init(argc, argv, "Image_Generation_Visual_Odometery"); // Package name.
	ros::NodeHandle n;
	image_transport::ImageTransport it(n); // Image transport.
	image_transport::Subscriber sub = it.subscribe("SENSORS/CAM/TEST", 1, imageCallback); // Camera subscriber.

	ros::ServiceClient  pp_c = n.serviceClient<gazebo_msgs::GetPhysicsProperties>("/gazebo/get_physics_properties");
	gazebo_msgs::GetPhysicsProperties getphysicsproperties;
	pp_c.call(getphysicsproperties); // getphysicsproperties now holds the current properties.
	// Create your desired model state and call the /gazebo/set_model_state service here.
	gazebo_msgs::GetModelState gcs,gos; // gcs - Get Camera State , gos - Get Obstacle State.
	gcs.request.model_name="flea3";
	gos.request.model_name="obstacle_on_path0"; // object0 - first object not on path, obstacle_on_path0 first object on path.
	ros::ServiceClient gms_c = n.serviceClient<gazebo_msgs::GetModelState>("/gazebo/get_model_state");
	gms_c.call(gcs); // gcs now holds the current 
	state.
	gms_c.call(gos);
  
	// Initializing variabels randomly around the chosen object.
	fcl::Quaternion3f q;
	r = RandomFloat(10,40);Theta=-PI/2;theta=RandomFloat((0*PI)/180,(PI*20)/180);phi=RandomFloat(0,2*PI);
        // Calculating chosen object frame.
	px = (W*nx)/(r*v_ang*(PI/180)); 
        py = (H*ny)/(r*v_ang*(PI/180));
	x = ((nx-px)/2);
	y = ((ny-py)/2);
        
    // We don't want to give the camera speed so initialized to 0.
	geometry_msgs::Twist twist;
	twist.linear.x = 0.0;
	twist.linear.y = 0.0;
	twist.linear.z = 0.0;
	twist.angular.x = 0.0;
	twist.angular.y = 0.0;
	twist.angular.z = 0.0;
	geometry_msgs::Pose pose; // Resetting the usual object to the origin point.
    
	// Setting camera state in gazebo.
	ros::ServiceClient client = n.serviceClient<gazebo_msgs::SetModelState>("/gazebo/set_model_state");
	gazebo_msgs::SetModelState setmodelstate;
	gazebo_msgs::ModelState modelstate;
	modelstate.model_name ="flea3";
	modelstate.pose = pose;
	modelstate.twist = twist;
	setmodelstate.request.model_state=modelstate;
        
	while(ros::ok())
	{
		if (!(num%10)) // Every 10 cropped pictures change camera location.
		{
			r = RandomFloat(10,40);theta=RandomFloat((0*PI)/180,(PI*20)/180);phi=RandomFloat(0,2*PI);   
		}

		// Updating camera position.
		pose.position.x = gos.response.pose.position.x + r*sin(Theta+theta)*cos(phi);
		pose.position.y = gos.response.pose.position.y + r*sin(Theta+theta)*sin(phi);
		pose.position.z = gos.response.pose.position.z + r*cos(Theta+theta) + (H)*0.40;
	
		// Updating camera orientation.
		q = Quanterion3f_from_RPY(0,theta,phi); // theta quentessence.
		pose.orientation.w = q.getW();
		pose.orientation.x = q.getX();
		pose.orientation.y = q.getY();
		pose.orientation.z = q.getZ();
		// Calculating chosen object frame.
		px = (W*nx)/(r*v_ang*(PI/180));
		py = (H*ny)/(r*v_ang*(PI/180));
		x = ((nx-px)/2);
		y = ((ny-py)/2);
		// Updating camera state in gazebo.
		modelstate.model_name ="flea3";
		modelstate.pose = pose;
		modelstate.twist = twist;
		setmodelstate.request.model_state=modelstate;
		client.call(setmodelstate);


		ros::spinOnce();
	} 
	
   }
