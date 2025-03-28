#***************************************************************************
#                                  _   _ ____  _
#  Project                     ___| | | |  _ \| |
#                             / __| | | | |_) | |
#                            | (__| |_| |  _ <| |___
#                             \___|\___/|_| \_\_____|
#
# Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
#
# This software is licensed as described in the file COPYING, which
# you should have received as part of this distribution. The terms
# are also available at https://curl.se/docs/copyright.html.
#
# You may opt to use, copy, modify, merge, publish, distribute and/or sell
# copies of the Software, and permit persons to whom the Software is
# furnished to do so, under the terms of the COPYING file.
#
# This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
# KIND, either express or implied.
#
# SPDX-License-Identifier: curl
#
###########################################################################
set(CERTCONFIGS 
  EdelCurlRoot-ca.prm 
  EdelCurlRoot-ca.cnf 
  Server-localhost-sv.prm 
  Server-localhost.nn-sv.prm 
  Server-localhost0h-sv.prm 
  Server-localhost-firstSAN-sv.prm 
  Server-localhost-lastSAN-sv.prm 
  stunnel-sv.prm)

set(GENERATEDCERTS 
  EdelCurlRoot-ca.cacert 
  EdelCurlRoot-ca.crt 
  EdelCurlRoot-ca.csr 
  EdelCurlRoot-ca.der 
  EdelCurlRoot-ca.key 
  Server-localhost-sv.crl 
  Server-localhost-sv.crt 
  Server-localhost-sv.csr 
  Server-localhost-sv.der 
  Server-localhost-sv.dhp 
  Server-localhost-sv.key 
  Server-localhost-sv.pem 
  Server-localhost-sv.pub.der 
  Server-localhost-sv.pub.pem 
  Server-localhost-sv.pubkey-pinned 
  Server-localhost.nn-sv.crl 
  Server-localhost.nn-sv.crt 
  Server-localhost.nn-sv.csr 
  Server-localhost.nn-sv.der 
  Server-localhost.nn-sv.dhp 
  Server-localhost.nn-sv.key 
  Server-localhost.nn-sv.pem 
  Server-localhost.nn-sv.pub.der 
  Server-localhost.nn-sv.pub.pem 
  Server-localhost.nn-sv.pubkey-pinned 
  Server-localhost0h-sv.crl 
  Server-localhost0h-sv.crt 
  Server-localhost0h-sv.csr 
  Server-localhost0h-sv.der 
  Server-localhost0h-sv.dhp 
  Server-localhost0h-sv.key 
  Server-localhost0h-sv.pem 
  Server-localhost0h-sv.pub.der 
  Server-localhost0h-sv.pub.pem 
  Server-localhost0h-sv.pubkey-pinned 
  Server-localhost-firstSAN-sv.crl 
  Server-localhost-firstSAN-sv.crt 
  Server-localhost-firstSAN-sv.csr 
  Server-localhost-firstSAN-sv.der 
  Server-localhost-firstSAN-sv.dhp 
  Server-localhost-firstSAN-sv.key 
  Server-localhost-firstSAN-sv.pem 
  Server-localhost-firstSAN-sv.pub.der 
  Server-localhost-firstSAN-sv.pub.pem 
  Server-localhost-firstSAN-sv.pubkey-pinned 
  Server-localhost-lastSAN-sv.crl 
  Server-localhost-lastSAN-sv.crt 
  Server-localhost-lastSAN-sv.csr 
  Server-localhost-lastSAN-sv.der 
  Server-localhost-lastSAN-sv.dhp 
  Server-localhost-lastSAN-sv.key 
  Server-localhost-lastSAN-sv.pem 
  Server-localhost-lastSAN-sv.pub.der 
  Server-localhost-lastSAN-sv.pub.pem 
  Server-localhost-lastSAN-sv.pubkey-pinned 
  stunnel-sv.crl 
  stunnel-sv.crt 
  stunnel-sv.csr 
  stunnel-sv.der 
  stunnel-sv.dhp 
  stunnel-sv.key 
  stunnel-sv.pem 
  stunnel-sv.der 
  stunnel-sv.pub.der 
  stunnel-sv.pub.pem 
  stunnel-sv.pubkey-pinned)

set(SRPFILES 
  srp-verifier-conf 
  srp-verifier-db)
