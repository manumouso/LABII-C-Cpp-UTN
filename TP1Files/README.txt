TP N°1 ARCHIVOS 

Si lo queres ejecutar en Visual Studio 2019 antes de hacerlo debes chequear estas configuraciones del
proyecto: 

1)compilar sin warnings: Proyecto->Propiedades->C/C++->Preprocesador->Def. de Preprocesado-> _CRT_SECURE_NO_WARNINGS;<opciones...>
Configuracion->Todas las configuraciones. Plataforma->X64
2)argumento users.txt: Depurar->Propiedades->Depuracion->ArgumentosDeComando-> users.txt

CONFIGURACION.TXT:
1)Pense un archivo de texto que tiene solo un char en la primera pos, 'o' || 'm', para que el usuario pueda determinar
  cuando quiere crear los archivos users.dat y pedidos.txt utilizando otros archivos distintos a los que propongo o 
  empezar de cero con las versiones actuales. Y para que no se esten creando con cada ejecucion una y otra vez.
  Ademas esto me permitio que las altas de los usuarios en users.dat persistan mas alla de la ejecucion actual.
2)Si por algun motivo no se encuentra el archivo configuracion.txt en la carpeta del proyecto:
  Creelo con ese mismo nombre, coloque una 'o' en la primera posicion, guardelo en la carpeta del proyecto, y cierre el archivo.
  El programa no funcionara sin este archivo.

Para el punto 3):
La version de insertar ordenado que entrego la plantee usando exclusivamente los temas 
referidos a archivos porque entiendo que a eso apunta el trabajo practico, pero ademas porque mi idea es que funcione 
con cualquier users.txt que se pruebe y no tener que estar definiendo un vector MAX con la posibilidad
de perder datos, o contar los registros y definir un max pero que no sea soportado en memoria.
Mas alla de esto: 
Dejo la version usando un vector para tamaños acotados de users.dat:

void altaUsuario(const char* rutaBinaria, STR_USUARIO usuarioInsertar) {

	FILE* usuarios = abrir(rutaBinaria, "rb+");
	
	STR_USUARIO vector[200];
	STR_USUARIO usuario;
	int posicionInsertar = 0;
	bool inserto = false;
	size_t cantidadLeida = 0;

	leer(&usuario, usuarios);
	while (!feof(usuarios)) {

		if ((strcmp(usuarioInsertar.email, usuario.email) < 0) && !inserto) {
			
			posicionInsertar = ftell(usuarios) - sizeof(STR_USUARIO);
			inserto = true;
			fseek(usuarios,posicionInsertar,SEEK_SET);
			cantidadLeida = fread(&vector,sizeof(STR_USUARIO),200,usuarios);
		}
		
		leer(&usuario, usuarios);
		}
	}
	//INSERTAR ORDENADO
	if (inserto) {
		
		fseek(usuarios, posicionInsertar, SEEK_SET);
		escribir(usuarioInsertar, usuarios);
		fwrite(&vector,sizeof(STR_USUARIO),cantidadLeida,usuarios);
		
	}
	else//insertar al final porque era el mayor
	{
		fseek(usuarios, 0, SEEK_END);
		escribir(usuarioInsertar, usuarios);
	}
	
	cerrar(usuarios);
	return;
}



