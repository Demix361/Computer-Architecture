#include <LPC23xx.H>

#define BIT_BTTN (1<<29)

#define STB 26 //Port1.26
#define CLK 27 //Port1.27
#define DIO 28 //Port1.28

#define STATE_OFF 0
#define STATE_ON 1

#define DIOD_VALVE 1
#define DIOD_IGNITE 2
#define DIOD_STARTER 3

void TimerDelay(void)
{
    T0TC = 0x00000000;
    T0TCR = 0x00000001;
    while (T0TCR&0x1);
}

void delay(unsigned int count)
{
    unsigned int i;
    for (i=0;i<count;i++){}
}

void Timer0_Init(void)
{
    //Предделитель таймера = 600
    T0PR = 600;
    //Сбросить счетчик и делитель
    T0TCR = 0x00000002;
    //При совпадении останавливаем, сбрасываем таймер
    T0MCR = 0x00000006;
    //Регистр совпадения = 1000 (1 Гц)
    T0MR0 = 100;
}

void tm1638_sendbyte(unsigned int x) 
{
    unsigned int i;
    IODIR1 |= (1 << DIO); //Устанавливаем пин DIO на вывод

    for(i = 0; i < 8; i++)
    {
        IOCLR1 = (1 << CLK); //Сигнал CLK устанавливаем в 0
        delay(1); //Задержка 

        if (x & 1)
        {
            IOSET1 = (1 << DIO); //Устанавливаем значение на выходе DIO
        } 
        else
        {
            IOCLR1 = (1 << DIO);
        }

        delay(1); //Задержка
        x >>= 1;
        IOSET1 = (1 << CLK); //Сигнал CLK устанавливаем в 1
        delay(2);           
    }
}


unsigned int tm1638_receivebyte() 
{
    unsigned int i;
    unsigned int x = 0;
    IODIR1 &= ~(1 << DIO); //Устанавливаем пин DIO на ввод

    for(i = 0; i < 32; i++)
    {
        IOCLR1 = (1 << CLK); //Сигнал CLK устанавливаем в 0
        delay(1);

        if (IOPIN1 & (1 << DIO))
        {
            x |= (1 << i);
        }

        delay(1);
        IOSET1 = (1 << CLK); //Сигнал CLK устанавливаем в 1
        delay(2);           
    }

    return x;
}


void tm1638_sendcmd(unsigned int x)
{
    //Устанавливаем пассивный высокий уровень сигнала STB
    IOSET1 = (1 << STB);
    //Устанавливаем пины CLK,DIO,STB на вывод
    IODIR1 = (1 << CLK) | (1 << DIO) | (1 << STB);
    //Устанавливаем активный низкий уровень сигнала STB
    IOCLR1 = (1 << STB);
    tm1638_sendbyte(x);
}


void tm1638_setadr(unsigned int adr)
{
    //Установить адрес регистра LED инидикации
    tm1638_sendcmd(0xC0 | adr); 
}


void tm1638_init()
{
    unsigned int i;
    //Разрешить работу индикации
    tm1638_sendcmd(0x88);   
    //Установить режим адресации: автоинкремент
    tm1638_sendcmd(0x40);
    //Установить адрес регистра LED инидикации
    tm1638_setadr(0);
    //Сбросить все 
    for (i = 0; i <= 0xf; i++)
        tm1638_sendbyte(0);
    //Установить режим адресации: фиксированный
    tm1638_sendcmd(0x44);
}

void SetDiod(unsigned diodNo, int state)
{
    diodNo *= 2;
    diodNo--;
    tm1638_setadr(diodNo); //устанавливаем адрес
    tm1638_sendbyte(state); //шлем данные
}

int main (void)
{
    unsigned i;
    unsigned flag = 0;
    unsigned j = 0;
    unsigned temp = 0;
    
    tm1638_init();
    Timer0_Init();
    
    SetDiod( DIOD_VALVE, STATE_OFF );
    SetDiod( DIOD_IGNITE, STATE_OFF );
    SetDiod( DIOD_STARTER, STATE_OFF );

    while (1) 
    {
        switch(flag)
        {
            case 0:
                 // Включение стартера и попеременное включение и выключение диода зажигания и клапана
                SetDiod(DIOD_STARTER, STATE_ON );
                SetDiod(DIOD_VALVE, STATE_ON );
                TimerDelay();
                SetDiod(DIOD_VALVE, STATE_OFF );
                SetDiod(DIOD_IGNITE, STATE_ON );
                TimerDelay();
                SetDiod(DIOD_IGNITE, STATE_OFF );
                // Обработка нажатия кнопки
                tm1638_sendcmd(0x46);
                i = tm1638_receivebyte();
                if (i == 1) 
                {
                    SetDiod(DIOD_STARTER, STATE_OFF ); // Отключение стартера
                    temp = 1;
                }
                if (temp == 1) // Cчетчик после нажатия кнопки
                    j++;
                if (j == 50) 
                {
                    SetDiod(DIOD_IGNITE, STATE_OFF ); // Отключение зажигания с закрытием клапана
                    SetDiod(DIOD_VALVE, STATE_OFF );
                    flag = 1;
                }
            case 1:
                // Полное отключение
                SetDiod(DIOD_IGNITE, STATE_OFF );
                SetDiod(DIOD_VALVE, STATE_OFF );
                SetDiod(DIOD_STARTER, STATE_OFF );
        }
    }
}
