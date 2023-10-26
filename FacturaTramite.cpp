#include "Cwsprestamos00024.h"

void Cwsprestamos00024::separarCodigo(int iRenglon, int iCantidad, bool bSeparaDescManual)
{
    int iRenglonSigCod = 0, iCantidadCods = 0;
    long lSobreprecio =0, lSobreprecio18 = 0, lGanoDE = 0, lGanoPuntDECod = 0, lDescDECod = 0, lPrecioUnitario =0;
    char cTexto[50]={0};
    short iRenglonInsert = 0, iCopearReng = 0;

    InfoCodigoMuebles facturaMovtoCod;  
    
    iRenglonSigCod = iRenglon;  

    do
    { 
        // indentificar sig renglón validando lineas de descuento..   
        iRenglonSigCod++;
        m_gridFactura.leerArticulo( (short)iRenglonSigCod, facturaMovtoCod );
    } 
    while  ( facturaMovtoCod.iTipoDescuento > 0 && facturaMovtoCod.iStatus > 0 );       

    m_gridFactura.leerArticulo( (short)iRenglon, facturaMovtoCod );
    iCantidadCods = facturaMovtoCod.i16Cantidad;

    // separar
    iRenglonInsert = (short)iRenglonSigCod; 

    for ( iCopearReng = (short)iRenglon;  iCopearReng < iRenglonSigCod;   iCopearReng++ )
    {
        m_gridFactura.leerArticulo( iCopearReng, facturaMovtoCod );

        if ( facturaMovtoCod.iTipoDescuento == 0 || bSeparaDescManual == false )
        { // es codigo o es descuento y no es manual(1,5,14).

            m_gridFactura.InsertRow(iRenglonInsert);            

            m_gridFactura.ponerValor( COL_STATUS, iRenglonInsert, facturaMovtoCod.iStatus );
            m_gridFactura.ponerValor( COL_CODIGO, iRenglonInsert, facturaMovtoCod.lCodigo );
            
            lSobreprecio = facturaMovtoCod.lSobrePrecio/iCantidadCods;
            lSobreprecio18 = facturaMovtoCod.lSobrePrecio18/iCantidadCods;
            //lGanoDE = facturaMovtoCod.lGanoDE/iCantidadCods;
            //lGanoPuntDECod = facturaMovtoCod.lGanoPuntDECod/iCantidadCods;
            //lDescDECod = facturaMovtoCod.lDescDECod/iCantidadCods;        
        
            //  actualiza valores
            if (facturaMovtoCod.iTipoDescuento > 0 && bSeparaDescManual == false )
            {
                m_gridFactura.ponerValor( COL_SOBREPRECIO, iCopearReng, lSobreprecio );
                m_gridFactura.ponerValor( COL_SOBREPRECIOPLAZO18, iCopearReng, lSobreprecio18 );

                //lDescDECod = facturaMovtoCod.lDescDECod/iCantidadCods;
                // No cambia siempre es 1 la  COL_CANTIDAD
                lPrecioUnitario = facturaMovtoCod.lPrecioUnitario / iCantidadCods; // COL_PRECIO en descuento no es unitario..
                m_gridFactura.ponerValor( COL_PRECIO, iCopearReng, ( lPrecioUnitario * iCantidad )); 
                m_gridFactura.ponerValor( COL_IMPORTE, iCopearReng, ( lPrecioUnitario * iCantidad ));               
            }
            else if (facturaMovtoCod.iTipoDescuento == 0) // es codigo
            {
                m_gridFactura.ponerValor( COL_SOBREPRECIO, iCopearReng, facturaMovtoCod.lSobrePrecio );
                m_gridFactura.ponerValor( COL_SOBREPRECIOPLAZO18, iCopearReng, facturaMovtoCod.lSobrePrecio18 );

                m_gridFactura.ponerValor( COL_CANTIDAD, iCopearReng, iCantidad );
                m_gridFactura.ponerValor( COL_PRECIO, iCopearReng, facturaMovtoCod.lPrecioUnitario );
                m_gridFactura.ponerValor( COL_IMPORTE, iCopearReng, (facturaMovtoCod.lPrecioUnitario * iCantidad));                 
            }           
            
            //m_gridFactura.ponerValor( COL_GANODE, iCopearReng, (lGanoDE * iCantidad));
            //m_gridFactura.ponerValor( COL_PUNTOSDINEROELECTRONICO, iCopearReng, (lGanoPuntDECod * iCantidad));
            //m_gridFactura.ponerValor( COL_DESC_ELECT, iCopearReng, (lDescDECod * iCantidad));

            //inserta nvos valores
            if (facturaMovtoCod.iTipoDescuento > 0 && bSeparaDescManual == false )
            {
                m_gridFactura.ponerValor( COL_SOBREPRECIO, iRenglonInsert, lSobreprecio );
                m_gridFactura.ponerValor( COL_SOBREPRECIOPLAZO18, iRenglonInsert, lSobreprecio18 );

                m_gridFactura.ponerValor( COL_CANTIDAD, iRenglonInsert, 1 ); // siempre es 1 en desc.
                lPrecioUnitario = facturaMovtoCod.lPrecioUnitario / iCantidadCods; // COL_PRECIO en descuento no es unitario..
                m_gridFactura.ponerValor( COL_PRECIO, iRenglonInsert, (lPrecioUnitario *( iCantidadCods - iCantidad)));// COL_PRECIO en descuento no es unitario..
                m_gridFactura.ponerValor( COL_IMPORTE, iRenglonInsert,(lPrecioUnitario *( iCantidadCods - iCantidad))); 

                m_gridFactura.QuickSetTextColor(COL_CODIGO, iRenglonInsert, RGB(255,255,255));
                m_gridFactura.QuickSetTextColor(COL_CANTIDAD, iRenglonInsert, RGB(255,255,255));
                m_gridFactura.QuickSetTextColor(COL_PRECIO, iRenglonInsert, RGB(255,255,255));
                m_gridFactura.QuickSetTextColor(COL_QUIENENTREGA, iRenglonInsert, RGB(255,255,255));
            }
            else if (facturaMovtoCod.iTipoDescuento == 0) // es codigo
            {
                m_gridFactura.ponerValor( COL_SOBREPRECIO, iRenglonInsert, facturaMovtoCod.lSobrePrecio );
                m_gridFactura.ponerValor( COL_SOBREPRECIOPLAZO18, iRenglonInsert, facturaMovtoCod.lSobrePrecio18 );

                m_gridFactura.ponerValor( COL_CANTIDAD, iRenglonInsert, (iCantidadCods - iCantidad));
                m_gridFactura.ponerValor( COL_PRECIO, iRenglonInsert, facturaMovtoCod.lPrecioUnitario );
                m_gridFactura.ponerValor( COL_IMPORTE, iRenglonInsert, (facturaMovtoCod.lPrecioUnitario *( iCantidadCods - iCantidad)));

                m_gridFactura.QuickSetTextColor(COL_CODIGO, iRenglonInsert, RGB(0,0,0));
                m_gridFactura.QuickSetTextColor(COL_CANTIDAD, iRenglonInsert, RGB(0,0,0));
                m_gridFactura.QuickSetTextColor(COL_PRECIO, iRenglonInsert, RGB(0,0,0));
                m_gridFactura.QuickSetTextColor(COL_QUIENENTREGA, iRenglonInsert, RGB(0,0,0));
            }       
            
            //m_gridFactura.ponerValor( COL_GANODE, iRenglonInsert, (lGanoDE *( iCantidadCods - iCantidad)));
            //m_gridFactura.ponerValor( COL_PUNTOSDINEROELECTRONICO, iRenglonInsert, (lGanoPuntDECod *( iCantidadCods - iCantidad)));
            //m_gridFactura.ponerValor( COL_DESC_ELECT, iRenglonInsert, (lDescDECod *( iCantidadCods - iCantidad)));

            
            //------------------

            //mysnprintf( cTexto, sizeof(cTexto)-1,"%c",facturaMovtoCod.cQuienEntrega);
            mysnprintf( cTexto, sizeof(cTexto)-1,"%s",facturaMovtoCod.cQuienEntrega);
            m_gridFactura.ponerValor( COL_QUIENENTREGA, iRenglonInsert, cTexto );

            mysnprintf(cTexto,10,"%.10s", facturaMovtoCod.cArticulo );
            m_gridFactura.ponerValor( COL_ARTICULO, iRenglonInsert, cTexto );

            mysnprintf(cTexto,10,"%.10s", facturaMovtoCod.cMarca );
            m_gridFactura.ponerValor( COL_MARCA, iRenglonInsert, cTexto );

            mysnprintf(cTexto,20,"%.20s", facturaMovtoCod.cModelo );
            m_gridFactura.ponerValor( COL_MODELO, iRenglonInsert, cTexto );     
            
            mysnprintf( cTexto, sizeof(cTexto)-1,"%c", facturaMovtoCod.cTipo );
            m_gridFactura.ponerValor( COL_TIPO, iRenglonInsert, cTexto );

            mysnprintf( cTexto, sizeof(cTexto)-1,"%c", facturaMovtoCod.cAgotadoInvent );
            m_gridFactura.ponerValor( COL_AGOTADOINVENT, iRenglonInsert, cTexto );
            
            m_gridFactura.ponerValor( COL_EXISTENCIA, iRenglonInsert, facturaMovtoCod.i16Existencia );

            mysnprintf( cTexto, sizeof(cTexto)-1,"%c", facturaMovtoCod.cBodegaQueEntrega );
            m_gridFactura.ponerValor( COL_BODEGAENTREGA, iRenglonInsert, cTexto );

            mysnprintf( cTexto, sizeof(cTexto)-1,"%c", facturaMovtoCod.cSemanaEntrega );
            m_gridFactura.ponerValor( COL_SEMANAENTREGA, iRenglonInsert, cTexto );

            mysnprintf( cTexto, sizeof(cTexto)-1,"%c", facturaMovtoCod.cMesEntrega );
            m_gridFactura.ponerValor( COL_MESENTREGA, iRenglonInsert, cTexto );

            mysnprintf( cTexto, sizeof(cTexto)-1,"%c", facturaMovtoCod.cModoDeEntrega );
            m_gridFactura.ponerValor( COL_MODOENTREGA, iRenglonInsert, cTexto );
            
            mysnprintf( cTexto, sizeof(cTexto)-1,"%c", facturaMovtoCod.cImportacion );
            m_gridFactura.ponerValor( COL_IMPORTACION, iRenglonInsert, cTexto );

            m_gridFactura.ponerValor( COL_STOCK, iRenglonInsert, (long)facturaMovtoCod.i16Stock );
            m_gridFactura.ponerValor( COL_INTERES12,iRenglonInsert, (long)facturaMovtoCod.fInteres12 );
            
            mysnprintf(cTexto,15,"%.15s", facturaMovtoCod.cNumeroSerieUdi );
            m_gridFactura.ponerValor( COL_SERIE, iRenglonInsert, cTexto );

            mysnprintf( cTexto, sizeof(cTexto)-1,"%c", facturaMovtoCod.cCausaUdi );
            m_gridFactura.ponerValor( COL_CAUSA, iRenglonInsert, cTexto);

            mysnprintf(cTexto,44,"%.44s", facturaMovtoCod.cDescripcionDano );
            m_gridFactura.ponerValor( COL_DESCRIPCIONDANO, iRenglonInsert, cTexto );

            mysnprintf( cTexto, sizeof(cTexto)-1,"%c", facturaMovtoCod.cFlagApartado );
            m_gridFactura.ponerValor( COL_FLAGAPARTADO, iRenglonInsert, cTexto);

            mysnprintf( cTexto, sizeof(cTexto)-1,"%c", facturaMovtoCod.cFlagCodigoEspecial );
            m_gridFactura.ponerValor( COL_FLAGCODIGOESPECIAL, iRenglonInsert, cTexto);

            mysnprintf( cTexto, sizeof(cTexto)-1,"%c", facturaMovtoCod.cAgotado );
            m_gridFactura.ponerValor( COL_AGOTADO, iRenglonInsert, cTexto);

            mysnprintf( cTexto, sizeof(cTexto)-1,"%c", facturaMovtoCod.cFlagTipoEntrada );
            m_gridFactura.ponerValor( COL_FLAGTIPOENTRADA, iRenglonInsert, cTexto );

            mysnprintf( cTexto, sizeof(cTexto)-1,"%c", facturaMovtoCod.cFlagPlazo18 );
            m_gridFactura.ponerValor( COL_FLAGPLAZO18, iRenglonInsert, cTexto );

            mysnprintf( cTexto, sizeof(cTexto)-1,"%c", facturaMovtoCod.cCveImpuestoSuntuario );
            m_gridFactura.ponerValor( COL_CVEIMPUESTOSUNTUARIO, iRenglonInsert, cTexto );

            m_gridFactura.ponerValor( COL_FAMILIA, iRenglonInsert, facturaMovtoCod.lFamilia );
            
            mysnprintf( cTexto, sizeof(cTexto)-1,"%c", facturaMovtoCod.cFlagNoArmarArticulo );
            m_gridFactura.ponerValor( COL_FLAGNOARMARARTICULO, iRenglonInsert, cTexto );

            mysnprintf( cTexto, sizeof(cTexto)-1,"%c", facturaMovtoCod.cFlagVtaChip );
            m_gridFactura.ponerValor( COL_FLAGVENTACHIP, iRenglonInsert, cTexto );

            mysnprintf(cTexto,18,"%.18s", facturaMovtoCod.cNumSerieTel );
            m_gridFactura.ponerValor( COL_SERIECELULAR, iRenglonInsert, cTexto );

            m_gridFactura.ponerValor( COL_FLAGVTAMIN, iRenglonInsert, facturaMovtoCod.iFlagVtaMin );
            //m_gridFactura.ponerValor( COL_FLAGSERIEVALIDA, iRenglonInsert, facturaMovtoCod. );
            m_gridFactura.ponerValor( COL_FLAGCODIGOOCULTO, iRenglonInsert, facturaMovtoCod.iFlagCodigoOculto );
            m_gridFactura.ponerValor( COL_CODIGOOCULTO, iRenglonInsert, facturaMovtoCod.lCodigoOculto );
            m_gridFactura.ponerValor( COL_TIPODESCUENTO, iRenglonInsert, facturaMovtoCod.iTipoDescuento );
            
            mysnprintf(cTexto,5,"%.5s", facturaMovtoCod.cFlagentregafuturo );
            m_gridFactura.ponerValor( COL_ENTREGAFUTURO, iRenglonInsert, cTexto );

            mysnprintf(cTexto,5,"%.5s", facturaMovtoCod.cFlagta );
            m_gridFactura.ponerValor( COL_FLAGTA, iRenglonInsert, cTexto );

            m_gridFactura.ponerValor( COL_BODEGAAPARTADO, iRenglonInsert, facturaMovtoCod.iBodega );
            m_gridFactura.ponerValor( COL_GARANTIA, iRenglonInsert, facturaMovtoCod.iGarantia );
            m_gridFactura.ponerValor( COL_INTERES18, iRenglonInsert, (long)facturaMovtoCod.fInteres18 );
            m_gridFactura.ponerValor( COL_TESTIENDA, iRenglonInsert, facturaMovtoCod.lTesTienda );
            m_gridFactura.ponerValor( COL_TESBODEGA, iRenglonInsert, facturaMovtoCod.lTesBodega );
            m_gridFactura.ponerValor( COL_CELULAR, iRenglonInsert, facturaMovtoCod.iCelular );
            m_gridFactura.ponerValor( COL_DESCUENTO_ESPECIAL, iRenglonInsert, facturaMovtoCod.iDescEsp );       
            m_gridFactura.ponerValor( COL_FLAG_ALTO_RIESGO,  iRenglonInsert, facturaMovtoCod.iFlagRiesgo );
            //m_gridFactura.ponerValor( COL_CAMPANAELECTRONICO, iRenglonInsert, facturaMovtoCod.iCampanaDE );

            m_gridFactura.iTotalRenglon++;  
            m_gridFactura.GotoRow(m_gridFactura.iTotalRenglon);

            iRenglonInsert++;
            m_gridFactura.RedrawAll();
        }
    }
}