#include <RPC/encryption.h>

EVP_PKEY* generateRSAKey() {
    EVP_PKEY* pkey = EVP_PKEY_new();
    if (!pkey) {
        return NULL;
    }

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (!ctx) {
        EVP_PKEY_free(pkey);
        return NULL;
    }

    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return NULL;
    }

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return NULL;
    }

    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        return NULL;
    }

    EVP_PKEY_CTX_free(ctx);
    return pkey;
}

X509* generateCertificate(EVP_PKEY* pkey) {
    X509* x509 = X509_new();
    if (!x509) {
        return NULL;
    }

    if (!X509_set_version(x509, 2)) {
        X509_free(x509);
        return NULL;
    }

    if (!ASN1_INTEGER_set(X509_get_serialNumber(x509), 0)) {
        X509_free(x509);
        return NULL;
    }

    if (!X509_gmtime_adj(X509_get_notBefore(x509), 0)) {
        X509_free(x509);
        return NULL;
    }

    if (!X509_gmtime_adj(X509_get_notAfter(x509), 31536000L)) { // 1 an
        X509_free(x509);
        return NULL;
    }

    if (!X509_set_pubkey(x509, pkey)) {
        X509_free(x509);
        return NULL;
    }

    X509_NAME* name = X509_get_subject_name(x509);
    if (!name ||
        !X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (unsigned char*)"US", -1, -1, 0) ||
        !X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (unsigned char*)"MyOrganization", -1, -1, 0) ||
        !X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (unsigned char*)"localhost", -1, -1, 0)) {
        X509_free(x509);
        return NULL;
    }

    if (!X509_set_issuer_name(x509, name)) {
        X509_free(x509);
        return NULL;
    }

    if (!X509_sign(x509, pkey, EVP_sha256())) {
        X509_free(x509);
        return NULL;
    }

    return x509;
}
