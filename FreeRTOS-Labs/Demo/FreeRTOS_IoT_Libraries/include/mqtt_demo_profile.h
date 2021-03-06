/*
 * FreeRTOS Kernel V10.3.0
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
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
 * Each profile corresponds to a different service. The demos that use the same
 * profile share the same service. It is important to understand the
 * correspondence between the preconfigured profiles and demos, since each
 * service have different connection configuration and credential validation.
 *
 *  - mqtt_demo_profile.h (current) is preconfigured to test.mosquitto.org MQTT
 * broker. The file is used with mqtt_plain_text demo and
 * mqtt_basic_tls_server_auth demo. Feel free to try out other broker with the
 * demos.
 *
 *  - https_demo_profile.h is preconfigured to httpbin.org server. The file is
 * used with http_plain_text demo and https_basic_tls_server_auth demo.
 *
 *  - aws_iot_demo_profile.h contains information to connect to AWS
 * IoT. The file is used with mqtt_tls_mutual_auth demo, https_tls_mutual_auth
 * demo, and other AWS IoT related demo.
 */

#ifndef MQTT_DEMO_PROFILE_H
#define MQTT_DEMO_PROFILE_H

#include "demo_config.h"

/**
 * @brief Details of the MQTT broker to connect to.
 *
 * The Mosquitto test server provides a freely available MQTT server that can
 * be used to test MQTT clients.
 */
#define mqttdemoprofileBROKER_ENDPOINT    "test.mosquitto.org"

/**
 * @brief The port to use for the demo.
 *
 * If trying to connect to Mosquitto test broker (the default broker), use
 * port 1883 for unencrypted MQTT traffic and port 8883 for encrypted MQTT
 * traffic. These are also standard MQTT ports, but consult broker
 * setting for the actual port.
 */
#if ( democonfigENABLE_TLS == 1 )
    #define mqttdemoprofileBROKER_PORT    8883
#else
    #define mqttdemoprofileBROKER_PORT    1883
#endif

/**
 * @brief The topic to subscribe and publish to in the example.
 *
 * The topic starts with the client identifier to ensure that each demo interacts
 * with a unique topic.
 */
#define mqttdemoprofileCLIENT_IDENTIFIER    "mqttexampleclient"

#endif /* ifndef MQTT_DEMO_PROFILE_H */
