# COMPILACIÓN

Para compilar primero es necesario que instalemos las librerías de **libmicrohttpd** y **prometheus-client-c**. Para ello, seguiremos los siguientes pasos:

### 1. Instalar libmicrohttpd
```bash
sudo apt-get install -y libmicrohttpd-dev
```
### 2. Instalar prometheus-client-c (prom y promhttp)
Seguimos los siguientes comandos:

```bash
git clone https://github.com/digitalocean/prometheus-client-c.git
cd prometheus-client-c
rm -rf promhttp  # Esta carpeta es removida porque al estar modificada será instalada luego con CMake
cd prom
mkdir build
cd build
cmake ..
make
sudo make install
```
En este punto tendremos instalado tanto libmicrohttpd como prom. La instalación de promhttp se hará luego con el archivo CMake.

### 3. Instalar Conan
Conan se usará para la gestión de las dependencias de cJSON y Unity. Procedemos con los siguientes comandos:

```bash
sudo apt-get update
sudo apt-get install -y python3-pip
pip3 install conan
```
Nos dirigimos a la raíz del repositorio y ejecutamos:

```bash
conan install ./ --output-folder=build --build=missing --settings=build_type=Debug
```
Este último comando utilizará el conanfile.txt que se encuentra en la raíz del repositorio para gestionar las dependencias de cJSON y Unity. Se generará una carpeta llamada build.

### 4. Configuración con CMake
Nos movemos a la carpeta build y ejecutamos:

```bash
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=./build/Debug/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug -DRUN_COVERAGE=1 -DENABLE_CJSON_TEST=OFF
```
Este comando tomará el archivo CMakeLists.txt, que a su vez utilizará los otros CMakeLists.txt dentro del submódulo y la carpeta tests, para generar un archivo Makefile. Al ejecutarlo, se generarán los binarios correspondientes en la carpeta build y subcarpetas correspondientes.

### 5. Compilación final
Finalmente, ejecutamos dentro de la carpeta build:

```bash
make -j$(nproc)
```
Si todo siguió su curso correctamente, en la carpeta build encontraremos:

1. El binario ShellProject.
2. Una subcarpeta con el nombre del submódulo que contendrá otro binario: METRICAS.
3. Una subcarpeta tests que contendrá el binario TEST (opcional para pruebas).

# Ejecución de la Shell
Con el binario ShellProject ejecutamos nuestra shell.
La shell es capaz de soportar comandos internos como externos (cd, echo, ls, etc.)
Dentro de ella, usamos el comando:
```bash
monitor --start
```
Este comando ejecutará el binario del monitor.
tambien tenemos mas comandos relacionados al monitor, como 
```bash
monitor --stop  #para el monitor
monitor --status #nos da el estado actual del monitor
monitor --restart  #restea el monitor cargandole de nuevo las configuraciones
monitor --changecfg #cambia el cJSON segun el input del usuario
```
