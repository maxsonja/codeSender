#include <RCSwitch.h>
#include <Convert.h>

Convert convert;
RCSwitch mySwitch = RCSwitch();

struct dataFormat {
  String name;
  String data;
};
//number of saved slots
const byte saveNumb = 10;

dataFormat newData;
dataFormat save[saveNumb];



void sendData (struct dataFormat &inData)
{
  Serial.print("sending '");
  char data[inData.data.length()+1];
  inData.data.toCharArray(data,inData.data.length()+1);
  for(int i = 0; i<inData.data.length(); i++) Serial.print(data[i]);
  Serial.println("'");

}

void formatCode(String inCode, String inFormat)
{
  inCode.trim();
  newData.data="";
  int devider;
  byte format;
  
  inFormat.trim();
  inFormat.toUpperCase();  
  if(inFormat=="DEC") format=0;
  if(inFormat=="HEX") format=1;  
  if(inFormat=="CHAR") format=2;
  
  switch (format)
  {
    case 0: //dec
    do
    {      
        devider=inCode.indexOf(32);
    
        if(devider>-1) newData.data+=(char)convert.toInt(inCode.substring(0, devider));
        else newData.data+=(char)convert.toInt(inCode);
    
        inCode=inCode.substring(devider+1);     
    
    }while(devider>-1);
    break;
    
    case 1: //hex
    do
    {      
        devider=inCode.indexOf(32);
    
        if(devider>-1) newData.data+=(char)convert.toHex(inCode.substring(0, devider));
        else newData.data+=(char)convert.toHex(inCode);
    
        inCode=inCode.substring(devider+1);      
    
    }while(devider>-1);    
    break;
    
    case 2: // char
    newData.data=inCode;
  }
}
  
void saveData(String rawData)
{
  rawData.trim();  
  int devider1=rawData.indexOf(';');
  int devider2=rawData.indexOf(';',devider1+1);
  int devider3=rawData.indexOf(';',devider2+1);
  byte slot;
  
  String rawCode = rawData.substring(0,devider1);
  rawCode.trim();
      
  
  //get slot number
  newData.name = rawData.substring(devider3);
  newData.name.trim();  
  slot=convert.toInt(newData.name);
  
  //get code & format
  newData.name=rawData.substring(devider1+1,devider2);
  newData.name.trim();
  formatCode(rawCode, newData.name);
  
  //get slot name
  newData.name=rawData.substring(devider2+1,devider3);
  newData.name.trim();
      
  //save data
  if(slot>=saveNumb) slot=saveNumb-1;
  else if(slot<0) slot=0;
  save[slot]=newData; 
   
  //printout 
  Serial.print("code: '");
  Serial.print(save[slot].data);
  Serial.print("' named '");
  Serial.print(save[slot].name);
  Serial.print("' saved at slot ");  
  Serial.println(slot);
}

void sendPrep(String rawData)
{
  rawData.trim();
  
  //send by slot number
  if(rawData.substring(0,4)=="slot")
  {
    newData.name=rawData.substring(4);
    newData.name.trim();
    int slotNumb=convert.toInt(newData.name);
    
    if(slotNumb<10)
    {
      if(slotNumb<=0) { newData=save[0]; sendData(newData); }
      else { newData=save[slotNumb]; sendData(newData); }
    }
    else { newData=save[9]; sendData(newData); }
  }
  //
  else
  {
    int devider = rawData.indexOf(';');
    
    //send by name
    if(devider<0) { for(int i = 0; i< saveNumb; i++) { if(rawData == save[i].name){ newData=save[i]; sendData(newData);}} }  
    //send raw code
    else
    {
      newData.data=rawData.substring(0,devider);
      newData.data.trim();
      newData.name=rawData.substring(devider+1);
      newData.name.trim();
      formatCode(newData.data, newData.name);
      newData.name="";
      sendData(newData);
    }
      
  }
  
}

void printData(dataFormat &inData)
{
  if(inData.data=="")
  {
    Serial.println("empty");
  }
  else
  {
  Serial.print(" name: ");
  Serial.print(inData.name);
  Serial.print(" code: ");
  Serial.println(inData.data);
  }
}

void setup()
{
  Serial.begin(9600);
  for(int i = 0;i<saveNumb;i++) save[i]=newData;
}

void loop()
{
  
}

void serialEvent()
{
  String input;
  String command;
  int devider;
  do {    
    delay(10);
    input+=(char)Serial.read();
    delay(10);
  } while(Serial.available());
  
  input.trim();
  devider=input.indexOf(32);
  if(devider<0) command = input;
  else command=input.substring(0,devider);
  input=input.substring(devider+1);
  input.trim();
  command.trim();
  command.toLowerCase();
  
  if(command == "save") {saveData(input);}
  else if (command == "send") {sendPrep(input);}
  else if (command == "repeat" || command== "rep" || command == "resend") {sendData(newData);}
  else if (command == "help") 
  {
    Serial.println(F(""));
    Serial.println(F("-------------------"));
    Serial.println(F("with this script you can send any code by RCSwitch library via serial input"));
    Serial.println(F("code can be of any size but it is preferable to be under 32bytes"));
    Serial.println(F("code can be written in hexadecimal (hex), decimal(dec) or asci characters(char)"));
    Serial.println(F("by defoult you have 10 save slots (from 0-9), but it can be changed by changing saveNumb variable"));
    Serial.println(F("available commands are 'save' ,'send', 'rep' or 'repeat' or 'resend' ,'info' ,'help'"));
    Serial.println(F("-------------------"));
    Serial.println(F("----------"));
    Serial.println(F("saving data:"));
    Serial.println(F("command 'save'"));
    Serial.println(F("[command][space][code][;][format][;][name][;][slot]"));
    Serial.println(F("example:"));
    Serial.println(F("save 12345;char;exp1;0")); 
    Serial.println(F("----------"));
    Serial.println(F("----------"));
    Serial.println(F("sending data:"));
    Serial.println(F("command 'send'"));
    Serial.println(F("[command][space][code][;][format]"));
    Serial.println(F("[command][space][slot number]"));
    Serial.println(F("[command][space][slot name]"));
    Serial.println(F(""));
    Serial.println(F("example:"));
    Serial.println(F(" send 0xff 0xa2 0x45 ; hex ; exmp2 ; 1"));
    Serial.println(F(" send 100 52 10 ; dec ; exmp3 ; 2"));
    Serial.println(F(" send slot1"));
    Serial.println(F(" send exmp3"));
    Serial.println(F("----------"));
    Serial.println(F("----------"));
    Serial.println(F("repeat:"));
    Serial.println(F("last entered code (saved or sent)"));
    Serial.println(F("----------"));
    Serial.println(F("----------"));
    Serial.println(F("info:"));
    Serial.println(F("overview of saved slots"));
    Serial.println(F("you can see only wanted slot by typing 'slot[number]' after 'info'"));
    Serial.println(F("----------"));
    Serial.println(F("----------"));
    Serial.println(F("help:"));
    Serial.println(F("you are reading it :P"));
    Serial.println(F("----------"));
    Serial.println(F("-------------------"));
    Serial.println(F(""));   
  }
  else if(command=="info")
  {
    if(devider>0 && input.substring(0,4)=="slot")
  {
    command=input.substring(4);
    command.trim();
    devider=convert.toInt(command);
    if(devider<=0) devider=0;
    else if (devider>=saveNumb) devider=saveNumb-1;
    Serial.print(F("slot "));
    Serial.print(devider);
    Serial.print(": ");
    printData(save[devider]);
  }
  else 
  {
    for(int i = 0;i<saveNumb;i++)
    {
      Serial.print(F("slot "));
      Serial.print(i);
      Serial.print(": ");
      printData(save[i]);
    }
  }    
  }
}
  
  
