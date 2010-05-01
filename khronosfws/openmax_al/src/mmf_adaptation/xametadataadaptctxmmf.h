#ifndef XAMETADATAADAPTCTXMMF_H
#define XAMETADATAADAPTCTXMMF_H

#include "xaadaptationmmf.h"

/* TYPEDEFS */

typedef struct XAMetadataAdaptationMMFCtx_ XAMetadataAdaptationMMFCtx;

/* FUNCTIONS */
XAAdaptationBaseCtx* XAMetadataAdaptCtxMMF_Create( XADataSource *pDataSrc );
XAresult XAMetadataAdaptCtxMMF_PostInit( XAAdaptationMMFCtx* bCtx );
void XAMetadataAdaptCtxMMF_Destroy( XAAdaptationMMFCtx* bCtx );
#endif /*XAMETADATAADAPTCTXMMF_H*/

