/* ==========================================================================
 * Fecha    : 18/07/05 ( dd/mm/aa )  Hora: 4:25:p.m.
 * Elaboró  : Ana Beatríz Bastidas Félix
 * Solicita : Lic. Mendoza ( Jesus Villegas )
 * Actividad: Versión 2005.
 * ==========================================================================
 * Fecha    : 15/09/05 ( dd/mm/aa )  Hora: 8:40:a.m.
 * Elaboró  : Ivan Rivera Montoya
 * Solicita : Lic. Mendoza()
 * Actividad: Se realizo la funcion mensajeActOfertaD100
 * ==========================================================================*/

#include "Cwsprestamos00024.h"

int Cwsprestamos00024::buscarOfertaUnica( long lCodigo, OfertaUnica &ofertaUnicax, long lCliente, int iComprador, long lPrecioUnitario, bool &bContinuar )
{

	int iFlag = 0;
	char cTxt[80] = {0}, cSqlTxt[512]={0}, cSql[512]={0}, cLog[512]={0};
	long lUltimoPrecio = 0L, lPrecioOfertaUnica = 0L;

    CRegistro ofertaUnicaSQL( &odbcTiendaNumero );

    ofertaUnicaSQL.agregarCampoEntero("tienda",DT_ENTERO_CORTO);
    ofertaUnicaSQL.agregarCampoEntero("codigo",DT_ENTERO);
    ofertaUnicaSQL.agregarCampoEntero("importe",DT_ENTERO);
    ofertaUnicaSQL.agregarCampoEntero("flagmovto",DT_ENTERO_CORTO);
    ofertaUnicaSQL.agregarCampoEntero("flagetiqueta",DT_ENTERO_CORTO);
    ofertaUnicaSQL.agregarCampoEntero("porcentajedescuento",DT_ENTERO_CORTO);
    ofertaUnicaSQL.agregarCampoEntero("montodescuento",DT_ENTERO_CORTO);
    ofertaUnicaSQL.agregarCampoFecha("fechainicio");
    ofertaUnicaSQL.agregarCampoFecha("fechaterminacion");
    ofertaUnicaSQL.agregarCampoFecha("fechaantiguedad");


    {
        mysnprintf( cSqlTxt, sizeof(cSqlTxt)-1,"%s","SELECT tienda,codigo,importe,flagmovto,flagetiqueta,iPorcentaje, "
            "fechainicio,fechaterminacion,fechaantiguedad "
            "FROM muBuscarOfertaUnica('%ld'); ",lCodigo);

        if (!ofertaUnicaSQL.Exec(cSqlTxt))
        {
            mysnprintf( cLog, sizeof(cSql)-1, "Error en el mensaje 24:[%.150s]", ofertaUnicaSQL.error() );
            //logx( cLog, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);
        }
        else
        {
            ofertaUnicaSQL.activarCols();

            while( ofertaUnicaSQL.Leer() )
            {
                ofertaUnicax.iTienda = ofertaUnicaSQL.valorShort("tienda");
                ofertaUnicax.lCodigo = ofertaUnicaSQL.valorLong("codigo");
                ofertaUnicax.lImporte = ofertaUnicaSQL.valorLong("importe");


                mysnprintf(&ofertaUnicax.cFlagMovto,2,"%d",ofertaUnicaSQL.valorShort("flagmovto"));
                mysnprintf(&ofertaUnicax.cFlagEtiqueta,2,"%d",ofertaUnicaSQL.valorShort("flagetiqueta"));

                ofertaUnicax.iPorcentaje = ofertaUnicaSQL.valorShort("iPorcentaje");

                SqlTimeStamp *tempFecha = ofertaUnicaSQL.valorFecha("fechainicio");
                char cTxt[10] = {0};

                mysnprintf(cTxt,10,"%04d%02d%02d",tempFecha->ano(),tempFecha->mes(),tempFecha->dia());
                ofertaUnicax.lFechaInicio = valor_campo(cTxt,8);
                
                tempFecha = ofertaUnicaSQL.valorFecha("fechaterminacion");
                mysnprintf(cTxt,1,"%s","");

                mysnprintf(cTxt,10,"%04d%02d%02d",tempFecha->ano(),tempFecha->mes(),tempFecha->dia());
                ofertaUnicax.lFechaTerminacion = valor_campo(cTxt,8);

                tempFecha = ofertaUnicaSQL.valorFecha("fechaantiguedad");
                mysnprintf(cTxt,1,"%s","");

                mysnprintf(cTxt,10,"%04d%02d%02d",tempFecha->ano(),tempFecha->mes(),tempFecha->dia());
                ofertaUnicax.lFechaAntiguedad = valor_campo(cTxt,8);

                iFlag = 1;

            }
        }
    }

    if( iFlag == 1 && lCliente > 0L )
    {
        //Se checa si el art. tiene descto de promoción
        {
            mysnprintf( cSqlTxt, sizeof(cSqlTxt)-1,"SELECT muobtenerpreciodescm('%ld','%ld','%d'); ",lCodigo,lCliente,iComprador);

            CRegistro precioDescuentoSQL(&odbcTiendaNumero);

            precioDescuentoSQL.agregarCampoEntero("muobtenerpreciodescm",DT_ENTERO);

            if (!precioDescuentoSQL.Exec(cSqlTxt))
            {
                mysnprintf(cLog,sizeof(cSql)-1," ERROR AL CONSULTAR muobtenerpreciodescm : [%s]",precioDescuentoSQL.error());
                //logx( cLog, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);

                bContinuar = false;
            }
            else
            {
                precioDescuentoSQL.activarCols();
                while( precioDescuentoSQL.Leer() )
                {
                    lUltimoPrecio = precioDescuentoSQL.valorLong("muobtenerpreciodescm");
                }
            }
        }

        if ( lUltimoPrecio > 0L )
        {
            lPrecioOfertaUnica =  obtenerPrecioOftaUnicaConDesc(lCodigo) ;
            if ( (lUltimoPrecio < ( lPrecioOfertaUnica )) ||  lPrecioOfertaUnica <1)
            {
                iFlag = 0;
            }
            else
            {
                ofertaUnicax.lImporte = lPrecioOfertaUnica;
            }
        }
    }

    return( iFlag ) ;
}

long Cwsprestamos00024::obtenerPrecioOftaUnicaConDesc(long lCodigo)
{
    char cSqlTxt[512] = {0};    
    long lPrecioRegresa = 0L;

    CRegistro consultarOfertaUnica(&odbcTiendaNumero);

    //juan carlos me dijo que lo modificara
    mysnprintf( cSqlTxt, sizeof(cSqlTxt)-1,"%s","SELECT a.porcentajedescuento,b.precio "
            "FROM muOfertaUnica AS a JOIN muinvent AS b ON a.codigo=b.codigo "
            "WHERE a.codigo = %ld AND a.tipo = 0 AND b.existencia = 1",lCodigo);

    consultarOfertaUnica.agregarCampoEntero("porcentajedescuento",DT_ENTERO_CORTO);
    consultarOfertaUnica.agregarCampoEntero("precio",DT_ENTERO);

    if( consultarOfertaUnica.Exec(cSqlTxt) )
    {
        consultarOfertaUnica.activarCols();
        if(consultarOfertaUnica.leer())
        {
            short porcentajedescuento = consultarOfertaUnica.valorShort("porcentajedescuento");
            int  precio = consultarOfertaUnica.valorShort("precio");

            lPrecioRegresa = precio - ((precio * porcentajedescuento)/100L); 
        }
    }

    return lPrecioRegresa;
}

int Cwsprestamos00024::disponibleParaOferta( sSolicitud24_QuienEntrega *sQuienEntrega ,long lCodigo )
{
    int iSuma = 0;

    if ( sQuienEntrega->codigosMuebles.totalNodos() > 0 )
    {
        sQuienEntrega->codigosMuebles.primero();

        do
        {
            if( sQuienEntrega->codigosMuebles.nodo()->iStatus != 0 )
            {
                if ( sQuienEntrega->codigosMuebles.nodo()->iStatus == 1 || sQuienEntrega->codigosMuebles.nodo()->iStatus == 4 )
                {
                    if ( sQuienEntrega->codigosMuebles.nodo()->cQuienEntrega == 'T' || sQuienEntrega->codigosMuebles.nodo()->cQuienEntrega == 'S' )
                    {
                        if ( lCodigo == sQuienEntrega->codigosMuebles.nodo()->lCodigo )
                        {
                            iSuma += sQuienEntrega->codigosMuebles.nodo()->i16Cantidad;
                        }
                    }
                }
            }
        }while( sQuienEntrega->codigosMuebles.siguiente() );
        
    }

    return iSuma;
}
