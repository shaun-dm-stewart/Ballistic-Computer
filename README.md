# Ballistic Computer

A ballistic computer designed to run on an ESP32 device.

Shooting over long distances, say greater than 1000m requires the shooter to take into account a number of environmental and other conditions.  The shooter needs to know the temperature, atmospheric pressure, relative humidity, wind direction and speed.  Along with these the shooter needs to make allowance for range, Coriolis forces and the vertical angle between shooter and target.  And last but not least the projectile weight, ballistic coefficient, the distance between the scope and the center of the barrel and muzzle velocity.

My aim for this project is to design and build a ballistic computer.  It will read the base data such as bullet weight, ballistic coefficient etc from a micro SD card. It will take the environmental measurements and generate a firing solution based on a given range.  The solution could be delivered as an MOA correction for elevation and windage or perhaps graphically as an aiming point on a mil dot or equivalent reticule.

To generate the data used by this device a separate repository has been created.  BallisticDB is a desktop application that allows the user to generate baseline data.

To process all this information is I believe well beyond the capability of say an Aruino Uno.  There is a device though that just might be up to the task.  It is the Expressif ESP32.

My current thoughts on a bill of materials are:<br>
1 x ESP32                 - Microcontroller<br>
1 x BME280                - Pressure, temperature and relative humidity        
1 x UBLOX NEO-6M          - Latitude and altitude<br>
1 x ITEAD Nextion 5.0"    - Touch Display<br>
1 x MPU-6050              - 6 DOF gyro and compass<br>
1 x WeatherFlow WINDMeter - Anemometer<br>
1 x Buck converter        - To generate 5V for the Nextion<br>
1 x SD card module        - To store the necessary data

The exterior ballistics calculations were influenced by the Gnu Exterior Ballistics library.  This version has been modified to avoid the need for dynamic memory allocation and returns return a firing solution for a specified range.  The solution is calculated based upon the range and the measured environmental conditions and geolocation information.
