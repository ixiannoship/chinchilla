# chinchilla
CHINCHILLA V1.0 | Telegram @krujiente

Accesorio para el cargador Trydan de V2C | Grupo Telegram t.me/v2c_trydan

Visualiza información relativa a diversas cargas y producción fotovoltaica

No es necesario ningún sistema intermedio de terceros, como por ejemplo Home Assitant, app, etc..

Toda la información se obtiene directamente via JSON del Trydan 

El Trydan a su vez toma los valores del Shelly, pinzas V2C, inversor u otro medidor configurado en el Trydan. Si no están bien configurados los datos se mostrarán mal

Basado en Arduino, placa esp32 Lilygo/TTGO T-display de 1,14 pulgadas y sus librerías necesarias

Alimentación: 5V por USB | batería de 3,3V en el socket habilitado | 3,3V ó 5V a través de los pines de la placa

Activada de serie la actualización de firmware vía OTA (Over The Air, WIFI) como alternativa a USB


Por orden muestra:

-Producción fotovoltaica | No es necesaria una instalación FV

-Consumo de la casa

-Consumo del Trydan | blanco: coche sin manguera conectada; verde: manguera conectada; azul: suministrando al coche

-Balance en ese instante; rojo: importando de la red; verde: exportando excedentes

-Suma del consumo casa + Trydan

-Total de energía suministrada al coche en esa sesión de carga

-Duración de la sesión de carga | Falta por encontrar la lógica que emplea

* La sesión se reinicia bien desconectando la manguera o comenzando a cargar después de una pausa | falta averiguar exáctamente la lógica que emplea

-IP de la chinchilla

-Versión del Firmware del Trydan


#INSTALACION y REQUISITOS

Instalación de Arduino IDE:

Se recomienda ver este vídeo, a partir del minuto 2:29 https://www.youtube.com/watch?v=b8254--ibmM&t=438s

La línea para ponder instalar la biblioteca TFT_eSPI es: https://dl.espressif.com/dl/package_esp32_index.json

Escoger la placa ES32 Dev Module en lugar de la del video.

Si no las tenemos, instalar las librerias ArduinoOTA y ArduinoJson

Para ello en Arduino IDE, vamos al menú Tools -> Manage Libraries... y las buscamos e instalamos.

Descargamos el .zip de la chinchilla y extraemos la carpeta chinchillaV1 en el PC /Documentos/Arduino

La ruta debería quedar así: \Documentos\Arduino\cinchillaV1\ y aquí todos los archivos

CONFIGURACION DE LA CHINCHILLA

Hacemos doble click en el archivo chinchillaV1.ide o el que tenga como icono el logo de Arduino

Se muestra el código y lo configuramos para poder funcionar con nuestra red WIFI y Trydan

- El Trydan y la chinchilla DEBEN estar en la misma subred, aunque estén en diferentes WIFIS debido a repetidores o PLCs.

- Ejemplo BIEN: Trydan 192.168.1.40 y chinchilla 192.168.1.156 | Ejemplo MAL: Trydan 192.168.1.40 y chinchilla 192.168.0.156. Ojo con el 0 y el 1

- SE RECOMIENDA ENCARECIDAMENTE conectar tanto el Trydan como la chinchilla a una red WIFI 2,4 Ghz.
  
- NO SE RECOMIENDA redes mixtas 2,4 y 5 Ghz con el mismo nombre, puede ser el origen de posibles problemas de conexión y volvernos locos porque ni la chinchilla ni/o el Trydan conectan o pierden la señal. 

- NO CONECTAR a redes de 5 Ghz, ni el Trydan ni la chinchilla.

SUBIR SOFTWARE A LA PLACA

Conectamos la placa por USB y abrimos el Administrador de Dispositivos de Windows. Vemos el puerto COM al que se ha conectado, generalmente el último. Ese será el puerto que debemos indicar en Ardunino IDE en Tools -> Ports y lo elegimos.

Si da error de comunicación, habría que ir probando los que muestra windows en el Administrador de dispositivos. Un truco es conectar y desconectar la placa para ver cual aparece y desaparece.

Un vez configurado el puerto ya lo tenemos todo y podecemos a subir el firmware a la placa, para ello pulsamos el botón UPLOAD, icono del medio de los tres que hay arriba a la derecha en Arduino IDE.

Esperamos que compile y lo suba a la placa. Si todo ha ido bien debería estar funcionando ;)
