/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   atm328p.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:38:17 by dbaladro          #+#    #+#             */
/*   Updated: 2025/03/08 19:43:39 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ATM328P_H
# define ATM328P_H

# include <avr/io.h>
# include <util/delay.h>
# include <avr/interrupt.h>
# include <util/twi.h>

/* I2C MACRO */
# define START 0x08 /* START code after START was send */
# define SLA_ACK_W 0x18 /* Code for slave ACK */
# define SLA_ACK_R 0x40 /* Code for slave ACK */

/* ************************************************************************** */
/*                                   UART                                     */
/* ************************************************************************** */
void	uart_init(void);
uint8_t	uart_rx(void);
void	uart_tx(const char c);
void	uart_printstr(const char* str);
void	uart_printhex(unsigned char c);

/* ************************************************************************** */
/*                                   I2C                                      */
/* ************************************************************************** */
void	i2c_init(void);
void 	i2c_start(void);
void 	i2c_stop(void);
void	i2c_write(uint8_t data);
uint8_t	i2c_read_ack(void);
uint8_t	i2c_read_nack(void);
void	i2c_ping_addr(uint8_t addr);
void	i2c_ping(void);

#endif
