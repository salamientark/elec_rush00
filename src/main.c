/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 16:31:16 by dbaladro          #+#    #+#             */
/*   Updated: 2025/03/09 12:41:02 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/atm328p.h"
#include <avr/io.h>
#include <stdint.h>
#include <time.h>
#include <util/twi.h>

uint8_t	g_status = WAIT_START; /* Define slave or mnaster */

/** *All the information that I needed for this can be found at:
 *  - https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061A.pdf
*/


/* ************************************************************************** */
/*                                 INTERRUPTS                                 */
/* ************************************************************************** */
ISR(TWI_vect) {
	uint8_t status = TWSR & 0xF8;
	// TWCR |= (1 << TWINT);
	switch (status) {
		case TW_ST_SLA_ACK:
		case TW_SR_DATA_ACK:
            g_status = SLAVE_RECEIVER;
            TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA); // Clear interrupt flag, enable ACK
			break ;

		case TW_SR_STOP:
			TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA); // Clear interrupt flag, enable ACK
			break ;

		default:
            TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA); // Clear interrupt flag, enable ACK
			uart_printstr("Error\r\n");
			break;
	}
	// TWCR |= (1 << TWINT);
	// TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA) | (1 << TWIE);
}



/* I2C Arbitration */

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


/**
 * @brief Get ready to play
 */
void	get_ready(void) {
	uint8_t b1_last_state = 0;
	uint8_t b1_state = 0;
	uart_printstr("Get ready to play\r\n");
	while (1) {
		b1_state = getButtonStatus(PIND, 2);
		if (b1_state == 0 && b1_last_state == 1){ /* Repress */
			/* Player lose */
		}
		if (b1_state == 1) { /* Unhold button */
			if (g_status == MASTER_RECEIVER) {
				/* Wait for ping
				 * if press
				 *  - loose
				 * if no ping
				 *  - wait
				 * if ping
				 *  - Check release
				 */
				if (TWCR & (1 << TWINT))
				continue ;
			}
			else {
				/* On release
				 * Ping
				 *  if press
				 *   - Loose
				 *  if NACK
				 *   - Ping again
				 *  if ACK
				 *   - Lets Play !
				 */
				continue ;
			}
			
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
