# Level-5 - Grupo 5 - EDAoogle 🌐

## Participantes 👥

* ***Santino Nastasi:*** programador.
* ***Camila Castro:*** programador.

## Esquema de la base de datos e implementación 🧑‍💻

El esquema que decidimos implementar fue el de una base de datos con ***tablas indexadas***, utilizando ***FTS*** como herramienta. Tomamos esta decisión ya que las tablas indexadas mejoran el rendimiento cuando se manejan muchos datos y optimizan la busqueda y obtención de los datos. Por otro lado, incluimos el uso de FTS ya que estamos trabajando con textos grandes y también porque permite busquedas utilizando operadores como AND y OR. 

Logramos implementar ambas herramientas en ***mkindex.cpp*** donde se crea la base de datos. Manejamos dos tablas, la primera es la tabla principal (*wiki_pages*) donde se tienen dos parámetros (aparte del id): *page*, referida al nombre de la página HTML, y *pageText*, referida al texto que contiene la página. La segunda tabla es la tabla indexada que implementa FTS5 ya que las búsquedas se realizarán en esta tabla. La misma cuenta con dos campos: *page_name* y *content*.

En contra partida a usar FTS5, podemos decir que, si bien mejora la optimizacion a la hora de realizar la busqueda, el coste a la hora de generar la base de datos es elevado. Esto se debe a que ademas de crear la tabla, ahora hay que copiarla a una tabla virtual. El cambio es notable, antes de implementar FTS5 la creacion de la base de datas era de alrededor de 30 segundos y usando FTS es de alrededor de 1 minuto. Sin embargo, es un precio que estamos dispuestos a pagar, la forma que tuvimos de pensarlo fue si EDAoogle fuese un producto comercial lo importante seria que funcione lo mas optimizado posible para el cliente, el cual no experimentaria el proceso de creacion de la base de datos.  

Una vez implementado FTS5, el trabajo de la búsqueda se reduce extremadamente. La búsqueda con FTS la realizamos usando *SELECT page_name from wiki_pages_fts WHERE wiki_pages_fts MATCH 'PALABRA/s a buscar'*. Para eso armamos una función que utliza ***sqlite3_exec()*** como callback para devolver lo que encuentra la búsqueda. La funcion es muy simple, como solo hacemos *SELECT page_name*, lo que devuelve la base de datos es una unica columna con los nombres de las paginas, es decir ***argv*** va a tener un único argumento, el nombre de la página. Luego, al nombre le agregamos el formato para que el string que se guarda en results sea el link a la página de wikipedia.

## Optimización ✅

A la hora de optimizar el código decidimos hacerlo mediante la implementacion de tablas indexadas y FTS, explicado previamente. Esto permite una búsqueda rápida y eficiente que no deja al usuario esperando y devuelve los resultados de la búsqueda de la forma más óptima posible.

## Seguridad 🔒

Para asegurarnos de la seguridad de nuestro algoritmo, evitamos la inyección SQL utilizando consultas preparadas de SQL. Estas consultas nos permiten separar los datos de entrada de la lógica SQL al usar marcadores de posición en lugar de incrustar directamente los valores en la cadena SQL. Luego, utilizamos las funciones *sqlite3_bind_text* y *sqlite3_step* para vincular de forma segura los valores a estos marcadores de posición antes de ejecutar la consulta. Esto garantiza que los datos proporcionados por el usuario no se interpreten como código SQL y evita la posibilidad de inyección SQL.

## ✨BONUS✨: Implementación de operadores 

Nuevamente, FTS5 al rescate!! La implementación de operadores lógicos es muy sencilla usando esta extención. Primero, asignamos distintos símbolos como operadores lógicos(indicados al usuario a traves de un cartel en la pagina):
* ~ es NOT 
* | es OR 
* & es AND
Luego, recorremos el string que viene del usuario y, en caso de encontrar alguno de estos símbolos, remplazamos el mismo por los operadores que usa FTS, que son las palabras **NOT**, **OR** y **AND** respectivamente. Aprovechamos este recorrido para eliminar símbolos que entran en conflicto con FTS5, como el punto(.), la coma(,), etc.

## Un poco de FTS5 📚

Ya que FTS hizo mucho del trabajo por nosotros, decidimos dedicar un poco de investigación para entender cómo funciona internamente. 

### Tokenizacion 🔑

Lo primero que hace FTS5 por nosotros es dividir el contenido del texto en palabras individuales o frases y las guarda en paquetes que se llaman **tokens**. Cabe aclarar que el tokenizador (la función interna que genera los distintos tokens) puede ser modificada por el usuario para considerar distintas palabras como iguales. Por ejemplo, si queremos que "Primero" y "1ero" sean considerados iguales, tenemos que modificar el tokenizador. Nosotros utilizamos la función default. Este proceso involucra también evitar palabras super usadas como "que", "el", etc. Y además, transforma algunas palabras en palabras raices, por ejemplo, verbos conjugados los guarda junto el mismo token del verbo no conjugado.

### Indexado 🗂️

FTS5 genera un índice invertido, es decir, mapea cada token con una lista de documentos donde aparece. Por lo que la búsqueda consiste en una simple revisión de este índice. Además, el mismo esta construido sobre una tabla virtual, cuya función es imitar el comportamiento de una tabla pero, en vez de hacerlo sobre la base de datos, lo hace sobre estructuras ***in-memory*** generadas por el motor de FTS5.

Según leímos los árboles B fueron creados bajo la inspiración de la búsqueda de texto. Por lo tanto, es más óptimo que un árbol de búsqueda binario, la gran diferencia esta en la incerción *O(log(n))* contra *O(n)*. FTS5 no usa árboles B comunes, sino que utiliza un estructura basada en las necesidades del problema. (*https://darksi.de/13.sqlite-fts5-structure/*) 
