// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

#define NBUCKETS 13

extern uint ticks;

struct {
  struct spinlock lock;
  struct buf buf[NBUF];
} bcache;

struct {
  struct spinlock lock;
  struct buf head;
} bcaches[NBUCKETS];

void
binit(void)
{
  struct buf *b;
  int i;

  initlock(&bcache.lock, "bcache");

  for(b = bcache.buf; b < bcache.buf+NBUF; b++){
    b->tick = 0;
    initsleeplock(&b->lock, "buffer");
  }

  for (i = 0; i < NBUCKETS; ++i) {
    initlock(&bcaches[i].lock, "bcache.bucket");
    bcaches[i].head.prev = &bcaches[i].head;
    bcaches[i].head.next = &bcaches[i].head;
  }
}

void resetlrunode(uint dev, uint blockno, struct buf **lrunode, struct buf* head) {
  (*lrunode)->dev = dev;
  (*lrunode)->blockno = blockno;
  (*lrunode)->valid = 0;
  (*lrunode)->refcnt = 1;
  (*lrunode)->tick = ticks+1;

  (*lrunode)->next = head->next;
  (*lrunode)->prev = head;
  head->next->prev = *lrunode;
  head->next = *lrunode;
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  struct buf *b, *lrunode = 0;
  int numbucket = blockno%NBUCKETS;
  acquire(&bcaches[numbucket].lock);

  // Is the block already cached?
  for(b = bcaches[numbucket].head.next; b != &bcaches[numbucket].head; b = b->next){
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;
      b->tick = ticks+1;
      release(&bcaches[numbucket].lock);
      acquiresleep(&b->lock);
      return b;
    }
    if (b->refcnt == 0) {
      if (lrunode == 0 || lrunode->tick > b->tick) lrunode = b;
    }
  }
  // use cur bucket buffer
  if (lrunode) {
    lrunode->dev = dev;
    lrunode->blockno = blockno;
    lrunode->valid = 0;
    lrunode->refcnt = 1;
    lrunode->tick = ticks+1;
    release(&bcaches[numbucket].lock);
    acquiresleep(&(lrunode->lock));
    return lrunode;
  }

  // Not cached.
  // Recycle the least recently used (LRU) unused buffer (use another bucket).
  int ou;
  while (1){
    lrunode = 0;
    for (b = bcache.buf; b < bcache.buf+NBUF; b++){
      if (b->refcnt == 0){
        if (lrunode == 0 || lrunode->tick > b->tick) lrunode = b;
      }
    }
    if (lrunode == 0) break;
    if (lrunode->tick == 0){
      acquire(&bcache.lock);
      if (lrunode->refcnt == 0){
        resetlrunode(dev, blockno, &lrunode, &(bcaches[numbucket].head));

        release(&bcache.lock);
        release(&bcaches[numbucket].lock);
        acquiresleep(&lrunode->lock);
        return lrunode;
      } else {
        release(&bcache.lock);
      }
    } else {  // Steal another bucket buffer
      ou = (lrunode->blockno)%NBUCKETS;
      acquire(&bcaches[ou].lock);
      if (lrunode->refcnt == 0){
        lrunode->next->prev = lrunode->prev;
        lrunode->prev->next = lrunode->next;
        resetlrunode(dev, blockno, &lrunode, &(bcaches[numbucket].head));

        release(&bcaches[numbucket].lock);
        release(&bcaches[ou].lock);
        acquiresleep(&lrunode->lock);
        return lrunode;
      } else {
        release(&bcaches[ou].lock);
      }
    }
  }
  release(&bcaches[numbucket].lock);
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  int no = (b->blockno)%NBUCKETS;

  acquire(&bcaches[no].lock);
  b->refcnt--;
  release(&bcaches[no].lock);
}

void
bpin(struct buf *b) {
  int no = (b->blockno)%NBUCKETS;

  acquire(&bcaches[no].lock);
  b->refcnt++;
  release(&bcaches[no].lock);
}

void
bunpin(struct buf *b) {
  int no = (b->blockno)%NBUCKETS;

  acquire(&bcaches[no].lock);
  b->refcnt--;
  release(&bcaches[no].lock);
}


