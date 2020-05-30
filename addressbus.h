#ifndef ADDRESSBUS_H
#define ADDRESSBUS_H

#pragma once

#include "memorybus.h"


class AddressBus : public MemoryBus
{
public:
    AddressBus();
    AddressBus(class Memory *MemPointer, class Register *RegPointer);
    //Abstract function for writing to memory
    void WriteToMemory() override;
};

#endif // ADDRESSBUS_H
