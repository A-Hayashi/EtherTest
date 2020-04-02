#include "EtherCard/EtherCard.h"
#include "Xcp.h"
//
static uint16_t Xcp_EthCtrRx = 0;
static uint16_t Xcp_EthCtrTx = 0;
static boolean Xcp_Connected = 1;

void Xcp_SoAdIfRxIndication(uint8 *buff, uint16 len) {
	uint16 ctr = (buff[3] << 8) | buff[2];
    if(Xcp_Connected && ctr && ctr != Xcp_EthCtrRx) {
		DEBUG(DEBUG_HIGH, "Xcp_SoAdRxIndication - ctr:%d differs from expected: %d\n", ctr, Xcp_EthCtrRx);
    }

    Xcp_EthCtrRx = ctr+1;
//    Xcp_RxIndication(XcpRxPduPtr->SduDataPtr+4, XcpRxPduPtr->SduLength-4);
}

Std_ReturnType Xcp_Transmit(const void* data, int len)
{
    uint8 buf[len+4];

    SET_UINT16(buf, 0, len);
    SET_UINT16(buf, 2, ++Xcp_EthCtrTx);
    memcpy(buf+4, data, len);

    return 1;
//    return SoAdIf_Transmit(XCP_PDU_ID_TX, &pdu);
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
