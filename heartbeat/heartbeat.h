// #define TESTING

typedef enum {
  sOFF = 1,
  sRECEIVING_HEARTBEAT,
  sSTORING_HEARTBEAT,
  sSENDING_HEARTBEAT,
} state;

state updateFsm(state curState, uint32_t mils, int sensorSignal);
