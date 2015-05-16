#define RainPin 5

//Rain variable
bool RainHigh=false;
const float LowAmt=5.0; //when rain is low, takes this ml to trip
const float HiAmt=5.0;    //when rain is high, takes this ml to trip
float RainAccum=0.0;     //Rain accumulator since start of sample

void setup(void) {
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
}

if ((RainHigh==true)&&(digitalRead(RainPin)==LOW))
{
   RainHigh=false;
   RainAccum+=HiAmt;
}
}
