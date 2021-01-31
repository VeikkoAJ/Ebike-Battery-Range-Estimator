# Ebike-Battery-Range-Estimator

Arduino based Electric bicycle control unit. 

Main features:
* Headlight brightness control
* Intelligent battery capacity monitoring
* Range estimation
* Vertical display
* Cool headlight and display animation on startup

Main motivation for this project was a need for a more precise battery capacity monitor. Basic capacity monitors use a linear voltage model to estimate battery charge state. More complex system was needed in order to compensate for battery internal resistance voltage sag and non linearity of cell voltages.  
Additional challenge this project had was the LCD display. The LCD display had to be mounted vertically on the frame due to frame width limitations, making conventional methods of using the display impossible. This was solved with custom characters which were turned 90 degrees sideways.



## Usage
*It is recommended to fully customize this program, as most of this project was made with parts at hand thus containing unnecessary part (such as GPS module instead of Hall sensors).*

**Base voltage of the Ebike is 48 V**, meaning the voltage divider resistors are sized for 5 V pin voltage at 59 V battery voltage, so it probably will not work with 36 V or 24 V systems, or at least the voltage precision will be poor.

**State Of Charge (SOC) estimation**

State of charge is estimated in two modes: standby voltage and active current. Standby voltage is used to estimate the battery Depth of Discharge (DOD). 

As li-ion batteries do not have a linear voltage to charge connection, a custom lookup table was created. The lookup table is based on a LG 18650 M26 2600mAh cell and its discharge graph. Discharge graph and the fitted plot can be found [here](https://github.com/VeikkoAJ/Ebike-Battery-Range-Estimator/blob/main/README.md#sources). The fitted function was used to generate a lookup table, with almost even charge distances.

The system automatically switches to active current mode and back when a current threshold of 0.2 A is passed. Active current mode measures current and time intervals to integrate used charge. Used charge is then added to DOD value. Measuring the battery voltage under heavy load (+20 A) gives and inaccurate reading, as the battery internal resistance sags the voltage. This is a common problem with simpler voltage only battery capacity monitors.

**LCD Display**

The LCD display is mounted vertically on the bike frame because the frame was not wide enough for a 16\*2 display to be mounted horizontally. Therefore, the display is only capable of showing 2-digit numbers per row and unit definitions have to be painted next to the lcd. Upside of mounting the display vertically is that a proper battery bar can be displayed on it. 
8 extra custom characters (numbers 2,4,5,6,7,8,9 and a "half bar") are saved to the LCD display. Because of the LCD displays memory limit on custom symbols (8 custom characters), "0","1", "3" are reused "o", "-", "m". "Half bar" is used to increase battery capacity bar resolution to 5%. to Custom symbols can be found at [CustomChar.c](https://github.com/VeikkoAJ/Ebike-Battery-Range-Estimator/blob/main/EbikeBatteryRangeEstimator/CustomChar.c)

<img src="https://github.com/VeikkoAJ/Ebike-Battery-Range-Estimator/blob/main/pics/LCD_interface_scaled.jpg" width='400'> 

*Still missing decals to cover wiring mess :)* 

**GPS**

GPS module was chosen mainly because of I already had one from an old project. A hall sensor would be better suited for measuring the bike velocity as it is faster and more precise than a GPS module.


## Parts and modules

**Electric bicycle control unit**

* **Arduino Nano 3.0**
* **ACS712 30A** current sensor 
* **Neo-6m** GPS module
* **hd44780** 16*2 lcd display 
* **PCF8574** I2C backpack

**Other parts on the bike**

* **Battery pack** 2600 mAh 18650 cell in 13s3p configuration, with a 30 A BMS-board and  30A automotive fuze
* **Rear hub motor** 2000 W 3-phase direct drive
* **Motor Controller** 48 V, 30 A max rated controller with display, brake sensor and pedal assist sensors
* **step-down converters** 60 V to 12 V (60 W) for driving lights and 12 V to (5 V 5 W) for driving controller and sensors

## Schematic 

**Arduino pins and voltage dividers**
![Arduino wiring](https://github.com/VeikkoAJ/Ebike-Battery-Range-Estimator/blob/main/pics/wiring.svg)


The bike operates on 3 voltage levels:
 * 48 V: motor controller, main voltage divider
 * 12 V: produced by the 60v->12V (60W) step-down converter. 12 V is used to power the light switch voltage divider, headlight, backlight, and high beam.
 * 5 V: produced by the 12V->5V (10W) step-down converter. 5 V is used to drive all the modules and the LCD display.
 
Additionally, the bike has a main relay driven by an ignition switch. The relay is located directly after the battery. The current sensor is located after the relay, measuring full current flow, except the relay coil current.

## Libraries
* **Wire.h**, I2C interface to display
* **SoftwareSerial.h***, GPS connection
* **[TinyGPS.h](https://github.com/mikalhart/TinyGPS)** GPS message parsing
* **[hd44780.h](https://github.com/duinoWitchery/hd44780)**, LCD driver
* **[hd44780ioClass/hd44780_I2Cexp.h](https://github.com/duinoWitchery/hd44780)**, I2C interface

## Possible features
* include relay and battery pack wiring in README
* LCD dimming with headlight state
* Hour meter (already included in most of the stock Ebike control units)

## Known Bugs
* Powering the bike from battery breaks the connection from Arduino to the laptop. This is probably caused by the 5V step-down converter having different voltage than the computers usb port.
* Battery charge bar not rounding to 100 % after gull recharge
* When the headlight was connected directly to the 12V step-down converter, the hub motor could be used as a dynamo powering the headlight in case of battery dying. This feature disappeared when the headlight was connected to a mosfet led driver
* Setting LCD custom symbols should be done with pointers

## More pictures
**Full bicycle**

<img src="https://github.com/VeikkoAJ/Ebike-Battery-Range-Estimator/blob/main/pics/fullbike.png" width="760">

**Frame design**

<img src="https://github.com/VeikkoAJ/Ebike-Battery-Range-Estimator/blob/main/pics/runko%20v2%20Drawing%20v2-1.jpg" width="760">


## Sources
* [Battery capacity and discharge graphs, lygte-info](https://lygte-info.dk/review/batteries2012/LG%2018650%20M26%202600mAh%20(Purple)%20UK.html)
* [A Closer Look at State of Charge (SOC) and State of Health (SOH)Estimation Techniques for Batteries, Analog Devices](https://www.analog.com/media/en/technical-documentation/technical-articles/a-closer-look-at-state-of-charge-and-state-health-estimation-techniques.pdf)
* [Battery capacity function, Geogebra](https://www.geogebra.org/graphing/rwusccpe)
