# To build, copy this file to gcconfig.pri and then fill in the paths to your
# local installs of Boost and srmio in the copy.  If you don't want
# support for SRM downloads, just comment out the SRMIO_INSTALL line.

BOOST_INSTALL = /usr/include/boost
#SRMIO_INSTALL = /usr/local/srmio
#D2XX_INCLUDE = /usr/local/include/D2XX

# If you want Twitter support you must install liboauth
# http://liboauth.sourceforge.net/
# Set the path to where liboauth.a was installed
# By default it will have been installed like this:
#
#LIBOAUTH_INSTALL = /usr/local
#LIBCRYPTO_INSTALL = -lcrypto
#LIBCURL_INSTALL=-lcurl
#LIBZ_INSTALL=-lz

# If you want 3D plotting, you need to install qwtplot3d
#
#  http://qwtplot3d.sourceforge.net/
#
#  If you are running Linux and have font problems, download
#  the Tar ball from http://qwtplot3d.svn.sourceforge.net/viewvc/qwtplot3d/
#  Follow Branches -> Maintain_0_2_x -> qwtplot3d
#
## then set the following variable appropriately:

#QWT3D_INSTALL = /usr/local/qwtplot3d

# If you want support for Google Earth .kml files then you need
# to install the Google libkml library
#
# http://code.google.com/p/libkml/
# or on Linux sudo apt-get install libkml-dev
#
# then set the following variable appropriately
# to the root of the libs/includ path

#KML_INSTALL = /usr/local


# We recommend a debug build for development, and a static build for releases.
CONFIG += debug
#CONFIG += static

# Edit these paths only if you have a Boost/srmio install that uses
# a non-standard directory layout.

BOOST_INCLUDE = $${BOOST_INSTALL}/include

!isEmpty( SRMIO_INSTALL ) {
    SRMIO_INCLUDE = $${SRMIO_INSTALL}/include
    SRMIO_LIB = $${SRMIO_INSTALL}/lib/libsrmio.a
}

macx {
    # Uncomment this line to build with OS X Tiger support on a Leopard system:
    #QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk

    # Uncomment this line to build a OS X universal binary:
    #CONFIG+=x86 ppc
}

