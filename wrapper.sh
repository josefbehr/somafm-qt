#!/bin/sh

# fonts
# export XDG_DATA_HOME=$SNAP/usr/share
export FONTCONFIG_PATH=$SNAP/etc/fonts/config.d
export FONTCONFIG_FILE=$SNAP/etc/fonts/fonts.conf

# cd $SNAP_USER_DATA

exec $SNAP/usr/bin/somafm-qt
