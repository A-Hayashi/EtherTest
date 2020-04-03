/* Copyright (C) 2010 Joakim Plate, Peter Fridlund
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "Xcp_Internal.h"

/**
 * Read a character from MTA
 * @return
 */
static uint8 Xcp_MtaGetMemory(Xcp_MtaType* mta)
{
    return *(uint8*)(mta->address++);
}

/**
 * Read a character from memory
 * @return
 */
static void Xcp_MtaReadMemory(Xcp_MtaType* mta, uint8* data, int len)
{
    memcpy(data, (void*)mta->address, len);
    mta->address += len;
}

/**
 * Write a character to memory
 * @return
 */
static void Xcp_MtaWriteMemory(Xcp_MtaType* mta, uint8* data, int len)
{
    memcpy((void*)mta->address, data, len);
    mta->address += len;
}


/**
 * Write a character to MTA
 * @param val
 */
static void Xcp_MtaPutMemory(Xcp_MtaType* mta, uint8 val)
{
    *(uint8*)(mta->address++) = val;
}

/**
 * Generic function that writes character to mta using put
 * @param val
 */
static void Xcp_MtaWriteGeneric(Xcp_MtaType* mta, uint8* data, int len)
{
    while(len-- > 0) {
        mta->put(mta, *(data++));    }
}

/**
 * Generic function that reads buffer from mta using get
 * @param val
 */
static void Xcp_MtaReadGeneric(Xcp_MtaType* mta, uint8* data, int len)
{
    while(len-- > 0) {
        *(data++) = mta->get(mta);
    }
}

/**
 * Set the MTA pointer to given address on given extension
 * @param address
 * @param extension
 */
void Xcp_MtaInit(Xcp_MtaType* mta, intptr_t address, uint8 extension)
{
    mta->address   = address;
    mta->extension = extension;
    mta->read      = Xcp_MtaReadGeneric;
    mta->write     = Xcp_MtaWriteGeneric;
    mta->flush     = NULL;

    if(extension == XCP_MTA_EXTENSION_MEMORY) {
        mta->get   = Xcp_MtaGetMemory;
        mta->put   = Xcp_MtaPutMemory;
        mta->read  = Xcp_MtaReadMemory;
        mta->write = Xcp_MtaWriteMemory;
    } else if(extension == XCP_MTA_EXTENSION_FLASH) {
        mta->get   = Xcp_MtaGetMemory;
        mta->put   = NULL;
        mta->read  = Xcp_MtaReadGeneric;
        mta->write = NULL;
    } else {
        mta->get   = NULL;
        mta->put   = NULL;
        mta->read  = NULL;
        mta->write = NULL;
    }
}

