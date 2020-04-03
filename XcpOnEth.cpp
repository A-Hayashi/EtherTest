#include "EtherCard/EtherCard.h"
#include "Xcp_Internal.h"
//
static uint16_t Xcp_EthCtrRx = 0;
static uint16_t Xcp_EthCtrTx = 0;
static boolean Xcp_Connected = 1;

void Xcp_SoAdIfRxIndication(uint8 *data, uint16 len) {

	if(len<=4){
		DEBUG(DEBUG_HIGH, "Xcp_SoAdRxIndication - too short length:%d\n", len);
		return;
	}

	uint16 ctr = (data[3] << 8) | data[2];
    if(Xcp_Connected && ctr && ctr != Xcp_EthCtrRx) {
		DEBUG(DEBUG_HIGH, "Xcp_SoAdRxIndication - ctr:%d differs from expected: %d\n", ctr, Xcp_EthCtrRx);
		return;
    }

    Xcp_EthCtrRx = ctr+1;
    Xcp_RxIndication(data+4, len-4);
}

Std_ReturnType Xcp_Transmit(uint8* data, uint16 len)
{
    uint8 buf[len+4];

    SET_UINT16(buf, 0, len);
    SET_UINT16(buf, 2, ++Xcp_EthCtrTx);
    memcpy(buf+4, data, len);

    return SoAdIf_Transmit(data, len+4);
}


//
///**
// * Called when the core of xcp have received a transport layer command
// * @param pid
// * @param data
// * @param len
// * @return
// */
//
////CMD
//extern Std_ReturnType Xcp_CmdTransportLayer(uint8 pid, void* data, int len)
//{
//    Xcp_TxError(XCP_ERR_CMD_UNKNOWN);
//    return E_OK;
//}
