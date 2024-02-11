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

* Es posible acceder a la chinchilla para consultar los datos en navegador. el formato es http://ipchinchilla:8080

* Notifica via bot de Telegram de las posibles desconexiones del Trydan.

Hay que tener en cuenta que las desconexiones pueden deberse a una red wifi defectuosa o mal configurada, o bien a un mal funcionamiento del Trydan.

Ver el archivo instalacion.txt para configurar correctamente la chinchilla.

Si te ha gustado la chinchilla, quieres agradecerlo y te vas a comprar un Tesla, puedes usar mi referido :)

https://www.tesla.com/referral/vctormanuel88854
