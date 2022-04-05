-Puntos a realizar para la primera entrega:
Se debe de implementar un servidor web que trabaje de forma concurrente, en base a las clases facilitadas por el profesor jeisson hidalgo.
Estudio de las clases para comprender su funcionamiento.
El servidor web debe de ser capaz de aceptar y atender multiples conexiones, para ello una cola que se encargue de guardar las conexiones.
Implementar la clase HttpConnectionHandler que deriva de una clase consumer-thread, la cual se comporta como un hilo y se encarga de una conexion a la vez.
Mejorar el recibimiento de valores por el URI, para asi poder recibir una lista de numeros.
Implementar la aplicacion web para calcular las sumas de goldbach orientada a objetos llamada GoldbachWebApp, que recibe las solicitudes Http, saca los numeros y se las envia a la clase GoldbachCalculator, la cual se encarga de obtener las sumas para cada numero y retornar sus resultados a la clase GoldbachWebApp, la cual se encarga de generar el response
Si el programa es apagado con Ctrl C o el comando kill, este debe de dejar de aceptar conexiones y terminar el trabajo pendiente, asi como liberar memoria y recursos.
Documentacion formato doxygen
Cpp Lintern
Correr el programa con asan, tsan, ubsan, msan
Buenas practicas de programacion
Separacion de Archivos