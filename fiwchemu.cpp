#include "Cwsprestamos00024.h"
int Cwsprestamos00024::checarEntregarMuestra( SysMuebles *sysMuebles, CListaCodigoMuebles *codigosMuebles )
{
    char cTexto[10] = {0}, cLog[128]={0};
    int iFlag = 0, iTotalUnidades = 0, iDepto = 0, iClase = 0;
    long lCodigo = 0L;

    codigosMuebles->ultimo(); // Este es el codigo que se esta validando
    if ( sysMuebles->iFlagCondicionMuestra == 1 )
    {
        lCodigo = codigosMuebles->nodo()->lCodigo;

        mysnprintf( cLog, sizeof(cLog)-1,"Codigo [ %ld ], QuienEntrega [ %c ], stock [%d] ",lCodigo,codigosMuebles->nodo()->cQuienEntrega,codigosMuebles->nodo()->i16Stock);
        //logx( cLog, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);

        if ( ( codigosMuebles->nodo()->cQuienEntrega != 'T' &&
                codigosMuebles->nodo()->cQuienEntrega != 'S' ) ||
                codigosMuebles->nodo()->i16Stock > 0 )
        {
            mysnprintf( cTexto, sizeof(cTexto)-1,"%06ld",codigosMuebles->nodo()->lCodigo );
            iDepto = cTexto[0]-'0';

            mysnprintf( cLog, sizeof(cLog)-1,"iDepto [ %d ]",iDepto);
            //logx( cLog, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);

            mysnprintf( cTexto, sizeof(cTexto)-1,"%04d",dcf((short)codigosMuebles->nodo()->lFamilia) );   //fiwvadcf.cpp
            iClase = (int)valor_campo( &cTexto[0],2 );

            mysnprintf( cLog, sizeof(cLog)-1,"iClase [ %d ]",iClase);
            //logx( cLog, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);

            if ( codigosMuebles->nodo()->i16Existencia > 0 &&
                    venderMuestraTS( iDepto, codigosMuebles, sysMuebles ) != 1 &&
                    checarMovtoVR( codigosMuebles ) != 1 )
            {
                if ( iDepto !=6 || iClase != 1 )
                {
                    codigosMuebles->primero();
                    do
                    {
                        if( codigosMuebles->nodo()->iStatus > 0 )
                        {
                            if( (codigosMuebles->nodo()->iStatus == 1 || codigosMuebles->nodo()->iStatus == 4) && 
                                codigosMuebles->nodo()->lCodigo == lCodigo ||
                                (codigosMuebles->nodo()->cQuienEntrega == 'T' || codigosMuebles->nodo()->cQuienEntrega == 'S') )
                            {
                                iTotalUnidades += codigosMuebles->nodo()->i16Cantidad;
                            }
                        }
                    }while( codigosMuebles->siguiente() && codigosMuebles->nodo()->iStatus > 0  );

                    if ( (  codigosMuebles->nodo()->cQuienEntrega == 'T' ||
                            codigosMuebles->nodo()->cQuienEntrega == 'S' ) &&
                            codigosMuebles->nodo()->cTipo == '5' &&
                            codigosMuebles->nodo()->cModoDeEntrega == 'B' )
                    { 
                        if ( sysMuebles->iFlagTiendaZonaLibre != 1 )
                        {
                            if ( codigosMuebles->nodo()->i16Existencia - iTotalUnidades <= 0 )
                            {
                                    iFlag = 1;
                            }
                        }
                    }
                }
            }
        }
    }

    mysnprintf( cLog, sizeof(cLog)-1,"RESULTADO checarEntregarMuestra [ %d ]", iFlag);
    //logx( cLog, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);

    return iFlag;
}

int Cwsprestamos00024::venderMuestraTS( int iDepto, CListaCodigoMuebles *codigosMuebles,SysMuebles *sysMuebles )
{
    char cSql[128] = {0}, cLog[526]={0};
    int iSuma = 0, iFlag = 0, iTiempoExhib = 0;

    if ( codigosMuebles->nodo()->cQuienEntrega == 'B' &&
         checarAgotadoCodigo( codigosMuebles->nodo()->cFlagApartado, codigosMuebles->nodo()->cAgotadoInvent ) == 1 &&
         ( iDepto == 4 || iDepto == 5 ) )
    {
        //Vta a Futuro
    }
    else
    {
        iSuma = obtenerSumaCapt(codigosMuebles);
        codigosMuebles->ultimo();
        if ( ( codigosMuebles->nodo()->cQuienEntrega == 'T' ||
               codigosMuebles->nodo()->cQuienEntrega == 'S' ) &&
               (codigosMuebles->nodo()->i16Existencia - iSuma) > 0 )
        {

            CRegistro consultarFechaMuestra( &odbcTiendaNumero );

            mysnprintf( cSql, sizeof(cSql)-1, "SELECT codigo, dcf, fechamuestra FROM MuInvent WHERE codigo = %ld AND fechamuestra <> '1900-01-01'",codigosMuebles->nodo()->lCodigo );

            consultarFechaMuestra.agregarCampoEntero("codigo",DT_ENTERO);
            consultarFechaMuestra.agregarCampoEntero("dcf",DT_ENTERO);
            consultarFechaMuestra.agregarCampoFecha("fechamuestra");

            if ( !consultarFechaMuestra.Exec( cSql ) )
            {
                
            }
            else
            {
                consultarFechaMuestra.activarCols();
                if ( consultarFechaMuestra.Leer() )
                {
                    tiempoExhibicionUdi( codigosMuebles->nodo()->lFamilia, iTiempoExhib );

                    SqlTimeStamp *tempFecha = consultarFechaMuestra.valorFecha("fechamuestra");

                    if ( sysMuebles->iAnioSys > tempFecha->ano() )
                    {
                        if ( iTiempoExhib > 0 )
                        {
                            if ( meses_transcurridos( 1, tempFecha->mes(), tempFecha->ano(), sysMuebles->iDiaSys, sysMuebles->iMesSys, sysMuebles->iAnioSys ) > iTiempoExhib )
                            {
                                iFlag = 1;
                            }
                        }
                        else
                        {
                            if ( meses_transcurridos( 1, tempFecha->mes(), tempFecha->ano(), sysMuebles->iDiaSys, sysMuebles->iMesSys, sysMuebles->iAnioSys ) > 3 )
                            {
                                iFlag = 1;
                            }
                        }
                    }
                    else
                    {
                        if ( iTiempoExhib > 0 )
                        {
                            if ( (sysMuebles->iMesSys - tempFecha->mes() ) > iTiempoExhib ) iFlag = 1;
                        }
                        else
                        {
                            if ( (sysMuebles->iMesSys - tempFecha->mes() ) > 3 ) iFlag = 1;
                        }
                    }
                }
            }
        }
    }

    mysnprintf( cLog, sizeof(cLog)-1,"RESULTADO venderMuestraTS [ %d ]",iFlag);
    //logx( cLog, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);
    return iFlag;
}

int Cwsprestamos00024::obtenerSumaCapt( CListaCodigoMuebles *codigosMuebles )
{
    int iSuma = 0, j=0;
    long lCodigo = 0L;

    lCodigo = codigosMuebles->nodo()->lCodigo;
    
    codigosMuebles->primero();
    
    do
    {
        if( codigosMuebles->nodo()->iStatus == 1 )
        {
            if( codigosMuebles->nodo()->cQuienEntrega == 'T' || codigosMuebles->nodo()->cQuienEntrega == 'S' )
            {
                if( lCodigo == codigosMuebles->nodo()->lCodigo )
                {
                   if( j != codigosMuebles->totalNodos()) iSuma += codigosMuebles->nodo()->i16Cantidad;
                }
            }
        }
        j++;
    }while (codigosMuebles->siguiente() && codigosMuebles->nodo()->iStatus != 0);

    return iSuma;
}

int Cwsprestamos00024::checarMovtoVR( CListaCodigoMuebles *codigosMuebles )
{
    int iFlag = 0, iSuma = 0;
    char cSql[64] = {0}, cLog[256] = {0};

    mysnprintf( cSql, sizeof(cSql)-1, "SELECT muChecarMovtoVR('%ld');", codigosMuebles->nodo()->lCodigo );

    CRegistro movtoVRSQL( &odbcTiendaNumero );
    movtoVRSQL.agregarCampoEntero("muchecarmovtovr",DT_ENTERO_CORTO);

    if ( !movtoVRSQL.Exec( cSql ) )
    {
        mysnprintf( cLog, sizeof(cLog)-1, " ERROR EN LA FUNCION CHECARMOVTOVR SQL->[ %.200s ]", movtoVRSQL.error() );
        //logx( cLog  , _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);
    }
    else
    {
        movtoVRSQL.activarCols();
        while( movtoVRSQL.Leer() )
        {
            iFlag = movtoVRSQL.valorShort("muchecarmovtovr");
        }
    }

    if ( iFlag == 1 )
    {
        iSuma = obtenerSumaCapt(codigosMuebles );

        if ( codigosMuebles->nodo()->i16Existencia - ( iSuma + codigosMuebles->nodo()->i16Cantidad ) != 0 )
        {
            iFlag = 0;
        }
    }

    return iFlag;
}

int Cwsprestamos00024::checarAgotadoCodigo( char cFlagApartado, char cAgotadoInvent )
{
    int iFlagAgotado = 0, iFlagSistApartado = 0;

    consultarFlag( &odbcTiendaNumero , '0' , FLAGM_SISTEMAAPARTADO, &iFlagSistApartado );

    if ( iFlagSistApartado == 1 )
    {
        if ( cFlagApartado == '1' || cAgotadoInvent == 'A' )
        {
            iFlagAgotado = 1;
        }
    }
    else
    {
        if ( cAgotadoInvent == 'A' ) iFlagAgotado = 1;
    }

    return iFlagAgotado;
}

bool Cwsprestamos00024::tiempoExhibicionUdi( long lFamilia, int &iTiempoExhib )
{
    bool bContinuar = false;
    char cSqlTxt[128] = {0}, cLog[128]={0};
    mysnprintf( cSqlTxt, sizeof(cSqlTxt)-1,"SELECT tiempoduraciontda FROM mudcf WHERE familia = %ld ",lFamilia);

    CRegistro sysFlagsSQL(&odbcTiendaNumero);

    sysFlagsSQL.agregarCampoEntero("tiempoduraciontda",DT_ENTERO_CORTO);

    if (!sysFlagsSQL.Exec(cSqlTxt))
    {
        mysnprintf( cLog, sizeof(cSqlTxt)-1,"ERROR AL CONSULTAR [ tiempoduraciontda ] SQL -> [ %.80s ]", sysFlagsSQL.error() );
        //logx( cLog, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);
    }
    else
    {
        sysFlagsSQL.activarCols();
        while( sysFlagsSQL.Leer() )
        {
            iTiempoExhib = (int)sysFlagsSQL.valorShort("tiempoduraciontda" );
            bContinuar = true;
        }
    }
    if ( iTiempoExhib < 0 || iTiempoExhib > 15 ) iTiempoExhib = 0;

    mysnprintf( cLog, sizeof(cLog)-1,"tiempoExhibicionUdi = [ %d ], RESULTADO [ %s ]",iTiempoExhib,bContinuar? "true":"false" );
    //logx( cLog, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);
    
    return( bContinuar );
}