#ifndef ENCRYPTION_H
#define ENCRYPTION_H
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

EVP_PKEY* generateRSAKey();

X509* generateCertificate(EVP_PKEY* pkey);

#endif