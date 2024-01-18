OBJS=main.o readfile.o map.o gfx.o gfx_decoder.o
CFLAGS += -Wall -Wextra -Werror -g

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

biomenace: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

format:
	find src -name "*.[h|c]" -exec sh -c 'clang-format --style=LLVM $$0 > $$0.frmt; mv $$0.frmt $$0' {} \;

clean:
	rm -f $(OBJS) biomenace
