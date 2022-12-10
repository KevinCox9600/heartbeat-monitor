#include "receiver.h"

/*
 * A struct to keep state inputs in one place
 */
typedef struct {
  long mils;
} state_inputs;

/*
 * A struct to keep state variables in one place
 */
typedef struct {
  int saved_clock;
  int server_message;
} state_vars;

bool test_transition(
    state start_state,
    state end_state,
    state_inputs inputs,
    state_vars start_vars,
    state_vars end_vars) {
  savedClock = start_vars.saved_clock;
  serverMessage = start_vars.server_message;

  state result_state = updateFsm(start_state, start_state.mils);

  return (result_state == end_state &&
          end_state.saved_clock == savedClock &&
          end_state.server_message == serverMessage);
}


const state test_states_in[12] = {sRECEIVING, sRECEIVING, sRECEIVING, sRECEIVING, sOFF, sOFF, sDISPLAY_HEARTBEAT, sDISPLAY_HEARTBEAT, sERROR, sERROR, sFLATLINE, sFLATLINE};
const state test_states_out[12] = {sRECEIVING, sOFF, sDISPLAY_HEARTBEAT, sERROR, sOFF, sRECEIVING, sRECEIVING, sDISPLAY_HEARTBEAT, sRECEIVING, sERROR, sRECEIVING, sFLATLINE};
const state_inputs test_input[12] = {{100}, {1000}, {1000}, {1000}, {1000}, {3000}, {1000}, {3000}, {1000}, {3000}, {1000}, {3000}};
const state_vars test_in_vars[12] = {{50, 0}, {50, 0}, {50, 60}, {0, 0}, {0, 0}, {0, 404}, {0, 404}, {0, 60}, {0, 0}, {0, 0}, {0, 444}, {0, 444}};
const state_vars test_out_vars[12] = {{50, 0}, {1000, 404}, {0, 60}, {1000, 60}, {0, 404}, {3000, 60}, {0, 60}, {3000, 60}, {0, 0}, {3000, 60}, {0, 444}, {3000, 60}};
const int num_tests = 12;


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
    if (!test_transition(test_states_in[i], test_states_out[i], test_input[i], test_in_vars[i], test_out_vars[i])) {
      return false;
    }
    Serial.println();
  }
  Serial.println("All tests passed!");
  return true;
}