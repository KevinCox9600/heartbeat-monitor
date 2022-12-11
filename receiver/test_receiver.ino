#ifdef TESTING
/*
* A struct to keep state inputs in one place
*/
typedef struct {
 long mils; // the mils
} state_inputs;

/*
* A struct to keep state variables in one place
*/
typedef struct {
 int saved_clock; 
 int server_message;
} state_vars;

/**
 * test_transition header
*/
bool test_transition(
   state start_state,
   state end_state,
   state_inputs inputs,
   state_vars start_vars,
   state_vars end_vars,
   int server_value);

/*        
 * Helper function for printing states
 */
char* s2str(state s) {
  switch(s) {
    case sRECEIVING:
    return "(1) sRECEIVING";
    case sOFF:
    return "(2) sOFF";
    case sDISPLAY_HEARTBEAT:
    return "(3) sDISPLAY_HEARTBEAT";
    case sERROR:
    return "(4) sERROR";
    default:
    return "???";
  }
}

/**
 * Tests a transition from one state to another state.
*/
bool test_transition(
   state start_state,
   state end_state,
   state_inputs inputs,
   state_vars start_vars,
   state_vars end_vars,
   int server_value) {
  savedClock = start_vars.saved_clock;
  serverMessage = start_vars.server_message;

  serverValue = server_value;

  state result_state = updateFsm(start_state, inputs.mils);

  bool test_result = (result_state == end_state &&
        end_vars.saved_clock == savedClock &&
        end_vars.server_message == serverMessage);

  if (!test_result) {
    char s_to_print[200];
    Serial.println(s2str(start_state));
    sprintf(s_to_print, "Test from %s to %s FAILED", s2str(start_state), s2str(end_state));
    Serial.println(s_to_print);
    sprintf(s_to_print, "End state expected: %s | actual: %s", s2str(end_state), s2str(result_state));
    Serial.println(s_to_print);
    sprintf(s_to_print, "Inputs: mils %ld", inputs.mils);
    Serial.println(s_to_print);
    sprintf(s_to_print, "serverValue: %d", serverValue);
    Serial.println(s_to_print);
    sprintf(s_to_print, "          %11s | %15s", "saved_clock", "server_message");
    Serial.println(s_to_print);
    sprintf(s_to_print, "start:    %11d | %15d", start_vars.saved_clock, start_vars.server_message);
    Serial.println(s_to_print);
    sprintf(s_to_print, "expected: %11d | %15d", end_vars.saved_clock, end_vars.server_message);
    Serial.println(s_to_print);
    sprintf(s_to_print, "actual:   %11d | %15d", savedClock, serverMessage);
    Serial.println(s_to_print);
    
  }
  return test_result;
}

// Test every transition
const state test_states_in[10] = {(state) 0, (state) 0, (state) 0, (state) 0, (state) 1, (state) 1, (state) 2, (state) 2, (state) 3, (state) 3};
const state test_states_out[10] = {(state) 0, (state) 1, (state) 2, (state) 3, (state) 1, (state) 0, (state) 0, (state) 2, (state) 0, (state) 3};
const state_inputs test_input[10] = {{100}, {1000}, {1000}, {1000}, {1000}, {3000}, {3000}, {1000}, {7000}, {1000}};
const state_vars test_in_vars[10] = {{50, 0}, {50, 0}, {50, 60}, {0, 0}, {0, 404}, {0, 404}, {0, 404}, {0, 60}, {0, 0}, {0, 0}};
const state_vars test_out_vars[10] = {{50, 0}, {1000, 404}, {1000, 60}, {1000, 0}, {0, 404}, {0, 404}, {0, 404}, {0, 60}, {0, 0}, {0, 0}};
const int server_status[10] = {0, 404, 60, 0, 404, 60, 60, 0, 0, 60};
const int num_tests = 10;


// sRECEIVING->sRECEIVING, mils=100, in: server_message=0, saved_clock=50 out: server_message=0, saved_clock=50
// sRECEIVING->sOFF, mils=1000, in: server_message=0, saved_clock=50 out: server_message=404 (OFF), saved_clock=1000
// sRECEIVING->sDISPLAY_HEARTBEAT, mils=1000, in: server_message=60, saved_clock=50 out: server_message=60, saved_clock=0
// sRECEIVING->sERROR, mils=1000 in: server_message=0, saved_clock=0 out: server_message=60, saved_clock=1000 (make server error)
// sOFF->sOFF, mils=1000 in: server_message=404, saved_clock=0 out: server_message=404, saved_clock=0
// sOFF->sRECEIVING, mils=3000 in: server_message=404, saved_clock=0 out: server_message=60, saved_clock=3000
// sDISPLAY_HEARTBEAT->sDISPLAY_HEARTBEAT, mils=1000 in: server_message=60, saved_clock=0 out: server_message=60, saved_clock=0
// sDISPLAY_HEARTBEAT->sRECEIVING, mils=3000 in: server_message=60, saved_clock=0 out: server_message=60, saved_clock=3000
// sERROR->sERROR, mils=1000 in: server_message=0, saved_clock=0 out: server_message=0, saved_clock=0
// sERROR->sRECEIVING, mils=3000 in: server_message=0, saved_clock=0 out: server_message=60, saved_clock=3000

/*
* Runs through all the test cases defined above
*/
bool test_all_tests() {
 for (int i = 0; i < num_tests; i++) {
   Serial.print("Running test ");
   Serial.println(i);
   if (!test_transition(test_states_in[i], test_states_out[i], test_input[i], test_in_vars[i], test_out_vars[i], server_status[i])) {
     return false;
   }
   Serial.println();
 }
 Serial.println("All tests passed!");
 return true;
}
#endif
