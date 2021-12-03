/******************************************************************************
* File Name:   mqtt_client_config.h
*
* Description: This file contains all the configuration macros used by the
*              MQTT client in this example.
*
* Related Document: See README.md
*
*
*******************************************************************************
* Copyright 2020-2021, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/

#ifndef MQTT_CLIENT_CONFIG_H_
#define MQTT_CLIENT_CONFIG_H_

#include "cy_mqtt_api.h"

/*******************************************************************************
* Macros
********************************************************************************/

/***************** MQTT CLIENT CONNECTION CONFIGURATION MACROS *****************/
/* MQTT Broker/Server address and port used for the MQTT connection. */
//#define MQTT_BROKER_ADDRESS               "192.168.1.5"
#define MQTT_BROKER_ADDRESS "45d10f894eec440fb4b40c5d42f407ac.s1.eu.hivemq.cloud"

#define MQTT_PORT                         8883

/* Set this macro to 1 if a secure (TLS) connection to the MQTT Broker is  
 * required to be established, else 0.
 */
#define MQTT_SECURE_CONNECTION            ( 1 )

/* Configure the user credentials to be sent as part of MQTT CONNECT packet */
#define MQTT_USERNAME                     "WeatherStation1"
#define MQTT_PASSWORD                     "WeatherStation1"


/********************* MQTT MESSAGE CONFIGURATION MACROS **********************/
/* The MQTT topics to be used by the publisher ctand subscriber. */
#define MQTT_PUB_TOPIC                    "ledstatus"
#define MQTT_SUB_TOPIC                    "ledstatus"

/* Set the QoS that is associated with the MQTT publish, and subscribe messages.
 * Valid choices are 0, 1, and 2. Other values should not be used in this macro.
 */
#define MQTT_MESSAGES_QOS                 ( 1 )

/* Configuration for the 'Last Will and Testament (LWT)'. It is an MQTT message 
 * that will be published by the MQTT broker if the MQTT connection is 
 * unexpectedly closed. This configuration is sent to the MQTT broker during 
 * MQTT connect operation and the MQTT broker will publish the Will message on 
 * the Will topic when it recognizes an unexpected disconnection from the client.
 * 
 * If you want to use the last will message, set this macro to 1 and configure
 * the topic and will message, else 0.
 */
#define ENABLE_LWT_MESSAGE                ( 0 )
#if ENABLE_LWT_MESSAGE
    #define MQTT_WILL_TOPIC_NAME          MQTT_PUB_TOPIC "/will"
    #define MQTT_WILL_MESSAGE             ("MQTT client unexpectedly disconnected!")
#endif

/* MQTT messages which are published on the MQTT_PUB_TOPIC that controls the
 * device (user LED in this example) state in this code example.
 */
#define MQTT_DEVICE_ON_MESSAGE            "TURN ON"
#define MQTT_DEVICE_OFF_MESSAGE           "TURN OFF"


/******************* OTHER MQTT CLIENT CONFIGURATION MACROS *******************/
/* A unique client identifier to be used for every MQTT connection. */
#define MQTT_CLIENT_IDENTIFIER            "Rocky"

/* The timeout in milliseconds for MQTT operations in this example. */
#define MQTT_TIMEOUT_MS                   ( 5000 )

/* The keep-alive interval in seconds used for MQTT ping request. */
#define MQTT_KEEP_ALIVE_SECONDS           ( 60 )

/* Every active MQTT connection must have a unique client identifier. If you 
 * are using the above 'MQTT_CLIENT_IDENTIFIER' as client ID for multiple MQTT 
 * connections simultaneously, set this macro to 1. The device will then
 * generate a unique client identifier by appending a timestamp to the 
 * 'MQTT_CLIENT_IDENTIFIER' string. Example: 'psoc6-mqtt-client5927'
 */
#define GENERATE_UNIQUE_CLIENT_ID         ( 1 )

/* The longest client identifier that an MQTT server must accept (as defined
 * by the MQTT 3.1.1 spec) is 23 characters. However some MQTT brokers support 
 * longer client IDs. Configure this macro as per the MQTT broker specification. 
 */
#define MQTT_CLIENT_IDENTIFIER_MAX_LEN    ( 23 )

/* As per Internet Assigned Numbers Authority (IANA) the port numbers assigned 
 * for MQTT protocol are 1883 for non-secure connections and 8883 for secure
 * connections. In some cases there is a need to use other ports for MQTT like
 * port 443 (which is reserved for HTTPS). Application Layer Protocol 
 * Negotiation (ALPN) is an extension to TLS that allows many protocols to be 
 * used over a secure connection. The ALPN ProtocolNameList specifies the 
 * protocols that the client would like to use to communicate over TLS.
 * 
 * This macro specifies the ALPN Protocol Name to be used that is supported
 * by the MQTT broker in use.
 * Note: For AWS IoT, currently "x-amzn-mqtt-ca" is the only supported ALPN 
 *       ProtocolName and it is only supported on port 443.
 * 
 * Uncomment the below line and specify the ALPN Protocol Name to use this 
 * feature.
 */
// #define MQTT_ALPN_PROTOCOL_NAME           "x-amzn-mqtt-ca"
/* Server Name Indication (SNI) is extension to the Transport Layer Security 
 * (TLS) protocol. As required by some MQTT Brokers, SNI typically includes the 
 * hostname in the Client Hello message sent during TLS handshake.
 * 
 * Uncomment the below line and specify the SNI Host Name to use this extension
 * as specified by the MQTT Broker.
 */
#define MQTT_SNI_HOSTNAME                 "xxxxxxxxxx.s1.eu.hivemq.cloud"

/* A Network buffer is allocated for sending and receiving MQTT packets over 
 * the network. Specify the size of this buffer using this macro.
 * 
 * Note: The minimum buffer size is defined by 'CY_MQTT_MIN_NETWORK_BUFFER_SIZE' 
 * macro in the MQTT library. Please ensure this macro value is larger than 
 * 'CY_MQTT_MIN_NETWORK_BUFFER_SIZE'.
 */
#define MQTT_NETWORK_BUFFER_SIZE          ( 2 * CY_MQTT_MIN_NETWORK_BUFFER_SIZE )

/* Maximum MQTT connection re-connection limit. */
#define MAX_MQTT_CONN_RETRIES            (150u)

/* MQTT re-connection time interval in milliseconds. */
#define MQTT_CONN_RETRY_INTERVAL_MS      (2000)


/**************** MQTT CLIENT CERTIFICATE CONFIGURATION MACROS ****************/

/* Configure the below credentials in case of a secure MQTT connection. */
/* PEM-encoded client certificate */
#define CLIENT_CERTIFICATE      \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDhTCCAm0CFDFdb1o0mwhdxklDdQEwH7WxuRa6MA0GCSqGSIb3DQEBCwUAMHgx\n" \
"CzAJBgNVBAYTAkJFMRAwDgYDVQQIDAdMaW1idXJnMRAwDgYDVQQHDAdIYXNzZWx0\n" \
"MQswCQYDVQQKDAJVSDELMAkGA1UECwwCRUExDTALBgNVBAMMBFBTT0MxHDAaBgkq\n" \
"hkiG9w0BCQEWDXRlc3RAdGVzdC5jb20wHhcNMjExMDI3MjAxODUwWhcNMjIxMDI3\n" \
"MjAxODUwWjCBhTELMAkGA1UEBhMCQkUxEDAOBgNVBAgMB0xpbWJ1cmcxEDAOBgNV\n" \
"BAcMB0hhc3NlbHQxFTATBgNVBAoMDFVIRGllcGVuYmVlazELMAkGA1UECwwCRUEx\n" \
"DjAMBgNVBAMMBVJvY2t5MR4wHAYJKoZIhvcNAQkBFg90ZXN0M0B0ZXN0My5jb20w\n" \
"ggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDQEQ3BUAneE4snAKrYJBW3\n" \
"TW52xSayrmByKtQI0E+akFMTsumzexmpUIm5biQ8mspMBBMA/ULBuWWUHRyBE1cg\n" \
"orUvpZoHVTLZXBMT2aq8bjyF3OGNVatsZygX4976YI3UAdJjydDT+qNwjGq2hF/k\n" \
"qnx34a7FvkkBGtWVpF7P7S/kmez2N41lShrCmzUSuY4ZhhLqVhLgcab4NluP54pA\n" \
"gSNv2ypWDOCKinbf7voHKaZVF2eHx59lzRjDgH70WXvXISE9lbjJY60//FhFe962\n" \
"HqhTKhzzYM0kR+A7KBUCW43RKroEHbQu+o6rtZkaXcWCoPCyW2ksFc8vNBPEaUCP\n" \
"AgMBAAEwDQYJKoZIhvcNAQELBQADggEBAIcdC+FUweZQIxd33C+W4ZEr294L7WJH\n" \
"75TssXfO67WCZR0Br1HyXrSy3ESdOCETJq01UJyRfOuUTX4vYDQMazb6jzdtzb+A\n" \
"GYACcT51RjNTqVcE8ghjEZ0ZkUbdVyNam0HRrpItS14rOYTSqyRlZwSXxMJ7XXaw\n" \
"Bt+qThu0+QmoD83RtDM9ULVJJssuuILj4KA8iPsmMGjdSgtzW+bg+kT0JrDDt4vs\n" \
"WyJmdAoF1NTuOLEppHBgr8C8X+UMBBWKiyxW0vPoVw6uE8PinY0nM/hgVBre19id\n" \
"xJantB0Cp6H9RZscskqd2Z0ySlZv0k0VpOhxkojVX8+Zk+9GrLRLEFg=\n" \
"-----END CERTIFICATE-----"

/* PEM-encoded client private key */
#define CLIENT_PRIVATE_KEY          \
"-----BEGIN RSA PRIVATE KEY-----\n" \
"MIIEpgIBAAKCAQEA0BENwVAJ3hOLJwCq2CQVt01udsUmsq5gcirUCNBPmpBTE7Lp\n" \
"s3sZqVCJuW4kPJrKTAQTAP1CwblllB0cgRNXIKK1L6WaB1Uy2VwTE9mqvG48hdzh\n" \
"jVWrbGcoF+Pe+mCN1AHSY8nQ0/qjcIxqtoRf5Kp8d+Guxb5JARrVlaRez+0v5Jns\n" \
"9jeNZUoawps1ErmOGYYS6lYS4HGm+DZbj+eKQIEjb9sqVgzgiop23+76BymmVRdn\n" \
"h8efZc0Yw4B+9Fl71yEhPZW4yWOtP/xYRXveth6oUyoc82DNJEfgOygVAluN0Sq6\n" \
"BB20LvqOq7WZGl3FgqDwsltpLBXPLzQTxGlAjwIDAQABAoIBAQCa9Jhx59Hxekyf\n" \
"93dFEVq68QfbnGh9mjBp1kujZMBkNCgq+4vhB0TNPBXV65lBKWaPxlkA25zj+dcU\n" \
"v7mCx4yT2ZlmqsZTv7fm+ROcLPdi7hZU+3NfdvX7/bd2U1a00UPPKKwvdRX5SSh/\n" \
"oZO36k4gVnfsMD+hNV3ZXMrEb23NG1b3Chn4Ggj3Qgece4X/7i2odWQ+M9VV7qJA\n" \
"yElJBDAWKFsTfIRMVXBhTKh+jpOI5rO6P5nnjvFLWcTklog/BYlMX8xfz1oZwBip\n" \
"ZEgmEm43r/yd4c2tLF9oJQFoaP95zTpkQ+xwphwZdvSAdjLuM4jkUfHbw3ZHMMI8\n" \
"EsznPgJBAoGBAPxGJACUNZ0L5j+3aA9y5LYpLTkakIPIe8ztjMh4R8+JTI5+80fW\n" \
"zvJXpFcDSpCx93n2wTBE+7XZocHvSVlE2L43hfIlonDERtR/O2GauUIE48sqkvL5\n" \
"MrW04VjSLnMGHJdzl/PpeuGsD8fIWmXsqvojGGHf6HEpVy5pRCahYwG1AoGBANMj\n" \
"w1W70tqQotgShsLKJcF0jE++RhtblGB8UdMd7mYx+qDLsGoRAMUAGbnsYdn1AtwX\n" \
"6O3rLc0g3FObTJsE/NuSt/b+oPH0G3MIhZ+hAwpoT/KHDS4CxWsfUSm552wtcNTK\n" \
"F3LfaqeL1rBGSNhv63zS/k7i9Byrjl28uLOOeDOzAoGBAJWWzdBeEfB+1tgfXCAV\n" \
"A9OUVRGQU/Mg1XAtyBYzZHmGs5OnaYDGuk3vdC54cCWesd1rWSDTNuKdGxLRXhAG\n" \
"B/O7Hrz4Xfs6F7n3e/GjeojQN3cIVrym0c/3Ysx1TT+9dwTDcW4bm1bn3S+c+SWJ\n" \
"EdVjLBkVWVKIrf6CQvwZJIklAoGBALVy2Y6XMFh2zguEuF00HghH4cEyEsu2Ayh8\n" \
"DlGxOkyHDQ0cn0o3vIKQWIuIUntd7Qw6XAoC/0R2xgHvx4C9Y5RLXV+dabElCGXu\n" \
"X+9UDunlxTb2yuPMq94odra2NeSolHu6BTbKO70rFuA8NqQSYrzOEpC2x/TPljSG\n" \
"V/CvMiuzAoGBALKfHeU9uKr3BYcLIF2BqFuxWnW3b18cFQIptDOyj8TFuiT2MdxI\n" \
"CWz5HYlw/qiF3iq4YBXLX47+JFWPuIDV32DJd0swEAd3kne1mSPYbqI2ssNCBPWP\n" \
"dZGrytCm8Zck2tpB19Yql7yYkFSlxZ08XAJ3AIqyDKxNX94902laq34D\n" \
"-----END RSA PRIVATE KEY-----"

/* PEM-encoded Root CA certificate */
#define ROOT_CA_CERTIFICATE     \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n" \
"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
"DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n" \
"PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n" \
"Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
"AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n" \
"rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n" \
"OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n" \
"xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n" \
"7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n" \
"aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n" \
"HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n" \
"SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n" \
"ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n" \
"AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n" \
"R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n" \
"JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n" \
"Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n" \
"-----END CERTIFICATE-----"
/* "MIID0TCCArmgAwIBAgIUOGxX+nLS/vdj6uVedI3QlGMhYx4wDQYJKoZIhvcNAQEL\n" \
"BQAweDELMAkGA1UEBhMCQkUxEDAOBgNVBAgMB0xpbWJ1cmcxEDAOBgNVBAcMB0hh\n" \
"c3NlbHQxCzAJBgNVBAoMAlVIMQswCQYDVQQLDAJFQTENMAsGA1UEAwwEUFNPQzEc\n" \
"MBoGCSqGSIb3DQEJARYNdGVzdEB0ZXN0LmNvbTAeFw0yMTEwMjcyMDEyMTRaFw0y\n" \
"MjEwMjcyMDEyMTRaMHgxCzAJBgNVBAYTAkJFMRAwDgYDVQQIDAdMaW1idXJnMRAw\n" \
"DgYDVQQHDAdIYXNzZWx0MQswCQYDVQQKDAJVSDELMAkGA1UECwwCRUExDTALBgNV\n" \
"BAMMBFBTT0MxHDAaBgkqhkiG9w0BCQEWDXRlc3RAdGVzdC5jb20wggEiMA0GCSqG\n" \
"SIb3DQEBAQUAA4IBDwAwggEKAoIBAQDClAlmwl4iCbDWvNtXE5b4fe7NGt+Bo4I2\n" \
"6oeARp+bvU0ijSci0ldqwziGF0UyOBf0hzvje1qsWqadoSsy1e+cSJTdONatSHFQ\n" \
"JcZBZJiYzniAZgVKykpMoPAKZMTOrobty0UWb2cU6VkLOzaSaPav/cbEsJ0eZDjN\n" \
"djjH1aV69NGIxTQkq9gJqxtSynFw1IoYEcjYLGKlHu8ZfC+Uv2RDOHrvr0ykaYkW\n" \
"AnR+LwzYxle4gFPtmeDn74WSpWBt03cUtCLgo6WZMQBny+j+Swth04n+csJ2qpix\n" \
"SVNHUdGYkE1yI1mJuKuayuHo8MAMgUnpUb5SEVLfg1v7bYfg6yYdAgMBAAGjUzBR\n" \
"MB0GA1UdDgQWBBQzD1gzl6Snx6pqa3h+BALpjPlWBzAfBgNVHSMEGDAWgBQzD1gz\n" \
"l6Snx6pqa3h+BALpjPlWBzAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUA\n" \
"A4IBAQDCNm4y56Apl7mYCEvE0USJa+WLTYJRnP6OjnYJQlrITRNKj4b8iiIEzB2q\n" \
"SXnBe5zFJ1Q2xnm7Xkxc0ekBghoRmd1DWeHBl9r5vorfBS82sae6tmbHCJj0shIp\n" \
"XlCH7kPV9Pc4w4I2oE+1/Tz6KJ8S55Yj+2WFERTczJ82rg8QBSorXXMLpS2/QOkv\n" \
"ufJy7WpyaMdfE9S8wQ1EqQtnSSdp3o9bK7tb05dTHvVRk932Jzby9DJ7W71wLNrt\n" \
"+soOvtNklRvdaBjMQjAvmpDVAj6bENdOoHP4enKkIcYem6g5Tm2cHNr/h8fG0H3r\n" \
"eAo2eG4aC4L9j5can+8impyNeqFs\n" \ */



/******************************************************************************
* Global Variables
*******************************************************************************/
extern cy_mqtt_broker_info_t broker_info;
extern cy_awsport_ssl_credentials_t  *security_info;
extern cy_mqtt_connect_info_t connection_info;


#endif /* MQTT_CLIENT_CONFIG_H_ */
