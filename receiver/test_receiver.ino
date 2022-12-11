#ifdef TESTING
/*
* A struct to keep state inputs in one place
*/
typedef struct {
 long mils; // the mils
} state_inputs;

/**
* A struct to hold testing vals related to failing the server.
*/
typedef struct {
 bool fail_to_connect_client;
 bool fail_to_connect_get;
 bool fail_to_read_get;
} server_errors;

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
   bool fail_server);

/**
 * Tests a transition from one state to another state.
*/
bool test_transition(
   state start_state,
   state end_state,
   state_inputs inputs,
   state_vars start_vars,
   state_vars end_vars,
   bool fail_server) {
  savedClock = start_vars.saved_clock;
  serverMessage = start_vars.server_message;

  failServer = fail_server;

  state result_state = updateFsm(start_state, inputs.mils);

  bool test_result = (result_state == end_state &&
        end_vars.saved_clock == savedClock &&
        end_vars.server_message == serverMessage);

  if (!test_result) {
    Serial.print("Start state: ");
    Serial.println(start_state);
    Serial.print("Expected end state: ");
    Serial.println(end_state);
    Serial.print("Actual end state: ");
    Serial.println(result_state);
  }
}


const state test_states_in[10] = {(state) 0, (state) 0, (state) 0, (state) 0, (state) 1, (state) 1, (state) 2, (state) 2, (state) 3, (state) 3};
const state test_states_out[10] = {(state) 0, (state) 1, (state) 2, (state) 3, (state) 1, (state) 0, (state) 0, (state) 2, (state) 0, (state) 3};
const state_inputs test_input[10] = {{100}, {1000}, {1000}, {1000}, {1000}, {3000}, {1000}, {3000}, {1000}, {3000}};
const state_vars test_in_vars[10] = {{50, 0}, {50, 0}, {50, 60}, {0, 0}, {0, 0}, {0, 404}, {0, 404}, {0, 60}, {0, 0}, {0, 0}};
const state_vars test_out_vars[10] = {{50, 0}, {1000, 404}, {0, 60}, {1000, 60}, {0, 404}, {3000, 60}, {0, 60}, {3000, 60}, {0, 0}, {3000, 60}};
const bool server_status[10] = {false, false, false, true, false, false, false, false, false, false};
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
// sFLATLINE->sFLATLINE, mils=1000 in: server_message=444, saved_clock=0 out: server_message=444, saved_clock=0
// sFLATLINE->sRECEIVING, mils=3000 in: server_message=444, saved_clock=0 out: server_message=60, saved_clock=3000

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
