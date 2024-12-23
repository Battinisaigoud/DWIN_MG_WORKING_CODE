#ifndef _EVSE_CP_IN_H
#define _EVSE_CP_IN_H

#define CP_IN_PIN (35)
#define CP_AVR_VALUE (100)

class CP_IN
{
public:
    int read_cp_in(void);
    // int32_t CP_IN:: read_cp_in(void)
    void read_cp_init(void);
};
#endif