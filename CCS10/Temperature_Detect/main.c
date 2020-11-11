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
void LCD_Write_Command(unsigned char com); //写命令函数
void LCD_Write_Data(unsigned char dat); //写数据函数
void lcd1602_init();
void lcd1602_display();

int main()
{
    float Vin=0,sum=0;
    char count=0;
    WDTCTL = WDTPW + WDTHOLD; //关看门狗
    lcd1602_init();
    ADC10_init();
    display_normal();
    while(1)
    {
      ADC10CTL0 |= ENC + ADC10SC; //在主循环中开启 ADC10 转换
      _bis_SR_register(CPUOFF + GIE); //休眠开总中断，等待 ADC10 转换完成后，进入中断运行唤醒 CPU 代码

      Vin=(ADC10MEM*2.5*40)/1023;  //电压表达式，先扩大40倍
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

void display_normal()//显示学号
{
    unsigned char i=0;
    LCD_Write_Command(0x80);
    LCD_Write_Command(0x80 + 0x05); //数据指针设置，第一行显示
    __delay_cycles(1000);
    for (i = 0; i < sizeof(a); i++)
    {
        LCD_Write_Data(a[i]);
    }
    LCD_Write_Command(0x80 + 0x47); //数据指针设置，第二行显示
    __delay_cycles(1000);
    for (i = 0; i< sizeof(b); i++)
    {
        LCD_Write_Data(b[i]);
    }
    __delay_cycles(1000000);
    LCD_Write_Command(0x01);//清屏指令1

    LCD_Write_Command(0x80);
    LCD_Write_Command(0x80 + 0x01); //数据指针设置，第一行显示
    __delay_cycles(1000);
    for (i = 0; i < sizeof(c); i++)
    {
        LCD_Write_Data(c[i]);
    }
    __delay_cycles(1000);
    LCD_Write_Command(0x40+0x80);//学号从第2行第7个开始显示,显示地址+指针
    for (i = 0; i < sizeof(d); i++)
    {
        LCD_Write_Data(d[i]);
    }
    __delay_cycles(1000000);
    LCD_Write_Command(0x01);//清屏指令1

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
    LCD_Write_Data(0x43);//显示最高位
}

void ADC10_init(void)
{
    ADC10CTL0 &= ~ENC;
    ADC10CTL0 = ADC10IE|ADC10ON | REFON| REF2_5V | ADC10SHT_0 | SREF_1;
    //启用内部2.5V参考电压，采样保持时间为4 x ADC10CLKs
    ADC10CTL1 = CONSEQ_0 | ADC10SSEL_0 | ADC10DIV_1 | SHS_0 | INCH_0;
    //ADC时钟分频为不分频
    ADC10AE0 = 0x1;//MSP430的P1.0为ADC输入端
}


void lcd1602_display()
{
    LCD_Write_Command(0x09+0x80);//送电压显示初地址
    if(temp>99)//100~999（3位）
    {

        LCD_Write_Data(temp/100+0x30);//显示最高位
        LCD_Write_Data((temp/10)%10+0x30);//显示第2位数
        LCD_Write_Data(temp%10+0x30);//显示第3位数

        LCD_Write_Data(0x2E);//显示小数点
        LCD_Write_Data(xiaoshu+0x30);//显示小数
    }
    else
    {
        LCD_Write_Data(0x20);
        if(temp>9)//10~99
        {
            LCD_Write_Data(temp/10+0x30);//显示最高位
            LCD_Write_Data(temp%10+0x30);//显示第2位小数
            LCD_Write_Data(0x2E);//显示小数点
            LCD_Write_Data(xiaoshu+0x30);//显示小数
        }
        else if(temp>0)
        {
            LCD_Write_Data(0x20);
            LCD_Write_Data(temp+0x30);//显示最高位
            LCD_Write_Data(0x2E);//显示小数点
            LCD_Write_Data(xiaoshu+0x30);//显示小数
        }
        else
        {
            LCD_Write_Data(0x20);
            LCD_Write_Data(0+0x30);//显示最高位
            LCD_Write_Data(0x2E);//显示小数点
            LCD_Write_Data(xiaoshu+0x30);//显示小数
        }
    }
}

void lcd1602_init()
{
    P2DIR = 0xff;             //端口初始化
    P1DIR |= BIT1 + BIT2 + BIT3;
    RST_E;
    LCD_Write_Command(0x38);//功能设置指令6：8位数据，双行显示，5*7字形
    __delay_cycles(1000);
    LCD_Write_Command(0x0c);//显示开关控制指令4：开启显示屏，关光标，光标不闪烁
    __delay_cycles(1000);
    LCD_Write_Command(0x06);//置输入模式指令3：数据读写后光标右移，画面不移动
    __delay_cycles(1000);
    LCD_Write_Command(0x01);//清屏指令1
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
    WDTCTL = WDTPW + WDTHOLD; //关看门狗
    ADC10_init();
    LCD_Init();
    while(1)
    {
      ADC10CTL0 |= ENC + ADC10SC; //在主循环中开启 ADC10 转换
      _bis_SR_register(CPUOFF + GIE); //休眠开总中断，等待 ADC10 转换完成后，进入中断运行唤醒 CPU 代码
      //-----ADC转换完成中断唤醒CPU后才执行以下代码-----
      Vin=(ADC10MEM*2.5*40)/1023;
      temp=(unsigned int)Vin; //转换为电阻值，并100倍处理，精确到小数点后两位
      Vin=Vin-temp;
      xiaoshu=10*Vin;
      LCD_Display();
    }
}


void LCD_Init()
{
    TCA6416A_Init();
    HT1621_init();
    //相关硬件的初始化，其中 I2C 模块的初始化由 TCA6416A 初始化函数在内部完成了， LCD_128 库函数由 HT1621 初始化函数在内部引用了
    //-----显示固定不变的LCD段-----
    LCD_DisplaySeg(_LCD_AUTO);
    LCD_DisplaySeg(_LCD_RUN);
    LCD_DisplaySeg(_LCD_TI_logo);
    LCD_DisplaySeg(_LCD_QDU_logo);
    LCD_DisplaySeg(_LCD_OHOM);
    LCD_DisplaySeg(_LCD_DOT4);
}


void LCD_Display()
{
          //-----清除6位显示数字-----
          LCD_DisplayDigit(LCD_DIGIT_CLEAR,1);
          LCD_DisplayDigit(LCD_DIGIT_CLEAR,2);
          LCD_DisplayDigit(LCD_DIGIT_CLEAR,3);
          LCD_DisplayDigit(LCD_DIGIT_CLEAR,4);
          LCD_DisplayDigit(LCD_DIGIT_CLEAR,5);
          LCD_DisplayDigit(LCD_DIGIT_CLEAR,6);
          //-----根据temp拆分并显示数字-----

          if(temp>99)//100~999（3位）
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
          HT1621_Reflash(LCD_Buffer);//-----更新缓存，真正显示-----
}



void ADC10_init(void)
{
    ADC10CTL0 &= ~ENC;
    ADC10CTL0 = ADC10IE|ADC10ON | REFON| REF2_5V | ADC10SHT_0 | SREF_1;
    //启用内部2.5V参考电压，采样保持时间为64 x ADC10CLKs
    ADC10CTL1 = CONSEQ_0 | ADC10SSEL_0 | ADC10DIV_7 | SHS_0 | INCH_0;
    //ADC时钟分频为7分频
    ADC10AE0 = 0x1;//P1.0为ADC输入端
    __delay_cycles(30000);
    //ADC10CTL0 |= ENC;
}



#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    LPM0_EXIT;
}*/
