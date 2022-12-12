/*
 * A struct to keep all state inputs in one place
 */
typedef struct {
  uint32_t mils;
  int sensorSignal;
} state_inputs;

/*
 * A struct to keep all state variables in one place
 */
typedef struct {
  bool previouslyBelowThreshold;
  bool off;
  uint32_t buf[bufLen];
} state_vars;

bool test_transition(state start_state,
                     state end_state,
                     state_inputs test_state_inputs,
                     state_vars start_state_vars,
                     state_vars end_state_vars,
                     bool verbos);
/*
 * Helper function for printing states
 */
char *s2str(state s) {
  switch (s) {
  case sOFF:
    return "(1) OFF";
  case sRECEIVING_HEARTBEAT:
    return "(2) RECEIVING_HEARTBEAT";
  case sSTORING_HEARTBEAT:
    return "(3) STORING_HEARTBEAT";
  case sSENDING_HEARTBEAT:
    return "(4) SENDING_HEARTBEAT";
  default:
    return "???";
  }
}

/**
 * Given two buffers, determines if they are equal
 */
bool bufEq(uint32_t gbuf[], uint32_t test_buf[]) {
  for (int i = 0; i < bufLen; i++) {
    if (gbuf[i] != test_buf[i]) {
      return false;
    }
  }
  return true;
}

/** Converts buffer into a string, which it prints to the given array. */
void buf2str(uint32_t buffr[], char s_to_print[]) {
  sprintf(s_to_print, "{ %4ld, %4ld, %4ld, %4ld, %4ld }", buffr[0], buffr[1], buffr[2], buffr[3], buffr[4]);
}

/*
 * Given a start state, inputs, and starting values for state variables, tests that
 * update_fsm returns the correct end state and updates the state variables correctly
 * returns true if this is the case (test passed) and false otherwise (test failed)
 *
 * Need to use "verbos" instead of "verbose" because verbose is apparently a keyword
 */
bool test_transition(state start_state,
                     state end_state,
                     state_inputs test_state_inputs,
                     state_vars start_state_vars,
                     state_vars end_state_vars,
                     bool verbos) {

  // set state variables
  previouslyBelowThreshold = start_state_vars.previouslyBelowThreshold;
  off = start_state_vars.off;
  // fill the buffer
  clearBuf();
  for (int i = 0; i < bufLen; i++) {
    uint32_t bufVal = start_state_vars.buf[i];
    if (bufVal != 404) {
      bufPush(bufVal);
    }
  }

  state result_state = updateFsm(start_state, test_state_inputs.mils, test_state_inputs.sensorSignal);
  bool passed_test = (end_state == result_state and
                      // savedClock == end_state_vars.savedClock and
                      // mostRecentHeartbeat == end_state_vars.mostRecentHeartbeat and
                      previouslyBelowThreshold == end_state_vars.previouslyBelowThreshold and
                      off == end_state_vars.off and
                      bufEq(buf, end_state_vars.buf));
  if (!verbos) {
    return passed_test;
  } else if (passed_test) {
    char s_to_print[200];
    sprintf(s_to_print, "Test from %s to %s PASSED", s2str(start_state), s2str(end_state));
    Serial.println(s_to_print);
    return true;
  } else {
    char s_to_print[200];
    Serial.println(s2str(start_state));
    sprintf(s_to_print, "Test from %s to %s FAILED", s2str(start_state), s2str(end_state));
    Serial.println(s_to_print);
    sprintf(s_to_print, "End state expected: %s | actual: %s", s2str(end_state), s2str(result_state));
    Serial.println(s_to_print);
    sprintf(s_to_print, "Inputs: mils %ld | sensorSignal %d", test_state_inputs.mils, test_state_inputs.sensorSignal);
    Serial.println(s_to_print);
    sprintf(s_to_print, "          %24s | %3s | %15s", "previouslyBelowThreshold", "off", "buffer");
    Serial.println(s_to_print);
    char stp2[200];
    buf2str(end_state_vars.buf, stp2);
    sprintf(s_to_print, "expected: %24d | %3d | %15s", end_state_vars.previouslyBelowThreshold, end_state_vars.off, stp2);
    Serial.println(s_to_print);
    buf2str(buf, stp2);
    sprintf(s_to_print, "actual:   %24d | %3d | %15s", previouslyBelowThreshold, off, stp2);
    Serial.println(s_to_print);
    return false;
  }
}

// Test cases
const int num_tests = 10;
const uint32_t null_buf[] = {0, 0, 0, 0, 0};
const state test_states_in[10] = {(state)1, (state)1, (state)2, (state)2, (state)2, (state)3, (state)3, (state)3, (state)4, (state)4};
const state test_states_out[10] = {(state)1, (state)2, (state)2, (state)3, (state)4, (state)2, (state)4, (state)4, (state)1, (state)2};
const state_inputs test_input[10] = {{1000, 5}, {1000, 5}, {1000, 750}, {1000, 850}, {1000, 1000}, {1000, 1000}, {1000, 1000}, {1000, 1000}, {1000, 1000}, {1000, 1000}};
const state_vars test_in_vars[10] = {{false, true, {0, 0, 0, 0, 0}}, {false, false, {0, 0, 0, 0, 0}}, {false, false, {0, 0, 0, 0, 0}}, {true, false, {0, 0, 0, 0, 0}}, {false, true, {0, 0, 0, 0, 0}}, {false, false, {0, 0, 0, 404, 404}}, {false, true, {0, 0, 0, 0, 0}}, {false, false, {0, 0, 0, 0, 0}}, {false, true, {0, 0, 0, 0, 0}}, {false, false, {0, 0, 0, 0, 0}}};
const state_vars test_out_vars[10] = {{false, true, {0, 0, 0, 0, 0}}, {false, false, {0, 0, 0, 0, 0}}, {true, false, {0, 0, 0, 0, 0}}, {false, false, {0, 0, 0, 0, 0}}, {false, true, {0, 0, 0, 0, 0}}, {false, false, {0, 0, 0, 1000, 0}}, {false, true, {0, 0, 0, 0, 0}}, {false, false, {1000, 0, 0, 0, 0}}, {false, true, {0, 0, 0, 0, 0}}, {false, false, {0, 0, 0, 0, 0}}};

/*
 * Runs through all the test cases defined above
 */
bool test_all_tests() {
  for (int i = 0; i < num_tests; i++) {
    Serial.print("Running test ");
    Serial.println(i);
    if (!test_transition(test_states_in[i], test_states_out[i], test_input[i], test_in_vars[i], test_out_vars[i], true)) {
      return false;
    }
    Serial.println();
  }
  Serial.println("All tests passed!");
  return true;
}
