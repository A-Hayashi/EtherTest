#include "Xcp_Internal.h"

Xcp_BufferType Xcp_Buffers[XCP_MAX_RXTX_QUEUE];
Xcp_FifoType   Xcp_FifoFree;
Xcp_FifoType   Xcp_FifoRx = {.front=NULL, .back=NULL,  .free = &Xcp_FifoFree, .lock=NULL };
Xcp_FifoType   Xcp_FifoTx = {.front=NULL, .back=NULL,  .free = &Xcp_FifoFree,.lock=NULL  };
int                 Xcp_Connected;
int                 Xcp_Inited;
Xcp_MtaType         Xcp_Mta;


void Xcp_Init(void)
{
    Xcp_Fifo_Init(&Xcp_FifoFree, Xcp_Buffers, Xcp_Buffers+sizeof(Xcp_Buffers)/sizeof(Xcp_Buffers[0]));

    Xcp_Inited = 1;
}

void Xcp_RxIndication(uint8* data, uint16 len)
{
    if(len > XCP_MAX_DTO) {
        DEBUG(DEBUG_HIGH, "Xcp_RxIndication - length %d too long\n", len);
        return;
    }

    if(len == 0)
        return;

    FIFO_GET_WRITE(Xcp_FifoRx, it) {
        memcpy(it->data, data, len);
        it->len = len;

    }
}

static Std_ReturnType Xcp_CmdShortUpload(uint8 pid, void* data, int len)
{
    DEBUG(DEBUG_HIGH, "Received short upload\n");
    uint8  count = GET_UINT8 (data, 0);
    uint8  ext   = GET_UINT8 (data, 2);
    uint32 addr  = GET_UINT32(data, 3);

    if(count > XCP_MAX_CTO - XCP_ELEMENT_SIZE) {
        RETURN_ERROR(XCP_ERR_CMD_SYNTAX, "Xcp_CmdShortUpload - Too long data requested\n");
    }

    Xcp_MtaInit(&Xcp_Mta, addr, ext);
    if(Xcp_Mta.read == NULL) {
        RETURN_ERROR(XCP_ERR_CMD_SYNTAX, "Xcp_CmdShortUpload - invalid memory address\n");
    }

    FIFO_GET_WRITE(Xcp_FifoTx, e) {
        SET_UINT8 (e->data, 0, XCP_PID_RES);
        if(XCP_ELEMENT_SIZE > 1)
            memset(e->data+1, 0, XCP_ELEMENT_SIZE - 1);
        Xcp_MtaRead(&Xcp_Mta, e->data + XCP_ELEMENT_SIZE, count);
        e->len = count + XCP_ELEMENT_SIZE;
    }
    return E_OK;
}

/**************************************************************************/
/**************************************************************************/
/*************************** COMMAND PROCESSOR ****************************/
/**************************************************************************/
/**************************************************************************/

/**
 * Structure holding a map between command codes and the function
 * implementing the command
 */
static Xcp_CmdListType Xcp_CmdList[1] = {
		{ .fun = Xcp_CmdShortUpload         , .len = 8, .lock=1 }
};

/**
 * Xcp_Recieve_Main is the main process that executes all received commands.
 *
 * The function queues up replies for transmission. Which will be sent
 * when Xcp_Transmit_Main function is called.
 */
void Xcp_Recieve_Main()
{
    FIFO_FOR_READ(Xcp_FifoRx, it) {
        uint8 pid = GET_UINT8(it->data,0);

        /* process standard commands */
//        Xcp_CmdListType* cmd = Xcp_CmdList+pid;
        Xcp_CmdListType* cmd = Xcp_CmdList;
        if(cmd->fun) {

            if(cmd->len && it->len < cmd->len) {
                DEBUG(DEBUG_HIGH, "Xcp_RxIndication_Main - Len %d to short for %u\n", it->len, pid);
                return;
            }
            cmd->fun(pid, it->data+1, it->len-1);
        } else {
            Xcp_TxError(XCP_ERR_CMD_UNKNOWN);
        }
    }
}


/**
 * Xcp_Transmit_Main transmits queued up replies
 */
void Xcp_Transmit_Main(){
    Xcp_BufferType* item = Xcp_Fifo_Get(&Xcp_FifoTx);
    while(item) {
        if(Xcp_Transmit(item->data, item->len) != E_OK) {
            Xcp_Fifo_Put_Front(&Xcp_FifoTx, item);
            break;
        } else {
            Xcp_Fifo_Free(&Xcp_FifoTx, item);
        }
        item = Xcp_Fifo_Get(&Xcp_FifoTx);
    }
}

void Xcp_MainFunction(void)
{
	Xcp_Recieve_Main();
    Xcp_Transmit_Main();
}


/* RETURN_ERRORからもコール */
void Xcp_TxError(Xcp_ErrorType code)
{
    FIFO_GET_WRITE(Xcp_FifoTx, e) {
        SET_UINT8 (e->data, 0, XCP_PID_ERR);
        SET_UINT8 (e->data, 1, code);
        e->len = 2;
    }
}

/**
 * Xcp_TxSuccess sends a basic RES response without
 * extra data to master
 */

/* RETURN_SUCCESSからコール */
void Xcp_TxSuccess()
{
    FIFO_GET_WRITE(Xcp_FifoTx, e) {
        SET_UINT8 (e->data, 0, XCP_PID_RES);
        e->len = 1;
    }
}






