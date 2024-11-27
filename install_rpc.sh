#!/bin/bash

# Locațiile de instalare
LIB_DIR="/usr/local/lib"
INCLUDE_DIR="/usr/local/include/RPC"
PKG_CONFIG_DIR="/usr/local/lib/pkgconfig"

# Verifică dacă scriptul este rulat cu privilegii de root
if [ "$(id -u)" -ne 0 ]; then
    echo "Eroare: Trebuie să rulezi acest script cu privilegii de root (sudo)."
    exit 1
fi

# Verifică dacă fișierele necesare sunt prezente
if [ ! -f "libRPC.so" ]; then
    echo "Eroare: Fișierul libRPC.so nu a fost găsit. Asigură-te că ai compilat biblioteca."
    exit 1
fi

if [ ! -d "include/RPC" ]; then
    echo "Eroare: Directorul include/RPC nu a fost găsit. Asigură-te că fișierele de antet sunt prezente."
    exit 1
fi

# Creează directoarele de destinație dacă nu există
echo "Creare directoare de destinație..."
mkdir -p ${LIB_DIR}
mkdir -p ${INCLUDE_DIR}
mkdir -p ${PKG_CONFIG_DIR}

# Copiază biblioteca dinamică
echo "Copiere libRPC.so în ${LIB_DIR}..."
cp libRPC.so ${LIB_DIR}

# Copiază fișierele de antet
echo "Copiere fișiere de antet în ${INCLUDE_DIR}..."
cp -r include/RPC/* ${INCLUDE_DIR}/

# Creează fișierul pkg-config pentru a permite utilizarea corectă în proiecte
echo "Creare fișierul pkg-config pentru libRPC..."
cat <<EOL > ${PKG_CONFIG_DIR}/libRPC.pc
prefix=${LIB_DIR}
exec_prefix=\${prefix}
libdir=\${exec_prefix}/lib
includedir=\${prefix}/include

Name: libRPC
Description: Biblioteca RPC pentru aplicații client-server
Version: 1.0
Libs: -L\${libdir} -lRPC
Cflags: -I\${includedir}/RPC
EOL

# Actualizează cache-ul bibliotecilor dinamice
echo "Actualizare cache biblioteci dinamice..."
ldconfig

# Mesaj final
echo "Biblioteca RPC a fost instalată cu succes în sistem!"

# Oferă instrucțiuni de utilizare pentru utilizatori
echo "Pentru a utiliza biblioteca în proiecte, adăugați opțiunile corespunzătoare în CMake sau Makefile:"
echo "  - CMake: target_link_libraries(proiect libRPC)"
echo "  - Makefile: -L/usr/local/lib -lRPC -I/usr/local/include"
