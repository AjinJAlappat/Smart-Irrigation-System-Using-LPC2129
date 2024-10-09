#include<lpc21xx.h>
#include"lcd_fun.c"
#define SWITCH_PIN    (1 << 10)
#define RELAY_PIN     (1 << 12)
#define MOISTURE_PIN  (1 << 13)

int main()
{	
	unsigned int temp_val;
	//unsigned int moisture_value;

	IODIR0&=~SWITCH_PIN;
	IODIR0|=RELAY_PIN;	// RELAY
	T0PR=14;
	T0MR1=5000000; // FOR TEMPERATURE SENSOR
	T0MCR=(1<<0)|(1<<4)|(1<<3);
	T0TCR=(1<<1);
	T0TCR=(1<<0);

	IODIR0|=(1<<28);
	PINSEL1=(1<<24);
	PINSEL1&=~(1<<25);
	 // for temperature sensor coonfiguration
	// Configure to ADC TO select Channel 1, clk div = 4 --->0 3 MHz , Burst mode , PDN
	ADCR=((1<<0)| (1<<1) |(4<<8)|(1<<16)|(1<<21));

	//For Moisture sensor
	IODIR0|=MOISTURE_PIN; 

	while(1)
	{
		while(!(ADDR&(1<<31))); // 	MONITOR DONE BIT to checck the end of the conversion , Polling statement
		lcd_config();
		lcd_str("Temp = ");
		temp_val=(ADDR&(0X3FF<<6));	// Extract 10 bit digital result
		temp_val=temp_val>>6;		   // to get as LSB
		temp_val=temp_val/3.3;		  // TO GET Equivalent 8 bit value
		lcd_cmd(0x88);
		lcd_num(temp_val);
		delay(1000);

		if(T0IR&(1<<1))
		{
			if((IOPIN0 & MOISTURE_PIN)&&(temp_val<26))
			{
				IOSET0=RELAY_PIN;	 //RELAY
				lcd_cmd(0xC0);
				lcd_str("No Moisture");
				delay(5000);
				IOCLR0=RELAY_PIN;
				delay(1000);
				lcd_cmd(0x01);
			}
			else
			{
				IOCLR0=RELAY_PIN;
			}	
		}
		if(IOPIN0 & SWITCH_PIN)
		{
			// Switch is pressed, turn on relay
            IOSET0 = RELAY_PIN;
			delay(1000);
		}
		else 
		{
            // Switch is released, turn off relay
            IOCLR0 = RELAY_PIN;
        }	

}
}
