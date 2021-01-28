# Ebike-Battery-Range-Estimator





## Usage


## Used parts and modules
* **Arduino Nano 3.0**
* **ACS712 30A** current sensor 
* **Neo-6m** gps module
* **hd44780** 16*2 lcd display 
* **PCF8574** I2C packback

## Libraries
* **Wire.h**, secondary serialport to gps module
* **SoftwareSerial.h***, gps connection
* **[TinyGPS.h](https://github.com/mikalhart/TinyGPS)** gps message parsing
* **hd44780.h**, LCD driver
* **hd44780ioClass/hd44780_I2Cexp.h**, I2C interface

## Sources
* [Battery capacity and discharge graphs, lygte-info](https://lygte-info.dk/review/batteries2012/LG%2018650%20M26%202600mAh%20(Purple)%20UK.html)
* [A Closer Look at State of Charge (SOC) and State of Health (SOH)Estimation Techniques for Batteries, Analog Devices](https://www.analog.com/media/en/technical-documentation/technical-articles/a-closer-look-at-state-of-charge-and-state-health-estimation-techniques.pdf)
* [Battery capacity function, Geogebra](https://www.geogebra.org/graphing/rwusccpe)
