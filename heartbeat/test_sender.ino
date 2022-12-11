// #include "heartbeat.h"

/*
 * A struct to keep all three state inputs in one place
 */
typedef struct {
  uint32_t mils;
  int sensorSignal;
} state_inputs;

/*
 * A struct to keep all 9 state variables in one place
 */
typedef struct {
  // uint32_t savedClock;
  // uint32_t mostRecentHeartbeat;
  bool previouslyBelowThreshold;
  bool off;
  // any global
} state_vars;

bool test_transition(state start_state,
                     state end_state,
                     state_inputs test_state_inputs,
                     state_vars start_state_vars,
                     state_vars end_state_vars,
                     bool fill_the_buffer,
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
                     bool fill_the_buffer,
                     bool verbos) {

  if (fill_the_buffer) {
    const int interval = 990;
    for (int i = 0; i < 7 * interval; i += interval) {
      bufPush(i);
    }
  }

  // set state variables
  // savedClock = start_state_vars.savedClock;
  // mostRecentHeartbeat = start_state_vars.mostRecentHeartbeat;
  previouslyBelowThreshold = start_state_vars.previouslyBelowThreshold;
  off = start_state_vars.off;

  char s_to_print[200];
  sprintf(s_to_print, "Inputs: mils %ld | sensorSignal %d", test_state_inputs.mils, test_state_inputs.sensorSignal);
  Serial.println(s_to_print);

  state result_state = updateFsm(start_state, test_state_inputs.mils, test_state_inputs.sensorSignal);
  bool passed_test = (end_state == result_state and
                      // savedClock == end_state_vars.savedClock and
                      // mostRecentHeartbeat == end_state_vars.mostRecentHeartbeat and
                      previouslyBelowThreshold == end_state_vars.previouslyBelowThreshold and
                      off == end_state_vars.off);
  // passed_test = false;
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
    sprintf(s_to_print, "          %24s | %3s", "previouslyBelowThreshold", "off");
    Serial.println(s_to_print);
    sprintf(s_to_print, "expected: %24d | %3d", end_state_vars.previouslyBelowThreshold, end_state_vars.off);
    Serial.println(s_to_print);
    sprintf(s_to_print, "actual:   %24d | %3d", previouslyBelowThreshold, off);
    Serial.println(s_to_print);
    return false;
  }
}

/*
 * REPLACE THE FOLLOWING 6 LINES WITH YOUR TEST CASES
 */
const int num_tests = 10;
const state test_states_in[10] = {(state)1, (state)1, (state)2, (state)2, (state)2, (state)3, (state)3, (state)3, (state)4, (state)4};
const state test_states_out[10] = {(state)1, (state)2, (state)2, (state)3, (state)4, (state)2, (state)4, (state)4, (state)1, (state)2};
const state_inputs test_input[10] = {{1000, 5}, {1000, 5}, {1000, 750}, {1000, 850}, {1000, 1000}, {1000, 1000}, {1000, 1000}, {1000, 1000}, {1000, 1000}, {1000, 1000}};
const state_vars test_in_vars[10] = {{false, true}, {false, false}, {false, false}, {true, false}, {false, true}, {false, false}, {false, true}, {false, false}, {false, true}, {false, false}};
const state_vars test_out_vars[10] = {{false, true}, {false, false}, {true, false}, {false, false}, {false, true}, {false, false}, {false, true}, {false, false}, {false, true}, {false, false}};
const bool fill_buf[10] = {false, false, false, false, false, false, true, false, false, false};
// const int num_tests = 24;
// const state test_states_in[1] = {(state)1};
// const state test_states_out[24] = {(state)1};
// const state_vars test_out_vars[24] = {{1, 2, UP, 4, 4, 1, 2, 500, 4}, {1, 2, RIGHT, 0, 1, 2, 0, 1600, -1}, {0, 0, DOWN, 0, 16, 0, 1000, 42069, -1}, {0, 0, UP, 0, 16, 0, 1000, 42069, -1}, {0, 0, DOWN, 0, 16, 0, 1000, 42069, 4}, {0, 1, DOWN, 1, 16, 0, 1000, 41000, 1}, {0, 2, UP, 0, 13, 4, 1000, 1000, 2}, {0, 1, DOWN, 0, 16, 0, 4, 0, 1}, {2, 4, DOWN, 4, 4, 3, 1, 3, 2}, {0, 3, RIGHT, 0, 2, 0, 1000, 42000, 4}, {0, 0, LEFT, 1, 16, 0, 1000, 41000, 2}, {0, 0, LEFT, 0, 16, 0, 1000, 41000, 0}, {0, 0, DOWN, 0, 15, 4, 0, 1, 2}, {0, 0, UP, 0, 15, 2, 1, 4, 1}, {0, 0, LEFT, 7, 7, 4, 0, 0, 1}, {2, 4, UP, 0, 2, 2, 1000, 42000, 3}, {1, 0, UP, 1, 15, 4, 950, 42069, 2}, {0, 0, LEFT, 0, 15, 4, 1000, 41000, 1}, {0, 0, RIGHT, 0, 4, 4, 0, 3, 4}, {4, 3, LEFT, 3, 4, 2, 4, 1600, -1}, {0, 0, LEFT, 1, 16, 3, 1000, 41000, 0}, {0, 0, LEFT, 0, 16, 4, 1000, 41000, 0}, {0, 0, DOWN, 0, 15, 0, 0, 1, 4}, {0, 0, UP, 0, 15, 2, 3, 2, 4}};

// const state test_states_in[24] = {(state)1, (state)1, (state)1, (state)1, (state)2, (state)2, (state)2, (state)3, (state)3, (state)4, (state)4, (state)4, (state)5, (state)5, (state)5, (state)6, (state)6, (state)6, (state)7, (state)1, (state)4, (state)4, (state)5, (state)5};
// const state test_states_out[24] = {(state)1, (state)1, (state)2, (state)6, (state)2, (state)3, (state)3, (state)4, (state)7, (state)4, (state)5, (state)5, (state)2, (state)6, (state)7, (state)6, (state)1, (state)7, (state)7, (state)1, (state)5, (state)5, (state)2, (state)6};
// const state_inputs test_input[24] = {{3, RIGHT, 600}, {3, DOWN, 1600}, {0, UP, 42069}, {0, RIGHT, 42069}, {0, DOWN, 42069}, {1, DOWN, 42069}, {0, UP, 2000}, {0, RIGHT, 2}, {0, LEFT, 3}, {0, UP, 42069}, {2, DOWN, 42069}, {0, DOWN, 42069}, {2, RIGHT, 4}, {3, LEFT, 1}, {2, UP, 0}, {4, DOWN, 42069}, {1, UP, 42069}, {2, DOWN, 42069}, {4, DOWN, 3}, {3, DOWN, 1600}, {2, DOWN, 42069}, {0, DOWN, 42069}, {4, DOWN, 3}, {3, LEFT, 0}};
// const state_vars test_in_vars[24] = {{1, 2, UP, 4, 4, 1, 2, 500, 4}, {1, 2, RIGHT, 0, 1, 2, 0, 1000, 0}, {0, 0, DOWN, 0, 16, 0, 1000, 41000, -1}, {0, 0, UP, 0, 16, 0, 1000, 41000, -1}, {0, 0, DOWN, 0, 16, 0, 1000, 42069, 4}, {0, 0, DOWN, 0, 16, 0, 1000, 41000, 1}, {0, 3, UP, 0, 13, 4, 1000, 1000, 2}, {0, 1, DOWN, 0, 16, 0, 4, 0, 1}, {2, 4, DOWN, 4, 4, 3, 1, 3, 2}, {0, 3, RIGHT, 0, 2, 0, 1000, 42000, 4}, {0, 0, DOWN, 0, 16, 0, 1000, 41000, 2}, {0, 0, DOWN, 0, 16, 0, 1000, 41000, 0}, {0, 0, DOWN, 0, 15, 4, 0, 1, 2}, {0, 0, UP, 0, 15, 2, 1, 4, 1}, {0, 0, LEFT, 7, 7, 4, 0, 0, 1}, {2, 4, UP, 0, 2, 2, 1000, 42000, 3}, {5, 0, UP, 1, 15, 3, 1000, 41000, 1}, {0, 0, LEFT, 0, 15, 4, 1000, 41000, 1}, {0, 0, RIGHT, 0, 4, 4, 0, 3, 4}, {4, 3, LEFT, 3, 4, 2, 4, 500, 0}, {0, 0, DOWN, 0, 16, 3, 1000, 41000, 0}, {0, 0, DOWN, 0, 16, 4, 1000, 41000, 0}, {0, 0, DOWN, 0, 15, 0, 0, 1, 4}, {0, 0, UP, 0, 15, 2, 3, 2, 4}};
// const state_vars test_out_vars[24] = {{1, 2, UP, 4, 4, 1, 2, 500, 4}, {1, 2, RIGHT, 0, 1, 2, 0, 1600, -1}, {0, 0, DOWN, 0, 16, 0, 1000, 42069, -1}, {0, 0, UP, 0, 16, 0, 1000, 42069, -1}, {0, 0, DOWN, 0, 16, 0, 1000, 42069, 4}, {0, 1, DOWN, 1, 16, 0, 1000, 41000, 1}, {0, 2, UP, 0, 13, 4, 1000, 1000, 2}, {0, 1, DOWN, 0, 16, 0, 4, 0, 1}, {2, 4, DOWN, 4, 4, 3, 1, 3, 2}, {0, 3, RIGHT, 0, 2, 0, 1000, 42000, 4}, {0, 0, LEFT, 1, 16, 0, 1000, 41000, 2}, {0, 0, LEFT, 0, 16, 0, 1000, 41000, 0}, {0, 0, DOWN, 0, 15, 4, 0, 1, 2}, {0, 0, UP, 0, 15, 2, 1, 4, 1}, {0, 0, LEFT, 7, 7, 4, 0, 0, 1}, {2, 4, UP, 0, 2, 2, 1000, 42000, 3}, {1, 0, UP, 1, 15, 4, 950, 42069, 2}, {0, 0, LEFT, 0, 15, 4, 1000, 41000, 1}, {0, 0, RIGHT, 0, 4, 4, 0, 3, 4}, {4, 3, LEFT, 3, 4, 2, 4, 1600, -1}, {0, 0, LEFT, 1, 16, 3, 1000, 41000, 0}, {0, 0, LEFT, 0, 16, 4, 1000, 41000, 0}, {0, 0, DOWN, 0, 15, 0, 0, 1, 4}, {0, 0, UP, 0, 15, 2, 3, 2, 4}};
// const int num_tests = 24;

/*
 * Runs through all the test cases defined above
 */
bool test_all_tests() {
  for (int i = 0; i < num_tests; i++) {
    Serial.print("Running test ");
    Serial.println(i);
    if (!test_transition(test_states_in[i], test_states_out[i], test_input[i], test_in_vars[i], test_out_vars[i], fill_buf[i], true)) {
      return false;
    }
    Serial.println();
  }
  Serial.println("All tests passed!");
  return true;
}
