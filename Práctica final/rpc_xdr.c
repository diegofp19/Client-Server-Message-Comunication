/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "rpc.h"

bool_t
xdr_username (XDR *xdrs, username *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_opaque (xdrs, objp->username, 256))
		 return FALSE;
	return TRUE;
}
