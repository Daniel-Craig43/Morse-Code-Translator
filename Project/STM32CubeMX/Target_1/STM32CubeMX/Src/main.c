/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#include "main.h"

// Sets up the system clock
void SystemClock_Config(void);

// Sets of the GPIO pins
static void MX_GPIO_Init(void);

// This lab will require a lot of programming, but most of what you'll need
//	is the function in the lecture slides! Slides 13-18 from Lecture 4 part 3
//	have all the code you need to work with the LCD, and slides 32-45 have
//	all the code you need to work with the keypad

// The main function, and the entry point of the program

void SysTick_Initialize(uint32_t);
uint8_t Read_Keypad();
void Delay(unsigned int);
void Write_SR_LCD(uint8_t);
void LCD_nibble_write(uint8_t, uint8_t);
void Write_Instr_LCD(uint8_t);
void Write_Char_LCD(uint8_t);
void Write_String_LCD(char*);
static void MX_GPIO_Init(void);
void Init_buzzer();
void Init_TIM2_C1_periodic_interrupts(uint32_t, uint32_t, uint32_t);

uint32_t Find_Letter(uint8_t, int); //user made function to save space in main to find letters given number and cycle

volatile uint8_t wait = 1;				//wait flag (1 means go off, 0 means read)

//morse code message
volatile int morse_index = 0;				//sets the index of the morse code array (also serves as the cursor location)
volatile int morse_index_comp = 0;	//placeholder variable to help calculate the next letter in the array to be read
volatile uint32_t morse_code[16];		//array to hold the codes of all of the array
volatile uint8_t read;							//the 2 bit code that is being read from a given letter
volatile uint8_t step;							//where in the letter code you are
volatile uint8_t buzz = 0;					//flag to tell the buzzer to go

volatile uint8_t num_press = 1;

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
	Init_buzzer();
	Init_TIM2_C1_periodic_interrupts(2096, 1, 1);

  // TODO: Put any other initialization code here

	/* LCD controller reset sequence */
	Delay(20);
	LCD_nibble_write(0x30,0);
	Delay(5);
	LCD_nibble_write(0x30,0);
	Delay(1);
	LCD_nibble_write(0x30,0);
	Delay(1);
	LCD_nibble_write(0x20,0);
	Delay(1);

	Write_Instr_LCD(0x28); /* set 4 bit data LCD - two line display - 5x8 font*/
	Write_Instr_LCD(0x0E); /* turn on display, turn on cursor , turn off blinking */
	Write_Instr_LCD(0x01); /* clear display screen and return to home position*/ 
	Write_Instr_LCD(0x06); /* move cursor to right (entry mode set instruction)*/
	
	//create strings for lines 1 and 2
	char* line1;
	char* line2;
	
	//create the intro statement
	line1 = "Morse code proj";
	line2 = "use num to type ";

	Write_String_LCD(line1);	//Write line 1 to the LCD
	Write_Instr_LCD(0xC0);
	Write_String_LCD(line2);	//Write line 2 to the LCD

	//for initial message
	uint8_t input;
	uint8_t init_press = 0;		//flag to see if initial button is pressed
	
	int cycle = -1;				//flag to see what letter you are on

	SysTick_Initialize(1039999);	//Systick goes every 0.5 seconds

  while (1)
  {
    // TODO: Put your main code here. It will run over and over again until the program is stopped
		
		input = Read_Keypad();
		
		//wake LCD on first key press
		if(input>=0 && input<=15 && init_press == 0){	//if a button is pressed for the first time
			Write_Instr_LCD(0x01);	//clear the LCD
			init_press = 1;	//indicate initial press has happened
		}
		
		//if 1 is pressed, lock in the letter
		else if(input==1){
			if(morse_index<=15 && morse_index >= 0){	//if the end of the array has not been reached
				Write_Instr_LCD(0x14);	//move cursor to the right
				if(num_press == 1)
					morse_code[morse_index] = 0x01000000;
				else
					morse_code[morse_index] = Find_Letter(num_press,cycle);
				morse_index++;					//increment the number of the index
			}
			num_press = 1;	//reset num_press
			cycle = -1;			//RESET cycle
		}
		
		//if 2 is pressed cycle a, b, and c
		else if((input==2) && ((num_press==1)||(num_press==2))){
			cycle++; //tell what letter you are on
			num_press = 2; //store what number you are on
			
			if(cycle == 0){
				Write_Char_LCD('a');		//write letter
				Write_Instr_LCD(0x10);	//move cursor to the left (works for all of the below)
			}
			else if(cycle == 1){
				Write_Char_LCD('b');
				Write_Instr_LCD(0x10);
			}
			else if(cycle == 2){
				Write_Char_LCD('c');
				Write_Instr_LCD(0x10);
				cycle = -1; //reset cycle 
			}
		}	
		
		else if((input==3) && ((num_press==1)||(num_press==3))){
			cycle++; //tell what letter you are on
			num_press = 3; //store what number you are on
			
			if(cycle == 0){
				Write_Char_LCD('d');		//write letter
				Write_Instr_LCD(0x10);	//move cursor to the left (works for all of the below)
			}
			else if(cycle == 1){
				Write_Char_LCD('e');
				Write_Instr_LCD(0x10);
			}
			else if(cycle == 2){
				Write_Char_LCD('f');
				Write_Instr_LCD(0x10);
				cycle = -1; //reset cycle 
			}
		}	
		
		else if((input==4) && ((num_press==1)||(num_press==4))){
			cycle++; //tell what letter you are on
			num_press = 4; //store what number you are on
			
			if(cycle == 0){
				Write_Char_LCD('g');		//write letter
				Write_Instr_LCD(0x10);	//move cursor to the left (works for all of the below)
			}
			else if(cycle == 1){
				Write_Char_LCD('h');
				Write_Instr_LCD(0x10);
			}
			else if(cycle == 2){
				Write_Char_LCD('i');
				Write_Instr_LCD(0x10);
				cycle = -1; //reset cycle 
			}
		}	
		
		else if((input==5) && ((num_press==1)||(num_press==5))){
			cycle++; //tell what letter you are on
			num_press = 5; //store what number you are on
			
			if(cycle == 0){
				Write_Char_LCD('j');		//write letter
				Write_Instr_LCD(0x10);	//move cursor to the left (works for all of the below)
			}
			else if(cycle == 1){
				Write_Char_LCD('k');
				Write_Instr_LCD(0x10);
			}
			else if(cycle == 2){
				Write_Char_LCD('l');
				Write_Instr_LCD(0x10);
				cycle = 0; //reset cycle 
			}
		}	
		
		else if((input==6) && ((num_press==1)||(num_press==6))){
			cycle++; //tell what letter you are on
			num_press = 6; //store what number you are on
			
			if(cycle == 0){
				Write_Char_LCD('m');		//write letter
				Write_Instr_LCD(0x10);	//move cursor to the left (works for all of the below)
			}
			else if(cycle == 1){
				Write_Char_LCD('n');
				Write_Instr_LCD(0x10);
			}
			else if(cycle == 2){
				Write_Char_LCD('o');
				Write_Instr_LCD(0x10);
				cycle = -1; //reset cycle 
			}
		}	
		
		else if((input==7) && ((num_press==1)||(num_press==7))){
			cycle++; //tell what letter you are on
			num_press = 7; //store what number you are on
			
			if(cycle == 0){
				Write_Char_LCD('p');		//write letter
				Write_Instr_LCD(0x10);	//move cursor to the left (works for all of the below)
			}
			else if(cycle == 1){
				Write_Char_LCD('q');
				Write_Instr_LCD(0x10);
			}
			else if(cycle == 2){
				Write_Char_LCD('r');
				Write_Instr_LCD(0x10);
			}
			else if(cycle == 3){
				Write_Char_LCD('s');
				Write_Instr_LCD(0x10);
				cycle = -1; //reset cycle 
			}
		}	
		
		else if((input==8) && ((num_press==1)||(num_press==8))){
			cycle++; //tell what letter you are on
			num_press = 8; //store what number you are on
			
			if(cycle == 0){
				Write_Char_LCD('t');		//write letter
				Write_Instr_LCD(0x10);	//move cursor to the left (works for all of the below)
			}
			else if(cycle == 1){
				Write_Char_LCD('u');
				Write_Instr_LCD(0x10);
			}
			else if(cycle == 2){
				Write_Char_LCD('v');
				Write_Instr_LCD(0x10);
				cycle = -1; //reset cycle 
			}
		}	
		
		else if((input==9) && ((num_press==1)||(num_press==9))){
			cycle++; //tell what letter you are on
			num_press = 9; //store what number you are on
			
			if(cycle == 0){
				Write_Char_LCD('w');		//write letter
				Write_Instr_LCD(0x10);	//move cursor to the left (works for all of the below)
			}
			else if(cycle == 1){
				Write_Char_LCD('x');
				Write_Instr_LCD(0x10);
			}
			else if(cycle == 2){
				Write_Char_LCD('y');
				Write_Instr_LCD(0x10);
			}
			else if(cycle == 3){
				Write_Char_LCD('z');
				Write_Instr_LCD(0x10);
				cycle = -1; //reset cycle 
			}
		}	
		
		//if A is pressed input == 10
		else if(input==10 && ((num_press==1)||(num_press==10))){
			num_press = 10;
			if(morse_index > 0){
				wait = 0;
			}
		}
		
		//if B is pressed input == 11
		
		//if D is pressed delete a char and remove the item from the array
		else if(input==13 && ((num_press==1)||(num_press==13))){
			num_press = 1;
			if(morse_index > 0){
				Write_Char_LCD(' ');
				Write_Instr_LCD(0x10);
				Write_Instr_LCD(0x10);
				Write_Char_LCD(' ');
				Write_Instr_LCD(0x10);
				morse_index--;
			}
		}
		
		//if C is pressed input == 12
		
		//if * is pressed input == 14
		
		//if # is pressed input == 15
	}
}

void SysTick_Handler(void)
{
	
	if(wait == 1){
		GPIOB->ODR &= ~GPIO_ODR_OD0;	// Turn off PB0 (External LED)
		buzz = 0;
	}
	else{
		if(morse_index_comp == 0){
			morse_index_comp = morse_index;
		}
		if(morse_index_comp > 0){					//if the end of the array is not reached
			if((morse_code[morse_index-morse_index_comp] & (3<<2*(16-step))) == (3<<2*(16-step))){	//code 11
				GPIOB->ODR |= GPIO_ODR_OD0;	// Turn on PB0 (External LED)
				buzz = 1;
				GPIOA->ODR |= (1<<9);	// Turn off PC0 (EXTI)
				step++;
			}
			else if((morse_code[morse_index-morse_index_comp] & (3<<2*(16-step))) == (1<<2*(16-step))){	//code 01
				GPIOB->ODR &= ~GPIO_ODR_OD0;	// Turn off PB0 (External LED)
				buzz = 0;
				step = 1;
				morse_index_comp--;
				if(morse_index_comp == 0){
					wait = 1;
					morse_index = 0;
					Write_Instr_LCD(0x01);
					num_press = 1;
				}
			}
			else{	//code 00
				GPIOB->ODR &= ~GPIO_ODR_OD0;	// Turn off PB0 (External LED)
				buzz = 0;
				step++;
			}
		}
	}
}

void TIM2_IRQHandler(void)
{
if((TIM2->SR&(1<<1))!=0)
{
TIM2->SR&=~(1<<1);
	if(buzz == 1)
		GPIOC->ODR^=(1<<9);
}
}

// Set up the GPIO pins
static void MX_GPIO_Init(void)
{
  // TODO: Initialize any input or output pins you'll use here
	
	uint32_t temp;	//temporary value used for initialization
	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;	//Enable GPIOA clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;	//Enable GPIOB clock

	//Set PA5 and PA10 to output  for LCD
	temp = GPIOA->MODER;
	temp &= ~(0x03<<(2*5));
	temp|=(0x01<<(2*5)); 
	temp &= ~(0x03<<(2*10));
	temp|=(0x01<<(2*10)); 
	GPIOA->MODER = temp;

	temp=GPIOA->OTYPER;
	temp &=~(0x01<<5);
	temp &=~(0x01<<10);
	GPIOA->OTYPER=temp;

	temp=GPIOA->PUPDR;
	temp&=~(0x03<<(2*5));
	temp&=~(0x03<<(2*10));
	GPIOA->PUPDR=temp;

	//Set PB5 to output for LCD
	temp = GPIOB->MODER;
	temp &= ~(0x03<<(2*5));
	temp|=(0x01<<(2*5));
	GPIOB->MODER = temp;

	temp=GPIOB->OTYPER;
	temp &=~(0x01<<5);
	GPIOB->OTYPER=temp;
	
	temp=GPIOB->PUPDR;
	temp&=~(0x03<<(2*5));
	GPIOB->PUPDR=temp;
	
	
	//Pin initialization for the keypad
	/* row0 to 3 are PB11, PB10, PB9, PB8 */
	temp = GPIOB->MODER;
	temp &= ~(0x03<<(2*11));
	temp &= ~(0x03<<(2*10));
	temp &= ~(0x03<<(2*9));
	temp &= ~(0x03<<(2*8));
	GPIOB->MODER = temp;

	temp=GPIOB->OTYPER;
	temp &=~(0x01<<11);
	temp &=~(0x01<<10);
	temp &=~(0x01<<9);
	temp &=~(0x01<<8);
	GPIOB->OTYPER=temp;

	temp=GPIOB->PUPDR;
	temp&=~(0x03<<(2*11));
	temp&=~(0x03<<(2*10));
	temp&=~(0x03<<(2*9));
	temp&=~(0x03<<(2*8));
	GPIOB->PUPDR=temp;

	/* Col 0 to 3 are PB1, PB2, PB3, PB4*/
	/*configure output*/ 
	temp = GPIOB->MODER;
	temp &= ~(0x03<<(2*1));
	temp|=(0x01<<(2*1)); 
	temp &= ~(0x03<<(2*2));
	temp|=(0x01<<(2*2)); 
	temp &= ~(0x03<<(2*3));
	temp|=(0x01<<(2*3));
	temp &= ~(0x03<<(2*4));
	temp|=(0x01<<(2*4));
	GPIOB->MODER = temp;

	temp=GPIOB->OTYPER;
	temp &=~(0x01<<1); 
	temp &=~(0x01<<2); 
	temp &=~(0x01<<3); 
	temp &=~(0x01<<4); 
	GPIOB->OTYPER=temp;

	temp=GPIOB->PUPDR;
	temp&=~(0x03<<(2*1));
	temp&=~(0x03<<(2*2));
	temp&=~(0x03<<(2*3)); 
	temp&=~(0x03<<(2*4));
	GPIOB->PUPDR=temp;
	
	// Set PB0 to output mode (LED on the breadboard)
  GPIOB->MODER &= ~GPIO_MODER_MODE0;  // Clear mode bits for PA1
  GPIOB->MODER |= GPIO_MODER_MODE0_0; // Set PA1 to output mode

  GPIOB->OTYPER &= ~GPIO_OTYPER_OT0; // Clear output type bits for PA1
	
  GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD0; // Clear pull-up/pull-down bits for PA1
	
	// Set PA9 to output mode (Buzzer EXTI)
  temp = GPIOA->MODER;
	temp &= ~(0x03<<(2*9));
	temp|=(0x01<<(2*9));
	GPIOA->MODER = temp;

	temp=GPIOA->OTYPER;
	temp &=~(0x01<<9);
	GPIOA->OTYPER=temp;
	
	temp=GPIOA->PUPDR;
	temp&=~(0x03<<(2*9));
	GPIOA->PUPDR=temp;
}

void Init_buzzer(){
	uint32_t temp;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN; /* enable GPIOC clock */
	
	temp = GPIOC->MODER;
	temp &= ~(0x03<<(2*9));
	temp|=(0x01<<(2*9));
	GPIOC->MODER = temp;
	temp=GPIOC->OTYPER;
	temp &=~(0x01<<9);
	GPIOC->OTYPER=temp;
	temp=GPIOC->PUPDR;
	temp&=~(0x03<<(2*9));
	GPIOC->PUPDR=temp;
}


//returns a created code for a given letter 11 is on 00 is off 01 is delimiter
uint32_t Find_Letter(uint8_t num, int cycle){
	if(cycle == 0){
		if(num == 2)
			return 0xcfc10000;	// a *-
		else if(num == 3)
			return 0xfccc1000;	// d -**
		else if(num == 4)
			return 0xfcfcc100; // g --*
		else if(num == 5)
			return 0xcfcfcfc1; // j *---	
		else if(num == 6)
			return 0xfcfc1000;	//m --
		else if(num == 7)
			return 0xcfcfcc10;	//p *--*
		else if(num == 8)
			return 0xfc100000;	//t -
		else if(num == 9)
			return 0xcfcfc100;	//w *--
	}
	if(cycle == 1){
		if(num == 2)
			return 0xfcccc100; // b -***
		else if(num == 3)
			return 0xc1000000; // e *
		else if(num == 4)
			return 0xcccc1000; // h ****
		else if(num == 5)
			return 0xfccfc100; // k -*-
		else if(num == 6)
			return 0xfcc10000;	//n -*
		else if(num == 7)
			return 0xfcfccfc1;	//q --*-
		else if(num == 8)
			return 0xccfc1000;	//u **-
		else if(num == 9)
			return 0xfcccfc10;	//x -**-
	}
	
	if(cycle == -1){
		if(num == 2)
			return 0xfccfcc10; // c -*-*
		else if(num == 3)
			return 0xccfcc100; // f **-*
		else if(num == 4)
			return 0xcc100000; // i **
		else if(num == 5)
			return 0xcfccc100; // l *-**
		else if(num == 6)
			return 0xfcfcfc10;	//o ---
		else if(num == 7)
			return 0xccc10000;	//s ***
		else if(num == 8)
			return 0xcccfc100;	//v ***-
		else if(num == 9)
			return 0xfcfccc10;	//z --**
	}
	
	if(cycle == 2){
		if(num == 7)
			return 0xcfcc1000;	//r *-*
		else if(num == 9)
			return 0xfccfcfc1;	//y -*--
	}
	
	return 0;
}

void Init_TIM2_C1_periodic_interrupts(uint32_t P, uint32_t C, uint32_t A)
{
// Enable the clock of TIM2
RCC->APB1ENR1|= 0x1; // enable TIM2
TIM2->PSC=P;
TIM2->CCR1=C;
TIM2->ARR=A;
//Enable Compare output for channel 1
TIM2->CCER|=(0x1<<0);
// enable counting TIM2
TIM2->CR1|=0x1;
TIM2->CR1&=~(0x1<<4); //counting up
TIM2->DIER|=(1<<1);// enable interrupt of channel 1
NVIC_EnableIRQ(28);
NVIC_SetPriority(TIM2_IRQn, 3);		//lower priority of SysTick to make it more important than exti handler
}

void SysTick_Initialize(uint32_t ticks)
{
  SysTick->LOAD = ticks;
  SysTick->VAL = 0;

  // Select processor clock to internal: 1 = processor clock; 0 = external clock
  SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

  // Enable counting of SysTick
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

  // Enables SysTick interrupt
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	
	NVIC_SetPriority(SysTick_IRQn, 2);		//lower priority of SysTick to make it more important than exti handler
}

//
// BEYOND	THIS POINT IS INITIALIZATION CODE PROVIDED TO US
//

void Delay(unsigned int n){
	int i;
	if(n!=0){
		for(; n>0; n--)
		for(i=0; i<136; i++);
	}
}

uint8_t Read_Keypad(){ 
	uint8_t a;
	/*set all columns high and wait until a putton is pressed*/
	GPIOB->ODR|=(1<<1);
	GPIOB->ODR|=(1<<2);
	GPIOB->ODR|=(1<<3); 
	GPIOB->ODR|=(1<<4);

	while((GPIOB->IDR &(0x1<<8))==0 && (GPIOB->IDR &(0x1<<9))==0 && (GPIOB->IDR &(0x1<<10))==0 &&	(GPIOB->IDR &(0x1<<11))==0){}

	Delay(25);	/*debouncing*/
	
	/*scanning */
	while(1){
		GPIOB->ODR&=~(1<<1);
		GPIOB->ODR&=~(1<<2);
		GPIOB->ODR&=~(1<<3);
		GPIOB->ODR&=~(1<<4);

		//Scan Col 0 - PB1 == HIGH
		GPIOB->ODR|=(1<<1);
		Delay(2);
		
		//Check rows
		if((GPIOB->IDR &(0x1<<11))!=0){
			a=14;
			break;
		}
		if((GPIOB->IDR &(0x1<<10))!=0){
			a=7;
			break;
		}
		if((GPIOB->IDR &(0x1<<9))!=0){
			a=4;
			break;
		}
		if((GPIOB->IDR &(0x1<<8))!=0){
			a=1;
			break;
		}
		
		//Scan Col 1
		GPIOB->ODR&=(1<<1);
		Delay(2);
		GPIOB->ODR|=(1<<2);
		Delay(2);
		
		if((GPIOB->IDR &(0x1<<11))!=0){
			a=0;
			break;
		}
		if((GPIOB->IDR &(0x1<<10))!=0){
			a=8;
			break;
		}
		if((GPIOB->IDR &(0x1<<9))!=0){
			a=5;
			break;
		}
		if((GPIOB->IDR &(0x1<<8))!=0){
			a=2;
			break;
		}
		
		//Scan Col 2
		GPIOB->ODR&=(1<<2);
		Delay(2);
		GPIOB->ODR|=(1<<3);
		Delay(2);
		
		if((GPIOB->IDR &(0x1<<11))!=0){
			a=15;
			break;
		}
		if((GPIOB->IDR &(0x1<<10))!=0){
			a=9;
			break;
		}
		if((GPIOB->IDR &(0x1<<9))!=0){
			a=6;
			break;
		}
		if((GPIOB->IDR &(0x1<<8))!=0){
			a=3;
			break;
		}
		
		//Scan Col 3
		GPIOB->ODR&=(1<<3);
		Delay(2);
		GPIOB->ODR|=(1<<4);
		Delay(2);
		
		if((GPIOB->IDR &(0x1<<11))!=0){
			a=13;
			break;
		}
		if((GPIOB->IDR &(0x1<<10))!=0){
			a=12;
			break;
		}
		if((GPIOB->IDR &(0x1<<9))!=0){
			a=11;
			break;
		}
		if((GPIOB->IDR &(0x1<<8))!=0){
			a=10;
			break;
		}
	}
	
	//Wait until button is released
	GPIOB->ODR|=(1<<1);
	Delay(2);
	GPIOB->ODR|=(1<<2);
	Delay(2);
	GPIOB->ODR|=(1<<3);
	Delay(2);
	GPIOB->ODR|=(1<<4);
	Delay(2);
	
	while((!(	((GPIOB->IDR &(0x1<<8))==0) && ((GPIOB->IDR &(0x1<<9))==0) && 	((GPIOB->IDR &(0x1<<10))==0)	&&	((GPIOB->IDR &(0x1<<11))==0))))
	{}
	Delay(25);	//Debouncing
	return(a);
}

void Write_SR_LCD(uint8_t temp) { 
	int i;
	uint8_t mask=0b10000000;
	for(i=0; i<8; i++) {
		if((temp&mask)==0)
			GPIOB->ODR&=~(1<<5);
		else
			GPIOB->ODR|=(1<<5);

/*	Sclck */
		GPIOA->ODR&=~(1<<5);
		GPIOA->ODR|=(1<<5);
		Delay(1);

		mask=mask>>1;
	}

/*Latch*/
	GPIOA->ODR|=(1<<10);
	GPIOA->ODR&=~(1<<10);
}

void LCD_nibble_write(uint8_t temp, uint8_t s){

/*writing instruction*/ 
	if (s==0){ 
		temp=temp&0xF0;
		temp=temp|0x02; /*RS (bit 0) = 0 for Command EN (bit1)=high */ 
		Write_SR_LCD(temp);

    temp=temp&0xFD; /*RS (bit 0) = 0 for Command EN (bit1) = low*/ 
    Write_SR_LCD(temp);
	}

/*writing data*/ 
	else if (s==1) {
		temp=temp&0xF0;
		temp=temp|0x03;	/*RS(bit 0)=1 for data EN (bit1) = high*/ 
		Write_SR_LCD(temp);

		temp=temp&0xFD; /*RS(bit 0)=1 for data EN(bit1) = low*/ 
		Write_SR_LCD(temp); 
	}
}

void Write_Instr_LCD(uint8_t code){
	LCD_nibble_write(code&0xF0,0);

	code=code<<4; LCD_nibble_write(code,0);
}

void Write_Char_LCD(uint8_t code){
	LCD_nibble_write(code&0xF0,1);

	code=code<<4;
	LCD_nibble_write(code,1);
}

void Write_String_LCD(char *temp) {
	int i=0;
	while(temp[i]!=0){
		Write_Char_LCD(temp[i]); i=i+1;
	}
}

// Set up the system clock
// ! IMPORTANT ! This function is generated by STM32CubeMX and should not be modified
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  // Configure the main internal regulator output voltage
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  // Initializes the RCC Oscillators according to the specified parameters
  //  in the RCC_OscInitTypeDef structure.
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  // Initializes the CPU, AHB and APB buses clocks
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}



// This function is called whenever there is an error, such as illegal memory access
void Error_Handler(void)
{
  // We don't really need to do anything for errors, so just stay here forever
  __disable_irq();
  while (1)
  {
  }
}



// ! IMPORTANT ! All code below this line is generated by STM32CubeMX and should not be modified
#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
