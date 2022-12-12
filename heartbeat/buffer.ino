const int bufLen = 5;
int bufStart = 0;
int bufEnd = 0;
uint32_t buf[bufLen];
bool empty = true;

/**
 * Push to the circular buffer, overwriting last value if full
 * Params:
 *    n (uint32_t) - the number to push to the buffer
 */
void bufPush(uint32_t n) {
  noInterrupts();
  buf[bufEnd] = n;
  bufEnd = (bufEnd + 1) % bufLen;
  // move start forward if necessary
  bool bufEndBeyondStart = (bufEnd - bufStart) % bufLen == 1;
  bool bufFullWithStartAtEnd = (bufEnd == 0) && (bufStart == bufLen - 1);
  if (!empty && (bufEndBeyondStart || bufFullWithStartAtEnd)) {
    bufStart = bufEnd;
  }
  empty = false;
  interrupts();
}

/**
 * Calculate the average heartbeat of a full buffer, returning 0 if not full
 */
float bufAvg() {
  // if full, calculate average bpm
  if (bufferFull()) {
    noInterrupts();
    int endIndex = (bufEnd + bufLen - 1) % bufLen;
    uint32_t endEl = buf[endIndex];
    uint32_t startEl = buf[bufStart];
    float avgInterval = (endEl - startEl) / ((float)bufLen - 1);
    float avgBpm = 60.0 * 1000 / avgInterval;
    interrupts();

    return avgBpm;
  }

  return 0.0;
}

/** Prints the contents of a buffer. */
void printBuf() {
  for (int i = 0; i < bufLen; i++) {
    int index = (bufStart + i) % bufLen;
    Serial.print(buf[index]);
    Serial.print(", ");
  }
  Serial.println();
}

/** True if full, false if not */
bool bufferFull() {
  return !empty && bufStart == bufEnd;
}

/**
 * Resets the buffer to be written to
 */
void clearBuf() {
  noInterrupts();
  bufEnd = 0;
  empty = true;
  bufStart = bufEnd;
  interrupts();
}
