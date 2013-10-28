#ifndef __mii_buffering_h__
#define __mii_buffering_h__
#include "ethernet.h"
#include "swlock.h"
#include "hwlock.h"

#ifndef ETHERNET_USE_HARDWARE_LOCKS
#define ETHERNET_USE_HARDWARE_LOCKS 1
#endif

#ifdef __XC__
extern "C" {
#endif

typedef struct mii_packet_t {
  int length;           //!< The length of the packet in bytes
  int timestamp;        //!< The transmit or receive timestamp
  int filter_result;    //!< The bitfield of filter passes
  int src_port;         //!< The ethernet port which a packet arrived on
  int timestamp_id;     //!< Client channel number which is waiting for a
                        //   Tx timestamp
  int stage;            //!< What stage in the Tx or Rx path the packet has
  int tcount;           //!< Number of remaining clients who need to be send
                        //   this RX packet minus one
  int crc;              //!< The calculated CRC
  int forwarding;       //!< A bitfield for tracking forwarding of the packet
                        //   to other ports
  unsigned int data[(MAX_ETHERNET_PACKET_SIZE+3)/4];
} mii_packet_t;

typedef struct mempool_info_t {
  int *rdptr;
  int *wrptr;
  int *start;
  int *end;
#if !ETHERNET_USE_HARDWARE_LOCKS
  swlock_t lock;
#endif
} mempool_info_t;

typedef struct malloc_hdr_t {
  int next;
  mempool_info_t *info;
} malloc_hdr_t;

typedef unsigned * mii_mempool_t;
typedef unsigned * mii_buffer_t;
typedef unsigned * mii_rdptr_t;

mii_mempool_t mii_init_mempool(unsigned *buffer, int size);

mii_packet_t *mii_reserve(mii_mempool_t mempool,
                          REFERENCE_PARAM(unsigned *, end_ptr));

mii_packet_t *mii_reserve_at_least(mii_mempool_t mempool,
                                   REFERENCE_PARAM(unsigned *, end_ptr),
                                   int min_size);

void mii_commit(mii_packet_t *buf, unsigned *endptr);

void mii_free(mii_packet_t *buf);
mii_rdptr_t mii_init_my_rdptr(mii_mempool_t mempool);
mii_rdptr_t mii_update_my_rdptr(mii_mempool_t mempool, mii_rdptr_t rdptr);
mii_packet_t *mii_get_my_next_buf(mii_mempool_t mempool, mii_rdptr_t rdptr);
mii_packet_t *mii_get_next_buf(mii_mempool_t mempool);
unsigned *mii_get_wrap_ptr(mii_mempool_t mempool);
unsigned *mii_packet_get_wrap_ptr(mii_packet_t *buf);
int mii_get_and_dec_transmit_count(mii_packet_t *buf);

extern hwlock_t ethernet_memory_lock;

#ifdef __XC__
} // extern "C"
#endif


#endif //__mii_buffering_h__
