# Ebike-Battery-Range-Estimator

Arduino based electric bicycle control unit. 
Main features:
* Light controlling
* Intelligent batttery charge monitoring
* Range estimation
* Vertical display
* Cool headlight and display animation on startup 

Main motivation for this project was poor battery capacity displaying with stock ebike control units, as well as the need to change headlight brightness during driving. Using the lcd caused additional challenges, as it had to  be mounted vertically on the frame due to frame width limitations.


## Usage
*It is recommended to fully customize this program, as most of this project was made with parts at hand thus containing unnescessary part (such as GPS module instead of Hall sensors).*

**Base voltage of the Ebike is 48 V**, meaning the voltage divider resistors are sized for 5 V pin voltage at 59 V battery voltage, so it probably won't work with 36 V or 24 V systems.

**State Of Charge (SOC) estimation**

State of charge is estimated in two modes: standby voltage and active current. Standby voltage is used to estimate the battery Depth of Dischardge (DOD). 

As li-ion batteries don't have a linear voltage to charge connection, a custom lookup table was created. The lookup table is based on a LG 18650 M26 2600mAh cell and it's discharge graph. Discharge graph and the fitted plot can be found [here](https://github.com/VeikkoAJ/Ebike-Battery-Range-Estimator/blob/main/README.md#sources). The fitted function was used to generate a lookup table, with almost even charge distances.

The system automatically switches to active current mode and back, when a current streshold of 0.2 A is passed. Active current mode measures current and time intervals to integrate used charge. Used charge is then added to DOD value. Measuring the battery voltage under heavy load (+20 A) gives and inaccurate reading, as the battery internal resistance saggs the voltage. This is a common problem with simpler voltage only battery capacity monitors.

**LCD Display**

The LCD display is mounted vertically on the bike frame, because the frame wasn't wide enough for a 16\*2 display to be mounted horizontally. This is why the display is only capable of showing 2 digit numbers per row and unit definitions have to be painted next to the lcd. Upside of mounting the display vertically is that a proper battery bar can be dispalyed on it. 
8 extra custom characters (numbers 2,4,5,6,7,8,9 and a "halfbar") are saved to the LCD display. A custom function is used to print numbers to the display. Custom symbols located in [CustomChar.c](https://github.com/VeikkoAJ/Ebike-Battery-Range-Estimator/blob/main/EbikeBatteryRangeEstimator/CustomChar.c)

<img src="https://github.com/VeikkoAJ/Ebike-Battery-Range-Estimator/blob/main/pics/LCD_interface_scaled.jpg" width='400'> 

*Still missing decals to cover the horrible wiring :)* 

**GPS**

GPS module was chosen because of the possible extra features it has over a hall sensor and i already had one ready from an old project. A hall sensor would be better suited for measuring the bike velocity as it is faster and more precise than a gps module.


## Used parts and modules
* **Arduino Nano 3.0**
* **ACS712 30A** current sensor 
* **Neo-6m** gps module
* **hd44780** 16*2 lcd display 
* **PCF8574** I2C packback

## Schematic 

**Arduino pins and voltage dividers**
![Arduino wiring](https://github.com/VeikkoAJ/Ebike-Battery-Range-Estimator/blob/main/pics/wiring.svg)


The bike operates on 3 voltage levels
 * 48 V: motor controller, main voltage divider
 * 12 V: produced by a (60v->12V) 60W step-down converter. 12 V is used to power the light switch votlagedivider, Headlight, backlight and high beam.
 * 5 V: produced by a (12V->5V) 10W step.down converter. 5 V is used to drive all the modules and the LCD display.
 
Additionally the bike has a main relay driven by an ignition switch. The relay is located directly after the battery. The current sensor is located after the relay, measuring full current flow, exept the relay coil current.

## Libraries
* **Wire.h**, secondary serialport to gps module
* **SoftwareSerial.h***, gps connection
* **[TinyGPS.h](https://github.com/mikalhart/TinyGPS)** gps message parsing
* **hd44780.h**, LCD driver
* **hd44780ioClass/hd44780_I2Cexp.h**, I2C interface

## Upcoming features
* relay and batterypack wiring
* lcd dimmming with light states
* Hourmeter (already included in most of the stock Ebike control units)

## Known Bugs
* Powering the bike from battery stop the connection to the laptop. This is probably caused by the 5V step-down converter having different voltage than the computers usb port.
* Before driving the headlight with a mosfet, the hubmotor could be used as a dynamo powering the headlight in case of battery dying
* Custom symbol mapping shoud use pointers

## Sources
* [Battery capacity and discharge graphs, lygte-info](https://lygte-info.dk/review/batteries2012/LG%2018650%20M26%202600mAh%20(Purple)%20UK.html)
* [A Closer Look at State of Charge (SOC) and State of Health (SOH)Estimation Techniques for Batteries, Analog Devices](https://www.analog.com/media/en/technical-documentation/technical-articles/a-closer-look-at-state-of-charge-and-state-health-estimation-techniques.pdf)
* [Battery capacity function, Geogebra](https://www.geogebra.org/graphing/rwusccpe)
