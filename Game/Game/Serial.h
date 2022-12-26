// Serial.h: Schnittstelle für die Klasse Serial.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIAL_H__1C8AA91E_29EB_4BEE_99FF_8C33DE61B035__INCLUDED_)
#define AFX_SERIAL_H__1C8AA91E_29EB_4BEE_99FF_8C33DE61B035__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Windows.h>
#include <string>

using namespace std;

class Serial  
{
private:
  // Der Port-Name (COM1, COM2)
 string portName;
  // Die Datenübertragungsrate
  int baudrate;
  // Die Anzahl der Datenbits (5 .. 8)
  int dataBits;
  // Der Anzahl der Stoppbits (ONESTOPBIT, ONE5STOPBITS, TWOSTOPBITS)
  int stopBits;
  // Festlegung der Parität (EVENPARITY, NOPARITY, ODDPARITY)
  int parity;

  /* Handle für den Com-Port */
  HANDLE handle;

public:
  Serial(string portName, int baudrate, int dataBits, int stopBits, int parity);
  ~Serial();

  bool open();
  void close();

  int dataAvailable(); // Anzahl Zeichen, die im Empfangspuffer stehen
  int read();
  int read(char *buffer, int bufSize);
  string readLine();
  void write(int value);
  void write(const char *buffer, int bytesToWrite);
  void write(string s);
  void setRTS(bool arg);
  void setDTR(bool arg);
  bool isCTS();
  bool isDSR();
};

#endif // !defined(AFX_SERIAL_H__1C8AA91E_29EB_4BEE_99FF_8C33DE61B035__INCLUDED_)
