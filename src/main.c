/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 16:31:16 by dbaladro          #+#    #+#             */
/*   Updated: 2025/03/08 21:02:47 by fguarrac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/atm328p.h"

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
