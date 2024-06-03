# Level-5 - Grupo 5 - EDAoogle 🌐

## Participantes 👥

* ***Santino Nastasi:*** programador.
* ***Camila Castro:*** programador.

## Esquema de la base de datos e implementación 🧑‍💻

El esquema que decidimos implementar fue el de una base de datos con ***tablas indexadas***, utilizando ***FTS*** como herramienta. Tomamos esta decisión ya que las tablas indexadas mejoran el rendimiento cuando se manejan muchos datos y optimizan la busqueda y obtención de los datos. Por otro lado, incluimos el uso de FTS ya que estamos trabajando con textos grandes y también porque permite busquedas utilizando operadores como AND y OR. 

Logramos implementar ambas herramientas en ***mkindex.cpp*** donde se crea la base de datos. Manejamos dos tablas, la primera es la tabla principal (*wiki_pages*) donde se tienen dos parámetros (aparte del id): *page*, referida al nombre de la página HTML, y *pageText*, referida al texto que contiene la página. La segunda tabla es la tabla indexada que implementa FTS5 ya que las búsquedas se realizarán en esta tabla. La misma cuenta con dos campos: *page_name* y *content*.

Una vez implementado fts, el trabajo de la busqueda se reduce extremadamente. La busquedas con fts la realizamos con SELECT page_name from wiki_pages_fts WHERE wiki_pages_fts MATCH 'PALABRA/s a buscar'. Para eso armamos una funcion que utliza sqlite3_exec() como callback para devolvernos lo que encuentra la busqueda. La funcion es muy simple, como solo hacemos SELECT page_name, lo que devuelve la base de datos es una unica columna con los nombres de las paginas, es decir argv va a tener un unico argumento, el nombre de la pagina. Luego, al nombre de la pagina le agregamos el formato para que el string que se guarda en results sea el link a la pagina de wikipedia.

## Optimización ✅

A la hora de optimizar el código decidimos hacerlo mediante la implementacion de tablas indexadas y FTS, explicado previamente. Esto permite una búsqueda rápida y eficiente que no deja al usuario esperando y devuelve los resultados de la búsqueda rápidamente.

## Seguridad 🔒

Para asegurarnos de la seguridad de nuestro algoritmo, evitamos la inyección SQL utilizando consultas preparadas de SQL. Estas consultas nos permiten separar los datos de entrada de la lógica SQL al usar marcadores de posición en lugar de incrustar directamente los valores en la cadena SQL. Luego, utilizamos las funciones *sqlite3_bind_text* y *sqlite3_step* para vincular de forma segura los valores a estos marcadores de posición antes de ejecutar la consulta. Esto garantiza que los datos proporcionados por el usuario no se interpreten como código SQL y evita la posibilidad de inyección SQL.

## ✨BONUS✨: Implementación de operadores 

@Nastasi-Santino ...
