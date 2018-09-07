#!/bin/bash

set -e
set -x

kvm-ok
service apt-cacher-ng start
ls -l /dev/kvm

cd /gitian-builder
./bin/make-base-vm --suite precise --arch i386
./bin/make-base-vm --suite precise --arch amd64

# trusty for macosx
# https://stackoverflow.com/questions/40032727/ubuntu-vm-builder-fails-on-etc-sudoers-any-workaround/44041908
sed -i 's/dist-upgrade/update/' /usr/lib/python2.7/dist-packages/VMBuilder/plugins/ubuntu/dapper.py
rm /usr/lib/python2.7/dist-packages/VMBuilder/plugins/ubuntu/dapper.pyc
./bin/make-base-vm --suite trusty --arch amd64

# linux
./bin/gbuild ../sparklecoin/contrib/gitian-descriptors/boost-linux.yml </dev/null
cp build/out/boost-linux*-1.55.0-gitian-r1.zip inputs/
./bin/gbuild ../sparklecoin/contrib/gitian-descriptors/deps-linux.yml </dev/null
cp build/out/sparklecoin-deps-linux*-gitian-r1.zip inputs/
# windows
./bin/gbuild ../sparklecoin/contrib/gitian-descriptors/boost-win.yml </dev/null
cp build/out/boost-win*-1.55.0-gitian-r1.zip inputs/
./bin/gbuild ../sparklecoin/contrib/gitian-descriptors/deps-win.yml </dev/null
cp build/out/sparklecoin-deps-win*-gitian-r1.zip inputs/
./bin/gbuild ../sparklecoin/contrib/gitian-descriptors/qt-win.yml </dev/null
cp build/out/qt-win*-4.8.5-gitian-r1.zip inputs/
# osx
./bin/gbuild --commit osxcross=master,libdmg-hfsplus=master ../sparklecoin/contrib/gitian-descriptors/osxcross.yml </dev/null
cp build/out/osxcross.tar.xz inputs/
./bin/gbuild ../sparklecoin/contrib/gitian-descriptors/deps-osx.yml </dev/null
cp build/out/sparklecoin-deps-osx-gitian-r1.tar.xz inputs/
./bin/gbuild ../sparklecoin/contrib/gitian-descriptors/qt-osx.yml </dev/null
cp build/out/qt-osx-5.5.0-gitian.tar.xz inputs/
