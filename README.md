# Speed Detection System
This is an Arduino based project that is designed to average velocity of vehicle.

  In this experimental project, a structure that is alternative to the EDT is implemented.
While “Elektronik Denetleme Sistemi” is used for various applications such as detection of
average speed or detection of driver using seatbelt, “Speed Detection System” is designed to
detect average speed of vehicles inside specific part of the road. The main advantage of the
project is its affordability. It can be used for the roads where speed detection system is required
as an alternative of EDT.

  In the project, there are four components used. The main component is an Arduino UNO
which controls other components and make calculations to assign speeding ticket to drivers.
There are two RFID modules that receives signal from tags to detect vehicles’ arrival. LCD
Shield is used to display data related to current status.

The system contains three main states. In the first state called “Idle State”, the system
waits a vehicle to pass through the first sensor to enter the next state called “Detection State”.
  In this state, sensor detects the identity of the vehicle and assigns a timer for vehicle’s entrance.
When the vehicle passes throughout the second sensor, the system counts for second timer. By
subtracting the second timer from first, it finds the time period spent in lane for the vehicle and
deduces whether the driver will get speeding ticket or not.

  We designed a model that contains toy track to inspect the condition of the system.
There is a circular road that the model vehicles pass through repeatedly. The system works
perfectly when a vehicle passes through the sensor under a particular speed. If the vehicle
exceeds that speed threshold, the sensors become unreliable and may not detect vehicle’s
arrival.

# Bill of Materials List
![image](https://user-images.githubusercontent.com/79690427/210045850-ef6f7e33-0e57-48aa-95cc-fd82e5f93bcb.png)

*In the project, (20x4) version of LCD Shield is used but (16x4) version shown in the table is convenient.*


*Project is designed for EE402 graduation project at Ozyegin University.*
