// This is sample Arduino code for the Maxim MAX31856 thermocouple IC
// http://datasheets.maximintegrated.com/en/ds/MAX31856.pdf
//
// Written by Peter Easton (www.whizoo.com)
// Released under CC BY-SA 3.0 license
//
// Look for the MAX31856 breakout boards on www.whizoo.com and eBay
// http://stores.ebay.com/madeatrade
//
// Looking to build yourself a reflow oven?  It isn't that difficult to
// do!  Take a look at the build guide here:
// http://www.whizoo.com
//
// Library Implementation Details
// ==============================
// DRDY and FAULT lines are not used in this driver. DRDY is useful for low-power mode so samples are only taken when
// needed; this driver assumes power isn't an issue.  The FAULT line can be used to generate an interrupt in the host
// processor when a fault occurs.  This library reads the fault register every time a reading is taken, and will
// return a fault error if there is one.  The MAX31856 has sophisticated usage scenarios involving FAULT.  For
// example, low and high temperature limits can be set, and the FAULT line triggers when these temperatures are
// breached. This is beyond the scope of this sample library.  The assumption is that most applications will be
// polling for temperature readings - but it is good to know these features are supported by the hardware.
//
// The MAX31856 differs from earlier thermocouple IC's in that it has registers that must be configured before
// readings can be taken.  This makes it very flexible and powerful, but one concern is power loss to the IC.  The IC
// should be as close to the cold junction as possible, which might mean there is a cable connecting the breakout
// board to the host processor.  If this cable is disconnected and reconnected (MAX31856 loses power) then the
// registers must be reinitialized.  This library detects this condition and will automatically reconfigure the
// registers.  This simplifies the software running on the host.
//
// A lot of configuration options appear in the .H file.  Of particular note is the line frequency filtering, which
// defaults to 60Hz (USA and others).  If your line voltage is 50Hz you should set CR0_NOISE_FILTER_50HZ.
//
// This library handles the full range of temperatures, including negative temperatures.
//
// When connecting the thermocouple, remember the 2 wires are polarized.  If temperatures go up when you expect
// them to go down just reverse the wires.  No damage will be done to the MAX31856.
//
// Change History:
// 25 June 2015        Initial Version
// 31 July 2015        Fixed spelling and formatting problems


#include <MAX31856.h>

// This sample code works with this breakout board:
// http://www.ebay.com/itm/301671408961
//
// The power requirement for the board is less than 2mA.  Most microcontrollers can source or sink a lot more
// than that one each I/O pin.  For example, the ATmega328 supports up to 20mA.  So it is possible to power the
// board using I/O pins for power - so you can turn the board on and off (if you want to).
// FAULT and DRDY are not used by the library (see above)
#define SCK    3
#define CS     4
#define SDI    5
#define SDO    6

// MAX31856 Initial settings (see MAX31856.h and the MAX31856 datasheet)
// The default noise filter is 60Hz, suitable for the USA
#define CR0_INIT  (CR0_AUTOMATIC_CONVERSION + CR0_OPEN_CIRCUIT_FAULT_TYPE_K /* + CR0_NOISE_FILTER_50HZ */)
#define CR1_INIT  (CR1_AVERAGE_2_SAMPLES + CR1_THERMOCOUPLE_TYPE_K)
#define MASK_INIT (~(MASK_VOLTAGE_UNDER_OVER_FAULT + MASK_THERMOCOUPLE_OPEN_FAULT))

MAX31856 *temperature;

void setup() {
  // Display temperatures using the serial port
  Serial.begin(9600);
  delay(3000);
  Serial.println("MAX31856 Sample application");
  
  // Define the pins used to communicate with the MAX31856
  temperature = new MAX31856(SDI, SDO, CS, SCK);
  
  // Initializing the MAX31855's registers
  temperature->writeRegister(REGISTER_CR0, CR0_INIT);
  temperature->writeRegister(REGISTER_CR1, CR1_INIT);
  temperature->writeRegister(REGISTER_MASK, MASK_INIT);
  
  // Wait for the first sample to be taken
  delay(200);
}


void loop () {
  float t;
  
  // Display the junction (IC) temperature
  t = temperature->readJunction(CELSIUS);
  Serial.print("Junction (IC) temperature =");
  printTemperature(t);
  
  // Display the thermocouple temperature
  t = temperature->readThermocouple(CELSIUS);
  Serial.print("  Thermocouple temperature = ");
  printTemperature(t);
  
  Serial.println();
  delay(1000);
}


// Print the temperature, or the type of fault
void printTemperature(double temperature) {
  switch ((int) temperature) {
    case FAULT_OPEN:
      Serial.print("FAULT_OPEN");
      break;
    case FAULT_VOLTAGE:
      Serial.print("FAULT_VOLTAGE");
      break;
    case NO_MAX31856:
      Serial.print("NO_MAX31856");
      break;
    default:
      Serial.print(temperature);
      break;
  }
  Serial.print(" ");
}




