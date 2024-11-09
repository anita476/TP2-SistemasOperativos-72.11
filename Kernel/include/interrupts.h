/*
 *   interrupts.h
 *
 *  Created on: Apr 18, 2010
 *      Author: anizzomc
 */

#ifndef INTERRUPS_H_
#define INTERRUPS_H_

#include <idtLoader.h>

void _irq00_handler(void);
void _irq01_handler(void);
void _int80_handler(void);

void _exception0_handler(void);
void _exception6_handler(void);
void _schedule(void);

void _cli(void);

void _sti(void);

void _hlt(void);

void pic_master_mask(uint8_t mask);

void pic_slave_mask(uint8_t mask);

void halt_cpu(void);

#endif /* INTERRUPS_H_ */
