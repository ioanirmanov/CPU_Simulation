#ifndef DATABUS_H
#define DATABUS_H

#pragma once

#include "memorybus.h"


class DataBus : public MemoryBus
{
public:
    DataBus();
    DataBus(class Memory *MemPointer, class Register *RegPointer);
    void WriteToMemory() override;
    void WriteToRegister();

    void SetValue(bool Val[24]);
};

#endif // DATABUS_H
