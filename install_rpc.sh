#!/bin/bash

# Definește directoarele de instalare
LIB_DIR="/usr/local/lib"
INCLUDE_DIR="/usr/local/include"
PKG_CONFIG_DIR="/usr/local/lib/pkgconfig"

# Verifică dacă scriptul este rulat cu permisiuni de root
if [ "$EUID" -ne 0 ]; then
  echo "Te rog rulează scriptul cu permisiuni de root."
  exit 1
fi

# Verifică și instalează protobuf dacă nu este instalat
if ! dpkg -s protobuf-compiler libprotobuf-dev >/dev/null 2>&1; then
  echo "Protobuf nu este instalat. Instalare protobuf..."
  apt-get update
  apt-get install -y protobuf-compiler libprotobuf-dev
else
  echo "Protobuf este deja instalat."
fi

# Verifică și instalează openssl dacă nu este instalat
if ! dpkg -s openssl libssl-dev >/dev/null 2>&1; then
  echo "OpenSSL nu este instalat. Instalare OpenSSL..."
  apt-get update
  apt-get install -y openssl libssl-dev
else
  echo "OpenSSL este deja instalat."
fi

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