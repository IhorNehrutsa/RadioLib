#include "PhysicalLayer.h"

PhysicalLayer::PhysicalLayer(float freqStep, size_t maxPacketLength) {
  _freqStep = freqStep;
  _maxPacketLength = maxPacketLength;
  #if !defined(RADIOLIB_EXCLUDE_DIRECT_RECEIVE)
  _bufferBitPos = 0;
  _bufferWritePos = 0;
  #endif
}

#if defined(RADIOLIB_BUILD_ARDUINO)
int16_t PhysicalLayer::transmit(__FlashStringHelper* fstr, uint8_t addr) {
  // read flash string length
  size_t len = 0;
  PGM_P p = reinterpret_cast<PGM_P>(fstr);
  while(true) {
    char c = RADIOLIB_NONVOLATILE_READ_BYTE(p++);
    len++;
    if(c == '\0') {
      break;
    }
  }

  // dynamically allocate memory
  #if defined(RADIOLIB_STATIC_ONLY)
    char str[RADIOLIB_STATIC_ARRAY_SIZE];
  #else
    char* str = new char[len];
  #endif

  // copy string from flash
  p = reinterpret_cast<PGM_P>(fstr);
  for(size_t i = 0; i < len; i++) {
    str[i] = RADIOLIB_NONVOLATILE_READ_BYTE(p + i);
  }

  // transmit string
  int16_t state = transmit(str, addr);
  #if !defined(RADIOLIB_STATIC_ONLY)
    delete[] str;
  #endif
  return(state);
}

int16_t PhysicalLayer::transmit(String& str, uint8_t addr) {
  return(transmit(str.c_str(), addr));
}
#endif

int16_t PhysicalLayer::transmit(const char* str, uint8_t addr) {
  return(transmit((uint8_t*)str, strlen(str), addr));
}

int16_t PhysicalLayer::transmit(uint8_t* data, size_t len, uint8_t addr) {
  (void)data;
  (void)len;
  (void)addr;
  return(RADIOLIB_ERR_UNSUPPORTED);
}

#if defined(RADIOLIB_BUILD_ARDUINO)
int16_t PhysicalLayer::receive(String& str, size_t len) {
  int16_t state = RADIOLIB_ERR_NONE;

  // user can override the length of data to read
  size_t length = len;

  // build a temporary buffer
  #if defined(RADIOLIB_STATIC_ONLY)
    uint8_t data[RADIOLIB_STATIC_ARRAY_SIZE + 1];
  #else
    uint8_t* data = NULL;
    if(length == 0) {
      data = new uint8_t[_maxPacketLength + 1];
    } else {
      data = new uint8_t[length + 1];
    }
    if(!data) {
      return(RADIOLIB_ERR_MEMORY_ALLOCATION_FAILED);
    }
  #endif

  // attempt packet reception
  state = receive(data, length);

  // any of the following leads to at least some data being available
  // let's leave the decision of whether to keep it or not up to the user
  if((state == RADIOLIB_ERR_NONE) || (state == RADIOLIB_ERR_CRC_MISMATCH) || (state == RADIOLIB_ERR_LORA_HEADER_DAMAGED)) {
    // read the number of actually received bytes (for unknown packets)
    if(len == 0) {
      length = getPacketLength(false);
    }

    // add null terminator
    data[length] = 0;

    // initialize Arduino String class
    str = String((char*)data);
  }

  // deallocate temporary buffer
  #if !defined(RADIOLIB_STATIC_ONLY)
    delete[] data;
  #endif

  return(state);
}
#endif

int16_t PhysicalLayer::receive(uint8_t* data, size_t len) {
  (void)data;
  (void)len;
  return(RADIOLIB_ERR_UNSUPPORTED);
}

int16_t PhysicalLayer::sleep() {
  return(RADIOLIB_ERR_UNSUPPORTED);
}

int16_t PhysicalLayer::standby() {
  return(standby(RADIOLIB_STANDBY_DEFAULT));
}

int16_t PhysicalLayer::standby(uint8_t mode) {
  (void)mode;
  return(RADIOLIB_ERR_UNSUPPORTED);
}

int16_t PhysicalLayer::startReceive(uint32_t timeout, uint16_t irqFlags, uint16_t irqMask, size_t len) {
  (void)timeout;
  (void)irqFlags;
  (void)irqMask;
  (void)len;
  return(RADIOLIB_ERR_UNSUPPORTED);
}

#if defined(RADIOLIB_BUILD_ARDUINO)
int16_t PhysicalLayer::startTransmit(String& str, uint8_t addr) {
  return(startTransmit(str.c_str(), addr));
}
#endif

int16_t PhysicalLayer::startTransmit(const char* str, uint8_t addr) {
  return(startTransmit((uint8_t*)str, strlen(str), addr));
}

int16_t PhysicalLayer::startTransmit(uint8_t* data, size_t len, uint8_t addr) {
  (void)data;
  (void)len;
  (void)addr;
  return(RADIOLIB_ERR_UNSUPPORTED);
}

int16_t PhysicalLayer::finishTransmit() {
  return(RADIOLIB_ERR_UNSUPPORTED);
}

#if defined(RADIOLIB_BUILD_ARDUINO)
int16_t PhysicalLayer::readData(String& str, size_t len) {
  int16_t state = RADIOLIB_ERR_NONE;

  // read the number of actually received bytes
  size_t length = getPacketLength();

  if((len < length) && (len != 0)) {
    // user requested less bytes than were received, this is allowed (but frowned upon)
    // requests for more data than were received will only return the number of actually received bytes (unlike PhysicalLayer::receive())
    length = len;
  }

  // build a temporary buffer
  #if defined(RADIOLIB_STATIC_ONLY)
    uint8_t data[RADIOLIB_STATIC_ARRAY_SIZE + 1];
  #else
    uint8_t* data = new uint8_t[length + 1];
    if(!data) {
      return(RADIOLIB_ERR_MEMORY_ALLOCATION_FAILED);
    }
  #endif

  // read the received data
  state = readData(data, length);

  // any of the following leads to at least some data being available
  // let's leave the decision of whether to keep it or not up to the user
  if((state == RADIOLIB_ERR_NONE) || (state == RADIOLIB_ERR_CRC_MISMATCH) || (state == RADIOLIB_ERR_LORA_HEADER_DAMAGED)) {
    // add null terminator
    data[length] = 0;

    // initialize Arduino String class
    str = String((char*)data);
  }

  // deallocate temporary buffer
  #if !defined(RADIOLIB_STATIC_ONLY)
    delete[] data;
  #endif

  return(state);
}
#endif

int16_t PhysicalLayer::readData(uint8_t* data, size_t len) {
  (void)data;
  (void)len;
  return(RADIOLIB_ERR_UNSUPPORTED);
}

int16_t PhysicalLayer::transmitDirect(uint32_t frf) {
  (void)frf;
  return(RADIOLIB_ERR_UNSUPPORTED);
}

int16_t PhysicalLayer::receiveDirect() {
  return(RADIOLIB_ERR_UNSUPPORTED);
}

int16_t PhysicalLayer::setFrequency(float freq) {
  (void)freq;
  return(RADIOLIB_ERR_UNSUPPORTED);
}

int16_t PhysicalLayer::setBitRate(float br) {
  (void)br;
  return(RADIOLIB_ERR_UNSUPPORTED);
}

int16_t PhysicalLayer::setFrequencyDeviation(float freqDev) {
  (void)freqDev;
  return(RADIOLIB_ERR_UNSUPPORTED);
}

int16_t PhysicalLayer::setDataShaping(uint8_t sh) {
  (void)sh;
  return(RADIOLIB_ERR_UNSUPPORTED);
}

int16_t PhysicalLayer::setEncoding(uint8_t encoding) {
  (void)encoding;
  return(RADIOLIB_ERR_UNSUPPORTED);
}

float PhysicalLayer::getFreqStep() const {
  return(_freqStep);
}

size_t PhysicalLayer::getPacketLength(bool update) {
  (void)update;
  return(0);
}

float PhysicalLayer::getRSSI() {
  return(RADIOLIB_ERR_UNSUPPORTED);
}

float PhysicalLayer::getSNR() {
  return(RADIOLIB_ERR_UNSUPPORTED);
}

int32_t PhysicalLayer::random(int32_t max) {
  if(max == 0) {
    return(0);
  }

  // get random bytes from the radio
  uint8_t randBuff[4];
  for(uint8_t i = 0; i < 4; i++) {
    randBuff[i] = randomByte();
  }

  // create 32-bit TRNG number
  int32_t randNum = ((int32_t)randBuff[0] << 24) | ((int32_t)randBuff[1] << 16) | ((int32_t)randBuff[2] << 8) | ((int32_t)randBuff[3]);
  if(randNum < 0) {
    randNum *= -1;
  }
  RADIOLIB_DEBUG_PRINTLN("%d", randNum);
  return(randNum % max);
}

int32_t PhysicalLayer::random(int32_t min, int32_t max) {
  if(min >= max) {
    return(min);
  }

  return(PhysicalLayer::random(max - min) + min);
}

uint8_t PhysicalLayer::randomByte() {
  return(0);
}

int16_t PhysicalLayer::startDirect() {
  // disable encodings
  int16_t state = setEncoding(RADIOLIB_ENCODING_NRZ);
  RADIOLIB_ASSERT(state);

  // disable shaping
  state = setDataShaping(RADIOLIB_SHAPING_NONE);
  RADIOLIB_ASSERT(state);

  // set frequency deviation to the lowest possible value
  state = setFrequencyDeviation(-1);
  return(state);
}

#if !defined(RADIOLIB_EXCLUDE_DIRECT_RECEIVE)
int16_t PhysicalLayer::available() {
  return(_bufferWritePos);
}

void PhysicalLayer::dropSync() {
  if(_directSyncWordLen > 0) {
    _gotSync = false;
    _syncBuffer = 0;
  }
}

uint8_t PhysicalLayer::read(bool drop) {
  if(drop) {
    dropSync();
  }
  _bufferWritePos--;
  return(_buffer[_bufferReadPos++]);
}

int16_t PhysicalLayer::setDirectSyncWord(uint32_t syncWord, uint8_t len) {
  if(len > 32) {
    return(RADIOLIB_ERR_INVALID_SYNC_WORD);
  }
  _directSyncWordMask = 0xFFFFFFFF >> (32 - len);
  _directSyncWordLen = len;
  _directSyncWord = syncWord;

  // override sync word matching when length is set to 0
  if(_directSyncWordLen == 0) {
    _gotSync = true;
  }

  return(RADIOLIB_ERR_NONE);
}

void PhysicalLayer::updateDirectBuffer(uint8_t bit) {
  // check sync word
  if(!_gotSync) {
    _syncBuffer <<= 1;
    _syncBuffer |= bit;

    RADIOLIB_VERBOSE_PRINTLN("S\t%X", _syncBuffer);

    if((_syncBuffer & _directSyncWordMask) == _directSyncWord) {
      _gotSync = true;
      _bufferWritePos = 0;
      _bufferReadPos = 0;
      _bufferBitPos = 0;
    }

  } else {
    // save the bit
    if(bit) {
      _buffer[_bufferWritePos] |= 0x01 << _bufferBitPos;
    } else {
      _buffer[_bufferWritePos] &= ~(0x01 << _bufferBitPos);
    }
    _bufferBitPos++;

    // check complete byte
    if(_bufferBitPos == 8) {
      _buffer[_bufferWritePos] = Module::flipBits(_buffer[_bufferWritePos]);
      RADIOLIB_VERBOSE_PRINTLN("R\t%X", _buffer[_bufferWritePos]);

      _bufferWritePos++;
      _bufferBitPos = 0;
    }
  }
}

void PhysicalLayer::setDirectAction(void (*func)(void)) {
  (void)func;
}

void PhysicalLayer::readBit(RADIOLIB_PIN_TYPE pin) {
  (void)pin;
}

#endif

int16_t PhysicalLayer::setDIOMapping(RADIOLIB_PIN_TYPE pin, uint8_t value) {
  (void)pin;
  (void)value;
  return(RADIOLIB_ERR_UNSUPPORTED);
}

void PhysicalLayer::setDio1Action(void (*func)(void)) {
  (void)func;
}

void PhysicalLayer::clearDio1Action() {
}

#if defined(RADIOLIB_INTERRUPT_TIMING)
void PhysicalLayer::setInterruptSetup(void (*func)(uint32_t)) {
  Module* mod = getMod();
  mod->TimerSetupCb = func;
}

void PhysicalLayer::setTimerFlag() {
  Module* mod = getMod();
  mod->TimerFlag = true;
}
#endif
