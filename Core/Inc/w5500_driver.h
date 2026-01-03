#ifndef W5500_DRIVER_H_
#define W5500_DRIVER_H_
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "main.h"
#include "socket.h"
#include "dhcp.h"
#include "httpServer.h"

void w5500_init(void);
void w5500_spi_init(void);
void wizchip_cs_pin_init(void);

#endif
