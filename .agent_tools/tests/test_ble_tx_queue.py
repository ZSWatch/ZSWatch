#!/usr/bin/env python3
"""Exercise the production NUS queue with bounded ATT capacity and reconnects.

No hardware. Compile the actual queue functions against a deterministic kernel /
transport adapter, including failures that require a later work-queue invocation.
"""
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]


class BleTxQueueTests(unittest.TestCase):
    def test_production_queue(self):
        source = (ROOT / 'app/src/ble/ble_comm.c').read_text()
        queue = source[source.index('/* A complete NUS message'):source.index('\nvoid ble_comm_set_pairable')]
        harness = r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define MIN(a,b) ((a)<(b)?(a):(b))
#define ARG_UNUSED(x) (void)(x)
#define K_NO_WAIT 0
#define K_MSEC(x) (x)
#define BT_HCI_ERR_REMOTE_USER_TERM_CONN 0x13
struct bt_conn { int refs; uint16_t mtu; int disconnects; };
struct k_spinlock { int unused; };
typedef int k_spinlock_key_t;
struct k_work { int unused; };
struct k_work_delayable { int unused; };
struct fifo { void *first, *last; };
#define K_FIFO_DEFINE(name) struct fifo name
#define K_WORK_DELAYABLE_DEFINE(name,fn) struct k_work_delayable name
static struct bt_conn *current_conn;
static struct k_spinlock ble_tx_lock;
static uint32_t ble_tx_generation;
static bool ble_tx_failed;
static int schedules, allocations, budget=15, attempts, fail_at=-1, fatal=-EIO;
static bool alloc_fail, in_isr;
static int64_t clock_ms;
static uint8_t received[20000];
static size_t received_len;
static k_spinlock_key_t k_spin_lock(struct k_spinlock *lock) { (void)lock; return 0; }
static void k_spin_unlock(struct k_spinlock *lock, int key) { (void)lock; (void)key; }
static int k_is_in_isr(void) { return in_isr; }
static int64_t k_uptime_get(void) { return clock_ms; }
static void *k_malloc(size_t n) { if (alloc_fail) return NULL; allocations++; return malloc(n); }
static void k_free(void *p) { allocations--; free(p); }
static struct bt_conn *bt_conn_ref(struct bt_conn *p) { p->refs++; return p; }
static void bt_conn_unref(struct bt_conn *p) { p->refs--; }
static uint16_t bt_gatt_get_mtu(struct bt_conn *p) { return p->mtu; }
static int bt_conn_disconnect(struct bt_conn *p,int why) { (void)why; p->disconnects++; return 0; }
static void k_fifo_put(struct fifo *q,void *p) {
    *(void **)p=NULL;
    if (q->last) *(void **)q->last=p; else q->first=p;
    q->last=p;
}
static void *k_fifo_get(struct fifo *q,int timeout) {
    (void)timeout; void *p=q->first;
    if (p) { q->first=*(void **)p; if (!q->first) q->last=NULL; }
    return p;
}
static int k_work_schedule(struct k_work_delayable *work,int delay) { (void)work;(void)delay; schedules++; return 1; }
static int k_work_reschedule(struct k_work_delayable *work,int delay) { return k_work_schedule(work,delay); }
static int ble_transport_send(struct bt_conn *p,const uint8_t *data,uint16_t len) {
    assert(p==current_conn); assert(len<=p->mtu-3);
    attempts++;
    if (attempts==fail_at) return fatal;
    if (!budget) return -ENOMEM;
    budget--; assert(received_len+len<=sizeof(received));
    memcpy(received+received_len,data,len); received_len+=len; return 0;
}
'''
        cases = r'''
static void drain(void) {
    for(int i=0;i<100 && (ble_tx_active || ble_tx_queue.first);i++) {
        budget=15; clock_ms+=5; ble_tx_work_fn(NULL);
    }
    assert(!ble_tx_active && !ble_tx_queue.first && ble_tx_bytes==0 && allocations==0);
}
int main(void) {
    struct bt_conn first={.refs=1,.mtu=247}, next={.refs=1,.mtu=23};
    uint8_t list[6144], original[6144], after[]={0x7b,0x7d};
    for(size_t i=0;i<sizeof(list);i++) original[i]=list[i]=(uint8_t)(i*17);
    assert(ble_comm_send(list,sizeof(list))==-ENOTCONN);
    current_conn=&first; ble_tx_generation=1;
    assert(ble_comm_send(NULL,1)==-EINVAL && ble_comm_send(list,0)==-EINVAL);
    assert(ble_comm_send(list,8193)==-EINVAL);
    in_isr=true;assert(ble_comm_send(list,1)==-EINVAL);in_isr=false;
    alloc_fail=true;assert(ble_comm_send(list,1)==-ENOMEM);alloc_fail=false;
    assert(first.refs==1 && ble_tx_bytes==0);
    // A list larger than the actual 15 ATT buffers must pause without truncation
    // or duplicate fragments. Caller buffers may be freed/reused after enqueue.
    assert(ble_comm_send(list,sizeof(list))==0);
    memset(list,0,sizeof(list));assert(ble_comm_send(after,sizeof(after))==0);
    ble_tx_work_fn(NULL);
    assert(received_len==15*244 && ble_tx_active && ble_tx_offset==15*244);
    assert(memcmp(received,original,received_len)==0 && schedules>=3);
    drain();
    assert(received_len==sizeof(original)+sizeof(after));
    assert(memcmp(received,original,sizeof(original))==0);
    assert(memcmp(received+sizeof(original),after,sizeof(after))==0 && first.refs==1);
    // Admission is all-or-nothing when heap budget is full; rejected bytes never
    // reach the wire. A queued connection reference cannot target a new session.
    received_len=0;assert(ble_comm_send(original,sizeof(original))==0);
    assert(ble_comm_send(original,sizeof(original))==-ENOBUFS);
    current_conn=&next;ble_tx_generation++;drain();
    assert(received_len==0 && first.refs==1);
    assert(ble_comm_send(original,1000)==0);drain();
    assert(received_len==1000 && memcmp(received,original,1000)==0 && next.refs==1);
    // Generation fencing also protects a reconnect reusing the same bt_conn.
    received_len=0;assert(ble_comm_send(original,1000)==0);
    ble_tx_generation++;drain();assert(received_len==0);
    // A permanent failure after a prefix must close/fence the connection rather
    // than append another JSON object to an incomplete frame.
    current_conn=&first;ble_tx_generation++;first.disconnects=0;
    assert(ble_comm_send(original,1000)==0 && ble_comm_send(after,sizeof(after))==0);
    budget=15;fail_at=attempts+2;ble_tx_work_fn(NULL);
    assert(received_len==244 && first.disconnects==1 && ble_tx_failed);
    assert(ble_comm_send(after,sizeof(after))==-ENOTCONN);drain();
    assert(received_len==244 && first.refs==1);
    // Expiration after temporary pressure likewise fences a partial frame.
    ble_tx_failed=false;ble_tx_generation++;received_len=0;fail_at=-1;
    assert(ble_comm_send(original,6144)==0);budget=15;ble_tx_work_fn(NULL);
    assert(ble_tx_active);clock_ms+=30001;ble_tx_work_fn(NULL);
    assert(ble_tx_failed && first.disconnects==2);drain();
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            p = Path(directory)
            (p / 'test.c').write_text(harness + queue + cases)
            subprocess.run(['cc', '-std=c11', '-Wall', '-Wextra', '-Werror',
                            '-fsanitize=address,undefined', str(p / 'test.c'),
                            '-o', str(p / 'test')], check=True)
            subprocess.run([str(p / 'test')], check=True)


if __name__ == '__main__':
    unittest.main()
