Pasos para ejecutar el servicio de mensajería:

1.
Compilar todo mediante make all

2.
Obtener la IP del contenedor donde se va a ejecutar el servidor RPC mediante ifconfig y ejecutarlo con ./servido_rpc

3.
Obtener la IP del contenedor donde se va a ejecutar el servidor del servicio mediante ifconfig y ejecutar el servidor mediante el comando ./servidor -p <puerto> -s <IP del servidor RPC>

4.
Ejecutar el publisher en background mediante el comando java servicio.ServicioPublisher & en cada contenedor que vaya a ejecutar el cliente.

5.

Ejecutar los clientes que se considero oportuno mediante el comando java -cp . client -s <IP del servidor> -p <Puerto>
