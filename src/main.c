/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 16:31:16 by dbaladro          #+#    #+#             */
/*   Updated: 2025/03/09 12:58:14 by fguarrac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/atm328p.h"

/** *All the information that I needed for this can be found at:
 *  - https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061A.pdf
*/

enum e_timer
{
	E_4LEDS,
	E_3LEDS,
	E_2LEDS,
	E_1LED,
	E_GAME_START
};

enum e_timer	g_animationState = E_4LEDS;

/* ************************************************************************** */
/*                               COUNTDOWN                                    */
/* ************************************************************************** */

	//	Animate led (state machine)	//	Timer interrupt
	
	//	turn on leds D1, D2, D3, D4
	//	delay()
	//	turn off led D4
	//	delay()
	//	turn off led D3
	//	delay()
	//	turn off led D2
	//	delay()
	//	turn off led D1

	//	If button pressed while state != GAME_START
		//	Lost

	//	start game

ISR(TIMER1_COMPA_vect)
{
	switch (g_animationState)
	{
		case E_4LEDS:
		{
			PORTB ^= (1u << PORTB0);	//	Put led D1 pin high
			PORTB ^= (1u << PORTB1);	//	Put led D2 pin high
			PORTB ^= (1u << PORTB2);	//	Put led D3 pin high
			PORTB ^= (1u << PORTB4);	//	Put led D4 pin high
			g_animationState = E_3LEDS;
			break ;
		}
		case E_3LEDS:
		{
			PORTB ^= (1u << PORTB4);	//	Put led D4 pin low
			g_animationState = E_2LEDS;
			break ;
		}
		case E_2LEDS:
		{
			PORTB ^= (1u << PORTB2);	//	Put led D3 pin low
			g_animationState = E_1LED;
			break ;
		}
		case E_1LED:
		{
			PORTB ^= (1u << PORTB1);	//	Put led D2 pin low
			g_animationState = E_GAME_START;
			break ;
		}
		case E_GAME_START:
		{
			PORTB ^= (1u << PORTB0);	//	Put led D1 pin high
			TIMSK1 &= ~(1u << OCIE1A);	//	Disable interrupt on timer compare match (Lock state machine)
			break ;
		}
	}
}
	
static void	countdown(void)
{
	unsigned char	cardButton;

	//	Init leds as output
	DDRB |= (1u << DDB0);	//	Set led D1 as output (PB0)
	DDRB |= (1u << DDB1);	//	Set led D2 as output (PB1)
	DDRB |= (1u << DDB2);	//	Set led D3 as output (PB2)
	DDRB |= (1u << DDB4);	//	Set led D4 as output (PB4)

	//	Set led pins low
	PORTB &= ~(1u << PORTB0);	//	Put led D1 pin low
	PORTB &= ~(1u << PORTB1);	//	Put led D2 pin low
	PORTB &= ~(1u << PORTB2);	//	Put led D3 pin low
	PORTB &= ~(1u << PORTB4);	//	Put led D4 pin low

	//	Init Timer/Counter

		//	CTC mode
		//	f = F_CPU / (2 * prescaler * (1 + OCR1A))
		//	1000 * (1 + OCR1A) = F_CPU / (2 * prescaler))
		//	1 + OCR1A = F_CPU / (2 * prescaler)
		//	OCR1A = (F_CPU / (2 * prescaler) - 1
		//	OCR1A = (16000000 / (2 * 256)) - 1 = 31249
		//	OCR1A = 31249

	TCCR1B |= (1u << WGM12);	//	Set CTC mode
	TCCR1B |= (1u << CS12);		//	Set prescaler to 256
	OCR1A = 31249;				//	Set TOP value
	TIMSK1 |= (1u << OCIE1A);	//	Enable compare match interrupt on A

	//	Enable global interrupts
	SREG |= (1u << 7);

	while (TIMSK1 & (1u << OCIE1A))
	{
		//	check for button press on board
		cardButton = (PIND & (1u << PIND2));
		if (!cardButton)
		{
			//	master/slave lost game
			//	if master -> switch from MR to MT + send message to inform slave
			//	If slave -> send LOST message to master
		}
		if (i2cButton)	///	Use TWI interrupt
		{
			//	check for button press on i2c
			//	if master -> 
			//	if slave -> receive message
		}
	}
	//	Start game
}

/* I2C Arbitration */
//void  i2c_arbitration(void)
//{
//  //  Read SDA state
//      //  If Ok to send, change state of SDA, sleep   -> MCU is master
//      //  Else, switch to slave mode.                 -> MCU is slave
//  if (PINC & (1u << PINC4))
//  {
//      PORTC ^= (1u << PORTC4);
//      _delay_ms(1000);
//      uart_printstr("I'm master\r\n");
//  }
//  else
//  {
//      i2c_slave_init(SLAVEADDR);
//      uart_printstr("I'm slave\r\n");
//  }
//}



/* ************************************************************************** */
/*                                   INIT                                     */
/* ************************************************************************** */

/* I2C Arbitration */
//void	i2c_arbitration(void)
//{
//	//	Read SDA state
//		//	If Ok to send, change state of SDA, sleep	-> MCU is master
//		//	Else, switch to slave mode.					-> MCU is slave
//	if (PINC & (1u << PINC4))
//	{
//		PORTC ^= (1u << PORTC4);
//		_delay_ms(1000);
//		uart_printstr("I'm master\r\n");
//	}
//	else
//	{
//		i2c_slave_init(SLAVEADDR);
//		uart_printstr("I'm slave\r\n");
//	}
//}

/* ************************************************************************** */
/*                                 INTERRUPTS                                 */
/* ************************************************************************** */
ISR(TWI_vect) {
	uint8_t status = TWSR & 0xF8;
	uint8_t	data= 0;
	TWCR |= (1 << TWINT);
	switch (status) {
		case 0x60:
			// uart_printstr("SLA+W received, ACK returned\r\n");
			break;
		case 0x68:
			// uart_printstr("Arbitration lost, own SLA+W received, ACK returned\r\n");
			break;
		case 0x70:
			// uart_printstr("General call received, ACK returned\r\n");
			break;
		case 0x78:
			// uart_printstr("Arbitration lost, General call received, ACK returned\r\n");
			break;
		case 0x80 :
			data = i2c_read_ack();
			uart_printhex(data);
			uart_printstr("address call, Data received, ACK returned\r\n");
			break;
		case 0x88 :
			uart_printstr("address call, Data received, NACK returned\r\n");
			break;
		case 0x90:
			// uart_printstr("General call, Data received, ACK returned\r\n");
			break;
		case 0x98:
			// uart_printstr("General call, Data received, NACK returned\r\n");
			break;
		case TW_SR_STOP :
			// uart_printstr("Stop or repeated start condition received while selected\r\n");
			break;
		default:
			uart_printstr("Error\r\n");
			break;
	}
	// TWCR |= (1 << TWINT);
	// TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA) | (1 << TWIE);
}

/**
 * @brief Initialize I2C slave
 *
 * @param address -- Slave address
 */
void i2c_slave_init(uint8_t address){
    TWAR = (address << 1); /* load address into TWI address register */
    // TWCR=0x0;   //WARNING
    TWCR = (1<<TWEA) | (1<<TWIE) | (1<<TWEN); /* set the TWCR to enable address matching and enable TWI, clear TWINT, enable TWI interrupt */
}


/* ************************************************************************** */
/*                                    MAIN                                    */
/* ************************************************************************** */
int main() {
	countdown();
	for (;;);
	i2c_init();
	i2c_slave_init(0x42);
	uart_init();
	i2c_slave_init(0x42);
	SREG |= (1 << 7); /* Enable global interrupts */

	/* Lets ping */
	while (1) {
		// _delay_ms(1000);
	}
	return (0);
}
