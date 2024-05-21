#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is connected to digital pin D2
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);  

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

float temp_f;
int temp_i;

void setup(void)
{
  sensors.begin();  // Start up the library
  Serial.begin(9600);
}

void loop(void)
{ 
  // Send the command to get temperatures
  sensors.requestTemperatures(); 
  temp_f = sensors.getTempCByIndex(0);
  temp_i = (int) temp_f;

  //print the temperature in Celsius
  Serial.print("Temperature: ");
  Serial.print(temp_i);
  Serial.print((char)176);//shows degrees character
  Serial.print("C  |  ");
  
  //print the temperature in Fahrenheit
  Serial.print((sensors.getTempCByIndex(0) * 9.0) / 5.0 + 32.0);
  Serial.print((char)176);//shows degrees character
  Serial.println("F");
  
  delay(500);
}
