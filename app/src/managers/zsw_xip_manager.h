#ifndef ZSW_XIP_MANAGER_H
#define ZSW_XIP_MANAGER_H

int _zsw_xip_enable(const char *requester);
int _zsw_xip_disable(const char *requester);

#ifdef CONFIG_NORDIC_QSPI_NOR_XIP
#define zsw_xip_enable() _zsw_xip_enable(__FUNCTION__)
#define zsw_xip_disable() _zsw_xip_disable(__FUNCTION__)
#else
#define zsw_xip_enable()
#define zsw_xip_disable()
#endif

#endif // ZSW_XIP_MANAGER_H