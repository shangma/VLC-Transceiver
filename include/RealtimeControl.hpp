/* Module to control PRU (Programmable Realtime Unit) for the Beaglebone Black.

   This should handle initializing the PRUs two units to do data transfer
   and modulation. 
   
   Author: Ben Caine
   Date: October 17, 2015
*/
#ifndef REALTIME_HPP
#define REALTIME_HPP

#include "prussdrv.h"
#include <pruss_intc_mapping.h>	 

#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <cstring>
#include <sys/mman.h>
#include <stdint.h>
#include <iostream>

#define DDR_BASEADDR     0x90000000

const int PACKET_SIZE = 88;

using namespace std;

class RealtimeControl {

public:

  bool InitPRU();
  void DisablePRU();
  void Transmit();
  void Test();
  
  
  /* Memory Related Functionality */
  bool OpenMem();
  void CloseMem();
  void pop(uint8_t* packet);
  void push(uint8_t* bytes);

  void setLength(int n) { *((uint32_t*)_length) = n; }
  uint32_t getLength() { return *((uint32_t*)_length); }

  // Returns pointer to current location of queue
  uint8_t* peek() { return (uint8_t*)_data + (_internal_cursor); }

  uint8_t* data() { return (uint8_t*)_data; }
  void setCursor(uint32_t val) { _internal_cursor = val; }

  uint32_t pruCursor() { return *((uint32_t*)_pru_cursor) - 8; }
  uint32_t internalCursor() { return _internal_cursor; }

  uint32_t max_bytes;
  uint32_t max_packets;

private:
  int mem_fd;
  void *ddrMem;

  // four bytes for length
  void* _length;
  // Four bytes for PRU cursor
  // VALUE IN BYTES
  void* _pru_cursor;
  // Rest is data
  void *_data;


  // Internal cursor controls knowing where we are writing
  // or reading data coming from encoding or going towards
  // decoding.
  // VALUE IN BYTES
  uint32_t _internal_cursor;

};

#endif // REALTIME_HPP
