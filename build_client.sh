g++ -g /home/vasile/Desktop/Remote-procedure-call-/Client/main_client.cpp /home/vasile/Desktop/Remote-procedure-call-/RPC/src/proto/procedure_format.pb.cc \
    -I /home/vasile/Desktop/Remote-procedure-call-/RPC/include            \
    -o /home/vasile/Desktop/Remote-procedure-call-/Client/main_client     \
    -L /home/vasile/Desktop/Remote-procedure-call-/build/lib/ \
    -lRPC -Wl,-rpath,/home/vasile/Desktop/Remote-procedure-call-/build/lib/ \
    -lprotobuf 
