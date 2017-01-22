#-------------------------------------------------
#
# Project created by QtCreator 2015-08-17T21:06:30
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qradiolink
TEMPLATE = app

CONFIG  += thread


SOURCES += main.cpp\
        mainwindow.cpp\
        audio/audioencoder.cpp\
        audio/audiointerface.cpp\
        audioop.cpp\
        controller.cpp\
        dtmfcommand.cpp\
        dtmfdecoder.cpp\
        databaseapi.cpp\
        mumbleclient.cpp\
        serverwrapper.cpp\
        server.cpp\
        settings.cpp\
        speech.cpp\
        sslclient.cpp\
        station.cpp\
        telnetclient.cpp\
        telnetserver.cpp\
        ext/agc.cpp\
        ext/Mumble.pb.cc\
        ext/QRadioLink.pb.cc\
        ext/vox.cpp\
        ext/utils.cpp\
    radioop.cpp \
    audio/alsaaudio.cpp \
    gr/gr_mod_gmsk.cpp \
    gr/gr_modem.cpp \
    gr/gr_vector_source.cpp \
    gr/gr_demod_gmsk.cpp \
    gr/gr_vector_sink.cpp \
    gr/gr_mod_bpsk.cpp \
    gr/gr_demod_bpsk.cpp \
    gr/gr_demod_bpsk_sdr.cpp \
    gr/gr_mod_bpsk_sdr.cpp \
    gr/gr_mod_qpsk_sdr.cpp \
    gr/gr_demod_qpsk_sdr.cpp

HEADERS  += mainwindow.h\
        audio/audioencoder.h\
        audio/audiointerface.h\
        audioop.h\
        controller.h\
        dtmfcommand.h\
        dtmfdecoder.h\
        databaseapi.h\
        mumbleclient.h\
        serverwrapper.h\
        server.h\
        settings.h\
        speech.h\
        sslclient.h\
        station.h\
        telnetclient.h\
        telnetserver.h\
        config_defines.h\
        ext/agc.h\
        ext/dec.h\
        ext/Goertzel.h\
        ext/Mumble.pb.h\
        ext/murmur_pch.h\
        ext/PacketDataStream.h\
        ext/QRadioLink.pb.h\
        ext/vox.h\
        ext/utils.h \
    radioop.h \
    audio/alsaaudio.h \
    gr/gr_mod_gmsk.h \
    gr/gr_modem.h \
    gr/gr_vector_source.h \
    gr/gr_demod_gmsk.h \
    gr/gr_vector_sink.h \
    gr/gr_mod_bpsk.h \
    gr/gr_demod_bpsk.h \
    gr/gr_demod_bpsk_sdr.h \
    gr/gr_mod_bpsk_sdr.h \
    gr/gr_mod_qpsk_sdr.h \
    gr/gr_demod_qpsk_sdr.h


FORMS    += mainwindow.ui


LIBS += -lgnuradio-pmt -lgnuradio-audio -lgnuradio-analog -lgnuradio-blocks \
        -lgnuradio-osmosdr -lgsm \
        -lgnuradio-blocks -lgnuradio-filter -lgnuradio-digital -lgnuradio-runtime \
        -lboost_thread$$BOOST_SUFFIX -lboost_system$$BOOST_SUFFIX -lboost_program_options$$BOOST_SUFFIX
LIBS += -lrt  # need to include on some distros

unix:!symbian: LIBS += -lprotobuf -lopus -lpulse-simple -lpulse -lcodec2 -lasound
                    #-lFestival -lestbase -leststring -lestools -lasound
#INCLUDEPATH += /usr/include/speech_tools

RESOURCES += \
    resources.qrc