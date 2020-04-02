/*
 * Xcp_cfg.h
 *
 *  Created on: 2020/04/01
 *      Author: Akinori
 */

#ifndef XCP_CFG_H_
#define XCP_CFG_H_

#define XCP_MAX_RXTX_QUEUE	10
//        Number of data packets the protocol can queue up for processing.
//        This should include send buffer aswell as STIM packet buffers.
//        This should at the minimum be set to
//            1 recieve packet + 1 send packet + number of DTO objects that
//            can be configured in STIM mode + allowed interleaved queue size.

#define XCP_MAX_DTO	255
#define XCP_MAX_CTO	255
//        Define the maximum size of a data/control packet. This will also
//        directly affect memory consumptions for XCP since the code will
//        always allocate XCP_MAX_DTO * XCP_MAX_RXTX_QUEUE bytes for
//        data buffers.


#endif /* XCP_CFG_H_ */
