insmod /opt/self_module/audio/soundcore.ko
insmod /opt/self_module/audio/snd.ko
insmod /opt/self_module/audio/snd-seq-device.ko
insmod /opt/self_module/audio/snd-page-alloc.ko
insmod /opt/self_module/audio/snd-rawmidi.ko
insmod /opt/self_module/audio/snd-timer.ko
insmod /opt/self_module/audio/snd-pcm.ko
insmod /opt/self_module/audio/snd-usb-lib.ko
insmod /opt/self_module/audio/snd-hwdep.ko
insmod /opt/self_module/audio/snd-usb-audio.ko
insmod /opt/self_module/audio/snd-mixer-oss.ko
insmod /opt/self_module/audio/snd-pcm-oss.ko 

ln -s /dev/sound/dsp /dev/dsp
ln -s /dev/sound/audio /dev/audio
ln -s /dev/sound/mixer /dev/mixer
