#ifndef ZSW_XIP_MANAGER_H
#define ZSW_XIP_MANAGER_H

int _zsw_xip_enable(const char *func, int line);
int _zsw_xip_disable(const char *func, int line);

#define zsw_xip_enable() _zsw_xip_enable(__FUNCTION__, __LINE__)
#define zsw_xip_disable() _zsw_xip_disable(__FUNCTION__, __LINE__)

#endif // ZSW_XIP_MANAGER_H