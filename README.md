# MiniMounter
This repository includes the design files of MiniMounter, Matlab codes for auto-focusing, and a graphic user interface (GUI) for controlling.

**MiniMounter_Master_Parts_List.xlsx**  
This file contains all the hardware required to build a MiniMounter for UCLA Miniscope V3.  

**MiniMounter_Parts**  
This folder contains the .step files of the original parts required for machining. *Main_Body_MiniMounter.STEP* shows a full view of MiniMounter.

**MiniMounter_Parts**  
This folder contains the .step files of the modified parts of the Miniscope housing and baseplate compatible with MiniMounter.

**MiniMounter_GUI**  
The controlling GUI uses mexopencv to control Miniscope in Matlab. Although our code contains the compiled mexopencv folder, it needs to be recompiled on a new computer before it can be used. To get started, make sure that you have downloaded or installed the following stuff (for Windows):   
  
● Matlab 2021a (or other versions)  
● OpenCV (http://opencv.org/)   
● mexopencv (https://github.com/kyamagu/mexopencv) **The version of mexopencv and OpenCV should match.** We use OpenCV 3.4.1 in our cases.  
● Visual Studio 2017 (as compiler)  

Steps:  
1. Open Matlab and add the extracted mexopencv folder to the path.  
2. Run `mex -setup` and choose VS2017 as the compiler.  
3. Run `mexopencv.make` for compiling.  
4. Run `UnitTest` to make sure you have compiled mexopencv successfully.  

If you meet any problems, check the instruction of mexopencv for help: https://kyamagu.github.io/mexopencv/.  
  
Download Thorlabs APT Software for motor controlling: https://www.thorlabschina.cn/newgrouppage9.cfm?objectgroup_id=9019. Once the software is installed, the control functions in Matlab can be used directly.  

The file *MrScopev2.mlapp* is the GUI for controlling MiniMounter and Miniscope, and the file *real_time.m* shows a window for real-time Miniscope images and evaluation indexes.  
