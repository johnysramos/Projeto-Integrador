#include <msp430G2553.h>
#include <intrinsics.h>

#define RS BIT3     //REGISTER SELECT DO LCD
#define LCD BIT0    //ENABLE DO LCD

void IniDisp(void);
void strobe();
void EscInst(unsigned char inst);
void EscDado(unsigned char dado);
void GotoXY(unsigned char Linha,unsigned char Coluna);
void MString(unsigned char *str);
unsigned char CHexAsc(unsigned char hex);

void IniDisp(void)
{

  __delay_cycles(20000);        //Espera 20 ms
  P1OUT &= BIT2+BIT1;   //Zera tudo menos BIT2 e BIT1
  P1OUT |= 0x30;        //RS=0,DB7,DB6,DB5,DB4 = 0011
  strobe();
  __delay_cycles(10000); //Espera 10 ms
  P1OUT &= BIT2+BIT1;   //Zera tudo menos BIT2 e BIT1
  P1OUT |= 0x30;        //RS=0,DB7,DB6,DB5,DB4 = 0011
  strobe();
  __delay_cycles(1000); //Espera 1mS
  P1OUT &= BIT2+BIT1;   //Zera tudo menos BIT2 e BIT1
  P1OUT |= 0x30;        //RS=0,DB7,DB6,DB5,DB4 = 0011
  strobe();
  __delay_cycles(1000); //Espera 1mS
  P1OUT &= BIT2+BIT1;   //Zera tudo menos BIT2 e BIT1
  P1OUT |= 0x20;        //RS=0,DB7,DB6,DB5,DB4 = 0010 => 4 bit mode
  strobe();
  __delay_cycles(1000); //Espera 100uS 
  EscInst(0x06);        //INSTRUCAO DE MODO DE OPERACAO
  EscInst(0x28);        //4-bits, 2 linhas
  EscInst(0x0C);        //INSTRUCAO DE Cursor Off
  EscInst(0x01);        //INSTRUCAO DE LIMPEZA DO DISPLAY
  __delay_cycles(100000);
}

void strobe()
{
  P1OUT |= LCD;         //HAB. LCD=1
  __delay_cycles(1);
  P1OUT &= ~LCD;        //FAZ LCD=0
}

void EscInst(unsigned char inst)
{
  P1OUT &= BIT2+BIT1;   //RS=0 Zera tudo menos BIT2 e BIT1
  P1OUT |= (inst & 0xF0); //Separa parte alta
  strobe();
  P1OUT &= BIT2+BIT1;   //RS=0 Zera tudo menos BIT2 e BIT1
  P1OUT |= ((inst<<4) & 0xF0); //Separa parte baixa e hab. LCD
  strobe();
  __delay_cycles(500);  //espera 500us
}


void EscDado(unsigned char dado)
{
  P1OUT &= BIT2+BIT1;   //RS=0 Zera tudo menos BIT2 e BIT1
  P1OUT |= (dado & 0xF0) | RS; //Separa parte alta e RS=1 E LCD=1
  strobe();
  P1OUT &= BIT2+BIT1;   //RS=0 Zera tudo menos BIT2 e BIT1
  P1OUT |= (dado<<4 & 0xF0) | RS; //Separa parte baixa e RS=1 E LCD=1
  strobe();
  __delay_cycles(500);  //espera 500us  
}


void GotoXY(unsigned char Linha,unsigned char Coluna)
{
  if (Linha == 0)
    Coluna |= 0x80;     //Calcula posição na linha 0
  else
    Coluna |= 0xC0;     //Calcula posição na linha 1
  EscInst(Coluna);      //INSTRUCAO
}


void MString(unsigned char *str)
{
  while (*str)
  {
    EscDado(*str);
    str++;
  }
}


unsigned char CHexAsc(unsigned char hex)
{
  if (hex < 10)           //Se número
    return hex+0x30;      //converte número em ASCII
  else
    return hex+0x37;      //converte letra em ASCII
}
