#!/bin/bash

# Rulează serverul
"Server/main_server" &
SERVER_PID=$!

# Așteaptă 2 secunde
sleep 2

# Rulează clientul
"Client/main_client"

# Așteaptă pentru a termina serverul
wait $SERVER_PID
