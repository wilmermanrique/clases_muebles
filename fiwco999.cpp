#include "Cwsprestamos00024.h"

int Cwsprestamos00024::checarCod999181( sSolicitudServicio *sSolicitud, int &iFlagOptica, int &iFlagJoyeria )
{
	int iFlag = 0, i = 0;
	long lTmpCodigo = 0L;

    iFlagOptica = 0;
    iFlagJoyeria = 0;

    if( sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo == 999181L || sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo == 999644L )
    {
        iFlagOptica = 1;
    }

    if( iFlagOptica == 0 && sSolicitud->sQuienEntrega.codigosMuebles.nodo()->cFlagCodigoEspecial == '1' )
    {
        iFlagJoyeria = 1;
    }

    sSolicitud->sQuienEntrega.codigosMuebles.primero();
    for (i = 0; i < sSolicitud->sQuienEntrega.codigosMuebles.totalNodos() && iFlagOptica == 0 && iFlagJoyeria == 0; i++ )
    {
        if( sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo == 999181L || sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo == 999644L )
        {
            iFlagOptica = 1;
        }

        if( iFlagOptica == 0 && sSolicitud->sQuienEntrega.codigosMuebles.nodo()->cFlagCodigoEspecial == '1' )
        {
            iFlagJoyeria = 1;
        }
        sSolicitud->sQuienEntrega.codigosMuebles.siguiente();
    }

    if ( sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo > 999L )
    {
        if ( iFlagOptica == 1 )
        {
            if ( sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo != 999181L && !checarCodigoJoyeria( sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo ) && sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo != 999644L )
            {
                iFlag = 1;
            }
        }

        if ( iFlag != 1 )
        {
            if ( iFlagJoyeria == 1 )
            {
                if ( sSolicitud->sQuienEntrega.codigosMuebles.nodo()->cFlagCodigoEspecial != '1' )
                {
                    iFlag = 2;
                }
            }
        }
    }

    

    return iFlag;
}
