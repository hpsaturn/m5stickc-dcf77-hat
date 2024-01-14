#!/bin/bash

######################################################
# CanAirIO deploy release utility 
#
# Author: @hpsaturn
# 2021 - 2024
######################################################

NAME=`cat platformio.ini | grep 'name =' | awk '{print $3}'`

if [[ -z "$NAME" ]]; then
  NAME=`basename "$PWD"`
else
  NAME=`echo "$NAME" | dos2unix`
fi

SRC_VER="${2}"
BUILD_DIR=.pio/build
DATE=`date +%Y%m%d`
RELDIR="releases"
RELNAME="${NAME}-v${SRC_VER}.zip"
OUTPUT="${RELDIR}/${RELNAME}"

showHelp () {
  echo ""
  echo "************************************************"
  echo "** Build and deploy tag and release           **"
  echo "************************************************"
  echo ""
  echo "Usage alternatives:"
  echo ""
  echo "./deploy clean"
  echo "./deploy build 0.1.0"
  echo "./deploy github 0.1.0"
  echo ""
}

validate_branch () {
  current_branch=`git rev-parse --abbrev-ref HEAD` 

  if [ ${current_branch} != "master" ]; then
    echo ""
    echo "Error: you are in ${current_branch} branch please change to master branch."
    echo ""
    exit 1
  fi 
}

clean () {
  rm -rf .pio
  rm -f $OUTPUT
}

build () {
  echo ""
  echo "***********************************************"
  echo "** Building v${SRC_VER}"
  echo "***********************************************"
  echo ""
  cd $BUILD_DIR
  array=(*/)
  mkdir "tmp"
  for dir in "${array[@]}"; do 
    flavor=`basename "${dir}"`
    cp $dir/firmware.bin "tmp/${flavor}_firmware.bin"
    cp $dir/partitions.bin "tmp/${flavor}_partitions.bin"
  done
  zip -r ../../${OUTPUT} tmp/*.bin
  rm -r tmp
  cd ../..
  echo ""
  echo "***********************************************"
  echo "************** Build done *********************" 
  echo "***********************************************"
  echo ""
  md5sum $OUTPUT
  du -hs $OUTPUT
  echo ""
}

publish_release () {
  echo ""
  echo "***********************************************"
  echo "********** Publishing release *****************" 
  echo "***********************************************"
  echo ""
  echo "Publishing release: v${SRC_VER}"
  echo "uploading: ${OUTPUT}"
  COMMIT_LOG=`git log -1 --format='%ci %H %s'`
  git tag -a "${SRC_VER}" -m "release v${SRC_VER}"
  git push origin "${SRC_VER}"
  git log -n 10 --pretty=format:"%h %s" | gh release create "${SRC_VER}" -F - -t "v${SRC_VER}" -p ${OUTPUT} 
  echo ""
  echo "***********************************************"
  echo "*************     done    *********************" 
  echo "***********************************************"
  echo ""
}

publish_pio () {
  pio package publish
}

if [ "$1" = "" ]; then
  showHelp
else
  validate_branch
  case "$1" in
    clean)
      clean
      ;;

    help)
      showHelp
      ;;

    --help)
      showHelp
      ;;

    -help)
      showHelp
      ;;

    -h)
      showHelp
      ;;

    print)
      printOutput
      ;;

    github)
      publish_release
      ;;

    *)
      build $1
      ;;
  esac
fi

exit 0

