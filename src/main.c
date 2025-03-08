/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 16:31:16 by dbaladro          #+#    #+#             */
/*   Updated: 2025/03/08 18:46:22 by fguarrac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/atm328p.h"

/** *All the information that I needed for this can be found at:
 *  - https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061A.pdf
 */

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
	uint8_t	data= 0;
	switch (TWSR & 0xF8) {
		case 0x60:
			uart_printstr("SLA+W received, ACK returned\r\n");
			break;
		case 0x68:
			uart_printstr("Arbitration lost, own SLA+W received, ACK returned\r\n");
			break;
		case 0x70:
			uart_printstr("General call received, ACK returned\r\n");
			break;
		case 0x78:
			uart_printstr("Arbitration lost, General call received, ACK returned\r\n");
            TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE);
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
			uart_printstr("General call, Data received, ACK returned\r\n");
            TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE);
			break;
		case 0x98:
			uart_printstr("General call, Data received, NACK returned\r\n");
			break;
		case 0xA0:
			uart_printstr("Stop or repeated start condition received while selected\r\n");
            TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE);
			break;
		default:
			uart_printstr("Error\r\n");
            TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE);
			break;
	}
}

/**
 * @brief Initialize I2C slave
 *
 * @param address -- Slave address
 */
void i2c_slave_init(uint8_t address){
    TWAR = (address << 1); /* load address into TWI address register */
    TWAR = (1<<TWGCE);  /* General call recognition enable */
    // TWCR=0x0;   //WARNING
    TWCR = (1<<TWIE) | (1<<TWEA) | (1<<TWINT) | (1<<TWEN); /* set the TWCR to enable address matching and enable TWI, clear TWINT, enable TWI interrupt */
}


/* ************************************************************************** */
/*                                    MAIN                                    */
/* ************************************************************************** */
int main() {
	i2c_init();
	i2c_slave_init(0x42);
	uart_init();
	SREG |= (1u << 7);

	/* Lets ping */
	while (1) {
	}

	return (0);
}
