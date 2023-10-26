#include "Cwsprestamos00024.h"

void Cwsprestamos00024::busquedaApartadoGE01( CListaCodigoMuebles *codigosMuebles, int iFlagSistemaApartado )
{
    int iFlagDesagotadoBod = 0;

    {
        if ( iFlagSistemaApartado == 1 &&
           codigosMuebles->nodo()->cQuienEntrega == 'B' &&
           !checarCodigoJoyeria( codigosMuebles->nodo()->lCodigo ) &&
           ChecarFamTarjetasTiempoAire( codigosMuebles->nodo()->lFamilia ) != 1 &&
           codigosMuebles->nodo()->iStatus == 1 &&
           codigosMuebles->nodo()->lCodigo > 99999L &&
           codigosMuebles->nodo()->lCodigo < 899999L &&
           checarRegUdiGE01( codigosMuebles ) != 1 &&
		   codigosMuebles->nodo()->lFamilia != 29811 )
        {
            codigosMuebles->nodo()->cFlagApartado = '1';
        }
        else
        {
            codigosMuebles->nodo()->cFlagApartado = '0';

            if ( codigosMuebles->nodo()->cAgotado == 'A' &&
                iFlagDesagotadoBod == 1 )
            {
                //para que no haga venta a futuro de articulo tipo 1
                if ( codigosMuebles->nodo()->cTipo == '1' && codigosMuebles->nodo()->cQuienEntrega == 'B' )
                {
                    // no hace el cambio de agotado a 'D' para no permitir venta a futuro
                }
                else
                {
                    //Se procede como una venta Normal
                    codigosMuebles->nodo()->cAgotado = 'D';
                }
            }
        }
    }
}

int Cwsprestamos00024::ChecarFamTarjetasTiempoAire( long lFamilia )
{
    int iFlag=0;
    //Tiempo Aire....
    if ( lFamilia==22415 ) iFlag=1;

    return iFlag;
}
