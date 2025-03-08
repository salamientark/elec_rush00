/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uart.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:35:51 by dbaladro          #+#    #+#             */
/*   Updated: 2025/03/08 12:43:09 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/atm328p.h"

/**
 * @brief Initialize UART
 */
void uart_init(void) {
	/* Set baud rate */
	UBRR0 = round((double)F_CPU / 16 / UART_BAUDRATE - 1); /* Set Baud rate in the
												        * USART Baud Rate registers
												        */
	/* Set frame format: 8data bits, No parity, 1stop bit */
	UCSR0C = 0B00000110; /* Set mode to:
		                  * Asynchronous USART
						  * No Parity
						  * 1 Stop bit
						  * 8-bit word size
						  */
	/* Enable transmitter + 8data bits */
	UCSR0B |= (1 << TXEN0); /* Enable transmitter */
}

/**
 * @brief Receive a character to UART
 *
 * @param c -- The character to transmit
 */
uint8_t uart_rx(void) {
	while (!(UCSR0A & (1 << RXC0))) {} /* Wait until data is received */
	return UDR0; /* Get and return received data from buffer */
}

/**
 * @brief Transmit a character to UART
 */
void uart_tx(const char c) {
	while (!(UCSR0A & (1 << UDRE0))) {} /* Check if the transmit buffer is empty */
	UDR0 = c; /* Write the character to the USART data register */
}

/**
 * @brief Send string to UART transmitter
 *
 * @param str - string to send
 */
void uart_printstr(const char* str) {
	while (*str) {
		uart_tx(*str);
		str++;
	}
}
