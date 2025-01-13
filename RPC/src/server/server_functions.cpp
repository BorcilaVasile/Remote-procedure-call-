#include <RPC/rpc_server.h>

RPC::ReturnValue Server::sayHello(std::vector<RPC::Argument> args){
    RPC::ReturnValue value;
    if(args.size()!=1){
        value.set_status(RPC::Status::INVALID_ARGUMENT);
        value.set_message("Invalid function call. Too many arguments");
        return value;
    }
    
    if(!args[0].has_string_val()){
        value.set_status(RPC::Status::INVALID_ARGUMENT);
        value.set_message("Invalid type argument");
        return value;
    }
    value.set_status(RPC::Status::OK);
    value.set_string_result("Hello "+args[0].string_val());
    return value; 
}

RPC::ReturnValue Server::returnTypeName(std::vector<RPC::Argument> args){
    RPC::ReturnValue value;
    if(args.size()>1){
        value.set_status(RPC::Status::INVALID_ARGUMENT);
        value.set_message("Invalid function call. Too many arguments");
        return value;
    }

    std::string type=typeid(args[0].value_case()).name();

    value.set_status(RPC::Status::OK);
    value.set_string_result("The pointer has the type "+type);
    return value;
}

RPC::ReturnValue Server::multiplyMatrix(std::vector<RPC::Argument> args){
    RPC::ReturnValue value;
    if(args.size()!=3)
    {
        value.set_status(RPC::Status::INVALID_ARGUMENT);
        value.set_message("Invalid function call. Too many arguments.");
        return value;
    }

    if(!args[0].has_int_val() || !args[1].has_matrix() || !args[2].has_matrix())
    {
        value.set_status(RPC::Status::INVALID_ARGUMENT);
        value.set_message("Invalid function call. Invalid type of the arguments");
        return value;
    }

    int dimension=args[0].int_val();
    int** A=convertFromMatrix(args[1].matrix());
    int** B=convertFromMatrix(args[2].matrix());

    int** result = new int*[dimension];
    for (int i = 0; i < dimension; i++) {
        result[i] = new int[dimension];
        for (int j = 0; j < dimension; j++) {
            result[i][j] = 0; 
        }
    }

    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            for (int k = 0; k < dimension; k++) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    value.set_status(RPC::Status::OK);
    *value.mutable_matrix_result() = convertToMatrix(result, dimension);

    for (int i = 0; i < dimension; i++) {
        delete[] A[i];
        delete[] B[i];
        delete[] result[i];
    }
    delete[] A;
    delete[] B;
    delete[] result;
    
    return value;
}

 int** Server::convertFromMatrix(const RPC::Matrix& matrix) {
        int dimension = matrix.dimension();
        int** array = new int*[dimension];
        for (int i = 0; i < dimension; ++i) {
            array[i] = new int[dimension];
            const RPC::Row& row = matrix.rows(i);
            for (int j = 0; j < row.int_val_size(); ++j) {
                array[i][j] = row.int_val(j);
            }
        }
        return array;
}

RPC::Matrix Server::convertToMatrix(int** array, int dimension) {
        RPC::Matrix matrix;
        matrix.set_dimension(dimension);

        for (int i = 0; i < dimension; ++i) {
            RPC::Row* row = matrix.add_rows();
            for (int j = 0; j < dimension; ++j) {
                row->add_int_val(array[i][j]);
            }
        }

        return matrix;
}

RPC::ReturnValue Server::open(std::vector<RPC::Argument> args){
    RPC::ReturnValue value;
    if(args.size()>3){
        value.set_status(RPC::Status::INVALID_ARGUMENT);
        value.set_message("Invalid function call. Too many arguments");
        return value;
    }

    if(args.size()<2){
        value.set_status(RPC::Status::INVALID_ARGUMENT);
        value.set_message("Invalid function call. Insuficient arguments.");
        value.set_error_result(EINVAL);
        return value; 
    }
    
    if(!args[0].has_string_val()){
        value.set_status(RPC::Status::INVALID_ARGUMENT);
        value.set_message("Invalid type for pathname.");
        return value;
    }

    if(!args[1].has_int_val()){
        value.set_status(RPC::Status::INVALID_ARGUMENT);
        value.set_message("Invalid type for flags.");
        return value;
    }

    if(args.size()==3 && !args[3].has_int_val()){
        value.set_status(RPC::Status::INVALID_ARGUMENT);
        value.set_message("Invalid type for mode.");
        return value;
    }
    
    const std::string& path = args[0].string_val();
    int flags = args[1].int_val();


    mode_t mode = (args.size() == 3) ? args[2].int_val() : 0644;
    std::cout << "Path: " << path 
          << ", Flags: " << flags 
          << ", Mode (octal): " << std::oct << mode << std::endl;

    int result =::open(path.c_str(), flags, mode);
    if (result == -1) {

        std::cout<<"Error opening file: "<<std::strerror(errno)<<std::endl;
        value.set_status(RPC::Status::ERROR);
        value.set_message(std::strerror(errno));
        value.set_error_result(errno);
    } else {
        value.set_status(RPC::Status::OK);
        value.set_int_result(result);
    }
    return value;
}

RPC::ReturnValue Server::close(std::vector<RPC::Argument> args){
    RPC::ReturnValue value;
    if(args.size()!=1)
    {
        value.set_status(RPC::Status::ERROR);
        value.set_message("Invalid number of arguments");
        return value;
    }

    if(!args[0].has_int_val()){
        value.set_status(RPC::Status::ERROR);
        value.set_message("Invalid type of the argument");
        return value;
    }

    int result= ::close(args[0].int_val());
    if (result == -1) {
        value.set_status(RPC::Status::ERROR);
        value.set_message(std::strerror(errno));
        value.set_error_result(errno); // Setează errno pentru eroarea specifică
        return value;
    }

    value.set_status(RPC::Status::OK);
    value.set_message("File closed succesfully");
    value.set_int_result(result);

    return value;
}

RPC::ReturnValue Server::read(std::vector<RPC::Argument> args){
    RPC::ReturnValue value; 
    if(args.size()!=3){
        value.set_status(RPC::Status::ERROR);
        value.set_message("Invalid number of arguments");
        return value;
    }

    if(!args[0].has_int_val()) 
    {
        value.set_status(RPC::Status::ERROR);
        value.set_message("Invalid type of the argument. The first argument should be an integer");
        return value;
    }
    if(!args[1].has_string_val()) 
    {
        value.set_status(RPC::Status::ERROR);
        value.set_message("Invalid type of the argument. The second argument should be an string");
        return value;
    }
    if(!args[2].has_int_val()) 
    {
        value.set_status(RPC::Status::ERROR);
        value.set_message("Invalid type of the argument. The third argument should be an integer");
        return value;
    }

    int fd = args[0].int_val();
    std::string buffer = args[1].string_val();
    int size = args[2].int_val();

    std::cout<<"Descriptor value: "<<fd<<std::endl;
    std::cout<<"Buffer value: "<<buffer<<std::endl;
    std::cout<<"Size value: "<<size<<std::endl;

    off_t position = lseek(fd, 0, SEEK_SET);
    if (position == -1) {
        std::cerr << "Error setting file cursor to the beginning: " << std::strerror(errno) << std::endl;
    } else {
        std::cout << "Cursor set to the beginning of the file." << std::endl;
    }

    position = lseek(fd, 0, SEEK_CUR);
    std::cout << "Current file position: " << position << std::endl;
    
    char* result = new char[size];
    int bytesRead = ::read(fd, result, size);



    if (bytesRead == -1) {
        value.set_status(RPC::Status::ERROR);
        value.set_message(std::strerror(errno));
        value.set_int_result(bytesRead);
        value.set_error_result(errno);
        delete[] result;
        return value;
    }

    std::cout<<"Text: "<<result<<std::endl;

    value.set_status(RPC::Status::OK);
    value.set_int_result(bytesRead);
    value.set_string_result(result);
    delete[] result;
    return value;
}

RPC::ReturnValue Server::write(std::vector<RPC::Argument> args){
        RPC::ReturnValue value; 
    if(args.size()!=3){
        value.set_status(RPC::Status::ERROR);
        value.set_message("Invalid number of arguments");
        return value;
    }

    if(!args[0].has_int_val()) 
    {
        value.set_status(RPC::Status::ERROR);
        value.set_message("Invalid type of the argument. The first argument should be an integer");
        return value;
    }
    if(!args[1].has_string_val()) 
    {
        value.set_status(RPC::Status::ERROR);
        value.set_message("Invalid type of the argument. The second argument should be an string");
        return value;
    }
    if(!args[2].has_int_val()) 
    {
        value.set_status(RPC::Status::ERROR);
        value.set_message("Invalid type of the argument. The third argument should be an integer");
        return value;
    }

    int fd = args[0].int_val();
    std::cout<<std::endl<<"Descriptor value: "<<fd<<std::endl;
    char* buffer =new char[args[1].string_val().size()];
    std::memcpy(buffer,args[1].string_val().c_str(), args[1].string_val().size()); 
    int size = args[2].int_val();

    int bytesWrite = ::write(fd, buffer, size);

    if (bytesWrite == -1) {
        value.set_status(RPC::Status::ERROR);
        value.set_message(std::strerror(errno));
        value.set_int_result(bytesWrite);
        value.set_error_result(errno);
        return value;
    }

    value.set_status(RPC::Status::OK);
    value.set_int_result(bytesWrite);
    return value;
}