g++ -g /home/vasile/Desktop/Remote-procedure-call-/Server/main_server.cpp   /home/vasile/Desktop/Remote-procedure-call-/RPC/src/proto/procedure_format.pb.cc \
    -I /home/vasile/Desktop/Remote-procedure-call-/RPC/include            \
    -I /home/vasile/Desktop/Remote-procedure-call-/RPC/src/proto          \
    -o /home/vasile/Desktop/Remote-procedure-call-/Server/main_server     \
    -L /home/vasile/Desktop/Remote-procedure-call-/build/lib/      \
    -lRPC -Wl,-rpath,/home/vasile/Desktop/Remote-procedure-call-/build/lib/ \
    -lprotobuf -lssl -lcrypto