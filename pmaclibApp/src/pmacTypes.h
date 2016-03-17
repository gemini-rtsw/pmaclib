#ifndef __INCpmacTypesH
#define __INCpmacTypesH

#define PMAC_PRIVATE FALSE

#if PMAC_PRIVATE
#define PMAC_LOCAL LOCAL
#else
#define PMAC_LOCAL
#endif

#define PMAC_MAX_CARDS  8   /* Maximum Number of Controller cards */

typedef volatile short	PMAC_DPRAM;
typedef int STATUS;
#define ERROR  -1
#define OK 	0

#endif
