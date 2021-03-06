/*
 * VMCACHE.H -- Public definitions for the VMcache interface.
 */

#define	VM_READONLY		0001
#define	VM_READWRITE		0002
#define	VM_WRITEONLY		0004
#define	VM_ASYNC		0010
#define	VM_SYNC			0020
#define	VM_LOCKFILE		0040
#define	VM_DESTROYREGION	0100
#define	VM_CANCELREFCNT		0200

void	*vm_initcache();
void	*vm_cacheregion();
