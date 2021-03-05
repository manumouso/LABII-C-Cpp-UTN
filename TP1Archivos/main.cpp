// main.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define FORMATO_USUARIO "\t%s\t\t%s\t\t%s\n"
#define FORMATO_PEDIDOS_ENTRADA "\t%s\t\t%d\t\t%.2f\t\t%d\n"
#define FORMATO_ENCABEZADO "Email\t\t\tNombre de Usuario\t\tCantidad de Productos\t\tTotal Facturable\n"
#define FORMATO_EMAIL_NOMBRE "%s\t\t%s\t\t"
#define FORMATO_RESULTADOS "\t\t%d\t\t\t\t$%.2f\n"

const int MAX_CARACTERES = 16;
const int MIN_CARACTERES = 3;
const int MAX_PASSWORD = 33;
const int MIN_PASSWORD = 8;
const int MAX_LINEA = 300;
const int MAX_AUX = 80;
const int MAX_PEDIDOS = 200;
const char EMAIL = 'e';
const char NOMBRE = 'n';

typedef struct Usuario {
	char nombre[MAX_CARACTERES];
	char email[MAX_CARACTERES];
	char password[MAX_PASSWORD];
}STR_USUARIO;

typedef struct Pedidos {
	char email[MAX_CARACTERES];//15 caracteres
	unsigned short idProducto;//max:65535 productos-5 caracteres(digitos)
	float valorUnitario;//8 caracteres(entre 6-7 digitos significativos + '.')
	unsigned short cantidadSolicitada;//max:65535-5 caracteres(digitos)
}STR_PEDIDOS;//por linea -> aprox 37 caracteres con los ';' y '\n'


void discardChars();
bool limiteCaracteres(const char* cadena);
bool esCaracterValido(char letra,char tipo);
bool esCadenaValida(const char* cadena,char tipo);
bool esPasswordValida(const char* cadena, char tipo);
bool unArroba(const char* cadena);
bool arrobaPosicionValida(const char* cadena);
bool puntoPosicionValida(const char* cadena);
bool validarDatos(const char* nombre, const char* email, const char* password);
FILE* abrir(const char* fileName, const char* modo);
void cerrar(FILE* file);
size_t leer(STR_USUARIO* registro, FILE* file);
size_t escribir(STR_USUARIO registro, FILE* file);
void imprimirTxt(const char* ruta);
bool longitudBufferPermitida(const char* cadena);
void esTokenNull(const char* token, FILE* texto, FILE* binario, FILE* textoSalida);
void generarArchivos(const char* rutaTextoEntrada, const char* rutaBinariaSalida, const char* rutaTextoSalida, const char* separadores);
void binTxtUsuarios(const char* rutaTexto, const char* rutaBinaria);
STR_USUARIO login(FILE* usuarios, const char* emailBuscado, const char* passwordBuscada);
void solicitarDatosLogin();
void altaUsuario(const char* rutaBinaria, STR_USUARIO usuarioInsertar);
bool existeEmail(const char* emailBuscado);
bool existeNombre(const char* nombreBuscado);
void solicitarDatosAlta();
void tokenNullPedidos(const char* token, FILE* texto, FILE* binario);
void txtBinPedidos(const char* rutaTexto, const char* rutaBinaria, const char* separadores);
void binTxtPedidosEntrada(const char* rutaTexto, const char* rutaBinaria);
STR_USUARIO obtenerUsuarioActual(const char* emailBuscado);
void generarResumenPedidos(const char* rutaTexto, const char* rutaBinaria);
void leerConfiguracion(const char* usersTxt);
void modificarConfiguracion();
void header();
void menu();
void footer();

//ANTES DE EMPEZAR, CHEQUEAR en VISUAL STUDIO:
//1)compilar sin warnings: Proyecto->Propiedades->C/C++->Preprocesador->Def. de Preprocesado-> _CRT_SECURE_NO_WARNINGS;<opciones...>
//Configuracion->Todas las configuraciones. Plataforma->X64
//2)argumento users.txt: Depurar->Propiedades->Depuracion->ArgumentosDeComando-> users.txt
int main(int argc, char* argv[])
{
	if (argc < 2) {
		printf("No ingresaste la ruta del archivo users.txt");
		exit(EXIT_FAILURE);
	}
	leerConfiguracion(argv[1]);
	menu();
	footer();
	return EXIT_SUCCESS;
}

void discardChars() {
	char c;
	while ((c = getchar()) != '\n' && c != EOF);
	return;
}

//Limite caracteres para email y usuario 
//max: 15 sin '\0'
//min: 3 sin '\0'
bool limiteCaracteres(const char* cadena) {

	return ((strlen(cadena) >= MIN_CARACTERES) && (strlen(cadena) < MAX_CARACTERES));
}
//Caracteres Validos: email:. @ digitos mayusculas y minusculas 
//usuario: . digitos mayusculas y minusculas
//password: digitos mayusculas y minusculas
bool esCaracterValido(char letra, char tipo) {

	if (tipo == EMAIL) {
		return (letra == '.' || letra == '@' || (letra >= '0' && letra <= '9') || (letra >= 'A' && letra <= 'Z') || (letra >= 'a' && letra <= 'z'));
	}
	else if (tipo == NOMBRE)
	{
		return (letra == '.' || (letra >= '0' && letra <= '9') || (letra >= 'A' && letra <= 'Z') || (letra >= 'a' && letra <= 'z'));
	}
	else {
		return ((letra >= '0' && letra <= '9') || (letra >= 'A' && letra <= 'Z') || (letra >= 'a' && letra <= 'z'));
	}

}

bool esCadenaValida(const char* cadena, char tipo) {

	int longitud = (int)strlen(cadena);
	bool valida = true;
	for (int i = 0; i < longitud; i++) {
		if (!esCaracterValido(cadena[i], tipo)) {
			valida = false;
		}
	}
	return valida;
}

bool esPasswordValida(const char* cadena, char tipo) {
	int longitud = 0;
	bool caracteresValidos = true;
	int cantidadNumeros = 0;
	int cantidadLetras = 0;
	while (*cadena) {
		longitud++;
		if (!esCaracterValido((*cadena), tipo)) {
			caracteresValidos = false;
		}
		if ((*cadena) >= '0' && (*cadena) <= '9'){
			cantidadNumeros++;
		}
		if (((*cadena) >= 'A' && (*cadena) <= 'Z') || ((*cadena) >= 'a' && (*cadena) <= 'z')) {
			cantidadLetras++;
		}
		cadena++;
	}
	return ((longitud >= MIN_PASSWORD && longitud < MAX_PASSWORD) && caracteresValidos && (cantidadNumeros != 0) && (cantidadLetras != 0));
}

//Chequea que exista solo 1 arroba
bool unArroba(const char* cadena) {

	int logitud = (int)strlen(cadena);
	int contador = 0;
	for (int i = 0; i < logitud; i++) {
		if (cadena[i] == '@') {
			contador++;
		}
	}
	return contador == 1;
}
//Posiciones prohibidas del arroba: inicio, fin
bool arrobaPosicionValida(const char* cadena) {
	
	int logitud = (int)strlen(cadena);

	return (cadena[0] != '@' && cadena[logitud - 1] != '@');
}

//Posiciones prohibidas para el punto: inicio,final,consecutivos y adelante/atras arroba
//No es obligatorio el punto, solo que si existe que sea coherente con la ubicacion dentro email
bool puntoPosicionValida(const char* cadena) {

	int logitud = (int)strlen(cadena);
	bool noAlrededorArroba = true;
	bool noConsecutivos = true;
	for (int i = 0; i < logitud; i++) {
		if (cadena[i] == '@') {
			if (cadena[i - 1] == '.' || cadena[i + 1] == '.') {
				noAlrededorArroba = false;
			}
		}
		if (cadena[i] == '.') {
			if (cadena[i + 1] == '.') {
				noConsecutivos = false;
			}
		}
	}

	return (cadena[0] != '.' && cadena[logitud-1] != '.' && noAlrededorArroba && noConsecutivos);
}

bool validarDatos(const char * nombre, const char * email, const char * password) {
	
	bool datosValidos = false;
	if (limiteCaracteres(nombre) && limiteCaracteres(email) && esPasswordValida(password, 'p')) {
		if (esCadenaValida(nombre, 'n') && esCadenaValida(email, 'e')) {
			if (unArroba(email)) {
				if (arrobaPosicionValida(email)) {
					if (puntoPosicionValida(email) && puntoPosicionValida(nombre)) {
						datosValidos = true;
					}
				}
			}
		}
	}

	return datosValidos;
}

FILE* abrir(const char* fileName, const char* modo)
{
	FILE* file = fopen(fileName, modo);
	if (file == NULL)
	{
		fprintf(stderr, "No se pudo abrir el archivo: %s", fileName);
		exit(EXIT_FAILURE);
	}

	return file;
}

void cerrar(FILE* file) {
	fclose(file);
	file = NULL;
}

size_t leer(STR_USUARIO* registro, FILE* file) {
	return fread(registro, sizeof(STR_USUARIO), 1, file);
}
size_t escribir(STR_USUARIO registro, FILE* file) {
	return fwrite(&registro, sizeof(STR_USUARIO), 1, file);
}

void imprimirTxt(const char* ruta) {
	FILE* archivoImprimir = abrir(ruta, "r");

	char letra;
	int leidos = fscanf(archivoImprimir, "%c", &letra);
	while (leidos == 1) {
		printf("%c", letra);
		leidos = fscanf(archivoImprimir, "%c", &letra);
	}

	cerrar(archivoImprimir);
	return;
}

bool longitudBufferPermitida(const char* cadena) {

	return ((int)strlen(cadena) < MAX_AUX);
}
void esTokenNull(const char * token, FILE* texto,FILE* binario, FILE* textoSalida) {
	
	if (token == NULL || !(longitudBufferPermitida(token))) {
		printf("Fallo parseo, o estas rebalsando buffers\n");
		printf("Intenta con otro archivo users.txt\n");
		cerrar(texto);
		cerrar(binario);
		cerrar(textoSalida);
		exit(EXIT_FAILURE);
	}
	return;
}
//precondicion: users.txt ordenado asc por email no tiene nombre usuario repetidos(NO ESTA VACIO), recibe como argumentos las rutas y los separadores
//postcondicion:crea archivos users.dat con usuarios validos y rejected.txt con los que no cumplen algun requisito
//users.txt no se elimina
//los usuarios validos con email repetido no son agregados en ninguna archivo-> se descartan
void generarArchivos(const char* rutaTextoEntrada, const char* rutaBinariaSalida,const char * rutaTextoSalida, const char * separadores) {

	FILE* texto = abrir(rutaTextoEntrada, "r");
	FILE* binario = abrir(rutaBinariaSalida, "wb");
	FILE* textoSalida = abrir(rutaTextoSalida, "w");

	STR_USUARIO usuario,anterior;
	strcpy(anterior.email, "0");
	char nombreAux[MAX_AUX];
	char emailAux[MAX_AUX];
	char passwordAux[MAX_AUX];
	char linea[MAX_LINEA];
	char* token = NULL;
	bool datosValidos = false;

	while ((fgets(linea, MAX_LINEA - 1, texto)) != NULL) {

		token = strtok(linea, separadores);
		esTokenNull(token,texto,binario,textoSalida);
		strcpy(nombreAux, token);

		token = strtok(NULL, separadores);
		esTokenNull(token, texto, binario, textoSalida);
		strcpy(emailAux, token);

		token = strtok(NULL, separadores);
		esTokenNull(token, texto, binario, textoSalida);
		strcpy(passwordAux, token);

		datosValidos = validarDatos(nombreAux,emailAux,passwordAux);
		
		if (datosValidos) {
			strcpy(usuario.nombre,nombreAux);
			strcpy(usuario.email,emailAux);
			strcpy(usuario.password,passwordAux);
			if (strcmp(usuario.email, anterior.email) != 0) {//con esta linea me aseguro de que email sea UK
				escribir(usuario, binario);
				strcpy(anterior.email, usuario.email);//el anterior valido
			}
		}
		else {
			fprintf(textoSalida, FORMATO_USUARIO,nombreAux,emailAux,passwordAux);
		}
	}
	
	cerrar(texto);
	cerrar(binario);
	cerrar(textoSalida);
	if (token != NULL) {
		token = NULL;
	}
	return;
}
void binTxtUsuarios(const char* rutaTexto, const char* rutaBinaria) {

	FILE* texto = abrir(rutaTexto, "w");
	FILE* binario = abrir(rutaBinaria, "rb");
	STR_USUARIO usuario;

	leer(&usuario, binario);
	while (!feof(binario)) {
		fprintf(texto, FORMATO_USUARIO, usuario.nombre, usuario.email, usuario.password);
		leer(&usuario, binario);
	}
	cerrar(texto);
	cerrar(binario);
	return;
}

//precondicion: recibe un puntero al archivo usuarios abierto modo lectura, email y password 
//postcondicion: devuelve un usuario completo si lo encuentra 
//o en el email "0" no pertenece a users.dat, "1" cuando existe pero mal password
STR_USUARIO login(FILE* usuarios, const char * emailBuscado,const char * passwordBuscada) {

	STR_USUARIO leido;
	long centro = 0;
	fseek(usuarios, 0, SEEK_SET);
	long inicio = ftell(usuarios);
	fseek(usuarios, 0, SEEK_END);
	long fin = ftell(usuarios) / sizeof(STR_USUARIO);

	while (inicio <= fin) {
		
		centro = ((fin + inicio) / 2);
		fseek(usuarios, sizeof(STR_USUARIO) * centro, SEEK_SET);

		leer(&leido, usuarios);
		if (strcmp(leido.email, emailBuscado)==0) {
			if (strcmp(leido.password, passwordBuscada) == 0) {
				return leido;
			}
			else {
				strcpy(leido.email, "1");
				return leido;
			}
		}
		else if (strcmp(leido.email,emailBuscado)>0) {
			fin = centro - 1;
		}
		else {
			inicio = centro + 1;
		}
	}

	strcpy(leido.email,"0"); 
	return leido;
}

void solicitarDatosLogin() {
	STR_USUARIO usuario;
	FILE* usuarios = abrir("users.dat", "rb");
	char emailAux[MAX_AUX];
	char passwordAux[MAX_AUX];

	printf("\t\t\t\tLOGIN\n");
	printf("\n");
	printf("\t\t\tIngrese email: ");
	scanf(" %s", emailAux);
	printf("\n");
	printf("\t\t\tIngrese password: ");
	scanf(" %s", passwordAux);
	discardChars();
	printf("\n");
	if (longitudBufferPermitida(emailAux) && longitudBufferPermitida(passwordAux)) {
		usuario = login(usuarios, emailAux, passwordAux);
	}
	else {
		printf("\n\t\t\t(*o*)!! Ingreso email o contraseña demasiado grandes.\n");
		cerrar(usuarios);
		exit(EXIT_FAILURE);
	}
	
	if (strcmp(usuario.email,emailAux) == 0) {
		printf("\t\t\tBIENVENIDO-->%s\t (^o^)y\n", usuario.nombre);
	}
	else {
		if (strcmp(usuario.email, "0") == 0) {
			printf("\t\t\t(-_-)? No se encontro al usuario.\n");
			printf("\n");
			printf("\t\t\tIntentelo nuevamente.\n");
		}
		else {
			printf("\t\t\t(*o*)!! Password erronea.\n");
			printf("\n");
			printf("\t\t\tIntentelo nuevamente.\n");
		}
	}
	
	cerrar(usuarios);
	return;
}

//precondicion: recibe la ruta al archivo users.dat que contiene al menos 1 dato y el usuario a insertar
//postcondicion: inserta ordenado en el archivo users.dat
//*solucion usando vectores en README.TXT
void altaUsuario(const char* rutaBinaria, STR_USUARIO usuarioInsertar) {

	FILE* usuarios = abrir(rutaBinaria, "rb+");
	FILE* temporal = abrir("temporal.dat", "wb+");
	
	STR_USUARIO usuario;
	int posicionInsertar = 0;
	bool inserto = false;
	size_t cantidadLeida = 1;

	leer(&usuario, usuarios);
	while (!feof(usuarios)) {

		if ((strcmp(usuarioInsertar.email, usuario.email) < 0) && !inserto) {
			
			posicionInsertar = ftell(usuarios) - sizeof(STR_USUARIO);
			inserto = true;
			
			while (!feof(usuarios)) {
				escribir(usuario, temporal);
				cantidadLeida = leer(&usuario, usuarios);
			}
			fflush(temporal);
		}
		if (cantidadLeida == 1) {
			leer(&usuario, usuarios);
		}
	}
	//INSERTAR ORDENADO
	if (inserto) {
		
		fseek(usuarios, posicionInsertar, SEEK_SET);
		escribir(usuarioInsertar, usuarios);

		fseek(temporal, 0, SEEK_SET);
		leer(&usuario, temporal);
		while (!feof(temporal)) {
			escribir(usuario, usuarios);
			leer(&usuario, temporal);
		}
	}
	else//insertar al final porque era el mayor
	{
		fseek(usuarios, 0, SEEK_END);
		escribir(usuarioInsertar, usuarios);
	}
	
	cerrar(usuarios);
	fclose(temporal);
	if((remove("temporal.dat"))!=0){
		printf("No se pudo eliminar el archivo temporal.dat");
		exit(EXIT_FAILURE);
	}
	if(temporal!=NULL){
		temporal = NULL;
	}
	
	return;
}
bool existeEmail(const char * emailBuscado) {

	FILE* usuarios = abrir("users.dat", "rb");

	STR_USUARIO leido;
	long centro = 0;
	fseek(usuarios, 0, SEEK_SET);
	long inicio = ftell(usuarios);
	fseek(usuarios, 0, SEEK_END);
	long fin = ftell(usuarios) / sizeof(STR_USUARIO);

	while (inicio <= fin) {
		
		centro = ((fin + inicio) / 2);
		fseek(usuarios, sizeof(STR_USUARIO) * centro, SEEK_SET);

		leer(&leido, usuarios);
		if (strcmp(leido.email, emailBuscado) == 0) {
			cerrar(usuarios);
			return true;
		}
		else if (strcmp(leido.email, emailBuscado) > 0) {
			fin = centro - 1;
		}
		else {
			inicio = centro + 1;
		}
	}
		
	cerrar(usuarios);
	return false;
}
bool existeNombre(const char * nombreBuscado) {

	FILE* usuarios = abrir("users.dat", "rb");
	STR_USUARIO usuario;
	leer(&usuario, usuarios);
	while (!feof(usuarios)) {
		if (strcmp(usuario.nombre, nombreBuscado) == 0) {
			cerrar(usuarios);
			return true;
		}
		leer(&usuario, usuarios);
	}
	cerrar(usuarios);
	return false;
}

void solicitarDatosAlta() {

	STR_USUARIO usuarioInsertar;
	char nombreAux[MAX_AUX];
	char emailAux[MAX_AUX];
	char passwordAux[MAX_AUX];
	bool datosValidos = false;
	printf("\t\t\t\tALTA\n");
	printf("\n");
	printf("\t\t\tIngrese nombre de usuario: ");
	scanf(" %s", nombreAux);
	printf("\n");
	printf("\t\t\tIngrese email: ");
	scanf(" %s", emailAux);
	printf("\n");
	printf("\t\t\tIngrese password: ");
	scanf(" %s", passwordAux);
	discardChars();
	printf("\n");
	datosValidos = validarDatos(nombreAux, emailAux, passwordAux);

	if (datosValidos) {
		if (!existeEmail(emailAux)) {
			if (!existeNombre(nombreAux)) {
				strcpy(usuarioInsertar.nombre, nombreAux);
				strcpy(usuarioInsertar.email, emailAux);
				strcpy(usuarioInsertar.password, passwordAux);

				altaUsuario("users.dat", usuarioInsertar);
				printf("\t\t\tAlta registrada satisfactoriamente. (^o^)y \n");
			}
			else {
				printf("\t\t(-_-)? Ya se encuentra registrado nombre: %s.\n", nombreAux);
				printf("\n");
				printf("\t\t\tIntenlo con otro nombre de usuario.\n");
			}
			
		}
		else {
			printf("\t\t(-_-)? Ya se encuentra registrado email: %s \n", emailAux);
			printf("\n");
			printf("\t\t\tIntenlo con otro email.\n");
		}
		
	}
	else {
		printf("\t\t\t(*o*)!! Dato/s invalido/s.\n");
		printf("\n");
		printf("\t\t\tIntentelo nuevamente.\n");
	}

	return;
}
void tokenNullPedidos(const char* token, FILE* texto, FILE* binario) {
	
	if (token == NULL) {
		printf("Fallo parseo.\n");
		cerrar(texto);
		cerrar(binario);
		exit(EXIT_FAILURE);
	}
	return;
}
//precondicion: recibe la ruta del archivo de texto listaPedidos con sus datos que se corresponden a los tipos de STR_PEDIDOS
//y todos los emails existen en users.dat
//por linea aprox 38 caracteres separados ;
//no estoy validando las longitudes de las cadenas de este archivo (porque lo arme yo, no lo recibe el prog)
//postcondicion: genera el archivo pedidos.dat necesario para el punto 4
void txtBinPedidos(const char* rutaTexto, const char* rutaBinaria, const char* separadores) {

	FILE* texto = abrir(rutaTexto, "r");

	FILE* binario = abrir(rutaBinaria, "wb");

	STR_PEDIDOS pedido;

	char linea[MAX_PEDIDOS];
	char* token = NULL;

	while ((fgets(linea, MAX_PEDIDOS - 1, texto)) != NULL) {

		token = strtok(linea, separadores);
		tokenNullPedidos(token,texto,binario);
		strcpy(pedido.email, token);

		token = strtok(NULL, separadores);
		tokenNullPedidos(token, texto, binario);
		pedido.idProducto = (unsigned short)atoi(token);

		token = strtok(NULL, separadores);
		tokenNullPedidos(token, texto, binario);
		pedido.valorUnitario = (float)atof(token);

		token = strtok(NULL, separadores);
		tokenNullPedidos(token, texto, binario);
		pedido.cantidadSolicitada = (unsigned short)atoi(token);

		fwrite(&pedido, sizeof(STR_PEDIDOS), 1, binario);
	}

	cerrar(texto);
	cerrar(binario);
	if (token != NULL) {
		token = NULL;
	}
	return;
}


void binTxtPedidosEntrada(const char* rutaTexto, const char* rutaBinaria) {

	FILE* texto = abrir(rutaTexto, "w");
	FILE* binario = abrir(rutaBinaria, "rb");
	STR_PEDIDOS pedido;

	fread(&pedido, sizeof(STR_PEDIDOS), 1, binario);
	while (!feof(binario)) {
		fprintf(texto, FORMATO_PEDIDOS_ENTRADA, pedido.email, pedido.idProducto, pedido.valorUnitario, pedido.cantidadSolicitada);
		fread(&pedido, sizeof(STR_PEDIDOS), 1, binario);
	}
	cerrar(texto);
	cerrar(binario);
	return;
}
 STR_USUARIO obtenerUsuarioActual(const char* emailBuscado) {

	FILE* usuarios = abrir("users.dat", "rb");

	STR_USUARIO leido;
	long centro = 0;
	fseek(usuarios, 0, SEEK_SET);
	long inicio = ftell(usuarios);
	fseek(usuarios, 0, SEEK_END);
	long fin = ftell(usuarios) / sizeof(STR_USUARIO);

	while (inicio <= fin) {

		centro = ((fin + inicio) / 2);
		fseek(usuarios, sizeof(STR_USUARIO) * centro, SEEK_SET);

		leer(&leido, usuarios);
		if (strcmp(leido.email, emailBuscado) == 0) {
			cerrar(usuarios);
			return leido;
		}
		else if (strcmp(leido.email, emailBuscado) > 0) {
			fin = centro - 1;
		}
		else {
			inicio = centro + 1;
		}
	}

	cerrar(usuarios);
	strcpy(leido.email, "0");//habria un usuario de pedidos.dat que no esta en users.dat, seria practicamente imposible por la precondicion de txtBinPedidos
	return leido;
}

void generarResumenPedidos(const char* rutaTexto, const char* rutaBinaria) {
	
	FILE* texto = abrir(rutaTexto, "w");
	FILE* binario = abrir(rutaBinaria, "rb");
	
	STR_PEDIDOS pedido;
	STR_USUARIO usuarioActual;
	int cantidadProductos = 0;
	float totalFacturable = 0;
		
	fprintf(texto, FORMATO_ENCABEZADO);
	fread(&pedido, sizeof(STR_PEDIDOS), 1, binario);
	while (!feof(binario)) {
		cantidadProductos = 0;
		totalFacturable = 0;
		usuarioActual = obtenerUsuarioActual(pedido.email);
		
		if (strcmp(usuarioActual.email, "0") != 0) {// chequeo que exista el usuario que hizo pedido en users.dat

			fprintf(texto,FORMATO_EMAIL_NOMBRE, usuarioActual.email, usuarioActual.nombre);
			while (strcmp(usuarioActual.email, pedido.email) == 0 && !feof(binario)) {
				cantidadProductos += pedido.cantidadSolicitada;
				totalFacturable += (pedido.valorUnitario * pedido.cantidadSolicitada);
				fread(&pedido, sizeof(STR_PEDIDOS), 1, binario);
			}
			fprintf(texto, FORMATO_RESULTADOS, cantidadProductos, totalFacturable);
		}
	}

	cerrar(binario);
	cerrar(texto);
	return;
}
//precondicion:recibe la ruta de users.txt
//postcondicion:llama a las funciones que generan los archivos users.dat y pedidos.txt la primera vez que ejecuta el programa
//*explicacion README.TXT
void leerConfiguracion(const char * usersTxt) {

	char letra = 'm';
	FILE* configuracion = fopen("configuracion.txt", "r+");
	if (!configuracion) {
		printf("No se pudo abrir el archivo de configuracion.\n");
		printf("Si lo elimino->ingrese a README.txt para encontrar una solucion.\n");
		exit(EXIT_FAILURE);
	}
	fscanf(configuracion, "%c", &letra);
	if (letra == 'o') {

		generarArchivos(usersTxt, "users.dat", "rejected.txt", ";\n");
		/*para crear pedidos.dat y testearlo:
		txtBinPedidos("listaPedidos.txt", "pedidos.dat", ";\n");
		binTxtPedidosEntrada("listaPedidosTest.txt", "pedidos.dat");*/
		generarResumenPedidos("pedidos.txt", "pedidos.dat");

		letra = 'm';
		fseek(configuracion, 0, SEEK_SET);
		fprintf(configuracion, "%c", letra);
	}
	fclose(configuracion);
	configuracion = NULL;

	return;
}
//modifica al archivo de configuracion.txt
//facilita que la proxima ejecucion del programa cree los archivos users.dat y pedidos.txt nuevamente
//a partir de los actuales o de otros que se quieran probar
void modificarConfiguracion() {
	char confirmar = 'N';
	int continuar = 0;
	
	printf("\t\t\tPara ejecutar el programa con otros archivos\n");
	printf("\t\t\tusers.txt y pedidos.dat:\n");
	printf("\n");
	printf("\t\t\tRecuerde copiar las versiones actuales.\n");
	printf("\t\t\tGuardelas fuera del proyecto\n");
	printf("\t\t\tpara conservar la informacion.\n");
	printf("\t\t\tDe lo contrario no podra recuperar sus datos.\n");
	printf("\n");
	printf("\t\t\tDesea confirmar la operacion? [S/N]: [ ]\b\b");
	scanf(" %c", &confirmar);
	discardChars();
	printf("\n");
	if (confirmar == 'S' || confirmar == 's') {

		char letra = 'o';
		FILE* configuracion = fopen("configuracion.txt", "w");
		if (!configuracion) {
			printf("No se pudo crear el archivo de configuracion.\n");
			exit(EXIT_FAILURE);
		}
		fprintf(configuracion, "%c", letra);
		fclose(configuracion);
		configuracion = NULL;
		
		printf("\t\t\tOperacion exitosa.\n");
		printf("\t\t\tYa puede colocar sus archivos users.txt\n");
		printf("\t\t\ty pedidos.dat en la carpeta del proyecto.\n");
		printf("\n");
	}
	else {
		printf("\t\t\tOperacion cancelada.\n");
		printf("\t\t\tContinuara ejecutando el programa\n");
		printf("\t\t\tcon los archivos actuales.\n");
		printf("\n");
	}
	
	return;
}

void header() {

	printf("\n     ======================================================================\n");
	printf("\t\t     TRABAJO PRACTICO N1-ARCHIVOS (^o^)y\n");
	printf("     ======================================================================\n");
	
	return;
}

void menu() {
		
	int opcion = 0;
	int continuar = 0;
	do
	{
		system("cls");
		header();
		printf("\n\t\t\t\tMENU \n");
		printf("\n\t\t\t[1].Imprimir usuarios recibidos\n");
		printf("\t\t\t[2].Imprimir usuarios validos\n");
		printf("\t\t\t[3].Imprimir usuarios rechazados\n");
		printf("\t\t\t[4].Login\n");
		printf("\t\t\t[5].Alta usuario\n");
		printf("\t\t\t[6].Imprimir lista pedidos\n");
		printf("\t\t\t[7].Imprimir resumen pedidos\n");
		printf("\t\t\t[8].Configuracion\n");
		printf("\t\t\t[0].Salir\n");
		printf("\n\t\t\tSeleccione una opcion: [ ]\b\b");
		scanf(" %d", &opcion);
		discardChars();

		switch (opcion){
			case 0:
				printf("\n");
				printf("\t\t\tTermino el programa. ( ^_^)/\n");
			break;
			case 1:
				printf("\n");
				printf("\tUSERS.TXT\n");
				printf("\n");
				imprimirTxt("users.txt");
				printf("\n");
				printf("\n");
				printf("\t\t\tIngrese un caracter para continuar-> ");
				continuar = getchar();
			break;
			case 2:
				printf("\n");
				printf("\t\t\tUSERS.DAT\n");
				printf("\n");
				binTxtUsuarios("imprimirDat.txt", "users.dat");
				imprimirTxt("imprimirDat.txt");
				printf("\n");
				printf("\n");
				printf("\t\t\tIngrese un caracter para continuar-> ");
				continuar = getchar();
			break;
			case 3:
				printf("\n");
				printf("\t\t\tREJECTED.TXT\n");
				printf("\n");
				imprimirTxt("rejected.txt");
				printf("\n");
				printf("\n");
				printf("\t\t\tIngrese un caracter para continuar-> ");
				continuar = getchar();
			break;
			case 4:
				printf("\n");
				solicitarDatosLogin();
				printf("\n");
				printf("\n");
				printf("\t\t\tIngrese un caracter para continuar-> ");
				continuar = getchar();
			break;
			case 5:
				printf("\n");
				solicitarDatosAlta();
				printf("\n");
				printf("\t\t\tIngrese un caracter para continuar-> ");
				continuar = getchar();
			break;
			case 6:
				printf("\n");
				printf("\t\t\tLISTAPEDIDOSTEST.TXT\n");
				printf("\n");
				binTxtPedidosEntrada("listaPedidosTest.txt", "pedidos.dat");
				imprimirTxt("listaPedidosTest.txt");
				printf("\n");
				printf("\n");
				printf("\t\t\tIngrese un caracter para continuar-> ");
				continuar = getchar();
			break;
			case 7:
				printf("\n");
				printf("\t\t\tPEDIDOS.TXT");
				printf("\n");
				printf("\n");
				imprimirTxt("pedidos.txt");
				printf("\n");
				printf("\n");
				printf("\t\t\tIngrese un caracter para continuar-> ");
				continuar = getchar();
			break;
			case 8:
				printf("\n");
				printf("\t\t\t\tCONFIGURACION\n");
				printf("\n");
				modificarConfiguracion();
				printf("\n");
				printf("\t\t\tIngrese un caracter para continuar-> ");
				continuar = getchar();
				break;
			default:
				printf("\n");
				printf("\t\t\tIngrese un numero valido [1,2,3,4,5,6,7,8]\n");
				printf("\t\t\t[0]:Finalizar.\n");
				printf("\n");
				printf("\t\t\tIngrese un caracter para continuar-> ");
				continuar = getchar();
			break;
		}
	} while (opcion != 0);

	return;
}

void footer() {

	printf("\n     /**********************************************************************\n");
	printf("\t\t\t*Alumno: Manuel Onetto Mouso\n");
	printf("\t\t\t*Email: manumouso91@gmail.com\n");
	printf("\t\t\t*Materias: PROG/LAB II\n");
	printf("\t\t\t*Carrera: TSSI-UTN-FRBA\n");
	printf("\t\t\t*Fecha: 28/9/2020\n");
	printf("     ***********************************************************************/\n");

	return;
}