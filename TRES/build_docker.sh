#!/bin/bash 

image_environment_name="tres_environment"
image_build_name="tres_build"
container_name="tres_build_container"
image_tag="latest"

# There are two images
#environment: Dependencies, the cross compiler etc..
#build: Copy the source files and in the future the build, now is in the run command

# if no existe imagen: docker image environment -t image_build_name .
docker inspect "${image_environment_name}:${image_tag}" > /dev/null 2>&1 || docker image build -t ${image_environment_name} -f "Dockerfile.environment" .
docker image build -t ${image_build_name} -f "Dockerfile.build" . > /dev/null 2>&1
docker run -ti --name ${container_name} --privileged ${image_build_name}:latest  ./build.sh
docker cp ${container_name}:/tmp/disco.img .
docker rm -f ${container_name}
docker image rm -f ${image_build_name} > /dev/null 2>&1