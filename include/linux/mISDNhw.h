/* $Id: mISDNhw.h,v 1.0 2007/06/06 10:43:45 kkeil Exp $
 *
 * Author	Karsten Keil <kkeil@novell.com>
 *
 *   Basic declarations for the mISDN HW channels
 *
 * Copyright 2007  by Karsten Keil <kkeil@novell.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef MISDNHW_H
#define MISDNHW_H
#include <linux/mISDNif.h>
#include <linux/timer.h>

/* 
 * HW DEBUG 0xHHHHGGGG
 * H - hardware driver specific bits
 * G - for all drivers
 */

#define DEBUG_HW		0x00000001
#define DEBUG_HW_DCHANNEL	0x00000100
#define DEBUG_HW_DFIFO		0x00000200
#define DEBUG_HW_BCHANNEL	0x00001000
#define DEBUG_HW_BFIFO		0x00002000

#define MAX_DFRAME_LEN_L1	300
#define MAX_MON_FRAME		32
#define MAX_LOG_SPACE		2048
#define MISDN_COPY_SIZE		32

/* channel->Flags bit field */
#define FLG_TX_BUSY		0	/* tx_buf in use */
#define FLG_TX_NEXT		1	/* next_skb in use */
#define FLG_L1_BUSY		2	/* L1 is permanent busy */
#define FLG_OPEN		5	/* channel is in use */
#define FLG_ACTIVE		6	/* channel is activated */
#define FLG_BUSY_TIMER		7
/* channel type */
#define FLG_DCHANNEL		8	/* channel is D-channel */
#define	FLG_BCHANNEL		9	/* channel is B-channel */
#define FLG_ECHANNEL		10	/* channel is E-channel */
#define FLG_TRANSPARENT		12	/* channel use transparent data */
#define FLG_HDLC		13	/* channel use hdlc data */
#define FLG_L2DATA		14	/* channel use L2 DATA primitivs */
#define FLG_ORIGIN		15	/* channel is on origin site */
/* channel specific stuff */
/* arcofi specific */
#define FLG_ARCOFI_TIMER	16
#define FLG_ARCOFI_ERROR	17
/* isar specific */
#define FLG_INITIALIZED		16
#define FLG_DLEETX		17
#define FLG_LASTDLE		18
#define FLG_FIRST		19
#define FLG_LASTDATA		20
#define FLG_NMD_DATA		21
#define FLG_FTI_RUN		22
#define FLG_LL_OK		23
#define FLG_LL_CONN		24
#define FLG_DTMFSEND		25


#define MSK_INIT_DCHANNEL	((1<<FLG_DCHANNEL)|(1<<FLG_HDLC))
#define MSK_INIT_BCHANNEL	(1<<FLG_BCHANNEL)
#define MSK_INIT_ECHANNEL	(1<<FLG_ECHANNEL)


struct dchannel {
	struct mISDNdevice	dev;
	u_long			Flags;
	u_int			state;
	void			*l1;
	/* HW access */
	u_char			(*read_reg) (void *, u_char);
	void			(*write_reg) (void *, u_char, u_char);
	void			(*read_fifo) (void *, u_char *, int);
	void			(*write_fifo) (void *, u_char *, int);
	void			*hw;
	struct timer_list	timer;
	/* receive data */
	struct sk_buff		*rx_skb;
	int			maxlen;
	/* send data */
	struct sk_buff_head	squeue;
	struct sk_buff		*tx_skb;
	int			tx_idx;
	int			debug;
	/* statistics */
	int			err_crc;
	int			err_tx;
	int			err_rx;
};

typedef int	(dchannel_l1callback)(struct dchannel *, u_int);
extern int	create_l1(struct dchannel *, dchannel_l1callback *);

/* private L1 commands */
#define INFO0		0x8002
#define INFO1		0x8102
#define INFO2		0x8202
#define INFO3_P8	0x8302
#define INFO3_P10	0x8402
#define INFO4_P8	0x8502
#define INFO4_P10	0x8602
#define LOSTFRAMING	0x8702
#define ANYSIGNAL	0x8802
#define HW_POWERDOWN	0x8902
#define HW_RESET_REQ	0x8a02
#define HW_POWERUP_REQ	0x8b02
#define HW_DEACT_REQ	0x8c02
#define HW_ACTIVATE_REQ	0x8e02
#define HW_D_NOBLOCKED  0x8f02
#define HW_RESET_IND	0x9002
#define HW_POWERUP_IND	0x9102
#define HW_DEACT_IND	0x9202
#define HW_ACTIVATE_IND	0x9302
#define HW_DEACT_CNF	0x9402
#define HW_TESTLOOP	0x9502
#define HW_TESTRX_RAW	0x9602
#define HW_TESTRX_HDLC	0x9702
#define HW_TESTRX_OFF	0x9802

struct layer1;
extern int	l1_event(struct layer1 *, u_int);


struct bchannel {
	struct mISDNchannel	ch;
	int			nr;
	u_long			Flags;
	u_int			state;
	/* HW access */
	u_char			(*read_reg) (void *, u_char);
	void			(*write_reg) (void *, u_char, u_char);
	void			(*read_fifo) (void *, u_char *, int);
	void			(*write_fifo) (void *, u_char *, int);
	void			*hw;
	struct timer_list	timer;
	/* receive data */
	struct sk_buff		*rx_skb;
	int			maxlen;
	/* send data */
	struct sk_buff		*next_skb;
	struct sk_buff		*tx_skb;
	int			tx_idx;
	int			debug;
	/* statistics */
	int			err_crc;
	int			err_tx;
	int			err_rx;
};

extern int	mISDN_initdchannel(struct dchannel *, ulong, int);
extern int	mISDN_initbchannel(struct bchannel *, ulong, int);
extern int	mISDN_freedchannel(struct dchannel *);
extern int	mISDN_freebchannel(struct bchannel *);
extern int	dchannel_senddata(struct dchannel *, struct sk_buff *);
extern int	bchannel_senddata(struct bchannel *, struct sk_buff *);

static inline struct sk_buff *
mI_alloc_skb(unsigned int len, gfp_t gfp_mask)
{
	struct sk_buff	*skb;

	skb = alloc_skb(len + MISDN_HEADER_LEN, gfp_mask);
	if (likely(skb))
		skb_reserve(skb, MISDN_HEADER_LEN);
	return skb;
}

static inline struct sk_buff *
_alloc_mISDN_skb(u_int prim, u_int id, u_int len, void *dp, gfp_t gfp_mask)
{
	struct sk_buff	*skb = mI_alloc_skb(len, gfp_mask);
	struct mISDNhead *hh;

	if (!skb)
		return NULL;
	if (len)
		memcpy(skb_put(skb, len), dp, len);
	hh = mISDN_HEAD_P(skb);
	hh->prim = prim;
	hh->id = id;
	hh->len = len;
	return skb;
}	

static inline void
mI_recv_newhead(struct mISDNchannel *ch, u_int prim,
    u_int id, struct sk_buff *skb)
{
	struct mISDNhead *hh;

	if (!ch->rst) {
		dev_kfree_skb(skb);
		return;
	}
	hh = mISDN_HEAD_P(skb);
	hh->prim = prim;
	hh->id = id;
	hh->len = skb->len;
	if (ch->recv(ch->rst, skb))
		dev_kfree_skb(skb);
}
		
static inline void
_queue_data(struct mISDNchannel *ch, u_int prim,
    u_int id, u_int len, void *dp, gfp_t gfp_mask)
{
	struct sk_buff		*skb;

	if (!ch->rst)
		return;
	skb = _alloc_mISDN_skb(prim, id, len, dp, gfp_mask);
	if (!skb)
		return;
	if (ch->recv(ch->rst, skb))
		dev_kfree_skb(skb);
}

static inline void
queue_ch_frame(struct mISDNchannel *ch, u_int pr, int id, struct sk_buff *skb)
{
	struct mISDNhead *hh;

	if (!skb) {
		_queue_data(ch, pr, id, 0, NULL, GFP_ATOMIC);
	} else {
		if (ch->rst) {
			hh = mISDN_HEAD_P(skb);
			hh->prim = pr;
			hh->id = id;
			hh->len = skb->len;
			if (!ch->recv(ch->rst, skb))
				return;
		}
		dev_kfree_skb(skb);
	}
}

static inline int
get_next_bframe(struct bchannel *bch)
{
	bch->tx_idx = 0;
	if (test_bit(FLG_TX_NEXT, &bch->Flags)) {
		bch->tx_skb = bch->next_skb;
		if (bch->tx_skb) {
			struct mISDNhead *hh = mISDN_HEAD_P(bch->tx_skb);

			bch->next_skb = NULL;
			test_and_clear_bit(FLG_TX_NEXT, &bch->Flags);
			queue_ch_frame(&bch->ch, PH_DATA_CNF, hh->id, NULL);
			return 1;
		} else {
			test_and_clear_bit(FLG_TX_NEXT, &bch->Flags);
			printk(KERN_WARNING "B TX_NEXT without skb\n");
		}
	}
	bch->tx_skb = NULL;
	test_and_clear_bit(FLG_TX_BUSY, &bch->Flags);
	return 0;
}

static inline int
get_next_dframe(struct dchannel *dch)
{
	dch->tx_idx = 0;
	dch->tx_skb = skb_dequeue(&dch->squeue);
	if (dch->tx_skb) {
		struct mISDNhead *hh = mISDN_HEAD_P(dch->tx_skb);

		queue_ch_frame(&dch->dev.D, PH_DATA_CNF, hh->id, NULL);
		return 1;
	}
	dch->tx_skb = NULL;
	test_and_clear_bit(FLG_TX_BUSY, &dch->Flags);
	return 0;
}

static inline void
reflect_packets(struct mISDNstack *st, struct sk_buff *skb)
{
	struct sk_buff	*cskb;

	while (st) {
		cskb = skb_copy(skb, GFP_KERNEL);
		if (!cskb) {
			printk(KERN_WARNING "%s: no skb\n", __FUNCTION__);
			break;
		}
		mISDN_queue_message(st, cskb);
		st = st->smux.next;
	}
}

static inline u_int
get_sapi_tei(u_char *p)
{
	u_int	sapi, tei;

	sapi = *p >> 2;
	tei = p[1] >> 1;
	return sapi | (tei << 8);
}

#endif