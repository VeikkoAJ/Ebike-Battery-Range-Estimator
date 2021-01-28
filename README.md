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

**Base voltage of the Ebike is 48V**, meaning the voltage divider resistors are sized for 5V pin voltage at 59 V battery voltage, so it probably won't work with 36V or 24V systems. The LCD was mounted sideways, so 8 extra characters are defined and saved to the LCD display. This is why the display is only capable of showing 2 digit numbers per row and unit definitions have to be painted next to the lcd.

**State of charge estimation**



## Used parts and modules
* **Arduino Nano 3.0**
* **ACS712 30A** current sensor 
* **Neo-6m** gps module
* **hd44780** 16*2 lcd display 
* **PCF8574** I2C packback

## Schematic 

![Arduino wiring](https://github.com/VeikkoAJ/Ebike-Battery-Range-Estimator/blob/main/pics/wiring.svg)

The bike operates on 3 voltage levels
 * 48V: motor controller, main voltage divider
 * 12V: produced by a (60v->12V) 60W step-down converter. 12V is used to power the light switch votlagedivider, Headlight, backlight and high beam.
 * 5V: produced by a (12V->5V) 10W step.down converter. 5V is used to drive all the modules and the LCD display.
 
Additionally the bike has a main relay driven by an ignition switch. The relay is located directly after the battery. The current senosr is located after the relay. 

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

## Sources
* [Battery capacity and discharge graphs, lygte-info](https://lygte-info.dk/review/batteries2012/LG%2018650%20M26%202600mAh%20(Purple)%20UK.html)
* [A Closer Look at State of Charge (SOC) and State of Health (SOH)Estimation Techniques for Batteries, Analog Devices](https://www.analog.com/media/en/technical-documentation/technical-articles/a-closer-look-at-state-of-charge-and-state-health-estimation-techniques.pdf)
* [Battery capacity function, Geogebra](https://www.geogebra.org/graphing/rwusccpe)
