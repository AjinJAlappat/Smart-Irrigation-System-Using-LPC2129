#include<lpc21xx.h>
#include"lcd_fun.c"
#define SWITCH_PIN    (1 << 23)
#define RELAY_PIN     (1 << 21)
#define MOISTURE_PIN  (1 << 22)

int main()
{	
	unsigned int temp_val;
	//unsigned int moisture_value;
	lcd_config();

	IODIR0&=~SWITCH_PIN;
	IODIR0|=RELAY_PIN;	// RELAY
	IOCLR0=RELAY_PIN;
	T0PR=14;
	T0MR0=5000000; // FOR TEMPERATURE SENSOR
	T0MCR=(1<<0)|(1<<1);
	T0TCR=(1<<1);
	T0TCR=(1<<0);

	IODIR0|=(1<<28);
	PINSEL1=(1<<24);
	PINSEL1&=~(1<<25);
	 // for temperature sensor coonfiguration
	// Configure to ADC TO select Channel 1, clk div = 4 --->0 3 MHz , Burst mode , PDN
	ADCR=((1<<1) |(4<<8)|(1<<16)|(1<<21));

	//For Moisture sensor
	IODIR0&=~MOISTURE_PIN; 

	while(1)
	{
		while(!(ADDR&(1<<31))); // 	MONITOR DONE BIT to checck the end of the conversion , Polling statement
		lcd_cmd(0x80);
		lcd_str("Temp = ");
		temp_val=(ADDR&(0X3FF<<6));	// Extract 10 bit digital result
		temp_val=temp_val>>6;		   // to get as LSB
		temp_val=temp_val/3.3;		  // TO GET Equivalent 8 bit value
		lcd_cmd(0x88);
		lcd_num(temp_val);
		delay(1000);
	  /*  lcd_cmd(0xC0);
		lcd_str("No Moisture");	  */
		
		if(T0IR&(1<<0))
		{
			if((IOPIN0 & MOISTURE_PIN)==1/||(temp_val<28)/)
			{
				IOSET0=RELAY_PIN;	 //RELAY
	
				lcd_cmd(0xC0);
				lcd_str("No Moisture");
				//delay(5000);
				//IOCLR0=RELAY_PIN;

				
				lcd_cmd(0x01);
			//	delay(5000);
			}
			else
			{
				lcd_cmd(0x01);
				lcd_cmd(0xC0);
				lcd_str("Moisture Present");
			//	delay(1000);
				IOCLR0=RELAY_PIN;
			}	
	
	
	if(IOPIN0 & SWITCH_PIN)
	{
		// Switch is pressed, turn on relay
        IOSET0 = RELAY_PIN;
		//delay(1000);
	}
	else 
	{
    // Switch is released, turn off relay
       IOCLR0 = RELAY_PIN;
    }
	T0IR=(1<<0);
	}
	}
//	delay(1000);	

}
