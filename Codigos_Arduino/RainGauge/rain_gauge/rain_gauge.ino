#define RainPin 5

//Rain variable
bool RainHigh=false;
const float LowAmt=1.0; //when rain is low, takes this ml to trip
const float HiAmt=1.0;    //when rain is high, takes this ml to trip
float RainAccum=0.0;     //Rain accumulator since start of sample
float DiezMl=0.0;       //10 mL accumulator

void setup(void) {
  Serial.begin(9600);  //Start the serial connection 
  // Rain get start state
   if (digitalRead(RainPin)==HIGH)
    {
        RainHigh=true;
    }
   else
   {
        RainHigh=false;
    }
}

void loop(void)
{

// Rain calculator, looks for Rain continuously
// Look for low to high
if ((RainHigh==false)&&(digitalRead(RainPin)==HIGH))
{
   RainHigh=true;
   RainAccum+=LowAmt;
   if (RainAccum==10)
   { 
     DiezMl+=1;
     RainAccum=0;
     }
}

if ((RainHigh==true)&&(digitalRead(RainPin)==LOW))
{
   RainHigh=false;
   RainAccum+=HiAmt;
   if (RainAccum==10)
   { 
     DiezMl+=1;
     RainAccum=0;
     }
     
}
  Serial.print("Acumulador: ");
  Serial.print(RainAccum);
  Serial.print("\t");
  Serial.print("10 mL Acumulador: ");
  Serial.println(DiezMl);
  
  delay(2000);
}
