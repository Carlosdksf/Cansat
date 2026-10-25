clc;
close all;
clear all;

% Configuración del puerto serial
port = 'COM11'; % Cambia esto por el puerto serial de tu Arduino
baudRate = 9600;

% Crear el objeto serial
s = serial(port, 'BaudRate', baudRate, 'Terminator', 'LF');

% Intentar abrir el puerto serial con manejo de errores
try
    fopen(s);
    disp('Puerto serial abierto con éxito');
catch ME
    error('Error al abrir el puerto serial: %s', ME.message);
end

% Inicialización de variables para el gráfico

figure;

% Subplots (dividimos la figura en 2x2 ventanas)
titles = {'Temperatura (°C)', 'Presión (hPa)', 'Altitud (m)', 'Humedad (%)'};
h = zeros(1, 4);
for i = 1:4
    subplot(2, 2, i);
    h(i) = plot(NaN, NaN);
    title(titles{i});
    xlabel('Tiempo (s)');
    switch i
        case 1, ylabel(titles{i}); ylim([20 40]);
        case 2, ylabel(titles{i}); ylim([750 900]);
        case 3, ylabel(titles{i}); ylim([1250 1800]);
        case 4, ylabel(titles{i}); ylim([0 100]);
    end
    grid on;
end

% Tiempo inicial
startTime = datetime('now');

% Crear archivos CSV nuevos con nombres únicos
index = 1;
while true
    sensorFileName = sprintf('sensores_datos_%d.csv', index);
    gpsFileName = sprintf('gps_datos_%d.csv', index);
    if ~exist(sensorFileName, 'file') && ~exist(gpsFileName, 'file')
        break;
    end
    index = index + 1;
end

% Escribir encabezados
fidSensor = fopen(sensorFileName, 'w');
fprintf(fidSensor, 'Fecha,Hora,Minuto,Segundo,AM/PM,Temperatura (°C),Presión (hPa),Altitud (m),Humedad (%%)\n');
fclose(fidSensor);

fidGPS = fopen(gpsFileName, 'w');
fprintf(fidGPS, 'Fecha,Hora,Minuto,Segundo,AM/PM,Latitud,Longitud\n');
fclose(fidGPS);

% Buffers para almacenar los datos
dataBuffer = [];
timeBuffer = [];
locationBuffer = [];

% Variables de control
stopFlag = false;
pauseFlag = false;

% Crear botones en la figura
uicontrol('Style', 'pushbutton', 'String', 'Stop', ...
    'Position', [20, 0, 100, 30], 'Callback', @(src, event) setStopFlag());
uicontrol('Style', 'pushbutton', 'String', 'Pause', ...
    'Position', [250, 0, 100, 30], 'Callback', @(src, event) setPauseFlag());
uicontrol('Style', 'pushbutton', 'String', 'Continue', ...
    'Position', [450, 0, 100, 30], 'Callback', @(src, event) setContinueFlag());

try
    while ~stopFlag
        if ~pauseFlag
            % Leer línea de datos del puerto serial
            dataLine = fgetl(s);

            % Separar los datos CSV
            data = sscanf(dataLine, '%f,%f,%f,%f,%f,%f');

            if length(data) == 6
                dataInt = round(data(1:4));
                latitude = data(5);
                longitude = data(6);

                elapsedTime = seconds(datetime('now') - startTime);
                dt = datetime('now');

                fecha = datestr(dt, 'dd-mm-yyyy');
                hora = str2double(datestr(dt, 'hh'));
                minuto = str2double(datestr(dt, 'MM'));
                segundo = str2double(datestr(dt, 'ss'));
                ampm = datestr(dt, 'AM');

                dataBuffer = [dataBuffer; dataInt'];
                timeBuffer = [timeBuffer; elapsedTime];
                locationBuffer = [locationBuffer; latitude, longitude];

                % Guardar datos del sensor
                fidSensor = fopen(sensorFileName, 'a');
                fprintf(fidSensor, '%s,%02d,%02d,%02d,%s,%d,%d,%d,%d\n', ...
                    fecha, hora, minuto, segundo, ampm, dataInt(1), dataInt(2), dataInt(3), dataInt(4));
                fclose(fidSensor);

                % Guardar datos GPS
                fidGPS = fopen(gpsFileName, 'a');
                fprintf(fidGPS, '%s,%02d,%02d,%02d,%s,%.6f,%.6f\n', ...
                    fecha, hora, minuto, segundo, ampm, latitude, longitude);
                fclose(fidGPS);

                % Actualizar gráficos
                for i = 1:4
                    subplot(2, 2, i);
                    set(h(i), 'XData', timeBuffer, 'YData', dataBuffer(:, i));
                end

                drawnow limitrate;

                fprintf('Ubicación: Latitud %.6f, Longitud %.6f\n', latitude, longitude);
            end
        else
            pause(0.1);
        end
    end

    disp('Lectura de datos finalizada');

catch ME
    disp('Se produjo un error: ');
    disp(ME.message);
end

% Cerrar el puerto serial
try
    fclose(s);
    delete(s);
    clear s;
    disp('Puerto serial cerrado');
catch ME
    disp('Error al cerrar el puerto serial:');
    disp(ME.message);
end

% Funciones de control
function setStopFlag()
    assignin('base', 'stopFlag', true);
end

function setPauseFlag()
    assignin('base', 'pauseFlag', true);
end

function setContinueFlag()
    assignin('base', 'pauseFlag', false);
end
