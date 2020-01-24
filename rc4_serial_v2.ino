#include <EEPROM.h>
#include <string.h>
#define S_SWAP(a,b) do { int t = S[a]; S[a] = S[b]; S[b] = t; } while(0)

/*
* ELIXOR v2.0
* @AUTHOR sleepyeos
* jan-23-2020
*
* RC4 implementation by shirokuade
* https://github.com/shirokuade/RC4-Arduino
*/


char key[256];
unsigned char S[256];
char buffer[256];
char sdata[256];

void setup() {
  Serial.begin(9600);
  delay(1000); 
}

void loop(){
    
  int response = getMenuSelection();
  
  if(response == 1) {
    encryptAndStore();
  } else if(response == 2) {
     decryptAndDisplay(); 
  }  else if(response == 3) {
     while(1); 
  }

}

void encryptAndStore() {
  Serial.println("\n\n[Info] WAITING FOR BTC PRIVATE KEY...");
  getSourceData(sdata);
  
  Serial.println("\n\n[Info] WAITING FOR ENCRYPTION KEY...");
  getSourceData(key);

  
  Serial.print("Source: ");
  Serial.println(sdata);
  
  rc4(key,sdata);
  
  Serial.print("Encrypted: ");
  Serial.println(buffer);
  Serial.println("\n\n");
  
  Serial.println("[i] Writing...");

  for(int i = 0; i < sizeof(sdata); i++) {
     EEPROM.write(i, buffer[i]); 
  }
  
  Serial.println("[+] Written to EEPROM.\n\n\n");
  
}

void decryptAndDisplay() {
  
  
  
  Serial.println("[+] Reading from EEPROM...");

  char stored[256];
  for(int i = 0; i < sizeof(sdata); i++) {
     stored[i] = EEPROM.read(i);
  }
  
  Serial.println("\n\n[i] WAITING FOR ENCRYPTION KEY...");
  getSourceData(key);
  
  rc4(key,stored);
  Serial.print("[+] Decrypted: ");
  Serial.println(buffer);
  
}

int getMenuSelection() {
  Serial.println("[1] WRITE NEW KEY");
  Serial.println("[2] RETRIEVE KEY");
  Serial.println("[3] EXIT");
  
  while(!Serial.available());
  delay(2000);
  
  int response = Serial.parseInt();
  return response;
}

void getSourceData(char *sd) {
  
  while(!Serial.available());
  delay(2000);
  
  String st = Serial.readString();
  st.toCharArray(sd, st.length() + 1);
}

void rc4(char *key, char *data){
     int i,j;
          
     for (i=0;i<256;i++){
         S[i] = i;
     }
     
     j = 0;
     for (i=0;i<256;i++){
         j = (j+S[i]+key[i%strlen(key)]) %256;    
         S_SWAP(S[i],S[j]);
     }
     
     i = j = 0;
     for (int k=0;k<strlen(data);k++){
         i = (i+1) %256;
         j = (j+S[i]) %256;
         S_SWAP(S[i],S[j]);
         buffer[k] = data[k]^S[(S[i]+S[j]) %256];
     }
     buffer[strlen(data)+1] = '\0';
}


