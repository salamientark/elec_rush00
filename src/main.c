/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 16:31:16 by dbaladro          #+#    #+#             */
/*   Updated: 2025/03/09 11:42:11 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/atm328p.h"
#include <stdint.h>
#include <time.h>
#include <util/twi.h>

#define SLAVE_ADDR 0x42

#define WAIT_START 0
#define GET_MASTER 1
#define GET_SLAVE 2
#define MASTER 3
#define SLAVE 4
#define MASTER_RECEIVER 5
#define MASTER_TRANSMITER 6
#define SLAVE_RECEIVER 7
#define SLAVE_TRANSMITER 8

uint8_t	g_status = WAIT_START; /* Define slave or mnaster */

/** *All the information that I needed for this can be found at:
 *  - https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061A.pdf
*/


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


/* ************************************************************************** */
/*                                 INTERRUPTS                                 */
/* ************************************************************************** */
// ISR(TWI_vect) {
// 	uint8_t status = TWSR & 0xF8;
// 	uint8_t	data= 0;
// 	TWCR |= (1 << TWINT);
// 	switch (status) {
// 		case TW_SR_SLA_ACK:
// 			uart_printstr("SLA+W received, ACK returned\r\n");
// 			break;
// 		case 0x68:
// 			// uart_printstr("Arbitration lost, own SLA+W received, ACK returned\r\n");
// 			break;
// 		case 0x70:
// 			// uart_printstr("General call received, ACK returned\r\n");
// 			break;
// 		case 0x78:
// 			// uart_printstr("Arbitration lost, General call received, ACK returned\r\n");
// 			break;
// 		case 0x80 :
// 			data = i2c_read_ack();
// 			uart_printhex(data);
// 			uart_printstr("address call, Data received, ACK returned\r\n");
// 			break;
// 		case 0x88 :
// 			uart_printstr("address call, Data received, NACK returned\r\n");
// 			break;
// 		case 0x90:
// 			// uart_printstr("General call, Data received, ACK returned\r\n");
// 			break;
// 		case 0x98:
// 			// uart_printstr("General call, Data received, NACK returned\r\n");
// 			break;
// 		case TW_SR_STOP :
// 			// uart_printstr("Stop or repeated start condition received while selected\r\n");
// 			break;
// 		default:
// 			uart_printstr("Error\r\n");
// 			break;
// 	}
// 	// TWCR |= (1 << TWINT);
// 	// TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA) | (1 << TWIE);
// }

void i2c_get_master(void) {
	i2c_start(); /* Initiate conversation */
	uart_printstr("Start conversation\r\n");
	uint8_t status = TWSR & 0xF8;
	if (status != TW_START && status != TW_REP_START) { /* Check for errors */
		uart_printstr("Error on i2c_start()\r\n");
		return ;
	}
	if (status == 0x38){
		uart_printstr("Arbitration lost\r\n");
		return ;
	}
	i2c_write(SLAVE_ADDR << 1 | 0); /* Write slave address */
	if ((TWSR & 0xF8) != TW_MT_SLA_ACK) { // Check for SLA+R transmitted and ACK received
		uart_printstr("Address not found: 0x");
		uart_printhex((unsigned char)SLAVE_ADDR);
		uart_printstr("\r\n");
		return ;
	}
	uart_printstr("Change status to MASTER\r\n");
	g_status = MASTER;
}


/* I2C Arbitration */
void	i2c_arbitration(void)
{
	//	Read SDA state
	if ((PINC & (1u << PINC4) && (PINC & (1u << PINC5)))) {
		g_status = GET_MASTER;
		uart_printstr("Change status to GET_MASTER\r\n");
	}
	else
	{
		g_status = GET_SLAVE;
		TWCR |= (1 << TWEA);
		uart_printstr("change status to get slave\r\n");
		g_status = SLAVE;
		uart_printstr("I'm slave\r\n");
	}
}

/**
 * @brief Read button status
 *
* @param pin_x the pin register
* @param pin_x_bit the pin bit
* @return 1 if button is pressed, else 0
*/
uint8_t getButtonStatus(uint8_t pin_x, uint8_t port_bit) {
	return (pin_x & (1 << port_bit)); /* Read the port_bit position of the pin_x register */
}

/**
* @brief wait for the game start
*/
void	wait(void) {
	uint8_t	b1_last_state = 1;
	uint8_t	b1_state = 1;

	PORTB &= ~(1 << PORTB0)  /* Turn OFF LED */
			& ~(1 << PORTB1)
			& ~(1 << PORTB2)
			& ~(1 << PORTB4);
	/* Wait for start */
	while (g_status != MASTER && g_status != SLAVE) {
		b1_state = getButtonStatus(PIND, 2);
		if (b1_state == 0) {
			if (b1_last_state == 1) {
				_delay_ms(20);
				i2c_arbitration();
				b1_last_state = 0;
			}
			if (g_status == GET_MASTER)
				i2c_get_master();
			// if (g_status == SLAVE || g_status == MASTER)
			// 	break ;
		}
		if (b1_state && b1_last_state == 0) {
			if (g_status == GET_MASTER)
				i2c_stop();
			TWCR &= ~(1 << TWEA); /* Disable ACK */
			b1_last_state = 1;
			g_status = WAIT_START;
			uart_printstr(" Change Status to WAIT_START\r\n");
			_delay_ms(20);
		}
		_delay_ms(10);
	}
}

void	i2c_switch_master_receive(void) {
	uart_printstr("Switching to master receiver\r\n");
	if (g_status == MASTER) {
		i2c_start();
		uint8_t	status = TWSR & 0xF8;
		if (status != TW_START && status != TW_REP_START) { /* Check for errors */
			uart_printstr("Error on i2c_start()\r\n");
			return ;
		}
		i2c_write(SLAVE_ADDR << 1 | TW_READ); /* Write slave address */
		if ((TWSR & 0xF8) != TW_MR_SLA_ACK) { // Check for SLA+R transmitted and ACK received
			uart_printstr("Slave not found error");
			uart_printstr("\r\n");
			return ;
		}
		g_status = MASTER_RECEIVER;
		uart_printstr("Change status to MASTER_RECEIVER\r\n");
		return ;
	}
	
}

/**
 * @brief Get ready to play
 */
void	get_ready(void) {
	while (1) {
		if (g_status == MASTER_RECEIVER) {
			
			continue ;
		}
		else {
			continue ;
		}
	}
}


/* ************************************************************************** */
/*                                    MAIN                                    */
/* ************************************************************************** */
int main() {
	i2c_init();
	uart_init();

	/* Slave init */
	TWAR = (SLAVE_ADDR << 1); /* load address into TWI address register */
	TWCR = (1 << TWINT) | (1<<TWIE) | (1<<TWEN); /* set the TWCR to enable address matching and enable TWI, clear TWINT, enable TWI interrupt */

	SREG |= (1 << 7); /* Enable global interrupts */

	DDRB |= (1 << DDB4) | (1 << DDB2) | (1 << DDB1) | (1 << DDB0); /* Set pin 0 of port B as input */
	/* Lets ping */
	while (1) {
		wait();
		PORTB |= (1 << PORTB0) /* Turrn on LED */
				| (1 << PORTB1)
				| (1 << PORTB2)
				| (1 << PORTB4);
		_delay_ms(100);
		if (g_status == MASTER){
			i2c_switch_master_receive();
		}
		if (g_status != MASTER_RECEIVER && g_status != SLAVE) {
			g_status = WAIT_START;
			continue ;
		}
		get_ready();

		_delay_ms(1000);
	}
	return (0);
}
