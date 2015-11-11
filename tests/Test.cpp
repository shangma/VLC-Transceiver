#include "Transceiver.hpp"
#include "ForwardErrorCorrection.hpp"
#include "RealtimeControl.hpp"
#include "SocketConnection.hpp"
#include "Golay.hpp"
#include "ByteQueue.hpp"
#include<iostream>
#include<exception>
#include<ctime>
#include<cstdlib>

using namespace std;

unsigned char* GenerateData(int bytes) {
  unsigned char *data = new unsigned char[bytes];

  // Generate Alphabet over and over
  for (int i = 0; i < bytes; i++) {
    data[i] = char((i % 26) + 97);
  }
  
  return data;
}

void CorruptData(unsigned char* data,
		 int n_corruptions,
		 int bytes) {

  for (int i = 0; i < n_corruptions; ++i) {
    // Corrupt a single bit
    int position = rand() % (bytes * 8);
    setBit(data, position, getBit(data, position) ^ 1);
  }
}

unsigned int HammingDistance(unsigned char* a,
			     unsigned char* b,
			     unsigned int n) {

  unsigned int num_mismatches = 0;
  while (n) {
    if (*a != *b) {
      for (int i = 0; i < 8; ++i)
	num_mismatches += (getBit(a, i) == getBit(b, i));
    }
    
    --n;
    ++a;
    ++b;
  }
  
  return num_mismatches;
}

void TestFEC() {
  int data_length = 96;
  int num_errors = 35;

  cout << "Testing FEC with " << num_errors << " errors" << endl;

  assert(data_length % 3 == 0);

  unsigned char* data = GenerateData(data_length);
  unsigned char* encoded = new unsigned char[data_length * 2];
  unsigned char* decoded = new unsigned char[data_length];

  cout << data << endl;
  cout << "-------------------------" << endl;

  ForwardErrorCorrection fec;
  
  fec.Encode(data, encoded, data_length);

  CorruptData(encoded, num_errors, data_length);

  fec.Decode(encoded, decoded, data_length * 2);

  cout << decoded << endl;

  cout << "Hamming Distance between input and output: ";
  cout << HammingDistance(data, decoded, data_length) << endl;

}

void TestByteQueue() {
  uint32_t* pru_cursor = new uint32_t;
  *pru_cursor = 0;

  ByteQueue queue(pru_cursor, 200);
  cout << "Queue Created" << endl;

  queue.push(GenerateData(100), 100);

  for (int i = 0; i < 4; i++) {
    cout << queue.pop() << endl;
  }
}


int main() {

  cout << "Forward Error Correction Test Running..." << endl;
  TestFEC();

  for(int i = 0; i < 5; i++)
    cout << endl;

  cout << "ByteQueue Test Running..." << endl;
  TestByteQueue();

  return 0;
};
