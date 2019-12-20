#include<SPI.h>
#include<Ethernet.h>
#include<SD.h>
#include<Chrono.h>

#define REQ_BUF_SZ 50

// Define stepper motor connections and steps per revolution:
int dirPin = 7;  // BLUE CABLE: Dig-Port 7
int stepPin = 6; // PURPLE CABLE: Dig-Port 6
int stepsPerRevolution = 200;
int upstream_switch = 3; //Dig-Port for upstream switch
int downstream_switch = 2; //Dig-Port for downstream switch
int val_up = 1;
int val_down = 1;
int Delay = 300; // Time delay between half-steps (microseconds)
int max_count = 666;
long int time_limit = 180000;

//Ethernet set-up
int led = 8;
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; //Arduino MAC Address
EthernetServer Server(80);
File webFile;
char HTTP_req[REQ_BUF_SZ] = {0};
char req_index = 0;

Chrono chronoD2U;
Chrono chronoU2D;
Chrono chrono_response;

void setup() {

  // disable Ethernet chip
  pinMode(53, OUTPUT);
  digitalWrite(53, HIGH);

  // Ethernet set-up:
  Serial.begin(9600);

  // initialize SD card
  Serial.println("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }
  Serial.println("SUCCESS - SD card initialized.");
  // check for index.htm file
  if (!SD.exists("index.htm")) {
    Serial.println("ERROR - Can't find index.htm file!");
    return;  // can't find index file
  }
  Serial.println("SUCCESS - Found index.htm file.");
  Ethernet.begin(mac);
  Server.begin();
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());

  //Stepper set-up
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  //Limit Switches set-up
  pinMode(upstream_switch, INPUT);
  pinMode(downstream_switch, INPUT);

}

void loop() {
  val_up = digitalRead(upstream_switch);
  val_down = digitalRead(downstream_switch);

  EthernetClient client = Server.available();

  if (client) {

    boolean currentLineIsBlank = true;

    while (client.connected()) {

      if (client.available()) {

        char c = client.read();

        if (req_index < (REQ_BUF_SZ - 1)) {
          HTTP_req[req_index] = c;
          req_index++;
        }

        if (c == '\n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");

          if (StrContains(HTTP_req, "buttonD2U")) {

            chronoD2U.restart();
            long int count = 0;
            int timeout = 0;

            digitalWrite(led, HIGH);

            // Set the spinning direction clockwise:
            digitalWrite(dirPin, HIGH);

            // Turn motor if FC has not reached destination:
            while (val_up == 1) {

              if (chronoD2U.hasPassed(time_limit)) {
                timeout = 1 ;
                Serial.print("Error, took too long, did not reach target destination\n");
                client.println("Content-Type: text/xml");
                client.println("Connection: keep-alive");
                client.println();
                XML_response(client, val_up, val_down, timeout);
                Serial.print("Error, took too long, did not reach target destination\n");
                break;
              }

              else {
                TakeAStep(Delay);
                count += 1;
                val_up = digitalRead(upstream_switch);

              }
            }

            if (val_up == 0) {
              client.println("Content-Type: text/xml");
              client.println("Connection: keep-alive");
              client.println();
              XML_response(client, val_up, val_down, timeout);
              Serial.print("Success \n");
              Serial.print("Count = ");
              Serial.print(int(count / stepsPerRevolution));
              Serial.print("\n");
            }

            else {
            }

            delay(1);


          }

          else if (StrContains(HTTP_req, "buttonU2D")) {
            
            chronoU2D.restart();
            long int count = 0;
            int timeout = 0;
            digitalWrite(led, LOW);
            // Set the spinning direction counterclockwise:
            digitalWrite(dirPin, LOW);
            while (val_down == 1) {

              if (chronoU2D.hasPassed(180000)) {
                timeout = 1 ;
                Serial.print("Error, took too long, did not reach target destination\n");
                client.println("Content-Type: text/xml");
                client.println("Connection: keep-alive");
                client.println();
                XML_response(client, val_up, val_down, timeout);

                break;
              }

              else {
                TakeAStep(Delay);
                count += 1;
                val_down = digitalRead(downstream_switch);

              }
            }

            if (val_down == 0) {
              client.println("Content-Type: text/xml");
              client.println("Connection: keep-alive");
              client.println();
              XML_response(client, val_up, val_down, timeout);
              Serial.print("Success \n");
              Serial.print("Count = ");
              Serial.print(int(count/stepsPerRevolution));
              Serial.print("\n");

            }

            else {

            }

          }


          else {  // web page request
            // send rest of HTTP header
            client.println("Content-Type: text/html");
            client.println("Connection: keep-alive");
            client.println();
            // send web page
            webFile = SD.open("index.htm");        // open web page file
            if (webFile) {
              while (webFile.available()) {
                client.write(webFile.read()); // send web page to client
              }
              webFile.close();
            }
          }


          // display received HTTP request on serial port
          Serial.print(HTTP_req);
          // reset buffer index and all buffer elements to 0
          req_index = 0;
          StrClear(HTTP_req, REQ_BUF_SZ);
          break;
        }

        if (c == '\n') {
          // last character on line of received text
          // starting new line with next character read
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // a text character was received from client
          currentLineIsBlank = false;
        }
      } // end if (client.available())
    }// end while (client.connected())


    delay(1);
    client.stop();


  }
}



void TakeAStep(int d) {
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(d);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(d);
}

void XML_response(EthernetClient cl, int upstream_switch, int downstream_switch, int timeout)
{

  cl.print("<?xml version = \"1.0\" ?>");
  cl.print("<inputs>");
  cl.print("<upstream_switch>");
  cl.print(upstream_switch);
  cl.print("</upstream_switch>");
  cl.print("<downstream_switch>");
  cl.print(downstream_switch);
  cl.print("</downstream_switch>");
  cl.print("<timeout>");
  cl.print(timeout);
  cl.print("</timeout>");
  cl.print("</inputs>");

}

// sets every element of str to 0 (clears array)
void StrClear(char *str, char length)
{
  for (int i = 0; i < length; i++) {
    str[i] = 0;
  }
}

// searches for the string sfind in the string str
// returns 1 if string found
// returns 0 if string not found
char StrContains(char *str, char *sfind)
{
  char found = 0;
  char index = 0;
  char len;

  len = strlen(str);

  if (strlen(sfind) > len) {
    return 0;
  }
  while (index < len) {
    if (str[index] == sfind[found]) {
      found++;
      if (strlen(sfind) == found) {
        return 1;
      }
    }
    else {
      found = 0;
    }
    index++;
  }

  return 0;
}
