#include "WiFiS3.h"
#include "EmonLib.h" 
#include "MySQL_Connection.h"
#include "MySQL_Cursor.h"
//////////////////////////////
const int ACPin = 1;

#define database 6 //資料庫ID
#define ACrange 25  //電流開關閥值

IPAddress server_addr();  // IP of the MySQL *server* here
char user[] = "";              // MySQL user login username
char password[] = "";        // MySQL user login password
int status = WL_IDLE_STATUS;
// WiFi card example
char ssid[] = "";    // your SSID
char pass[] = "";       // your SSID Password

WiFiClient client;            // Use this for WiFi instead of EthernetClient
MySQL_Connection conn((Client *)&client);

EnergyMonitor emon1;  

unsigned long previousMillis = 0;
const long interval = 300000; // Interval in milliseconds (5 minute)

/* -------------------------------------------------------------------------- */
void setup()
{
  /* -------------------------------------------------------------------------- */
  Serial.begin(115200);
  emon1.current(ACPin, 111.1); 
  for(int i =0;i<10;i++){
    Serial.println(emon1.calcIrms(1480));
    delay(100);
  }
  
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION)
  {
    Serial.println("Please upgrade the firmware");
  }

  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);

    delay(10000);
  }

  
  Serial.println("\nStarting connection to SQLserver...");
  if (conn.connect(server_addr, 3306, user, password)) {
    delay(1000);
  }
  else
    Serial.println("Connection failed.");
}

// Sample query
//char INSERT_SQL[] = "INSERT INTO test_arduino.hello_arduino (message) VALUES ('Hello, Arduino!')";
//UPDATE fishDB.feedalive SET time=NOW(),voltage=ACCurrentValue WHERE id=1;
/* -------------------------------------------------------------------------- */
void loop()
{
/* -------------------------------------------------------------------------- */
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    
    previousMillis = currentMillis;
    
    double ACCurrentValue = emon1.calcIrms(1480);// Calculate 電流 only
    
    // Initiate the query class instance
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    
    
    char update_SQL[100];
    //sprintf(update_SQL, "UPDATE fishDB.feed_alive SET time=NOW(),voltage=%.2f WHERE id=1", ACCurrentValue);
    
    sprintf(update_SQL, "INSERT INTO fishDB.feed_alive (time, voltage) VALUES (CONVERT_TZ(NOW(), '+00:00', '+08:00') , %.2f);", ACCurrentValue);
    cur_mem->execute(update_SQL);
    Serial.print("ACCurrentValue: ");
    Serial.println(ACCurrentValue);
    if(ACCurrentValue>ACrange){
      char *update_mode_SQL="UPDATE fishDB.decision SET mode = 4 WHERE id = 1";
      cur_mem->execute(update_mode_SQL);
      Serial.println("啟動觀察者模式");
    }
    else{
      char *update_mode_SQL="UPDATE fishDB.decision SET mode = 0 WHERE id = 1";
      cur_mem->execute(update_mode_SQL);
      Serial.println("關閉");
    }
      
    
 
    delete cur_mem;



  }//測試使用feed_alive存LOG
}
