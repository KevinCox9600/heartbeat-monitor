char ssid[] = "Brown-Guest";  // network SSID (name)
char pass[] = ""; // for networks that require a password
int status = WL_IDLE_STATUS;  // the WiFi radio's status
WiFiClient client;
char buffer[200];

/**
 * Set up the wifi connection for getting from the server.
*/
void setup_wifi() {
  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.println("Attempting to connect to: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid); // WiFi.begin(ssid, pass) for password
    delay(10000);
  }
  Serial.println("Connected!");
  if (connect_to_get()) {
    Serial.println("fetched webpage");
  } else {
    Serial.println("failed to fetch webpage");
  }
  Serial.println();
}

/**
 * Check if the client is connected.
 * Returns connection status.
*/
bool client_connected() {
  return client.connected();
}

/**
 * Connect to the server and return whether it was a success.
*/
bool connect_to_get() {
  if (client.connect("clasersohn.pythonanywhere.com", 80)) {
    client.println("GET /get HTTP/1.1");
    client.println("Host: clasersohn.pythonanywhere.com");
    client.println("Connection: close");
    client.println();
    return true;
  } else {
    Serial.println("Failed to fetch webpage");
    return false;
  }
}

/**
 * Read the value from the server.
 * Returns an integer value. 
*/
int read_from_get() {
  Serial.println("reading");
  int len = client.available();
  if (len == 0){
    return NULL;
  }
  memset(buffer, 0x0, sizeof(buffer));
  int index = 0;
  int afterHeart = 0;
  while (client.available()) {
    char c = client.read();
    if (!afterHeart || isDigit(c)) {
      buffer[index] = c;
      index++;  
    }
    char *rate = strstr(buffer, "HEART");
    if (rate) {
      memset(buffer, 0x0, sizeof(buffer));
      index = 0;
      afterHeart = 1;
    }
  }
  Serial.println(buffer);

  return atoi(buffer);
  
}

#ifndef TESTING
/**
 * Tries 101 times to read from the server. 
 * If the connection errors, it will return 0.
 * Otherwise, it will return the value of the server (int).
*/
int get_server_message() {
  int msg = read_from_get();
  int count = 0;
  while (msg == 0 && count < 100) {
    if (!client_connected()) {
      delay(1000);
      if (!connect_to_get()) {
        break;
      }
    }
    
    // read the server message
    msg = read_from_get();
    // increment the count
    count = count + 1;
    delay(50);
  }

  return msg;

}
#else 
/**
 * A test version of get_server_message. 
 * Simulate reading from the server.
*/
int get_server_message() {
  Serial.print("reading from server (test): ");
  Serial.println(serverValue);
  return serverValue;
}
#endif
