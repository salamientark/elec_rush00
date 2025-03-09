/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   i2c.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 17:27:28 by dbaladro          #+#    #+#             */
/*   Updated: 2025/03/09 15:52:13 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/atm328p.h"

extern uint8_t	g_status;
extern uint8_t	g_role;

/**
 * @brief Init I2C interface
 */
void i2c_init(void) {
	/* Set SCL to 100Khhz */
	TWBR = 1; /* Clock division factor */
	TWSR |= (1 << TWPS1) | (1 << TWPS0); /* Set prescaler to 1024 */
}

/**
 * @brief Initialize I2C slave
 *
 * @param address -- Slave address
 */
void i2c_slave_init(uint8_t address){
    TWAR = (address << 1); /* load address into TWI address register */
    TWCR = (1 << TWINT) |  (1<<TWEA) | (1<<TWIE) | (1<<TWEN); /* set the TWCR to enable address matching and enable TWI, clear TWINT, enable TWI interrupt */
}

/**
 * @brief Start I2C communication
 */
void i2c_start(void) {
	// uart_printstr("Starting I2C communication\r\n");
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); /* Enable TWI
																				* Clear TWINT flag
																				* generate start condition */
	// uart_printstr("Waiting to start I2C communication\r\n");
	while(!(TWCR & (1 << TWINT))) {} /* Wait for TWI flag set */
	// uart_printstr("start ok\r\n");
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
	// TWCR &= ~((1 << TWSTA) | (1 << TWSTO));
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
* @return 1 if address is found, else 0
 */
uint8_t	i2c_ping_addr(uint8_t addr) {
	uart_printstr("Pinging address: 0x");
	uart_printhex((unsigned char)addr);
	uart_printstr("\r\n");
	i2c_start();
	uint8_t	status = TWSR & 0xF8;
	if (status != TW_START || status != TW_REP_START) { /* Check for errors */
		uart_printstr("Error on i2c_start()\r\n");
		return (0);
	}
	i2c_write(addr << 1 | 0); /* General Master receive mode */
	if ((TWSR & 0xF8) != TW_MT_SLA_ACK) { // Check for SLA+R transmitted and ACK received
		i2c_stop();
		uart_printstr("Address not found: 0x");
		uart_printhex((unsigned char)addr);
		uart_printstr("\r\n");
		return (0);
	}
	uart_printstr("Address found: 0x");
	uart_printhex((unsigned char)addr);
	uart_printstr("\r\n");
	i2c_stop();
	return (0x01);
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

/**
 * @brief Get master status
 *
* @return 1 if master, else 0
 */
void	i2c_get_master(void) {
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
	i2c_write(SLAVE_ADDR << 1 | TW_WRITE); /* Write slave address */
	if ((TWSR & 0xF8) != TW_MT_SLA_ACK) { // Check for SLA+R transmitted and ACK received
		uart_printstr("Address not found: 0x");
		uart_printhex((unsigned char)SLAVE_ADDR);
		uart_printstr("\r\n");
		return ;
	}
	uart_printstr("Change status to MASTER\r\n");
	/* Set role and status */
	g_role = MASTER;
	// g_status = 

	return ;
}

/**
 * @brief Check for arbitration
 */
void	i2c_arbitration(void)
{
	/*	Read SDA state */
	if ((PINC & (1u << PINC4) && (PINC & (1u << PINC5)))) {
		g_status = GET_MASTER;
		uart_printstr("Change status to GET_MASTER\r\n");
	}
	else
	{
		g_status = GET_SLAVE;
		TWCR |= (1 << TWEA);
		uart_printstr("change status to get slave\r\n");
		g_role = SLAVE;
		uart_printstr("I'm slave\r\n");
	}
}

/**
* @brief Switch from Master transmit to master receiver
*/
void	i2c_switch_master_receive(void) {
	uart_printstr("Switching to master receiver\r\n");
		uint8_t	status = TWSR & 0xF8;
	i2c_start();
	if (status != TW_START && status != TW_REP_START) { /* Check for errors */
		g_role = WAIT_START;
		uart_printstr("Error on i2c_start()\r\n");
		return ;
	}
	i2c_write(SLAVE_ADDR << 1 | TW_READ); /* Write slave address */
	if ((TWSR & 0xF8) != TW_MR_SLA_ACK) { // Check for SLA+R transmitted and ACK received
		g_role = WAIT_START;
		uart_printstr("Slave not found error");
		uart_printstr("\r\n");
		return ;
	}
	// g_status = MASTER_RECEIVER;
	uart_printstr("Change to MASTER_RECEIVER\r\n");
	return ;
}

/**
* @brief Switch from Master Reciever to master Transmitter
*/
void	i2c_switch_master_transmit(void) {
	uart_printstr("Switching to master transmitter\r\n");
	if (g_status == MASTER) {
		i2c_start();
		uint8_t	status = TWSR & 0xF8;
		if (status != TW_START && status != TW_REP_START) { /* Check for errors */
			uart_printstr("Error on i2c_start()\r\n");
			return ;
		}
		i2c_write(SLAVE_ADDR << 1 | TW_WRITE); /* Write slave address */
		if ((TWSR & 0xF8) != TW_MR_SLA_ACK) { // Check for SLA+R transmitted and ACK received
			uart_printstr("Slave not found error");
			uart_printstr("\r\n");
			return ;
		}
		// g_status = MASTER_TRANSMITER;
		uart_printstr("Change status to MASTER_TRANSMITER\r\n");
		return ;
	}
}
