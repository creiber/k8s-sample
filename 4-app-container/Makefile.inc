##
##  k8s-sample -- Kubernetes Sample Application
##  Copyright (c) 2019-2020 Dr. Ralf S. Engelschall <rse@engelschall.com>
##  Distributed under MIT license <https://spdx.org/licenses/MIT.html>
##

#   ==== DEFAULT ====
#   default build target
TARGETS ?= build
all: $(TARGETS)

#   ==== COMMON ====
#   configuration of container image
IMAGE_CONFIG = \
	IMAGE_PREFIX=`egrep "ARG.*IMAGE_PREFIX" Dockerfile | sed -e 's;^.*=;;' | head -1` && \
	IMAGE_NAME=`egrep "ARG.*IMAGE_NAME" Dockerfile | sed -e 's;^.*=;;' | head -1` && \
	IMAGE_VERSION=`egrep "ARG.*IMAGE_VERSION" Dockerfile | sed -e 's;^.*=;;' | head -1` && \
	IMAGE_RELEASE=`egrep "ARG.*IMAGE_RELEASE" Dockerfile | sed -e 's;^.*=;;' | head -1`

#   ==== BUILD ====
#   (re)build a container image
DOCKER_BUILD_FLAGS ?=
build:
	@$(IMAGE_CONFIG) && \
	echo "++ building Docker image $${IMAGE_PREFIX}$${IMAGE_NAME}:$${IMAGE_VERSION}-$${IMAGE_RELEASE}" && \
	tar -c -f - -C $(DOCKER_BUILD_SOURCE) . | xz >src.tar.xz; \
	docker build \
	    $(DOCKER_BUILD_FLAGS) \
	    -t $${IMAGE_PREFIX}$${IMAGE_NAME}:latest \
	    -t $${IMAGE_PREFIX}$${IMAGE_NAME}:$${IMAGE_VERSION} \
	    -t $${IMAGE_PREFIX}$${IMAGE_NAME}:$${IMAGE_VERSION}-$${IMAGE_RELEASE} \
	    -f Dockerfile . && \
	rm -f src.tar.xz && \
	docker image ls $${IMAGE_PREFIX}$${IMAGE_NAME}:$${IMAGE_VERSION}-$${IMAGE_RELEASE}

#   ==== RUN ====
#   run a container image
DOCKER_RUN_FLAGS ?= --rm -i -t -e TERM --init
DOCKER_RUN_ARGS  ?=
run:
	@$(IMAGE_CONFIG) && \
	echo "++ running Docker image $${IMAGE_PREFIX}$${IMAGE_NAME}:$${IMAGE_VERSION}-$${IMAGE_RELEASE}" && \
	docker run \
	    --name "$${IMAGE_NAME}-temp" \
		$(DOCKER_RUN_FLAGS) \
	    $${IMAGE_PREFIX}$${IMAGE_NAME}:$${IMAGE_VERSION}-$${IMAGE_RELEASE} \
		$(DOCKER_RUN_ARGS)

#   ==== INSPECT ====
#   inspect a container image
DOCKER_INSPECT_FLAGS ?= --rm -i -t -e TERM -u root --entrypoint "/bin/bash"
DOCKER_INSPECT_ARGS  ?=
inspect:
	@$(IMAGE_CONFIG) && \
	echo "++ inspecting Docker image $${IMAGE_PREFIX}$${IMAGE_NAME}:$${IMAGE_VERSION}-$${IMAGE_RELEASE}" && \
	docker run \
	    --name "$${IMAGE_NAME}-temp" \
		$(DOCKER_INSPECT_FLAGS) \
	    $${IMAGE_PREFIX}$${IMAGE_NAME}:$${IMAGE_VERSION}-$${IMAGE_RELEASE} \
		$(DOCKER_INSPECT_ARGS)

#   ==== EXEC ====
#   enter a running container image
DOCKER_EXEC_FLAGS ?= -i -t -e TERM
DOCKER_EXEC_ARGS  ?= /bin/bash
exec:
	@$(IMAGE_CONFIG) && \
	echo "++ executing command in Docker container of Docker image $${IMAGE_PREFIX}$${IMAGE_NAME}:$${IMAGE_VERSION}-$${IMAGE_RELEASE}" && \
	docker exec \
	    $(DOCKER_EXEC_FLAGS) \
		`docker ps --filter "ancestor=$${IMAGE_PREFIX}$${IMAGE_NAME}:$${IMAGE_VERSION}-$${IMAGE_RELEASE}" --format "{{ .ID }}"` \
		$(DOCKER_EXEC_ARGS)

#   ==== PUSH ====
#   push container image to registry
push:
	@$(IMAGE_CONFIG) && \
	echo "++ pushing Docker image $${IMAGE_PREFIX}$${IMAGE_NAME}:$${IMAGE_VERSION}-$${IMAGE_RELEASE}" && \
	docker push $${IMAGE_PREFIX}$${IMAGE_NAME}:$${IMAGE_VERSION}-$${IMAGE_RELEASE} && \
	docker push $${IMAGE_PREFIX}$${IMAGE_NAME}:$${IMAGE_VERSION} && \
	docker push $${IMAGE_PREFIX}$${IMAGE_NAME}:latest

#   ==== CLEAN ====
#   remove container image
clean:
	@$(IMAGE_CONFIG) && \
	echo "++ removing Docker image $${IMAGE_PREFIX}$${IMAGE_NAME}:$${IMAGE_VERSION}-$${IMAGE_RELEASE}" && \
	docker image rm $${IMAGE_PREFIX}$${IMAGE_NAME}:$${IMAGE_VERSION}-$${IMAGE_RELEASE} >/dev/null 2>&1 || true && \
	docker image rm $${IMAGE_PREFIX}$${IMAGE_NAME}:$${IMAGE_VERSION} >/dev/null 2>&1 || true && \
	docker image rm $${IMAGE_PREFIX}$${IMAGE_NAME}:latest >/dev/null 2>&1 || true && \
	rm -f .built

#   ==== PRUNE ====
#   prune entire Docker environment
prune:
	@echo "++ pruning Docker environment"
	docker container prune -f
	docker network prune -f
	docker volume prune -f
	docker image prune -f

