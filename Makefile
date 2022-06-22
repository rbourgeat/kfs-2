# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/04/24 16:00:27 by rbourgea          #+#    #+#              #
#    Updated: 2022/06/22 17:06:39 by rbourgea         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# **************************************************************************** #
# 🎨 COLORS
# **************************************************************************** #

GREY    	=	\033[030m
RED     	=	\033[031m
GREEN   	=	\033[032m
YELLOW  	=	\033[033m
BLUE    	=	\033[034m
MAGENTA 	=	\033[035m
CYAN		=	\033[036m
BOLD		=	\033[1m
RESET   	=	\033[0m

# **************************************************************************** #
# 💾 VARIABLES
# **************************************************************************** #

KERNEL_OUT	=	build/rbourgea_kfs.bin
ISO_OUT		=	build/rbourgea_kfs.iso

BOOT		=	src/boot.s
SRC		=	src/kernel.c src/libk.c src/keyboard.c src/terminal.c
OBJ		=	boot.o kernel.o libk.o keyboard.o terminal.o
LINKER		=	src/linker.ld

FLAGS		=	-fno-builtin -fno-builtin -fno-builtin -nostdlib -nodefaultlibs

# **************************************************************************** #
# 📖 RULES
# **************************************************************************** #

all: build

build: fclean
	@mkdir -p build
	@nasm -f elf32 ${BOOT} -o boot.o
	@gcc -m32 -ffreestanding ${FLAGS} -c ${SRC}
	@echo "$(BOLD)$(GREEN)[✓] KERNEL BUILD DONE$(RESET)"
	@ld -m elf_i386 -T ${LINKER} -o ${KERNEL_OUT} ${OBJ}
	@echo "$(BOLD)$(GREEN)[✓] KERNEL LINK DONE$(RESET)"

build_debug: fclean
	@echo "$(BOLD)$(YELLOW)[✓] KERNEL DEBUG MODE ON$(RESET)"
	@mkdir -p build
	@nasm -f elf32 ${BOOT} -o boot.o
	@gcc -m32 -ffreestanding ${FLAGS} ${SRC} -ggdb
	@echo "$(BOLD)$(GREEN)[✓] KERNEL BUILD DONE$(RESET)"
	@ld -m elf_i386 -T ${LINKER} -o ${KERNEL_OUT} ${OBJ}
	@echo "$(BOLD)$(GREEN)[✓] KERNEL LINK DONE$(RESET)"

run: build
	@qemu-system-i386 -kernel ${KERNEL_OUT} -monitor stdio
	@echo "\n$(BOLD)$(CYAN)[✓] KERNEL EXIT DONE$(RESET)"

debug: build_debug
	@qemu-system-i386 -kernel ${KERNEL_OUT} -s -S &
	@gdb -x .gdbinit
	@echo "\n$(BOLD)$(CYAN)[✓] KERNEL DEBUG EXIT DONE$(RESET)"

iso: build
	@mkdir -p build/iso/boot/grub
	@cp grub.cfg build/iso/boot/grub
	@cp ${KERNEL_OUT} build/iso/boot
	@grub-mkrescue -o ${ISO_OUT} build/iso
	@echo "$(BOLD)$(GREEN)[✓] KERNEL ISO BUILD$(RESET)"

run-iso: iso
	@qemu-system-i386 -cdrom ${ISO_OUT}
	@echo "\n$(BOLD)$(CYAN)[✓] KERNEL EXIT DONE$(RESET)"

docker-run:
	clear
	@echo "\n$(BOLD)$(CYAN)[✓] STARTING DOCKER COMPILATION MODE$(RESET)"
	@-docker stop rbourgea-kfs
	@-docker rm rbourgea-kfs
	@docker build --platform linux/amd64 -t rbourgea-kfs .
	@docker run -d --name rbourgea-kfs --rm -i -t rbourgea-kfs
	@docker cp src/. rbourgea-kfs:/kfs
	@docker exec -t rbourgea-kfs nasm -f elf32 boot.s -o boot.o
	@docker exec -t rbourgea-kfs gcc -m32 -ffreestanding ${FLAGS} -c kernel.c libk.c keyboard.c terminal.c
	@docker exec -t rbourgea-kfs ld -m elf_i386 -T linker.ld -o rbourgea_kfs.bin ${OBJ}
	@-rm boot/rbourgea_kfs.bin
	@docker cp rbourgea-kfs:/kfs/rbourgea_kfs.bin boot/rbourgea_kfs.bin
	@qemu-system-i386 -kernel boot/rbourgea_kfs.bin -monitor stdio
	@echo "\n$(BOLD)$(CYAN)[✓] KERNEL EXIT DONE$(RESET)"

clean:
	@rm -rf $(KERNEL_OUT) $(ISO_OUT) *.o
	@echo "$(BOLD)$(RED)[♻︎] DELETE KERNEL DONE$(RESET)"

fclean: clean
	clear
	@rm -rf build/
	@echo "$(BOLD)$(RED)[♻︎] DELETE BUILD/ DONE$(RESET)"

re: fclean all

.PHONY: all clean fclean re