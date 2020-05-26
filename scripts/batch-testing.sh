#!/bin/sh

val=21

clearImage() {
  # clear generated images
  echo "Clearing config"
  for ((i = 1; i <= val; i++)); do
    rm -r svg/$i/
  done
}

clearConfig() {
  # clear generated config
  echo "Clearing config"
  for ((i = 1; i <= val; i++)); do
    rm -r config/$i/
  done
}

clearMetaData() {
  clearConfig
  clearImage
}

cp -r config/boundary/ config
for ((i = 1; i <= val; i++)); do
  cmake-build-debug/bin/./qtborder $i
done
for ((i = 1; i <= val; i++)); do
  cp -r svg/$i/ svg/boundary/$i
done
clearMetaData

cp -r config/center/ config
for ((i = 1; i <= val; i++)); do
  cmake-build-debug/bin/./qtborder $i
done
for ((i = 1; i <= val; i++)); do
  cp -r svg/$i/ svg/center/$i
done
clearMetaData

cp -r config/component/ config
for ((i = 1; i <= val; i++)); do
  cmake-build-debug/bin/./qtborder $i
done
for ((i = 1; i <= val; i++)); do
  cp -r svg/$i/ svg/component/$i
done
clearMetaData

cp -r config/detail/ config
for ((i = 1; i <= val; i++)); do
  cmake-build-debug/bin/./qtborder $i
done
for ((i = 1; i <= val; i++)); do
  cp -r svg/$i/ svg/detail/$i
done
clearMetaData
