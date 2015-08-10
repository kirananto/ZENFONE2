#ifndef __NET_SCHED_QCOMPAT_H
#define __NET_SCHED_QCOMPAT_H
#include <linux/version.h>

/* Backport some stuff. FIXME: I dont know when these entered 
   the kernel exactly. */
#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,14,0)

static inline u32 reciprocal_scale(u32 val, u32 ep_ro)
{
	return (u32)(((u64) val * ep_ro) >> 32);
}

#endif

#if LINUX_VERSION_CODE <= KERNEL_VERSION(3,16,0)
static inline void qdisc_qstats_backlog_dec(struct Qdisc *sch,
                                            const struct sk_buff *skb)
{
  sch->qstats.backlog -= qdisc_pkt_len(skb);
}

static inline void qdisc_qstats_backlog_inc(struct Qdisc *sch,
                                            const struct sk_buff *skb)
{
  sch->qstats.backlog += qdisc_pkt_len(skb);
}

static inline void __qdisc_qstats_drop(struct Qdisc *sch, int count)
{
  sch->qstats.drops += count;
}

static inline void qdisc_qstats_drop(struct Qdisc *sch)
{
  sch->qstats.drops++;
}

/*static inline void kvfree(const void *addr)
{
  if (is_vmalloc_addr(addr))
    vfree(addr);
  else
    kfree(addr);
}*/

#define ktime_get_ns() ktime_to_ns(ktime_get())
#define codel_stats_copy_queue(a,b,c,d) gnet_stats_copy_queue(a,c)
#define codel_watchdog_schedule_ns(a,b,c) qdisc_watchdog_schedule_ns(a,b)
#else
#define codel_stats_copy_queue(a,b,c,d) gnet_stats_copy_queue(a,b,c,d)
#define codel_watchdog_schedule_ns(a,b,c) qdisc_watchdog_schedule_ns(a,b,c)
#endif

#endif
#ifndef HAVE_SKB_GET_HASH
#define __skb_get_hash rpl__skb_get_rxhash
#define skb_get_hash rpl_skb_get_rxhash

extern u32 __skb_get_hash(struct sk_buff *skb);
static inline __u32 skb_get_hash(struct sk_buff *skb)
{
#ifdef HAVE_RXHASH
	if (skb->rxhash)
#ifndef HAVE_U16_RXHASH
		return skb->rxhash;
#else
		return jhash_1word(skb->rxhash, 0);
#endif
#endif
	return __skb_get_hash(skb);
}
#endif
u32 rpl__skb_get_rxhash(struct sk_buff *skb)
{
	struct flow_keys keys;
	u32 hash;

	if (!skb_flow_dissect(skb, &keys))
		return 0;

	/* get a consistent hash (same value on both flow directions) */
	if (((__force u32)keys.dst < (__force u32)keys.src) ||
	    (((__force u32)keys.dst == (__force u32)keys.src) &&
	     ((__force u16)keys.port16[1] < (__force u16)keys.port16[0]))) {
		swap(keys.dst, keys.src);
		swap(keys.port16[0], keys.port16[1]);
	}

	hash = jhash_3words((__force u32)keys.dst,
				  (__force u32)keys.src,
				  (__force u32)keys.ports, 0);
	if (!hash)
		hash = 1;

#ifdef HAVE_RXHASH
	skb->rxhash = hash;
#endif
	return hash;
}
EXPORT_SYMBOL_GPL(rpl__skb_get_rxhash);
