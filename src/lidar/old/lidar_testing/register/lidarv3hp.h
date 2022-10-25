#ifndef lidarv3hp_h
#define lidarv3hp_h

#define LIDARLITE_ADDR_DEFAULT 0x62

#include <Arduino.h>
#include <stdint.h>

class LIDARLite_v3HP
{
  public:
      void      configure   (uint8_t configuration = 0, uint8_t lidarliteAddress = LIDARLITE_ADDR_DEFAULT);

      void      setI2Caddr  (uint8_t newAddress, uint8_t disableDefault, uint8_t lidarliteAddress = LIDARLITE_ADDR_DEFAULT);
      uint16_t  readDistance(uint8_t lidarliteAddress = LIDARLITE_ADDR_DEFAULT);
      void      waitForBusy (uint8_t lidarliteAddress = LIDARLITE_ADDR_DEFAULT);
      uint8_t   getBusyFlag (uint8_t lidarliteAddress = LIDARLITE_ADDR_DEFAULT);
      void      takeRange   (uint8_t lidarliteAddress = LIDARLITE_ADDR_DEFAULT);
      void      resetReferenceFilter (uint8_t lidarliteAddress = LIDARLITE_ADDR_DEFAULT);

      void      write (uint8_t regAddr, uint8_t * dataBytes, uint8_t numBytes, uint8_t lidarliteAddress = LIDARLITE_ADDR_DEFAULT);
      void      read  (uint8_t regAddr, uint8_t * dataBytes, uint8_t numBytes, uint8_t lidarliteAddress = LIDARLITE_ADDR_DEFAULT);

      void      correlationRecordToSerial (uint16_t numberOfReadings = 1024, uint8_t lidarliteAddress = LIDARLITE_ADDR_DEFAULT);
      void      peakStackRead(int16_t * peakArray, int16_t * distArray, uint8_t lidarliteAddress = LIDARLITE_ADDR_DEFAULT);
};

#endif
