//Programa

#include <msp430g2553.h>
#include <intrinsics.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

// Declara funções de Display LCD do arquivo: LCD_Port1V3
extern void IniDisp(void);
extern void EscInst(unsigned char inst);
extern void EscDado(unsigned char dado);
extern void GotoXY(unsigned char Linha,unsigned char Coluna);
extern void MString(unsigned char *str);

//Variaveis globais do sistema   
unsigned char tecla;
unsigned char lcdLin = 0,lcdCol = 0;     //linha e coluna do LCD
int ctTimer = 2; //Contador do timer para temporizar
int vetorPosicao[30]; //Vetor responsável por armazenar a sequencia dos Leds
int estado = 3; // 0 = Mostrando a sequência de leds
                // 1 = Recebendo a sequência de leds do usuário
                // 2 = erro
                // 3 = estado após ligar
int ctPosicaoVetor = 0;
int ctAcerto = 2; // Contador de acerto (começa em dois porque o jogo começa acendendo dois leds)
bool ligaLed = false;
unsigned char pontuacao[4];
bool multAcerto = true;
int ctUn = 0;
int delay = 0;

void main(void)
{
  srand(time(0));
  
  WDTCTL = WDTPW+WDTHOLD; //DESABILITA WATCHDOG
  
  //INICIALIZAÇÃO TIMER 0
  TA0CTL = TASSEL_2 + MC_1 + ID_3 + TAIE; // SMCLK, MODO UP, DIV/8, INT. TIPO TAIFG
  TA0CCR0 = 62500; // EQUIVALE A 0,05s ou 500ms
  
  //Inicialização display
  // Programa os PORT´s
  P1DIR = 0xFF;         //PROGRAMA P1 TODOS COMO SAIDA
  P1OUT = 0x00;         //TODAS SAÍDAS DE P1 EM ZERO
  //Inicializa variáveis
  tecla = 0xFF;         //limpa tecla
  lcdLin = 0;
  lcdCol = 0;
  
  IniDisp();                //Inicializa Display LCD 2x16
  
  EscInst(0x0C);            //Desliga Cursor
  GotoXY(0,0);              //Posiciona Cursor
  MString("   SIMON SAYS   "); //Escreve "Dimmer" no display
  GotoXY(1,0);              //Posiciona Cursor na segunda linha do display
  MString("   PRESS START  "); //Apaga segunda linha do display 

  __delay_cycles(1000);

//INICIALIZAÇÃO UART
  DCOCTL = 0; //CALIBRAR DCO PARA 1MHz
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;
  P1SEL = BIT1+BIT2;    //SELECIONA A USCI NO P1.1 E P1.2
  P1SEL2 = BIT1 + BIT2; //Inicializa P1.1 E P1.2 como entrada Rx e Tx
  UCA0CTL1 |= UCSSEL_2; //SELECIONA SMCLK
  UCA0BR0 = 104;        //UART EM 9600BPS
  UCA0BR1 = 0;
  UCA0MCTL = UCBRS_1;
  UCA0CTL1 &= ~UCSWRST; //INICIA SERIAL
  IE2 |= UCA0RXIE;      //HABILITA INT. DE RX
  ///posStrResult = 0; //Inicializa variavel que controla posição no buffer de 
  //controle do dimmer
  
   __bis_SR_register(GIE); //HABILITA AS INTERRUPÇÕES GLOBALMENTE.

  P2DIR = BIT0 + BIT1 + BIT2 + BIT3;
  P2OUT = 0x00;
  
  vetorPosicao[0] = rand()%4;
  vetorPosicao[1] = rand()%4;
  
  while(1){
  }
}


// TRATAMENTO DE INTERRUPÇÃO DO TIMER0A
#pragma vector = TIMER0_A1_VECTOR
__interrupt void isr_TIMERA0(void) {
  switch (TA0IV) {
  case 0x0A: //INT. TAIFG
    
    switch (estado) {
      case 0:
        
        delay++;
        
        multAcerto = true;
        
        if (delay >= 4) {
          GotoXY(0,0);              //Posiciona Cursor
          MString("    EXIBINDO    "); //Escreve no display
          GotoXY(1,0);              //Posiciona Cursor
          MString("    SEQUENCIA..."); //Escreve no display
          
          if (ctTimer%2 == 0) {
            if (vetorPosicao[ctPosicaoVetor] == 0) {
              P2OUT |= BIT0;
            }
            if (vetorPosicao[ctPosicaoVetor] == 1) {
              P2OUT |= BIT1;
            }
             if (vetorPosicao[ctPosicaoVetor] == 2) {
              P2OUT |= BIT2;
            }
             if (vetorPosicao[ctPosicaoVetor] == 3) {
              P2OUT |= BIT3;
            }
          }
          if ((ctTimer%4 == 0) & (ctTimer >=4)) {
            P2OUT = 0x00;
            ctPosicaoVetor ++;
          }
          
          if ((ctTimer/4) == ctAcerto) {
            estado = 1;
            ctTimer = 1;
            ctPosicaoVetor = 0;
          }
          ctTimer++;
        
        }
        break;
    case 1:
      
        delay = 0;
      
        GotoXY(0,0);              //Posiciona Cursor
        MString("     DIGITE     "); //Escreve no display
        GotoXY(1,0);              //Posiciona Cursor
        MString("   A SEQUENCIA:  "); //Escreve no display
      
        if ((ctPosicaoVetor) == ctAcerto) {
          vetorPosicao[ctPosicaoVetor] = rand()%4;
          ctAcerto++;
          estado = 0;
          ctPosicaoVetor = 0;
          P2OUT = 0x00;
        }
        if (ligaLed == true) {
          if (vetorPosicao[ctPosicaoVetor] == 0) {
            P2OUT |= BIT0;
            ligaLed = false;
            ctUn++;
        }
        if (vetorPosicao[ctPosicaoVetor] == 1) {
            P2OUT |= BIT1;
            ligaLed = false;
            ctUn++;
        }
        if (vetorPosicao[ctPosicaoVetor] == 2) {
            P2OUT |= BIT2;
            ligaLed = false;
            ctUn++;
        }
        if (vetorPosicao[ctPosicaoVetor] == 3) {
            P2OUT |= BIT3;
            ligaLed = false;
            ctUn++;
        }
        ctPosicaoVetor++;
      }
        else {
          P2OUT = 0x00;
        }
      break;
      
    case 2:
      
      P2OUT ^= 0xff;
      
        if (multAcerto == true) {  
          ctAcerto = ((ctAcerto-2)*10)+ctUn;
          multAcerto = false;
        }
      
        int mil = ctAcerto/1000;
        ctAcerto = ctAcerto%1000;
        int cent = ctAcerto/100;
        ctAcerto = ctAcerto%100;
        int dec = ctAcerto/10;
        int un = ctAcerto%10;
        mil = mil + '0';
        cent = cent + '0';
        dec = dec + '0';
        un = un + '0';
        pontuacao[0] = mil;
        pontuacao[1] = cent;
        pontuacao[2] = dec;
        pontuacao[3] = un;
      
      GotoXY(1,0);              //Posiciona Cursor
      MString("  SCORE:  "); //Escreve no display
      GotoXY(1,10);              //Posiciona Cursor
      MString(pontuacao); //Escreve no display
      GotoXY(1,14);
      MString("  "); //Escreve no display
      GotoXY(0,0);              //Posiciona Cursor
      MString("   GAME OVER!   "); //Escreve no display
      
      break;
    }
    break;
  }
}
#pragma vector=USCIAB0RX_VECTOR
__interrupt void ISR_RX(void) {
  
    if(UCA0RXBUF != 0x0A) { //Se não chegou no final do recebimento pela serial
       
      if(estado==3) {
        if(UCA0RXBUF == 'S') {
          estado = 0;
        }
      }
      
      if(estado==2) {
        
        if(UCA0RXBUF == 'S') {
          estado = 0;
          ctTimer = 2; //Contador do timer para temporizar
          ctPosicaoVetor = 0;
          ctAcerto = 2; // Contador de acerto (começa em dois porque o jogo começa acendendo dois leds)
          ctUn = 0;
          delay = 0;
          ligaLed = false;
          vetorPosicao[0] = rand()%4;
          vetorPosicao[1] = rand()%4;
          P2OUT = 0x00;
        }
      }
      
      if (estado == 1) {
        if (UCA0RXBUF == 'G') {
          if (vetorPosicao[ctPosicaoVetor] == 0) {
            ligaLed = true;
          }
          else {
            estado = 2;
          }
        }
        if (UCA0RXBUF == 'R') {
          if (vetorPosicao[ctPosicaoVetor] == 1) {
            ligaLed = true;
          }
          else {
            estado = 2;
          }
        }
        if (UCA0RXBUF == 'W') {
          if (vetorPosicao[ctPosicaoVetor] == 2) {
            ligaLed = true;
          }
          else {
            estado = 2;
          }
        }
        if (UCA0RXBUF == 'B') {
          if (vetorPosicao[ctPosicaoVetor] == 3) {
            ligaLed = true;
          }
          else {
            estado = 2;
          }
        }
      }
      
      
      
  }
}