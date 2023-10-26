#include "Cwsprestamos00024.h"

int Cwsprestamos00024::checarZonaUdi( char cQuienEntrega, char cModoDeEntrega, int iStatus, long lCodigo, 
                  int iCiudadCte, int iColoniaCte, bool &bContinuar, C_ODBC *odbcTiendaNumero, int iNumTienda )
{
    int iFlag = 0;
    bContinuar = true;

    char cUsuarioBD[30] = {0}, cPassBD[36], cNombreBD[30]={0}, cIpBdServidor[20]={0}, cMensaje[256]={0}, cSql[128]={0};

    consultarIpServidor( odbcTiendaNumero, SERV_CARTERA , cIpBdServidor );

    if(obtenerDatosConexionBD(CONECTA_CARTERA,cPassBD,cUsuarioBD,cNombreBD,(long)iNumTienda))
    {
        C_ODBC odbcCartera;

        int iRespuesta = abrirConexion( &odbcCartera,cIpBdServidor,cNombreBD,cUsuarioBD );
        if (iRespuesta == _OK_)
        {
            if( cQuienEntrega == 'S' && cModoDeEntrega != '~' )
            {
                if( iStatus == 1 && lCodigo == 1L )
                {
                    //Buscar la ciudad y la colonia del cliente
                    mysnprintf( cSql, sizeof(cSql)-1, "SELECT COUNT(*) FROM crZonas WHERE ciudad = %d AND colonia = %d", iCiudadCte, iColoniaCte );

                    CRegistro crZonasSQL( &odbcCartera ); //conexión a bd cartera

                    crZonasSQL.agregarCampoEntero("count",DT_ENTERO_LARGO);

                    if( crZonasSQL.Exec( cSql ) )
                    {
                        crZonasSQL.activarCols();
                        if( crZonasSQL.Leer() )
                        {
                            if( crZonasSQL.valorLong("count") == 0L )
                            {
                                iFlag = 1;
                            }
                        }
                    }
                    else
                    {
                        mysnprintf( cMensaje, sizeof(cMensaje)-1,"OCURRIO UN ERROR AL CONSULTAR LA TABLA crZonas ERROR: [%s]",crZonasSQL.error());
                        //logx( cMensaje, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);   

                        bContinuar = false;
                    }
                }
            }
        }else
        {
            mysnprintf( cMensaje, sizeof(cMensaje)-1,"ERROR AL CONECTAR A LA BASE DE DATOS: %s",cNombreBD);
            //logx( cMensaje, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);
        }
    }else
    {
        mysnprintf( cMensaje, sizeof(cMensaje)-1,"ERROR AL OBTENER LOS DATOS DE CONEXION A LA BD: %s",cNombreBD);
        //logx( cMensaje, _LOGINFO_, "M00020_PG_OBTENERFINANCIAMIENTO", "192.168.136.176", 99094620);
    }

    return iFlag;
}
