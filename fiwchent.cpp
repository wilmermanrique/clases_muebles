#include "Cwsprestamos00024.h"
#include "gnfunciones.h"

int Cwsprestamos00024::checarCodigoEntrega( char *cTextoMsj, CListaCodigoMuebles *codigosMuebles, SysMuebles *sSysMuebles,xInfoCliente *xInfoCliente, globalEnviroment *sEnviroment )
{
    int iFlag = 0;

    codigosMuebles->ultimo();

    if ( sSysMuebles->iFlagTiendaZonaLibre != 1 || codigosMuebles->nodo()->cQuienEntrega != 'B' ||
         codigosMuebles->nodo()->cImportacion != 'I' || codigosMuebles->nodo()->cFlagApartado != '1' )
    {
        if ( ( codigosMuebles->nodo()->lCodigo != 999181L && codigosMuebles->nodo()->lCodigo != 199982L &&
               codigosMuebles->nodo()->lCodigo != 999644L ) || codigosMuebles->nodo()->cQuienEntrega != 'B' )
        {
            iFlag = checarDeptoCero( codigosMuebles->nodo()->lCodigo,codigosMuebles->nodo()->cQuienEntrega,cTextoMsj );
            if ( iFlag == 0 ) iFlag = checarEntregasB( codigosMuebles, cTextoMsj );
            if ( iFlag == 0 ) iFlag = checarQuienEntregaFactura ( codigosMuebles, cTextoMsj,xInfoCliente,sEnviroment,sSysMuebles );
            if ( iFlag == 0 ) iFlag = checarTipo5PorB( codigosMuebles,cTextoMsj );
        }
    }
    
    return iFlag;
}

int Cwsprestamos00024::checarDeptoCero( long lCodigo, char cQuienEntrega, char *cTextoMsj )
{
    int iFlag = 0;
    char cTexto[10] = {0};

    if ( lCodigo == 96903L )
    {
        mysnprintf( cTextoMsj,34,"%s", "NO SE PERMITE LA VENTA DE LA JABA" );
        iFlag = 1;
    }
    else
    {
        if ( cQuienEntrega != 'T' )
        {
            mysnprintf( cTexto,10, "%06ld", lCodigo );
            if ( cTexto[0] == '0' && lCodigo > 999L )
            {
                mysnprintf( cTextoMsj,42,"%s", "ARTICULO DEL DEPARTAMENTO CERO SOLO POR T" );
                iFlag = 1;
            }
        }
    }

    return iFlag;
}

int Cwsprestamos00024::checarEntregasB( CListaCodigoMuebles *codigosMuebles, char *cTextoMsj )
{
    int iFlag = 0; 

    if ( codigosMuebles->nodo()->cQuienEntrega == 'B' )
    {
        if ( codigosMuebles->nodo()->cTipo == '2' )
        {
            mysnprintf( cTextoMsj,45,"%s", " !! NO SE VENDE TIPO 2, PRESIONE < Esc > !! " );
            iFlag = 1;
        }
    }

    return iFlag;
}

int Cwsprestamos00024::checarTipo5PorB( CListaCodigoMuebles *codigosMuebles, char *cTextoMsj )
{
    int iFlag = 0, iExistencia=0, j = 0, iSuma = 0, iRen =0 ;

    iRen = codigosMuebles->totalNodos() - 1; // se quita 1 ya que el total 
    codigosMuebles->ultimo();

    if ( codigosMuebles->nodo()->cTipo == '5' &&
         codigosMuebles->nodo()->cModoDeEntrega == 'B' &&
         codigosMuebles->nodo()->cQuienEntrega != 'T' &&
         codigosMuebles->nodo()->cQuienEntrega != 'S' &&
         codigosMuebles->nodo()->i16Stock > 0 )
    {
        j = 0;
         codigosMuebles->primero();
        while( codigosMuebles->nodo()->iStatus > 0 )
        {
            if ( codigosMuebles->nodo()->iStatus == 1 )
            {
                if ( codigosMuebles->nodo()->cQuienEntrega == 'T' || codigosMuebles->nodo()->cQuienEntrega == 'S' )
                {
                    if ( codigosMuebles->nodo()->lCodigo == codigosMuebles->nodo()->lCodigo )
                    {
                        if ( j!= iRen ) iSuma += codigosMuebles->nodo()->i16Cantidad;
                    }
                }
            }
            j++;
            if ( !codigosMuebles->siguiente()){ break; }
        }

        codigosMuebles->ultimo();
        iExistencia = codigosMuebles->nodo()->i16Existencia - (short)iSuma;

        if ( iExistencia > codigosMuebles->nodo()->i16Stock )
        {
            mysnprintf( cTextoMsj,41,"%s", "!! VENDER POR T o S, PRESIONE < Esc > !!" );
            iFlag = 1;
        }
    }

    return iFlag;
}

int Cwsprestamos00024::checarQuienEntregaFactura( CListaCodigoMuebles *codigosMuebles, char *cTextoMsj ,xInfoCliente *xInfoCliente, globalEnviroment *sEnviroment, SysMuebles *sSysMuebles )
{
    int iFlag = 0, j = 0, iExistencia = 0, iSuma = 0, iSumax = 0, iExistenciax = 0,
        iDia = 0, iMes = 0, iAnio = 0, iFlagEnlaceApartado = 0, iFocoAnt = 0, iDiaAntiguedad = 0, iMesAntiguedad = 0, 
        iAnioAntiguedad = 0, iTiempoExhib = 0, iClase = 0, iValorFlag = 0, iRen = 0;
    short i16Stock = 0, iDepto = 0;
    iRen = codigosMuebles->totalNodos() -1 ;
    char cTexto[120] = {0}, cTexto2[80] = {0}, cAgotado = ' ', cSql[256] = {0},cSqlTxt[300],cIpServidorRegional[20] = {0}, cIpRegional[20] = {0}, cIpGlobal[20] = {0}, cLog[512]={0};
    long lCod = 0L,lNumeroEmpleadoHuella = 0L,iExistenciaPendiente=0, iFlagPedirHuella = 1, lFamilia = 0L, lEmpHuella = 0;

    codigosMuebles->ultimo();

    lCod = codigosMuebles->nodo()->lCodigo;
    mysnprintf( cTexto,6,"%06ld",lCod );
    iDepto = cTexto[0]-'0';

    if ( iDepto == 1 || iDepto == 8 || iDepto == 9 || iDepto == 7 || iDepto == 2 )
    {
        iFlagPedirHuella = 0;
    }

    if ( iDepto == 6 )
    {
        if ( codigosMuebles->nodo()->lFamilia / 100 == 601 )
        {
            iFlagPedirHuella = 0;
        }
    }

    iExistenciax = 0;
    iSumax = 0;
    iSuma = 0;
    iDia = 0;

    cAgotado = 'D'; 

    if ( checarAgotadoCodigo( codigosMuebles->nodo()->cFlagApartado, codigosMuebles->nodo()->cAgotadoInvent ) == 1 ) cAgotado = 'A';
    if ( !checarSisApartadoNoMsg( iFlagEnlaceApartado, &odbcTiendaNumero ) )       //gnwchapa.cpp
    {
        mysnprintf( cLog, sizeof(cLog)-1,"checarQuienEntregaFactura [ %s ] -> ERROR AL CHECAR ENLACE CON EL SISTEMA DE APARTADO", "0");
        //logx( cLog, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620); 
    }
    codigosMuebles->primero( );
    while ( codigosMuebles->nodo()->iStatus != 0 && j < iRen )
    {
        if ( codigosMuebles->nodo()->iStatus == 1 )
        {
            if ( codigosMuebles->nodo()->cQuienEntrega == 'T' ||
                 codigosMuebles->nodo()->cQuienEntrega == 'S' )
            {
                if ( lCod == codigosMuebles->nodo()->lCodigo )
                {
                    if ( j != iRen ) iSuma += codigosMuebles->nodo()->i16Cantidad;
                }
            }

            if ( codigosMuebles->nodo()->cQuienEntrega == 'T' ||
                 codigosMuebles->nodo()->cQuienEntrega == 'S' ||
                 codigosMuebles->nodo()->cQuienEntrega == 'B' )
            {
                if ( lCod == codigosMuebles->nodo()->lCodigo )
                {
                    //Se utiliza para sumar arts. modo de entrega T,S o B
                    //para evitar que en Zona Libre se vaya la exis. a rojo
                    if( j != iRen ) iSumax += codigosMuebles->nodo()->i16Cantidad;
                }
            }
        }
        j++;
        if ( !codigosMuebles->siguiente()){ break; }
    }
    codigosMuebles->ultimo();
    iExistencia =  codigosMuebles->nodo()->i16Existencia - iSuma;
    iExistenciax = codigosMuebles->nodo()->i16Existencia - iSumax;

    iClase = validarClase( codigosMuebles->nodo()->lFamilia );
    if ( codigosMuebles->nodo()->cQuienEntrega == 'T' && codigosMuebles->nodo()->i16Cantidad < 0 )
    {
        iExistencia = 100;
    }

    if ( codigosMuebles->nodo()->cQuienEntrega == 'T' || codigosMuebles->nodo()->cQuienEntrega == 'S' )
    {
        if ( iClase != 93 && iClase != 95 )
        {
            if ( cAgotado == 'A' && codigosMuebles->nodo()->i16Cantidad > 0 )
            {
                switch( codigosMuebles->nodo()->cTipo )
                {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '6':
                    case '7':
                    case '8':
                        if ( iExistencia == 0 || iExistencia - codigosMuebles->nodo()->i16Cantidad < 0 )
                        {
                            mysnprintf( cTextoMsj,17,"%s", "NO HAY EN TIENDA" );
                            iFlag = 1;
                        }
                        break;
                    case '4':
                    case '5':
                        if ( iExistencia == 0 || iExistencia - codigosMuebles->nodo()->i16Cantidad < 0 )
                        {
                            mysnprintf( cTextoMsj,17,"%s", "NO HAY EN TIENDA" );
                            iFlag = 1;
                        }
                        else if ( iExistencia == 1 || iExistencia - codigosMuebles->nodo()->i16Cantidad < 1 )
                        {
                            if ( iFlagPedirHuella == 1 )
                            {
                                if ( validarClaseSalasCocinas( codigosMuebles->nodo()->lFamilia ) == 0 )
                                {
                                    iFlag = 0;
                                    /*
                                    if ( validarHuellaGerente( 1, 27 ) != 1 )
                                    {
                                        iFlag = 1;
                                        sprintf( cTextoMsj, "SÓLO EL GERENTE PUEDE AUTORIZAR" );
                                    }
                                    */
                                }
                                else
                                {
                                    if ( validarEntregarSalasCocinas( codigosMuebles->nodo()->lFamilia, codigosMuebles->nodo()->lCodigo, codigosMuebles->nodo()->cTipo,xInfoCliente,sEnviroment,codigosMuebles ) == 0 )
                                    {
                                        iFlag = 1;
                                        mysnprintf( cTextoMsj,34,"%s", "NO SE PERMITE VENTA DE LA MUESTRA" );
                                    }
                                }
                            }
                        }
                        break;
                    default:
                        break;
                }
            }

            if ( cAgotado == 'D' && codigosMuebles->nodo()->i16Cantidad > 0 )
            {
                switch( codigosMuebles->nodo()->cTipo )
                {
                    case '0':
                    case '4':
                    case '5':
                        if ( iExistencia == 0 || iExistencia - codigosMuebles->nodo()->i16Cantidad < 0 )
                        {
                            mysnprintf( cTextoMsj,17,"%s", "NO HAY EN TIENDA" );
                            iFlag = 1;
                        }
                        else if ( iExistencia == 1 || iExistencia - codigosMuebles->nodo()->i16Cantidad < 1 )
                        {
                            if ( iFlagPedirHuella == 1 )
                            {
                                if ( validarClaseSalasCocinas( codigosMuebles->nodo()->lFamilia ) == 0 )
                                {
                                    iFlag = 0;
                                    //pedir huella
                                    /*
                                    if( consultarIpServidor( &odbcTiendaNumero, SERV_HLLASPASSTDA, cIpServidorRegional ) == 1 &&        
                                        consultarIpServidor( &odbcTiendaNumero, SERV_HUELLASEMPLEADOS, cIpRegional ) == 1 &&                            
                                        consultarIpServidor( &odbcTiendaNumero, SERV_CENTRALEMPS, cIpGlobal ) == 1 )
                                    {       
                                        InParametrosHxw inParametrosHxw;
                                        mymemset( &inParametrosHxw, 0, sizeof( InParametrosHxw ) );

                                        inParametrosHxw.iOpcion = 4;            
                                        mymemcpy(inParametrosHxw.cIpLocal,cIpRegional,16);
                                        mymemcpy(inParametrosHxw.cIpRegion,cIpServidorRegional,16);
                                        mymemcpy(inParametrosHxw.cIpCentral,cIpGlobal,16);

                                        lEmpHuella = verificarHuellaHxw( inParametrosHxw );
                                        
                                        checarDerechoOpcion( &conexionLocal,cSqlTxt,"M", VENTA_S_T_ULTIMA , lEmpHuella , iValorFlag );
                                    
                                        if( iValorFlag == 0 )
                                        {
                                            iFlag = 1;
                                            sprintf( cTextoMsj, "SÓLO EMPLEADO CON DERECHOS PUEDE AUTORIZAR" );
                                        }
                                    }
                                    /*if ( validarHuellaGerente( 1, 27 ) != 1 )
                                    {
                                        iFlag = 1;
                                        sprintf( cTextoMsj, "SÓLO EL GERENTE PUEDE AUTORIZAR" );
                                    }
                                    */
                                }
                                else
                                {
                                    if ( validarEntregarSalasCocinas( codigosMuebles->nodo()->lFamilia, codigosMuebles->nodo()->lCodigo, codigosMuebles->nodo()->cTipo,xInfoCliente,sEnviroment,codigosMuebles ) == 0 )
                                    {
                                        iFlag = 1;
                                        mysnprintf( cTextoMsj,34,"%s", "NO SE PERMITE VENTA DE LA MUESTRA" );
                                    }
                                }
                            }
                        }
                        break;
                    case '1':
                    case '2':
                    case '3':
                    case '6':
                    case '7':
                    case '8':
                        if ( iExistencia == 0 || iExistencia - codigosMuebles->nodo()->i16Cantidad < 0 )
                        {
                            mysnprintf( cTextoMsj, 17,"%s","NO HAY EN TIENDA" );
                            iFlag = 1;
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }

    if ( codigosMuebles->nodo()->cQuienEntrega == 'B' && codigosMuebles->nodo()->i16Cantidad > 0 && codigosMuebles->nodo()->lFamilia != 29811 )
    {

        if ( iClase != 93 && iClase != 95 )
        {
            if ( cAgotado == 'A' )
            {
                /*if( facturaMovtoCod.cTipo == '0' || facturaMovtoCod.cTipo == '3' || facturaMovtoCod.cTipo == '6' )
                {
                    generarVentaCaida(&conexionLocal, ventaCaidaMuebles, 16, iFlagVentaCaida, iClienteEspecial);
                    generarDetalleVentaCaida();
                    // llamar desde el front mensaje 20 proceso 45
                }*/
                switch ( codigosMuebles->nodo()->cTipo )
                {
                    case '0':
                        if ( codigosMuebles->nodo()->cFlagApartado != '1' )
                        {
                            mysnprintf( cTextoMsj,17,"%s", "NO HAY EN BODEGA." );
                            iFlag = 1;
                        }
                        break;
                    case '2':
                    case '3':
                        if ( iExistencia == 0 || iExistencia - codigosMuebles->nodo()->i16Cantidad < 0 )
                        {
                            mysnprintf( cTextoMsj,30,"%s", "NO HAY EN BODEGA NI EN TIENDA" );
                        }
                        else
                        {
                            mysnprintf( cTextoMsj,27,"%s", "NO HAY EN BODEGA SOLO EN T" );
                        }

                        iFlag = 1;
                        break;
                    case '1':
                    case '4':
                    case '5':
                        i16Stock = codigosMuebles->nodo()->i16Stock;

                        if ( i16Stock < 0 ) i16Stock = 0;

                        if ( iDepto == 7 || iDepto == 8 || iDepto == 9 ) i16Stock = 0;

                        if ( iDepto == 8 && codigosMuebles->nodo()->cTipo == '5' && codigosMuebles->nodo()->lFamilia >= 81100 && codigosMuebles->nodo()->lFamilia < 81300 )
                        {
                            i16Stock = codigosMuebles->nodo()->i16Stock;
                        }

                        if ( codigosMuebles->nodo()->i16Existencia - iSuma > i16Stock && iFlag != 1 )
                        {
                            if ( sSysMuebles->iFlagTiendaZonaLibre == 1 )
                            {
                                mysnprintf( cTextoMsj,41,"%s", "!! VENDER POR T o S, PRESIONE < Esc > !!" );
                                iFlag = 1;
                            }
                            else
                            {
                                if ( codigosMuebles->nodo()->cTipo != '1' && codigosMuebles->nodo()->cTipo != '5' )
                                {
                                    mysnprintf( cTextoMsj,41,"%s", "!! VENDER POR T o S, PRESIONE < Esc > !!" );
                                    iFlag=1;
                                }
                                else if ( codigosMuebles->nodo()->cTipo == '5' )
                                {
                                    if ( checarSistemaCjs( cAgotado, codigosMuebles->nodo()->cImportacion, codigosMuebles->nodo()->cQuienEntrega, sSysMuebles->iFlagBodegaCjs, cTextoMsj ) == 0 )
                                    {                                                                       
                                        iMes    = sSysMuebles->iMesSys;
                                        iDia    = sSysMuebles->iDiaSys;
                                        iAnio   = sSysMuebles->iAnioSys;
                                        sumarDia( 28,iDia,iMes,iAnio );        //gnwdia.cpp

                                        miniFecha( cTexto,iDia,iMes,iAnio );        //gnwfecha.cpp
                                        cTexto[5]=0;
                                        mysnprintf( cTexto2,80,T4A0B,cTexto );

                                        if ( codigosMuebles->nodo()->cFlagApartado != '1' )
                                        {
                                            mysnprintf( cTextoMsj,80, "%s", cTexto2 );
                                            //generarVentaCaida(&conexionLocal, ventaCaidaMuebles, 16, iFlagVentaCaida, iClienteEspecial);
                                            // mandar llamar el mensaje desde el front
                                            //generarDetalleVentaCaida();
                                        }                                   
                                    }
                                }
                            }
                        }
                        else
                        {
                            //fiwchcjs.cpp
                            if ( checarSistemaCjs( cAgotado, codigosMuebles->nodo()->cImportacion, codigosMuebles->nodo()->cQuienEntrega, sSysMuebles->iFlagBodegaCjs,cTextoMsj ) == 0 )
                            {
                                if ( codigosMuebles->nodo()->lFamilia == 29811 )
                                {
                                    //IMPRESION DE FOTOS.
                                }
                                else
                                {
                                    iMes    = sSysMuebles->iMesSys;
                                    iDia    = sSysMuebles->iDiaSys;
                                    iAnio   = sSysMuebles->iAnioSys;
                                    sumarDia( 28,iDia,iMes,iAnio );        //gnwdia.cpp

                                    miniFecha( cTexto,iDia,iMes,iAnio );        //gnwfecha.cpp
                                    cTexto[5]=0;
                                    mysnprintf( cTexto2,80,T4A0B,cTexto );

                                    if ( codigosMuebles->nodo()->cFlagApartado != '1' )
                                    {
                                        mysnprintf( cTextoMsj,80, "%s", cTexto2 );
                                        //logx( cTextoMsj, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);
                                        //AfxMessageBox( cTextoMsj , _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);
                                    }
                                }
                            }
                            else
                            {
                                iFlag = 1;
                            }
                        }

                        if ( iFlag != 1 && sSysMuebles->iFlagTiendaZonaLibre == 1 &&
                            codigosMuebles->nodo()->cImportacion == 'I' && iFlagEnlaceApartado == 0 )
                        {
                            mysnprintf( cTextoMsj,61,"%s", " NO SE PERMITE VENTA POR B DE IMPORTACION, PRESIONE < Esc > " );
                            iFlag = 1;
                        }
                        break;
                    case '6':
                        mysnprintf( cTextoMsj,17,"%s", "NO HAY EN BODEGA " );
                        iFlag = 1;
                        break;
                    default:
                        break;
                }
            }

            if ( cAgotado == 'D' )
            {
                /*if ( facturaMovtoCod.cTipo == '3' || facturaMovtoCod.cTipo == '6' )
                {
                    generarVentaCaida(&conexionLocal, ventaCaidaMuebles, 16, iFlagVentaCaida, iClienteEspecial);
                    generarDetalleVentaCaida();
                    // llamar desde el front mensaje 20 proceso 45
                }*/

                switch ( codigosMuebles->nodo()->cTipo )
                {
                    case '0':
                        if ( codigosMuebles->nodo()->cModoDeEntrega == 'T' || codigosMuebles->nodo()->cModoDeEntrega == 'M' )
                        {
                            if ( iExistencia == 0 || iExistencia - codigosMuebles->nodo()->i16Cantidad < 0 )
                            {
                            }
                            else
                            {
                                mysnprintf( cTextoMsj,41,"%s", "!! VENDER POR T o S, PRESIONE < Esc > !!" );
                                iFlag=1;
                            }
                        }
                        break;
                    case '3':
                        if ( iExistencia == 0 || iExistencia - codigosMuebles->nodo()->i16Cantidad < 0 )
                        {
                            mysnprintf( cTextoMsj,45,"%s", "DESCONTINUADO.NO SE PUEDE ENTREGAR DE BODEGA" );
                        }
                        else
                        {
                            mysnprintf( cTextoMsj,52,"%s", "DESCONTINUADO.NO SE PUEDE ENTREGAR DE BODEGA SOLO T" );
                        }

                        iFlag = 1;
                        break;
                    case '6':
                        mysnprintf( cTextoMsj,17,"%s", "NO HAY EN BODEGA" );
                        iFlag = 1;
                        break;
                    case '4':
                    case '5':
                        if ( iFlag != 1 && sSysMuebles->iFlagTiendaZonaLibre == 1 &&
                            codigosMuebles->nodo()->cImportacion == 'I' && iFlagEnlaceApartado == 0 )
                        {
                            mysnprintf( cTextoMsj,59,"%s", "NO SE PERMITE VENTA POR B DE IMPORTACION, PRESIONE < Esc >" );
                            iFlag = 1;
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        else
        {
            mysnprintf( cTextoMsj,46,"%s", "ESTE ARTICULO NO PUEDE VENDERSE POR BODEGA" );
            iFlag = 1;
        }
    }

    if ( !( codigosMuebles->nodo()->cQuienEntrega == 'B' || codigosMuebles->nodo()->cQuienEntrega == 'T' ||
            codigosMuebles->nodo()->cQuienEntrega == 'C' || codigosMuebles->nodo()->cQuienEntrega == 'S' ) )
    {
        codigosMuebles->nodo()->cQuienEntrega = ' ';
        mysnprintf( cTextoMsj,23,"%s", " ENTREGA NO VÁLIDA !!" );
        iFlag = 1;
    }

    if ( codigosMuebles->nodo()->cQuienEntrega == 'B' && codigosMuebles->nodo()->i16Cantidad > 0 && cAgotado == 'A' && 
         iExistencia < 2 && codigosMuebles->nodo()->cImportacion == 'I' && sSysMuebles->iFlagTiendaZonaLibre == 1 &&
         iFlag == 0 && codigosMuebles->nodo()->cFlagApartado != '1' )
    {
        if ( codigosMuebles->nodo()->cTipo != '2' && codigosMuebles->nodo()->cTipo != '3' )
        {
            mysnprintf( cTextoMsj,36,"%s", "NO HAY EN BODEGA NI EN DISTRIBUCION" );
        }
    }

    i16Stock = codigosMuebles->nodo()->i16Stock;
    if ( i16Stock < 0 ) i16Stock = 0;

    if ( codigosMuebles->nodo()->cQuienEntrega == 'B' && codigosMuebles->nodo()->i16Existencia - iSuma > 0 && iFlag == 0 )
    {
        //iFocoAnt = iFoco;
        if ( !checarAntiguedadCodigo( codigosMuebles->nodo()->lCodigo, &odbcTiendaNumero, cSqlTxt, iDiaAntiguedad, iMesAntiguedad, iAnioAntiguedad ) )
        {
            mysnprintf(cLog,sizeof(cLog)-1,"CDlgCapturarFactura : checarQuienEntregaFactura [%.100s] ERROR AL CONSULTAR ANTIGÜEDAD DE CÓDIGO", cSqlTxt );
            //logx( cLog, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);
        }

        if ( iAnioAntiguedad == 1900 )
        {
            CRegistro consultarFechaMuestra( &odbcTiendaNumero );

            consultarFechaMuestra.agregarCampoEntero("codigo",DT_ENTERO);
            consultarFechaMuestra.agregarCampoEntero("dcf",DT_ENTERO);
            consultarFechaMuestra.agregarCampoFecha("fechamuestra");

            mysnprintf( cSql,sizeof(cSql)-1, "SELECT dcf, fechamuestra FROM MuInvent WHERE codigo = %ld AND fechamuestra <> '1900-01-01'", codigosMuebles->nodo()->lCodigo );

            if ( !consultarFechaMuestra.Exec( cSql ) )
            {
                mysnprintf(cTextoMsj,27,"%s", "Error al Consultar Muestra");
                iFlag = 1;
            }
            else
            {
                consultarFechaMuestra.activarCols();
                if ( consultarFechaMuestra.Leer() )
                {
                    tiempoExhibicionUdi( consultarFechaMuestra.valorLong("dcf"), iTiempoExhib );

                    SqlTimeStamp *tFecha = consultarFechaMuestra.valorFecha("fechamuestra");

                    if ( sSysMuebles->iAnioSys > tFecha->ano() )
                    {
                        if ( iTiempoExhib > 0 )
                        {
                            if ( meses_transcurridos( 1, tFecha->mes(), tFecha->ano(), sSysMuebles->iDiaSys, sSysMuebles->iMesSys, sSysMuebles->iAnioSys ) > iTiempoExhib )
                            {
                                if ( iDia == 0 )
                                {
                                    mysnprintf( cTexto,120,"VENDER POR T o S, ARTICULO TIENE MAS DE %d MESES EN TDA, AUTORIZA GERENTE?",
                                    meses_transcurridos( tFecha->dia(), tFecha->mes(), tFecha->ano(), sSysMuebles->iDiaSys, sSysMuebles->iMesSys, sSysMuebles->iAnioSys )-1);                                   
                                    /*
                                    Aqui tenemos que preguntar y validar huella de gerente 
                                    **Definir status para esto
                                    if ( AfxMessageBox( cTexto, MB_ICONQUESTION | MB_YESNO ) == IDYES )                                 
                                    {
                                        if ( validarHuellaGerente( 1, 28 ) == 1 )
                                        {
                                            iFlag = 0;
                                        }
                                        else
                                        {
                                            iFlag = 1;
                                            sprintf( cTextoMsj, "SÓLO EL GERENTE PUEDE AUTORIZAR VENDER POR T o S." );
                                        }
                                    }
                                    else
                                    {
                                        iFlag = 1;
                                    }
                                    */
                                }
                            }
                        }
                        else
                        {
                            if ( meses_transcurridos( 1, tFecha->mes(), tFecha->ano(), sSysMuebles->iDiaSys, sSysMuebles->iMesSys, sSysMuebles->iAnioSys ) > 3 )
                            {
                                if ( iDia == 0 )
                                {
                                    //sprintf( cTexto,"VENDER POR T o S, ARTICULO TIENE MAS DE 3 MESES EN TDA, AUTORIZA GERENTE?" );
                                    mysnprintf( cTexto,55,"%s","VENDER POR T o S, ARTICULO TIENE MAS DE 3 MESES EN TDA" );

                                    /*if ( AfxMessageBox( cTexto, MB_ICONQUESTION | MB_YESNO ) == IDYES )
                                    {
                                        if ( validarHuellaGerente( 1, 28 ) == 1 )
                                        {
                                            iFlag = 0;
                                        }
                                        else
                                        {
                                            iFlag = 1;
                                            sprintf( cTextoMsj, "SÓLO EL GERENTE PUEDE AUTORIZAR VENDER POR T o S." );
                                        }
                                    }
                                    else
                                    {
                                        iFlag = 1;
                                        sprintf( cTextoMsj, "SÓLO EL GERENTE PUEDE AUTORIZAR VENDER POR T o S." );
                                    }*/
                                }
                            }
                        }
                    }
                    else
                    {
                        if ( iTiempoExhib > 0 )
                        {
                            if ( sSysMuebles->iMesSys-tFecha->mes() > iTiempoExhib )
                            {
                                if ( iDia == 0 )
                                {
                                    if ( codigosMuebles->nodo()->i16Existencia-iSuma==1 )
                                    {
                                        /*
                                        **Pendiente 
                                        if ( validarHuellaGerente( 1, 29 ) == 1 )
                                        {
                                            iFlag = 0;
                                        }
                                        else
                                        {
                                            iFlag = 1;
                                            sprintf( cTextoMsj, "SÓLO EL GERENTE PUEDE AUTORIZAR VENDER POR T o S." );
                                        }
                                        */
                                    }
                                    else
                                    {
                                        mysnprintf( cTextoMsj,41,"%s", "!! VENDER POR T o S, PRESIONE < Esc > !!" );
                                        iFlag = 1;
                                    }
                                }
                            }
                        }
                        else
                        {
                            if ( sSysMuebles->iMesSys- tFecha->mes() > 3 )
                            {
                                if ( iDia==0 )
                                {
                                    if ( codigosMuebles->nodo()->i16Existencia-iSuma==1 )
                                    {
                                        /*
                                        **Pendiente
                                        if ( validarHuellaGerente( 1, 29 ) == 1 )
                                        {
                                            iFlag = 0;
                                        }
                                        else
                                        {
                                            iFlag = 1;
                                            sprintf( cTextoMsj, "!! VENDER POR T o S, PRESIONE < Esc > !!" );
                                        }
                                        */
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            if ( iDia == 0 )
            {
                if ( iAnioAntiguedad > 1900 )
                {
                    mysnprintf( cTexto,119,"VENDER POR T o S, ARTICULO TIENE MAS DE %d MESES EN TDA, AUTORIZA GERENTE?",
                                    meses_transcurridos( iDiaAntiguedad, iMesAntiguedad, iAnioAntiguedad, sSysMuebles->iDiaSys, sSysMuebles->iMesSys, sSysMuebles->iAnioSys )-1);
                    
                    /*
                    **Pendiente 
                    if ( AfxMessageBox( cTexto, MB_ICONQUESTION | MB_YESNO ) == IDYES )
                    {
                        if ( validarHuellaGerente( 1, 28 ) == 1 )
                        {
                            iFlag = 0;
                        }
                        else
                        {
                            iFlag = 1;
                            sprintf( cTextoMsj, "SÓLO EL GERENTE PUEDE AUTORIZAR VENDER POR T o S." );
                        }
                    }
                    else
                    {
                        iFlag = 1;
                    }
                    */
                }
            }
        }

        //iFoco = iFocoAnt;
        i16Stock = codigosMuebles->nodo()->i16Stock;

        if ( iDepto == 7 || iDepto == 8 || iDepto == 9 ) i16Stock = 0;

        if (  iDepto == 8 && codigosMuebles->nodo()->cTipo == '5' && codigosMuebles->nodo()->lFamilia >= 81100 && codigosMuebles->nodo()->lFamilia < 81300 )
        {
            i16Stock = codigosMuebles->nodo()->i16Stock;
        }

        if ( codigosMuebles->nodo()->i16Existencia - iSuma > i16Stock && iFlag != 1 )
        {
            if ( sSysMuebles->iFlagTiendaZonaLibre == 1 )
            {
                if ( iDia == 0 )  //Se checa si es vta. futura
                {
                    mysnprintf( cTextoMsj,41,"%s", "!! VENDER POR T o S, PRESIONE < Esc > !!" );
                    iFlag = 1;
                }
            }
            else
            {
                if ( codigosMuebles->nodo()->cTipo != '1' && codigosMuebles->nodo()->cTipo != '5' )
                {
                    if ( iDia == 0 )
                    {
                        if ( codigosMuebles->nodo()->lFamilia != 22810 )
                        {
                            mysnprintf( cTextoMsj,41,"%s", "!! VENDER POR T o S, PRESIONE < Esc > !!" );
                            iFlag=1;
                        }
                    }
                }
            }
        }//664
    }

    if ( iFlag != 1 && sSysMuebles->iFlagTiendaZonaLibre == 1 )
    {
        if ( ( iExistenciax - codigosMuebles->nodo()->i16Cantidad ) < 0 && codigosMuebles->nodo()->cImportacion == 'I' )
        {
            mysnprintf( cTextoMsj,46,"%s", "!! LA EXISTENCIA NO PUEDE SER MENOR A CERO !!" );
            iFlag = 1;
        }
    }

    return iFlag;
}


int Cwsprestamos00024::validarClase( long lFamilia )
{
    int iClase = 0;
    char cTexto[6]={0}, cClase[6]={0};

    mysnprintf( cTexto,6,"%05d",lFamilia );

    mymemcpy( &cClase,&cTexto[1],2 );
    cClase[3]=0;

    iClase = intConverter( cClase );

    return iClase;
}


int Cwsprestamos00024::validarClaseSalasCocinas( long lFamilia )
{
    int iFlag = 0, iClase = 0;
    char cTexto[6]={0}, cClase[6]={0};

    mysnprintf( cTexto,6,"%05d",lFamilia );

    mymemcpy( &cClase,&cTexto[0],3 );
    cClase[3]=0;

    iClase = intConverter( cClase );

    return iFlag;
}


int Cwsprestamos00024::validarEntregarSalasCocinas( long lFamilia, long lCodigo, char cTipo,xInfoCliente *xInfoCliente, globalEnviroment *sEnviroment, CListaCodigoMuebles *codigosMuebles)
{
    int iFlag = 0;
    char cSql[1024]={0};
    bool bRespuesta = true;
    OfertaUnica ofertaUnica;

    if ( cTipo == '2' || cTipo == '6' || cTipo == '0' )
    {
        iFlag = 1;
    }
    else
    {
        if ( buscarOfertaUnica(lCodigo, ofertaUnica, xInfoCliente->lCliente, sEnviroment->iTipoComprador, codigosMuebles->nodo()->lPrecioUnitario, bRespuesta ) == 1  )
        {
            if ( ofertaUnica.cFlagMovto!= '1' )      //No tiene Movto
            {
                if ( ofertaUnica.cFlagEtiqueta=='1' )      //Ya se imprimi¢ etiqueta
                {
                    if ( sEnviroment->iTipoComprador!=EMPLEADO || sEnviroment->iPorcientoDescuentoEmpleado < ofertaUnica.iPorcentaje )
                    {
                        iFlag = 1;
                    }
                }
            }
        }
    }

    return iFlag;
}
