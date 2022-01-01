#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
 
// network credentials
const char* ssid     = "__WIFI_SSID__";
const char* password = "__WIFI_PASSWORD__";
 
ESP8266WebServer server(8082);   //instantiate server at port 8082

int pin_D8 = 15;
int pin_D0 = 16;
int pin_D1 = 5;
int pin_D2 = 4;
int pin_D5 = 14;
int pin_D6 = 12;
int pin_D7 = 13;

String page = "";

void setup(void){
  delay(1000);
  Serial.begin(115200);
  Serial.println("\nnodeMcu Started ......");
  Serial.println("Setting up Pins..");
  pinMode(pin_D8, OUTPUT);
  pinMode(pin_D0, INPUT);
  pinMode(pin_D1, INPUT);
  pinMode(pin_D2, INPUT);
  pinMode(pin_D5, INPUT);
  pinMode(pin_D6, INPUT);
  pinMode(pin_D7, INPUT);

  WiFi.begin(ssid, password); //begin WiFi connection
  Serial.println("connecting to ");
  Serial.println(ssid);
 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Mac Address: ");
  Serial.println(WiFi.macAddress());

//the HTML of the web page
page += "<html>";
page += "\n    <head>";
page += "\n        <title>";
page += "\n            Water Level Indicator";
page += "\n        </title>";
page += "\n    </head>";
page += "\n    <body onload='Populate(0)'>";
page += "\n        <h2><span id='date-time'></span></h2>";
page += "\n        <br/>";
page += "\n        <br/>";
page += "\n        <button id='refreshBtn' onclick='Refresh()'>Refresh</button>";
page += "\n        <ul id='list' style='font-size: large;'>";
page += "\n            <li id='7'>FULL !!</li>";
page += "\n            <li id='6'>80%</li>";
page += "\n            <li id='5'>60%</li>";
page += "\n            <li id='4'>50%</li>";
page += "\n            <li id='3'>30%</li>";
page += "\n            <li id='2'>15%</li>";
page += "\n            <li id='1'>EMPTY !!</li>";
page += "\n        </ul>";
page += "\n        <h3 id='err'>Something went wrong !!! please try again after some time..</h3>";
page += "\n    </body>";
page += "\n    <script>";
page += "\n        function showError()";
page += "\n        {";
page += "\n            document.getElementById('list').style = 'visibility: hidden;';";
page += "\n            document.getElementById('err').style = 'visibility: visible;color: red';";
page += "\n            var refreshBtn = document.getElementById('refreshBtn');";
page += "\n            refreshBtn.disabled = false;";
page += "\n            refreshBtn.innerHTML = 'Refresh';";
page += "\n        }";
page += "\n        function Populate(level)";
page += "\n        {";
page += "\n            document.getElementById('date-time').innerHTML=new Date().toLocaleString();";
page += "\n            document.getElementById('list').style = 'visibility: visible;';";
page += "\n            document.getElementById('err').style = 'visibility: hidden;';";
page += "\n            document.getElementById('1').innerHTML = 'EMPTY !!';";
page += "\n            document.getElementById('2').innerHTML = '15%';";
page += "\n            document.getElementById('3').innerHTML = '30%';";
page += "\n            document.getElementById('4').innerHTML = '50%';";
page += "\n            document.getElementById('5').innerHTML = '60%';";
page += "\n            document.getElementById('6').innerHTML = '80%';";
page += "\n            document.getElementById('7').innerHTML = 'FULL !!';";
page += "\n            document.getElementById('1').style = '';";
page += "\n            document.getElementById('2').style = '';";
page += "\n            document.getElementById('3').style = '';";
page += "\n            document.getElementById('4').style = '';";
page += "\n            document.getElementById('5').style = '';";
page += "\n            document.getElementById('6').style = '';";
page += "\n            document.getElementById('7').style = '';";
page += "\n            if(level != '0')";
page += "\n            {";
page += "\n                var element = document.getElementById(level)";
page += "\n                element.innerHTML += '<<<-----------<';";
page += "\n                if (level == '1' || level == '7')";
page += "\n                    element.style = 'font-weight: bold;color: red;';";
page += "\n                else";
page += "\n                element.style = 'font-weight: bold;color: blue;';";
page += "\n            }";
page += "\n            var refreshBtn = document.getElementById('refreshBtn');";
page += "\n            refreshBtn.disabled = false;";
page += "\n            refreshBtn.innerHTML = 'Refresh';";
page += "\n        }";
page += "\n        function Refresh()";
page += "\n        {";
page += "\n            var refreshBtn = document.getElementById('refreshBtn');";
page += "\n            refreshBtn.disabled = true;";
page += "\n            refreshBtn.innerHTML = 'Refreshing...';";
page += "\n            const request = (async () => {";
page += "\n                try";
page += "\n                {";
page += "\n                    const response = await fetch(`${window.location.href}getLevel`);";
page += "\n                    const level = await response.text();";
page += "\n                    if (parseInt(level) > 0 && parseInt(level) < 8)";
page += "\n                        Populate(level);";
page += "\n                    else";
page += "\n                    {";
page += "\n                        console.log('API returned level : ' + level);";
page += "\n                        showError();";
page += "\n                    }";
page += "\n                }";
page += "\n                catch(err)";
page += "\n                {";
page += "\n                    console.log(err);";
page += "\n                    showError();";
page += "\n                }";
page += "\n            }).call();";
page += "\n        }";
page += "\n    </script>";
page += "\n</html>";

  server.on("/", [](){
    server.send(200, "text/html", page);
  });
  
  server.on("/getLevel", [](){
    digitalWrite(pin_D8, HIGH);
    delay(1000);
    String level = "1";
    if (digitalRead(pin_D7) == HIGH)
      level = "7";
    else if (digitalRead(pin_D6) == HIGH)
      level = "6";
    else if (digitalRead(pin_D5) == HIGH)
      level = "5";
    else if (digitalRead(pin_D2) == HIGH)
      level = "4";
    else if (digitalRead(pin_D1) == HIGH)
      level = "3";
    else if (digitalRead(pin_D0) == HIGH)
      level = "2";
    server.sendHeader("Access-Control-Allow-Origin","*");
    server.send(200, "text/html", level);
    digitalWrite(pin_D8, LOW);
  });
  
  server.begin();
  Serial.println("Web server started!");
}
 
void loop(void){
  server.handleClient();
}
