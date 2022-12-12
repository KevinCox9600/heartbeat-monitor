char ssid[] = "Brown-Guest"; // network SSID (name)
char pass[] = "";            // for networks that require a password
int status = WL_IDLE_STATUS; // the WiFi radio's status

/**
 * Set up the wifi connection for posting to the server.
 */
void setup_wifi() {
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid); // WiFi.begin(ssid, pass) for password
    delay(10000);
  }
  Serial.println("Connected!");
  Serial.println();
}

bool post_heartrate_to_website(int rate) {
  String data = "{\"heartrate\": \"" + String(rate) + "\"}";
  if (client.connect("clasersohn.pythonanywhere.com", 80)) {
    client.println("POST /post HTTP/1.1");
    client.println("Host: clasersohn.pythonanywhere.com");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.println(data);
    client.println("Connection: close");
    return true;
  } else {
    Serial.println("Failed to fetch webpage");
    return false;
  }
}

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

void read_from_get() {
  int len = client.available();
  if (len == 0) {
    return;
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
}
