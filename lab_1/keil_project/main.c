#include <LPC23xx.h> // Описание LPC23xx

void delay(void)
{
	unsigned int i;
	for (i = 0; i < 0xfffff; i++) {}
}

int main(void)
{
	unsigned int n, k;
 	PINSEL3 = 0x00000000;
 	IODIR1 = 0x1C000000;
 	IOSET1 = 0x1C000000;

 	//26 - клапан
 	//27 - зажигание
 	//28 - стартер

 	while (1) 
 	{
 		if (IOPIN1 & (1 << 29))
		{	
			IOSET1 = (1 << 28);
			IOSET1 = (1 << 26);
			delay();
			IOCLR1 = (1 << 26);
			IOSET1 = (1 << 27);
			delay();
			IOCLR1 = (1 << 27); 
		}
 		else
 		{
			IOCLR1 = (1 << 26) | (1 << 27) | (1 << 28);
 		}
 	}
}