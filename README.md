# RPC - Remote Procedure Call

### Documentație

Proiect realizat de Borcilă Vasile  
Grupa C-113D

---

## 1. Introducere

### 1.1 Scopul documentului

Acest document are ca scop definirea specificațiilor tehnice și funcționale pentru dezvoltarea unei biblioteci de Remote Procedure Call (RPC). Biblioteca va permite comunicarea între procese prin apeluri de funcții la distanță, folosind socket-uri, și va suporta atât moduri de operare sincrone cât și asincrone.

### 1.2 Domeniul de aplicare

Proiectul se încadrează în dezvoltarea de sisteme distribuite, având ca obiectiv facilitarea interacțiunii între procese independente prin apeluri de funcții remote. Biblioteca va fi utilizată în medii unde este necesară executarea de operații pe servere sau alte procese distante, asigurând o comunicare eficientă și flexibilă.

### 1.3 Definiții și abrevieri

- **RPC (Remote Procedure Call)** – mecanism prin care un program poate executa o procedură (funcție) într-un alt spațiu de adrese (proces sau server) fără a gestiona detaliile comunicării.
- **Socket** - endpoint pentru comunicarea între două noduri într-o rețea.
- **Client** – procesul care inițiază cererea de apel RPC.
- **Server** - procesul care oferă funcțiile RPC către clienți.
- **Sincron** - modul de operare în care clientul așteaptă răspunsul de la server înainte de continua execuția.
- **Asincron** - modul de operare în care clientul își continuă execuția fără a aștepta răspunsul imediat de la server.

---

## 2. Obiectivele proiectului

### 2.1 Obiectiv general

Dezvoltarea unei biblioteci RPC care să permită efectuarea de apeluri de funcții remote între procese, utilizând socket-uri pentru comunicație și suportând atât moduri sincron cât și asincron de operare.

### 2.2 Obiective specifice

- Implementarea mecanismului de comunicație client-server folosind socket-uri.
- Definirea unui protocol de transmitere a numelui funcției și a parametrilor.
- Implementarea suportului pentru apeluri RPC sincrone și asincrone.
- Gestionarea erorilor și a excepțiilor în procesul de comunicare.
- Optimizarea performanței și a eficienței bibliotecii.
- Documentarea completă a bibliotecii și a procesului de utilizare.

---

## 3. Cerințe funcționale

### 3.1 Funcționalități de bază

1. **Comunicare client-server**:
    - Serverul să fie capabil să asculte pe un port specific și să accepte conexiuni multiple de la clienți.
    - Clientul să poată iniția o conexiune către server și să trimită cereri RPC.

2. **Apeluri RPC sincrone**:
    - Clientul să poată invoca funcții remote și să aștepte răspunsul înainte de a continua execuția.
    - Serverul să proceseze cererea, să execute funcția solicitată și să returneze rezultatul către client.

3. **Apeluri RPC asincrone**:
    - Clientul să poată invoca funcții remote și să continue execuția fără a aștepta răspunsul de la server imediat.
    - Implementarea mecanismelor de callback sau threading pentru gestionarea răspunsurilor asincrone.

4. **Gestionarea erorilor**:
    - Detectarea și gestionarea erorilor precum funcții inexistente, parametri incorecți sau probleme de conexiune.
    - Transmiterea mesajelor de eroare adecvate de la server la client.

### 3.2 Funcționalități avansate

1. **Serializare avansată**:
    - Utilizarea unor formate eficiente de serializare/deserializare, cum ar fi Protocol Buffers.

2. **Securitate**:
    - Implementarea autentificării între client și server.
    - Criptarea datelor transmise folosind SSH sau alte metode de securizare a comunicației.

3. **Logare și monitorizare**:
    - Implementarea unui sistem de logare.
    - Monitorizarea apelurilor RPC și a erorilor.

4. **Compatibilitate multi-limbaj**:
    - Suport pentru apeluri RPC între procese scrise în diferite limbaje de programare.

---

## 4. Cerințe non-funcționale

### 4.1 Performanță

- Timpul de răspuns să fie minimizat.
- Gestionarea unui număr mare de cereri simultane fără degradarea performanței.

### 4.2 Scalabilitate

- Arhitectura să permită extinderea ușoară a numărului de funcții RPC sau a numărului de clienți și servere.

### 4.3 Securitate

- Asigurarea confidențialității și integrității datelor transmise între client și server.
- Implementarea mecanismelor de autentificare și autorizare.

### 4.4 Ușurința în utilizare

- API-ul bibliotecii să fie ușor de utilizat și bine documentat.
- Documentație clară și exemple de utilizare.

---

## 5. Arhitectură și design

### 5.1 Diagramă de arhitectură

![Basic RPC Architecture](RPC.png)


### 5.2 Componente principale

1. **Client RPC**
    - Stabilește o conexiune către server folosind socket-uri.
    - Reutilizează conexiunea pentru mai multe apeluri RPC, dacă e cazul.
    - Creează mesajul RPC cu numele funcției și parametrii.
    - Inițializează apeluri RPC.
    - Gestionează răspunsurile în modul sincron, respectiv asincron.

2. **Server RPC**
    - Va avea un registru de funcții predefinite care pot fi apelate de clienți.
    - Ascultă cererile RPC de la clienți.
    - Recepționează cererile de la clienți.
    - Verifică existența funcției și validează parametrii.
    - Execută funcțiile solicitate și trimite răspunsurile.

3. **Protocol de comunicare** (prin intermediul socket-urilor)
    - Formatul mesajelor
    - Apel RPC
    - Răspuns RPC
    - Transmiterea mesajelor folosind un protocol de transport

4. **Mecanismul de sincronizare/asincronizare**
    - Moduri de operare sincrone/asincrone.
    - Utilizarea threading-ului și a altor mecanisme de concurență.

5. **Gestionarea erorilor**
    - Returnarea unui mesaj de eroare dacă funcția solicitată nu există sau parametrii nu sunt valizi.
    - Gestionarea excepțiilor interne și returnarea unor mesaje corespunzătoare.
    - Trimiterea de mesaje de eroare către utilizator în cazul în care RPC a eșuat.
    - Implementarea unor mecanisme de retry sau fallback (opțional).

---

## Bibliografie

- ”Distributed Systems” de Andrew S. Tanenbaum și Marteen Van Steen

## Webografie

- [Remote Procedure Call - Wikipedia](https://en.wikipedia.org/wiki/Remote_procedure_call)
- [Remote Procedure Call - TechTarget](https://www.techtarget.com/searchapparchitecture/definition/Remote-Procedure-Call-RPC)
- [Remote Procedure Call - IBM](https://www.ibm.com/docs/en/aix/7.3?topic=concepts-remote-procedure-call)
- [Serialization - Wikipedia](https://en.wikipedia.org/wiki/Serialization)
- [Serialization FAQ - isocpp](https://isocpp.org/wiki/faq/serialization)

