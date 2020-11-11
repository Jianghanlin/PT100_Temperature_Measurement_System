#include "msp430g2553.h"
#define SET_RS P1OUT |= BIT3
#define RST_RS P1OUT &= ~BIT3
#define SET_RW P1OUT |= BIT2
#define RST_RW P1OUT &= ~BIT2
#define SET_E P1OUT |= BIT1
#define RST_E P1OUT &= ~BIT1

unsigned char a[] = {"Temperature"};
unsigned char b[] = {"Detection"};
unsigned char c[] = {"Designed by JHL"};
unsigned char d[] = {"17EIE Xb17610107"};
unsigned char e[] = {"Temp:"};
int temp=0,xiaoshu=0;
void ADC10_init(void);
void display_normal();
void LCD_Write_Command(unsigned char com); //д�����
void LCD_Write_Data(unsigned char dat); //д���ݺ���
void lcd1602_init();
void lcd1602_display();

int main()
{
    float Vin=0,sum=0;
    char count=0;
    WDTCTL = WDTPW + WDTHOLD; //�ؿ��Ź�
    lcd1602_init();
    ADC10_init();
    display_normal();
    while(1)
    {
      ADC10CTL0 |= ENC + ADC10SC; //����ѭ���п��� ADC10 ת��
      _bis_SR_register(CPUOFF + GIE); //���߿����жϣ��ȴ� ADC10 ת����ɺ󣬽����ж����л��� CPU ����

      Vin=(ADC10MEM*2.5*40)/1023;  //��ѹ���ʽ��������40��
      sum=Vin+sum;
      count++;
      if(count==10)
      {
          Vin=sum/10.0;
          if(Vin>20)
          Vin=-0.00002*Vin*Vin+1.0364*Vin-0.2177;
          else
          Vin=0.0016*Vin*Vin+1.0323*Vin-0.3848;
          temp=(int)Vin;
          xiaoshu=(Vin-temp)*10;
          lcd1602_display();
          sum=0;
          count=0;
      }
    }
}

void display_normal()//��ʾѧ��
{
    unsigned char i=0;
    LCD_Write_Command(0x80);
    LCD_Write_Command(0x80 + 0x05); //����ָ�����ã���һ����ʾ
    __delay_cycles(1000);
    for (i = 0; i < sizeof(a); i++)
    {
        LCD_Write_Data(a[i]);
    }
    LCD_Write_Command(0x80 + 0x47); //����ָ�����ã��ڶ�����ʾ
    __delay_cycles(1000);
    for (i = 0; i< sizeof(b); i++)
    {
        LCD_Write_Data(b[i]);
    }
    __delay_cycles(1000000);
    LCD_Write_Command(0x01);//����ָ��1

    LCD_Write_Command(0x80);
    LCD_Write_Command(0x80 + 0x01); //����ָ�����ã���һ����ʾ
    __delay_cycles(1000);
    for (i = 0; i < sizeof(c); i++)
    {
        LCD_Write_Data(c[i]);
    }
    __delay_cycles(1000);
    LCD_Write_Command(0x40+0x80);//ѧ�Ŵӵ�2�е�7����ʼ��ʾ,��ʾ��ַ+ָ��
    for (i = 0; i < sizeof(d); i++)
    {
        LCD_Write_Data(d[i]);
    }
    __delay_cycles(1000000);
    LCD_Write_Command(0x01);//����ָ��1

    LCD_Write_Command(0x80);
    LCD_Write_Command(0x80+0x03);
    __delay_cycles(1000);
    for (i = 0; i < sizeof(e); i++)
    {
        LCD_Write_Data(e[i]);
    }

    LCD_Write_Command(0x80);
    LCD_Write_Command(0x80+0x0E);
    __delay_cycles(1000);
    LCD_Write_Data(0xdf);
    LCD_Write_Data(0x43);//��ʾ���λ
}

void ADC10_init(void)
{
    ADC10CTL0 &= ~ENC;
    ADC10CTL0 = ADC10IE|ADC10ON | REFON| REF2_5V | ADC10SHT_0 | SREF_1;
    //�����ڲ�2.5V�ο���ѹ����������ʱ��Ϊ4 x ADC10CLKs
    ADC10CTL1 = CONSEQ_0 | ADC10SSEL_0 | ADC10DIV_1 | SHS_0 | INCH_0;
    //ADCʱ�ӷ�ƵΪ����Ƶ
    ADC10AE0 = 0x1;//MSP430��P1.0ΪADC�����
}


void lcd1602_display()
{
    LCD_Write_Command(0x09+0x80);//�͵�ѹ��ʾ����ַ
    if(temp>99)//100~999��3λ��
    {

        LCD_Write_Data(temp/100+0x30);//��ʾ���λ
        LCD_Write_Data((temp/10)%10+0x30);//��ʾ��2λ��
        LCD_Write_Data(temp%10+0x30);//��ʾ��3λ��

        LCD_Write_Data(0x2E);//��ʾС����
        LCD_Write_Data(xiaoshu+0x30);//��ʾС��
    }
    else
    {
        LCD_Write_Data(0x20);
        if(temp>9)//10~99
        {
            LCD_Write_Data(temp/10+0x30);//��ʾ���λ
            LCD_Write_Data(temp%10+0x30);//��ʾ��2λС��
            LCD_Write_Data(0x2E);//��ʾС����
            LCD_Write_Data(xiaoshu+0x30);//��ʾС��
        }
        else if(temp>0)
        {
            LCD_Write_Data(0x20);
            LCD_Write_Data(temp+0x30);//��ʾ���λ
            LCD_Write_Data(0x2E);//��ʾС����
            LCD_Write_Data(xiaoshu+0x30);//��ʾС��
        }
        else
        {
            LCD_Write_Data(0x20);
            LCD_Write_Data(0+0x30);//��ʾ���λ
            LCD_Write_Data(0x2E);//��ʾС����
            LCD_Write_Data(xiaoshu+0x30);//��ʾС��
        }
    }
}

void lcd1602_init()
{
    P2DIR = 0xff;             //�˿ڳ�ʼ��
    P1DIR |= BIT1 + BIT2 + BIT3;
    RST_E;
    LCD_Write_Command(0x38);//��������ָ��6��8λ���ݣ�˫����ʾ��5*7����
    __delay_cycles(1000);
    LCD_Write_Command(0x0c);//��ʾ���ؿ���ָ��4��������ʾ�����ع�꣬��겻��˸
    __delay_cycles(1000);
    LCD_Write_Command(0x06);//������ģʽָ��3�����ݶ�д�������ƣ����治�ƶ�
    __delay_cycles(1000);
    LCD_Write_Command(0x01);//����ָ��1
}

void LCD_Write_Command(unsigned char com)
{
    RST_RS;
    RST_RW;
    P2OUT = com;
    SET_E;
    __delay_cycles(800);
    RST_E;
}

void LCD_Write_Data(unsigned char dat)
{
    SET_RS;
    RST_RW;
    P2OUT = dat;
    SET_E;
    __delay_cycles(800);
    RST_E;
}

#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    LPM0_EXIT;
}


/*#include "msp430g2553.h"
#include "LCD_128.h"
#include "HT1621.h"
#include "TCA6416A.h"

int temp=0,xiaoshu=0;
void ADC10_init(void);
void LCD_Init();
void LCD_Display();
int main()
{

    float Vin=0;
    WDTCTL = WDTPW + WDTHOLD; //�ؿ��Ź�
    ADC10_init();
    LCD_Init();
    while(1)
    {
      ADC10CTL0 |= ENC + ADC10SC; //����ѭ���п��� ADC10 ת��
      _bis_SR_register(CPUOFF + GIE); //���߿����жϣ��ȴ� ADC10 ת����ɺ󣬽����ж����л��� CPU ����
      //-----ADCת������жϻ���CPU���ִ�����´���-----
      Vin=(ADC10MEM*2.5*40)/1023;
      temp=(unsigned int)Vin; //ת��Ϊ����ֵ����100��������ȷ��С�������λ
      Vin=Vin-temp;
      xiaoshu=10*Vin;
      LCD_Display();
    }
}


void LCD_Init()
{
    TCA6416A_Init();
    HT1621_init();
    //���Ӳ���ĳ�ʼ�������� I2C ģ��ĳ�ʼ���� TCA6416A ��ʼ���������ڲ�����ˣ� LCD_128 �⺯���� HT1621 ��ʼ���������ڲ�������
    //-----��ʾ�̶������LCD��-----
    LCD_DisplaySeg(_LCD_AUTO);
    LCD_DisplaySeg(_LCD_RUN);
    LCD_DisplaySeg(_LCD_TI_logo);
    LCD_DisplaySeg(_LCD_QDU_logo);
    LCD_DisplaySeg(_LCD_OHOM);
    LCD_DisplaySeg(_LCD_DOT4);
}


void LCD_Display()
{
          //-----���6λ��ʾ����-----
          LCD_DisplayDigit(LCD_DIGIT_CLEAR,1);
          LCD_DisplayDigit(LCD_DIGIT_CLEAR,2);
          LCD_DisplayDigit(LCD_DIGIT_CLEAR,3);
          LCD_DisplayDigit(LCD_DIGIT_CLEAR,4);
          LCD_DisplayDigit(LCD_DIGIT_CLEAR,5);
          LCD_DisplayDigit(LCD_DIGIT_CLEAR,6);
          //-----����temp��ֲ���ʾ����-----

          if(temp>99)//100~999��3λ��
          {
              LCD_DisplayDigit(temp/100,3);
              LCD_DisplayDigit((temp/10)%10,4);
              LCD_DisplayDigit(temp%10,5);
              LCD_DisplayDigit(xiaoshu,6);
          }
          else if(temp>9)
          {
              LCD_DisplayDigit(temp/10,4);
              LCD_DisplayDigit(temp%10,5);
              LCD_DisplayDigit(xiaoshu,6);
          }
          else
          {
              LCD_DisplayDigit(temp,5);
              LCD_DisplayDigit(xiaoshu,6);
          }
          HT1621_Reflash(LCD_Buffer);//-----���»��棬������ʾ-----
}



void ADC10_init(void)
{
    ADC10CTL0 &= ~ENC;
    ADC10CTL0 = ADC10IE|ADC10ON | REFON| REF2_5V | ADC10SHT_0 | SREF_1;
    //�����ڲ�2.5V�ο���ѹ����������ʱ��Ϊ64 x ADC10CLKs
    ADC10CTL1 = CONSEQ_0 | ADC10SSEL_0 | ADC10DIV_7 | SHS_0 | INCH_0;
    //ADCʱ�ӷ�ƵΪ7��Ƶ
    ADC10AE0 = 0x1;//P1.0ΪADC�����
    __delay_cycles(30000);
    //ADC10CTL0 |= ENC;
}



#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    LPM0_EXIT;
}*/
