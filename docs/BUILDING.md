# 🔨 Guía de Compilación Detallada

Instrucciones completas para compilar ScanTailor en diferentes plataformas.

---

## 🖥️ Requisitos previos

### Por plataforma

#### **Linux (Debian/Ubuntu)**
```bash
# Dependencias
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    qtbase5-dev \
    qttools5-dev \
    libboost-all-dev \
    zlib1g-dev \
    libpng-dev \
    libjpeg-dev \
    libopenjp2-7-dev

# Verificar versiones
cmake --version      # >= 3.20
g++ --version        # >= 11
```

#### **macOS**
```bash
# Con Homebrew
brew install cmake qt boost zlib libpng jpeg openjpeg

# Verificar
cmake --version
g++ --version
```

#### **Windows (MSVC)**
```
Visual Studio 2022 con C++ toolkit
CMake 3.20+
Qt 5.15+ (descargado de qt.io)
Boost 1.70+ (compilado o pre-compilado)
```

---

## 📥 Descarga del código

```bash
# Clonar repositorio
git clone https://github.com/Di4l/scantailor.git
cd scantailor

# Usar rama de desarrollo (C++23 + wxWidgets futura)
git checkout upgrade

# Verificar rama actual
git branch
```

---

## 🔧 Compilación básica

### Paso 1: Crear directorio de build

```bash
mkdir -p build
cd build
```

### Paso 2: Configurar con CMake

```bash
# Configuración por defecto (Release)
cmake ..

# O con flags específicos
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_COMPILER=g++
```

### Paso 3: Compilar

```bash
# Con todos los núcleos disponibles
make -j$(nproc)

# O con específica cantidad de núcleos
make -j4

# Con output detallado (para debugging)
make VERBOSE=1
```

### Paso 4: Instalar (opcional)

```bash
sudo make install

# O con prefijo personalizado
cmake .. -DCMAKE_INSTALL_PREFIX=/opt/scantailor
make install
```

---

## ⚙️ Configuraciones avanzadas

### Debug vs Release

```bash
# Debug (con símbolos, más lento, para desarrollo)
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
./bin/scantailor

# Release (optimizado, rápido, para distribución)
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=native"
make -j$(nproc)
./bin/scantailor
```

### C++23 (rama upgrade)

```bash
cmake .. \
    -DCMAKE_CXX_STANDARD=23 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON \
    -DCMAKE_CXX_COMPILER=g++-12  # o g++-13
```

### Con Ninja (más rápido que Make)

```bash
cmake .. -G Ninja
ninja

# O en paralelo
ninja -j$(nproc)
```

---

## 🧹 Limpiar y recompilar

### Limpieza parcial

```bash
cd build
make clean
```

### Limpieza completa

```bash
cd ..
rm -rf build
mkdir build && cd build
cmake ..
make -j$(nproc)
```

---

## ✅ Verificar compilación

### Ejecutar aplicación

```bash
# Desde directorio de build
./bin/scantailor

# O desde root del proyecto (si instalada)
scantailor
```

### Ejecutar tests

```bash
cd build
make test

# O con output detallado
ctest --output-on-failure
```

### Verificar dependencias

```bash
# Listar librerías enlazadas
ldd ./bin/scantailor

# En macOS
otool -L ./bin/scantailor
```

---

## 🐛 Solucionar problemas de compilación

### Error: "Qt5 not found"

```bash
# Instalar Qt5
sudo apt-get install qtbase5-dev qttools5-dev

# O establecer ruta manualmente
cmake .. -DQt5_DIR=/usr/lib/cmake/Qt5
```

### Error: "boost not found"

```bash
# Instalar Boost
sudo apt-get install libboost-all-dev

# O especificar ruta
cmake .. -DBOOST_ROOT=/usr/include/boost
```

### Error: "CMake version too old"

```bash
# Actualizar CMake
sudo apt-get install cmake

# O descargar versión nueva
wget https://github.com/Kitware/CMake/releases/download/v3.25.0/cmake-3.25.0-linux-x86_64.tar.gz
tar xzf cmake-*.tar.gz
export PATH=$(pwd)/cmake-*/bin:$PATH
```

### Error: "compiler not found"

```bash
# Instalar GCC
sudo apt-get install build-essential g++-11

# Especificar compiler
cmake .. -DCMAKE_CXX_COMPILER=g++-11
```

### Error: "linker error: undefined reference"

```bash
# Problema: librería no enlazada
# Solución 1: Compilar en Release (optimizaciones)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Solución 2: Agregar librería manualmente en CMakeLists.txt
target_link_libraries(scantailor PRIVATE boost_system)

# Solución 3: Limpiar y recompilar
cd ..
rm -rf build && mkdir build && cd build
cmake .. && make -j$(nproc)
```

---

## 📦 Compilación para distribución

### Linux - Crear tarball

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local
make -j$(nproc)
make install DESTDIR=./scantailor-package

# Crear tarball
tar czf ../scantailor-linux-x64.tar.gz -C ./scantailor-package .
```

### macOS - Crear DMG

```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.13
make -j$(nproc)
make install

# Crear DMG (requiere herramientas adicionales)
hdiutil create -volname "ScanTailor" -srcfolder bin -ov -format UDZO scantailor.dmg
```

### Windows - Crear instalador

```cmd
cd build
cmake .. -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release -j 4

# Usar NSIS para crear instalador
"C:\Program Files (x86)\NSIS\makensis.exe" ../cmake/scantailor.nsi
```

---

## 🔄 Recompilación incremental

Durante desarrollo, es más rápido recompilar solo lo que cambió:

```bash
cd build

# Después de editar un archivo .cpp o .h
make -j$(nproc)

# Mucho más rápido que compilación limpia
# (típicamente 5-30 segundos vs 2-5 minutos)
```

---

## 🚀 Con IDE

### Qt Creator

```bash
# Qt Creator abre CMakeLists.txt directamente
qtcreator CMakeLists.txt &

# O desde Qt Creator:
# File → Open File or Project → CMakeLists.txt
```

Ventajas:
- Debugging gráfico
- Autocompletado
- Integrated terminal

### Visual Studio Code

```bash
# Instalar extensiones CMake Tools y C++

# Abre proyecto
code .

# Selecciona kit (compiler)
# Presiona Ctrl+Shift+B para compilar
```

### Visual Studio (Windows)

```bash
# Generar solución de VS
cd build
cmake .. -G "Visual Studio 17 2022"

# Abre .sln generado
start scantailor.sln
```

---

## 📊 Monitoreo de compilación

### Ver tiempo de compilación

```bash
# Medir tiempo total
time make -j$(nproc)

# Ver qué se está compilando
make VERBOSE=1 | grep "Building\|Linking"
```

### Detectar bottlenecks

```bash
# CMake con timing
cmake --build . --verbose

# Ver archivos más lentos de compilar
make -j1 VERBOSE=1 2>&1 | grep -oE '[^ ]+ *$' | sort | uniq -c | sort -rn | head
```

---

## 🔍 Verificación final

```bash
# 1. Verificar ejecutable existe
ls -lh ./bin/scantailor

# 2. Verificar ejecutable es válido
file ./bin/scantailor

# 3. Ejecutar
./bin/scantailor

# 4. Verificar en proceso
ps aux | grep scantailor

# 5. Ver librerías dinámicas (si aplica)
ldd ./bin/scantailor

# 6. Ejecutar tests
make test
```

---

## 📋 Checklist de compilación exitosa

- [ ] CMake configura sin errores
- [ ] Make compila sin errores (solo warnings aceptables)
- [ ] Ejecutable se crea en `build/bin/scantailor`
- [ ] Aplicación lanza sin crashes
- [ ] Tests pasan (`make test`)
- [ ] Enlaza todas las dependencias correctamente
- [ ] Modo Debug compila en <5 minutos
- [ ] Modo Release compila en <10 minutos

---

## 🆘 Soporte

Si tienes problemas de compilación:

1. **Verifica requisitos**: `BUILDING.md` (este archivo)
2. **Consulta logs**: `build/CMakeFiles/CMakeOutput.log`
3. **Busca en issues**: GitHub Issues del proyecto
4. **Abre issue nuevo**: Con output completo del error

---

**Última actualización**: 1 de febrero de 2026  
**Versión C++**: 20 (C++23 en rama upgrade)
