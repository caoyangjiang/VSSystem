# VSSystem

Video Streaming System (VSSystem) is designed for real-time survalliance purpose. Any number of cameras (using uvc drivers) are supported. Unlike conventional survalliance
system where minimal post-processing is allowed, this system implements various post-processing such as image denoising and cpu/gpu-based brightness/constrast adjustment. Other interesting futures such as face-detection and intruder detection can be added. On the implementationt side, the input images are captured via libv4l2, encoded with cpu-based x264 or gpu-based nvencoder, packetized using RTP and send out via boost:asio. 

How to compile:
==============

**For LINUX**

~~~~~~~~~~~~~~~~~~~~~
    mkdir build

    cd build

    cmake ..

    make -j$(nproc)
~~~~~~~~~~~~~~~~~~~~~

**WINDOWS Not Supported**
 

**Mac/Xcode Not Supported**

<br />


 
