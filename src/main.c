/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */ /*   main.c                                             :+:      :+:    :+:   */ /*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 16:31:16 by dbaladro          #+#    #+#             */
/*   Updated: 2025/03/04 22:58:14 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/atm328p.h"

/** *All the information that I needed for this can be found at:
 *  - https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061A.pdf
 */

/* ************************************************************************** */ /*                                   INIT                                     */
/* ************************************************************************** */

/* Init I2C interface */
void i2c_init(void) {
	/* Set SCL to 100Khhz */
	TWBR = 1; /* Clock division factor */
	TWSR |= (1 << TWPS1) | (1 << TWPS0); /* Set prescaler to 1024 */
}

/**
 * @brief Start I2C communication
 */
void i2c_start(void) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); /* Enable TWI
																				* Clear TWINT flag
																				* generate start condition */
	while(!(TWCR & (1 << TWINT))) {} /* Wait for TWI flag set */
}

void i2c_stop(void) {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); /* Enable TWI,
													   * Clear TWINT flag
													   * generate stop condition */
}

/**
 * @brief Send address packet
 *
* @param address -- Slave address
* @param mode -- 0 for write, 1 for read
 */
// void i2c_address_packet(uint8_t address, uint8_t mode) {
// 	TWDR = (address << 1) | mode; /* Load slave address and mode */
//
// }

void	i2c_write(uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN); /* Enable TWI,
 										* Clear TWINT flag */
	while(!(TWCR & (1 << TWINT))) {} /* Wait for TWI flag set */
}

/**
 * @brief Read data package with ACK
 *
 * @return received data
 */
uint8_t	i2c_read_ack(void) {
	TWCR = (TWCR | (1 << TWINT) | (1 << TWEA) | (1 << TWEN)) & ~(1 << TWSTA); /* Enable TWI,
																				* Clear TWINT flag
																				* Enable ACK */
	while (!(TWCR & (1 << TWINT))) {} /* Wait for TWI flag set */
	return (TWDR); /* Return received data */
}

/**
 * @brief Read data package with NO ACK
 *
 * @return received data
 */
uint8_t	i2c_read_nack(void) {
	TWCR = (TWCR | (1 << TWINT) | (1 << TWEN)) & ~(1 << TWSTA); /* Enable TWI,
																 * Clear TWINT flag
																 * Disable ACK */
	while (!(TWCR & (1 << TWINT))) {} /* Wait for TWI flag set */
	return (TWDR); /* Return received data */
}

/**
 * @brief Initialize DATA_DIRECTION registers
 */
 void init(void) {

}

/* ************************************************************************** */
/*                                    MAIN                                    */
/* ************************************************************************** */

static void uart_printhex(unsigned char c) {
    uint8_t val = c >> 4;
    if (val < 10)
        val += '0';
    else
        val += 'a' - 10;
    uart_tx(val);
    val = c & 0xF;
    if (val < 10)
        val += '0';
    else
        val += 'a' - 10;
    uart_tx(val);
}

// void uart_printdec(uint16_t nbr) {
//     if (nbr > 9)
//         uart_printdec(nbr / 10);
//     uart_tx("0123456789"[nbr % 10]);
// }

void i2c_ping(void) {
	for (uint8_t i = 0 ;i < 128; i++) {
		_delay_ms(10);
		i2c_start();
		if ((TWSR & 0xF8) != START) { /* Check for errors */
			uart_printstr("Error on start\r\n");
			continue ;
		}
		i2c_write(i << 1 | 0); /* General Master receive mode */
		if ((TWSR & 0xF8) != SLA_ACK_W) { // Check for SLA+R transmitted and ACK received
			i2c_stop();
			// uart_printstr("Error Address: ");
			// uart_printhex(i);
			// uart_printstr("\r\n");
			continue ;
		}
		uart_printstr("OK on addr ");
		uart_printhex(i);
		uart_printstr("\r\n");
		i2c_stop();
	}
		uart_printstr("\r\n");
}


int main() {
	i2c_init();
	uart_init();

	while (1) {
		i2c_ping();
		/* Make mesurment */
		_delay_ms(1000);
	}

	return (0);
}
