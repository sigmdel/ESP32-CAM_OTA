#pragma once

  // Function to setup the OTA server
  //
  //   Param      Function             Default   Meaning
  //
  //   hostname   mDNS host name         ""      hostname = "esp3232-[MAC]" 
  //   port       tcp port                0      port = 3232
  //   pwd        password               ""      no password protection
  //   pswdhash   MD5 hash of password   ""      no password protection 
  //
  // If password protection is desired, then only one of pwd or pswdhash should be defined. 
  // However, if a non emty pwdhash is given, pwd is ignored. 
  // It is not possible to use pwdhash in PlatformIO.
  //
void setupOTA(const char* hostname="", uint16_t port=0, const char* password="", const char* pswdhash="");

  // OTA "pump". Must be called regularly in loop() or (in a timer interrupt routine perhaps).
  //
void handleOTA(void);
