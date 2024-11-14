# RPC - Remote Procedure Call

Pentru utilizarea acestei biblioteci, va trebui să importați biblioteca RPC din cadrul proiectului (libRPC.so) și să rulați ulterior acest script
 ```bash
 !/bin/bash

# Define the installation directory
INSTALL_DIR="/usr/local/rpc_lib"

# Create the installation directory
mkdir -p $INSTALL_DIR

# Copy the library files to the installation directory
cp path/to/RPC/lib/libRPC.so $INSTALL_DIR

# Copy the header files to the installation directory
cp -r path/to/RPC/include $INSTALL_DIR

# Add the library path to LD_LIBRARY_PATH
echo "export LD_LIBRARY_PATH=$INSTALL_DIR:\$LD_LIBRARY_PATH" >> ~/.bashrc

# Reload the .bashrc file to apply changes
source ~/.bashrc

echo "RPC library installed successfully."
```

Salvează scriptul de instalare într-un fișier, de exemplu: install_rpc_lib.sh.

Fă scriptul executabil: 
```bash
chmod +x install_rpc_lib.sh
```
Rulează scriptul de instalare: 
```bash 
./install_rpc_lib.sh
```
Da, poți crea un script de instalare care să automatizeze procesul de linkare și configurare a variabilelor de mediu. Iată un exemplu de script de instalare în Bash:

```bash
#!/bin/bash

# Define the installation directory
INSTALL_DIR="/usr/local/rpc_lib"

# Create the installation directory
mkdir -p $INSTALL_DIR

# Copy the library files to the installation directory
cp path/to/RPC/lib/libRPC.so $INSTALL_DIR

# Copy the header files to the installation directory
cp -r path/to/RPC/include $INSTALL_DIR

# Add the library path to LD_LIBRARY_PATH
echo "export LD_LIBRARY_PATH=$INSTALL_DIR:\$LD_LIBRARY_PATH" >> ~/.bashrc

# Reload the .bashrc file to apply changes
source ~/.bashrc

echo "RPC library installed successfully."
```

### Pași pentru utilizare:

1. **Creează scriptul de instalare**:
   Salvează scriptul de mai sus într-un fișier, de exemplu `install_rpc_lib.sh`.

2. **Fă scriptul executabil**:
   ```sh
   chmod +x install_rpc_lib.sh
   ```

3. **Rulează scriptul de instalare**:
   ```sh
   ./install_rpc_lib.sh
   ```

Acest script va copia biblioteca și fișierele header în directorul "/usr/local/rpc_lib", va adăuga calea bibliotecii la variabila de mediu "LD_LIBRARY_PATH" și va reîncărca fișierul ".bashrc" pentru a aplica modificările.

Acum, proiectul tău ar trebui să poată găsi automat biblioteca RPC și să o linkeze corect.


