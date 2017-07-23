## This repository contains the code for predicting steering using pattern matching. The steering values generated from here can be used in ROS or Arduino to control the robot.

# There are two different sample patterns in the 'data' folder- p1.png and p2.png. To test the code take print out of any pattern and use it.(Note: These patterns are completely random and don't mean anything except testing patterns)

## Pre-requisites
OpenCV 2.x (preferred 2.4.13, or might work on 3.x as well)

## Compilation and Usage
First of all 'cd' to the repo's root directory and run:

$ chmod +x run.sh

Then follow any of the steps:
## For testing on Camera
# ./run.sh path_of_pattern camera_index
$ ./run.sh data/p1.png 0

## For testing on Videos
# ./run.sh path_of_pattern path_to_video
$ ./run.sh data/p1.png video_path

#(Note: The circle in the red bar on the top provides the filtered steering value)


