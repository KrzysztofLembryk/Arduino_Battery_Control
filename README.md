# Arduino_Battery_Control

## PROBLEM WITH ARDUINO RESETTING after uploading programme
-  My experience with yield() for the ESP8266 is because the core libraries have implemented the watchdog timer. If you take too long in the loop() function (around 1 sec I think) the WDT will cause a hardware reset. One thing that yield() does is reset the WDT timer.
- So because WiFi connection might not be the fastest and waiting times
might be quite long, in order not to allow our arduino to reset  we 
should use yield 

## ESP8266 01S datasheet
https://www.xecor.com/blog/esp-01-pinout-programming-datasheet-and-comparison

## Arduino miliseconds - how to time intervals
- when we substract we dont need to worry about unsigned long overflow since 
result of substraction will be correct
https://arduino.stackexchange.com/questions/33572/arduino-countdown-without-using-delay/33577#33577
- Why using Strings is dangerous:
https://hackingmajenkoblog.wordpress.com/2016/02/04/the-evils-of-arduino-strings/

## ArduinoJson
https://arduinojson.org/v6/how-to/reuse-a-json-document/
- JsonDocument::clear() is implicitly called when doing deserializeJson
- we shouldn't reuse JsonDocument
- StaticJson/DynamicJson is deprecated!!

## Plan
- Wysłanie z serwera 96 wierszy ładować/nieładować + ile ładować - GIT
- co n minut 
- sprawdzanie aktualnego czasu jaki jest i w odpowiednim momencie wysłanie 
zapytania do serwera
- przerobic na stream http client get
- sprawdzanie czasu zeby wiedziec kiedy wlaczyc prad a kiedy nie
- HTTPS i szyfrowanie 
- przerwania do komunikacji z WiFi
- jakieś UI lepsze