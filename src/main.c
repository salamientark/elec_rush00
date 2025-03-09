/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 16:31:16 by dbaladro          #+#    #+#             */
/*   Updated: 2025/03/09 20:45:36 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/atm328p.h"
#include <avr/io.h>

uint8_t	g_role = WAIT_START;
uint8_t	g_status = NONE; /* Define slave or mnaster */

/** *All the information that I needed for this can be found at:
 *  - https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061A.pdf
*/


/* ************************************************************************** */
/*                                 INTERRUPTS                                 */
/* ************************************************************************** */
ISR(TWI_vect) {
	uint8_t status = TWSR & 0xF8;
	TWCR &= ~(1 << TWIE); /* Disable TWI interrupt */
	uint8_t data;
	switch (status) {
		case TW_MR_DATA_ACK:
			if (g_role == MASTER) {
				// TWCR &= ~(1 << TWIE);
				data = i2c_read_ack();
				uart_printstr("Master received: 0x");
				uart_printhex(data);
				uart_printstr("\r\n");
				if (data == 0xBB) {
					uart_printstr("OK\r\n");
				}
				
				if (data == 0xFF){
					// i2c_stop();
					i2c_start();
					i2c_write(SLAVE_ADDR << 1 | TW_READ);
					TWCR |= (1 << TWIE);
					uart_printstr("MASTER RESET\r\n");
				}
				TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA); // Clear interrupt flag, enable ACK
				break ;
			}
		// case TW_ST_SLA_ACK:
		case TW_SR_DATA_ACK:
			if (g_role == SLAVE && g_status == READY) {
				uart_printstr("Slave ready YEAH");
				// i2c_write(SLAVE_READY_TO_PLAY);
				g_status = SEND_READY;
				// TWDR = 0xBB;
				uart_printstr("Slave ready to play\r\n");
			}
				// if (TWDR == SLAVE_LOST || TWDR == MASTER_LOST) {
				// 	g_status = WAIT_START;
				// 	uart_printstr("Change status to WAIT_START\r\n");
				// }
				// if (TWDR == SLAVE_READY_TO_PLAY) {
				// 	g_status = SLAVE;
				// 	uart_printstr("Change status to SLAVE\r\n");
				// }
            // g_status = SLAVE_RECEIVER;
			uart_printstr("Change status to SLAVE_RECEIVER\r\n");
            TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA); // Clear interrupt flag, enable ACK
			break ;

		case TW_SR_SLA_ACK:
			TWCR = (1 << TWEN) | (0 << TWIE) | (1 << TWINT) | (1 << TWEA); // Clear interrupt flag, enable ACK
			uart_printstr("SLAVE RECEIVE SEND DATA\r\n");
			// i2c_write(SLAVE_READY_TO_PLAY);
			// TWDR = 0xBB;
			break ;
		case TW_ST_SLA_ACK:
			TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA); // Clear interrupt flag, enable ACK
			// uart_printstr("SLAVE SEND DATA\r\n");
			// TWDR = 0xBB;
			// i2c_write(SLAVE_READY_TO_PLAY);
			break ;
		case TW_ST_DATA_ACK:
			// if (g_status == SLAVE) {
			// 	if (TWDR == SLAVE_LOST || TWDR == MASTER_LOST) {
			// 		g_status = WAIT_START;
			// 		uart_printstr("Change status to WAIT_START\r\n");
			// 	}
			// 	if (TWDR == SLAVE_READY_TO_PLAY) {
			// 		g_status = SLAVE;
			// 		uart_printstr("Change status to SLAVE\r\n");
			// 	}
				uart_printstr("Slave transmitted data\r\n");
				// data = TWDR;
				// uart_printstr("Data received: 0x");
				// uart_printhex(data);
				uart_printstr("\r\n");
			// }
            TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA); // Clear interrupt flag, enable ACK
			break ;
		case TW_SR_STOP:
			TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA); // Clear interrupt flag, enable ACK
			break ;
		default:
			uart_printstr("=== INTERUPT CODE: ");
			uart_printhex(status);
			uart_printstr("\r\n");
            TWCR = (1 << TWEN) | (1 << TWIE) | (1 << TWINT) | (1 << TWEA); // Clear interrupt flag, enable ACK
			uart_printstr("Error or default\r\n");
			break;
	}
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
void	get_role(void) {
	uint8_t	b1_last_state = 1;
	uint8_t	b1_state = 1;

	PORTB &= ~(1 << PORTB0)  /* Turn OFF LED */
			& ~(1 << PORTB1)
			& ~(1 << PORTB2)
			& ~(1 << PORTB4);
	/* Wait for start */
	while (g_role != MASTER && g_role != SLAVE) {
		b1_state = getButtonStatus(PIND, 2);

		if (b1_state == 0) { /* Pressed button */
			if (b1_last_state == 1) {
				_delay_ms(20);
				i2c_arbitration();
				b1_last_state = 0;
			}
			if (g_status == GET_MASTER)
				i2c_get_master();
		}
		if (b1_state && b1_last_state == 0) { /* Released button */
			if (g_status == GET_MASTER)
				i2c_stop();
			TWCR &= ~(1 << TWEA); /* Disable ACK */
			b1_last_state = 1;

			/* Reset Role and status */
			g_role = WAIT_START; /* Change role to init */
			g_status = NONE;

			uart_printstr(" Change Status to WAIT_START\r\n");
			_delay_ms(20);
		}
		_delay_ms(10);
	}

	if (g_role == SLAVE){
		TWCR = (1 << TWEN) | (0 << TWIE) | (1 << TWINT) | (1 << TWEA); /* Clear interrupt flag, enable ACK */
		SREG &= ~(1 << 7); /* Disable global interrupts */
	}
	if (g_role == MASTER) 
		i2c_switch_master_receive();
}


/**
 * @brief Get ready to play
 */
void	get_ready(void) {
	// uint8_t b1_last_state = 0;
	uint8_t b1_state = 0;
	uart_printstr("Get ready to play\r\n");
	if (g_role == MASTER)
		TWCR |= (1 << TWINT) | (1 << TWIE); /* Enable TWI interrupt */
	while (1) {
		if (g_role == WAIT_START) {
			uart_printstr("Change status to WAIT_START ===> WTF\r\n");
			break ;
		}

		b1_state = getButtonStatus(PIND, 2);
		(void)b1_state;
		if (g_role == SLAVE) {
			uart_printstr("Inside Slave\r\n");
			// TWCR &= ~(1 << TWIE); /* Disable TWI interrupt */
			// SREG &= ~(1 << 7); /* Enable global interrupts */
			if (g_status == SEND_READY) {
				i2c_write(SLAVE_READY_TO_PLAY);
			}
			// if (b1_state)
			// 	g_status = READY;
			// while ((PINC & (1u << PINC4) && (PINC & (1u << PINC5)))) {}
				// TWDR = SLAVE_READY_TO_PLAY;
		}


		//
		// if (!b1_state && b1_last_state == 1){ /* Repress */
		// 	if (g_role == MASTER) {
		// 		uart_printstr("Master pressed again\r\n");
		// 		/*
		// 		 * Switch to master transmitter
		// 		 * Send info that it lost
		// 		 * back to beginiing of the program */
		// 		// i2c_switch_master_transmit();
		// 		// i2c_write(MASTER_LOST);
		// 		// i2c_stop();
		// 		// g_status = WAIT_START;
		// 	}
		// 	else {
		// 		uart_printstr("slave pressed again\r\n");
		// 		// g_status = WAIT_START;
		// 		// TWDR = SLAVE_LOST;
		// 		// break ;
		// 	}
		// }
		//
		// if (b1_state) { /* Unhold button */
		// 	b1_last_state = 1;
		//
		// 	if (g_role == MASTER) { /* RELEASE BUTTON MASTER */
		// 		uart_printstr("Master released button\r\n");
		// 		// i2c_start();
		// 		// i2c_write(SLAVE_ADDR << 1 | TW_READ);
		// 		// TWCR |= (1 << TWEA);
		// 		// uint8_t status = TWSR & 0xF8;
		// 		// if (status != TW_START && status != TW_REP_START) {
		// 		// 	uart_printstr("Error on i2c_start()\r\n");
		// 		// 	return ;
		// 		// }
		// 		_delay_ms(50);
		// 		uint8_t data = i2c_read_nack();
		// 		if (data == SLAVE_READY_TO_PLAY) {
		// 			uart_printstr("MASTER is ready to play\r\n");
		// 			g_status = START_COUNTER;
		// 			// i2c_stop();
		// 			// break ;
		// 		}
		// 		if (data == SLAVE_LOST) { /* Loose */
		// 			uart_printstr("Slave lost\r\n");
		// 			g_role = WAIT_START;
		// 			g_status = NONE;
		// 			// return ;
		// 		}
		// 		uart_printstr("MASTER received data\r\n");
		// 	}
		//
		//
		// 	else { /* RELEASE BUTTON SLAVE */
		// 		uart_printstr("Slave released button\r\n");
		// 		g_status = READY;
		// 		// TWCR &= ~(1 << TWEA); /* Disable ACK */
		// 		// TWCR |= (1 << TWIE);
		// 		i2c_write(SLAVE_READY_TO_PLAY);
		//
		//
		// 		// TWDR = SLAVE_READY_TO_PLAY;
		// 		// TWCR |= (1 << TWINT) | (1 << TWEN); /* Enable TWI,
		// 		// 									* Clear TWINT flag */
		// 		// while(!(TWCR & (1 << TWINT))) {} /* Wait for TWI flag set */
		//
		//
		// 		if ((TWSR & 0xF8) == TW_ST_DATA_NACK) {
		// 			uart_printstr("Slave & master is ready to play\r\n");
		// 			/* Let's play */
		// 			// break ;
		// 		}
		// 		uart_printstr("Waiting for master\r\n");
		// 	}
		// 	_delay_ms(10);
		// 	
		// }

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
	DDRD = 0b01101000; /// Set RGB LED as outputs in the DATA_DIRECTION_REGISTER
	/* Lets ping */
	while (1) {
		PORTD = 0b00000000; /// Turn off the RGB LED
		get_role();
		PORTB |= (1 << PORTB0) /* Turrn on LED */
				| (1 << PORTB1)
				| (1 << PORTB2)
				| (1 << PORTB4);
		_delay_ms(100);
		if (g_role == SLAVE) {
			SREG &= ~(1 << 7); /* Disable global interrupts */
			TWCR &= ~(1 << TWIE); /* Disable TWI interrupt */
			while (1) {
				// while ((TWSR & 0xF8) != TW_SR_SLA_ACK) {}
				uart_printstr("Slave sending DATA\r\n");
				i2c_write(0x23);
				// TWDR = 0x23;
				_delay_ms(100);
				// while (TWSR != TW_ST_DATA_ACK) {}
			}
		}
		uint8_t data;
		if (g_role == MASTER) {
			SREG &= ~(1 << 7); /* Disable global interrupts */
			TWCR &= ~(1 << TWIE); /* Disable TWI interrupt */
			while (1) {
				// uart_printstr("Master GO START\r\n");
				// _delay_ms(10);
				uart_printstr("Master GO READ DATA\r\n");
				data = i2c_read_ack();
				uart_printstr("Master received: 0x");
				uart_printhex(data);
				uart_printstr("\r\n");
				i2c_stop();
				i2c_start();
				uart_printstr("Master SLA_R\r\n");
				i2c_write(SLAVE_ADDR << 1 | TW_READ);
				_delay_ms(100);
			}
		}
		if (g_role != MASTER && g_role != SLAVE) {
			g_role = WAIT_START;
			continue ;
		}
		/* OK */

		get_ready();
		// if (g_role == WAIT_START)
		// 	continue ;


		PORTD = 0b00100000; /// Turn on the RGB LED
		_delay_ms(1000);
	}
	return (0);
}
