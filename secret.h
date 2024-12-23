#include <pgmspace.h>
 
#define SECRET
#define THINGNAME "demoKelompok2_IoT"                         //change this
 
const char WIFI_SSID[] = "zaki";               //change this
const char WIFI_PASSWORD[] = "ojokdibagi";           //change this
const char AWS_IOT_ENDPOINT[] = "a38wfuktqkjn39-ats.iot.ap-southeast-1.amazonaws.com";       //change this
 
// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";
 
// Device Certificate                                               //change this
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAOKWi8doHhFdiuHOwTFVml9aGU2gMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yNDEyMjExNzQw
NTZaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDcBVegdNJpRSLo7dwm
ft5r6eg4mzum9rzO5KJXHLiAEItkvwm+baMl8r0Urqw3ZzAJWgfZydzPH/7xh1y6
X24CRW9rH1jkAqheVKPgfz1A53MV0e05CrYuSKEh0hNbAyTh7kp5Qic12ENBcToR
6cEfAy37XjDwYk63PVhpqdR5vnUcuHlJjOdl7SXe4MbFnAu6mrpdvmeIJDCm+ieg
nE9fyxYStTQ1Q+QwhFF9Y94QejE56mArlgpOXjB/Q5Hh73ZDUTo49B3oSwRba7FV
VmxH8iz/rSnaCoSnvXPpdAs7KQbaobRWAQz/gn5OlgZYo3zXH21S+NDXNlOriK1H
TQdRAgMBAAGjYDBeMB8GA1UdIwQYMBaAFAgRpKFKlJjXmmTgVxBYfcmhcBeKMB0G
A1UdDgQWBBRP7B3yRqHL3/jyGOI5/nJA0RjHWzAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAZOeukgQCDCZuE7abTSO60kaL
V/O8rBeKkNae3qBuXCTXQUL4P0urw6R17KHEHonzkTYA4QRh2selT3gqZ8pJq/gs
RMu2ij/hAPS+3oefD2oUIg+wgAkaDFxq31TGtQlpvndy1S68fl3zPEnuUQGB40e7
00MgMbUAhyGkNt569/erSwR52E1Ftw+MyKYM2OVX09CXr/P2WCB20sqSvKvLoZv4
fMujm5zsCTLHvNfyxviKi8xP2mVKVFx+5VIq7lE/oXWCoqppVmpJ/OnSYSTp1xOR
NyPaCxQLHm4JXcb3zfvv/8kOOhQf/+RJUkuw6srtjihViZFoQU9BxJkCviosaQ==
-----END CERTIFICATE-----
 
 
)KEY";
 
// Device Private Key                                               //change this
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEA3AVXoHTSaUUi6O3cJn7ea+noOJs7pva8zuSiVxy4gBCLZL8J
vm2jJfK9FK6sN2cwCVoH2cnczx/+8Ydcul9uAkVvax9Y5AKoXlSj4H89QOdzFdHt
OQq2LkihIdITWwMk4e5KeUInNdhDQXE6EenBHwMt+14w8GJOtz1YaanUeb51HLh5
SYznZe0l3uDGxZwLupq6Xb5niCQwpvonoJxPX8sWErU0NUPkMIRRfWPeEHoxOepg
K5YKTl4wf0OR4e92Q1E6OPQd6EsEW2uxVVZsR/Is/60p2gqEp71z6XQLOykG2qG0
VgEM/4J+TpYGWKN81x9tUvjQ1zZTq4itR00HUQIDAQABAoIBACrih6/o1VJbfXB1
XGHMfMH/95mA+GGU2jxhaBsep/+eOe1LJzUM3a354Ez1tu+dl0lbWQhkRjvA+spN
Hg/dpe2fZxY0J/lZY0Pvr3mEk4XhFxd7sYdaxSPEWFPlRIyK0lePQMjd0hK2snVB
ApaXP2MyFXZXZ6aTItCojmEaDyYjUtix9WF7we2OUcMQMDbIz0DSCKxmBgj/ue1L
h1+18E4QUfQqwvgDoPN9ZkINiiGTxKJ31l4xNzjaEZS19ysj01OfVx7YtUDLCqNb
DD15jltEzIe58b5i0/rZryxqKyFo7DGc3jqQW7kkgN71dHN54SJBOBYBTYLUBNn6
pCMcl6ECgYEA8yV0XO3eUktOxZgMi4fzDnjpmm7ynQpBe+rGr7fyz4WKcq5sU9JQ
DGfQUQ1VTKLQFlm4sH2enJ2Ur1mLizVYzrhLyfwCc5xhpSoxiES8gLcIgCmG+99r
NL9NRffbQA3Kt62UMA21S/2/zBC+epyUmRw9wjRZi73Q+5BKp9c3XTUCgYEA56bt
QLkhBtQmi9uINiGK4yvIGf35EwTDqhfWi9gz0fBx8CNMYYNoD0Y7KUqqPZiDdlY5
cf5g0HMn97uNYcHPzRxDTyrKYij0Z9eIAbHF0A2+RWA4WNwHWy1ijSKW60A0oe7y
BihW2cmFkmM7PXoeG7iW/WwcnTxQ8vslsIKgsS0CgYEAhqVDHwMKjn/tizZ8G/iY
/HawxjiSWayIPtrwU/aFm63T0VYN3w0kDvqePYQorFus5/OfdvzZjNkmsUPhJqHQ
IAmmzp+jq+n5mMEY1OFbkdvJ9U8J1QkQERGu9QQgggQhMj4JReMRdpd6UsMi+Dsl
fvRIrvbWb7AZvnVVIkTn/9ECgYEArN0DBJ5hh9/+V7Vdos9K/hSowMSl/kw62o3p
WvfeS5Z637DniYjuuR82fKyr90T9LOxQ98DUbBjSxWneqglf0PZ7P/N1jOR05ZyP
cHxBWe03hDu2MJEYqDBo71rVdf22iPRvBKtLjD92fnOwPEMJfrQzcC8h9y+QVJGX
YW1JDo0CgYEAurtIfv8Ii9uEGki4jKc7rF5oRcnQY2/nKaP8Ib5xOZGQrfc3vQOG
SHESPe9OcmClgI4vMykpNBfksiqF8RIgqjZqQQLPvgPv6yXvjqGCWYrC2X57C9ZB
vEf4jwsTqxlu3sMu5OyicGSlWrZMVXt1R+fHT+5DKDyNQjmn6N4rGTU=
-----END RSA PRIVATE KEY-----
 
 
)KEY";