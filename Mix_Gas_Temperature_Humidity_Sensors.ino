/////////////////////////////////////////////////////////
//  AIS NB-IOT

#include "Magellan.h"

//	AIS magellan
Magellan magel;

#warning CODE_ME :: the authenication string for accessing to AIS Magellan
char auth[]="";

/////////////////////////////////////////////////////////
//  MQ-2

// gas sensor pin
const int gasSensorPin = A0; 

//  accumulate the read sensor value
int accumulateSensorValue = 0;
float avgSensorValue;

//  number of samples from read sensor
const int numSamples = 100;

//  define R0
#define R0 4.15

/////////////////////////////////////////////////////////
//  DHT22

#include "DHT.h"

//  DHT22 pin
#define DHTPIN 22

// DHT 22 AM2302
#define DHTTYPE DHT22 

// Initialize DHT sensor.
DHT dht( DHTPIN, DHTTYPE );

/////////////////////////////////////////////////////////
//  setup
void setup() 
{
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  
  /////////////////////////////////////////////////////////
  //  AIS NB-IOT
  magel.begin(auth);           
}

void loop() 
{  
  /////////////////////////////////////////////////////////
  //  MQ-2

  /*--- Get a average data by testing 100 times ---*/
  accumulateSensorValue = 0;
  for( int x = 0 ; x < numSamples ; ++ x )
      accumulateSensorValue += analogRead( gasSensorPin );
  //  average
  avgSensorValue = accumulateSensorValue / float( numSamples );
  /*-----------------------------------------------*/

  //  calculate sensor volts
  float sensor_volts = avgSensorValue / 1024. * 5.;

  //  get the value of RS
  float rs_gas = ( 5.0 - sensor_volts ) / sensor_volts;

  //  calculate the ratio of RS/R0 is 9.8 in a clear air
  float rsPerR0 = rs_gas / R0;
  
  Serial.print( "sensor_volts = " );
  Serial.print( sensor_volts );
  Serial.println( " V");
  
  Serial.print( "RS = " );
  Serial.println( rs_gas );

  Serial.print( "RS/R0 = " );
  Serial.println( rsPerR0 );

  //  conver RS/R0 to string
  String rsPerR0String( rsPerR0, 4 );

  /////////////////////////////////////////////////////////
  //  DHT22
    
  //	reading temperature and humidity
  //		humidity
  float humidity = dht.readHumidity();
  //		temperature as Celsius 
  float temperature_celsius = dht.readTemperature();

  //  convert temperature and humidity to string
  String temperature_celsiusString( temperature_celsius, 4 );
  String humidityString( humidity, 4 );
  
  /////////////////////////////////////////////////////////
  //  AIS NB-IOT
  
  //	construct payload
  String payload = "{\"RS/R0\":" + rsPerR0String \
                      + ", \"temperature_celsius\":" + temperature_celsiusString \
                      + ", \"humidity\":" + humidityString + "}";
  
  //	post payload data to Magellan IoT platform
  magel.post( payload );
  
  //	wait 2000 milliseconds before the next loop
  delay( 2000 );
}
