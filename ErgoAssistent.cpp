/****Proyecto 1 Laboratorio de Programacion******
********Araya, Luis   Bejarano, Mauricio*********
************************************************/

//Se agregan librerias basicas
#include <iostream>
#include <fstream> /*Para manejar archivos*/
#include <string>
#include <string.h> /* Para memset, strlen y otras funciones*/
#include <unistd.h> /*Para getopt, optarg*/
#include <stdlib.h> /* Para exit */

//Se añaden librerias para realizar la conexion entre mysql y c++
#include "mysql_connection.h"
#include "mysql_driver.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

//Se definen variables para realizar la conexion entre mysql y c++
#define EXAMPLE_HOST "localhost"
#define EXAMPLE_USER "root"
#define EXAMPLE_PASS "password"
#define EXAMPLE_DB "lab1"

using namespace sql;
using namespace std;

//Variable necesaria para enviar al modo stealth
bool stealth = false;
//Variable que contiene el nombre que se despliega en los procesos activos
string stealth_name = "";

//Funcion que imprime en consola la ayuda de como correr el programa
void print_help(char * program_name) {
  cout << "Ayuda para ejecutar el programa de forma correcta" << endl;
}    

/*
 * La siguiente funcion logra que un "ps ax" no devuelva el nombre del ejecutable.
 * Se reemplaza el nombre original con el nombre que se desea para que aparezca en la lista que despliega el comando "ps ax".
 */
void go_stealth(int &argc, char **argv) {
  int argv_lengths[argc];
  //Se recorren los argumentos 
  //Se define el tamano de memoria necesaria para cada argumento
  for (int i=0; i<argc; i++) {
    argv_lengths[i] = strlen(argv[i]);
    memset(argv[i], '\0', argv_lengths[i]);
  } 
  //Se define el tamano maximo que puede tener el nombre falso
  int max_argv_length = 16;
  int maximum_size = strlen(stealth_name.c_str())+1;
  char * name_to_split = new char[maximum_size];
  memset(name_to_split, 0, maximum_size);
  strcpy(name_to_split, stealth_name.c_str());
  char * token = strtok(name_to_split, " ");
  int argv_index = 0;
  strncpy(argv[argv_index], token, argv_lengths[argv_index]);
  argv_index++;
  while ((token = strtok(NULL, " ")) != NULL) {
    strncpy(argv[argv_index], token, argv_lengths[argv_index]);
    argv_index++;
    if (argv_index >= argc)
      break;
  }
  delete[] name_to_split;
}

/*
 * Esta funcion logra separar los argumentos introducidos en la ejecucion del programa 
 */ 
void parse_arguments(int argc, char **argv) {
//Variable que contendra los distintos switches que puede tener el programa
char opt;
//Se analiza si colocar la informacion o no
//Se se imprime la ayuda se sale del programa
if ((argc == 1) || (argv[1] == "--help") || (argv[1] == "-h") || (argv[1] == "-help")) {
  print_help(argv[0]);
  exit(EXIT_SUCCESS);
}
//Se analizan todos los argumentos
//En este caso la funcion getopt, permite analizar lo que esta despues de la opcion -s
//Si se desean agregar mas opciones, en lugar de "s:" deberia ser "s:d:t:" por ejemplo
while ((opt = getopt (argc, argv, "s:")) != -1)
  //Por el momento solo maneja la opciones -s, pero se podria ampliar en el futuro
  switch (opt) {
    case 's':
      // Se activa el stealth mode 
      stealth = true;
      // Se guarda el nombre falso del programa
      stealth_name = optarg;
      break;
    default:
      cout << "Se introducieron parametros no validos." << endl ;
      print_help(argv[0]);
      exit(EXIT_SUCCESS);
  }
}

//Funcion que realiza la conexion entre c++ y mysql
bool mysqlConexion(string url, const string user, const string pass, const string database) {

  cout << "Ejecutando Conexion con mySQL desde C++" << endl;

  try {
    //Se definen las variables necesarias para realizar la conexion con mysql
    sql::Driver *driver;
    driver = get_driver_instance();
    std::auto_ptr<sql::Connection> con(driver->connect(url, user, pass));
    con->setSchema(database);
    std::auto_ptr<sql::Statement> stmt(con->createStatement());

    //Estan serian los comandos para crear la tabla en la base de datos y leer el archivo de entrada en caso de que no se encuentre en la base de datos
    //stmt->execute("CREATE TABLE example4 (CARNE INT NOT NULL, APELLIDO1 VARCHAR(100) NOT NULL, APELLIDO2 VARCHAR(100) NOT NULL, NOMBRE VARCHAR(100) NOT NULL, RECINTO INT NOT NULL, ANO INT NOT NULL, PERIODO INT NOT NULL, GRUPO INT NOT NULL, SIGLA VARCHAR(50) NOT NULL, NOTA VARCHAR(100) NOT NULL);");
    //stmt->execute("LOAD DATA INFILE '/home/kike/Documents/LabProgramacion/Lab1/notas.csv' INTO TABLE example4 FIELDS TERMINATED BY ',' LINES TERMINATED BY '\n';");
    
  }
  //Se encuentran las excepciones de sql 
  catch (sql::SQLException &e) {
    cout << "# ERR: SQLException in " << __FILE__;
    cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
    cout << "# ERR: " << e.what();
    cout << " (MySQL error code: " << e.getErrorCode();
    cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    return false;
  }
  return true;
}
     
/*
 * Porgrama principal
 */
int main(int argc, char **argv) { 

  //Se definen variables necesarias para la conexion entre mySQL y C++
  string url = EXAMPLE_HOST;
  const string user = EXAMPLE_USER;
  const string pass = EXAMPLE_PASS;
  const string database = EXAMPLE_DB;
  
  bool conexion = true;
  //Se llama a funcion que realiza la conexion con mysql
  conexion = mysqlConexion(url, user, pass, database);

  if (conexion) {
    cout << "Conexion con SQL realizada correctamente" << endl;
  }
  else{
    cout << "No se logro crear conexion con SQL" << endl;
    exit(EXIT_FAILURE);
  }

  //Se analizan los parametros que el usuario ejecuto
  parse_arguments(argc, argv);

  //Se analiza si activar el modo stealth
  if (stealth) {
    go_stealth(argc, argv);
  }

  //Los siguientes comandos permiten enviar el programa a segundo plano

  //Con la funcion fork se crea un proceso "child"
  pid_t pid = fork();
  //Se detecta error, fork funciono de forma erronea
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }
  //Se creo el proceso child de forma correcta
  if (pid > 0) {
    //Se imprime el PID que se obtuvo para el proceso "child"
    cout << pid << endl;
    exit(EXIT_SUCCESS); //En este momento el proceso "Parent" se muere y queda el proceso "child"
  }

  //Se crea un SID para el proceso "child"
  pid_t sid = setsid();
  //Sucedio un error
  if (sid < 0) {
    exit(EXIT_FAILURE);
  }
  //Se obtuvo el SID de forma correcta

  //Se crea archivo de prueba
  ofstream myfile;
  myfile.open ("example.txt");
  
  for (int i = 0 ; i < 10 ; i++) {
    sleep(1); 
    myfile << "Writing this to a file.\n";
  }
  myfile.close();
  cout << "Finalizando programa" << endl;
  return 0;
}