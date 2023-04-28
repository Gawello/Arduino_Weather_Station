#include <DFRobot_I2C_Multiplexer.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_BMP280.h>
#include "Air_Quality_Sensor.h"
#include <RTClib.h>
#include <string.h>


/*Create an I2C Multiplexer object, the default address of I2C Multiplexer is 0X70*/
DFRobot_I2C_Multiplexer I2CMultiplexer(&Wire, 0x70);
LiquidCrystal_I2C lcd(0x27, 20, 4);
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;
AirQualitySensor sensor(A0);
//DS3231 rtc(SDA,SCL);
RTC_DS3231 rtc;

float pressure;

char daysOfTheWeek[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

int Day; 
int Month;
int Year; 
int Secs;
int Minutes;
int Hours;


void setup() {
  //Run display
  I2CMultiplexer.begin();
  I2CMultiplexer.selectPort(0);
  lcd.begin();   // Inicjalizacja LCD 
  lcd.backlight(); // zalaczenie podwietlenia

  Serial.begin(57600);

  // ATH is temperature sensor
  // Check ATH sensor on port 1 and display info
  I2CMultiplexer.selectPort(1);
   if (! aht.begin()) {
    I2CMultiplexer.selectPort(0);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("AHT20 not found ");
    while (1) delay(1000);
    lcd.clear();
  } else {
    I2CMultiplexer.selectPort(0);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("AHT20 found");
    delay(1000);
  }

  // BMP begin with port!!!
  // BMP is pressure sensor 
  // Check BMP sersor on port 2 and display info
  I2CMultiplexer.selectPort(2);
  if (!bmp.begin(0x76)) {
    I2CMultiplexer.selectPort(0);
    lcd.setCursor(0, 1);
    lcd.print("BMP280 not found ");
    delay(1000);
  } else {
    I2CMultiplexer.selectPort(0);
    lcd.setCursor(0,1);
    lcd.print("BMP280 found");
    delay(1000);
  }

  // Waiting to initialize sensor
  // Sensor for Air quality need some time to initialize
  I2CMultiplexer.selectPort(0);
  lcd.setCursor(0,2);
  lcd.print("Waiting for sensor..");
  lcd.setCursor(0,3);
  delay(500);
  for (int i=0; i<20; i++){
    lcd.print("-");
    delay(500);
  }
  
  // Air quality sensor status
  if (sensor.init()) {
    I2CMultiplexer.selectPort(0);
    lcd.setCursor(0,2);
    lcd.print("                    ");
    lcd.setCursor(0,2);
    lcd.print("Air Sensor ready    ");
    delay(1000);
    } else {
      I2CMultiplexer.selectPort(0);
      lcd.setCursor(0,2);
      lcd.print("Air Sensor ERROR!   ");
      delay(1000);
    }

  // Clock initialize status
  I2CMultiplexer.selectPort(3);
  if (! rtc.begin()) {
    I2CMultiplexer.selectPort(0);
    lcd.setCursor(0,3);
    lcd.print("Couldn't find RTC   ");
    delay(1000);
    } else {
    I2CMultiplexer.selectPort(0);
    lcd.setCursor(0,2);
    lcd.print("RTC found           ");
    delay(1000);
    }
  I2CMultiplexer.selectPort(3);
  rtc.begin();
  // Delete comment when need to update time and date
  // Use only ones and comment again and resend program   
  //rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));



  //SETTINGS FOR BMP SENSOR
  I2CMultiplexer.selectPort(2);
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  // Clearing display after displaying all of the info
  I2CMultiplexer.selectPort(0);
  lcd.clear();
}

void loop() {
  // Declaration of necessary variables
  int quality = sensor.slope(); // Variable for Air quality
  String myDate;                // String for Date from RTC
  String myTime;                // String for time from RTC
  // Empty String for correct display time
  String time;
  //memset(time, NULL, sizeof(time));
  // Empty String for correct display date
  String date;

  I2CMultiplexer.selectPort(1);
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data

  I2CMultiplexer.selectPort(2);
  pressure = bmp.readPressure()/100;

  // Read and display time and date from RTC
  I2CMultiplexer.selectPort(3);
  DateTime now = rtc.now();     
  Day = now.day();
  Month = now.month();
  Year = now.year();
  Secs = now.second();
  Hours = now.hour();
  Minutes = now.minute();

  // Format time values as strings with leading zeros
  char formattedTime[6];
  sprintf(formattedTime, "%02d:%02d", Hours, Minutes);
  //char formattedMinutes[2];
  //sprintf(formattedMinutes, "%02d", Minutes);
  //time = String(formattedHours) + ":" + String(formattedMinutes);
  //strcpy(time, Hours);     // Copy formattedHours into result
  //strcat(time, ":");                // Append a colon to result
  //strcat(time, formattedMinutes);   // Append formattedMinutes to result

  //Format date values as strings with leading zeros
  char formattedDate[11];
  sprintf(formattedDate, "%02d/%02d/%04d", Day, Month, Year);
  // char formattedMonth[4];
  // sprintf(formattedMonth, "%02d", now.month());
  // char formattedYear[4];
  // sprintf(formattedYear, "%04d", now.year());
  // date = String(formattedDay) + "/" + String(formattedMonth) + "/" + String(formattedYear);
  // //strcat(date, Day);
  //strcat(date, "/");
  //strcat(date, formattedMonth);
  //strcat(date, "/");
  //strcat(date, formattedYear);
  
  // Old function for time
  // myTime = myTime + Hours +":"+ Minutes;
  // myDate = myDate + " "+ Day + "/" + Month + "/" + Year ;

  // Display values on LCD
  I2CMultiplexer.selectPort(0);
  // Printng separators
  lcd.setCursor(10,0);
  lcd.print("|");
  lcd.setCursor(10,1);
  lcd.print("|");

  // Printing temperature and humudity on LCD
  lcd.setCursor(0,0);
  lcd.print("T:"); 
  lcd.print(temp.temperature); lcd.print((char)223); lcd.print("C");
  lcd.setCursor(11,0);
  lcd.print("H:"); 
  lcd.print(humidity.relative_humidity); lcd.print("%");

  // Printing air pressure on LCD
  lcd.setCursor(0,1);
  lcd.print(F("P:"));
  lcd.print(pressure,0); //displaying the Pressure in hPa, you can change the unit
  lcd.print("hPa");

  // Printing air quality value and rating
  lcd.setCursor(11,1);
  lcd.print("Air:");
  lcd.print(sensor.getValue());

  lcd.setCursor(0,2);
  if (quality == AirQualitySensor::FORCE_SIGNAL) {
        lcd.print("Critical pollution!");
    } else if (quality == AirQualitySensor::HIGH_POLLUTION) {
        lcd.print("High pollution!    ");
    } else if (quality == AirQualitySensor::LOW_POLLUTION) {
        lcd.print("Low pollution!     ");
    } else if (quality == AirQualitySensor::FRESH_AIR) {
        lcd.print("Fresh air          ");
    }

  
  // Printing time and date on LCD
  lcd.setCursor(0,3);
  lcd.print(formattedTime);
  lcd.setCursor(9, 3);
  lcd.print(formattedDate);
  
  // Wait for next cycle
  delay(1500);
}
