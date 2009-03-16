#! /usr/bin/env python
# encoding: utf-8
# Daniel Moura, 2009 <oxe at oxesoft.com>

# the following two variables are used by the target "waf dist"
VERSION='0.6.1'
APPNAME='wired'

# these variables are mandatory ('/' are converted automatically)
srcdir = '.'
blddir = 'build'

def set_options(opt):
    opt.tool_options('compiler_cxx')

def configure(conf):
    conf.check_tool('compiler_cxx')
    conf.check_cfg(package='', path='wx-config', args='--cflags --libs', version='2.8', uselib_store='WX', mandatory=1) # name is missing in output
    conf.check_cfg(package='libxml-2.0', args='--cflags --libs', atleast_version='2.6.32', uselib_store='XML', mandatory=1)
    conf.check_cfg(package='portaudio-2.0', args='--cflags --libs', atleast_version='19', uselib_store='PORTAUDIO', mandatory=1)
    conf.check_cfg(package='sndfile', args='--cflags --libs', atleast_version='1.0.17', uselib_store='SNDFILE', mandatory=1)
    conf.check_cfg(package='samplerate', args='--cflags --libs', atleast_version='0.1.3', uselib_store='SAMPLERATE', mandatory=1)
    conf.check_cfg(package='soundtouch-1.0', args='--cflags --libs', atleast_version='1.3.1', uselib_store='SOUNDTOUCH', mandatory=1)
    conf.check(header_name='portmidi.h', mandatory=1)
    conf.env['LINKFLAGS_PORTMIDI'] = '-lportmidi'
    conf.check(header_name='porttime.h', mandatory=1)
    conf.env['LINKFLAGS_PORTTIME'] = '-lporttime'
    conf.sub_config('src')
    # add values to the configuration header
    conf.define('PACKAGE_LOCALE_DIR', 'locale')
    conf.define('DATA_DIR', 'data')
    conf.define('SYSCONF_DIR', 'conf')
    conf.define('LIB_DIR', 'plugins')
    conf.write_config_header('config.h')

def build(bld):
    bld.add_subdirs('src')
    bld.new_task_gen(
        features = 'cxx cprogram',
        source = 'src/gui/MainApp.cpp',
        includes = 'build/default',
        uselib_local = '''
            gui
            wireddssi
            save
            sequencer
            audio
            editmidi
            engine
            fileloader
            midi
            mixer
            plugins
            redist
            undo
            wiredxml
            wiredsamplerate
            wiredcodec
            wiredvideo
            WiredAkai
            WiredWidgets
            ''',
        uselib = '''
            WX
            PORTAUDIO
            PORTMIDI
            PORTTIME
            SNDFILE
            SAMPLERATE
            SOUNDTOUCH
            ''',
        target = 'wired')
