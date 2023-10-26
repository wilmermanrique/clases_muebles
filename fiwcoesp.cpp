#include "Cwsprestamos00024.h"

int Cwsprestamos00024::codigosEspeciales( sSolicitudServicio *sSolicitud, int iModo, int iFlagLlamaOptica )
{
    int iFlag = 0, iFlagx = 1, iTipoEsp = 0, iFlagOptica = 0, iFlagJoyeria = 0;
    char cLog[512] = {0};
    long lActualCodigo = 0L;

    sSolicitud->sQuienEntrega.sSysMuebles.iFlagCapturarDescripcion = 0;

    iTipoEsp = checarCod999181( sSolicitud, iFlagOptica, iFlagJoyeria );

    
    lActualCodigo = sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo;

    if( iModo == 1 || iModo == 2  )
    {
        if( sSolicitud->sQuienEntrega.codigosMuebles.totalNodos() > 0 )
        {
            if( iFlagOptica == 1 )
            {
                if( iModo == 2 )
                {
                    sSolicitud->sQuienEntrega.codigosMuebles.ultimo();
                    mysnprintf( cLog, sizeof(cLog)-1, "NO se permite captura de otro código al facturar el %6ld", sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo );
                    //logx( cLog, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);
                    iFlag = 1;
                }
                else
                {

                    sSolicitud->sQuienEntrega.codigosMuebles.primero( );

                    while( iFlag == 0 && sSolicitud->sQuienEntrega.codigosMuebles.nodo()->iStatus > 0 )
                    {
                        if( sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo != 999181L &&
                            !checarCodigoJoyeria( sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo ) &&
                            sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo != 999644L )
                        {
                            mysnprintf( cLog, sizeof(cLog)-1, "No se permite la captura de otro código al facturar el código %6ld ", lActualCodigo );
                            //logx( cLog, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);
                            iFlag = 1;
                            sSolicitud->sQuienEntrega.sSysMuebles.iFlagLimpiarCapturaCodigo = 1;
                        }
                        sSolicitud->sQuienEntrega.codigosMuebles.siguiente( );
                    }
                }
            }

            if( iFlagJoyeria == 1 )
            {
                if( iModo == 2 )
                {
                    mysnprintf( cLog, sizeof(cLog)-1,"%s", "No se permite la captura de otro código diferente" );
                    //logx( cLog, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);
                    iFlag = 1;
                }
                else
                {
                    sSolicitud->sQuienEntrega.codigosMuebles.primero( );
                    
                    while( iFlag == 0 && sSolicitud->sQuienEntrega.codigosMuebles.nodo()->iStatus > 0 )
                    {
                        if( sSolicitud->sQuienEntrega.codigosMuebles.nodo()->cFlagCodigoEspecial != '1' )
                        {
                            mysnprintf( cLog, sizeof(cLog)-1,"%s", "No se permite la captura de otro código diferente" );
                            //logx( cLog, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);
                            iFlag = 1;
                        }

                        if (!sSolicitud->sQuienEntrega.codigosMuebles.siguiente())
                        {
                            break;
                        }
                    }
                }
            }
        }
    }
    else
    {
        if( iTipoEsp != 0 )
        {
            if( iTipoEsp == 1 )
            {
                sSolicitud->sQuienEntrega.codigosMuebles.ultimo( );
                mysnprintf( cLog, sizeof(cLog)-1, "No se permite la captura de otro código al facturar el código %6ld ", sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo );
            }
            else
            {
                mysnprintf( cLog, sizeof(cLog)-1,"%s", "No se permite la captura de otro código diferente" );
            }
            iFlag = 1;
            //logx( cLog, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);

            sSolicitud->sQuienEntrega.sSysMuebles.iFlagLimpiarCapturaCodigo = 1;
        }

        if ( iFlag == 0 )
        {
            if ( sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo == 999181L || checarCodigoJoyeria( sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo ) ||
                 sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo == 999644L || sSolicitud->sQuienEntrega.codigosMuebles.nodo()->cFlagCodigoEspecial == '1' )
            {
                if ( sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo == 999181L || checarCodigoJoyeria( sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo ) ||
                     sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo == 999644L )
                {
                    if ( sSolicitud->sQuienEntrega.sSysMuebles.iFlag999181 != 1 )
                    {
                        mysnprintf( cLog, sizeof(cLog)-1, "Esta tienda NO está autorizada para utilizar el código %6ld ", sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo );
                        //logx( cLog, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);

                        iFlag = 1;
                        iFlagx = 0;
                        sSolicitud->sQuienEntrega.sSysMuebles.iFlagLimpiarCapturaCodigo = 1;
                    }
                }

                if ( iFlagx == 1 )
                {
                    sSolicitud->sQuienEntrega.codigosMuebles.nodo()->cBodegaQueEntrega = 'N';

                    if( sSolicitud->sQuienEntrega.sSysMuebles.iFlagTiendaZonaLibre == 1 )
                    {
                        if( sSolicitud->sQuienEntrega.codigosMuebles.nodo()->cImportacion == 'M' )
                        {
                            //si articulosM = 'N' flag=1 ,si articulosM = 'I' flag=2
                            if ( sSolicitud->sQuienEntrega.sSysMuebles.iFlagArticulosM == 1 )
                                sSolicitud->sQuienEntrega.codigosMuebles.nodo()->cImportacion = 'N';
                            if ( sSolicitud->sQuienEntrega.sSysMuebles.iFlagArticulosM == 2 )
                                sSolicitud->sQuienEntrega.codigosMuebles.nodo()->cImportacion = 'I';
                        }
                        sSolicitud->sQuienEntrega.codigosMuebles.nodo()->cBodegaQueEntrega = sSolicitud->sQuienEntrega.codigosMuebles.nodo()->cImportacion;
                    }

                    if ( iFlag != 1 )
                    {
                        if( sSolicitud->sQuienEntrega.codigosMuebles.nodo()->cQuienEntrega != 'B' && checarJoyeriaCoppel( sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo, sSolicitud->sQuienEntrega.sSysMuebles.iAnioSys, sSolicitud->sQuienEntrega.sSysMuebles.iMesSys, sSolicitud->sQuienEntrega.sSysMuebles.iDiaSys ) != 1 )
                        {
                            mysnprintf( cLog, sizeof(cLog)-1,"%s", "! Solo se permite venta por B !" );
                            //logx( cLog, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);
                            iFlag = 1;
                            sSolicitud->sQuienEntrega.sSysMuebles.iFlagLimpiarCapturaCodigo = 1;
                        }
                        else
                        {
                            iFlag = 0;

                            if ( iFlagLlamaOptica == 1 )
                            {
                                capturarDescripcionPrecio( sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo, sSolicitud->sQuienEntrega.codigosMuebles.nodo()->cFlagCodigoEspecial, 0, &sSolicitud->sQuienEntrega.sSysMuebles );
                            }
                            else
                            {
                                capturarDescripcionPrecio( sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo, sSolicitud->sQuienEntrega.codigosMuebles.nodo()->cFlagCodigoEspecial, 1, &sSolicitud->sQuienEntrega.sSysMuebles );
                            }
                        }
                    }
                }
            }

            if ( iFlag == 0 )
            {
                if ( sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo == 199982L )  //Activo Fijo
                {
                    iFlag = 1;
                    if ( sSolicitud->sQuienEntrega.sSysMuebles.iFlagVentaActivoFijo == 1 )
                    {
                        iFlag = 0;
                        capturarDescripcionPrecio( sSolicitud->sQuienEntrega.codigosMuebles.nodo()->lCodigo, sSolicitud->sQuienEntrega.codigosMuebles.nodo()->cFlagCodigoEspecial, 1, &sSolicitud->sQuienEntrega.sSysMuebles );
                    }
                } 
            }
        }
    }
    
    return iFlag;
}


void Cwsprestamos00024::capturarDescripcionPrecio( long lCodigo, char cFlagCodigoEspecial, int iFlagLlamaOptica, SysMuebles *sSysMuebles )
{
    sSysMuebles->iFlagLlamarOptica = (short)iFlagLlamaOptica;

    switch ( lCodigo )
    {
        case 199982L:
        case 999181L:
            sSysMuebles->iFlagCapturarDescripcion = CAPTURAR_DESCRIPCION_999181;
            break;
        case 999644L:
            sSysMuebles->iFlagCapturarDescripcion = CAPTURAR_DESCRIPCION_999644;
            break;
        case 199941L:
            sSysMuebles->iFlagCapturarDescripcion = CAPTURAR_DESCRIPCION_199941;
            break;
        default:
            break;
    }

    if ( cFlagCodigoEspecial == '1' )
    {
        sSysMuebles->iFlagCapturarDescripcion = CAPTURAR_DESCRIPCION_999181;
    }
}
