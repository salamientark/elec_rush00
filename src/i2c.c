/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   i2c.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 17:27:28 by dbaladro          #+#    #+#             */
/*   Updated: 2025/03/08 18:35:44 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/atm328p.h"

/**
 * @brief Init I2C interface
 */
void i2c_init(void) {
	/* Set SCL to 100Khhz */
	TWBR = 1; /* Clock division factor */
	TWSR |= (1 << TWPS1) | (1 << TWPS0); /* Set prescaler to 1024 */
}

/**
 * @brief Start I2C communication
 */
void i2c_start(void) {
	uart_printstr("Starting I2C communication\r\n");
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); /* Enable TWI
																				* Clear TWINT flag
																				* generate start condition */
	uart_printstr("Waiting to start I2C communication\r\n");
	while(!(TWCR & (1 << TWINT))) {} /* Wait for TWI flag set */
	uart_printstr("start ok\r\n");
}

/**
 * @brief Stop I2C communication (realease bus)
 */
void i2c_stop(void) {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); /* Enable TWI,
													   * Clear TWINT flag
													   * generate stop condition */
}

/**
* @brief Write data package
 *
 * @param address -- Slave address
 * @param mode -- 0 for write, 1 for read
 */
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
 * @brief Ping an I2C addresses
 *
 * @param addr -- I2C address
 */
void	i2c_ping_addr(uint8_t addr) {
	uart_printstr("Pinging address: 0x");
	uart_printhex((unsigned char)addr);
	i2c_start();
	if ((TWSR & 0xF8) != START) { /* Check for errors */
		i2c_stop();
		uart_printstr("Error on i2c_start()\r\n");
		return ;
	}
	i2c_write(addr << 1 | 0); /* General Master receive mode */
	if ((TWSR & 0xF8) != SLA_ACK_W) { // Check for SLA+R transmitted and ACK received
		i2c_stop();
		uart_printstr("Address not found: 0x");
		uart_printhex((unsigned char)addr);
		uart_printstr("\r\n");
		return ;
	}
	uart_printstr("Address found: 0x");
	uart_printhex((unsigned char)addr);
	uart_printstr("\r\n");
	i2c_stop();
}

/**
 * @brief Ping all I2C addresses
 */
void	i2c_ping(void) {
	for (uint8_t i = 0 ;i < 128; i++) {
		i2c_ping_addr(i);
		_delay_ms(10);
	}
}
