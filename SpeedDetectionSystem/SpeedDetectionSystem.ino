#include<RFID.h>
#include<SPI.h>
#include <LiquidCrystal.h>

RFID rfid1(10, 9);
RFID rfid2(4, 5);

const int rs = 0, en = 1, d4 = 7, d5 = 6, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int leftButton = 0;
int upButton = 0;
int downButton = 0;
int rightButton = 0;

bool isTagRead = false;

double entranceExitTimerV1_1 = 0;
double entranceExitTimerV1_2 = 0;
double entranceExitTimerV2_1 = 0;
double entranceExitTimerV2_2 = 0;
double entranceExitTimerV3_1 = 0;
double entranceExitTimerV3_2 = 0;

short int vehicleNumberList[10];
short int vehicleExceedingRatioList[10];
short int penaltyValueList[10];
short int vehicleRegisterIndex = 0;
short int scrollRowIndex = 0; // to control cursor while looking speed tickets

double vehicleSpeed = 0;
int vehicleExceedingRatio = 0;

int laneLength = 200; // in cm

void setup() {
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);

  // Serial.begin(9600);
  lcd.begin(20, 4);
  SPI.begin();
  rfid1.init();
  rfid2.init();

  SetCursorAndPrint(0, 0, "EE402 Finishing     ");
  SetCursorAndPrint(0, 1, "Project CS/UG       ");
  
  delay(2000);
  SetCursorAndPrint(0, 0, "Welcome to Speed");
  SetCursorAndPrint(0, 1, "Detection System");
}

void loop() 
{ 
  leftButton = digitalRead(A0);
  upButton = digitalRead(A1);
  downButton = digitalRead(A2);
  rightButton = digitalRead(A3);

  DetectButton();
  CheckIfVehiclePresentWithRFID();
}

void DetectButton()
{
  if (leftButton == HIGH)
  {
    lcd.clear();
    SetCursorAndPrint(0, 0, "Waiting for vehicle.");
    SetCursorAndPrint(0, 1, "0.8 m/s for Car     ");
    SetCursorAndPrint(0, 2, "1 m/s for Motor     ");
    SetCursorAndPrint(0, 3, "0.5 m/s for Truck   ");
  }
  else if (upButton == HIGH)
  {
    ScrollUp();
  }
  else if (downButton == HIGH)
  {
    ScrollDown();
  }
  else if (rightButton == HIGH)
  {
    ShowSpeedTicketsAsList();
  }
}

void CheckIfVehiclePresentWithRFID()
{
  if ((rfid1.isCard() && rfid1.readCardSerial()) && !isTagRead)
  {
    switch (rfid1.serNum[0])
    {
      case 83:
      entranceExitTimerV1_1 = millis();
      break;
      case 38:
      entranceExitTimerV2_1 = millis();
      break;
      case 163:
      entranceExitTimerV3_1 = millis();
      break;
    } 

    if ((entranceExitTimerV1_1 > 0 && entranceExitTimerV1_2 > 0) ||
        (entranceExitTimerV2_1 > 0 && entranceExitTimerV2_2 > 0) ||
        (entranceExitTimerV3_1 > 0 && entranceExitTimerV3_2 > 0)) 
    {
      switch (rfid1.serNum[0])
      {
        case 83:
        DetectVehicle(1); // called as car
        break;
        case 38:
        DetectVehicle(2); // called as motor
        break;
        case 163:
        DetectVehicle(3); // called as truck
        break;
      }  
    } 
  }
  else if ((rfid2.isCard() && rfid2.readCardSerial()) && !isTagRead)
  {
    switch (rfid2.serNum[0])
    {
      case 83:
      entranceExitTimerV1_2 = millis();
      break;
      case 38:
      entranceExitTimerV2_2 = millis();
      break;
      case 163:
      entranceExitTimerV3_2 = millis();
      break;
    }

    if ((entranceExitTimerV1_1 > 0 && entranceExitTimerV1_2 > 0) ||
        (entranceExitTimerV2_1 > 0 && entranceExitTimerV2_2 > 0) ||
        (entranceExitTimerV3_1 > 0 && entranceExitTimerV3_2 > 0)) 
    {
      switch (rfid2.serNum[0])
      {
        case 83:
        DetectVehicle(1); // called as car
        break;
        case 38:
        DetectVehicle(2); // called as motor
        break;
        case 163:
        DetectVehicle(3); // called as truck
        break;
      }    
    }
  }

  if (rfid1.readCardSerial() == 0 && rfid2.readCardSerial() == 0) 
  {
    isTagRead = false;
  }

  rfid1.halt();
  rfid2.halt(); // to end the execution of rfid1
}

void DetectVehicle(int vehicleType)
{
  switch(vehicleType)
  {
    case 1:
    CalculateVehicleSpeedAndRatio(1, 0.1, entranceExitTimerV1_2 - entranceExitTimerV1_1); // for car
    entranceExitTimerV1_2 = 0; entranceExitTimerV1_1 = 0;
    break;
    case 2:
    CalculateVehicleSpeedAndRatio(2, 0.1, entranceExitTimerV2_2 - entranceExitTimerV2_1); // for motor
    entranceExitTimerV2_2 = 0; entranceExitTimerV2_1 = 0;
    break;
    case 3:
    CalculateVehicleSpeedAndRatio(3, 0.6, entranceExitTimerV3_2 - entranceExitTimerV3_1); // for truck
    entranceExitTimerV3_2 = 0; entranceExitTimerV3_1 = 0;
    break;
  }

  isTagRead = true;
  // delay(500); // to prevent multiple execution of method due to exposure of sensor towards RFID
}

void CalculateVehicleSpeedAndRatio(int vehicleNumber, double vehicleSpeedLimit, double timeSpent)
{

  vehicleSpeed = (laneLength / 100) / (abs(timeSpent) / 1000);
  vehicleExceedingRatio = ((vehicleSpeed - vehicleSpeedLimit) / vehicleSpeedLimit * 100);

  CheckIfVehicleExceedsLimit(vehicleNumber, vehicleSpeed, vehicleSpeedLimit);
}

void CheckIfVehicleExceedsLimit(int vehicleNumber, int vehicleSpeed, double vehicleSpeedLimit)
{
  if (vehicleSpeed > (1.1 * vehicleSpeedLimit)) 
  {
    if (vehicleSpeed > (1.5 * vehicleSpeedLimit))
    {
      ShowSpeedTicket(vehicleNumber, vehicleSpeed, vehicleExceedingRatio, 1823);
      vehicleRegisterIndex++;
    }
    else if ((vehicleSpeed > 1.3 * vehicleSpeedLimit) && (vehicleSpeed < 1.5 * vehicleSpeedLimit))
    {
      ShowSpeedTicket(vehicleNumber, vehicleSpeed, vehicleExceedingRatio, 888);
      vehicleRegisterIndex++;
    }
    else if ((vehicleSpeed > 1.1 * vehicleSpeedLimit) && (vehicleSpeed < 1.3 * vehicleSpeedLimit))
    {
      ShowSpeedTicket(vehicleNumber, vehicleSpeed, vehicleExceedingRatio, 427);
      vehicleRegisterIndex++;
    }
  }
  else 
  {
    lcd.clear();
    SetCursorAndPrint(0, 0, "Vehicle did not get ");
    SetCursorAndPrint(0, 1, "caught by radar.    ");
  }
}

void ShowSpeedTicket(int vehicleNumber, double vehicleSpeed, int vehicleExceedingRatio, int penaltyValue)
{
    lcd.clear();
    // lcd.setCursor(0, 0);
    // lcd.println("V" + (String)vehicleNumber + " > " + (String)vehicleExceedingRatio + "% "
    //  + (String)penaltyValue + " tl");
    
    lcd.setCursor(0, 0);
    lcd.print("Type: V" + (String)vehicleNumber);
    lcd.setCursor(0, 1);
    lcd.print("Exceeded by " + (String)vehicleExceedingRatio + "%    ");
    lcd.setCursor(0, 2);
    lcd.print("Penalty: " + (String)penaltyValue + " tl    ");

    RegisterPenalty(vehicleNumber, vehicleSpeed, vehicleExceedingRatio, penaltyValue);

    // for serial monitor
    // Serial.println("V" + (String)vehicleNumber + " > " + "Speed: " + (String)vehicleSpeed 
    // + " m/s Exceeding Percentage: " + vehicleExceedingRatio + "% " + "Penalty: " + penaltyValue + " tl");

}

int RegisterPenalty(int vehicleNumber, double vehicleSpeedOf, int vehicleExceedingRatio, short int penaltyValue) // check for return
{
  vehicleExceedingRatioList[vehicleRegisterIndex] = vehicleExceedingRatio;
  penaltyValueList[vehicleRegisterIndex] = penaltyValue;
  vehicleNumberList[vehicleRegisterIndex] = vehicleNumber;
  return vehicleExceedingRatioList[vehicleRegisterIndex]; 
}

void ShowSpeedTicketsAsList()
{
  // vehicles will be names as V1 -> car, V2 -> motor, V3 -> truck
  // V1 -> 40% 888 tl
  
  for (int i = 0; i < 4; i++) // will be i < 4 after screen
  {
    lcd.setCursor(0, i);
    lcd.print("V" + (String)vehicleNumberList[scrollRowIndex + i] + " > " + (String)vehicleExceedingRatioList[scrollRowIndex + i] + "% "
      + (String)penaltyValueList[scrollRowIndex + i] + " tl   ");
  }
}

void ScrollUp()
{
  if (vehicleRegisterIndex > 4) 
  {
    if (scrollRowIndex > 0) { scrollRowIndex--; } 
    ShowSpeedTicketsAsList();
    delay(300);
  }
}

void ScrollDown()
{
  if (vehicleRegisterIndex > 2) 
    {
      if (scrollRowIndex < vehicleRegisterIndex - 4) { scrollRowIndex++; } // vehicleRegisterIndex - 4 after screen
      ShowSpeedTicketsAsList();
      delay(300);
    }
}

void SetCursorAndPrint(int row, int col, char text[]) // text type was String
{
  lcd.setCursor(row, col);
  lcd.print(text);
}

