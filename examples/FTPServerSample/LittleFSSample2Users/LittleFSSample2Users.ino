/*
   This is an example sketch to show the use of the espFTP server.

   JackV2020 added some security functionality to the server side of the library :
   - 2nd FTP user with default access restricted to readonly.
   - bool property FTPUser2WriteAccess enables/disables write access for 2nd user
   - anonymous ftp is not allowed
   - simple user ftp with password ftp is not allowed
   and
   - String property FTPaction can be used to show what is going on in the server

   Please replace
     YOUR_SSID and YOUR_PASS
   with your WiFi's values and compile.

   If you want to see debugging output of the FTP server, please
   select select an Serial Port in the Arduino IDE menu Tools->Debug Port

   Send L via Serial Monitor, to display the contents of the FS
   Send F via Serial Monitor, to fromat the FS

   This example is provided as Public Domain
   Based on LittleFSSample by Daniel Plasa <dplasa@gmail.com>
   and modified to demontrate the changes by JackV2020
   and also show creation and modification times in listDir.
   
*/
#if (defined ESP32)
#error "No LittlFS on the ESP32"
#endif

#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <FTPServer.h>
//#define BAUDRATE 74880
#define BAUDRATE 115200

//const char *ssid PROGMEM = "YOUR_SSID";
//const char *password PROGMEM = "YOUR_PASS";
const char *ssid PROGMEM = "No6gast";
const char *password PROGMEM = "1234567890";

// tell the FtpServer to use LittleFS
FTPServer ftpSrv(LittleFS);

void setup(void)
{
  Serial.begin(BAUDRATE);
  WiFi.begin(ssid, password);

  bool fsok = LittleFS.begin();
  Serial.printf_P(PSTR("FS init: %s\n"), fsok ? PSTR("ok") : PSTR("fail!"));

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.printf_P(PSTR("."));
  }
  Serial.printf_P(PSTR("\nConnected to %s, IP address is %s\n"), ssid, WiFi.localIP().toString().c_str());

  // setup the ftp server with 2 x username and password
  ftpSrv.begin(F("ftpuser1"), F("ftppwd1"),F("ftpuser2"), F("ftppwd2")); 
  // access for ftp user 2 is read only access 
  // use next to give user2 write access
  //ftpSrv.FTPUser2WriteAccess=true;
  if (ftpSrv.FTPUser2WriteAccess) {
    Serial.println(F("User ftpuser2 has write access"));
  } else {
    Serial.println(F("User ftpuser2 has no write access"));
  }
}

enum consoleaction
{
  show,
  wait,
  format,
  list
};
consoleaction action = show;
uint16_t listDir(String indent, String path);

void loop(void)
{
  // this is all you need
  // make sure to call handleFTP() frequently
  ftpSrv.handleFTP();

  // show current FTP server action
  if (ftpSrv.FTPaction !="" ) {
    Serial.println(ftpSrv.FTPaction);
    ftpSrv.FTPaction="";
  }
  //
  // Code below just to debug in Serial Monitor
  //
  if (action == show)
  {
    Serial.printf_P(PSTR("Enter 'F' to format, 'L' to list the contents of the FS\n"));
    action = wait;
  }
  else if (action == wait)
  {
    if (Serial.available())
    {
      char c = Serial.read();
      if (c == 'F')
        action = format;
      else if (c == 'L')
        action = list;
      else if (!(c == '\n' || c == '\r'))
        action = show;
    }
  }
  else if (action == format)
  {
    uint32_t startTime = millis();
    LittleFS.format();
    Serial.printf_P(PSTR("FS format done, took %lu ms!\n"), millis() - startTime);
    action = show;
  }
  else if (action == list)
  {
    uint16_t dircount=0, filecount=0; 
    uint32_t bytecount=0;
    Serial.printf_P(PSTR("Listing contents...\n"));
    listDir("", "/",dircount,filecount,bytecount);
    Serial.printf_P(PSTR("Total of %d files %d subdirs %d bytes\\nn"), filecount, dircount, bytecount);
    action = show;
  }
}

void listDir(String indent, String path,uint16_t & dirCount, uint16_t & fileCount, uint32_t & byteCount )
{
  Dir dir = LittleFS.openDir(path);
  while (dir.next())
  {
    if (dir.isDirectory())
    {
      ++dirCount;
      Serial.printf_P(PSTR("%s%s [Dir]\n"), indent.c_str(), dir.fileName().c_str());
      listDir(indent + "  ", path + dir.fileName() + "/", dirCount, fileCount, byteCount);
    } else {
      ++fileCount;
      Serial.printf_P(PSTR("%s%-25s"), indent.c_str(), dir.fileName().c_str());

      struct tm * tmstruct;
      File file = dir.openFile("r");

      time_t cr = file.getCreationTime();
      tmstruct = localtime(&cr);
      Serial.printf("  # Created : %d-%02d-%02d %02d:%02d:%02d", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);

      time_t lw = file.getLastWrite();
      tmstruct = localtime(&lw);
      Serial.printf(" # Changed : %d-%02d-%02d %02d:%02d:%02d   (%ld Bytes)\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec, (uint32_t)dir.fileSize());
      byteCount = byteCount + (uint32_t)dir.fileSize();
      file.close();
    }
  }
}
