OBJS=main.o readfile.o maps.o maptemp.o maphead.o gfx.o gfx_decoder.o gfx_pictures.o gfx_sprites.o gfx_fonts.o
CFLAGS += -Wall -Wextra -Werror -g

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

openmenace: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

format:
	find src -name "*.[h|c]" -exec sh -c 'clang-format --style=LLVM $$0 > $$0.frmt; mv $$0.frmt $$0' {} \;

clean:
	rm -f $(OBJS) biomenace
