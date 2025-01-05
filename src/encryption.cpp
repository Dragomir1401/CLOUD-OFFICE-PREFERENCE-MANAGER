#include "mbedtls/aes.h"
#include <Arduino.h>

// AES Key and IV
const unsigned char aesKey[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                  0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
unsigned char iv[16] = {0x00};

// Helper: Pad PKCS#7
String padPKCS7(const String &data)
{
    size_t padding = 16 - (data.length() % 16);
    String paddedData = data;
    for (size_t i = 0; i < padding; i++)
    {
        paddedData += (char)padding;
    }
    return paddedData;
}

// Helper: Unpad PKCS#7
String unpadPKCS7(const String &data)
{
    size_t padding = data[data.length() - 1];
    return data.substring(0, data.length() - padding);
}

// Helper: Base64 encode
String base64Encode(const uint8_t *data, size_t len)
{
    static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    String result;
    int i = 0;
    uint32_t bit_stream = 0;
    for (size_t j = 0; j < len; j++)
    {
        bit_stream = (bit_stream << 8) | data[j];
        i += 8;
        while (i >= 6)
        {
            result += base64_chars[(bit_stream >> (i - 6)) & 0x3F];
            i -= 6;
        }
    }
    if (i > 0)
    {
        result += base64_chars[(bit_stream << (6 - i)) & 0x3F];
    }
    while (result.length() % 4 != 0)
    {
        result += '=';
    }
    return result;
}

// Helper: Base64 decode
String base64Decode(const String &data)
{
    static const uint8_t base64_table[256] = {
        /* Initialize all values to 255 (invalid) */
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 62, 255, 255, 255, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 255, 255, 255, 255, 255, 255,
        255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 255, 255, 255, 255, 255,
        255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 255, 255, 255, 255, 255};
    String result;
    uint32_t bit_stream = 0;
    int i = 0;
    for (char c : data)
    {
        if (c == '=')
        {
            break;
        }
        uint8_t val = base64_table[(uint8_t)c];
        if (val == 255)
        {
            continue;
        }
        bit_stream = (bit_stream << 6) | val;
        i += 6;
        if (i >= 8)
        {
            result += (char)((bit_stream >> (i - 8)) & 0xFF);
            i -= 8;
        }
    }
    return result;
}

// Encrypt function
String encryptAES(const String &plaintext)
{
    // Apply PKCS#7 padding
    String paddedPlaintext = padPKCS7(plaintext);
    size_t len = paddedPlaintext.length();
    unsigned char encryptedData[len];

    // Initialize AES context
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, aesKey, 128);

    // Encrypt using CBC mode
    unsigned char ivCopy[16];
    memcpy(ivCopy, iv, 16); // Make a copy of the IV
    mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, len, ivCopy, (unsigned char *)paddedPlaintext.c_str(), encryptedData);
    mbedtls_aes_free(&aes);

    // Encode encrypted data to Base64
    return base64Encode((const uint8_t *)encryptedData, len);
}

String decryptAES(const String &ciphertext)
{
    // Decode Base64 input
    String decodedCiphertext = base64Decode(ciphertext);
    size_t len = decodedCiphertext.length();
    unsigned char decryptedData[len];

    // Initialize AES context
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_dec(&aes, aesKey, 128);

    // Decrypt using CBC mode
    unsigned char ivCopy[16];
    memcpy(ivCopy, iv, 16); // Make a copy of the IV
    mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, len, ivCopy, (unsigned char *)decodedCiphertext.c_str(), decryptedData);
    mbedtls_aes_free(&aes);

    // Remove PKCS#7 padding
    String decryptedPlaintext = String((char *)decryptedData);
    return unpadPKCS7(decryptedPlaintext);
}
