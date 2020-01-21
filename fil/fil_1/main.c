#include <LPC23xx.H> // Описание LPC23xx

void delay(void) {
	unsigned int i;
	for (i=0;i<0xfffff;i++){}
}

int main(void) {
	unsigned int n, k;
 	PINSEL3 = 0x00000000;
 	IODIR1 = 0x1C000000;
 	IOSET1 = 0x1C000000;
 	while (1) {
 		if (IOPIN1 & (1<<29))
		{
			IOSET1 = (1<<26);
			IOCLR1 = (1<<27);
			IOCLR1 = (1<<28);
		}
 		else{
			IOSET1 = (1<<27);
			IOSET1 = (1<<28);
 		}
 	}
}