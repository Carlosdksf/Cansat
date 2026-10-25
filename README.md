# CanSat
Un CanSat es una simulación de un satélite real, integrado en el volumen y la forma de una lata de refresco común. 

Misión: consiste recrear el funcionamiento de un satélite real en un dispositivo de
tamaño reducido, capaz de recopilar información y ejecutar tareas de forma
autónoma durante su trayecto. Esto significa que debe llevar a cabo su labor sin
intervención desde tierra, y al concluir, enviar los datos obtenidos y descender con
ayuda de un paracaídas.

# Secuencia de eventos de la misión

1. Preparación y configuración: Antes del lanzamiento, se realizan las
pruebas funcionales del CanSat, asegurando que todos los sensores,
sistemas de comunicación y mecanismos de liberación estén correctamente
configurados. El dispositivo es instalado y asegurado en el dron para su
elevación.

2. Enganche y elevación: El CanSat es transportado por un dron, el cual lo
eleva progresivamente hasta alcanzar una altitud de 400 metros sobre el
nivel del mar. Durante este ascenso, el sistema permanece en espera hasta
alcanzar el punto programado de liberación.

3. Liberación automática: Al llegar a la altitud establecida, se activa
automáticamente el mecanismo de liberación del CanSat.
Simultáneamente, se despliega el paracaídas, iniciando la fase de
descenso.

4. Descenso: El CanSat desciende de forma estable gracias al paracaídas,
mientras ejecuta de manera autónoma las tareas programadas, como la
recolección y almacenamiento de datos.

5. Impacto: Una vez finalizado el descenso, el CanSat aterriza suavemente
en tierra firme, completando la operación aérea.

6. Recuperación: El equipo localiza y recupera el dispositivo para proceder al
análisis de los datos recolectados y evaluar el funcionamiento del sistema a
lo largo de toda la misión

# Componentes 

- Sensor (BME280)

Recoge datos de temperatura, Altitud a nivel del mar, humedad y presión atmosférica.

- Módulo GPS

Permite saber la ubicación del CanSat en todo momento, proporcionando latitud y
longitud, lo cual facilita su seguimiento y recuperación al aterrizar.

- Microcontrolador Principal (Arduino Nano)

Es el "cerebro" del sistema. Se encarga de leer los sensores, activar el servomotor
cuando se alcanza la altitud, y enviar los datos en tiempo real a una estación base.

- Módulo de comunicación Principal (NRF24L01 con antena)

Permite la transmisión inalámbrica del microcontrolador a la estación base. La
antena ayuda a mantener una conexión estable durante el vuelo y la caída.

- Sistema de alimentación (Batería 9V/450 mAh)

Proporciona energía eléctrica a todos los módulos. El regulador asegura que cada
parte reciba el voltaje necesario sin sobrecargas.

- Servomotor de liberación de paracaídas

Está programado para activar el despliegue del paracaídas justo cuando se
alcanza la altura establecida. Este mecanismo permite que el CanSat descienda
de forma controlada.

- Buzzer

Una vez que el CanSat llega a cierta altura, el buzzer emite un sonido que ayuda a
encontrarlo fácilmente, sobre todo si aterriza en un lugar con vegetación o poco
visible.

# Documentacion 

Cansat.pdf

Documentacion Final.pdf
