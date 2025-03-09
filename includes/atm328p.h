/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   atm328p.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dbaladro <dbaladro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/08 12:38:17 by dbaladro          #+#    #+#             */
/*   Updated: 2025/03/09 15:39:16 by dbaladro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ATM328P_H
# define ATM328P_H

# include <avr/io.h>
# include <util/delay.h>
# include <avr/interrupt.h>
# include <util/twi.h>

/* I2C MACRO */
/* Roles */
#define WAIT_START 0
#define MASTER 1
#define SLAVE 2


/* Status in program */
# define NONE 0
# define GET_MASTER 1
# define GET_SLAVE 2
# define READY 4

# define SLAVE_ADDR 0x42
# define SLAVE_READY_TO_PLAY 0x01
# define SLAVE_LOST 0xF0
# define MASTER_LOST 0xF1




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
void	i2c_slave_init(uint8_t address);
void 	i2c_start(void);
void 	i2c_stop(void);
void	i2c_write(uint8_t data);
uint8_t	i2c_read_ack(void);
uint8_t	i2c_read_nack(void);
uint8_t	i2c_ping_addr(uint8_t addr);
void	i2c_ping(void);
void	i2c_get_master(void);
void	i2c_arbitration(void);
void	i2c_switch_master_receive(void);
void	i2c_switch_master_transmit(void);

#endif
