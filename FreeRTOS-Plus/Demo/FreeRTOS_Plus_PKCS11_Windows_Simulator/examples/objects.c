/*
 * FreeRTOS PKCS #11 V1.0.3
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Standard include. */
#include "stdio.h"

/* PKCS #11 includes. */
#include "iot_pkcs11_config.h"
#include "iot_pkcs11.h"
#include "pkcs11.h"

/* mbed TLS includes. */
#include "mbedtls/pk.h"

/* Demo includes. */
#include "demo_helpers.h"
#include "pkcs11_demos.h"

/* RSA private key that has been generated off the device.
 * This key will be used as an example for importing an object onto the device.
 * This is useful when the device itself cannot create credentials.
 */
#define pkcs11demo_RSA_PRIVATE_KEY                                       \
    ""                                                                   \
    "-----BEGIN RSA PRIVATE KEY-----\n"                                  \
    "MIIEowIBAAKCAQEAwLBla8T5AZ7+Kuvdam5PNv768gBqviiwDSCrcIu2CN9/TAtF\n" \
    "D+euyAB7l56wFb8tZXCXTL8IvkOFsW/dhgkv3MSa1GSfDjSzd9diz8UVdDDtngWr\n" \
    "S4uWGK9k5nNWslAZVdbgGGIsYNLEy3as2WDWHTgq6wUIYLyOMN7Ii2D6LvWYT7bP\n" \
    "DSWVYIHxpac5q6nIlqfmBWJGGseg0xgn5OGX46c5Q1ZbP+UxHwRLEs32Yngntwnq\n" \
    "nmSYigSM4q70/DL3G9n2EXX6HUe6ef0grWZ754BI33V6HU2QFlltxApPTpztwuXy\n" \
    "SRP+JmDyS06A5BB7xjxBWGiTPexvwy9g0eJ8AQIDAQABAoIBABH9pkqMpo8vMB4D\n" \
    "zbWNPRoubnG/9jdxBxcCsN8ePE3RyEnqV3VntVr5KJD8gQZWxj9NZuMEhyV0mnv5\n" \
    "rN5B51UeijJAftVNuF/9HT228gzaMKVrdBXmn4oIfkxCAps9fVAVcvDz4Z7crwEs\n" \
    "QHE/aF5sCx3MZ37YvBH2CxVwZYuSutrYs9mIAGIHPiEKS/LN0yxrOs+lpNic0yqh\n" \
    "dpWpi/IyalpEkpxXuTBu0u8u/cX8b3BUHAyFpKXm4gF/n609CGL1jexonJLbJb2B\n" \
    "I6Bmxdh4/IpufaGUJ3r7oYal0Q3grV7AA28bXI+Mg+Uzz/6I49Tthtn7VPONnTq4\n" \
    "aVM+700CgYEA/tntlWjB//UEDahsvyBmgJOUyeZuYG1asbFKG515rncBBHrI3QqW\n" \
    "78rVVqCQl4w1W0hh3rHmaRC8XneWljGyZ264672wmaoy8T6eOiDiQITfxk3ayhjQ\n" \
    "Yw1IcCvyUzes6RrbKW3qwIFc0qwhMU0t8iofr8plaGktJs+iTtoqbjMCgYEAwY69\n" \
    "RcPbSFrl7rAFg34tZss0nJC65QoGO9eEnX1UA4qW3vKEq6WKRNYrjPu4Cb3cfVQb\n" \
    "4I3pnZDQXUJ8sBX89UGxwlPVEPtQVjyhf9vH3zVTHHgfz1m4P0VD183Bj+mDkQsN\n" \
    "1LuVMaQiZRU2C+yyflJL2n88fJXNBD2+L09F0PsCgYEAq94COIF2W4w8GI3eEKLk\n" \
    "HY/phqNtCoCU6kK6zJbNqde9iuzXKyJqGfq7FvmfqWQ304Cj6BAnZ/GOZ1LU7iND\n" \
    "wjdMZEb/oxHB2ym0gVUN4B773ntAsow7dKFPmb45ZN+mL2oaJP1olDxpgfXtSJdZ\n" \
    "BfhceCvRabGMbqEWP4EnnjsCgYBgzo1nuT+RfrUFnlADpjX69aNIszJmn47imzYe\n" \
    "trIwWz5+H3mvh8oSB7QcfUFczfLbVBflRdUUE3vDSR+nyjeMSuEnOlqUdb2GhCia\n" \
    "FEhxIPCCiqoOQOdf7bqy0pv+6GH/RzIRtgg2ncr+F4/Cgw37FBoQHBcakKnLdz4J\n" \
    "13IpxQKBgAEj3FDco2hHjdmEVCVXTVA+SPhwkTlzaMk/GkS9ncteaJrHgPaTc3i2\n" \
    "PJz7uuDt64hf5O9Zi5ecXsPueG8DN3oIPMoFbo/mA2A4ZBUnB8BDrC+3inYuDRxu\n" \
    "GdWIVVVleMjIDhuhavfzmfBgoir2wQgwnJLPd0vOMDafC6WlcN17\n"             \
    "-----END RSA PRIVATE KEY-----"

/**
 * prvObjectImporting covers how to import a private RSA key that was
 * not generated by the Cryptoki library.
 *
 * Note: This method can also be used with certificates and public keys.
 * This Cryptoki library does not support importing RSA public keys.
 */
static void prvObjectImporting( void );

/**
 * prvObjectGeneration covers how to create a public key and private key pair
 * with Cryptoki defined attributes using C_GenerateKeyPair.
 *
 * Note: The "sign-verify.c" demo has a dependency on the objects created
 * in this function, and will not work without first running this function.
 */
static void prvObjectGeneration( void );

/* Length parameters for importing RSA-2048 private keys. */
#define MODULUS_LENGTH        pkcs11RSA_2048_MODULUS_BITS / 8
#define E_LENGTH              3
#define D_LENGTH              pkcs11RSA_2048_MODULUS_BITS / 8
#define PRIME_1_LENGTH        128
#define PRIME_2_LENGTH        128
#define EXPONENT_1_LENGTH     128
#define EXPONENT_2_LENGTH     128
#define COEFFICIENT_LENGTH    128

/**
 * This function details how to use the PKCS #11 "Object" functions to
 * manage the objects abstracted by cryptoki.
 *
 * http://docs.oasis-open.org/pkcs11/pkcs11-base/v2.40/os/pkcs11-base-v2.40-os.html
 * please consult the standard for more information.
 *
 * The standard has grouped the functions presented in this demo as:
 * Object Management Functions.
 *
 */
void vPKCS11ObjectDemo( void )
{
    configPRINTF( ( "\r\nStarting PKCS #11 Objects Demo.\r\n" ) );

    /* PKCS #11 defines objects as "An item that is stored on a token. May be
     * data, a certificate, or a key." This demo will show how to create objects
     * that are managed by Cryptoki. */
    prvObjectImporting();
    prvObjectGeneration();
    configPRINTF( ( "\r\nFinished PKCS #11 Objects Demo.\r\n" ) );
}

static void prvObjectImporting()
{
    configPRINTF( ( "---------Importing Objects---------\r\n" ) );
    configPRINTF( ( "Importing RSA Private Key...\r\n" ) );

    /* Helper variables and variables that have been covered. */
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE hSession = CK_INVALID_HANDLE;
    CK_SLOT_ID * pxSlotId = 0;
    CK_FUNCTION_LIST_PTR pxFunctionList = NULL;
    CK_BBOOL xTrue = CK_TRUE;
    CK_KEY_TYPE xPrivateKeyType = CKK_RSA;
    CK_OBJECT_CLASS xPrivateKeyClass = CKO_PRIVATE_KEY;
    CK_BYTE pxLabel[] = pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;
    CK_OBJECT_HANDLE xPrivateKeyHandle = CK_INVALID_HANDLE;

    /* mbed TLS variables. */
    int lMbedResult = 0;
    mbedtls_pk_context xMbedPkContext = { 0 };
    mbedtls_rsa_context * pxRsaContext = NULL;

    /* Initialize mbed TLS context. */
    mbedtls_pk_init( &xMbedPkContext );

    vStart( &hSession, &pxSlotId );

    /* Ensure the Cryptoki library has the necessary functions implemented. */
    xResult = C_GetFunctionList( &pxFunctionList );
    configASSERT( xResult == CKR_OK );
    configASSERT( pxFunctionList->C_CreateObject != NULL );

    /* Byte arrays of the various parameters for an RSA private key. This code
     * will be importing a 2048 bit RSA key, and the sizes are hard coded for
     * that value.
     *
     * For further explanation of these variables see:
     * https://en.wikipedia.org/wiki/RSA_(cryptosystem)
     *
     */
    CK_BYTE modulus[ MODULUS_LENGTH + 1 ] = { 0 };
    CK_BYTE e[ E_LENGTH + 1 ] = { 0 };
    CK_BYTE d[ D_LENGTH + 1 ] = { 0 };
    CK_BYTE prime1[ PRIME_1_LENGTH + 1 ] = { 0 };
    CK_BYTE prime2[ PRIME_2_LENGTH + 1 ] = { 0 };
    CK_BYTE exponent1[ EXPONENT_1_LENGTH + 1 ] = { 0 };
    CK_BYTE exponent2[ EXPONENT_2_LENGTH + 1 ] = { 0 };
    CK_BYTE coefficient[ COEFFICIENT_LENGTH + 1 ] = { 0 };

    /* Parse the RSA PEM string using mbed tls. See the mbed TLS documentation,
     * or pk.h for further explanation of this API. */
    lMbedResult = mbedtls_pk_parse_key( &xMbedPkContext,
                                        ( const unsigned char * ) pkcs11demo_RSA_PRIVATE_KEY,
                                        strlen( pkcs11demo_RSA_PRIVATE_KEY ) + 1,
                                        NULL,
                                        0 );
    configASSERT( lMbedResult == 0 );



    /* Export the RSA private key parameters into raw bytes. */
    pxRsaContext = xMbedPkContext.pk_ctx;
    lMbedResult = mbedtls_rsa_export_raw( pxRsaContext,
                                          modulus, MODULUS_LENGTH + 1,
                                          prime1, PRIME_1_LENGTH + 1,
                                          prime2, PRIME_2_LENGTH + 1,
                                          d, D_LENGTH + 1,
                                          e, E_LENGTH + 1 );
    configASSERT( lMbedResult == 0 );

    /* Export Exponent 1, Exponent 2, Coefficient. */
    lMbedResult = mbedtls_mpi_write_binary( ( mbedtls_mpi const * ) &pxRsaContext->DP,
                                            exponent1,
                                            EXPONENT_1_LENGTH + 1 );
    configASSERT( lMbedResult == 0 );

    lMbedResult = mbedtls_mpi_write_binary( ( mbedtls_mpi const * ) &pxRsaContext->DQ,
                                            exponent2,
                                            EXPONENT_2_LENGTH + 1 );
    configASSERT( lMbedResult == 0 );

    lMbedResult = mbedtls_mpi_write_binary( ( mbedtls_mpi const * ) &pxRsaContext->QP,
                                            coefficient,
                                            COEFFICIENT_LENGTH + 1 );
    configASSERT( lMbedResult == 0 );

    /*
     * Now we have created a template of CK_ATTRIBUTEs that describe the structure
     * of the RSA private key we want to create. We will pass this to the Cryptoki
     * library, as well as it's length, to create the described private key on
     * the token.
     *
     * The pointers to the various RSA parameters are incremented by one, in
     * order to remove the 0 padding if it was added, since we use the original
     * length of the RSA parameter.
     *
     */
    CK_ATTRIBUTE xPrivateKeyTemplate[] =
    {
        { CKA_CLASS,            &xPrivateKeyClass, sizeof( CK_OBJECT_CLASS ) },
        { CKA_KEY_TYPE,         &xPrivateKeyType,  sizeof( CK_KEY_TYPE )     },
        { CKA_LABEL,            pxLabel,           sizeof( pxLabel )         },
        { CKA_TOKEN,            &xTrue,            sizeof( CK_BBOOL )        },
        { CKA_SIGN,             &xTrue,            sizeof( CK_BBOOL )        },
        { CKA_MODULUS,          modulus + 1,       MODULUS_LENGTH            },
        { CKA_PRIVATE_EXPONENT, d + 1,             D_LENGTH                  },
        { CKA_PUBLIC_EXPONENT,  e + 1,             E_LENGTH                  },
        { CKA_PRIME_1,          prime1 + 1,        PRIME_1_LENGTH            },
        { CKA_PRIME_2,          prime2 + 1,        PRIME_2_LENGTH            },
        { CKA_EXPONENT_1,       exponent1 + 1,     EXPONENT_1_LENGTH         },
        { CKA_EXPONENT_2,       exponent2 + 1,     EXPONENT_2_LENGTH         },
        { CKA_COEFFICIENT,      coefficient + 1,   COEFFICIENT_LENGTH        }
    };

    configPRINTF( ( "Creating private key with label: %s \r\n",
                    pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) );
    configPRINTF( ( "FreeRTOS_P11_Key.dat has been created in the Visual Studio" \
                    " Solution directory\r\n" ) );

    /* Once the Cryptoki library has finished importing the new RSA private key
     * a CK_OBJECT_HANDLE is associated with it. The application can now use this
     * to refer to the object in following operations.
     *
     * xPrivateKeyHandle in the below example will have it's value modified to
     * be the CK_OBJECT_HANDLE.
     *
     * Compare the hard coded RSA private key, in PEM format, with the DER formatted
     * private key that is created by the Cryptoki library, with the following
     * OpenSSL command:
     * "$ openssl rsa -in FreeRTOS_P11_Key.dat -inform der -text"
     *
     * See this explanation for the difference between the PEM format and the
     * DER format:
     * https://stackoverflow.com/questions/22743415/what-are-the-differences-between-pem-cer-and-der/22743616
     *
     */
    xResult = pxFunctionList->C_CreateObject( hSession,
                                              ( CK_ATTRIBUTE_PTR ) &xPrivateKeyTemplate,
                                              sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                              &xPrivateKeyHandle );
    configASSERT( xResult == CKR_OK );
    configASSERT( xPrivateKeyHandle != CK_INVALID_HANDLE );

    /* Clean up mbed TLS context that was used to parse the RSA key. */
    mbedtls_pk_free( &xMbedPkContext );

    vEnd( hSession, pxSlotId );
    configPRINTF( ( "Finished Importing RSA Private Key.\r\n" ) );
    configPRINTF( ( "---------Finished Importing Objects---------\r\n" ) );
}

static void prvObjectGeneration()
{
    configPRINTF( ( "---------Generating Objects---------\r\n" ) );

    /* Helper variables. */
    CK_RV xResult = CKR_OK;
    CK_SESSION_HANDLE hSession = CK_INVALID_HANDLE;
    CK_SLOT_ID * pxSlotId = 0;
    CK_FUNCTION_LIST_PTR pxFunctionList = NULL;
    CK_BYTE * pxDerPublicKey = NULL;
    CK_ULONG ulDerPublicKeyLength = 0;
    CK_BBOOL xTrue = CK_TRUE;

    /* Specify the mechanism to use in the key pair generation. Mechanisms are
     * previously explained in the "mechanims_and_digests.c" demo. */
    CK_MECHANISM xMechanism =
    {
        CKM_EC_KEY_PAIR_GEN, NULL_PTR, 0
    };

    /* The EC curve used in this demo will be the named EC curve prime256v1.
     * For further explanations of EC Cryptography please see the following:
     * https://en.wikipedia.org/wiki/Elliptic-curve_cryptography
     * https://wiki.openssl.org/index.php/Elliptic_Curve_Cryptography
     */
    CK_BYTE xEcParams[] = pkcs11DER_ENCODED_OID_P256;

    /* Specify the key type to be EC. */
    CK_KEY_TYPE xKeyType = CKK_EC;

    /* Object handles are a token specific identifier for an object. They are
     * used so the application's sessions can specify which object to interact
     * with. Non-zero values are valid, 0 is always invalid, and is defined as
     * CK_INVALID_HANDLE
     *
     * The lifetime of the handle is not necessarily the same as the lifetime of
     * the object.
     */
    CK_OBJECT_HANDLE xPrivateKeyHandle = CK_INVALID_HANDLE;
    CK_OBJECT_HANDLE xPublicKeyHandle = CK_INVALID_HANDLE;


    /* Labels are application defined strings that are used to identify an
     * object. It should not be NULL terminated. */
    CK_BYTE pucPublicKeyLabel[] = { pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS };
    CK_BYTE pucPrivateKeyLabel[] = { pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS };

    /* CK_ATTTRIBUTE's contain an attribute type, a value, and the length of
     * the value. An array of CK_ATTRIBUTEs is called a template. They are used
     * for creating, searching, and manipulating for objects. The order of the
     * template does not matter.
     *
     * In the below template we are creating a public key:
     *      Specify the key type as EC.
     *      The key will be able to verify a message.
     *      Specify the EC Curve.
     *      Assign a label to the object that will be created.
     */
    CK_ATTRIBUTE xPublicKeyTemplate[] =
    {
        { CKA_KEY_TYPE,  NULL /* &xKeyType */, sizeof( xKeyType )              },
        { CKA_VERIFY,    NULL /* &xTrue */,    sizeof( xTrue )                 },
        { CKA_EC_PARAMS, NULL /* xEcParams */, sizeof( xEcParams )             },
        { CKA_LABEL,     pucPublicKeyLabel,    sizeof( pucPublicKeyLabel ) - 1 }
    };

    /* Aggregate initializers must not use the address of an automatic variable. */
    /* See MSVC Compiler Warning C4221 */
    xPublicKeyTemplate[ 0 ].pValue = &xKeyType;
    xPublicKeyTemplate[ 1 ].pValue = &xTrue;
    xPublicKeyTemplate[ 2 ].pValue = &xEcParams;

    /* In the below template we are creating a private key:
     *      The key type is EC.
     *      The key is a token object.
     *      The key will be a private key.
     *      The key will be able to sign messages.
     *      Assign a label to the object that will be created.
     */
    CK_ATTRIBUTE xPrivateKeyTemplate[] =
    {
        { CKA_KEY_TYPE, &xKeyType,          sizeof( xKeyType )               },
        { CKA_TOKEN,    &xTrue,             sizeof( xTrue )                  },
        { CKA_PRIVATE,  &xTrue,             sizeof( xTrue )                  },
        { CKA_SIGN,     &xTrue,             sizeof( xTrue )                  },
        { CKA_LABEL,    pucPrivateKeyLabel, sizeof( pucPrivateKeyLabel ) - 1 }
    };

    vStart( &hSession, &pxSlotId );

    xResult = C_GetFunctionList( &pxFunctionList );
    configASSERT( xResult == CKR_OK );

    configPRINTF( ( "Creating private key with label: %s \r\n",
                    pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) );
    configPRINTF( ( "Creating public key with label: %s \r\n",
                    pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ) );

    /* This function will generate a new EC private and public key pair. You can
     * use " $openssl ec -inform der -in FreeRTOS_P11_Key.dat -text " to see
     * the structure of the keys that were generated.
     */
    xResult = pxFunctionList->C_GenerateKeyPair( hSession,
                                                 &xMechanism,
                                                 xPublicKeyTemplate,
                                                 sizeof( xPublicKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                 xPrivateKeyTemplate,
                                                 sizeof( xPrivateKeyTemplate ) / sizeof( CK_ATTRIBUTE ),
                                                 &xPublicKeyHandle,
                                                 &xPrivateKeyHandle );
    configASSERT( xResult == CKR_OK );
    configPRINTF( ( "FreeRTOS_P11_Key.dat has been created in the Visual Studio" \
                    " Solution directory\r\n" ) );
    configPRINTF( ( "Extracting public key bytes...\r\n" ) );

    /* Export public key as hex bytes and print the hex representation of the
     * public key. */
    vExportPublicKey( hSession,
                      xPublicKeyHandle,
                      &pxDerPublicKey,
                      &ulDerPublicKeyLength );
    vWriteHexBytesToConsole( "Public Key in Hex Format",
                             pxDerPublicKey,
                             ulDerPublicKeyLength );
    configPRINTF( ( "---------Finished Generating Objects---------" ) );
    vEnd( hSession, pxSlotId );
}
